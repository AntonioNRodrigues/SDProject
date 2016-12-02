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

#include <error.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <poll.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#include <errno.h>
#include "inet.h"
#include "network_client-private.h"
#include "table_skel-private.h"

struct server_t *backup_server;

typedef struct n_servers {
	struct server_t *backup_server;
	FILE *file;
	char *state;
	char *name;

} backup_server2;

int ignsigpipe() {
	struct sigaction s;
	s.sa_handler = SIG_IGN;
	return sigaction(SIGPIPE, &s, NULL);
}

void *send_receive_backup(void * msg) {

	struct message_t *msg_to_backup = (struct message_t*) msg;
	struct message_t *msg_from_backup = network_send_receive(backup_server,
			msg_to_backup);
	if (msg_from_backup == NULL) {
		printf("The backup server is down\n");
		backup_server->sock_file_descriptor = -10;
		return NULL;
	}
	return (void *) msg_from_backup;
}

/* Função para preparar uma socket de receção de pedidos de ligação.
 */
int make_server_socket(short port) {
	int socket_fd;
	int reuse_address = 1, reuse_port = 1;
	struct sockaddr_in server;

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error making the socket_fd");
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
		perror("Error doing the bind");
		close(socket_fd);
		return -1;
	}

	if (listen(socket_fd, 0) < 0) {
		perror("Error making the listen");
		close(socket_fd);
		return -1;
	}
	return socket_fd;
}
/**
 *
 */
