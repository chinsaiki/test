
#ifndef _CL_VTY_H_
#define _CL_VTY_H_


#include "cli.h"

#define CONTROL(X)  ((X) - '@')

/* Vty read buffer size. */
#define VTY_READ_BUFSIZ 512


/* Below is vty's connection type ,there're three types total */

#define _CL_VTY_TELNET_ 0
#define _CL_VTY_CONSOLE_ 1
#define _CL_VTY_SSH_ 2
#define _CL_VTY_TELNET_V6_ 3

/* define custom sa_family for Console */
#define AF_TTYS 9

#define	IAC	255		/* interpret as command: */
#define	DONT	254		/* you are not to use option */
#define	DO	253		/* please, you use option */
#define	WONT	252		/* I won't use option */
#define	WILL	251		/* I will use option */
#define	SB	250		/* interpret as subnegotiation */
#define	GA	249		/* you may reverse the line */
#define	EL	248		/* erase the current line */
#define	EC	247		/* erase the current character */
#define	AYT	246		/* are you there */
#define	AO	245		/* abort output--but let prog finish */
#define	IP	244		/* interrupt process--permanently */
#define	BREAK	243		/* break */
#define	DM	242		/* data mark--for connect. cleaning */
#define	NOP	241		/* nop */
#define	SE	240		/* end sub negotiation */
#define EOR     239             /* end of record (transparent mode) */

#define SYNCH	242		/* for telfunc calls */

/* Vty events */
enum event
{ VTY_READ, VTY_WRITE, VTY_READ_ALIVE, VTY_TIMEOUT_RESET ,VTY_EXIT,VTY_TELNET_RELAY_EXIT};


typedef char *cl_lib_caddr_t;

struct cl_lib_iovec {
	cl_lib_caddr_t	iov_base;
	int	iov_len;
};


/* Prototypes. */
void vty_reset (void);
struct vty *vty_new (void);
int vty_big_out (struct vty *vty, int maxlen,const char *format, ...);
int vty_direct_out (struct vty *vty , const char *format, ...);
void cl_vty_color_out(struct vty *vty,char *str);

void vty_flush_all(struct vty *vty);
void vty_buffer_reset(struct vty *vty);

void vty_free(struct vty *vty);
int cl_read( int fd, char *buf, int len );
int cl_write( int fd, char *buf, int len );
void vty_task_exit(struct vty *vty);
void vty_close (struct vty *vty);
void vty_get(struct vty *vty);
void vty_put(struct vty * vty );
int cl_close( int fd );
int vty_config_lock (struct vty *);
int vty_config_unlock (struct vty *);
void vty_hello (struct vty *);
void vty_event (enum event event, int sock, struct vty *vty);
void vty_buffer_reset_out (struct vty *vty);

int vty_command (struct vty *vty, char *buf);
int cl_write( int fd, char *buf, int len );
int cl_vty_monitor_syslog_out(char *timestamp,int log_type,int log_level,char *logmsg,char *levelstr,char *typestr);
//int cl_vty_assert_out(const char *format, ...);
//int cl_vty_console_out(const char *format, ...);

/* Set time out value. */
int cl_exec_timeout (struct vty *vty, char *min_str, char *sec_str);

/* Put out prompt and wait input from user. */
void vty_prompt (struct vty *vty);
void vty_direct_prompt(struct vty *vty);
void cl_vty_up_line(int fd);
void cl_vty_clear_screen(struct vty *vty);

int cl_vty_serv_console (int console_fd);

void cl_vty_serv_telnet_enable(unsigned short port);

void vos_telnet_init();


void cl_vty_serv_telnet_disable(void);

int cl_vty_close_all_vty(void);


int cl_vty_close_all_telnet_vty(struct vty *v);


int cl_vty_close_by_username(char *del_username,struct vty *v);


int cl_vty_close_by_sessionid(struct vty *v, int id);

int cl_exec_timeout_show(struct vty *vty);

/**************************************************

 init vty struct  and cl_vty_serv_thread

 and init some global val

*******************************************************/

char *vty_get_user_name(struct vty *vty);
char *vty_get_user_ip(struct vty *vty);


void cl_vty_init ();

void cl_vty_monitor_count_inc(void);

void cl_vty_monitor_count_dec(void);

//int cl_vty_monitor_out(const char *format, ...);

VOID cl_vty_syn_index(struct vty * v, void * index, 
                    enum node_type cur_node, enum node_type pre_node, char *prompt);

VOID cl_vty_syn_index_later( struct vty * v, void * index,
                             enum node_type cur_node, enum node_type pre_node, char * prompt );
int cl_vty_exit_all_vty(void);

int cl_vty_console_out( const char * format, ... );
int cl_vty_close_telnet_admin_by_console(struct vty *v, int id);



#endif /* _CL_VTY_H_ */

