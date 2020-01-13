#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <signal.h>

int request_queue, receive_queue, send_queue;
int queues[3];

#include "funct_server.h"
#include "structs.h"

Dm_list ds;

void dm_init() {
	for (int i = 0; i < 16; i++) {
		ds.active_array[i] = 0;
	}
}

int dm_check_active(char * a, char * b) {
	for (int i = 0; i < 16; i++) {
		if ((!strcmp(a, ds.login_array[i][0]) || !strcmp(a, ds.login_array[i][1])) &&
		     (!strcmp(b, ds.login_array[i][0]) || !strcmp(b, ds.login_array[i][1])) &&
		     (ds.active_array[i] == 1)) return 1;
	}
	return 0;
}

void dm_activate(char * a, char * b) {
	for (int i = 0; i < 16; i++) {
		if (!ds.active_array[i]) {
			ds.active_array[i] = 1;
			strcpy(ds.login_array[i][0], a);
			strcpy(ds.login_array[i][1], b);
		}
	}
}

int main() {
	printf("server up\n");
	User users[256];
	Group groups[256];
	int user_nr, group_nr;

	user_nr = load_users(users);
	group_nr = load_groups(groups);
	//init dm conncections data structure
	dm_init(ds);

	request_queue = msgget(0x100, 0600 | IPC_CREAT);
	receive_queue = msgget(0x101, 0600 | IPC_CREAT);
	send_queue = msgget(0x102, 0600 | IPC_CREAT);
	queues[0] = request_queue;
	queues[1] = receive_queue;
	queues[2] = send_queue;
	signal(2, catch_sigint);

	while (1) {
		handle_login(users, user_nr);
		handle_logout(users, user_nr);
		handle_req_logged(users, user_nr);
		handle_req_dm(users, user_nr);
		sleep(1);
	}

	return 0;
}