int network_receive_send_backup(int sockfd) {
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

/* Função "inversa" da função network_send_receive usada no table-client.
 Neste caso a função implementa um ciclo receive/send:

 Recebe um pedido;
 Aplica o pedido na tabela;
 Envia a resposta.
 this message returns 1 (OK) and -1 (NOK)
 */
int network_receive_send(int sockfd) {
	struct message_t *temp, *temp1;
	pthread_t thread;
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

	/*after receiving the message ask the backup server*/
	/*check if the operation is a Write type*/
	/*IN THIS MOMENT WE HAVE TO CHECK IF THE BACKUP SERVER IS UP || DOWN
	 * IF IS NOT DONT DO THIS ---> mutex or a condition check
	 */
	if (msg_pedido->opcode == OC_DEL || msg_pedido->opcode == OC_PUT
			|| msg_pedido->opcode == OC_UPDATE) {
		if (backup_server->sock_file_descriptor == -10) {
			printf("The server is runing without backup\n");
		} else {
			//build temp message equal to the initial request to "send" in the thread
			temp = (struct message_t*) malloc(sizeof(struct message_t));
			temp->c_type = msg_pedido->c_type;
			temp->opcode = msg_pedido->opcode;

			if (msg_pedido->opcode == OC_DEL) {
				temp->content.key = strdup(msg_pedido->content.key);
			} else {
				temp->content.entry = entry_dup(msg_pedido->content.entry);
			}
			//init the thread with the initial message request
			pthread_create(&thread, NULL, send_receive_backup, (void *) temp);

		}

	}
	//waiting for the thread to finish
	printf("Thread for the backup server fininshed\n");
	temp1 = (struct message_t*) malloc(sizeof(struct message_t));

	pthread_join(&thread, (void *) &temp1);
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

/**
 * function that iterates over the connections[] and finds the first
 * available position. Each fd in the array is marked with -1 if is empty
 */
int find_free_connection(struct pollfd *conn) {
	int free_index = -1;
	int k = 0;
	for (k = N_POS_NOT_FREE; k < MAX_SOCKETS; k++) {
		if (conn[k].fd == -1) {
			free_index = k;
			break;
		}
	}
	return free_index;
}

void *main_backup_server(void * argv) {
	//copy of argv
	char ** argv_backup = (char **) argv;
	struct sockaddr_in client;
	socklen_t size_client = sizeof(struct sockaddr_in);
	struct pollfd connections[MAX_SOCKETS];
	int listening_socket, result, i = 1, j = 1, l = 1;
	int number_clients = 0;

	if (ignsigpipe() != 0) {
		perror("ignsigpipe falhou");
		return -1;
	}

	//test argc
	if (argv_backup[2] == NULL) {
	printf("Use: ./table-server <Port> <size table>\n");
	printf("use exemple: ./table-server 54321 10\n");
	return NULL;
	}

	//listening socket up
	if ((listening_socket = make_server_socket(atoi(argv_backup[1]))) < 0) {
		return NULL;
	}
	//table_skel_init
	//init the table as a global variable in the table_skel
	if (table_skel_init(atoi(argv_backup[2])) == -1) {
		close(listening_socket);
		return NULL;
	}

	printf("***********************************\n");
	printf("*  SERVER BACKUP                  *\n");
	printf("***********************************\n\n");
	printf("Waiting for clients\n");

	//init each positions of connections[i].fd with -1
	for (i = 1; i < MAX_SOCKETS; i++) {
		connections[i].fd = -1;
	}

	connections[LISTENING_SOCKET_POS].fd = listening_socket;
	connections[LISTENING_SOCKET_POS].events = POLLIN;

	connections[STDIN_POS].fd = fileno(stdin);
	connections[STDIN_POS].events = POLLIN;

	while ((result = poll(connections, MAX_SOCKETS, TIMEOUT)) >= 0) {
		if (result > 0) {

			// listenning socket has a new connection
			if ((connections[LISTENING_SOCKET_POS].revents & POLLIN)
					&& ((number_clients - N_POS_NOT_FREE) < MAX_SOCKETS)) {

				int free_index = find_free_connection(connections);
				if (free_index != -1) {

					if ((connections[free_index].fd = accept(
							connections[LISTENING_SOCKET_POS].fd,
							(struct sockaddr *) &client, &size_client)) > 0) {
						connections[free_index].events = POLLIN;
						number_clients++;
						printf("Client connected %d\n\n", number_clients);
					} else {
						printf("there was some error with the accept\n");
					}
				}

				result--;
			}
			if (connections[STDIN_POS].revents & POLLIN) {
				char input[81];
				fgets(input, sizeof(input), stdin);
				input[strlen(input) - 1] = '\0';

				(strcmp(input, "print") == 0) ?
						print_status() : printf("Command Invalid\n");

				result--;
			}
			for (j = N_POS_NOT_FREE; j < MAX_SOCKETS && result > 0; j++) {
				//if socket has data to read
				if (connections[j].revents & POLLIN) {
					if (network_receive_send_backup(connections[j].fd) < 0) {
						close(connections[j].fd);
						number_clients--;
						connections[j].fd = -1;
						printf("A client has disconnect from the server\n");
						printf("The server has %d clients\n", number_clients);
					}
				}
			}
		}
	}
	table_skel_destroy();
	for (l = 0; l < MAX_SOCKETS; l++) {
		close(connections[l].fd);
	}
	return NULL;
}

int main(int argc, char **argv) {
	pthread_t thread_backup;
	struct sockaddr_in client;
	socklen_t size_client = sizeof(struct sockaddr_in);
	// struct of file descripters
	struct pollfd connections[MAX_SOCKETS];
	int listening_socket, result, i = 1, j = 1, l = 1;
	int number_clients = 0;

	if (argc == 3) {
		pthread_create(&thread_backup, NULL, main_backup_server, (void *) argv);
	}
	pthread_join(&thread_backup, NULL);
	//test argc

	if (argc != 4) {
		printf(
				"Uso: ./table-server <porta TCP> <dimensão da tabela> <IP:Port backup_sever>\n");
		printf("Exemplo de uso: ./table-server 54321 10 127.0.0.1:44445\n");
		return -1;
	}

	if (ignsigpipe() != 0) {
		perror("ignsigpipe falhou");
		return -1;
	}
	/*listening socket up*/
	if ((listening_socket = make_server_socket(atoi(argv[1]))) < 0) {
		return -1;
	}

	/*build a client socket to connecte to the backup server*/
	backup_server = network_connect(argv[3]);
	if (backup_server == NULL) {
		printf("The server is going without backup\n");
	}
	printf("The primary sever is connected to its backup\n");
	//table_skel_init
	//init the table as a global variable in the table_skel
	if (table_skel_init(atoi(argv[2])) == -1) {
		close(listening_socket);
		return -1;
	}

	printf("***********************************\n");
	printf("*  SERVER --  SUPPORTS %d CLIENTS  *\n",
	MAX_SOCKETS - N_POS_NOT_FREE);
	printf("***********************************\n\n");
	printf("Waiting for clients\n");

//init each positions of connections[i].fd with -1
	for (i = 1; i < MAX_SOCKETS; i++) {
		connections[i].fd = -1;
	}
//first position of connetions is the listening_socket
	connections[LISTENING_SOCKET_POS].fd = listening_socket;
// POLLIN ==> data to be read and in this case a new connections received
	connections[LISTENING_SOCKET_POS].events = POLLIN;

	connections[STDIN_POS].fd = fileno(stdin);
	connections[STDIN_POS].events = POLLIN;

	while ((result = poll(connections, MAX_SOCKETS, TIMEOUT)) >= 0) {
		if (result > 0) {

			// listenning socket has a new connection
			if ((connections[LISTENING_SOCKET_POS].revents & POLLIN)
					&& ((number_clients - N_POS_NOT_FREE) < MAX_SOCKETS)) {

				int free_index = find_free_connection(connections);

				//-1 there is no space in the array --> do not accept socket
				if (free_index != -1) {

					if ((connections[free_index].fd = accept(
							connections[LISTENING_SOCKET_POS].fd,
							(struct sockaddr *) &client, &size_client)) > 0) {
						connections[free_index].events = POLLIN;
						number_clients++;
						printf("Clients connected %d\n\n", number_clients);
					} else {
						printf("there was some error with the accept\n");
					}
				}

				result--;
			}
			if (connections[STDIN_POS].revents & POLLIN) {
				char input[81];
				fgets(input, sizeof(input), stdin);
				input[strlen(input) - 1] = '\0';

				(strcmp(input, "print") == 0) ?
						print_status() : printf("Command Invalid\n");

				result--;

			}
			for (j = N_POS_NOT_FREE; j < MAX_SOCKETS && result > 0; j++) {
				//if socket has data to read
				if (connections[j].revents & POLLIN) {
					if (network_receive_send(connections[j].fd) < 0) {
						close(connections[j].fd);
						number_clients--;
						connections[j].fd = -1;
						printf("A client has disconnect from the server\n");
						printf("The server has %d clients\n", number_clients);
					}
				}

			}
		}
	}
	table_skel_destroy();
	for (l = N_POS_NOT_FREE; l < MAX_SOCKETS; l++) {
		close(connections[l].fd);
	}
	return 0;
}

