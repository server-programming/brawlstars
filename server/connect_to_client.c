#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int connect_to_client(int ns, int cur_client_num, char *buf, int flag) {
	int network_status;
	char buf1[50];
	char buf2[1024];

	if (flag == 1) {
		strcpy(buf1, buf);
		network_status = send(ns, buf1, sizeof(buf1), 0);
	}

	if (flag == 2) {
		strcpy(buf2, buf);
		network_status = send(ns, buf2, sizeof(buf2), 0);
	}

	if (network_status == -1 || network_status == 0) {
		return 0;
	} else {
		return 1;
	}
}
