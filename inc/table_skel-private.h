#ifndef _TABLE_SKEL_PRIVATE_H
#define _TABLE_SKEL_PRTIVATE_H

#define MAX_SOCKETS 5
#define TIMEOUT -1
#define LISTENING_SOCKET_POS 0

#include "table-private.h"
#include "table_skel.h"

struct message_t *build_error_msg(struct message_t *msg_error);

#endif
