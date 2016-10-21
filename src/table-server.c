/*
 Programa que implementa um servidor de uma tabela hash com chainning.
 Uso: table-server <porta TCP> <dimensão da tabela>
 Exemplo de uso: ./table_server 54321 10
 */
#include <error.h>

#include "inet.h"
#include "table-private.h"
#include "message-private.h"

/* Função para preparar uma socket de receção de pedidos de ligação.
 */
int make_server_socket(short port) {
	int socket_fd;
	struct sockaddr_in server;

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Erro ao criar socket");
		return -1;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(socket_fd, (struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("Erro ao fazer bind");
		close(socket_fd);
		return -1;
	}

	//---------------------------> atencao ao Zero do parametro do listen
	if (listen(socket_fd, 0) < 0) {
		perror("Erro ao executar listen");
		close(socket_fd);
		return -1;
	}
	return socket_fd;
}

/* Função que recebe uma tabela e uma mensagem de pedido e:
 - aplica a operação na mensagem de pedido na tabela;
 - devolve uma mensagem de resposta com o resultado.
 */
struct message_t *process_message(struct message_t *msg_pedido,
		struct table_t *tabela) {

	struct message_t *msg_resposta = (struct message_t *) malloc(
			sizeof(struct message_t));
	int result;

	/* Verificar parâmetros de entrada */
	if (msg_pedido == NULL || tabela == NULL)
		return NULL;

	/* Verificar opcode e c_type na mensagem de pedido */
	if (msg_pedido->opcode == NULL || msg_pedido->c_type == NULL) {
		return NULL;
	}
	/* Aplicar operação na tabela */

	/* Preparar mensagem de resposta */

	switch (msg_pedido->opcode) {
	case OC_SIZE:
		result = table_size(tabela);
		//the table is empty
		if (result < 0) {
			msg_resposta->c_type = CT_RESULT;
			msg_resposta->opcode = OC_RT_ERROR;
			msg_resposta->content.result = -1;
		}
		msg_resposta->c_type = CT_RESULT;
		msg_resposta->opcode = OC_SIZE + 1;
		msg_resposta->content.result = result;
		break;
	case OC_PUT:
		result = table_put(tabela, msg_pedido.content->entry->key,
				msg_pedido.content->entry->value);
		//table_put failed
		if (result == -1) {
			return NULL;
			//build error message
		}
		msg_resposta->c_type = OC_PUT + 1;
		msg_resposta->content.result = result;
		break;
	case OC_GET:
		char * temp_key = msg_pedido->content.key;
		// temp_key is NULL
		if (temp_key == NULL) {
			return NULL;
		}
		//key is ! --> GET ALL KEYS
		if (strcmp("!", temp_key) == 0) {
			msg_resposta->c_type = CT_KEYS;
			msg_resposta->opcode = OC_GET + 1;
			msg_resposta->content.keys = table_get_keys(temp_key);
		} else {
			struct data_t temp_data = table_get(tabela, temp_key);
			//the key is present
			if (temp_data != NULL) {
				msg_resposta->c_type = CT_VALUE;
				msg_resposta->opcode = OC_GET + 1;
				msg_resposta->content.data = data_dup(temp_data);
				data_destroy(temp_data);
				//key does not exist
			} else {
				struct data_t *t = data_create2(0, NULL);
				msg_resposta->c_type = CT_RESULT;
				msg_resposta->opcode = OC_RT_ERROR;
				msg_resposta->content.data = t;
				data_destroy(t);
			}
		}
		break;
	case OC_UPDATE:
		result = table_update(tabela, msg_pedido.content->entry->key,
				msg_pedido.content->entry->value);
		//table_update failed
		if (result == -1) {
			//build error message
			return NULL;
		}
		msg_resposta->c_type = OC_UPDATE + 1;
		msg_resposta->content.result = result;
		break;
	case OC_DEL:
		result = table_del(tabela, msg_pedido.content->key);
		//table_del failed
		if (result == -1) {
			//build error message
			return NULL;
		}
		msg_resposta->c_type = OC_UPDATE + 1;
		msg_resposta->content.result = result;
		break;
	default:
		break;
	}

	return msg_resposta;
}

/* Função "inversa" da função network_send_receive usada no table-client.
 Neste caso a função implementa um ciclo receive/send:

 Recebe um pedido;
 Aplica o pedido na tabela;
 Envia a resposta.
 */
int network_receive_send(int sockfd, struct table_t *table) {
	char *message_resposta, *message_pedido;
	int msg_length;
	int message_size, msg_size, result;
	struct message_t *msg_pedido, *msg_resposta;
	struct list_t *results;

	/* Verificar parâmetros de entrada */
	if (table == NULL || sockfd == -1) {
		return -1;
	}

	/* Com a função read_all, receber num inteiro o tamanho da 
	 mensagem de pedido que será recebida de seguida.*/

	result = read_all(sockfd, (char *) &msg_size, _INT);

	/* Verificar se a receção teve sucesso */
	if (msg_size < 0) {
		return -1;
	}
	message_size = ntohl(msg_size);

	/* Alocar memória para receber o número de bytes da
	 mensagem de pedido. */
	message_pedido = (char *) malloc(message_size);

	/* Com a função read_all, receber a mensagem de pedido. */
	result = read_all(sockfd, message_pedido, message_size);

	/* Verificar se a receção teve sucesso */
	if(result < 0){
		return - 1;
	}
	/* Desserializar a mensagem do pedido */
	msg_pedido = buffer_to_message(message_pedido, message_size);

	/* Verificar se a desserialização teve sucesso */
	if(msg_pedido == NULL){
		return -1;
	}
	/* Processar a mensagem */
	msg_resposta = process_message(msg_pedido, table);

	/* Serializar a mensagem recebida */
	message_size = message_to_buffer(msg_resposta, &message_resposta);

	/* Verificar se a serialização teve sucesso */
	if(message_size < 0){
		return -1;
	}

	/* Enviar ao cliente o tamanho da mensagem que será enviada
	 logo de seguida
	 */
	msg_size = htonl(message_size);
	result = write_all(sockfd, (char *) &msg_size, _INT);

	/* Verificar se o envio teve sucesso */
	if(result < 0){
		return -1;
	}

	/* Enviar a mensagem que foi previamente serializada */

	result = write_all(sockfd, message_resposta, message_size);

	/* Verificar se o envio teve sucesso */

	/* Libertar memória */

	return 0;
}

int main(int argc, char **argv) {
	int listening_socket, connsock, result;
	struct sockaddr_in client;
	socklen_t size_client;
	struct table_t *table;

	if (argc != 2) {
		printf("Uso: ./server <porta TCP> <dimensão da tabela>\n");
		printf("Exemplo de uso: ./table-server 54321 10\n");
		return -1;
	}

	if ((listening_socket = make_server(atoi(argv[1]))) < 0)
		return -1;

	if ((table = table_create(atoi(argv[2]))) == NULL) {
		result = close(listening_socket);
		return -1;
	}

	while ((connsock = accept(listening_socket, (struct sockaddr *) &client,
			&size_client)) != -1) {
		printf(" * Client is connected!\n");

		while (/* condição */) {

			/* Fazer ciclo de pedido e resposta */
			network_receive_send(connsock, table) < 0
		);

		/* Ciclo feito com sucesso ? Houve erro?
		 Cliente desligou? */

	}
}
}
