/*server_fun.c*/
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

#include "server_fun.h"
#include "chlng.h"
#include "utils.h"
#include "tcp.h"

//VARIABLES:
const char* START_MESSAGE="M: Guess the missing ____!\nM: Send your guess in the form \'R: word\\r\\n\'\0";
const char* INVALID_SYNTAX="M: Error in formating, the answer was counted as wrong\n";
const char* SUCCESS = "O: Congratulation - challenge passed!\n";
const char* WRONG = "F: Wrong guess \'%s\' - expected \'%s\'\n";
const char* END = "M: You mastered %d/%d challenges. Good bye!";

//INPUT------------------------------------------------------------------------
//GET OPTIONS
void getOptions( int argc, char* argv[], input* inp){
    //the argument position
    int argPos=1;
    //we set default values
    //machineInit(machine, toStudent, toSuppliers);
    int opt;
    //we use getopt for options
    while((opt = getopt(argc, argv, "ftp:"))!=-1){
        switch(opt){
            case 'p':
                getPosInt(&inp->port,'p',optarg);
                argPos+=2;
                break;
            case 'f':
                inp->fork=1;
                argPos+=1;
                break;
            case 't':
                inp->thread=1;;
                argPos+=1;
                break;
            case ':'://nothing to be done but call an error
            case '?'://unknown flag meaning invalid input
                fprintf(stderr,"invalid option '%c'\n", opt);
                exit(1);
                break;
        }
    }
    //is there still something left? ERROR in input
    if(argv[argPos]!=NULL){
        fprintf(stderr,"too many arguments supplied\n");
        exit(EXIT_FAILURE);
    }
    if(inp->fork==1 && inp->thread==1){
        fprintf(stderr,"Server cannot both fork and multithread. Make a choice\n");
        exit(EXIT_FAILURE);
    }
}
//QUERY------------------------------------------------------------------------
int query(chlng_t* text){
    //we reset the data
    chlng_reset(text);
    //we fetch the data in a child process
    int len = chlng_fetch_text(text);
    if(len<0){
        fprintf(stderr,"fetching text failed\n");
        return -1;
    }
    //we hide the word and store it in word property
    int hide = chlng_hide_word(text);
    if(hide<0){
        fprintf(stderr,"error while hiding word\n");
        return -1;
    }
    return 0;
    //we clean after oursleves
}
//START SERVER-----------------------------------------------------------------
void start_server(int port){
    sock = tcp_listen((const char*)HOST, int_to_str(port));
    if(sock==-1){
        fprintf(stderr,"error initating the server\n");
        exit(EXIT_FAILURE);
    }
    int lst = listen(sock, LISTN_BCKLG);
    if (lst<0){
        fprintf(stderr,"error listening\n");
        exit(EXIT_FAILURE);
    }
}
//ACCEPT CLIENT----------------------------------------------------------------
int accept_client(){
    int acpt = accept(sock, NULL,NULL);
    
    if (acpt<0){
        fprintf(stderr,"error accepting connection\n");
        return -1;
    }
    return acpt;
}
//PROCESS CLIENT---------------------------------------------------------------
int process_client(int client){
    int answered=0;
    int total=0;
    //send the start message
    if(send_init(client)){
        return -1;
    }
    //we create the struct
    chlng_t* text=NULL;
    text = chlng_new();
    if (text==NULL){
        fprintf(stderr,"error creatinf a chlng_t struct\n");
    }
    //now we enter the loop
    while(1){
        total++;
        //we retreive the text struct
        if(query(text)<0){
            fprintf(stderr,"error generating question\n");
            chlng_del(text);
            return -1;
        }
        //we send the task
        if(send_question(client, text)<0){
            chlng_del(text);
            return -1;
        }
        //we check the answer
        if(get_answer(client,text,&answered,total)){
            break;
        }
    }
    chlng_del(text);
    return 0;
}
//SEND ITIAL MESSAGE-----------------------------------------------------------
int send_init(int acpt){
    char buf[256];
    //size_t buf_len=256;
    //we need to send the start response
    printf("starting game with client: %d\n", acpt);
    snprintf(buf, sizeof(buf),"%s\n", START_MESSAGE);
    if (tcp_write(acpt, buf, strlen(buf))<0){
        fprintf(stderr,"error sending inital message\n");
        return -1;
    }
    return 0;
}
//SEND QUESTION----------------------------------------------------------------
int send_question(int client, chlng_t* text){
    char buf[256];
    snprintf(buf, sizeof(buf),"C: %s", text->text);
    if(tcp_write(client, buf, strlen(buf))<0){
        fprintf(stderr,"error sending question to client\n");
        return -1;
    }
    return 0;
}
//GET ANSWER-------------------------------------------------------------------
int get_answer(int client, chlng_t* text, int* answered, int total){
    char msg_buf[256];
    size_t msg_buf_len=256;
    char buf[256];
    size_t buf_len=256;
    char answ[256];
    memset(buf, 0, buf_len);
    int read_len = tcp_read(client,msg_buf,msg_buf_len);
    if (read_len<=0){
        fprintf(stderr,"error reading from client or connection lost\n");
        exit(EXIT_FAILURE);
    }
    printf("received: *%s*", msg_buf);

    //we process the answer
    if (sscanf(msg_buf,"R: %s\r\n",answ)==1){
        printf("actual answer: %s\n",answ);
        if(strcmp(answ,text->word)==0){
            (*answered)++;
            tcp_write(client, SUCCESS, strlen(SUCCESS));
        } else {
            snprintf(buf, sizeof(buf), WRONG, answ, text->word);
            tcp_write(client,buf, buf_len);
        }
    } else if(strncmp(msg_buf, "Q:", 2) == 0){
        snprintf(buf, sizeof(buf), END, *answered, total-1);
        tcp_write(client,buf, buf_len);
        return 1;
    } else {
        fprintf(stderr,"error reading\n");
        tcp_write(client, INVALID_SYNTAX, strlen(INVALID_SYNTAX));
    }
    return 0;
}
//=============================================================================
void* thread_client(void* socket_descriptor){
    int client = *((int*) socket_descriptor);
    free(socket_descriptor);
    int rc = process_client(client);
    close(client);
    if(rc<0){
        perror("Error processing client");
    }
    pthread_exit(NULL);
}
