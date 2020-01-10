#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include "funct_server.h"
#include "structs.h"

int main() {
	printf("server up\n");
	User users[256];
	Group groups[256];
	int user_nr, group_nr;

	user_nr = load_users(users);
	group_nr = load_groups(groups);

	int request_queue = msgget(0x100, 0600 | IPC_CREAT);
	int receive_queue = msgget(0x101, 0600 | IPC_CREAT);
	int send_queue = msgget(0x102, 0600 | IPC_CREAT);

	while (1) {
		handle_login(users, user_nr);
		sleep(1);
	}

	return 0;
}
