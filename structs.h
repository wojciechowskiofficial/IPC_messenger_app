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
#endif
