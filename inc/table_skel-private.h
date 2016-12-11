/*
 * Grupo 33
 * Miguel Vale n.39279
 * António Rodrigues n.º40853
 * Ricardo Veloso n.º44842
 */

#ifndef _TABLE_SKEL_PRIVATE_H
#define _TABLE_SKEL_PRTIVATE_H

#define MAX_SOCKETS 10
#define STDIN_POS 1
#define TIMEOUT -1
#define LISTENING_SOCKET_POS 0
#define N_POS_NOT_FREE 2

#define PORT_LEN 6
#define PORT_PRIM 44444
#define UP 1
#define DOWN 0
#define NONE -1
#define PRIMARY 1
#define SECUNDARY 2

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
