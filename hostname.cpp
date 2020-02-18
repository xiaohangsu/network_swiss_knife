#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// TODO
int main(int argc, char *argv[]) {

	if(argc!=2)
	{
		printf("\nFormat %s <hostname>\n", argv[0]);
		return 0;
	}

        char* hostname = argv[1];
        if (argc == 2) {
                int ret = gethostname(hostname, strlen(hostname));

        } else {

        }

        return 0;
}