#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "header/dns_utils.h"

void wrongFormatAndExit(char *argv[]) {
        printf("\nFormat %s <ip/hostname>\n", argv[0]);
        exit(0);
}

int main(int argc, char *argv[]) {

	if(argc!=2)
	{
		wrongFormatAndExit(argv);
	}

        char* res;
        unsigned char buf[sizeof(struct in6_addr)];
        if (argc == 2) {
                printf("\nChecking if it is an ipv4...\n");
                int ret = inet_pton(AF_INET, argv[1], buf);
                if (ret <= 0) {
                        printf("\nIt is not a IPv4 address: %s.\n", argv[1]);
                        printf("\nChecking if it is an ipv6...\n");
                        int ret = inet_pton(AF_INET6, argv[1], buf);
                        if (ret <= 0) {
                                printf("\nIt is not a IPv6 address: %s.\n", argv[1]);
                                printf("\nstarting using %s as hostname:\n", argv[1]);
                                res = DNSUtils::reverse_dns_lookup(argv[1]);
                                if (res != NULL) {
                                        printf("\n%s resovled with: %s\n", argv[1], res);
                                        return 0;
                                }
                        }
                }

                struct sockaddr_in addr_con;
                res = DNSUtils::dns_lookup(argv[1], &addr_con);
                if (res != NULL) {
                        printf("%s resovled with: %s", argv[1], res);
                        return 0;
                }
        } else {
                wrongFormatAndExit(argv);
        }
        return 0;
}