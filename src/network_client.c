/*
 * Grupo 33
 * Miguel Vale n.39279
 * António Rodrigues n.º40853
 * Ricardo Veloso n.º44842
 */

#include "network_client-private.h"

#include <stdlib.h>

int write_all(int sock_fd, char *buffer, int lenght) {
	int nbytes_write = lenght;

	while (lenght > 0) {
		nbytes_write = write(sock_fd, buffer, lenght);
		if (nbytes_write < 0) {
			printf("%s \n", "Write failed");
			return -1;
		}
		lenght -= nbytes_write;
		buffer += nbytes_write;
	}
	return nbytes_write;
}

int read_all(int sock_fd, char *buffer, int length) {
	int nbytes_read;
	while (length > 0) {
		nbytes_read = read(sock_fd, buffer, length);
		if (nbytes_read < 0) {
			perror("Error reading from the buffer");
			return -1;
		}
		if (nbytes_read == 0) {
			return 0;
		}

		buffer += nbytes_read;
		length -= nbytes_read;
	}
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
		free(server);
		return NULL;
	}

	//Preenche estrutura struct sockaddr_in server com dados do endereço do servidor
	server->server.sin_family = AF_INET;
	server->server.sin_port = htons(atoi(token2)); //Porta TCP

	if (inet_pton(AF_INET, token1, &server->server.sin_addr) < 1) {
		printf("Erro ao converter IP\n");
		close(server->sock_file_descriptor);
		free(server);
		return NULL;
	}
	/* Estabelece a ligação */
	/* Se a ligação não foi estabelecida, retornar NULL */

	if (connect(server->sock_file_descriptor,
			(struct sockaddr *) &server->server, sizeof(server->server)) < 0) {
		perror("Erro ao conectar-se ao servidor");
		close(server->sock_file_descriptor);
		free(server);
		return NULL;
	}
	return server;
}

/**
 * function to establish the connection with the server
 */
struct server_t *net_connect(struct server_t *server) {

	if (server == NULL) {
		return NULL;
	}

	if (connect(server->sock_file_descriptor,
			(struct sockaddr *) &server->server, sizeof(server->server)) < 0) {
		perror("Erro ao conectar-se ao servidor");
		close(server->sock_file_descriptor);
		free(server);
		return NULL;
	}
	return server;
}

struct server_t *network_reconnect(struct server_t* server) {
	if (server == NULL) {
		return NULL;
	}

	struct server_t *new_server = (struct server_t *) malloc(
			sizeof(struct server_t));
	if (new_server == NULL) {
		return NULL;
	}

	if ((new_server->sock_file_descriptor = socket(AF_INET, SOCK_STREAM, 0))
			< 0) {
		perror("Error creating the socket");
		free(new_server);
		return NULL;
	}
	new_server->server.sin_family = AF_INET;
	new_server->server.sin_port = server->server.sin_port;
	new_server->server.sin_addr = server->server.sin_addr;

	if (connect(new_server->sock_file_descriptor,
			(struct sockaddr *) &new_server->server, sizeof(new_server->server))
			< 0) {
		perror("Error connecting to server");
		close(new_server->sock_file_descriptor);
		free(new_server);
		return NULL;
	}
	//
	if (network_close(server) == 0) {
		printf(
				"The reconnect has been made properly.\n"
						" The old struct server has been closed and a new one has been init\n");
	} else {
		printf("The reconnect has not been made properly.\n"
				" The old struct server has been closed with errors\n");
	}

	return new_server;

}

struct server_t *network_prepare(const char *address_port) {

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

	//Cria a socket
	if ((server->sock_file_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Erro ao criar socket");
		free(server);
		return NULL;
	}

	//Preenche estrutura struct sockaddr_in server com dados do endereço do servidor
	server->server.sin_family = AF_INET;
	server->server.sin_port = htons(atoi(token2)); //Porta TCP

	if (inet_pton(AF_INET, token1, &server->server.sin_addr) < 1) {
		printf("Erro ao converter IP\n");
		close(server->sock_file_descriptor);
		free(server);
		return NULL;
	}

	return server;
}

struct message_t *network_send_receive(struct server_t *server,
		struct message_t *msg) {
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
	/* Verificar se a serialização teve sucesso */
	if (message_size <= 0) {
		return NULL;
	}

	/* Enviar ao servidor o tamanho da mensagem que será enviada
	 logo de seguida
	 */
	msg_size = htonl(message_size);
	result = write_all(server->sock_file_descriptor, (char *) &msg_size, _INT);

	/* Verificar se o envio teve sucesso */
	if (result != _INT) {
		close(server->sock_file_descriptor);
		return NULL;
	}

	/* Enviar a mensagem que foi previamente serializada */
	result = write_all(server->sock_file_descriptor, message_out, message_size);
	/* Verificar se o envio teve sucesso */
	if (result != message_size) {
		//close(server->sock_file_descriptor);
		return NULL;
	}
	/* De seguida vamos receber a resposta do servidor:

	 Com a função read_all, receber num inteiro o tamanho da
	 mensagem de resposta.


	 Alocar memória para receber o número de bytes da
	 mensagem de resposta.

	 Com a função read_all, receber a mensagem de resposta.

	 */
	int size_returned_msg;
	result = read_all(server->sock_file_descriptor, (char *) &size_returned_msg,
	_INT);
	if (result != _INT) {
		close(server->sock_file_descriptor);
		return NULL;
	}
	int msg_returned = ntohl(size_returned_msg);
	message_in = (char *) malloc(msg_returned);

	result = read_all(server->sock_file_descriptor, message_in, msg_returned);

	/* Desserializar a mensagem de resposta */
	msg_resposta = buffer_to_message(message_in, msg_returned);

	/* Verificar se a desserialização teve sucesso */
	if (msg_resposta == NULL) {
		free(message_in);
		free(message_out);
		return NULL;
	}

	/* Libertar memória */
	free(message_in);
	free(message_out);
	return msg_resposta;
}

int network_close(struct server_t *server) {
	/* Verificar parâmetros de entrada */
	if (server == NULL)
		return -1;

	/* Terminar ligação ao servidor */
	/*result = (0 ==> sucess) || (-1 ==> error) */
	int result = close(server->sock_file_descriptor);

	free(server);
	return result;
}

