#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>

#include "structs.h"

void req_login(char * login, char * password) {
	int mid = msgget(0x100, 0);

	Login_req req;
	req.type = 2;
	strcpy(req.login, login);
	strcpy(req.password, password);

	msgsnd(mid, &req, sizeof(req) - sizeof(long), 0);

	Server_resp resp;
	int dump;
	msgrcv(mid, &resp, sizeof(resp) - sizeof(long), dump, 1);
	if (!resp.code) {
		printf("login successful!!!\n");
	}
	else {
		printf("login failed, deal with it\n");
	}
}
