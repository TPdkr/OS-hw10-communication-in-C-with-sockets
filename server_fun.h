/*server_fun.h*/
/*
This is assignment 10
Done by Timofei Podkorytov
for Operating systems course
in fall 2024
*/
#ifndef SERVER_FUN
#define SERVER_FUN

#include "chlng.h"
#define LISTN_BCKLG 100

//inout struct for server
typedef struct {
    int port;
    int fork;
    int thread;
}input;
//the strings we pass to client
extern const char* START_MESSAGE;
extern const char* INVALID_SYNTAX;
extern const char* SUCCESS;
extern const char* WRONG;
extern const char* END;
//socket fd
extern int sock;

//INOUT------------------------------------------------------------------------
//GET OPTIONS
void getOptions( int argc, char* argv[], input* inp);
//MAIN FUNCTIONS---------------------------------------------------------------

/*QUERY
This function takes a pointer to text and resets it and puts new values 
in it*/
int query(chlng_t* text);
/*START SERVER
This function starts a server on a given port*/
void start_server(int port);
/*ACCEPT CLIENT
This function accepts a client connection and return a file descrptor*/
int accept_client();
/*PROCESS CLIENT
This function processes communication with one client based on a given file 
decriptor*/
int process_client(int client);

//COMMUNICATION FUNCTIONS------------------------------------------------------
/*SEND INITAL MESSAGE
This function sends the inital message from server to client*/
int send_init(int acpt);
/*SEND QUESTION
Send a given text in text struct to client*/
int send_question(int client, chlng_t* text);
/*GET ANSWER
Read answer and evaluate it. The result is saved into answered
*/
int get_answer(int client, chlng_t* text, int* answered, int total);

//PARALLEL EXECUTION===========================================================
/*EXECUTE IN A SEPARATE THREAD
This function processes clients using threads*/
void* thread_client(void* socket_descriptor);
#endif