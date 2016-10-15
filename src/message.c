#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "message-private.h"

void free_message(struct message_t *msg) {

	if (msg != NULL) {
		if (msg->c_type == CT_VALUE) {
			data_destroy(msg->content.data);
		} else if (msg->c_type == CT_ENTRY) {
			entry_destroy(msg->content.entry);
		} else if (msg->c_type == CT_KEYS) {
			table_free_keys(msg->content.keys);
		} else if (msg->c_type == CT_KEY) {
			free(msg->content.key);
		}
		/* libertar msg */
		//free(msg->content);
		free(msg);
	}
}

int message_to_buffer(struct message_t *msg, char **msg_buf) {

	/* Verificar se msg é NULL */
	if (msg == NULL) {
		return -1;
	}
	/* Consoante o msg->c_type, determinar o tamanho do vetor de bytes
	 que tem de ser alocado antes de serializar msg
	 */
	int buffer_size;

	if (msg->c_type == CT_RESULT) {
		buffer_size += _SHORT * 2 + _INT;
	} else if (msg->c_type == CT_VALUE) {
		buffer_size += (_SHORT * 2 + _INT + msg->content.data->datasize);

	} else if (msg->c_type == CT_KEYS) {

		char **temp_keys = msg->content.keys;
		int i = 0;
		while (temp_keys != NULL) {
			buffer_size += (strlen(temp_keys[i]) - 1);
			i++;
		}
		buffer_size += (_SHORT * 3 + _INT);

	} else if (msg->c_type == CT_KEY) {
		buffer_size += _SHORT * 3 + (strlen(msg->content.key) - 1);

	} else if (msg->c_type == CT_ENTRY) {
		buffer_size += (_SHORT * 3 + _INT + msg->content.data->datasize);
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
	case CT_RESULT:
		long_value = htonl(msg->content.result);
		memcpy(ptr, &long_value, _INT);
		ptr += _INT;
		break;
	case CT_KEY:
		short_value = htons(strlen(msg->content.key) - 1);
		memcpy(ptr, &short_value, _SHORT);
		return buffer_size;
	}
}
struct message_t *buffer_to_message(char *msg_buf, int msg_size) {
	if (msg_buf == NULL || msg_size <= 0)
		return NULL;

	int short_aux = 0, int_aux = 0;

	/* Alocar memória para uma struct message_t */
	struct message_t *msg = (struct message_t *) malloc(msg_size);
	if (msg == NULL)
		return NULL;

	/* Recuperar o opcode e c_type */
	memcpy(&short_aux, msg_buf, _SHORT);
	msg->opcode = ntohs(short_aux);
	msg_buf += _SHORT;

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
		break;
	case CT_KEY:
		//kEYSIZE
		memcpy(&short_aux, msg_buf, _SHORT);
		int size_key = ntohs(short_aux);
		msg_buf += size_key;
		//KEY
		msg->content.key = (char *) malloc(size_key + 1);
		memcpy(msg->content.key, msg_buf, size_key);
		msg->content.key[size_key] = '\0';
		break;
	case CT_VALUE:
		//DATASIZE
		memcpy(&int_aux, msg_buf, _INT);
		int data_size = ntohs(int_aux);
		msg_buf += data_size;
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
		msg_buf += size_key;
		//KEY

		break;
	case CT_KEYS:
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

