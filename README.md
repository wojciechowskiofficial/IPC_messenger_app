server ---[1 ... inf]--- architecture - server is run separately from clients

APPRECIATED
simplest way to run whole system:
	1. run *make server* to start server
	2. run *make client* to start clients (from different terminal that server)

DEPRECIATED
compiling server:
	1. run *gcc inf136376_s.c -o server*

running server:
	1. compile server
	2. run *./server*

compiling client:
	1. run *gcc inf136376_c.c -o client*

running client:
	1. compile client
	2. run *./client*

inf136376_s.c
is a server side. in main function:
	1. users and groups are loaded from init.txt
	2. crucial administrative queues are created
	3. infinite while loop is run
in infinite while loop:
	1. runs request login handler
	2. runs request logout handler
	3. runs request logged users list handler
	4. runs request direct message connection handler
	5. runs request traffic handler (dm propagating engine) 
	6. runs request direct message connection termination handler

inf136376_c.c
is a client side.
	1. crucial local data structures are defined before main function
	2. void catch_siging(int sig) sigint handling function created
	3. main function
in main function:
	1. is_logged flag is set to 0
	2. welcoming message is printed
	3. login and password are scaned
	4. login request is sent to server
	5. login response is received by client
	6. is_logged flag is set to 1
	7. more local data structures are defined such as current connection structure and interact variable
	8. signal function is run to secure sigint
	9. user is presented with options
	10. infinite while loop is started
in infinite while loop:
	1. there is 1 second to asynchronously interact with client by typing $ symbol and pressing enter
	2. switch case alike structure is run which switches between requests: logout, request_logged, establish_dm, write_dm, terminate_dm if interact variable was scanned
	3. synchronously handlers: response direct message, traffic receiver, terminate direct message signale receiver are run if interact variable was not scanned

all necessary functions are in inf136376_funct_[client/server].h files
all necessary data structures are in inf136376_structs.h file
