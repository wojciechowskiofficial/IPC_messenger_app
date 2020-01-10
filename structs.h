#include <string.h>

#define max_message_size 1024

#ifndef STRUCTS_H
#define STRUCTS_H

struct User {
	int id;
	char login[256];
	char password[256];
};

typedef struct User User;

struct Group {
	int id;
	//pointer to legitimate users array
	User * users;
	char name[256];
	int members[256];
	int end_ptr;
};

typedef struct Group Group;

//type: 1
struct Server_resp {
	long type;
	int code;
};

typedef struct Server_resp Server_resp;

//type: 2
struct Login_req {
	long type;
	char login[256];
	char password[256];
};

typedef struct Login_req Login_req;

//type: 3
struct Dm {
	long type;
	char text[max_message_size];
};

typedef struct Dm Dm;

#endif
