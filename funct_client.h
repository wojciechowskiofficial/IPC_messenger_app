#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>

#include "structs.h"

#ifndef FUNCT_CLIENT_H
#define FUNCT_CLIENT_H

void req_login(char * login, char * password) {
	int mid = msgget(0x100, 0);

	Req req;
	req.type = 2;
	strcpy(req.login, login);
	strcpy(req.password, password);

	msgsnd(mid, &req, sizeof(req) - sizeof(long), 0);
	sleep(1);

	Resp resp;
	msgrcv(mid, &resp, sizeof(resp) - sizeof(long), 1, 0);
	if (!resp.code) {
		printf("login successful!!!\n\n");
	}
	else {
		printf("login failed, deal with it\n");
		exit(0);
	}
}

void present_options() {
	printf("chat manual\n\n");
	printf("$ + *enter*\t\t\t\tenter command mode\n");
	printf("$ + *enter* + help + *enter*\t\tprint help\n");
	printf("$ + *enter* + quit + *enter*\t\tquit command mode\n");
	printf("$ + *enter* + logout + *enter*\t\tlogout\n");
	printf("$ + *enter* + establish_dm + *enter*\tenter dming mode\n");
	printf("$ + *enter* + write_dm + *enter*\twrite direct message\n");
	printf("$ + *enter* + terminate_dm + *enter*\tquit dm mode\n");
	printf("$ + *enter* + request_logged + *enter*\trequest list of logged users\n\n");
}

void req_logout(char * login, char * password) {
	int mid = msgget(0x100, 0);
	
	Req req;
	req.type = 4;
	strcpy(req.login, login);
	strcpy(req.password, password);

	msgsnd(mid, &req, sizeof(req) - sizeof(long), 0);
	sleep(1);

	Resp resp;
	msgrcv(mid, &resp, sizeof(resp) - sizeof(long), 3, 0);
	if (!resp.code) {
		printf("logout successful!!!\n\n");
		exit(0);
	}
	else {
		printf("logout failed, deal with it\n");
		exit(-1);
	}
}

void req_logged(char * login, char * password) {
	int mid = msgget(0x100, 0);

	Req req;
	req.type = 6;
	strcpy(req.login, login);
	strcpy(req.password, password);
	
	msgsnd(mid, &req, sizeof(req) - sizeof(long), 0);
	sleep(1);

	Resp resp;
	msgrcv(mid, &resp, sizeof(resp) - sizeof(long), 5, 0);

	printf("logged users:\n");
	for (int i = 0; i < resp.ints[0]; i++) {
		printf("%s\n", resp.strings[i]);
	}
	printf("\n");
}

void req_dm(char * login, char * password, char * message_to) {
	int mid = msgget(0x100, 0);

	Req req;
	req.type = 8;
	strcpy(req.login, login);
	strcpy(req.password, password);
	strcpy(req.message_to, message_to);
	req.wanna_dm = 1;

	msgsnd(mid, &req, sizeof(req) - sizeof(long), 0);
	sleep(1);
}

void handle_resp_dm(char * login, char * password, Current_connection * curr_conn) {
	int mid = msgget(0x100, 0);

	Resp resp;
	msgrcv(mid, &resp, sizeof(resp) - sizeof(long), 7, IPC_NOWAIT);

	//condition prevents processing dm resps
	if (curr_conn->is_dming) return;

	strcpy(curr_conn->introvert, resp.strings[1]);
	strcpy(curr_conn->extrovert, resp.strings[0]);

	if (resp.type == 7) {
		if (!strcmp(curr_conn->extrovert, login)) {
			printf("message from: server\n");
			printf("dm connection with %s established successfuly\n\n", curr_conn->introvert);
			curr_conn->is_dming = 1;
			curr_conn->id = resp.ints[0];
			curr_conn->mid = msgget(0x200 + curr_conn->id, 0600);

		}
		if (!strcmp(curr_conn->introvert, login)) {
			printf("message from: server\n");
			printf("dm connection with %s established successfuly\n\n", curr_conn->extrovert);
			curr_conn->is_dming = 1;
			curr_conn->id = resp.ints[0];
			curr_conn->mid = msgget(0x200 + curr_conn->id, 0600);
		}
	}
}

void write_dm(char * login, char * password, Current_connection * curr_conn) {
	int mid = curr_conn->mid;

	printf("write your message down below (enters are forbidden):\n");
	char text[max_message_size];
	scanf("%s", text);

	Dm dm;
	dm.type = 21;
	dm.is_read = 0;
	//establishing intro/extrovert 
	strcpy(dm.introvert, curr_conn->introvert);
	strcpy(dm.extrovert, curr_conn->extrovert);
	//establishing from/to
	strcpy(dm.from, login);
	if (!strcmp(curr_conn->introvert, login)) {
		strcpy(dm.to, curr_conn->extrovert);
	}
	else {
		strcpy(dm.to, curr_conn->introvert);
	}
	strcpy(dm.text, text);

	msgsnd(mid, &dm, sizeof(dm) - sizeof(long), 0);
	sleep(1);
}

void handle_traffic(char * login, char * password, Current_connection * curr_conn) {
	if (!curr_conn->is_dming) return;

	int mid = curr_conn->mid;

	Dm dm;

	int receive_code = msgrcv(mid, &dm, sizeof(dm) - sizeof(long), 22, IPC_NOWAIT);

	if (!dm.is_read && dm.type == 22) {
		if (!strcmp(login, dm.from)) {
			printf("message from: server\n");
			printf("message delivered\n\n");
		}
		else {
			printf("message from %s\n", dm.from);
			printf("%s\n\n", dm.text);
		}
		dm.is_read = 1;
	}
}

void terminate_dm(char * login, char * password, Current_connection * curr_conn) {
	int mid = curr_conn->mid;
	Dm dm;
	dm.type = 23;
	dm.is_read = 0;
	dm.id_to_terminate = curr_conn->id;

	strcpy(dm.from, login);
	strcpy(dm.introvert, curr_conn->introvert);
	strcpy(dm.extrovert, curr_conn->extrovert);

	strcpy(curr_conn->introvert, "");
	strcpy(curr_conn->extrovert, "");
	curr_conn->id = 0;
	curr_conn->is_dming = 0;
	
	msgsnd(mid, &dm, sizeof(dm) - sizeof(long), 0);
}

void handle_terminate_dm(char * login, char * password, Current_connection * curr_conn) {
	if (curr_conn->is_dming) return;

	int mid = curr_conn->mid;

	Dm dm;

	int receive_code = msgrcv(mid, &dm, sizeof(dm) - sizeof(long), 244, IPC_NOWAIT);
	printf("wchodze\n");

	if (!dm.is_read && dm.type == 24) {
		printf("message from: server\n");
		printf("DM CONNECTION TERMINATED\n\n");
		dm.is_read = 1;
		if (!strcmp(dm.introvert, login)) {
			msgctl(mid, IPC_RMID, NULL);
		}
		curr_conn->mid = -1;
	}

}

#endif
