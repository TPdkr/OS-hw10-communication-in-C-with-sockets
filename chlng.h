/*
* chlng.h
*/
/*
This is assignment 10
Done by Timofei Podkorytov
for Operating systems course
in fall 2024
*/
#ifndef CHLNG_H
#define CHLNG_H
typedef struct {
    char *text;/* text with a missing word */
    char *word;/* the missing word */
} chlng_t;
/* Allocate a new challenge. */
extern chlng_t* chlng_new(void);
/* Reset the internal state of the challenge. */
extern void chlng_reset(chlng_t*);
/* Delete a challenge and all its resources. */
extern void chlng_del(chlng_t*);
/* Fetch new text from an invocation of 'fortune'. */
extern int chlng_fetch_text(chlng_t *c);
/* Select and hide a word in the text. */
extern int chlng_hide_word(chlng_t *c);
/* Split the text into words array (mine)*/
int get_words(char* text, char*** out_words);
#endif