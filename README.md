# Assignment 10
## Submitted by Timofei Podkorytov
for operating systems course in fall 2024
## Compiling: Makefile
The following make commands are available:
```
$ make all
$ make server
$ make client
```
The first produces both server (./gwgd) and client(./nd) executable files. While the next 2 make only the corresponding one.

## Running:
These are inputs that are accepted by the prohram
```
./gwgd -p _number>0_ -f -t
./nd _string_ _port number_
```
The server can have a fork or thread option set as well as the port number. But both f and t cannot be set as it is contradictory.

The client takes a host string and a port number. If localhost is given it connects to local server instead.
## Files:
#### General:
- Makefile
- README.md
- chlng.c
- chlng.h
- utils.c
- utils.h
- tcp.h
- tcp.c
#### Client specific:
- client.c
- player.c
- player.h
#### Server specific:
- server_fun.h
- server_fun.c
- server.c

Makefile provides make commands. README gives the description of the overall homework submission.

Chlng file has functions for allocation and destruction of the chlng struct as well as producing the text and word in it, hiding the word.

Utils files have useful 2 functions that both client and server want to use.

TCP files contain the functions provided on the slides.

For client i have player files with player functions and client.c where the main fucntion is located.

For server I have a special file server_fun with all the function it needs as well as server.c where the main function can be found.

## Output
Here are some examples of output:
### Server:
```
$ ./gwgd -p 8000
p:8000 f:0 t:0
using blocking server
starting game with client: 4
text: Good-___.  I am leaving because I am bored.
                -- George Saunders' dying words
word: bye
received: *R: bye
*actual answer: bye
text: "... And remember: if you don't like the ____, go out and make some of
your own."
                -- "Scoop" Nisker, KFOG radio reporter Preposterous Words
word: news
received: *Q:
^C
```
### Client:
```
$ ./nd localhost 8000
M: Guess the missing ____!
M: Send your guess in the form 'R: word\r\n'
C: Good-___.  I am leaving because I am bored.
                -- George Saunders' dying words
R: bye
O: Congratulation - challenge passed!
C: "... And remember: if you don't like the ____, go out and make some of
your own."
                -- "Scoop" Nisker, KFOG radio reporter Preposterous Words
ending the game
M: You mastered 1/1 challenges. Good bye!
```