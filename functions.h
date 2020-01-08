#include "structs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void check_semi(FILE * in) {
	char semi[1];
	fscanf(in, "%s", semi);
	if (strcmp(semi, ";")) {
		printf("file format violated");
		exit(1);
	}
}

void load_users(User * users) {
	FILE * in;
	in = fopen("init.txt", "r");
	int user_nr;
	fscanf(in, "%d", &user_nr);
	check_semi(in);
	for (int i = 0; i < user_nr; i++) {
		fscanf(in, "%d", &users[i].id);
		fscanf(in, "%s", &users[i].login);
		fscanf(in, "%s", &users[i].password);
		check_semi(in);
	}
	fclose(in);
}

void load_groups(Group * groups) {
	FILE * in;
	in = fopen("init.txt", "r");
	int user_nr, group_nr;
	fscanf(in, "%d", &user_nr);
	check_semi(in);
	char dump[256];
	for (int i = 0; i < user_nr * 4; i++) fscanf(in, "%s", dump);
	fscanf(in, "%d", &group_nr);
	check_semi(in);
	int members_nr;
	for (int i = 0; i < group_nr; i++) {
		fscanf(in, "%d", &groups[i].id);
		fscanf(in, "%s", &groups[i].name);
		fscanf(in, "%d", &members_nr);
		for (int j = 0; j < members_nr; j++) {
			fscanf(in, "%d", &groups[i].members[j]);
		}
		groups[i].end_ptr = members_nr - 1;
		check_semi(in);
	}
}

#endif
