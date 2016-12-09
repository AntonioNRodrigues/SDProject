#ifndef _PRIMARY_BACKUP_PRIVATE_H
#define _PRIMARY_BACKUP_PRIVATE_H

#include "inet.h"
#include "network_client.h"

int ask_status(struct server_t *server);
int hello_again(struct server_t *server, char *ip_port);

#endif

