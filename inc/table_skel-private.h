/*
 * Grupo 33
 * Miguel Vale n.39279
 * António Rodrigues n.º40853
 * Ricardo Veloso n.º44842
 */

#ifndef _TABLE_SKEL_PRIVATE_H
#define _TABLE_SKEL_PRTIVATE_H

#define MAX_SOCKETS 5
#define STDIN_POS 1
#define TIMEOUT -1
#define LISTENING_SOCKET_POS 0
#define N_POS_NOT_FREE 2

#include "table-private.h"
#include "table_skel.h"

/**
 * function to build the error message returned by the sever
 */
struct message_t *build_error_msg(struct message_t *msg_error);

/**
 * function to print the status (the keys and it's value) of the table.
 */
void print_status();

#endif
