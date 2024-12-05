/*client.c*/
/*
This is assignment 10
Done by Timofei Podkorytov
for Operating systems course
in fall 2024
*/
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils.h"
#include "player.h"
#include "tcp.h"
//GET INPUT FOR CLIENT
int getInput(int argc, char* argv[], char** host){
    if (argc<3){
        fprintf(stderr,"too few arguments\n");
        return 0;
    } else if(argc>3){
        fprintf(stderr,"too many arguments\n");
        return 0;
    }
    int port;
    int suc = sscanf(argv[2], "%d", &port);
    if(suc<0){
        perror("sscanf");
        return 0;
    }
    if (strcmp(argv[1],"localhost")==0){
        *host=strdup(HOST);
    } else {
        *host=strdup(argv[1]);
    }
    return port;
}

int sock;
//CLIENT MAIN FUNCTION
int main(int argc, char* argv[]){
    player_t *p;
    char *msg;
    char* host;
    int rc;
    //we get the input message
    int port = getInput(argc, argv, &host);
    p = player_new();
        if (! p) {
        return EXIT_FAILURE;
    }
    //we connect to the server
    sock = tcp_connect((const char*) host, int_to_str(port));
    free(host);
    if (sock<0){
        exit(EXIT_FAILURE);
    }
    //we do the greeting part
    rc = player_get_greeting(p, &msg);
    if (rc > 0) {
        (void) fputs(msg, stdout);
        (void) free(msg);
    }
    //WHILE
    while (!p->finished){
        char *line = NULL;
        size_t linecap = 0;

        rc = player_get_challenge(p, &msg);
        if (rc > 0) {
            (void) fputs(msg, stdout);
            (void) free(msg);
        } else {
            printf("error rc=%d\n",rc);
        }

        if (getline(&line, &linecap, stdin) <= 0) {
            printf("ending the game\n");
            rc = player_post_challenge(p, "Q:\r\n", &msg);
            if (rc > 0) {
                (void) fputs(msg, stdout);
                (void) free(msg);
                if(line!=NULL){
                    (void) free(line);
                }
            }
            break;
        }
        rc = player_post_challenge(p, line, &msg);
        if (rc > 0) {
            (void) fputs(msg, stdout);
            (void) free(msg);
        }
        if(line!=NULL){
            (void) free(line);
        }
    }
    printf("\n");
    player_del(p);
    return EXIT_SUCCESS;
}