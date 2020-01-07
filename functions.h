#include "structs.h"

#include <stdio.h>
#include <string.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void load_users(User * users) {
	FILE * in;
	in = fopen("init.txt", "r");
	int user_nr;
	char semicolon[256];
	fscanf(in, "%d", &user_nr);
	fscanf(in, "%s", semicolon);
	for (int i = 0; i < user_nr; i++) {
		fscanf(in, "%s", &users[i].login);
		fscanf(in, "%s", &users[i].password);
		fscanf(in, "%s", semicolon);
	}
	fclose(in);
}
#endif
