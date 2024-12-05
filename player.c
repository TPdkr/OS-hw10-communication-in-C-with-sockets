/*player.c*/
/*
This is assignment 10
Done by Timofei Podkorytov
for Operating systems course
in fall 2024
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "player.h"
#include "chlng.h"
#include "tcp.h"

/* Allocate a new player. */
extern player_t* player_new(void){
    player_t* player;
    player=malloc(sizeof(player));
    if (player==NULL){
        perror("malloc");
        return NULL;
    }//we set deafult values
    player->solved=0;
    player->total=0;
    player->finished=false;
    player->chlng=chlng_new();
    if (player->chlng==NULL){
        fprintf(stderr,"error creating chlng\n");
        free(player);
        return NULL;
    }
    return player;
}
/* Reset the internal state of the player. */
extern void player_reset(player_t* player){
    player->solved=0;
    player->total=0;
    player->finished=false;
    chlng_reset(player->chlng);
}
/* Delete a player and all its resources. */
extern void player_del(player_t* player){
    chlng_del(player->chlng);
    free(player);
}
/* Allocate a new challenge for the player. */
extern int player_fetch_chlng(player_t* p){
    return 0;  
}
/* Retrieve a greeting message. */
extern int player_get_greeting(player_t *p, char ** msg){
    char buf[256];
    size_t buf_len=256;
    memset(buf, 0, buf_len);//we make sure that the buffer is clean
    if(tcp_read(sock, buf, buf_len)<0){
        fprintf(stderr,"Error in tcp reading\n");
        return -1;
    }
    *msg=strdup(buf);//copy buffer into a pointer
    if(!msg){
        perror("strdup");
        return -1;
    }
    return strlen(*msg);

}
/* Retrieve the challenge message. */
extern int player_get_challenge(player_t* p, char **msg){
    char buf[256];
    size_t buf_len=256;
    memset(buf, 0, buf_len);//we clean the buffer
    if(tcp_read(sock, buf, buf_len)<0){
        fprintf(stderr,"Error in tcp reading\n");
        return -1;
    }
    *msg=strdup(buf);//we copy the message
    if(!msg){
        perror("strdup");
        return -1;
    }
    p->total++;
    return strlen(*msg);
}
/* Post a message to the player and retrieve the response message. */
extern int player_post_challenge(player_t* p, char *answ, char **msg){
    char buf[256];
    size_t buf_len=256;
    //we send our answer
    if(tcp_write(sock, answ, strlen(answ))<0){
        fprintf(stderr,"Error in tcp writing\n");
        return -1;
    }
    //we retreive the response
    memset(buf, 0, buf_len);
    if(tcp_read(sock, buf, buf_len)<0){
        fprintf(stderr,"Error in tcp reading\n");
        return -1;
    }
    *msg=strdup(buf);//result is copied
    if(!msg){
        perror("strdup");
        return -1;
    }
    if ((*msg)[1]=='O'){
        p->solved++;
    }
    return strlen(*msg);
}