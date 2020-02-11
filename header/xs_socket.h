#pragma once

#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

class XSSocket {
private:
        int sockfd;
public:
        XSSocket(int domain, int type, int protocol) {
                sockfd = socket(domain, type, protocol);
                if(sockfd<0) {
                        printf("\nSocket file descriptor not received!!\n");
                        exit(1);
                } else {
                        printf("\nSocket file descriptor %d received\n", sockfd);
                }
        }

        virtual size_t send(const void *pckt, size_t pckt_size, const sockaddr* ip_addr) = 0;

        virtual size_t recv(void * pckt, size_t pckt_size, sockaddr* r_addr) = 0;

        int get_socket_fd() {
                return sockfd;
        }

        ~XSSocket() {
                close(sockfd);
        }
};
