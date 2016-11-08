/*
 * Grupo 33
 * Miguel Vale n.39279
 * António Rodrigues n.º40853
 * Ricardo Veloso n.º44842
 */

/*
 Programa que implementa um servidor de uma tabela hash com chainning.
 Uso: table-server <porta TCP> <dimensão da tabela>
 Exemplo de uso: ./table_server 54321 10
 */

//TO BE MOVED TO THE TABLE SKEL PRIVATE
//number of file descriptor
#define NFDS 10
#define TIMEOUT 50

#include <error.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <poll.h>
#include <fcntl.h>

#include "inet.h"
#include "table-private.h"
#include "message-private.h"
#include "network_client-private.h"
#include "table_skel-private.h"


/* Função para preparar uma socket de receção de pedidos de ligação.
 */
int make_server_socket(short port) {
	int socket_fd;
	int reuse_address, reuse_port = 1;
	struct sockaddr_in server;

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Erro ao criar socket");
		return -1;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	//make the socket reusable
	//REUSEADDR
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (int *) &reuse_address,
			sizeof(reuse_address)) < 0) {
		perror("Error reusing the socket :: REUSEADDR");
	}
	//REUSEPORT
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, (int *) &reuse_port,
			sizeof(reuse_port)) < 0) {
		perror("Error reusing the socket :: REUSEPORT");
	}

	if (bind(socket_fd, (struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("Erro ao fazer bind");
		close(socket_fd);
		return -1;
	}

	if (listen(socket_fd, 0) < 0) {
		perror("Erro ao executar listen");
		close(socket_fd);
		return -1;
	}
	return socket_fd;
}


/* Função "inversa" da função network_send_receive usada no table-client.
 Neste caso a função implementa um ciclo receive/send:

 Recebe um pedido;
 Aplica o pedido na tabela;
 Envia a resposta.
 */
int network_receive_send(int sockfd) {
	char *message_resposta, *message_pedido;
	int message_size, msg_size, result;
	struct message_t *msg_pedido, *msg_resposta;

	/* Verificar parâmetros de entrada */
	if (sockfd == -1) {
		return -1;
	}

	/* Com a função read_all, receber num inteiro o tamanho da 
	 mensagem de pedido que será recebida de seguida.*/

	result = read_all(sockfd, (char *) &msg_size, _INT);
	/* Verificar se a receção teve sucesso */
	if (result == 0) {
		return -1;
	}

	message_size = ntohl(msg_size);
	/* Alocar memória para receber o número de bytes da
	 mensagem de pedido. */
	message_pedido = (char *) malloc(message_size);
	if (message_pedido == NULL) {
		return -1;
	}

	/* Com a função read_all, receber a mensagem de pedido. */
	result = read_all(sockfd, message_pedido, message_size);
	/* Verificar se a receção teve sucesso */
	if (result != message_size) {
		free(message_pedido);
		return -1;
	}
	/* Desserializar a mensagem do pedido */
	msg_pedido = buffer_to_message(message_pedido, message_size);
	/* Verificar se a desserialização teve sucesso */
	if (msg_pedido == NULL) {
		free(message_pedido);
		return -1;
	}
	/* Processar a mensagem */
	msg_resposta = invoke(msg_pedido);
	/* Serializar a mensagem recebida */
	message_size = message_to_buffer(msg_resposta, &message_resposta);

	/* Verificar se a serialização teve sucesso */
	if (message_size < 0) {
		free_message(msg_resposta);
		free_message(msg_pedido);
		return -1;
	}

	/* Enviar ao cliente o tamanho da mensagem que será enviada
	 logo de seguida
	 */
	msg_size = htonl(message_size);
	result = write_all(sockfd, (char *) &msg_size, _INT);

	/* Verificar se o envio teve sucesso */
	if (result < 0) {
		free_message(msg_resposta);
		free_message(msg_pedido);
		return -1;
	}

	/* Enviar a mensagem que foi previamente serializada */

	result = write_all(sockfd, message_resposta, message_size);

	/* Verificar se o envio teve sucesso */
	if (result < 0) {
		free_message(msg_pedido);
		free_message(msg_resposta);
		return -1;
	}
	/* Libertar memória */
	free_message(msg_pedido);
	free_message(msg_resposta);
	return 1;
}
int find_free_connection(struct pollfd *conn) {
	int free_index = -1;
	for (int k = 1; k < NFDS; k++) {
		if (conn[k].fd == -1) {
			free_index = k;
			break;
		}
	}
	return free_index;
}

int main(int argc, char **argv) {
	struct sockaddr_in client;
	socklen_t size_client;
	// struct of file descripters
	struct pollfd connections[NFDS];
	int listening_socket, result;
	int number_clients = 1;

	//test argc
	if (argc != 3) {
		printf("Uso: ./table-server <porta TCP> <dimensão da tabela>\n");
		printf("Exemplo de uso: ./table-server 54321 10\n");
		return -1;
	}

	//listening socket up
	if ((listening_socket = make_server_socket(atoi(argv[1]))) < 0) {
		return -1;
	}
	//table_skel_init
	if (table_skel_init(atoi(argv[2])) == -1) {
		printf("table\n");
		result = close(listening_socket);
		return -1;
	}

	printf("***********************************\n");
	printf("*   SERVER WITH POLL              *\n");
	printf("***********************************\n\n");
	printf("A espera de cliente\n");

	//init each positions of connections[i].fd with -1
	for (int i = 1; i < NFDS; i++) {
		connections[i].fd = -1;
	}
	//first position of connetions is the listening_socket
	connections[0].fd = listening_socket;
	connections[0].events = POLLIN; // POLLIN ==> data to be read and in this case a new connections received
	int ret;

	while ((ret = poll(connections, NFDS, TIMEOUT) >= 0)) {
		printf("%d", ret);
		if (ret > 0) {

			// listenning socket has a new connection
			if ((connections[0].revents & POLLIN) && (number_clients < NFDS)) {

				int free_index = find_free_connection(connections);

				//-1 there is no space in the array --> do not accept socket
				if (free_index != -1) {
					if ((connections[free_index].fd = accept(connections[0].fd,
							(struct sockaddr *) &client, &size_client)) > 0) {
						connections[free_index].events = POLLIN;
						number_clients++;
					}

				}
				ret--;
			}
			printf("-- %d\n", ret);
			for (int j = 1; j < NFDS && ret > 0; j++) {

				//if socket has data to read
				if (connections[j].revents & POLLIN) {
					/*if (network_receive_send(connections[j].fd, table) < 0) {
					 printf("client quit");
					 close(connections[j].fd);
					 number_clients--;
					 connections[j].fd = -1;
					 printf("closed connection %d", number_clients);
					 } else {*/
					network_receive_send(connections[j].fd);
					/*}*/
				}
				if (connections[j].revents == POLLHUP
						|| connections[j].revents == POLLERR) {
					close(connections[j].fd);
					connections[j].fd = -1;
					number_clients--;
				}

			}
		}

	}
	//table_skel_destroy();
	for (int l = 1; l < NFDS; l++) {
		close(connections[l].fd);
	}
	return 0;
}

