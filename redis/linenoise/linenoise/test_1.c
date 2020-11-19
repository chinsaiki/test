#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "test_help.h"
#include "test_zmalloc.h"

#include "linenoise.h"

#include "sds.h"


#define CLI_HELP_COMMAND 1
#define CLI_HELP_GROUP 2


typedef struct {
    int type;
    int argc;
    sds *argv;
    sds full;

    /* Only used for help on commands */
    struct commandHelp *org;
} helpEntry;


static helpEntry *helpEntries;
static int helpEntriesLen;
char cli_prompt[64]  ={"DBG> "};

static void cliInitHelp(void) {
    int commandslen = sizeof(commandHelp)/sizeof(struct commandHelp);
    int groupslen = sizeof(commandGroups)/sizeof(char*);
    int i, len, pos = 0;
    helpEntry tmp;

    helpEntriesLen = len = commandslen+groupslen;
    helpEntries = zmalloc(sizeof(helpEntry)*len);

    for (i = 0; i < groupslen; i++) {
        tmp.argc = 1;
        tmp.argv = zmalloc(sizeof(sds));
        tmp.argv[0] = sdscatprintf(sdsempty(),"@%s",commandGroups[i]);
        tmp.full = tmp.argv[0];
        tmp.type = CLI_HELP_GROUP;
        tmp.org = NULL;
        helpEntries[pos++] = tmp;
    }

    for (i = 0; i < commandslen; i++) {
        tmp.argv = sdssplitargs(commandHelp[i].name,&tmp.argc);
        tmp.full = sdsnew(commandHelp[i].name);
        tmp.type = CLI_HELP_COMMAND;
        tmp.org = &commandHelp[i];
        helpEntries[pos++] = tmp;
    }
}

/* Linenoise completion callback. */
static void rtoax_completionCallback(const char *buf, linenoiseCompletions *lc) {
    size_t startpos = 0;
    int mask;
    int i;
    size_t matchlen;
    sds tmp;

    if (strncasecmp(buf,"help ",5) == 0) {
        startpos = 5;
        while (isspace(buf[startpos])) startpos++;
        mask = CLI_HELP_COMMAND | CLI_HELP_GROUP;
    } else {
        mask = CLI_HELP_COMMAND;
    }

    for (i = 0; i < helpEntriesLen; i++) {
        if (!(helpEntries[i].type & mask)) continue;

        matchlen = strlen(buf+startpos);
        if (strncasecmp(buf+startpos,helpEntries[i].full,matchlen) == 0) {
            tmp = sdsnewlen(buf,startpos);
            tmp = sdscat(tmp,helpEntries[i].full);
            linenoiseAddCompletion(lc,tmp);
            sdsfree(tmp);
        }
    }
}


/* Linenoise hints callback. */
static char *rtoax_hintsCallback(const char *buf, int *color, int *bold) {
//    if (!pref.hints) return NULL;

    int i, argc, buflen = strlen(buf);
    sds *argv = sdssplitargs(buf,&argc);
    int endspace = buflen && isspace(buf[buflen-1]);

    /* Check if the argument list is empty and return ASAP. */
    if (argc == 0) {
        sdsfreesplitres(argv,argc);
        return NULL;
    }

    for (i = 0; i < helpEntriesLen; i++) {
        if (!(helpEntries[i].type & CLI_HELP_COMMAND)) continue;

        if (strcasecmp(argv[0],helpEntries[i].full) == 0 ||
            strcasecmp(buf,helpEntries[i].full) == 0)
        {
            *color = 90;
            *bold = 0;
            sds hint = sdsnew(helpEntries[i].org->params);

            /* Remove arguments from the returned hint to show only the
             * ones the user did not yet typed. */
            int toremove = argc-1;
            while(toremove > 0 && sdslen(hint)) {
                if (hint[0] == '[') break;
                if (hint[0] == ' ') toremove--;
                sdsrange(hint,1,-1);
            }

            /* Add an initial space if needed. */
            if (!endspace) {
                sds newhint = sdsnewlen(" ",1);
                newhint = sdscatsds(newhint,hint);
                sdsfree(hint);
                hint = newhint;
            }

            sdsfreesplitres(argv,argc);
            return hint;
        }
    }
    sdsfreesplitres(argv,argc);
    return NULL;
}

static void rtoax_freeHintsCallback(void *ptr) {
    sdsfree(ptr);
}

/* Split the user provided command into multiple SDS arguments.
 * This function normally uses sdssplitargs() from sds.c which is able
 * to understand "quoted strings", escapes and so forth. However when
 * we are in Lua debugging mode and the "eval" command is used, we want
 * the remaining Lua script (after "e " or "eval ") to be passed verbatim
 * as a single big argument. */
static sds *cliSplitArgs(char *line, int *argc) {
//    if (config.eval_ldb && (strstr(line,"eval ") == line ||
//                            strstr(line,"e ") == line))
//    {
//        sds *argv = sds_malloc(sizeof(sds)*2);
//        *argc = 2;
//        int len = strlen(line);
//        int elen = line[1] == ' ' ? 2 : 5; /* "e " or "eval "? */
//        argv[0] = sdsnewlen(line,elen-1);
//        argv[1] = sdsnewlen(line+elen,len-elen);
//        return argv;
//    } else {
        return sdssplitargs(line,argc);
//    }
}

/* Set the CLI preferences. This function is invoked when an interactive
 * ":command" is called, or when reading ~/.redisclirc file, in order to
 * set user preferences. */
