#include <string.h>

#define max_message_size 1024

#ifndef STRUCTS_H
#define STRUCTS_H

struct User {
	int id;
	char login[256];
	char password[256];
	int is_logged;
	int is_writing;
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

struct Resp {
	long type;
	char strings[16][256];
	int ints[16];
	int code;
};

typedef struct Resp Resp;

struct Req {
	long type;
	char login[256];
	char password[256];
	int wanna_dm, wanna_gm;
	char message_to[256];
};

typedef struct Req Req;

struct Dm {
	long type;
	char text[max_message_size];
};

typedef struct Dm Dm;

#endif
