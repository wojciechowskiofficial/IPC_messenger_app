#include "structs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

Dm_list ds;

void catch_sigint(int sig) {
	for (int i = 0; i < 3; i++) {
		msgctl(queues[i], IPC_RMID, NULL);
	}
	for (int i = 0; i < 16; i++) {
		if (ds.active_array[i]) {
			msgctl(ds.mid_array[i], IPC_RMID, NULL);
		}
	}
	exit(EXIT_SUCCESS);
}

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
			int ds_connection_iter = dm_activate(resp.strings[0], resp.strings[1]);
			resp.ints[0] = ds_connection_iter;
			//printf("%s %s\n", resp.strings[0], resp.strings[1]);
			//turning is_writing flag to 1 in both users
			users[message_to_id].is_writing = 1;
			users[who_is_that(users, user_nr, req.login)].is_writing = 1;
			//creating dm queue
			ds.mid_array[ds_connection_iter] = msgget(0x200 + ds_connection_iter, 0600 | IPC_CREAT);
			//establish logged users number
			int nr = establish_logged_users_nr(users, user_nr);
			//send x messages where x is logged users number
			for (int i = 0; i < nr; i++) {
				msgsnd(local_request_queue, &resp, sizeof(Resp) - sizeof(long), 0);
			}
			printf("!@#$%^&*\n");
			printf("dm req successful\n");
			printf("login: %s\n", req.login);
			printf("password: %s\n\n", req.password);
		}
		if (resp.code != 0) {
			printf("!@#$%^&*\n");
			printf("dm req failed\n");
			printf("login: %s\n", req.login);
			printf("password: %s\n\n", req.password);
		}
	}
}

Dm cp_dm(Dm in_dm) {
	Dm out_dm;
	out_dm.type = in_dm.type;
	strcpy(out_dm.introvert, in_dm.introvert);
	strcpy(out_dm.extrovert, in_dm.extrovert);
	strcpy(out_dm.from, in_dm.from);
	strcpy(out_dm.to, in_dm.to);
	strcpy(out_dm.text, in_dm.text);
	out_dm.is_read = in_dm.is_read;
	return out_dm;
}

void handle_traffic(User * users, int user_nr) {
	Dm in_dm;
	for (int i = 0; i < 16; i++) {
		if (ds.active_array[i]) {
			int receive_code = msgrcv(ds.mid_array[i], &in_dm, sizeof(in_dm) - sizeof(long), 21, IPC_NOWAIT);
			if (!in_dm.is_read && in_dm.type == 21) {
				//copy received in_dm to out_dm
				Dm out_dm = cp_dm(in_dm);
				//change in_dm fields
				in_dm.is_read = 1;
				//change out_dm fields
				out_dm.type = 22;
				out_dm.is_read = 0;
				msgsnd(ds.mid_array[i], &out_dm, sizeof(out_dm) - sizeof(long), 0);
				msgsnd(ds.mid_array[i], &out_dm, sizeof(out_dm) - sizeof(long), 0);
				printf("!@#$%^&*\n");
				printf("dm propagation successful\n");
				//getting users password
				char tmp_password[256];
				strcpy(tmp_password, users[who_is_that(users, user_nr, in_dm.from)].password);
				printf("login: %s\n", in_dm.from);
				printf("password: %s\n\n", tmp_password);

			}
		}
	}
}

void handle_req_dm_termination(User * users, int user_nr) {
	Dm in_dm;
	for (int i = 0; i < 16; i++) {
		if (ds.active_array[i]) {
			int receive_code = msgrcv(ds.mid_array[i], &in_dm, sizeof(in_dm) - sizeof(long), 23, IPC_NOWAIT);
			if (!in_dm.is_read && in_dm.type == 23 && i == in_dm.id_to_terminate) {
				//creating out_dm
				Dm out_dm;
				out_dm.type = 24;
				out_dm.is_read = 0;
				strcpy(out_dm.introvert, in_dm.introvert);
				strcpy(out_dm.extrovert, in_dm.extrovert);
				strcpy(out_dm.from, in_dm.from);
				int tmp_mid = ds.mid_array[in_dm.id_to_terminate];
				//performing dm_termination actions on ds
				ds.active_array[in_dm.id_to_terminate] = 0;
				ds.mid_array[in_dm.id_to_terminate] = -1;
				strcpy(ds.login_array[in_dm.id_to_terminate][0], "");
				strcpy(ds.login_array[in_dm.id_to_terminate][1], "");
				in_dm.is_read = 1;
				//responding to both clients
				msgsnd(tmp_mid, &out_dm, sizeof(out_dm) - sizeof(long), 0);
				msgsnd(tmp_mid, &out_dm, sizeof(out_dm) - sizeof(long), 0);
				//server print message
				printf("!@#$%^&*\n");
				printf("dm termination successful\n");
				//getting users password
				char tmp_password[256];
				strcpy(tmp_password, users[who_is_that(users, user_nr, out_dm.from)].password);
				printf("login: %s\n", out_dm.from);
				printf("password: %s\n\n", tmp_password);
			}
		}
	}
}

#endif
