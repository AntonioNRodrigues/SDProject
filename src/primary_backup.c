#ifndef _PRIMARY_BACKUP_H
#define _PRIMARY_BACKUP_H

#include "primary_backup-private.h"
#include "message.h"
#include "network_client-private.h"
#include "table_skel-private.h"
#include <stdio.h>
#include <error.h>

struct server;
/**
 * Funcão usada para um servidor avisar o "server" de que já acordou.
 * Retorna 0 em caso de sucesso, -1 em caso de insucesso
 */
int hello(struct server_t *server) {
	if (server == NULL) {
		printf("the message was not send, The server is down\n");
		return -1;
	}
	struct message_t *msg_out = (struct message_t *) malloc(
			sizeof(struct message_t));
	if (msg_out == NULL)
		return -1;
	msg_out->opcode = OC_UP;
	msg_out->c_type = CT_KEY;
	msg_out->content.key = "127.0.0.1:44445"; //<IP_PRIM:PORT_PRIM> // estamos no ambito do secundario logo nao temos acesso a isto
	struct message_t *tt = network_send_receive(server, msg_out);
	if (tt == NULL) {
		return -1;
	}
	printf("the message was send\n");
	network_close(server);
	return 0;
}
//int hello (struct server_t *server, char*ip_port){

//}

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
		printf("INSIDE UPDATE_STATE\n");
		char ** temp1 = t->content.keys;
		int i = 0;
		while (temp1[i] != NULL) {
			msg_out->content.key = strdup(temp1[i]);
			// get the data from all keys
			struct message_t *tt = network_send_receive(server, msg_out);
			msg_out_2->content.entry = entry_create(temp1[i], tt->content.data);
			//put it on its table
			invoke(msg_out_2);
			i++;
		}
		table_free_keys(temp1);
		free_message(msg_out_2);
		free_message(msg_out);
	}
	printf("the state was updated\n");
	return 0;
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
	msg_out->opcode = OC_UP;
	msg_out->c_type = CT_KEY;
	msg_out->content.key = strdup(ip_port);
	struct message_t *tt = network_send_receive(server, msg_out);
	if (tt == NULL) {
		return -1;
	}
	printf("the message was send\n");
	network_close(server);
	return 0;
}
int hello_special(struct server_t *server) {
	if (server == NULL) {
		printf("the message was not send, The server is down\n");
		return -1;
	}
	struct message_t *msg_out = (struct message_t *) malloc(
			sizeof(struct message_t));
	if (msg_out == NULL)
		return -1;
	msg_out->opcode = OC_STATUS;
	msg_out->c_type = CT_RESULT;
	msg_out->content.result = -100;
	struct message_t *tt = network_send_receive(server, msg_out);
	if (tt == NULL) {
		return -1;
	}
	printf("the message of OC_STATUS was send\n");
	network_close(server);
	return tt->content.result;
}

#endif
