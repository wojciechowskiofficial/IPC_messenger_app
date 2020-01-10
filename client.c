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
	return 0;
}
