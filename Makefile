#assignmnet 10 by timofei podkorytov for OS24
server: server.c utils.c chlng.c tcp.c server_fun.c
	gcc -Wall -o gwgd server.c utils.c chlng.c tcp.c server_fun.c
client: client.c utils.c tcp.c player.c chlng.c
	gcc -Wall -o nd client.c utils.c tcp.c player.c chlng.c
all: client.c utils.c tcp.c player.c chlng.c server.c server_fun.c
	gcc -Wall -o gwgd server.c utils.c chlng.c tcp.c server_fun.c
	gcc -Wall -o nd client.c utils.c tcp.c player.c chlng.c