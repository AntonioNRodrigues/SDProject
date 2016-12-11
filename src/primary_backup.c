#ifndef _PRIMARY_BACKUP_H
#define _PRIMARY_BACKUP_H

#include "primary_backup-private.h"
#include "message-private.h"
#include "network_client-private.h"
#include "table_skel-private.h"
#include <stdio.h>
#include <error.h>

/**
 * Funcão usada para um servidor avisar o "server" de que já acordou.
 * Retorna 0 em caso de sucesso, -1 em caso de insucesso
 */
int hello(struct server_t *server) {
	//function not used;
	return 0;
}

/**
 * Pede atualização de estado ao server.
 * Retorna 0 em caso de sucesso e -1 em caso de insucesso*/

int update_state(struct server_t *server) {
	if (server != NULL) {
		struct message_t *msg_out = (struct message_t *) malloc(
				sizeof(struct message_t));
		if (msg_out == NULL)
			return -1;
		//built a msg to get all keys from the server
		msg_out->opcode = OC_GET;
		msg_out->c_type = CT_KEY;
		msg_out->content.key = strdup("!");

		struct message_t * msg_out_2 = (struct message_t *) malloc(
				sizeof(struct message_t));
		if (msg_out_2 == NULL) {
			free_message(msg_out);
			return -1;
		}

		//build a msg to put the entry in the table of the secundary
		msg_out_2->opcode = OC_PUT;
		msg_out_2->c_type = CT_ENTRY;

		struct message_t * t = network_send_receive(server, msg_out);
		//is the table is empty the message sent is with content.result wit zero
		if (t->content.result == 0) {
			printf("the state was updated table has no elements\n");
		} else {
			char ** temp1 = t->content.keys;
			int i = 0;
			while (temp1[i] != NULL) {
				msg_out->content.key = strdup(temp1[i]);
				// get the data from all keys
				struct message_t *tt = network_send_receive(server, msg_out);
				msg_out_2->content.entry = entry_create(temp1[i],
						tt->content.data);
				//put it on its table
				invoke(msg_out_2);
				i++;
			}
			table_free_keys(temp1);
		}

		free_message(msg_out_2);
		free_message(msg_out);

		printf("the state was updated\n");
		return 0;
	}
	printf("the state was not updated the server was null\n");
	return -1;
}

int hello_again(struct server_t *server, char *ip_port) {
	if (server == NULL) {
		printf("the message was not send, The server is down\n");
		return -1;
	}
	struct message_t *msg_out = (struct message_t *) malloc(
			sizeof(struct message_t));
	if (msg_out == NULL)
		return -1;
	msg_out = build_ip_msg(msg_out, ip_port);
	struct message_t *tt = network_send_receive(server, msg_out);
	if (tt == NULL) {
		return -1;
	}
	printf("the message was send\n");
	re_use_socket(server);
	network_close(server);
	free_message(tt);
	free_message(msg_out);
	return 0;
}
int ask_status(struct server_t *server) {
	if (server == NULL) {
		printf("the message was not send, The server is down\n");
		return -1;
	}
	struct message_t *msg_out = (struct message_t *) malloc(
			sizeof(struct message_t));
	if (msg_out == NULL)
		return -1;
	msg_out = build_status_msg(msg_out);

	struct message_t *tt = network_send_receive(server, msg_out);
	if (tt == NULL) {
		return -1;
	}
	printf("the message with OC_STATUS was send\n");
	int result = tt->content.result;
	free_message(tt);
	free_message(msg_out);
	return result;
}

void re_use_socket(struct server_t *server) {
	//make the socket reusable
	int reuse_address = 1, reuse_port = 1;
	//REUSEADDR
	if (setsockopt(server->sock_file_descriptor, SOL_SOCKET, SO_REUSEADDR,
			(int *) &reuse_address, sizeof(reuse_address)) < 0) {
		perror("Error reusing the socket :: REUSEADDR");
	}
	//REUSEPORT
	if (setsockopt(server->sock_file_descriptor, SOL_SOCKET, SO_REUSEPORT,
			(int *) &reuse_port, sizeof(reuse_port)) < 0) {
		perror("Error reusing the socket :: REUSEPORT");
	}
}
#endif
