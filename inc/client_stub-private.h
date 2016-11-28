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
	/*SERVER_ONE primary server is server_one*/
	/*SERVER_TWO primary server is server_two*/
	int current_server;
};

/**
 * function to define a retry after a RETRY_TIME of miliseconds
 */
int retry(struct rtable_t *remote_table);
/*
 * function to reestablish the connection
 */
struct rtable_t *rtable_rebind(struct rtable_t *remote_table);

/**
 * function to prepare the backup server. Fill all the
 * struct server_t server_two
 */
int prepare_backup_server(struct rtable_t * remote_table, const char *address_port);

#endif
