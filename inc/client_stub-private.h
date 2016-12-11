/*
 * Grupo 33
 * Miguel Vale n.39279
 * António Rodrigues n.º40853
 * Ricardo Veloso n.º44842
 */

#ifndef _CLIENT_STUB_PRIVATE_H
#define _CLIENT_STUB_PRIVATE_H

/*the time out to retry sending the message if the server fails to responds.*/
#define RETRY_TIME 5000
/*quantity of servers*/
#define N_SERVERS 2
#define SERVER_ONE 1
#define SERVER_TWO 2

#include "client_stub.h"

/* Remote table. A definir pelo grupo em client_stub-private.h 
 */
struct rtable_t {
	struct server_t *server_one;
	struct server_t *server_two;
	char *ip_port_server_one;
	char *ip_port_server_two;
	/*SERVER_ONE primary server is server_one*/
	/*SERVER_TWO primary server is server_two*/
	int current_server;
};

/*
 * function to simulate the pattern, in case of errors.
 * If the message to one server is null, try the other server
 * if that one does not respond, sleep 5000ms and try again
 * the same order
 */
struct message_t * retry_servers(struct rtable_t *rtable,
		struct message_t* msg_out);
/*
 * function to check which server is active and returned it
 */
struct server_t *current_server(struct rtable_t *rtable);

/**
 * function to switch between the two servers activating
 * one and closing the other.
 */
void switch_server(struct rtable_t *rtable);
/**
 *
 */
void complete_remote_table(struct rtable_t *remote_table, char **argv_copy);

#endif
