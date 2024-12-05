/*server.c*/
/*
This is assignment 10
Done by Timofei Podkorytov
for Operating systems course
in fall 2024
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include "chlng.h"
#include "utils.h"
#include "tcp.h"
#include "server_fun.h"

int sock;

int main(int argc, char* argv[]){
    //we retreive the input and set the parameters
    input inp;
    inp.port=0;
    inp.thread=0;
    inp.fork=0;
    //we initate rand
    srand(time(NULL));
    //we get input processed
    getOptions(argc,argv,&inp);
    //we output inital data in the server log
    printf("p:%d f:%d t:%d\n", inp.port, inp.fork, inp.thread);
    //we create the server
    start_server(inp.port);
    while(1){
        //we accept the connection
        int acpt = accept_client();
        if (acpt<0){
            exit(EXIT_FAILURE);
            close(sock);
        }
        //we process the client
        if(inp.fork==0 && inp.thread==0){
            //BLOCKING SERVER
            //we process client in ghe main thread
            printf("using blocking server\n");
            int rc = process_client(acpt);
            close(acpt);
            if(rc<0){//we check for errors
                close(sock);
                exit(EXIT_FAILURE);
            }
        } else if (inp.thread==1){
            //MULTI THREAD SERVER
            printf("using thread server\n");
            //we create a thread
            pthread_t tid;
            //we allocate memory for the file descriptor
            int* client_sock = malloc(sizeof(int));
            if (!client_sock) {
                perror("malloc");
                close(acpt);
                close(sock);
                exit(EXIT_FAILURE);
            }
            *client_sock = acpt;
            //detached attribute is created here
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            //we create thread
            if (pthread_create(&tid, &attr, thread_client, client_sock) != 0) {
                perror("pthread_create");
                free(client_sock);
                close(acpt);
                close(sock);
                exit(EXIT_FAILURE);
            }
            //we destroy attribute
            pthread_attr_destroy(&attr);
        } else {
            //FORKING SERVER
            printf("using forking server\n");
            //we fork
            int rc = fork();
            int status;
            if (rc==-1){
            //ERROR
                perror("fork");
                close(acpt);
                close(sock);
                exit(EXIT_FAILURE);
            } else if(rc==0){
            //CHILD
                close(sock);//we close the listening socket
                int rc = process_client(acpt);
                close(acpt);//we close the client
                if(rc<0){//check for errors
                    close(sock);
                    exit(EXIT_FAILURE);
                }
                exit(EXIT_SUCCESS);
            }
            //PARENT
            close(acpt); 
            //we wait for any exited process
            while (waitpid(-1, &status, WNOHANG) > 0) {
                if(status==EXIT_FAILURE){
                    close(sock);
                    exit(EXIT_FAILURE);
                }
            }     
        }
    }
    close(sock);
    return 0;
}
