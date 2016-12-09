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
#define PORT_PRIM 44444
#define UP 1
#define DOWN 0
#define NONE -1
#define PRIMARY 1
#define SECUNDARY 2
#include <stdio.h>
#include <error.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "inet.h"
#include "network_client-private.h"
#include "table_skel-private.h"
#include "primary_backup.h"

struct shared_t {
	struct server_t *current_backup;
	char *ip_secundary;
	char *ip_primary;
	char *port_secundary;
	char *port_primary;

};

struct server_t *server;
struct shared_t shared;
int bit_control = 0;
int state = NONE;
int status;
pthread_mutex_t dados = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t dados_dispo = PTHREAD_COND_INITIALIZER;

int ignsigpipe() {
	struct sigaction s;
	s.sa_handler = SIG_IGN;
	return sigaction(SIGPIPE, &s, NULL);
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

/* Função "inversa" da função network_send_receive usada no table-client.
 Neste caso a função implementa um ciclo receive/send:

 Recebe um pedido;
 Aplica o pedido na tabela;
 Envia a resposta.
 this message returns 1 (OK) and -1 (NOK)
 */
int network_receive_send(int sockfd) {
	char *message_resposta, *message_pedido;
	int message_size, msg_size, result;
	struct message_t *msg_pedido, *msg_resposta, *msg_from_secundary;

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

	if (msg_pedido->opcode == OC_STATUS) {
		msg_resposta->opcode = OC_STATUS;
		msg_resposta->c_type = CT_RESULT;
		msg_resposta->content.result = status;
		print_msg(msg_resposta);
	}

	if (msg_pedido->opcode == OC_UP) {
		shared.current_backup = network_connect(msg_pedido->content.key);
		if (shared.current_backup == NULL) {
			printf(
					"The secundary could not establish connection to the primary\n");
		}
		printf("The secundary is back on\n");
		state = UP;
	}

	/*in this stage the if the bitcontrol == 0 the primary already
	 * passed here and the secundary needs to run this
	 * in the end change the bitcontrol to 1.*/
	//pthread_mutex_lock(&dados);
	//if (bit_control == 0) {
	if (msg_pedido->opcode == OC_DEL || msg_pedido->opcode == OC_PUT
			|| msg_pedido->opcode == OC_UPDATE) {
		//only runs if the opcode is impar, in this case the message has been to the primary
		//and has to be sent to the secudary
		if (state == UP && status == PRIMARY) {
			//msg_pedido->opcode += 1;
			msg_from_secundary = network_send_receive(shared.current_backup,
					msg_pedido);
		
		//in the first time the message from the server is null mark the state of the secundary as DOWN
		if (msg_from_secundary == NULL && state != DOWN) {
			printf("The secundary is down\n");
			state = DOWN;
			printf("State = %d Status = \n", state);
		}
		}
	}

	//	bit_control = 1;
	//pthread_cond_signal(&dados_dispo);
	//pthread_mutex_unlock(&dados);
	//}

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

void write_to_file(char *name_file, char *str) {
	char port[100];
	sprintf(port, "%d", PORT_PRIM);

	FILE *f = fopen(name_file, "w");
	if (f == NULL) {
		perror("File failed to open");
	}
	fprintf(f, "%s", strcat(strcat(str, ":"), port));
	fclose(f);
	printf("File with ip_port of client has been written\n");

}
char * read_from_file(char *name_file) {
	char c[1000];
	FILE *f = fopen(name_file, "r");
	if (f == NULL) {
		perror("File failed to open");
		return NULL;
	}
	fscanf(f, "%[^\n]", c);
	printf("%s\n", c);
	fclose(f);
	return *c;
}

int file_exists(char *name_file) {
	FILE *f;
	if ((f = fopen(name_file, "r"))) {
		fclose(f);
		return 1;
	}
	return 0;
}
hello_again(struct server_t *server, char *ip_port) {
	if (server == NULL) {
		printf("the message was not send, The server is down\n");
		return -1;
	}
	struct message_t *msg_out = (struct message_t *) malloc(
			sizeof(struct message_t));
	if (msg_out == NULL)
		return -1;
	msg_out->opcode = OC_UP;
	msg_out->c_type = CT_KEY;
	msg_out->content.key = strdup(ip_port);
	struct message_t *tt = network_send_receive(server, msg_out);
	if (tt == NULL) {
		return -1;
	}
	printf("the message was send\n");
	network_close(server);
	return 0;
}
hello_special(struct server_t *server) {
	if (server == NULL) {
		printf("the message was not send, The server is down\n");
		return -1;
	}
	struct message_t *msg_out = (struct message_t *) malloc(
			sizeof(struct message_t));
	if (msg_out == NULL)
		return -1;
	msg_out->opcode = OC_STATUS;
	msg_out->c_type = CT_RESULT;
	msg_out->content.result = -100;
	struct message_t *tt = network_send_receive(server, msg_out);
	if (tt == NULL) {
		return -1;
	}
	printf("the message of OC_STATUS was send\n");
	network_close(server);
	return tt->content.result;
}
/**
 * function to run when the creation of a thread for the secundary
 */
void *main_secundary(void * argv) {
	char ** argv_backup = (char **) argv;
	// build a server to be the client of the secundary
	shared.current_backup = network_connect(argv_backup[3]);

	while (1) {
		pthread_mutex_lock(&dados);
		/* while bit_control is not 0 --> in the case of zero the secundary is "active"*/
		while (bit_control != 0)
			//waits for dados to be available
			pthread_cond_wait(&dados_dispo, &dados);
		bit_control = 0;
		/*all done free mutex*/
		pthread_mutex_unlock(&dados);
		if (bit_control != 0)
			break;
	}
	return NULL;
}
void *main_secundary2(void * argv) {
	char ** argv_backup = (char **) argv;

	while (1) {
		pthread_mutex_lock(&dados);
		/* while bit_control is not 0 --> in the case of zero the secundary is "active"*/
		while (bit_control != 0)
			//waits for dados to be available
			pthread_cond_wait(&dados_dispo, &dados);
		bit_control = 0;
		/*all done free mutex*/
		pthread_mutex_unlock(&dados);
		if (bit_control != 0)
			break;
	}
	return NULL;
}

/**
 *
 */
void create_thread(char **argv) {
	pthread_t thread_sec;
	if (pthread_create(&thread_sec, NULL, main_secundary, (void *) argv) != 0) {
		perror("Thread failed");
		exit(EXIT_FAILURE);
	}
}

void create_thread2(char **argv) {
	pthread_t thread_sec;
	if (pthread_create(&thread_sec, NULL, main_secundary2, (void *) argv)
			!= 0) {
		perror("Thread failed");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv) {
	struct sockaddr_in client;
	socklen_t size_client = sizeof(struct sockaddr_in);
	struct pollfd connections[MAX_SOCKETS];
	int listening_socket, result, i = 1, j = 1, l = 1;
	int number_clients = 0;

	/*if (argc != 3 || argc != 4) {
	 printf(
	 "Use for primary: ./table-server <port TCP> <size table> <IP:Port backup_sever>\n");
	 printf("use ex: ./table-server 54321 10 127.0.0.1:44445\n");

	 printf("Use for secundary: ./table-server <port TCP> <size table> \n");
	 printf("use ex: ./table-server 54321 10 \n");

	 exit(EXIT_FAILURE);
	 }*/

	/*listening socket up*/
	if ((listening_socket = make_server_socket(atoi(argv[1]))) < 0) {
		return -1;
	}

	if (table_skel_init(atoi(argv[2])) == -1) {
		close(listening_socket);
		return -1;
	}

	//its primary passing here
	if (argc == 4) {
		struct server_t *temp_client_s = network_connect(argv[3]);
		int v = hello_special(temp_client_s);
		printf("status %d\n", v);
		if (v == PRIMARY) {
			temp_client_s = network_connect(argv[3]);
			update_state(temp_client_s);
			hello_again(temp_client_s, "127.0.0.1:44444");
			create_thread2(argv);
		} else {

			printf("STATE %d\n", state);

			create_thread(argv);
			status = PRIMARY;
			state = UP;

		}
	}

	if (ignsigpipe() != 0) {
		perror("ignsigpipe falhou");
		return -1;
	}

	if (argc == 3) {
		if (file_exists("secundary") == 1) {
			char address[100];
			FILE *f = fopen("secundary", "r");
			fscanf(f, "%[^\n]", address);
			fclose(f);
			//connect from the secundary to the primary to update its state
			struct server_t *temp_client_s = network_connect(address);
			if (temp_client_s == NULL) {
				printf("Connection failed\n");
			} else {
				printf("The Secundary is client of the primary\n");
				update_state(temp_client_s);
				hello(temp_client_s);
			}
		}
		status = SECUNDARY;
		state = UP;

	}

	printf("***********************************\n");
	printf("*  SERVER -  SUPPORTS %d CLIENTS  *\n",
	MAX_SOCKETS - N_POS_NOT_FREE);
	printf("***********************************\n\n");
	printf("Waiting for clients\n");

	/*init each positions of connections[i].fd with -1*/
	for (i = 1; i < MAX_SOCKETS; i++) {
		connections[i].fd = -1;
	}
	/*first position of connetions is the listening_socket*/
	connections[LISTENING_SOCKET_POS].fd = listening_socket;
	/* POLLIN ==> data to be read and in this case a new connections received*/
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
						//get the IP
						char str[INET_ADDRSTRLEN];
						inet_ntop(AF_INET, &(client.sin_addr), str,
						INET_ADDRSTRLEN);
						/*only writes the ip of the primary so in case of crash of the secundary
						 * we can make a call to the primary to update the state of the table in the secundary*/
						if (argc == 3) {
							write_to_file("secundary", str);
						}

						printf(
								"Client on IP = %s connected.\nIt's client number %d\n\n",
								str, number_clients);
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
					//printf("----POLLIN %d\n", bit_control);

					if (network_receive_send(connections[j].fd) < 0) {
						close(connections[j].fd);
						number_clients--;
						connections[j].fd = -1;
						printf("A client has disconnect from the server\n");
						printf("The server has %d clients\n", number_clients);
						//printf("%d\", status)
						/* Since a secundary server only has the primary server as client
						 * a disconnection means the primary server is down*/
						if (status == SECUNDARY){
							status = PRIMARY;
							state = DOWN;
							printf("Primary server is offline, status switched to primary\n");
						}
					}
//					pthread_mutex_lock(&dados);
					//			if (bit_control == 0) {
					//printf("BEF:: bit == %d\n", bit_control);
					//			bit_control = 1;
					//printf("AFT:: bit == %d\n", bit_control);
					//		} else if (bit_control == 1) {
					//printf("BEF:: bit == %d\n", bit_control);
					//		bit_control = 0;
					//printf("AFT:: bit == %d\n", bit_control);
				}
				//pthread_cond_signal(&dados_dispo);
				//printf("----POLLIN %d\n", bit_control);
				//	pthread_mutex_unlock(&dados);
				//}

			}
		}
	}
	table_skel_destroy();
	for (l = N_POS_NOT_FREE; l < MAX_SOCKETS; l++) {
		close(connections[l].fd);
	}
	return 0;
}

