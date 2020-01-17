server: inf136376_s.o 
	gcc inf136376_s.o -o app
	./app
inf136376_s.o: inf136376_s.c inf136376_funct_server.h inf136376_structs.h
	gcc -c inf136376_s.c 
client: inf136376_c.o
	gcc inf136376_c.o -o c
	./c
inf136376_c.o: inf136376_c.c inf136376_funct_client.h inf136376_structs.h
	gcc -c inf136376_c.c
clean:
	rm -f *.gch *.o app a.out
	ipcrm -a
