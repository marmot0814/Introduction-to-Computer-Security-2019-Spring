#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define PCKT_LEN 8192

struct ipheader {
	unsigned char      iph_ihl:5, iph_ver:4;
	unsigned char      iph_tos;
	unsigned short int iph_len;
	unsigned short int iph_ident;
	unsigned char      iph_flag;
	unsigned short int iph_offset;
	unsigned char      iph_ttl;
	unsigned char      iph_protocol;
	unsigned short int iph_chksum;
	unsigned int       iph_sourceip;
	unsigned int       iph_destip;
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
    if (argc != 5) {
        printf("- Usage %s <source hostname/IP> <source port> <target hostname/IP> <target port>\n", argv[0]);
        exit(-1);
    }

	char buffer[PCKT_LEN];
    struct ipheader *ip = (struct ipheader *) buffer;
    struct udpheader *udp = (struct udpheader *) (buffer + sizeof(struct ipheader));
    int sd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP);
    printf ("%d\n", sd);
    if (sd < 0) {
        perror("socket() error");
        exit(-1);
    } else printf("socket() - Using SOCK_RAW socket and UDP protocol is OK.\n");

    struct sockaddr_in sin, din;
    sin.sin_family = AF_INET;
    din.sin_family = AF_INET;


}
