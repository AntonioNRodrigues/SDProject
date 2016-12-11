/*
 < * Grupo 33
 * Miguel Vale n.39279
 * António Rodrigues n.º40853
 * Ricardo Veloso n.º44842
 */

#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include "client_stub-private.h"
#include "network_client-private.h"

const char *address; // TO BE DELETED

struct rtable_t *rtable_bind(const char *address_port) {
	if (address_port == NULL)
		return NULL;
	address = strdup(address_port);
	if (address == NULL)
		return NULL;

	struct rtable_t *remote_table = (struct rtable_t *) malloc(
			sizeof(struct rtable_t));
	if (remote_table == NULL)
		return NULL;

	remote_table->server_one = network_connect(address_port);

	if (remote_table == NULL) {
		free(remote_table);
		return NULL;
	}
	//set the current server as the server_one
	remote_table->current_server = SERVER_ONE;
	return remote_table;
}

/*tTO BE DELETED*/
struct rtable_t *rtable_rebind(struct rtable_t *remote_table) {
	if (remote_table == NULL)
		return NULL;

	// if server inside remote_table is down try to connect
	if (remote_table->server_one == NULL) {
		remote_table->server_one = network_connect(address);
		if (remote_table->server_one == NULL) {
			return NULL;
		}
		return remote_table;
	} else {
		//use the server inside the remote_table to reconnect
		remote_table->server_one = network_reconnect(remote_table->server_one);
	}
	// it was not possible to reconnect
	if (remote_table->server_one == NULL) {
		return NULL;
	}

	return remote_table;
}

/*TO BE DELETED ------->*/
int retry(struct rtable_t *remote_table) {

	printf("The server failed to respond, trying again in %d ms\n",
	RETRY_TIME);
	/*time out in miliseconds*/
	poll(0, 0, RETRY_TIME);
	//remote_table = rtable_rebind(remote_table);
	if (remote_table == NULL) {
		rtable_unbind(remote_table);
		return -1;
	}

	return 1;
}

int rtable_unbind(struct rtable_t *rtable) {
	if (rtable == NULL)
		return -1;
	int result = network_close(rtable->server_one);
	if (result == 0)
		printf("The connection has been closed without errors\n");

	if (result == -1)
		printf("The connection has been closed with errors\n");

	free(rtable);
	return result;
}

int prepare_backup_server(struct rtable_t * rtable, const char *address_port) {
	if (rtable == NULL || address_port == NULL) {
		return -1;
	}
	rtable->server_two = network_prepare(address_port);
	if (rtable->server_two == NULL) {
		return -1;
	}
	return 0;
}

struct server_t *current_server(struct rtable_t *rtable) {
	if (rtable == NULL)
		return NULL;
	return (rtable->current_server == SERVER_ONE) ?
			rtable->server_one : rtable->server_two;
}

void switch_server(struct rtable_t *rtable) {
	if (rtable != NULL) {
		if (rtable->current_server == SERVER_ONE) {
			//close(current_server(rtable)->sock_file_descriptor);
			rtable->current_server = SERVER_TWO;
			rtable->server_two = net_connect(rtable->server_two);
		} else {
			//close(current_server(rtable)->sock_file_descriptor);
			rtable->current_server = SERVER_ONE;
			rtable->server_one = net_connect(rtable->server_one);
		}
	}
}

/*@see client_stub-private.h*/
struct message_t * retry_servers(struct rtable_t *rtable,
		struct message_t* msg_out) {
	struct message_t *msg_resposta = network_send_receive(
			current_server(rtable), msg_out);
	if (msg_resposta == NULL) {
		switch_server(rtable);
		//send message to next server
		msg_resposta = network_send_receive(current_server(rtable), msg_out);
		//try sending the message one more time
		if (msg_resposta == NULL) {
			poll(0, 0, RETRY_TIME);
			switch_server(rtable);
			msg_resposta = network_send_receive(current_server(rtable),
					msg_out);
			if (msg_resposta == NULL) {
				switch_server(rtable);
				msg_resposta = network_send_receive(current_server(rtable),
						msg_out);
			}
		}
	}
	return msg_resposta;
}
int rtable_put(struct rtable_t *rtable, char *key, struct data_t *value) {
	if (rtable == NULL || key == NULL || value == NULL)
		return -1;

	struct message_t *msg_out = (struct message_t *) malloc(
			sizeof(struct message_t));
	if (msg_out == NULL)
		return -1;

	msg_out->opcode = OC_PUT;
	msg_out->c_type = CT_ENTRY;
	msg_out->content.entry = entry_create(key, value);

	/*send message to current_server*/
	struct message_t * msg_resposta = retry_servers(rtable, msg_out);

	printf("----Message Sent--------\n");
	print_msg(msg_out);
	printf("----Message Received----\n");
	if (msg_resposta == NULL) {
		printf("There was no answer\n ");
		return -1;
	} else {
		print_msg(msg_resposta);
		if (msg_resposta->opcode == OC_RT_ERROR) {
			printf("Error\n");
			//trying again in 5000 ms
			printf("The system is going to retry in %d ms\n\n", RETRY_TIME);
			poll(0, 0, RETRY_TIME);
			msg_resposta = network_send_receive(rtable->server_one, msg_out);

			if (msg_resposta->opcode == OC_RT_ERROR) {
				printf("Error\n");
			}
		}
		if ((msg_resposta->c_type == CT_RESULT)
				&& (msg_resposta->content.result == -10)) {
			printf("Key Already in the table \n");

		} else {
			printf("Result: %d\n\n", msg_resposta->content.result);
		}

		free_message(msg_resposta);
	}
	free_message(msg_out);

	return 0;
}

