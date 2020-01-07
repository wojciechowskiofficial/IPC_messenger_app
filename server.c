#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "functions.h"
#include "structs.h"

int main() {
	User users[256];
	load_users(users);
	return 0;
}
