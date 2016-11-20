/*
 * Grupo 33
 * Miguel Vale n.39279
 * António Rodrigues n.º40853
 * Ricardo Veloso n.º44842
 */

#ifndef _CLIENT_STUB_PRIVATE_H
#define _CLIENT_STUB_PRIVATE_H

//the time out to retry sending the message if the server fails to responds.
#define RETRY_TIME 5000

#include "client_stub.h"

/* Remote table. A definir pelo grupo em client_stub-private.h 
 */
struct rtable_t {
	struct server_t *server;
};
/**
 * function to define a retry after a RETRY_TIME of miliseconds
 */
int retry(struct rtable_t *remote_table);
/*
 * function to reestablish the connection
 */
struct rtable_t *rtable_rebind(struct rtable_t *remote_table);

#endif
