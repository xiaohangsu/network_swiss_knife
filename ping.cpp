#include <signal.h>
#include <sys/time.h>
#include <unistd.h>

// Define the Packet Constants
// ping packet size
#define PING_PKT_S 64

// Automatic port number
#define PING_SLEEP_RATE 1000000

#if defined(__APPLE__) && defined(__MACH__)
#include <mach/clock.h>
#include <mach/mach.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

int clock_gettime (int flag, struct timespec* t) {
	struct timeval now;
	int rv = gettimeofday(&now, NULL);
	if (rv) return rv;
	t->tv_sec = now.tv_sec;
	t->tv_nsec = now.tv_usec;
	return 0;
}
// ping packet structure
struct ping_pkt
{
    struct icmp hdr;
    char msg[PING_PKT_S - sizeof(struct icmp)];
};

#elif __linux__
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
struct ping_pkt
{
    struct icmphdr hdr;
    char msg[PING_PKT_S - sizeof(struct icmphdr)];
};
#endif


#include "header/dns_utils.h"
#include "header/ICMP_socket.h"
#include "header/utils.h"

// infinite ping loop
int pingloop = 1;

void intHandler(int dummy) {
        pingloop = 0;
}

void send_ping(const sockaddr_in* ping_addr, const char* ping_dom, const char* ping_ip, const char* rev_host) {
	ICMPSocket sock;
	struct sockaddr_in r_addr;
	struct timespec time_start, time_end, tfs, tfe;
	struct ping_pkt pckt;
	int flag = 1, msg_count = 0, msg_received_count = 0;
	long double rtt_msec=0, total_msec=0;
	clock_gettime(CLOCK_MONOTONIC, &tfs);

        while (pingloop) {
		// flag is whether packet was sent or not
		flag = 1;

		// filling packet
		bzero(&pckt, sizeof(pckt));
#if defined(__APPLE__) && defined(__MACH__)
		pckt.hdr.icmp_type = ICMP_ECHO;
		pckt.hdr.icmp_id = getpid();
        	for (int i = 0; i < sizeof(pckt.msg) - 1; i++)
            		pckt.msg[i] = i + '0';

        	pckt.msg[sizeof(pckt.msg) - 1] = 0;
        	pckt.hdr.icmp_seq = msg_count++;
        	pckt.hdr.icmp_cksum = checksum(&pckt, sizeof(pckt));
#elif __linux__
		pckt.hdr.type = ICMP_ECHO;
		pckt.hdr.un.echo.id = getpid();
		for (int i = 0; i < sizeof(pckt.msg)-1; i++)
		pckt.msg[i] = i + '0';

		pckt.msg[sizeof(pckt.msg) - 1] = 0;
		pckt.hdr.un.echo.sequence = msg_count++;
		pckt.hdr.checksum = checksum(&pckt, sizeof(pckt));
#endif
		usleep(PING_SLEEP_RATE);
		clock_gettime(CLOCK_MONOTONIC, &time_start);

		if (sock.send(&pckt, sizeof(pckt), (struct sockaddr*) ping_addr) <= 0) {
			printf("\nPacket sending failed.\n");
			flag = 0;
		}
		if (sock.recv(&pckt, sizeof(pckt), (struct sockaddr*)&r_addr) <= 0 && msg_count > 1) {
			printf("\nPacket receive failed!\n");
		} else {
			clock_gettime(CLOCK_MONOTONIC, &time_end);
			double timeElapsed = ((double)(time_end.tv_nsec -
                                 time_start.tv_nsec))/1000000.0;
            		rtt_msec = (time_end.tv_sec-
                          time_start.tv_sec) * 1000.0
                        + timeElapsed;

			// if packet was not sent, don't receive
			if(flag) {
#if defined(__APPLE__) && defined(__MACH__)
				if(!(pckt.hdr.icmp_type ==69 && pckt.hdr.icmp_code ==0)) {
					printf("Error..Packet received with ICMP type %d code %d\n",
					       pckt.hdr.icmp_type, pckt.hdr.icmp_code);
				}

#elif __linux__
				if(!(pckt.hdr.type ==69 && pckt.hdr.code==0)) {
					printf("Error..Packet received with ICMP type %d code %d\n",
					       pckt.hdr.type, pckt.hdr.code);
				}


#endif
				else {
					printf("%d bytes from %s (h: %s) (%s) msg_seq=%d ttl=%d rtt = %Lf ms.\n",
						PING_PKT_S, ping_dom, rev_host,
						ping_ip, msg_count,
						PING_TTL, rtt_msec);

					msg_received_count++;
				}
			}
		}
        }

	clock_gettime(CLOCK_MONOTONIC, &tfe);
	double timeElapsed = ((double)(tfe.tv_nsec - tfs.tv_nsec))/1000000.0;

	total_msec = (tfe.tv_sec-tfs.tv_sec)*1000.0+ timeElapsed;

	printf("\n===%s ping statistics===\n", ping_ip);
	printf("\n%d packets sent, %d packets received, %f percent packet loss. Total time: %Lf ms.\n\n",
		msg_count, msg_received_count,
		((msg_count - msg_received_count)/msg_count) * 100.0,
		total_msec);
}

int main(int argc, char *argv[]) {

	if(argc!=2)
	{
		printf("\nFormat %s <address>\n", argv[0]);
		return 0;
	}

        char* ip_addr, *reverse_hostname;
        struct sockaddr_in addr_con;
        signal(SIGINT, intHandler);

        ip_addr = DNSUtils::dns_lookup(argv[1], &addr_con);
	if(ip_addr==NULL)
	{
		printf("\nDNS lookup failed! Could not resolve hostname!\n");
		return 1;
	}

        reverse_hostname = DNSUtils::reverse_dns_lookup(ip_addr);
	printf("\nTrying to connect to '%s' IP: %s\n", argv[1], ip_addr);
	printf("\nReverse Lookup domain: %s\n", reverse_hostname);

        send_ping(&addr_con, reverse_hostname, ip_addr, argv[1]);

        return 0;
}