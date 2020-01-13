#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include "structs.h"
#include "funct_client.h"

int id;
char login[256];
char password[256];

int main() {
	printf("welcome to the lowest level chat the world has ever got addicted to!!!\n");
	printf("please login\n");
	printf("login: ");
	scanf("%s", login);
	printf("password: ");
	scanf("%s", password);
	printf("\n");

	req_login(login, password);
	
	char interact[1024];
	int is_dming = 0;

	present_options();

	while (1) {
		scanf("%s", interact);
		//entering command mode
		if (!strcmp(interact, "$")) {
			printf("COMMAND MODE\n\n");
			scanf("%s", interact);
			if (!strcmp(interact, "quit")) {
				//do nothing
			}
			else if (!strcmp(interact, "help")) {
				present_options();
			}
			else if (!strcmp(interact, "logout")) {
				req_logout(login, password);	
			}
			else if (!strcmp(interact, "request_logged")) {
				req_logged(login, password);
			}
			else if (!strcmp(interact, "dm")) {
				printf("establish connection with: ");
				char message_to[256];
				scanf("%s", message_to);
				req_dm(login, password, message_to);
			}
			else if (!strcmp(interact, "")) {

			}
			else {
				printf("unrecognised command\n\n");
			}
		}
	}
	return 0;
}
