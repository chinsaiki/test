#define MY_RPC_PROG_NUM       0x38000010

struct my_io_data_s
{
    int mtype;
    int len;
    char data[1024];
};

typedef struct my_io_data_s my_io_data_t;

program MY_RPC_PROG {
    version MY_RPC_VERS1 {
        int MY_RPCC ( my_io_data_t ) = 1;
    } = 1;

    version MY_RPC_VERS2 {
        my_io_data_t MY_RPCC ( my_io_data_t ) = 1;
    } = 2;
} = MY_RPC_PROG_NUM;