int rtable_update(struct rtable_t *rtable, char *key, struct data_t *value) {
	if (rtable == NULL || key == NULL || value == NULL)
		return -1;

	struct message_t * msg_out = (struct message_t *) malloc(
			sizeof(struct message_t));
	if (msg_out == NULL)
		return -1;

	msg_out->opcode = OC_UPDATE;
	msg_out->c_type = CT_ENTRY;
	msg_out->content.entry = entry_create(key, value);

	/*send message to current_server*/
	struct message_t * msg_resposta = retry_servers(rtable, msg_out);

	printf("----Message Sent--------\n");
	print_msg(msg_out);
	printf("----Message Received----\n");
	if (msg_resposta == NULL) {
		printf("There was no answer\n ");
	} else {
		print_msg(msg_resposta);
		if (msg_resposta->opcode == OC_RT_ERROR) {
			printf("key does not exist or error\n");
		} else {
			printf("Result: %d\n\n", msg_resposta->content.result);
		}
		free_message(msg_resposta);
	}
	return 0;
}

struct data_t *rtable_get(struct rtable_t *rtable, char *key) {
	if (rtable == NULL || key == NULL)
		return NULL;
	struct message_t *msg_out = (struct message_t *) malloc(
			sizeof(struct message_t));
	if (msg_out == NULL)
		return NULL;
	msg_out->opcode = OC_GET;
	msg_out->c_type = CT_KEY;
	msg_out->content.key = strdup(key);

	/*send message to current_server*/
	struct message_t * msg_resposta = retry_servers(rtable, msg_out);

	printf("----Message Sent--------\n");
	print_msg(msg_out);
	free_message(msg_out);
	printf("----Message Received----\n");
	if (msg_resposta == NULL) {
		printf("There was no answer\n ");
		free_message(msg_resposta);
		return NULL;
	} else {
		print_msg(msg_resposta);
		if (msg_resposta->opcode == OC_RT_ERROR) {
			printf("key does not exist or error\n");
			free_message(msg_resposta);
			return NULL;
		}
		if (msg_resposta->c_type == CT_VALUE) {
			printf("Key: %s, Value: %s\n\n", key,
					msg_resposta->content.data->data);
		}
	}
	struct data_t * temp = msg_resposta->content.data->data;
//free_message(msg_resposta);
	return temp;

}
int rtable_del(struct rtable_t *rtable, char *key) {
	if (rtable == NULL || key == NULL)
		return -1;
	int ret = -1;
	struct message_t *msg_out = (struct message_t *) malloc(
			sizeof(struct message_t));
	if (msg_out == NULL)
		return -1;

	msg_out->opcode = OC_DEL;
	msg_out->c_type = CT_KEY;
	msg_out->content.key = strdup(key);

	/*send message to current_server*/
	struct message_t * msg_resposta = retry_servers(rtable, msg_out);

	printf("----Message Sent--------\n");
	print_msg(msg_out);
	free_message(msg_out);
	printf("----Message Received----\n");
	if (msg_resposta == NULL) {
		printf("There was no answer\n ");
		ret = -1;
	} else {
		print_msg(msg_resposta);
		if (msg_resposta->opcode == OC_RT_ERROR) {
			printf("key does not exist or error\n");
			ret = -1;
		} else {
			printf("Result: %d\n\n", msg_resposta->content.result);
			ret = 0;
		}
		free_message(msg_resposta);

	}
	return ret;
}
int rtable_size(struct rtable_t *rtable) {
	if (rtable == NULL)
		return -1;
	int ret = 0;
	struct message_t *msg_out = (struct message_t *) malloc(
			sizeof(struct message_t));
	if (msg_out == NULL)
		return -1;

	msg_out->opcode = OC_SIZE;
	msg_out->c_type = CT_RESULT;
	msg_out->content.result = 0;

	/*send message to current_server*/
	struct message_t * msg_resposta = retry_servers(rtable, msg_out);

	printf("----Message Sent--------\n");
	print_msg(msg_out);
	free_message(msg_out);
	printf("----Message Received----\n");
	if (msg_resposta == NULL) {
		printf("There was no answer\n ");
		ret = -1;
	} else {
		print_msg(msg_resposta);
		if (msg_resposta->opcode == OC_RT_ERROR) {
			printf("Error\n");
			ret = -1;
		} else {
			printf("Result: %d\n\n", msg_resposta->content.result);
			ret = 0;
		}
		free_message(msg_resposta);
	}
	return ret;
}

char **rtable_get_keys(struct rtable_t *rtable) {
	if (rtable == NULL)
		return NULL;
	struct message_t *msg_out = (struct message_t *) malloc(
			sizeof(struct message_t));
	if (msg_out == NULL)
		return NULL;

	msg_out->opcode = OC_GET;
	msg_out->c_type = CT_KEY;
	msg_out->content.key = strdup("!");

	/*send message to current_server*/
	struct message_t * msg_resposta = retry_servers(rtable, msg_out);

	printf("----Message Sent--------\n");
	print_msg(msg_out);
	free_message(msg_out);
	printf("----Message Received----\n");
	if (msg_resposta == NULL) {
		printf("There was no answer\n ");
		return NULL;
	} else {
		print_msg(msg_resposta);
		if (msg_resposta->content.result == 0) {
			printf("Table is empty\n");
			return NULL;
		}
		if (msg_resposta->c_type == CT_KEYS) {
			int i = 0;
			while (msg_resposta->content.keys[i] != NULL) {
				printf("key[%d]:%s ", i, msg_resposta->content.keys[i]);
				i++;
			}
			printf("\n");
		}
	}
	return msg_resposta->content.keys;

}

void rtable_free_keys(char **keys) {
	if (keys != NULL) {
		table_free_keys(keys);
	}
}
