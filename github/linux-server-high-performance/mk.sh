# !/bin/sh

#simple make command
g++ -Wall epollclient.cpp -o epollclient.out
g++ -Wall epollserver.cpp -o epollserver.out

g++ 5-3-testlisten.c -o testlisten.out
g++ 5-5-testaccept.c -o testaccept.out
g++ 5-6-testoobsend.c -o testoobsend.out
g++ 5-7-testoobrecv.c -o testoobrecv.out
g++ 5-10-setsendbuffer.c -o setsendbuffer.out
g++ 5-11-setrecvbuffer.c -o setrecvbuffer.out
g++ 5-12-daytime.c -o daytime.out
g++ 6-1-dup-cgi.c -o  dup-cgi.out
g++ 6-2-web-writev.c -o web-writev.out
g++ 6-3-sendfile.c -o sendfiletest.out
g++ 6-4-splice.c   -o splicetest.out
g++ 6-5-tee.c -o teetest.out
g++ 7-1-uideuid.c -o uideuidtest.out
g++ 8-3-http-analyze.c -o http-analyze.out
g++ 9-1-select-recvdata.c -o select-recvdata.out
g++ 9-3-epoll-ltandet.c -o epoll-ltoret.out
g++ 9-4-epolloneshot.c -o epolloneshottest.out -lpthread
g++ 9-5-nonblock-connect.c -o nonblock-connect.out
g++ 9-6-client.c -o client-topic.out
g++ 9-7-server.c -o server-topic.out
g++ 9-8-tcp-udp.c -o tcp-udp-server.out
g++ 10-1-unify-signal.c -o unified-signal.out
g++ 10-3-sigurg-signal.c -o sigurg-signal.out
gcc 13-3-ipc_private.c -o ipc-private.out
g++ 13-4-shm-chatroom.c -o shm-chatroom.out -lrt
g++ 13-5-send-fd.c -o send-fd.out
g++ 14-1-dead-lock.c -o dea-lock.out -lpthread
g++ 14-3-multi-thread-fork.c -o multi-thread-fork.out -lpthread
g++ 14-5-thread-handle-signal.c -o threadhandlesignal.out -lpthread
g++ processpool.h 15-2-process-pool-cgi.c -o process-pool-cgi.out