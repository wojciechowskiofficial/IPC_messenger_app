#include "structs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void catch_sigint(int sig) {
	for (int i = 0; i < 3; i++) {
		msgctl(queues[i], IPC_RMID, NULL);
	}
	exit(EXIT_SUCCESS);
}

Dm_list ds;

void dm_init() {
	for (int i = 0; i < 16; i++) {
		ds.active_array[i] = 0;
		ds.id_array[i] = i;
	}
}

int dm_get_id(char * a, char * b) {
	for (int i = 0; i < 16; i++) {
		if ((!strcmp(a, ds.login_array[i][0]) || !strcmp(a, ds.login_array[i][1])) &&
		     (!strcmp(b, ds.login_array[i][0]) || !strcmp(b, ds.login_array[i][1])) &&
		     (ds.active_array[i] == 1)) return i;
	}
	return -1;
}

int dm_activate(char * a, char * b) {
	for (int i = 0; i < 16; i++) {
		if (!ds.active_array[i]) {
			ds.active_array[i] = 1;
			strcpy(ds.login_array[i][0], a);
			strcpy(ds.login_array[i][1], b);
			return i;
		}
	}
}

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

void handle_logout(User * users, int user_nr) {
	Req req;
	int local_request_queue = msgget(0x100, 0600);
	int receive_code = msgrcv(local_request_queue, &req, sizeof(req) - sizeof(long), 4, IPC_NOWAIT);
	if (receive_code != -1) {
		Resp resp;
		resp.type = 3;
		resp.code = 1;
		for (int i = 0; i < user_nr; i++) {
			if (!strcmp(users[i].login, req.login) &&
			    !strcmp(users[i].password, req.password) &&
			    users[i].is_logged) {
				resp.code = 0;
				users[i].is_logged = 0;
				printf("!@#$%^&*\n");
				printf("logout successful\n");
				printf("login: %s\n", req.login);
				printf("password: %s\n\n", req.password);
				break;
			}
		}
		if (resp.code != 0) {
				printf("!@#$%^&*\n");
				printf("logout failed\n");
				printf("login: %s\n", req.login);
				printf("password: %s\n\n", req.password);
		}
		msgsnd(local_request_queue, &resp, sizeof(Resp) - sizeof(long), 0);
	}
}

void handle_req_logged(User * users, int user_nr) {
	Req req;
	int local_request_queue = msgget(0x100, 0600);
	int receive_code = msgrcv(local_request_queue, &req, sizeof(req) - sizeof(long), 6, IPC_NOWAIT);
	if (receive_code != -1) {
		Resp resp;
		resp.type = 5;
		resp.code = 1;
		int iter = 0;
		for (int i = 0; i < user_nr; i++) {
			if (users[i].is_logged) {
				strcpy(resp.strings[iter++], users[i].login);
			}
		}
		resp.ints[0] = iter;
		resp.code = 0;
		printf("!@#$%^&*\n");
		printf("logged req successful\n");
		printf("login: %s\n", req.login);
		printf("password: %s\n\n", req.password);
		msgsnd(local_request_queue, &resp, sizeof(Resp) - sizeof(long), 0);
	}
}

int is_user_avaliable(User * users, int user_nr, char * nick) {
	for (int i = 0; i < user_nr; i++) {
		if (!strcmp(users[i].login, nick) &&
		users[i].is_logged &&
		!users[i].is_writing) {
			return i;
		}
	}
	return -1;
}

int establish_logged_users_nr(User * users, int user_nr) {
	int nr = 0;
	for (int i = 0; i < user_nr; i++) {
		if (users[i].is_logged) nr++;
	}
	return nr;
}

int who_is_that(User * users, int user_nr, char * nick) {
	for (int i = 0; i < user_nr; i++) {
		if (!strcmp(users[i].login, nick)) return i;
	}
	return -1;
}

void handle_req_dm(User * users, int user_nr) {
	Req req;
	int local_request_queue = msgget(0x100, 0600);
	int receive_code = msgrcv(local_request_queue, &req, sizeof(req) - sizeof(long), 8, IPC_NOWAIT);
	printf("%s %s %d\n", req.login, req.password, req.type);
	Resp resp;
	if (!req.wanna_dm) resp.code = -1;
	if (receive_code != -1) {
		resp.type = 7;
		resp.code = 1;
		int message_to_id = is_user_avaliable(users, user_nr, req.message_to);
		if (message_to_id != -1) {
			//defining pair of users that are going to dm
			strcpy(resp.strings[0], req.login);
			//printf("%s %s\n", resp.strings[0], req.login);
			strcpy(resp.strings[1], users[message_to_id].login);
			//resp.ints[0] = dm_get_id(resp.strings[0], resp.strings[1]);
			resp.code = 0;
			//activate dm connection in dm struct
			resp.ints[0] = dm_activate(resp.strings[0], resp.strings[1]);
			//printf("%s %s\n", resp.strings[0], resp.strings[1]);
			//turning is_writing flag to 1 in both users
			users[message_to_id].is_writing = 1;
			users[who_is_that(users, user_nr, req.login)].is_writing = 1;
			//establish logged users number
			int nr = establish_logged_users_nr(users, user_nr);
			printf("!@#$%^&*\n");
			printf("dm req successful\n");
			printf("login: %s\n", req.login);
			printf("password: %s\n\n", req.password);
			//send x messages where x is logged users number
			for (int i = 0; i < nr; i++) {
				msgsnd(local_request_queue, &resp, sizeof(Resp) - sizeof(long), 0);
			}
		}
		if (resp.code != 0) {
			printf("!@#$%^&*\n");
			printf("dm req failed\n");
			printf("login: %s\n", req.login);
			printf("password: %s\n\n", req.password);
		}
	}
}
#endif
