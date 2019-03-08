#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

#define PCKT_LEN 8192

struct ipheader {
	unsigned char      iph_ihl:5, iph_ver:4;    // ip header length, ip header version
	unsigned char      iph_tos;                 // ip header type to service
	unsigned short int iph_len;                 // ip header datagram length
	unsigned short int iph_ident;               // ip header identifier
	unsigned char      iph_flag;                // ip header flag
	unsigned short int iph_offset;				// ip header offset
	unsigned char      iph_ttl;					// ip header time to live
	unsigned char      iph_protocol;			// ip header protocol
	unsigned short int iph_chksum;				// ip header check sum
	unsigned int       iph_sourceip;			// ip header source ip
	unsigned int       iph_destip;				// ip header destination ip
};

struct udpheader {
	unsigned short int udph_srcport;
	unsigned short int udph_destport;
	unsigned short int udph_len;
	unsigned short int udph_chksum;
};

unsigned short csum(unsigned short *buf, int nwords) {
    unsigned long sum;
    for(sum=0; nwords>0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum &0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

int main(int argc, char *argv[]) {

    // fool-proof design
    if (argc != 5) {
        printf("- Usage %s <source hostname/IP> <source port> <target hostname/IP> <target port>\n", argv[0]);
        exit(-1);
    }

    // datagram
	char buffer[PCKT_LEN];

    // header's structure
    struct ipheader *ip = (struct ipheader *) buffer;
    struct udpheader *udp = (struct udpheader *) (buffer + sizeof(struct ipheader));

    // create a raw socket with UDP protocol
    int sd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP);
    printf ("%d\n", sd);
    if (sd < 0) {
        // fail to create
        perror("socket() error. Be sure you are rooted");
        exit(-1);
    } else printf("socket() - Using SOCK_RAW socket and UDP protocol is OK.\n");

    struct sockaddr_in sin, din;
    // address family
    sin.sin_family = AF_INET;
    din.sin_family = AF_INET;
    // Port numbers
    sin.sin_port = htons(atoi(argv[2]));
    din.sin_port = htons(atoi(argv[4]));
    // IP addresses
    sin.sin_addr.s_addr = inet_addr(argv[1]);
    din.sin_addr.s_addr = inet_addr(argv[3]);

    // ip header
    ip->iph_ihl = 5;
    ip->iph_ver = 4;
    ip->iph_tos = 16;	// low delay
    ip->iph_len = sizeof(struct ipheader) + sizeof(struct udpheader);
    ip->iph_ident = htons(54321);
    ip->iph_ttl = 64;
    ip->iph_protocol = 17;
    ip->iph_sourceip = inet_addr(argv[1]);
    ip->iph_destip = inet_addr(argv[3]);

    udp->udph_srcport = htons(atoi(argv[2]));
	udp->udph_destport = htons(atoi(argv[4]));
	udp->udph_len = htons(sizeof(struct udpheader));
	
	ip->iph_chksum = csum((unsigned short *)buffer, sizeof(struct ipheader) + sizeof(struct udpheader));
	
	int one = 1;
	const int *val = &one;
	if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
		perror("setsockopt() error");
		exit(-1);
	} else printf("setsockopt() is OK.\n");
	printf("Trying...\n");
	printf("Using raw socket and UDP protocol\n");
	printf("Using Source IP: %s port: %u, Target IP: %s port: %u.\n", argv[1], atoi(argv[2]), argv[3], atoi(argv[4]));

	for (int cnt = 1 ; cnt <= 20 ; cnt++) {
		if (sendto(sd, buffer, ip->iph_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
			perror("sendto() error");
			exit(-1);
		} else printf("Count #%u - sendto() is OK.\n", cnt), sleep(2);
	}
	close(sd);
	return 0;


}
