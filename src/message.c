/*
 * Grupo 33
 * Miguel Vale n.39279
 * António Rodrigues n.º40853
 * Ricardo Veloso n.º44842
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "message-private.h"

struct message_t * build_ip_msg(struct message_t *msg_ip, char *ip_port) {
	msg_ip->opcode = OC_UP;
	msg_ip->c_type = CT_KEY;
	msg_ip->content.key = strdup(ip_port);
	return msg_ip;
}

struct message_t * build_status_msg(struct message_t *msg_status) {
	msg_status->opcode = OC_STATUS;
	msg_status->c_type = CT_RESULT;
	msg_status->content.result = -100;
	return msg_status;
}

struct message_t * build_error_msg(struct message_t *msg_error) {
	msg_error->c_type = CT_RESULT;
	msg_error->opcode = OC_RT_ERROR;
	msg_error->content.result = -1;
	return msg_error;
}

void free_message(struct message_t *msg) {

	if (msg != NULL) {
		if (msg->c_type == CT_VALUE)
			data_destroy(msg->content.data);

		if (msg->c_type == CT_ENTRY)
			entry_destroy(msg->content.entry);

		if (msg->c_type == CT_KEYS)
			table_free_keys(msg->content.keys);

		if (msg->c_type == CT_KEY)
			free(msg->content.key);
		free(msg);
	}
}

int message_to_buffer(struct message_t *msg, char **msg_buf) {
	/* Verificar se msg é NULL */
	if (msg != NULL) {
		/* Consoante o msg->c_type, determinar o tamanho do vetor de bytes
		 que tem de ser alocado antes de serializar msg
		 */
		int buffer_size = 0, nkeys = 0, i = 0;

		if (msg->c_type == CT_RESULT) {
			buffer_size += (_SHORT * 2) + _INT;

		} else if (msg->c_type == CT_VALUE) {
			buffer_size += (_SHORT * 2 + _INT + msg->content.data->datasize);
		} else if (msg->c_type == CT_KEYS) {

			i = 0;
			//temp_keys = msg->content.keys;
			while (msg->content.keys[i] != NULL) {
				buffer_size += (strlen(msg->content.keys[i]));
				i++;
			}
			nkeys = i;

			buffer_size += (_SHORT * 2) + (_SHORT * nkeys) + _INT;

		} else if (msg->c_type == CT_KEY) {
			buffer_size += _SHORT * 3 + (strlen(msg->content.key));

		} else if (msg->c_type == CT_ENTRY) {
			buffer_size += (_SHORT * 3 + strlen(msg->content.entry->key) + _INT
					+ msg->content.entry->value->datasize);
		}

		/* Alocar quantidade de memória determinada antes
		 *msg_buf = ....
		 */
		*msg_buf = (char *) malloc(buffer_size);
		/* Inicializar ponteiro auxiliar com o endereço da memória alocada */

		char *ptr;

		ptr = *msg_buf;

		uint16_t short_value;
		uint32_t long_value;

		short_value = htons(msg->opcode);
		memcpy(ptr, &short_value, _SHORT);
		ptr += _SHORT;

		short_value = htons(msg->c_type);
		memcpy(ptr, &short_value, _SHORT);
		ptr += _SHORT;

		/* Consoante o conteúdo da mensagem, continuar a serialização da mesma */

		switch (msg->c_type) {
		case CT_ENTRY:

			short_value = htons(strlen(msg->content.entry->key));
			memcpy(ptr, &short_value, _SHORT);
			ptr += _SHORT;

			int size = strlen(msg->content.entry->key);
			memcpy(ptr, msg->content.entry->key, size);
			ptr += size;

			long_value = htonl(msg->content.entry->value->datasize);
			memcpy(ptr, &long_value, _INT);
			ptr += _INT;

			int data_size = msg->content.entry->value->datasize;
			memcpy(ptr, msg->content.entry->value->data, data_size);

			break;
		case CT_KEY:
			short_value = htons(strlen(msg->content.key));
			memcpy(ptr, &short_value, _SHORT);
			ptr += _SHORT;
			memcpy(ptr, msg->content.key, strlen(msg->content.key));
			break;
		case CT_KEYS:
			long_value = htonl(nkeys);

			memcpy(ptr, &long_value, _INT);
			ptr += _INT;

			i = 0;
			while (msg->content.keys[i] != NULL) {
				size = strlen(msg->content.keys[i]);
				short_value = htons(size);
				memcpy(ptr, &short_value, _SHORT);
				ptr += _SHORT;

				memcpy(ptr, msg->content.keys[i], size);
				ptr += size;
				i++;
			}
			break;
		case CT_VALUE:
			long_value = htonl(msg->content.data->datasize);
			memcpy(ptr, &long_value, _INT);
			ptr += _INT;
			memcpy(ptr, msg->content.data->data, msg->content.data->datasize);
			break;

		case CT_RESULT:
			long_value = htonl(msg->content.result);
			memcpy(ptr, &long_value, _INT);
			ptr += _INT;
			break;
		default:
			break;
		}
		return buffer_size;
	}
	return -1;
}

struct message_t *buffer_to_message(char *msg_buf, int msg_size) {
	if (msg_buf == NULL || msg_size < 0)
		return NULL;

	int short_aux = 0, int_aux = 0;

	/* Alocar memória para uma struct message_t */
	struct message_t *msg = (struct message_t *) malloc(
			sizeof(struct message_t));

