#ifndef __BITS_CLI_H
#define __BITS_CLI_H 1


#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>


/**
 *  cli errno 
 */
#define CLI_OK                   0
#define CLI_ERROR               -1
#define CLI_QUIT                -2
#define CLI_ERROR_ARG           -3
#define CLI_AMBIGUOUS           -4
#define CLI_UNRECOGNIZED        -5
#define CLI_MISSING_ARGUMENT    -6
#define CLI_MISSING_VALUE       -7
#define CLI_BUILDMODE_START     -8
#define CLI_BUILDMODE_ERROR     -9
#define CLI_BUILDMODE_EXTEND    -10
#define CLI_BUILDMODE_CANCEL    -11
#define CLI_BUILDMODE_EXIT      -12


/* privilege */
#define CLI_PRIVILEGE_UNPRIVILEGED   0
#define CLI_PRIVILEGE_PRIVILEGED     15

/* mode */
#define CLI_MODE_ANY         -1
#define CLI_MODE_EXEC        0
#define CLI_MODE_CONFIG      1

enum cli_optarg_flags {
    CLI_CMD_OPTIONAL_FLAG       = 1 << 0,
    CLI_CMD_OPTIONAL_ARGUMENT   = 1 << 1,
    CLI_CMD_ARGUMENT            = 1 << 2,
    CLI_CMD_ALLOW_BUILDMODE     = 1 << 3,
    CLI_CMD_OPTION_MULTIPLE     = 1 << 4,
    CLI_CMD_OPTION_SEEN         = 1 << 5,
    CLI_CMD_TRANSIENT_MODE      = 1 << 6,
    CLI_CMD_DO_NOT_RECORD       = 1 << 7,
    CLI_CMD_REMAINDER_OF_LINE   = 1 << 8,
    CLI_CMD_HYPHENATED_OPTION   = 1 << 9,
    CLI_CMD_SPOT_CHECK          = 1 << 10,
};



/* cli struct */
struct cli_struct;

/* filter */
struct cli_filter;

struct cli_command;

struct cli_comphelp;

/* cmd optarg */
struct cli_optarg;

/* optarg pair */
struct cli_optarg_pair;





/* APIs */
struct cli_struct *cli_init(const char *banner, const char *hostname);
int cli_done(struct cli_struct *cli);
struct cli_command *cli_register_command(struct cli_struct *cli, struct cli_command *parent, const char *command,
                                         int (*callback)(struct cli_struct *, const char *, char **, int), int privilege,
                                         int mode, const char *help);
int cli_unregister_command(struct cli_struct *cli, const char *command);
int cli_run_command(struct cli_struct *cli, const char *command);
int cli_loop(struct cli_struct *cli, int sockfd);
int cli_file(struct cli_struct *cli, FILE *fh, int privilege, int mode);
void cli_set_auth_callback(struct cli_struct *cli, int (*auth_callback)(const char *, const char *));
void cli_set_enable_callback(struct cli_struct *cli, int (*enable_callback)(const char *));
void cli_allow_user(struct cli_struct *cli, const char *username, const char *password);
void cli_allow_enable(struct cli_struct *cli, const char *password);
void cli_deny_user(struct cli_struct *cli, const char *username);
void cli_set_banner(struct cli_struct *cli, const char *banner);
void cli_set_hostname(struct cli_struct *cli, const char *hostname);
void cli_set_client_timeout(struct cli_struct *cli, struct timeval *tv);
void cli_set_promptchar(struct cli_struct *cli, const char *promptchar);
void cli_set_modestring(struct cli_struct *cli, const char *modestring);
int cli_set_privilege(struct cli_struct *cli, int privilege);
int cli_set_configmode(struct cli_struct *cli, int mode, const char *config_desc);
void cli_reprompt(struct cli_struct *cli);
void cli_regular(struct cli_struct *cli, int (*callback)(struct cli_struct *cli));
void cli_regular_interval(struct cli_struct *cli, int seconds);
void cli_print(struct cli_struct *cli, const char *format, ...) __attribute__((format(printf, 2, 3)));
void cli_bufprint(struct cli_struct *cli, const char *format, ...) __attribute__((format(printf, 2, 3)));
void cli_vabufprint(struct cli_struct *cli, const char *format, va_list ap);
void cli_error(struct cli_struct *cli, const char *format, ...) __attribute__((format(printf, 2, 3)));
void cli_print_callback(struct cli_struct *cli, void (*callback)(struct cli_struct *, const char *));
void cli_free_history(struct cli_struct *cli);
void cli_set_idle_timeout(struct cli_struct *cli, unsigned int seconds);
void cli_set_idle_timeout_callback(struct cli_struct *cli, unsigned int seconds, int (*callback)(struct cli_struct *));

// Enable or disable telnet protocol negotiation.
// Note that this is enabled by default and must be changed before cli_loop() is run.
void cli_telnet_protocol(struct cli_struct *cli, int telnet_protocol);

// Set/get user context
void cli_set_context(struct cli_struct *cli, void *context);
void *cli_get_context(struct cli_struct *cli);

void cli_free_comphelp(struct cli_comphelp *comphelp);
int cli_add_comphelp_entry(struct cli_comphelp *comphelp, const char *entry);
void cli_set_transient_mode(struct cli_struct *cli, int transient_mode);
struct cli_command *cli_register_filter(struct cli_struct *cli, const char *command,
                                        int (*init)(struct cli_struct *cli, int, char **, struct cli_filter *filt),
                                        int (*filter)(struct cli_struct *, const char *, void *), int privilege, int mode,
                                        const char *help);

int cli_unregister_filter(struct cli_struct *cli, const char *command);
int cli_register_optarg(struct cli_command *cmd, const char *name, int flags, int priviledge, int mode,
                        const char *help,
                        int (*get_completions)(struct cli_struct *cli, const char *, const char *, struct cli_comphelp *),
                        int (*validator)(struct cli_struct *cli, const char *, const char *),
                        int (*transient_mode)(struct cli_struct *, const char *, const char *));
char *cli_find_optarg_value(struct cli_struct *cli, char *name, char *find_after);
struct cli_optarg_pair *cli_get_all_found_optargs(struct cli_struct *cli);
struct cli_optarg_pair *cli_get_next_optargs(struct cli_optarg_pair *optarg);
const char *cli_get_name_optargs(struct cli_optarg_pair *optarg);
const char *cli_get_value_optargs(struct cli_optarg_pair *optarg);


int cli_unregister_optarg(struct cli_command *cmd, const char *name);
char *cli_get_optarg_value(struct cli_struct *cli, const char *name, char *find_after);
int cli_set_optarg_value(struct cli_struct *cli, const char *name, const char *value, int allow_multiple);
void cli_unregister_all_optarg(struct cli_command *c);
void cli_unregister_all_filters(struct cli_struct *cli);
void cli_unregister_all_commands(struct cli_struct *cli);
void cli_unregister_all(struct cli_struct *cli, struct cli_command *command);


#endif /*<__BITS_CLI_H>*/

