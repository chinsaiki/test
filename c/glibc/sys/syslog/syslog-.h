#include <syslog.h>

//closelog, openlog, syslog, vsyslog - send messages to the system logger
void openlog(const char *ident, int option, int facility);
void syslog(int priority, const char *format, ...);
void closelog(void);

