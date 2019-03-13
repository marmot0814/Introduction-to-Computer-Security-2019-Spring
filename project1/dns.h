#include <stdio.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
struct dns {
	unsigned short transaction_id;
	unsigned short flags;
	unsigned short question_count;
	unsigned short answer_rrs;
	unsigned short authority_rrs;
	unsigned short additional_rrs;
};
struct _dns_query {
	unsigned short qtype;
	unsigned short qclass;
};
struct pseudo {
	u_int32_t src_ip;
	u_int32_t dst_ip;
	u_int8_t placeholder;
	u_int8_t protocol;
	u_int16_t udp_len;
};
void sendDNSQuery(char *src_ip, int src_port, char *dst_ip, int dst_port, char *query, int type);
