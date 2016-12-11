/*
 * Grupo 33
 * Miguel Vale n.39279
 * António Rodrigues n.º40853
 * Ricardo Veloso n.º44842
 */

#ifndef _PRIMARY_BACKUP_PRIVATE_H
#define _PRIMARY_BACKUP_PRIVATE_H

#include "inet.h"
#include "network_client.h"

int ask_status(struct server_t *server);
int send_connect_ip(struct server_t *server, char *ip_port);

#endif

