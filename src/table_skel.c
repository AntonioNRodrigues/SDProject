#include <error.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "table.h"
#include "table-private.h"
#include "message.h"
#include "message-private.h"
#include "network_client-private.h"

#include "table_skel-private.h"

struct table_t *tabela;

int table_skel_init(int n_lists) {
	if (n_lists <= 0) {
		return -1;
	}
	tabela = table_create(n_lists);
	if (tabela == NULL) {
		return -1;
	}
	return 0;
}
int table_skel_destroy() {
	table_destroy(tabela);

	return tabela == NULL ? 0 : -1;
}

/**
 * function to build the error message returned by the sever
 */
/*struct message_t * build_error_msg(struct message_t *msg_error) {
	msg_error->c_type = CT_RESULT;
	msg_error->opcode = OC_RT_ERROR;
	msg_error->content.result = -1;
	return msg_error;
}*/

struct message_t *invoke(struct message_t *msg_in) {
	char *temp_key;
	struct message_t *msg_resposta = (struct message_t *) malloc(
			sizeof(struct message_t));
	if (msg_resposta == NULL) {
		return NULL;
	}
	int result;

	/* Verificar parâmetros de entrada */
	if (msg_in == NULL || tabela == NULL) {
		free_message(msg_resposta);
		return NULL;
	}
	/* Verificar opcode e c_type na mensagem de pedido */
	if (msg_in->opcode < 0 || msg_in->c_type < 0) {
		free_message(msg_resposta);
		return NULL;
	}
	/* Aplicar operação na tabela */

	/* Preparar mensagem de resposta */
	switch (msg_in->opcode) {
	case OC_SIZE:
		result = table_size(tabela);
		//the table is empty
		if (result < 0) {
			msg_resposta = build_error_msg(msg_resposta);
		} else {
			msg_resposta->c_type = CT_RESULT;
			msg_resposta->opcode = OC_SIZE + 1;
			msg_resposta->content.result = result;
		}
		break;
	case OC_PUT:
		result = table_put(tabela, msg_in->content.entry->key,
				msg_in->content.entry->value);
		//table_put failed
		if (result == -1) {
			msg_resposta = build_error_msg(msg_resposta);

		} else {
			msg_resposta->c_type = CT_RESULT;
			msg_resposta->opcode = OC_PUT + 1;
			msg_resposta->content.result = result;
		}
		break;
	case OC_GET:

		temp_key = strdup(msg_in->content.key);

		// temp_key is NULL
		if (temp_key == NULL) {
			msg_resposta = build_error_msg(msg_resposta);
		}
		//key is ! --> GET ALL KEYS
		if (strcmp("!", temp_key) == 0) {
			if (tabela->quantity_entry == 0) {
				msg_resposta = build_error_msg(msg_resposta);
			} else {
				msg_resposta->c_type = CT_KEYS;
				msg_resposta->opcode = OC_GET + 1;
				msg_resposta->content.keys = table_get_keys(tabela);
			}
		} else {
			struct data_t *temp_data = table_get(tabela, temp_key);
			//the key is present
			if (temp_data != NULL) {
				msg_resposta->c_type = CT_VALUE;
				msg_resposta->opcode = OC_GET + 1;
				msg_resposta->content.data = data_dup(temp_data);
				data_destroy(temp_data);
				//key does not exist
			} else {
				msg_resposta = build_error_msg(msg_resposta);
			}
		}
		break;
	case OC_UPDATE:
		result = table_update(tabela, msg_in->content.entry->key,
				msg_in->content.entry->value);
		//table_update failed
		if (result == -1) {
			msg_resposta = build_error_msg(msg_resposta);
		} else {
			msg_resposta->c_type = CT_RESULT;
			msg_resposta->opcode = OC_UPDATE + 1;
			msg_resposta->content.result = result;
		}
		break;

	case OC_DEL:
		result = table_del(tabela, msg_in->content.key);
		//table_del failed
		if (result == -1) {
			msg_resposta->c_type = CT_RESULT;
			msg_resposta->opcode = OC_RT_ERROR;
			msg_resposta->content.result = result;
		} else {
			msg_resposta->opcode = OC_DEL + 1;
			msg_resposta->c_type = CT_RESULT;
			msg_resposta->content.result = result;
		}
		break;
	default:
		break;
	}
	return msg_resposta;
}
