#ifndef _CLIENT_STUB_PRIVATE_H
#define _CLIENT_STUB_PRIVATE_H

//the time out to retry sending the message if the server fails to responds.
#define TIMEOUT_RETRY 5000

#include "client_stub.h"

/* Remote table. A definir pelo grupo em client_stub-private.h 
 */
struct rtable_t {
	struct server_t *server;
};
/**
 * function to define a retry after a TIMEOUT_RETRY of miliseconds
 */
void retry();

#endif
