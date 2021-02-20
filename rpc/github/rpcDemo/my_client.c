/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "my.h"
#include "rpc_msg.h"

int
my_rpc_prog_1(char *host, my_io_data_t* in_msg)
{
	CLIENT *clnt;
	int  *result_1;
	//my_io_data_t  my_rpcc_1_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, MY_RPC_PROG, MY_RPC_VERS1, "udp");
	if (clnt == NULL) {
		//clnt_pcreateerror (host);
		printf("Fail to create rpc client1!\n");
		//exit (1);
		return -1;
	}
#endif	/* DEBUG */

	result_1 = my_rpcc_1(in_msg, clnt);
	if (result_1 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
		return -1;
	}
        return 0;
//#ifndef	DEBUG
//	clnt_destroy (clnt);
//#endif	 /* DEBUG */
}


my_io_data_t*
my_rpc_prog_2(char *host, my_io_data_t* in_msg)
{
	CLIENT *clnt;
	my_io_data_t  *result_1 = NULL;
	//my_io_data_t  my_rpcc_2_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, MY_RPC_PROG, MY_RPC_VERS2, "udp");
	if (clnt == NULL) {
		//clnt_pcreateerror (host);
		printf("Fail to create rpc client1!\n");
		//exit (1);
		return NULL;
	}
#endif	/* DEBUG */
        //my_rpcc_2_arg.mtype = 3;
        //my_rpcc_2_arg.len = 18;
	result_1 = my_rpcc_2(in_msg, clnt);
	if (result_1 == (my_io_data_t *) NULL) {
		clnt_perror (clnt, "call failed");
                return NULL;
	}
        return result_1;
        //fprintf(stderr, "recv msg from server! mtype:%d len:%d \n",result_1->mtype,result_1->len);
//#ifndef	DEBUG
//	clnt_destroy (clnt);
//#endif	 /* DEBUG */
}

void get_compute_result(char *host, int type, int para1, int para2)
{
    my_io_data_t in_msg;
    my_msg_t* rsp;
    my_io_data_t* out_msg;
    my_msg_t* req = (my_msg_t*) &in_msg;

    memset(&in_msg, 0, sizeof(in_msg));
    req->msg_hdr.mtype = type;
    req->msg_hdr.len = sizeof(in_msg) - sizeof(my_msg_hdr_t);
    req->para1 = para1;
    req->para2 = para2;

    out_msg = my_rpc_prog_2(host, &in_msg);

    rsp = (my_msg_t*)out_msg;

    if(NULL == rsp)
    {
        printf("RPC call fail\n");
        return ;
    }
    printf("compute result is %d\n", rsp->result);
}

void server_switch(char *host, int type)
{
    my_io_data_t msg;
    my_msg_t* in_msg = (my_msg_t*)&msg;
    memset(&msg, 0, sizeof(msg));
    in_msg->msg_hdr.mtype = type;
    in_msg->msg_hdr.len = sizeof(msg) - sizeof(my_msg_hdr_t);


    if(my_rpc_prog_1(host, &msg))
    {
        printf("enable server fail!\n");
    }
    printf("Configure server successfully!\n");
}

int
main (int argc, char *argv[])
{
	

        server_switch(SERVER_IP, RPC_enable);
        sleep(1);

        get_compute_result(SERVER_IP, RPC_ADD, 6, 3);
        sleep(1);

        get_compute_result(SERVER_IP, RPC_SUB, 6, 3);
        sleep(1);

        get_compute_result(SERVER_IP, RPC_MUL, 6, 3);
        sleep(1);

        get_compute_result(SERVER_IP, RPC_DIV, 6, 3);
        sleep(1);

        server_switch(SERVER_IP, RPC_disable);
        return 0;
//      char *host;

//	if (argc < 2) {
//		printf ("usage: %s server_host\n", argv[0]);
//		exit (1);
//	}
//	host = argv[1];
	//my_rpc_prog_1 (host);
//	my_rpc_prog_2 (host);
//exit (0);
}