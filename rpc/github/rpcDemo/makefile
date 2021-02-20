obj1 = my_clnt.c my_client.c my_xdr.c
obj2 = my_svc.c my_server.c my_xdr.c

all: client server

client:${obj1}
	gcc -o client ${obj1}

server:${obj2}
	gcc -o server ${obj2} 

clean:
	rm -f client server
