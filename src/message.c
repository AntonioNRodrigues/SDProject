#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "message-private.h"

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
	printf("BUFFER_TO_MESSAGE\n");
	/* Verificar se msg é NULL */
	if (msg != NULL) {

		/* Consoante o msg->c_type, determinar o tamanho do vetor de bytes
		 que tem de ser alocado antes de serializar msg
		 */
		int buffer_size;
		int nkeys;
		int i;
		char **temp_keys = msg->content.keys;

		if (msg->c_type == CT_RESULT) {
			buffer_size += _SHORT * 2 + _INT;
		} else if (msg->c_type == CT_VALUE) {
			buffer_size += (_SHORT * 2 + _INT + msg->content.data->datasize);

		} else if (msg->c_type == CT_KEYS) {

			while (temp_keys != NULL) {
				buffer_size += (strlen(temp_keys[i]) - 1);
				i++;
			}
			nkeys = i - 1;
			buffer_size += (_SHORT * 3 + _INT);

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

		printf("C_TYPE:: %d\n", msg->c_type);
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
			while (temp_keys != NULL) {
				short_value = htons(strlen(temp_keys[i]) - 1);
				memcpy(ptr, &long_value, _SHORT);
				ptr += _SHORT;

				memcpy(ptr, &temp_keys[i], strlen(temp_keys[i]) - 1);
				ptr += strlen(msg->content.keys) - 1;
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
	printf("MESSAGE_TO_BUFFER");
	if (msg_buf == NULL || msg_size <= 0)
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
	printf("OPCODE:: %d\n", msg->opcode);
	memcpy(&short_aux, msg_buf, _SHORT);
	msg->c_type = ntohs(short_aux);
	msg_buf += _SHORT;

	/* A mesma coisa que em cima mas de forma compacta, ao estilo C! */
	//msg->opcode = ntohs(*(short *) msg_buf++);
	//msg->c_type = ntohs(*(short *) ++msg_buf);
	//msg_buf += _SHORT;
	/* O opcode e c_type são válidos? */
	if ((valid(msg->opcode, msg->c_type)) != 0)
		return NULL;

	/* Consoante o c_type, continuar a recuperação da mensagem original */
	switch (msg->c_type) {
	case CT_RESULT:
		memcpy(&int_aux, msg_buf, _INT);
		msg->content.result = ntohl(int_aux);
		printf("C_TYPE:: %d\n", msg->c_type);
		printf("RESULT %d\n", msg->content.result);
		break;
	case CT_KEY:

		//KEYSIZE
		memcpy(&short_aux, msg_buf, _SHORT);
		int size_key = ntohs(short_aux);
		msg_buf += _SHORT;
		printf("%d\n", size_key);
		//KEY
		msg->content.key = (char *) malloc(size_key + 1);
		memcpy(msg->content.key, msg_buf, size_key);
		msg->content.key[size_key] = '\0';
		printf(msg->content.key);
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
		printf("DATASIZE:::::%d\n",data_size);
		struct data_t * temp_data = data_create(data_size);
		if(temp_data == NULL){
			free(aux_key);
			free_message(msg);
			return NULL;
		}
		//ENTRY

		msg->content.entry = entry_create(aux_key, temp_data);
		printf("---------------->%s:::: %d\n", msg->content.entry->key, msg->content.entry->value->datasize);
		if (msg->content.entry == NULL) {
			free(aux_key);
			data_destroy(temp_data);
			free_message(msg);
			return NULL;
		}
		memcpy(msg->content.entry->value->data, msg_buf, data_size);
		printf(msg->content.entry->key);
		free(aux_key);
		data_destroy(temp_data);
		break;

	case CT_KEYS:
		memcpy(&int_aux, msg_buf, _INT);
		int nkeys = ntohl(int_aux);
		msg_buf += _INT;
		int i;
		while (i < nkeys) {
			//KEYSIZE
			memcpy(&short_aux, msg_buf, _SHORT);
			int size_key = ntohs(short_aux);
			msg_buf += _SHORT;
			//KEY
			msg->content.keys[i] = (char *) malloc(size_key + 1);
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
	return ((opcode == OC_DEL || opcode == OC_SIZE || opcode == OC_PUT
			|| opcode == OC_UPDATE || opcode == OC_GET)
			&& (c_type == CT_ENTRY || c_type == CT_KEY || c_type == CT_KEYS
					|| c_type == CT_RESULT || c_type == CT_VALUE)) ? 0 : -1;

}

