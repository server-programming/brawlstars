#ifndef NETWORK_CONNECT
#define NETWORK_CONNECT

typedef struct {
	struct sockaddr_in sin;
	int sd;
	int client_num;
} network;

network network_connect();

#endif