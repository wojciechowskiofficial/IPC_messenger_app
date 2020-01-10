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

int load_users(User * users) {
	FILE * in;
	in = fopen("init.txt", "r");
	int user_nr;
	fscanf(in, "%d", &user_nr);
	check_semi(in);
	for (int i = 0; i < user_nr; i++) {
		users[i].is_logged = 0;
		fscanf(in, "%d", &users[i].id);
		fscanf(in, "%s", &users[i].login);
		fscanf(in, "%s", &users[i].password);
		check_semi(in);
	}
	fclose(in);
	return user_nr;
}

int load_groups(Group * groups) {
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
	fclose(in);
	return group_nr;
}

void handle_login(User * users, int user_nr) {
	Req req;
	int dump;
	int local_request_queue = msgget(0x100, 0600);
	int receive_code = msgrcv(local_request_queue, &req, sizeof(req) - sizeof(long), 2, IPC_NOWAIT);
	if (receive_code != -1) {
		Resp resp;
		resp.type = 1;
		resp.code = 1;
		for (int i = 0; i < user_nr; i++) {
			if (!strcmp(users[i].login, req.login) &&
			    !strcmp(users[i].password, req.password) &&
			    !users[i].is_logged) {
				resp.code = 0;
				users[i].is_logged = 1;
				printf("!@#$%^&*\n");
				printf("login successful\n");
				printf("login: %s\n", req.login);
				printf("password: %s\n\n", req.password);
				break;
			}
		}
		if (resp.code != 0) {
				printf("!@#$%^&*\n");
				printf("login failed\n");
				printf("login: %s\n", req.login);
				printf("password: %s\n\n", req.password);
		}
		msgsnd(local_request_queue, &resp, sizeof(Resp) - sizeof(long), 0);
	}
}

void handle_logout() {

}

#endif
