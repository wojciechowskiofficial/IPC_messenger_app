#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include "functions.h"
#include "structs.h"

int main() {
	User users[256];
	load_users(users);
	Group groups[256];
	load_groups(groups);
	int receive_queue = msgget(0x123, 0600 | IPC_CREAT);
	//int send_queue = msgget(0x101, 0600 | IPC_CREAT);
	Dm dm;
	msgrcv(receive_queue, &dm, max_message_size, 1, 0);
	printf("%s", dm.text);
	return 0;
}
