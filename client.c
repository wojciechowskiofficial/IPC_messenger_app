#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include "functions.h"
#include "structs.h"

int main() {
	int send_queue = msgget(0x123, 0600);
	Dm dm;
	dm.type = 1;
	strcpy(dm.text, "jajko");
	msgsnd(send_queue, &dm, 6, 0);
	return 0;
}
