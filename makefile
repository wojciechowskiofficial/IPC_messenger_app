all: server.o 
	gcc server.o -o app
	./app
server.o: server.c functions.h structs.h
	gcc -c server.c 
clean:
	rm -f *.gch *.o app
