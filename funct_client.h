#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>

#include "structs.h"

void req_login(char * login, char * password) {
	int mid = msgget(0x100, 0);

	Login_req req;
	req.type = 2;
	strcpy(req.login, login);
	strcpy(req.password, password);

	msgsnd(mid, &req, sizeof(req) - sizeof(long), 0);
	sleep(1);

	Server_resp resp;
	int dump;
	msgrcv(mid, &resp, sizeof(resp) - sizeof(long), dump, 1);
	if (!resp.code) {
		printf("login successful!!!\n");
	}
	else {
		printf("login failed, deal with it\n");
		exit(0);
	}
}



void present_options() {
	printf("chat manual\n");
	printf("$ + *enter*\tenter command mode\n");
	printf("$ + *enter* + help + *enter*\tprint help\n");
	printf("$ + *enter* + quit + *enter*\tquit command mode\n");
	printf("$ + *enter* + logout + *enter*\tlogout\n");
	printf("$ + *enter* + dm + *enter*\tenter dming mode\n\n");
}
