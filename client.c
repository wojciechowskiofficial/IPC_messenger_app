#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <poll.h>

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
	
	printf("%s\n", login);
	char interact[1024];
	strcpy(interact, "");
	Current_connection * curr_conn;
	curr_conn = (Current_connection *)malloc(sizeof(Current_connection));
	curr_conn->is_dming = 0;

	present_options();

	//using poll function to disable scanf function process blocking
	struct pollfd mypoll = {STDIN_FILENO, POLLIN | POLLPRI};

	while (1) {
		//asynchronous section
		if (poll(&mypoll, 1, 1000)) {
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
				else if (!strcmp(interact, "write_dm")) {
					write_dm(login, password, curr_conn);
				}
				else if (!strcmp(interact, "")) {

				}
				else {
					printf("unrecognised command\n\n");
				}
			}
		}
		//synchronous section
		//frequency ~ 1Hz
		handle_resp_dm(login, password, curr_conn);
	}
	return 0;
}
