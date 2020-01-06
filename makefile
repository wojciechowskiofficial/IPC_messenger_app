all: server.o
	gcc server.o -o app
server.o: server.c structs.c
	gcc -c server.c structs.c