void cliSetPreferences(char **argv, int argc, int interactive) {
    if (!strcasecmp(argv[0],":set") && argc >= 2) {
        if (!strcasecmp(argv[1],"hints")) printf("pref.hints = 1");
        else if (!strcasecmp(argv[1],"nohints")) printf("pref.hints = 0");
        else {
            printf("%sunknown redis-cli preference '%s'\n",
                interactive ? "" : ".redisclirc: ",
                argv[1]);
        }
    } else {
        printf("%sunknown redis-cli internal command '%s'\n",
            interactive ? "" : ".redisclirc: ",
            argv[0]);
    }
}


static void cliRefreshPrompt(const char *hostip, const int hostport) {

    sds prompt = sdsempty();
    char addr[256];
    snprintf(addr, sizeof(addr), "%s:%d", hostip, hostport);
    prompt = sdscatlen(prompt,addr,strlen(addr));

    /* Copy the prompt in the static buffer. */
    prompt = sdscatlen(prompt,"> ",2);
    printf("\nConnect to %s\n",prompt);
    snprintf(cli_prompt, 64, "%s",prompt);
    sdsfree(prompt);
}


static int issueCommandRepeat(int argc, char **argv, long repeat) {
    int i;
    for(i=0;i<argc; i++)
        printf("%s ", argv[i]);
    printf("\n");
    return 0;
}

static int issueCommand(int argc, char **argv) {
    return issueCommandRepeat(argc, argv, 1);
}


int main(int argc, char **argv) 
{
    sds historyfile = NULL;
    int history = 0;
    char *line;
    char *prgname = argv[0];

    int cli_argc;
    sds *cli_argv;

    /* Parse options, with --multiline we enable multi line editing. */
    while(argc > 1) {
        argc--;
        argv++;
        if (!strcmp(*argv,"--multiline")) {
            linenoiseSetMultiLine(1);
            printf("Multi-line mode enabled.\n");
        } else if (!strcmp(*argv,"--keycodes")) {
            linenoisePrintKeyCodes();
            exit(0);
        } else {
            fprintf(stderr, "Usage: %s [--multiline] [--keycodes]\n", prgname);
            exit(1);
        }
    }

    cliInitHelp();

    linenoiseSetCompletionCallback(rtoax_completionCallback);
    linenoiseSetHintsCallback(rtoax_hintsCallback);

    linenoiseSetFreeHintsCallback(rtoax_freeHintsCallback);
    
    /* Load history from file. The history file is just a plain text file
     * where entries are separated by newlines. */
    linenoiseHistoryLoad("history.txt"); /* Load the history at startup */

    /* Now this is the main loop of the typical linenoise-based application.
     * The call to linenoise() will block as long as the user types something
     * and presses enter.
     *
     * The typed string is returned as a malloc() allocated string by
     * linenoise, so the user needs to free() it. */
    
    while((line = linenoise(cli_prompt)) != NULL) {
        if (line[0] != '\0') {
            long repeat = 1;
            int skipargs = 0;
            char *endptr = NULL;
            
            argv = cliSplitArgs(line,&argc);

            /* check if we have a repeat command option and
             * need to skip the first arg */
            if (argv && argc > 0) {
                errno = 0;
                repeat = strtol(argv[0], &endptr, 10);
                if (argc > 1 && *endptr == '\0') {
                    if (errno == ERANGE || errno == EINVAL || repeat <= 0) {
                        fputs("Invalid redis-cli repeat command option value.\n", stdout);
                        sdsfreesplitres(argv, argc);
                        linenoiseFree(line);
                        continue;
                    }
                    skipargs = 1;
                } else {
                    repeat = 1;
                }
            }

            /* Won't save auth or acl setuser commands in history file */
            int dangerous = 0;
            if (argv && argc > 0) {
                if (!strcasecmp(argv[skipargs], "auth")) {
                    dangerous = 1;
                } else if (skipargs+1 < argc &&
                           !strcasecmp(argv[skipargs], "acl") &&
                           !strcasecmp(argv[skipargs+1], "setuser"))
                {
                    dangerous = 1;
                }
            }

            if (!dangerous) {
                if (history) linenoiseHistoryAdd(line);
                if (historyfile) linenoiseHistorySave(historyfile);
            }

            if (argv == NULL) {
                printf("Invalid argument(s)\n");
                linenoiseFree(line);
                continue;
            } else if (argc > 0) {
                if (strcasecmp(argv[0],"quit") == 0 ||
                    strcasecmp(argv[0],"exit") == 0)
                {
                    exit(0);
                } else if (argv[0][0] == ':') {
                    cliSetPreferences(argv,argc,1);
                    sdsfreesplitres(argv,argc);
                    linenoiseFree(line);
                    continue;
                } else if (strcasecmp(argv[0],"restart") == 0) {
                    printf("<%s>\n", argv[0]);
                    int eval = 0;
                    if (eval) {
                        sdsfreesplitres(argv,argc);
                        linenoiseFree(line);
                        return; /* Return to evalMode to restart the session. */
                    } else {
                        printf("Use 'restart' only in Lua debugging mode.");
                    }
                } else if (argc == 3 && !strcasecmp(argv[0],"connect")) {
                    cliRefreshPrompt(argv[1], atoi(argv[2]));
                } else if (argc == 1 && !strcasecmp(argv[0],"clear")) {
                    linenoiseClearScreen();
                } else {
                    long long start_time = time(NULL), elapsed;

                    issueCommandRepeat(argc-skipargs, argv+skipargs, repeat);

                    elapsed = time(NULL)-start_time;
                    if (elapsed >= 500 ) {
                        printf("elapsed >= 500 (%.2fs)\n",(double)elapsed/1000);
                    } else {
                        printf("elapsed <  500 (%.2fs)\n",(double)elapsed/1000);
                    }
                }
            }
            /* Free the argument vector */
            sdsfreesplitres(argv,argc);
        }
        /* linenoise() returns malloc-ed lines like readline() */
        linenoiseFree(line);
    }
    return 0;
}

