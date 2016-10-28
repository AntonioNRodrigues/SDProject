/*
 Programa que implementa um servidor de uma tabela hash com chainning.
 Uso: table-server <porta TCP> <dimensão da tabela>
 Exemplo de uso: ./table_server 54321 10
 */
#include <error.h>

#include "inet.h"
#include "table-private.h"
#include "message-private.h"
#include "network_client-private.h"

/* Função para preparar uma socket de receção de pedidos de ligação.
 */
int make_server_socket(short port) {
	printf("MAKE SERVER CONNECTION\n");
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
	printf("PROCESS_MESSAGE --> BEGIN\n");
	char *temp_key;
	struct message_t *msg_resposta = (struct message_t *) malloc(
			sizeof(struct message_t));
	if (msg_resposta == NULL) {
		printf("PROCESS_MESSAGE --> msg-resposta is NULL\n");
		return NULL;
	}
	int result;

	/* Verificar parâmetros de entrada */
	if (msg_pedido == NULL || tabela == NULL) {
		printf("PROCESS_MESSAGE --> msg_pedido || tabels is NULL\n");
		return NULL;
	}
	printf("PROCESS_MESSAGE --> PARAM CHECK\n");
	/* Verificar opcode e c_type na mensagem de pedido */
	if (msg_pedido->opcode < 0 || msg_pedido->c_type < 0) {
		return NULL;
	}
	/* Aplicar operação na tabela */

	/* Preparar mensagem de resposta */
	printf("PROCESS_MESSAGE --> OPCODE = %d\n", msg_pedido->opcode);
	switch (msg_pedido->opcode) {
	case OC_SIZE:
		result = table_size(tabela);
		//the table is empty
		printf("PROCESS_MESSAGE --> SIZE TABELA::::%d\n", result);
		if (result < 0) {
			msg_resposta->c_type = CT_RESULT;
			msg_resposta->opcode = OC_RT_ERROR;
			msg_resposta->content.result = -1;
		} else {
			msg_resposta->c_type = CT_RESULT;
			msg_resposta->opcode = OC_SIZE + 1;
			msg_resposta->content.result = result;
		}
		break;
	case OC_PUT:
		printf("PROCESS_MESSAGE:::::OC_PUT\n");
		result = table_put(tabela, msg_pedido->content.entry->key,
				msg_pedido->content.entry->value);
		printf("PROCESS_MESSAGE --> PUT TABELA::::%d\n", result);
		//table_put failed
		if (result == -1) {
			msg_resposta->c_type = CT_RESULT;
			msg_resposta->opcode = OC_RT_ERROR;
			msg_resposta->content.result = -1;

		} else {
			msg_resposta->c_type = CT_RESULT;
			msg_resposta->opcode = OC_PUT + 1;
			msg_resposta->content.result = result;
		}
		break;
	case OC_GET:

		temp_key = strdup(msg_pedido->content.key);

		// temp_key is NULL
		if (temp_key == NULL) {
			msg_resposta->c_type = CT_RESULT;
			msg_resposta->opcode = OC_RT_ERROR;
			msg_resposta->content.result = -1;
		}
		//key is ! --> GET ALL KEYS
		if (strcmp("!", temp_key) == 0) {
			printf("TABLE GET ALLLLLLL KEYS\n");
			if (tabela->quantity_entry == 0) {
				msg_resposta->c_type = CT_RESULT;
				msg_resposta->opcode = OC_RT_ERROR;
				msg_resposta->content.result = -1;
			} else {
				msg_resposta->c_type = CT_KEYS;
				msg_resposta->opcode = OC_GET + 1;
				msg_resposta->content.keys = table_get_keys(tabela);
				printf("TABLE KEYS = %d", msg_resposta->opcode);
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
				msg_resposta->c_type = CT_RESULT;
				msg_resposta->opcode = OC_RT_ERROR;
				msg_resposta->content.result = -1;
			}
		}
		break;
	case OC_UPDATE:
		result = table_update(tabela, msg_pedido->content.entry->key,
				msg_pedido->content.entry->value);
		//table_update failed
		if (result == -1) {
			msg_resposta->c_type = CT_RESULT;
			msg_resposta->opcode = OC_RT_ERROR;
			msg_resposta->content.result = result;
		} else {
			msg_resposta->c_type = CT_RESULT;
			msg_resposta->opcode = OC_UPDATE + 1;
			msg_resposta->content.result = result;
		}
		break;

	case OC_DEL:
		printf("PROCESS_MESSAGE ::::: OC_DEL\n");
		result = table_del(tabela, msg_pedido->content.key);
		printf("PROCESS_MESSAGE ::::: OC_DEL::RESULT = %d\n", result);
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

/* Função "inversa" da função network_send_receive usada no table-client.
 Neste caso a função implementa um ciclo receive/send:

 Recebe um pedido;
 Aplica o pedido na tabela;
 Envia a resposta.
 */
int network_receive_send(int sockfd, struct table_t *table) {
	char *message_resposta, *message_pedido;
	int message_size, msg_size, result;
	struct message_t *msg_pedido, *msg_resposta;

	/* Verificar parâmetros de entrada */
	if (table == NULL || sockfd == -1) {
		return -1;
	}

	/* Com a função read_all, receber num inteiro o tamanho da 
	 mensagem de pedido que será recebida de seguida.*/

	result = read_all(sockfd, (char *) &msg_size, _INT);
	/* Verificar se a receção teve sucesso */
	if(result == 0){
		return -1;
	}

	
	
	message_size = ntohl(msg_size);
	/* Alocar memória para receber o número de bytes da
	 mensagem de pedido. */
	message_pedido = (char *) malloc(message_size);

	/* Com a função read_all, receber a mensagem de pedido. */
	result = read_all(sockfd, message_pedido, message_size);
	/* Verificar se a receção teve sucesso */
	if (result != message_size) {
		return -1;
	}
	/* Desserializar a mensagem do pedido */
	msg_pedido = buffer_to_message(message_pedido, message_size);
	/* Verificar se a desserialização teve sucesso */
	if (msg_pedido == NULL) {
		return -1;
	}
	/* Processar a mensagem */
	msg_resposta = process_message(msg_pedido, table);
	/* Serializar a mensagem recebida */
	message_size = message_to_buffer(msg_resposta, &message_resposta);

	/* Verificar se a serialização teve sucesso */
	if (message_size < 0) {
		return -1;
	}

	/* Enviar ao cliente o tamanho da mensagem que será enviada
	 logo de seguida
	 */
	msg_size = htonl(message_size);
	result = write_all(sockfd, (char *) &msg_size, _INT);

	/* Verificar se o envio teve sucesso */
	if (result < 0) {
		return -1;
	}

	/* Enviar a mensagem que foi previamente serializada */

	result = write_all(sockfd, message_resposta, message_size);

	/* Verificar se o envio teve sucesso */
	if (result < 0) {
		return -1;
	}
	/* Libertar memória */

	return 1;
}

/*void printTable(struct table_t* table){
 int x, y;
 for(x = 0; x < table->quantity_entry; x++){
 printf("lista %d:\n", x);
 struct node_t* aux = table->buckets[x]->head;
 while(aux != NULL){
 printf("\t KEY: %s  DATASIZE: %d  DATA: %s \n", aux->entry->key, aux->entry->value->datasize, (char *)aux->entry->value->data);
 aux = aux->next;
 }
 }

 }*/

int main(int argc, char **argv) {
	int listening_socket, connsock, result;
	struct sockaddr_in client;
	socklen_t size_client;
	struct table_t *table;

	if (argc != 3) {
		printf("Uso: ./server <porta TCP> <dimensão da tabela>\n");
		printf("Exemplo de uso: ./table-server 54321 10\n");
		return -1;
	}

	if ((listening_socket = make_server_socket(atoi(argv[1]))) < 0) {
		printf("LISTENING\n");
		return -1;

	}

	if ((table = table_create(atoi(argv[2]))) == NULL) {
		printf("table\n");
		result = close(listening_socket);
		return -1;
	}

	//printTable(table);
	while ((connsock = accept(listening_socket, (struct sockaddr *) &client,
			&size_client)) != -1) {

		printf(" * Client is connected!\n");
		printf(" ================================= \n");
		while (listening_socket != 0) {
			int aux1 = network_receive_send(connsock, table);
			if(aux1 == -1){
				printf("Cliente fez quit\n");
				close(connsock);
				break;
			}
			//correu tudo bem
			//erro
			//client fechou sai do ciclo close connection

			printf(" ================================= \n");
		}

		//printTable(table);

		close(connsock);
		printf("Connection closed.\n Waiting for new connection.\n");
		/* Ciclo feito com sucesso ? Houve erro?
		 Cliente desligou? */
	}

	close(listening_socket);
	return 0;
}

