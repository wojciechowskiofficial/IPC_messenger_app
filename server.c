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
		for (int i = 0; i < user_nr; i++) {
			printf("%s %d %d\n", users[i].login, users[i].is_logged, users[i].is_writing);
		}
		printf("%s %s\n", ds.login_array[1][0], ds.login_array[1][1]);
		sleep(1);
	}

	return 0;
}
