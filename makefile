all: server.o 
	gcc server.o -o app
	./app
server.o: server.c funct_server.h structs.h
	gcc -c server.c 
clean:
	rm -f *.gch *.o app a.out
	ipcrm -a
