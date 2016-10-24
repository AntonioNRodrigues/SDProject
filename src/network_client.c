#include "network_client-private.h"

#include <stdlib.h>

int write_all(int sock, char *buf, int len) {
	printf("WRITE_ALL-->BEGIN\n");
	int nbytes_write = len;
	int msg;
	while (len > 0) {
		msg = write(sock, buf, len);
		if (msg == -1) {
			printf("%s \n", "Write failed");
			return msg;
		}
		buf += msg;
		len -= msg;
	}
	printf("WRITE_ALL-->END %d\n", nbytes_write);
	return nbytes_write;
}

int read_all(int sock, char *buf, int len) {
	printf("WRITE_ALL-->BEGIN\n");
	int nbytes_read = len;
	int msg;
	while (len > 0) {
		msg = read(sock, buf, len);
		if (msg == 0) {
			return 0;
		}
		if (msg == -1) {
			printf("%s \n", "Read failed");
			return msg;
		}
		buf += msg;
		len -= msg;
	}
	printf("READ-->END %d\n", nbytes_read);
	return nbytes_read;

}

struct server_t *network_connect(const char *address_port) {

	/* Verificar parâmetro da função e alocação de memória */
	if (address_port == NULL) {
		return NULL;
	}

	struct server_t *server = (struct server_t *) malloc(
			sizeof(struct server_t));
	if (server == NULL) {
		return NULL;
	}

	char *token1, *token2;

	token1 = strtok(strdup(address_port), ":");
	token2 = strtok(NULL, "\n");

	/* Estabelecer ligação ao servidor:


	 Preencher estrutura struct sockaddr_in com dados do
	 endereço do servidor.

	 Criar a socket.

	 Estabelecer ligação.
	 */

	//Cria a socket
	if ((server->sock_file_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Erro ao criar socket");
		return NULL;
	}

	//Preenche estrutura struct sockaddr_in server com dados do endereço do servidor
	server->server.sin_family = AF_INET;
	server->server.sin_port = htons(atoi(token2)); //Porta TCP

	if (inet_pton(AF_INET, token1, &server->server.sin_addr) < 1) {
		printf("Erro ao converter IP\n");
		close(server->sock_file_descriptor);
		return NULL;
	}
	/* Estabelece a ligação */
	/* Se a ligação não foi estabelecida, retornar NULL */

	if (connect(server->sock_file_descriptor,
			(struct sockaddr *) &server->server, sizeof(server->server)) < 0) {
		free(server); // nao chega------
		perror("Erro ao conectar-se ao servidor");
		close(server->sock_file_descriptor);
		return NULL;
	}
	return server;
}

struct message_t *network_send_receive(struct server_t *server,
		struct message_t *msg) {
	printf("NETWORK_SEND_RECEIVE-->BEGIN\n");
	char *message_out;
	char *message_in;
	int message_size, msg_size, result;
	struct message_t *msg_resposta;

	/* Verificar parâmetros de entrada */
	if (msg == NULL || server == NULL) {
		return NULL;
	}

	/* Serializar a mensagem recebida */
	message_size = message_to_buffer(msg, &message_out);
	printf("NETWORK_SEND_RECEIVE-->MESSAGE_SIZE:: %d\n", message_size);
	/* Verificar se a serialização teve sucesso */
	if (message_size <= 0) {
		return NULL;
	}

	/* Enviar ao servidor o tamanho da mensagem que será enviada
	 logo de seguida
	 */
	msg_size = htonl(message_size);
	printf("NETWORK_SEND_RECEIVE-->htonl MSG_SIZE:: %d\n", msg_size);

	result = write_all(server->sock_file_descriptor, (char *) &msg_size, _INT);
	printf("NETWORK_SEND_RECEIVE-->MESSAGE_SIZE:: %d\n", result);

	/* Verificar se o envio teve sucesso */
	if (result != _INT) {
		return NULL;
	}

	/* Enviar a mensagem que foi previamente serializada */
	result = write_all(server->sock_file_descriptor, message_out, message_size);
	printf("NETWORK_SEND_RECEIVE-->MESSAGE_SIZE:: %d\n", message_size);
	/* Verificar se o envio teve sucesso */
	if (result != message_size) {
		return NULL;
	}

	/* De seguida vamos receber a resposta do servidor:

	 Com a função read_all, receber num inteiro o tamanho da
	 mensagem de resposta.


	 Alocar memória para receber o número de bytes da
	 mensagem de resposta.

	 Com a função read_all, receber a mensagem de resposta.

	 */
	int size_returned_msg = 0;
	result = read_all(server->sock_file_descriptor, (char *) &size_returned_msg, _INT);
	if (result != _INT) {
		//free
		return NULL;
	}
	printf("NETWORK_SEND_RECEIVE-->READ ALL MESSAGE_SIZE:: %d\n", result);

	int msg_returned = ntohl(size_returned_msg);
	message_in = (char *) malloc(msg_returned);

	result = read_all(server->sock_file_descriptor, message_in, msg_returned);
	printf("NETWORK_SEND_RECEIVE-->READ ALL MESSAGE :: %d\n", result);
	/* Desserializar a mensagem de resposta */
	msg_resposta = buffer_to_message(message_in, msg_returned);
	printf("NETWORK_SEND_RECEIVE-->READ ALL MESSAGE_RESPONSE:: %d\n", msg_resposta->content.result);

	/* Verificar se a desserialização teve sucesso */
	if (msg_resposta == NULL) {
		return NULL;
	}

	/* Libertar memória */
	//-------------------------TO DO-------------------------------->
	printf("NETWORK_SEND_RECEIVE-->END\n");
	return msg_resposta;
}

int network_close(struct server_t *server) {
	/* Verificar parâmetros de entrada */
	if (server == NULL)
		return -1; //--------------------->DEVE RETORNAR O QUE?---------------1
	/* Terminar ligação ao servidor */
	int result = close(server->sock_file_descriptor);
	if (result < 0)
		return -1;
	free(server);
	return result;
}

