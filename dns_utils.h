#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Automatic port number
#define PORT_NO 0

class DNSUtils {
private:

public:
        static char* dns_lookup(char * addr_host, struct sockaddr_in * addr_con) {
                printf("\n Resolving DNS...\n");
                struct hostent *host_entity;
                char* ip = (char*)malloc(NI_MAXHOST*sizeof(char));

                int i;

                if ((host_entity = gethostbyname(addr_host)) == NULL) {
                        printf("\n No ip found for hostname \n");
                        return NULL;
                }

                strcpy(ip, inet_ntoa(*(struct in_addr *)
                                     host_entity->h_addr));

                (*addr_con).sin_family = host_entity->h_addrtype;
                (*addr_con).sin_port = htons(PORT_NO);
                (*addr_con).sin_addr.s_addr = *(long*)host_entity->h_addr;

                return ip;

        }

        static char* reverse_dns_lookup(char * ip_addr) {
                struct sockaddr_in temp_addr;
                socklen_t len;
                char buf[NI_MAXHOST], *ret_buf;

                temp_addr.sin_family = AF_INET;
                temp_addr.sin_addr.s_addr = inet_addr(ip_addr);
                len = sizeof(struct sockaddr_in);

                if (getnameinfo((struct sockaddr *) &temp_addr, len, buf, sizeof(buf), NULL, 0, NI_NAMEREQD)) {
                        printf("Could not resolve reverse lookup of hostname\n");
                        return NULL;
                }

                ret_buf = (char*)malloc((strlen(buf) + 1) * sizeof(char));

                strcpy(ret_buf, buf);

                return ret_buf;
        }
};