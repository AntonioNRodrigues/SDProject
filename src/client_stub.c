/*
 * Grupo 33
 * Miguel Vale n.39279
 * António Rodrigues n.º40853
 * Ricardo Veloso n.º44842
 */


#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include "client_stub-private.h"
#include "network_client-private.h"

const char *address;

struct rtable_t *rtable_bind(const char *address_port) {
	if (address_port == NULL) {
		return NULL;
	}
	address = strdup(address_port);
	if (address == NULL) {
		return NULL;
	}

	struct rtable_t *remote_table = (struct rtable_t *) malloc(
			sizeof(struct rtable_t));
	if (remote_table == NULL) {
		return NULL;
	}
	remote_table->server = network_connect(address_port);

	if (remote_table == NULL) {
		free(remote_table);
		return NULL;
	}
	return remote_table;
}

/*struct rtable_t *rtable_rebind(struct rtable_t *remote_table) {
 if (remote_table == NULL) {
 return NULL;
 }
 if (remote_table->server == NULL) {
 return NULL;
 }
 remote_table->server = network_reconnect(remote_table->server);

 if (remote_table->server == NULL) {
 return NULL;
 }

 return remote_table;
 }*/

int rtable_unbind(struct rtable_t *rtable) {
	if (rtable == NULL) {
		return -1;
	}
	int result = network_close(rtable->server);
	if (result == 0)
		printf("The connection has been closed without errors\n");

	if (result == -1)
		printf("The connection has been closed with errors\n");

	free(rtable);
	return result;
}

int rtable_put(struct rtable_t *rtable, char *key, struct data_t *value) {

	if (rtable == NULL || key == NULL || value == NULL) {
		return -1;
	}
	struct message_t *msg_out = (struct message_t *) malloc(
			sizeof(struct message_t));
	msg_out->opcode = OC_PUT;
	msg_out->c_type = CT_ENTRY;
	msg_out->content.entry = entry_create(key, value);

	struct message_t * msg_resposta = network_send_receive(rtable->server,
			msg_out);

	//try sending the message one more time
	if (msg_resposta == NULL) {
		if (retry(rtable) != -1)
			msg_resposta = network_send_receive(rtable->server, msg_out);
		else
			printf("the server didnt response");
	}

	printf("----Message Sended------\n");
	print_msg(msg_out);
	free_message(msg_out);
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

int rtable_update(struct rtable_t *rtable, char *key, struct data_t *value) {
	if (rtable == NULL || key == NULL || value == NULL) {
		return -1;
	}

	struct message_t * msg_out = (struct message_t *) malloc(
			sizeof(struct message_t));
	msg_out->opcode = OC_UPDATE;
	msg_out->c_type = CT_ENTRY;
	msg_out->content.entry = entry_create(key, value);

	struct message_t * msg_resposta = network_send_receive(rtable->server,
			msg_out);

	//try sending the message one more time
	if (msg_resposta == NULL) {
		if (retry(rtable) != -1)
			msg_resposta = network_send_receive(rtable->server, msg_out);
		else
			printf("the server didnt response");
	}

	printf("----Message Sended------\n");
	print_msg(msg_out);
	free_message(msg_out);
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
	if (rtable == NULL || key == NULL) {
		return NULL;
	}
	struct message_t *msg_out = (struct message_t *) malloc(
			sizeof(struct message_t));
	msg_out->opcode = OC_GET;
	msg_out->c_type = CT_KEY;
	msg_out->content.key = strdup(key);

	struct message_t * msg_resposta = network_send_receive(rtable->server,
			msg_out);

	//try sending the message one more time
	if (msg_resposta == NULL) {
		if (retry(rtable) != -1)
			msg_resposta = network_send_receive(rtable->server, msg_out);
		else
			printf("the server didnt response");
	}

	printf("----Message Sended------\n");
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
			printf("Key: %s, Value: %s\n\n", key, msg_resposta->content.data->data);
		}
	}
	struct data_t * temp = msg_resposta->content.data->data;
	//free_message(msg_resposta);
	return temp;

}
int rtable_del(struct rtable_t *rtable, char *key) {
	if (rtable == NULL || key == NULL) {
		return -1;
	}
	int ret = -1;
	struct message_t *msg_out = (struct message_t *) malloc(
			sizeof(struct message_t));
	if (msg_out == NULL) {
		return -1;
	}
	msg_out->opcode = OC_DEL;
	msg_out->c_type = CT_KEY;
	msg_out->content.key = strdup(key);

	struct message_t *msg_resposta = network_send_receive(rtable->server,
			msg_out);

	//try sending the message one more time
	if (msg_resposta == NULL) {
		if (retry(rtable) != -1)
			msg_resposta = network_send_receive(rtable->server, msg_out);
		else
			printf("the server didnt response");
	}

	printf("----Message Sended------\n");
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
	if (rtable == NULL) {
		return -1;
	}
	int ret = 0;
	struct message_t *msg_out = (struct message_t *) malloc(
			sizeof(struct message_t));
	if (msg_out == NULL) {
		return -1;
	}
	msg_out->opcode = OC_SIZE;
	msg_out->c_type = CT_RESULT;
	msg_out->content.result = 0;

	struct message_t *msg_resposta = network_send_receive(rtable->server,
			msg_out);

	//try sending the message one more time
	if (msg_resposta == NULL) {
		if (retry(rtable) != -1)
			msg_resposta = network_send_receive(rtable->server, msg_out);
		else
			printf("the server didnt response");
	}
	printf("----Message Sended------\n");
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
	msg_out->opcode = OC_GET;
	msg_out->c_type = CT_KEY;
	msg_out->content.key = strdup("!");

	struct message_t * msg_resposta = network_send_receive(rtable->server,
			msg_out);

	//try sending the message one more time
	if (msg_resposta == NULL) {
		if (retry(rtable) != -1)
			msg_resposta = network_send_receive(rtable->server, msg_out);
		else
			printf("the server didnt response");
	}

	printf("----Message Sended------\n");
	print_msg(msg_out);
	free_message(msg_out);
	printf("----Message Received----\n");
	if (msg_resposta == NULL) {
		printf("There was no answer\n ");
		return NULL;
	} else {
		print_msg(msg_resposta);
		if (msg_resposta->opcode == OC_RT_ERROR) {
			printf("key does not exist or error\n");
			return NULL;
		}
		if (msg_resposta->c_type == CT_KEYS) {
			int i = 0;
			while (msg_resposta->content.keys[i] != NULL) {
				printf("key[%d]:%s, ", i,msg_resposta->content.keys[i]);
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

int retry(struct rtable_t *remote_table) {

	printf("The server failed to respond, trying again in %d ms\n",
	RETRY_TIME);
	//time out in miliseconds
	poll(0, 0, RETRY_TIME);
	remote_table = rtable_bind(address);
//	remote_table = rtable_rebind(remote_table);
	if (remote_table == NULL) {
		rtable_unbind(remote_table);
		return -1;
	}

	return 1;
}

