/*
 * Grupo 33
 * Miguel Vale n.39279
 * António Rodrigues n.º40853
 * Ricardo Veloso n.º44842
 */

#ifndef _MESSAGE_PRIVATE_H
#define _MESSAGE_PRIVATE_H

#define _SHORT 2
#define _INT 4

#define OC_RT_ERROR 99
#define OC_UP 100
#define OC_DOWN 101
#define OC_STATUS 102

#include "table-private.h"
#include "message.h"

int valid(short opcode, short c_type);

/**
 * function to build a message to send the ip and port between severs
 */
void print_msg(struct message_t *msg);
/**
 * function to build a message to send the ip and port between severs
 */
struct message_t * build_ip_msg(struct message_t *msg_ip, char *ip_port) ;

/**
 * function to build a message to send the status between servers
 */
struct message_t * build_status_msg(struct message_t *msg_status);
/**
 * function to build a message to send the an msg_error
 */
struct message_t * build_error_msg(struct message_t *msg_error);

#endif