	if (msg == NULL)
		return NULL;

	/* Recuperar o opcode e c_type */
	memcpy(&short_aux, msg_buf, _SHORT);
	msg->opcode = ntohs(short_aux);
	msg_buf += _SHORT;

	memcpy(&short_aux, msg_buf, _SHORT);
	msg->c_type = ntohs(short_aux);
	msg_buf += _SHORT;

	/* O opcode e c_type são válidos? */
	if ((valid(msg->opcode, msg->c_type)) != 0) {
		free_message(msg);
		return NULL;
	}
	/* Consoante o c_type, continuar a recuperação da mensagem original */
	switch (msg->c_type) {
	case CT_RESULT:
		memcpy(&int_aux, msg_buf, _INT);
		msg->content.result = ntohl(int_aux);
		break;
	case CT_KEY:

		//KEYSIZE
		memcpy(&short_aux, msg_buf, _SHORT);
		int size_key = ntohs(short_aux);
		msg_buf += _SHORT;
		//KEY
		msg->content.key = (char *) malloc(size_key + 1);
		memcpy(msg->content.key, msg_buf, size_key);
		msg->content.key[size_key] = '\0';
		break;
	case CT_VALUE:
		//DATASIZE
		memcpy(&int_aux, msg_buf, _INT);
		int data_size = ntohl(int_aux);
		msg_buf += _INT;

		//DATA
		msg->content.data = data_create(data_size);
		if (msg->content.data == NULL) {
			free(msg);
			return NULL;
		}
		memcpy(msg->content.data->data, msg_buf, data_size);
		break;
	case CT_ENTRY:
		//KEYSIZE
		memcpy(&short_aux, msg_buf, _SHORT);
		int key_size = ntohs(short_aux);
		msg_buf += _SHORT;

		//KEY
		char *aux_key = (char *) malloc(key_size + 1);
		memcpy(aux_key, msg_buf, key_size);
		aux_key[key_size] = '\0';
		msg_buf += key_size;

		//DATASIZE
		memcpy(&int_aux, msg_buf, _INT);
		data_size = ntohl(int_aux);
		msg_buf += _INT;
		struct data_t * temp_data = data_create(data_size);
		if (temp_data == NULL) {
			free(aux_key);
			free_message(msg);
			return NULL;
		}
		//ENTRY

		msg->content.entry = entry_create(aux_key, temp_data);
		if (msg->content.entry == NULL) {
			free(aux_key);
			data_destroy(temp_data);
			free_message(msg);
			return NULL;
		}
		memcpy(msg->content.entry->value->data, msg_buf, data_size);
		free(aux_key);
		data_destroy(temp_data);
		break;

	case CT_KEYS:
		memcpy(&int_aux, msg_buf, _INT);
		int nkeys = ntohl(int_aux);
		msg_buf += _INT;
		int i = 0;

		msg->content.keys = (char **) malloc((nkeys + 1) * sizeof(char *));
		while (i < nkeys) {
			//KEYSIZE
			memcpy(&short_aux, msg_buf, _SHORT);
			size_key = ntohs(short_aux);
			msg_buf += _SHORT;
			//KEY
			msg->content.keys[i] = (char *) malloc((size_key + 1));
			memcpy(msg->content.keys[i], msg_buf, size_key);
			msg->content.keys[i][size_key] = '\0';
			msg_buf += size_key;
			i++;
		}
		msg->content.keys[i] = NULL;
		break;

	default:
		break;
	}
	return msg;
}
/**
 * valid opcode && c_type returns 0, invalid returns -1
 */

int valid(short opcode, short c_type) {
	return ((opcode = OC_STATUS || opcode == OC_UP || opcode == OC_DOWN
			|| opcode == OC_DEL || opcode == OC_SIZE || opcode == OC_PUT
			|| opcode == OC_UPDATE || opcode == OC_GET || opcode == OC_RT_ERROR
			|| opcode == OC_DEL + 1 || opcode == OC_SIZE + 1
			|| opcode == OC_PUT + 1 || opcode == OC_UPDATE + 1
			|| opcode == OC_GET + 1 || opcode == OC_DEL + 1)
			&& (c_type == CT_ENTRY || c_type == CT_KEY || c_type == CT_KEYS
					|| c_type == CT_RESULT || c_type == CT_VALUE)) ? 0 : -1;

}
/*
 * Prints the message in a readable format.
 */
void print_msg(struct message_t *msg) {
	int i;
	if (valid(msg->opcode, msg->c_type) == 0) {
		printf("====== MESSAGE =========\n");
		printf("opcode: %d, c_type: %d\n", msg->opcode, msg->c_type);
		switch (msg->c_type) {
		case CT_ENTRY: {
			printf("key: %s\n", msg->content.entry->key);
			printf("datasize: %d\n", msg->content.entry->value->datasize);
		}
			break;
		case CT_KEY: {
			printf("key: %s\n", msg->content.key);
		}
			break;
		case CT_KEYS: {
			for (i = 0; msg->content.keys[i] != NULL; i++) {
				printf("key[%d]: %s\n", i, msg->content.keys[i]);
			}
		}
			break;
		case CT_VALUE: {
			printf("datasize: %d\n", msg->content.data->datasize);
		}
			break;
		case CT_RESULT: {
			printf("result: %d\n", msg->content.result);
		}
			;
		}
		printf("========================\n\n");
	}

}

