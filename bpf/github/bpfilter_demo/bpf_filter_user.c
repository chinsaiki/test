// bpf_filter_user.c
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bpf/bpf.h>
#include <linux/bpf.h>
#include "bpf_util.h"

#define SIZE	32

static int action_map_fd;

int main(int argc, char **argv)
{
	int o, i = 0, opt = 0, action;
	in_addr_t addr, *paddr;
	char buff[64], *token;
	char *optstring[32];
   	const char s[2] = " ";
	char *mapfile;

	// 传入的正是xtables-monitor捕获的规则集的变化
	optstring[i++] = calloc(1, SIZE);
	strcpy(optstring[0], "EVENT:");
	sprintf(buff, "%s\n", strstr(argv[1], optstring[0]));

	mapfile = argv[2];

	token = strtok(buff, s);
	token = strtok(NULL, s);

	while (token != NULL) {
		optstring[i] = malloc(SIZE);
		strcpy(optstring[i++], token);
		token = strtok(NULL, s);
	}
	optstring[i] = NULL;

	while((o = getopt(i, optstring, "4t:A:D:s:j:")) != EOF) {
		switch (o) {
		case 'A':
			 opt = 1; break;
		case 'D':
			 opt = 0; break;
		case 's': {
			 char *raw_addr = strtok(optarg, "/");
			 addr = inet_addr(raw_addr);
			 paddr = &addr;
			 break;
			  }
		case 'j':
			 if (!strncmp(optarg, "DROP", 4)) {
			 	action = 1;
			 } else if (!strncmp(optarg, "ACCEPT", 6)) {
			 	action = 0;
			 }
			 break;
		default: break;
		}
	}

	// 这是一个PIN住的全局map，我们打开它。
	action_map_fd = bpf_obj_get(mapfile);
	// 增加或者删除规则，只要体现在XDP的map上
	if (opt == 0) {
		bpf_map_delete_elem(action_map_fd, (const void *)paddr);
		printf("delete source IP:%x\n", addr);
	} else if (opt == 1) {
		bpf_map_update_elem(action_map_fd, (const void *)&addr, (const unsigned int *)&action, 0);
		printf("add/update source IP:%x\n", addr);
	}
	return 0;
}

