//getservent, getservbyname, getservbyport, setservent, endservent - get service entry

#include <netdb.h>

/*网络服务servent结构体*/
struct servent
{
	char *s_name;		//服务的名称
	char **s_aliases;	//服务可能的别名
	int s_port;			//服务可能的端口
	char *s_proto;		//服务使用的协议
};


//struct servent *getservent(void);
//
//struct servent *getservbyname(const char *name, const char *proto);
//
//struct servent *getservbyport(int port, const char *proto);
//
void setservent(int stayopen);

//void endservent(void);




