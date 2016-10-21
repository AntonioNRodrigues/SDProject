#include "network_client-private.h"

#include <stdlib.h>

int write_all(int sock, char *buf, int len) {
	int nbytes_write = len;
	int msg;
	while (len > 0){
		msg = write(sock, buf, len);
		if(msg == -1){
			if(errno==EINTR) continue;
			prinf("%s \n", "Write failed");
			return msg;
		}
		buf += msg;
		len -= msg;
	}
	return nbytes_write;
}

int read_all(int sock, char *buf, int len) {
	int nbytes_read = len;
	int msg;
	while(len > 0){
		msg = read(sock, buf, len);
		if(msg == 0){
			return 0;
		}
		if(msg == -1){
			if(errno==EINTR) continue;
			printf("%s \n", "Read failed");
			return msg;
		}
		buf += msg;
		len -= msg;
	}
	return nbytes_read;

}

struct server_t *network_connect(const char *address_port) {

	struct server_t *server = malloc(sizeof(struct server_t));

	/* Verificar parâmetro da função e alocação de memória */
	if(address_port == NULL || server == NULL){
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
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        perror("Erro ao criar socket");
        return NULL;
    }
	
	//Preenche estrutura struct sockaddr_in server com dados do endereço do servidor
	server.sin_family = AF_INET;
    server.sin_port = htons(atoi(token2); //Porta TCP
    if (inet_pton(AF_INET, token1, &server.sin_addr) < 1) {
		printf("Erro ao converter IP\n");
		close(server->sock_file_descriptor);
		return NULL;
	}
	
	/* Estabelece a ligação */
	/* Se a ligação não foi estabelecida, retornar NULL */

	if (connect(server->sock_file_descriptor,(struct sockaddr *)&server, sizeof(server)) < 0) {
		free(server);
		perror("Erro ao conectar-se ao servidor");
		close(server->sock_file_descriptor);
		return NULL;
	}

	return server;
}

struct message_t *network_send_receive(struct server_t *server,
		struct message_t *msg) {
	char *message_out;
	char *message_in;
	int message_size, msg_size, result;
	struct message_t *msg_resposta; // ISTO TEM DE SER UM message_t* e nao um message_t

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
		return NULL;
	}

	/* Enviar a mensagem que foi previamente serializada */
	result = write_all(server->sock_file_descriptor, message_out, message_size);

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
	int size_returned_msg;
	result = read_all(server->sock_file_descriptor, &size_returned_msg, _INT);
	if (size_returned_msg <= 0) {
		//free
		return NULL;
	}
	message_in = (char *) malloc(size_returned_msg);

	result = read_all(server->sock_file_descriptor, message_in, size_returned_msg);

	/* Desserializar a mensagem de resposta */
	msg_resposta = buffer_to_message(message_in, size_returned_msg);

	/* Verificar se a desserialização teve sucesso */
	if(msg_resposta == NULL){
		return NULL;
	}

	/* Libertar memória */
	//-------------------------TO DO-------------------------------->
	return msg_resposta;
}

int network_close(struct server_t *server) {
	/* Verificar parâmetros de entrada */
	if(server == NULL)
		return -1; //--------------------->DEVE RETORNAR O QUE?---------------1
	/* Terminar ligação ao servidor */
	int result = close(server->sock_file_descriptor);
	free(server);
	return result;
}

