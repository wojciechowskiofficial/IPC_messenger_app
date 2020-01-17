#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <poll.h>
#include <signal.h>

#include "inf136376_structs.h"
#include "inf136376_funct_client.h"

int id;
char login[256];
char password[256];
Current_connection * curr_conn;
int is_logged;

void catch_sigint(int sig) {
	if (curr_conn->is_dming) {
		terminate_dm(login, password, curr_conn);
		Dm dm;
		sleep(1);
		msgrcv(curr_conn->mid, &dm, sizeof(dm) - sizeof(long), 24, IPC_NOWAIT);
		sleep(1);
		msgctl(curr_conn->mid, IPC_RMID, NULL);
	}
	if (is_logged) {
		req_logout(login, password, curr_conn);	
	}

}

int main() {
	is_logged = 0;
	printf("welcome to the lowest level chat the world has ever got addicted to!!!\n");
	printf("please login\n");
	printf("login: ");
	scanf("%s", login);
	printf("password: ");
	scanf("%s", password);
	printf("\n");

	req_login(login, password);
	is_logged = 1;
	
	char interact[1024];
	strcpy(interact, "");
	curr_conn = (Current_connection *)malloc(sizeof(Current_connection));
	curr_conn->is_dming = 0;
	//to prevent collision since id range from 0 to 15 inclusive
	curr_conn->id = -1;

	//catch sig_int
	signal(2, catch_sigint);

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
				else if (!curr_conn->is_dming && !strcmp(interact, "logout")) {
					req_logout(login, password, curr_conn);	
				}
				else if (!curr_conn->is_dming && !strcmp(interact, "request_logged")) {
					req_logged(login, password);
				}
				else if (!curr_conn->is_dming && !strcmp(interact, "establish_dm")) {
					printf("establish connection with: ");
					char message_to[256];
					scanf("%s", message_to);
					if (!strcmp(message_to, login)) {
						printf("please do not write to yourself. it's irrational. go out or get some help...\n");
					}
					else {
						req_dm(login, password, message_to);
					}
					printf("\n");
				}
				else if (!strcmp(interact, "write_dm")) {
					write_dm(login, password, curr_conn);
				}
				else if (!strcmp(interact, "terminate_dm")) {
						terminate_dm(login, password, curr_conn);
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
		handle_traffic(login, password, curr_conn);
		handle_terminate_dm(login, password, curr_conn);
	}
	return 0;
}
