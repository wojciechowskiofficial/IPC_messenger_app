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
	printf("$ + *enter*\t\t\tenter command mode\n");
	printf("$ + *enter* + help + *enter*\tprint help\n");
	printf("$ + *enter* + quit + *enter*\tquit command mode\n");
	printf("$ + *enter* + logout + *enter*\tlogout\n");
	printf("$ + *enter* + dm + *enter*\tenter dming mode\n");
	printf("$ + *enter* + request_logged + *enter*\n\n");
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

	printf("%s\n", req.login);

	msgsnd(mid, &req, sizeof(req) - sizeof(long), 0);
	sleep(1);
}

void dm_mode() {
	printf("were in dm mode\n");
}

void handle_resp_dm(char * login, char * password) {
	int mid = msgget(0x100, 0);

	Resp resp;
	msgrcv(mid, &resp, sizeof(resp) - sizeof(long), 7, IPC_NOWAIT);

	if(resp.type == 7) {
		printf("%s %s %d\n", resp.strings[0], resp.strings[1], resp.ints[0]);
	}
}

#endif
