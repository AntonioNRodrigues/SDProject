#include <stdlib.h>
#include <string.h>
#include "client_stub-private.h"
#include "network_client-private.h"

struct rtable_t *rtable_bind(const char *address_port) {
	if (address_port == NULL) {
		return NULL;
	}
	struct rtable_t *remote_table = (struct rtable_t*) malloc(
			sizeof(struct rtable_t));
	if (remote_table == NULL) {
		return NULL;
	}
	remote_table->server = network_connect(address_port);
	if (remote_table->server == NULL) {
		free(remote_table);
		return NULL;
	}

	return remote_table;

}

int rtable_unbind(struct rtable_t *rtable) {
	if (rtable == NULL) {
		return -1;
	}
	int result = network_close(rtable->server);
	if (result == 0)
		printf("The connection has been closed without errors");

	if (result == -1)
		printf("The connection has been closed with errors");

	free(rtable);
	return result;
}

int rtable_put(struct rtable_t *rtable, char *key, struct data_t *value) {

	if (rtable == NULL || key == NULL || value == NULL) {
		return -1;
	}

	return 0;
}

int rable_update(struct rtable_t *rtable, char *key, struct data_t *value) {

}

struct data_t *rtable_get(struct rtable_t *table, char *key) {

}
int rtable_del(struct rtable_t *table, char *key) {

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
	print_msg(msg_out);
	free_message(msg_out);
	printf("Mensagem Recebida\n\n");
	if (msg_resposta == NULL) {
		printf("Nao houve resposta\n");
		ret = -1;
	} else {
		print_msg(msg_resposta);
		if (msg_resposta->opcode == OC_RT_ERROR) {
			printf("Houve um erro\n");
			ret = -1;
		} else {
			printf("Resultado: %d\n\n", msg_resposta->content.result);
			ret = 0;
		}
		free_message(msg_resposta);
	}
	return ret;
}

char **rtable_get_keys(struct rtable_t *rtable) {

}

void rtable_free_keys(char **keys) {

}
