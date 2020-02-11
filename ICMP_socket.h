#pragma once

#if defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#include <netinet/in.h>
#define SOL_IP IPPROTO_IP
#endif

#include "xs_socket.h"

// Gives the timeout delay for receiving packets
// in seconds
#define RECV_TIMEOUT 1

static int PING_TTL = 64;

class ICMPSocket : public XSSocket {
private:
        struct timeval tv_out;
public:
        ICMPSocket()
        : XSSocket(AF_INET, SOCK_RAW, IPPROTO_ICMP) {
                tv_out.tv_sec = RECV_TIMEOUT;
                tv_out.tv_usec = 0;
                int ttl_val = PING_TTL;
                if (setsockopt(get_socket_fd(), SOL_IP, IP_TTL, &ttl_val, sizeof(ttl_val)) != 0) {
                        printf("\nSetting socket options to TTL failed!\n");
                        return;
                } else {
                        printf("\nSocket set TTL..\n");
                }
                if (setsockopt(get_socket_fd(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_out, sizeof tv_out) != 0) {
                        printf("\nSetting socket options SO_RCVTIMEO failed!\n");
                        return;
                } else {
                        printf("\nSocket set SO_RCVTIMEO..\n");
                }
        }

        virtual size_t send(const void *pckt, size_t pckt_size, const sockaddr* ip_addr) {
                return sendto(get_socket_fd(), pckt, pckt_size, 0, (struct sockaddr*)ip_addr, sizeof(ip_addr));
        }

        virtual size_t recv(void * pckt, size_t pckt_size, sockaddr* r_addr) {
                socklen_t addr_len = sizeof(r_addr);
                return recvfrom(get_socket_fd(), pckt, pckt_size, 0, r_addr, &addr_len);
        }

};