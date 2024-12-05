/*
This is assignment 10
Done by Timofei Podkorytov
for Operating systems course
in fall 2024
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "chlng.h"
/* Create new struct instance*/ 
extern chlng_t* chlng_new(void){
    chlng_t* text;
    text=(chlng_t*) malloc(sizeof(chlng_t));
    if (text==NULL){
        perror("malloc");
        return NULL;
    }
    text->word=NULL;
    text->text=NULL;
    return text;
}
/* Reset the internal state of the challenge. */
extern void chlng_reset(chlng_t* text){
    free(text->word);
    free(text->text);
    text->word=NULL;
    text->text=NULL;
}
/* Delete a challenge and all its resources. */
extern void chlng_del(chlng_t* text){
    free(text->word);
    free(text->text);
    free(text);
}
/* Fetch new text from an invocation of 'fortune'. */
extern int chlng_fetch_text(chlng_t *c){
    pid_t pid;
    int pfd[2];
    if (pipe(pfd) == -1) {//pipe is created
        perror("pipe");
        return -1;
    }
    pid = fork();//we fork
    if (pid == -1) {
        (void) close(pfd[0]);
        (void) close(pfd[1]);
        perror("fork");
        return -1;
    }
    if (pid == 0) {
        //CHILD
        if(dup2(pfd[1],STDOUT_FILENO)==-1){
            perror("dup2");
            (void) close(pfd[0]);
            (void) close(pfd[1]);
            exit(-1);
        }
        (void) close(pfd[0]);
        (void) close(pfd[1]);
        execlp("fortune","fortune","-s", NULL); // we launch the command in child
        perror("execlp");
        exit(-1);
    } else {
        //PARENT
        //we create a buffer
        char* temp;
        size_t already_read=0;
        size_t new_read=0;
        size_t size=2048;
        int status;
        temp = (char*) malloc(sizeof(char)*size);
        if(temp==NULL){ //check for correct allocation
            perror("malloc");
            (void) close(pfd[0]);
            (void) close(pfd[1]);
            return -1;
        }
        //we close of the writing end
        (void) close(pfd[1]);
        //reading starts here
        while ((new_read = read(pfd[0], temp+already_read, size-already_read-1)) > 0) {
            already_read+=new_read;
            //do we need more space?
            if(already_read>size-1){
                size*=2;
                char* new_temp = realloc(temp, size);
                if (new_temp==NULL){
                    perror("realloc");
                    (void) close(pfd[0]);
                    free(temp);
                    return -1;
                }
                temp=new_temp;
            }
        }        
        (void) close(pfd[0]);//we close the pipe
        temp[already_read]='\0';//we put the 0 at the end of a string to terminate it
        if(waitpid(pid,&status,0)==-1){//wait for child
            return -1;
        }
        c->text=strdup(temp);
        free(temp);
        return already_read;
    }
    return -1;
}
/* Select and hide a word in the text. */
extern int chlng_hide_word(chlng_t *c){
    char** words=NULL;
    int count = get_words(c->text, &words);//we split the text
    if(count==-1){
        fprintf(stderr,"error extracting words\n");
        return -1;
    }
    int repl = rand()%count;
    c->word=strdup(words[repl]);//we chose the word
    if(c->word==NULL){
        perror("strdup");
        return -1;
    }
    //now we replace the word with '_'
    char* location=strstr(c->text,c->word);
    char* ch=location;
    for(;isalpha(*ch);ch++){
        *ch='_';
    }
    //we free the words array after use
    for(int i=0;i<count;i++){
        free(words[i]);
    }
    free(words);
    //we print the data in server log
    printf("text: %s",c->text);
    printf("word: %s\n", c->word);
    return 0;
}
/* split the string into an array of words */
int get_words(char* text, char*** out_words){
    char* delim = " ";
    char* temp_text=strdup(text);
    //we get rid of redundant character as we do not care about them
    for (char* ch = temp_text;*ch;ch++){
        if(!isalpha(*ch)){
            *ch=' ';
        }
    }
    //we tokenize the string
    char** words;
    int size=16;
    int count=0;
    //we allocate memory
    words=(char**) malloc(sizeof(char*)*size);
    if (words==NULL){
        perror("malloc");
        return -1;
    }
    char* split = strtok(temp_text, delim);
    //we go through the words
    while (split!=NULL){
        if(count==size){//limit was reached
            //we increase the size
            size*=2;
            char** temp_words=realloc(words,sizeof(char*)*size);
            if(temp_words==NULL){
                free(temp_text);
                for (int i = 0; i < count; i++) {
                    free(words[i]);
                }
                free(words);
                perror("realloc");
                return -1;
            }
            words=temp_words;
        }
        words[count] = strdup(split);//we copy data
        if (words[count] == NULL) {
            perror("strdup");
            free(temp_text);
            for (int i = 0; i < count; i++) {
                free(words[i]);
            }
            free(words);
            return -1;
        }
        //we only want words over 2 symbols long as otherwise its too short
        if(strlen(words[count])>2){
            count++;
        } else {//not long enough
            free(words[count]);
        }
        split = strtok(NULL,delim);
    }
    free(temp_text);
    *out_words = words; 
    return count;
}