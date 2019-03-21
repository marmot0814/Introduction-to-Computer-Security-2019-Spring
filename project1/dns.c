#include "dns.h"
#define PKG_LEN 4096
void createDNSHeader(struct dns *dns_header) {
	dns_header->transaction_id = (unsigned short)htons(getpid());
	dns_header->flags = htons(0x0100);
	dns_header->question_count = htons(1);
	dns_header->answer_rrs = 0;
	dns_header->authority_rrs = 0;
	dns_header->additional_rrs = htons(1);
}
void formatDNSQuery(char *dns_header, char *query) {
    // transform query into dns form, for example: www.google.com => 3w6google3com
	strcat(query, ".");
	for (int i = 0, lock = 0 ; query[i] ; i++) {
		if (query[i] == '.') {
			*dns_header++ = i - lock;
			for ( ; lock < i ; lock++)
				*dns_header++ = query[lock];
			lock++;
		}
	}
	*dns_header++ = '\0';
}
void createDNSQuery(char *dns_header, char *query, unsigned long *dns_data_len, int type) {
	formatDNSQuery(dns_header, query);
	struct _dns_query *dns_query = (struct _dns_query *)(dns_header + strlen((const char *)dns_header) + 1);
	dns_query->qtype = htons(type);
	dns_query->qclass = htons(1);
    char *edns = (char *)(dns_header + strlen((const char *)dns_header) + 1 + sizeof(struct _dns_query)) + 1;
    memset(edns    , 0x00, 1);
    memset(edns + 1, 0x29, 1);
    memset(edns + 2, 0xFF, 2);
    memset(edns + 4, 0x00, 7);
	*dns_data_len = sizeof(struct dns) + strlen((const char *)dns_header) + 1 + sizeof(struct _dns_query) + 11;
}
void createDNSData(char *dns_data, unsigned long *dns_data_len, char *src_ip, int src_port, char *dst_ip, int dst_port, char *query, int type) {
	struct dns *dns_header = (struct dns *)dns_data;
	createDNSHeader(dns_header);
	createDNSQuery((char *)(dns_data + sizeof(struct dns)), query, dns_data_len, type);
}
void createUDPHeader(unsigned long dns_data_len, struct udphdr *udp_header, int src_port, int dst_port) {
	udp_header->uh_sport = htons(src_port);
	udp_header->uh_dport = htons(dst_port);
	udp_header->uh_ulen = htons(sizeof(struct udphdr) + dns_data_len);
	udp_header->uh_sum = 0;
}
unsigned short calculateCheckSum(unsigned short *ptr, int nbytes) {
    // calculate checksum, sum of each two bytes(short)
	register long sum = 0, answer;
	unsigned short oddbyte;
	while (nbytes > 1) {
		sum += *ptr++;
		nbytes -= 2;
	}

    // special case: odd bytes
	if (nbytes == 1) {
		oddbyte = 0;
		*((u_char *)&oddbyte) = *(u_char *)ptr;
		sum += oddbyte;
	}

    // add carry to low bit when overflow
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);

    // flip each bits and get the checksum
	answer = (short)~sum;
	return (answer);
}
void createIPHeader(unsigned long dns_data_len, struct ip *ip_header, char *src_ip, char *dst_ip, struct pseudo *pseudo_header) {
	ip_header->ip_hl = 5;
	ip_header->ip_v = IPVERSION;
	ip_header->ip_tos = IPTOS_PREC_ROUTINE;
	ip_header->ip_len = sizeof(struct ip) + sizeof(struct udphdr) + dns_data_len;
	ip_header->ip_id = htons(getpid());
	ip_header->ip_off = 0;
	ip_header->ip_ttl = MAXTTL;
	ip_header->ip_p = IPPROTO_UDP;
	ip_header->ip_src.s_addr = inet_addr(src_ip);
	ip_header->ip_dst.s_addr = inet_addr(dst_ip);
	ip_header->ip_sum = calculateCheckSum((unsigned short *)ip_header, ip_header->ip_len);

	pseudo_header->src_ip = ip_header->ip_src.s_addr;
	pseudo_header->dst_ip = ip_header->ip_dst.s_addr;
	pseudo_header->placeholder = 0;
	pseudo_header->protocol = IPPROTO_UDP;
	pseudo_header->udp_len = htons(sizeof(struct udphdr) + dns_data_len);
}
void sendDNSQuery(char *src_ip, int src_port, char *dst_ip, int dst_port, char *_query, int type) {

	// create datagram array and ip udp pointer
	char datagram[PKG_LEN], query[100]; 
	memcpy(query, _query, strlen(_query) + 1);
	struct ip *ip_header = (struct ip *) datagram;
	struct udphdr *udp_header = (struct udphdr *)(datagram + sizeof(struct ip));

	// create dns array and length
	char dns_data[1000];
	unsigned long dns_data_len;

	// create DNS data
	createDNSData(dns_data, &dns_data_len, src_ip, src_port, dst_ip, dst_port, query, type);

    // copy dns header to datagram
	memcpy(datagram + sizeof(struct ip) + sizeof(struct udphdr), dns_data, dns_data_len);

    // create UDP header
	createUDPHeader(dns_data_len, udp_header, src_port, dst_port);

    // create socket object and ste dst_port and dst_ip
	struct sockaddr_in dst;
	dst.sin_family = AF_INET;
	dst.sin_port = htons(dst_port);
	dst.sin_addr.s_addr = inet_addr(dst_ip);

    // collect target which need to be calculated in UDP checksum
	struct pseudo pseudo_header;

    // create IP header
	createIPHeader(dns_data_len, ip_header, src_ip, dst_ip, &pseudo_header);

    // setup udp checksum from pseudo header
	int pseudo_len = (int)(sizeof(struct pseudo) + sizeof(struct udphdr) + dns_data_len);
	char *pseudo_data = malloc(pseudo_len);
	memcpy(pseudo_data, (char *)&pseudo_header, sizeof(struct pseudo));
	memcpy(pseudo_data + sizeof(struct pseudo), udp_header, sizeof(struct udphdr) + dns_data_len);
	udp_header->uh_sum = calculateCheckSum((unsigned short *)pseudo_data, pseudo_len);

    // send packet to socket
	int s = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (s < 0)
		perror("socet failed.\n");
	const int on = 1;
	if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0)
		perror("setsockopt failed.\n");
	int t;
	if (t = sendto(s, datagram, ip_header->ip_len, 0, (struct sockaddr *)&dst, sizeof(dst)) < 0)
		perror("sendto failed.\n");
	printf ("%d\n", t);

}
