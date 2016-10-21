/*
 Programa cliente para manipular tabela de hash remota.
 Os comandos introduzido no programa não deverão exceder
 80 carateres.

 Uso: table-client <ip servidor>:<porta servidor>
 Exemplo de uso: ./table_client 10.101.148.144:54321
 */

#include "network_client-private.h"

int testInput(int argc) {
	if (argc < 3) {
		printf("Uso: table-client <ip servidor>:<porta servidor>\n");
		printf("Exemplo de uso: ./table_client 10.101.148.144:54321\n");
		return -1;
	}
	return 0;
}

int main(int argc, char **argv) {
	struct server_t *server;
	char input[81];
	struct message_t *msg_out, *msg_resposta;
	char *token;

	/* Testar os argumentos de entrada */
	if (testInput(argc) < 0)
		return -1;

	/* Usar network_connect para estabelcer ligação ao servidor */
	strcpy(input, argv[1]);
	strcat(input, ":");
	strcat(input, argv[2]);
	server = network_connect(input);

	/* Fazer ciclo até que o utilizador resolva fazer "quit" */
	while (input != "quit") {

		printf(">>> "); // Mostrar a prompt para inserção de comando

		/* Receber o comando introduzido pelo utilizador
		 Sugestão: usar fgets de stdio.h
		 Quando pressionamos enter para finalizar a entrada no
		 comando fgets, o carater \n é incluido antes do \0.
		 Convém retirar o \n substituindo-o por \0.
		 */
		fgets(input, sizeof(input), stdin);
		input[strlen(input) - 1] = '\0';

		/* Verificar se o comando foi "quit". Em caso afirmativo
		 não há mais nada a fazer a não ser terminar decentemente.
		 */
		token = strtok(input, " ");
		switch (token) {
		case "quit":
			break;
			/* Caso contrário:

			 Verificar qual o comando;

			 Preparar msg_out;

			 Usar network_send_receive para enviar msg_out para
			 o server e receber msg_resposta.
			 */
		case "size":
			msg_out = (struct message_t *) malloc(sizeof(struct message_t));
			msg_out->opcode = OC_SIZE;

			msg_resposta = network_send_receive(server, msg_out);
			free(msg_out);
			break;
		case "get":
			token = strtok(NULL, " ");
			if (token == NULL) {
				printf("erro");
				break;
			}

			msg_out = (struct message_t *) malloc(sizeof(struct message_t));
			msg_out->opcode = OC_SIZE;
			msg_out->c_type = CT_KEY;
			msg_out->content.key = strdup(token);

			msg_resposta = network_send_receive(server, msg_out);
			free(msg_out);
			break;
		case "del":
			token = strtok(NULL, " ");
			if (token == NULL) {
				printf("erro");
				break;
			}

			msg_out = (struct message_t *) malloc(sizeof(struct message_t));
			msg_out->opcode = OC_DEL;
			msg_out->c_type = CT_KEY;
			msg_out->content.key = strdup(token);

			msg_resposta = network_send_receive(server, msg_out);
			free(msg_out);
			break;
		case "put":
			token = strtok(NULL, " ");
			if (token == NULL) {
				printf("error");
				break;
			}

			char key[sizeof(input)];
			strcpy(key, token);
			if (token == NULL) {
				printf("error");
				break;
			}
			char datastr[sizeof(input)];
			strcpy(datastr, token);
			token = strtok(NULL, " ");
			while (token != NULL) {
				strcat(datastr, "_");
				strcat(datastr, token);

				token = strtok(NULL, " ");
			}
			struct data_t *data;

			data = data_create2(strlen(datastr) + 1, datastr);

			msg_out = (struct message_t *) malloc(sizeof(struct message_t));
			msg_out->opcode = OC_PUT;
			msg_out->c_type = CT_ENTRY;
			msg_out->content.entry = entry_create(datastr, data);
			data_destroy(data);

			msg_resposta = network_send_receive(server, msg_out);
			free(msg_out);
			break;
		case "update":
			token = strtok(NULL, " ");
			if (token == NULL) {
				printf("error");
				break;
			}

			char key[sizeof(input)];
			strcpy(key, token);
			if (token == NULL) {
				printf("error");
				break;
			}
			char datastr[sizeof(input)];
			strcpy(datastr, token);
			token = strtok(NULL, " ");
			while (token != NULL) {
				strcat(datastr, "_");
				strcat(datastr, token);

				token = strtok(NULL, " ");
			}
			struct data_t *data;

			data = data_create2(strlen(datastr) + 1, datastr);

			msg_out = (struct message_t *) malloc(sizeof(struct message_t));
			msg_out->opcode = OC_UPDATE;
			msg_out->c_type = CT_ENTRY;
			msg_out->content.entry = entry_create(datastr, data);
			data_destroy(data);

			msg_resposta = network_send_receive(server, msg_out);
			free(msg_out);
			break;
		}

	}
	return network_close(server);
}

