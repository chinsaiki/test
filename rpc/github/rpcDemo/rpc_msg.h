#define SERVER_IP "192.168.24.130"

enum RPC_REQ_TYPE_1
{
    RPC_enable,
    RPC_disable,
};

enum RPC_REQ_TYPE_2
{
    RPC_ADD,
    RPC_SUB,
    RPC_MUL,
    RPC_DIV,
};

typedef struct my_msg_hdr_s
{
    int mtype;
    int len;
}my_msg_hdr_t;

typedef struct my_msg_s
{
    my_msg_hdr_t msg_hdr;
    int para1;
    int para2;
    int result;
}my_msg_t;

