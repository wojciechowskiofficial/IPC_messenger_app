#ifndef STRUCTS_H
#define STRUCTS_H
struct User {
	int id;
	char login[256];
	char password[256];
};

typedef struct User User;
#endif
