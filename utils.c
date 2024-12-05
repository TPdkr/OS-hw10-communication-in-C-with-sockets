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

//I defined important values
//#define LISTN_BCKLG 100
//int domain = AF_UNIX;

//COVERT INT TO STRING---------------------------------------------------------
const char* int_to_str(int value) {
    static char buffer[20];
    snprintf(buffer, sizeof(buffer), "%d", value);
    return buffer;
}
//GET POSITIVE INT-------------------------------------------------------------
void getPosInt(int* num, char flag,char inp[]){
    int suc=sscanf(inp, "%d", num);
    //is the arguemnt int check
    if (suc!=1){
        fprintf(stderr, "server: invalid number %s for -%c option\n", inp,flag);
        exit(1);
    }
    //invalid int check
    if(*num<0){
        fprintf(stderr,"server: value %d for -%c option should be >= 0\n", *num,flag);
        exit(1);
    }
}