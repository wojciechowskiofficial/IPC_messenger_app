server: server.o 
	gcc server.o -o app
	./app
server.o: server.c funct_server.h structs.h
	gcc -c server.c 
client: client.o
	gcc client.o -o c
	./c
client.o: client.c funct_client.h structs.h
	gcc -c client.c
clean:
	rm -f *.gch *.o app a.out
	ipcrm -a
