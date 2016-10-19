#ifndef _NETWORK_CLIENT_PRIVATE_H
#define _NETWORK_CLIENT_PRIVATE_H

#include "inet.h"
#include "network_client.h"

struct server_t{
	int sock_file_descriptor;
	struct sockaddr_in server;
};

/* Função que garante o envio de len bytes armazenados em buf,
   através da socket sock.
*/
int write_all(int sock, char *buf, int len);

/* Função que garante a receção de len bytes através da socket sock,
   armazenando-os em buf.
*/
int read_all(int sock, char *buf, int len);

#endif
