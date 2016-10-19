#include "network_client-private.h"

#include <stdlib.h>

int write_all(int sock, char *buf, int len) {
}

int read_all(int sock, char *buf, int len) {
}

struct server_t *network_connect(const char *address_port) {

	struct server_t *server = malloc(sizeof(struct server_t));

	/* Verificar parâmetro da função e alocação de memória */

	/* Estabelecer ligação ao servidor:

	 Preencher estrutura struct sockaddr_in com dados do
	 endereço do servidor.

	 Criar a socket.

	 Estabelecer ligação.
	 */

	/* Se a ligação não foi estabelecida, retornar NULL */

	return server;
}

struct message_t *network_send_receive(struct server_t *server,
		struct message_t *msg) {
	char *message_out;
	char *message_in;
	int message_size, msg_size, result;
	struct message_t msg_resposta; // ISTO TEM DE SER UM message_t* e nao um message_t

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
		return 1;
	/* Terminar ligação ao servidor */

	/* Libertar memória */
}

