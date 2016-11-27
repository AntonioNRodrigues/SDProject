/*
 * Grupo 33
 * Miguel Vale n.39279
 * António Rodrigues n.º40853
 * Ricardo Veloso n.º44842
 */

#ifndef _NETWORK_CLIENT_PRIVATE_H
#define _NETWORK_CLIENT_PRIVATE_H

#include "inet.h"
#include "network_client.h"

struct server_t {
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

/**
 * function that reconnects to the server. Uses the information presented in the
 * param server to fill a new_server an closes the server in the param.
 */
struct server_t *network_reconnect(struct server_t* server);
/**
 * function that prepares a sever_t with the propor IP and Port to be connected
 * if the primary is down
 */
struct server_t *network_prepare(const char *address_port);

/**
 * function to establish the connection with the server
 */
struct server_t *net_connect(struct server_t *server);

#endif
