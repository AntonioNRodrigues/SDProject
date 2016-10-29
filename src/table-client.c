/*
 Programa cliente para manipular tabela de hash remota.
 Os comandos introduzido no programa não deverão exceder
 80 carateres.

 Uso: table-client <ip servidor>:<porta servidor>
 Exemplo de uso: ./table_client 10.101.148.144:54321
 */

#include "network_client-private.h"

int testInput(int argc, char ** argv) {
	if (argc != 2) {
		printf("Uso: table-client <ip servidor>:<porta servidor>\n");
		printf("Exemplo de uso: ./table_client 10.101.148.144:54321\n");
		return -1;
	}
	int n, i;
	char *token;
	token = strtok(strdup(argv[1]), ".:");

	n = atoi(token);
	for (i = 0; i < 4; i++) {

		if (n < 0 || n > 255) {
			printf("O endereço de IP nao e valido\n");
			return -1;
		}
		token = strtok(NULL, ".:");
		n = atoi(token);
	}
	if (n < 0 || n > 65535) {
		printf("A porta nao e valida\n");
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
	if (testInput(argc, argv) < 0) {
		return -1;
	}
	/* Usar network_connect para estabelcer ligação ao servidor */
	printf("A tentar estabelecer ligacao\n\n");
	server = network_connect(argv[1]);
	if (server == NULL)
		return -1;
	printf("ligacao estabelecida\n\n");
	/* Fazer ciclo até que o utilizador resolva fazer "quit" */

	printf("***********************************\n");
	printf("*            CLIENT               *\n");
	printf("***********************************\n\n");

	while (strcmp(input, "quit") != 0) {

		printf(">>> "); // Mostrar a prompt para inserção de comando

		/* Receber o comando introduzido pelo utilizador
		 Sugestão: usar fgets de stdio.h
		 Quando pressionamos enter para finalizar a entrada no
		 comando fgets, o carater \n é incluido antes do \0.
		 Convém retirar o \n substituindo-o por \0.
		 */
		fgets(input, sizeof(input), stdin);
		if (input[0] != '\n') {
			input[strlen(input) - 1] = '\0';

			/* Verificar se o comando foi "quit". Em caso afirmativo
			 não há mais nada a fazer a não ser terminar decentemente.
			 */
			token = strtok(input, " ");
			if (strcmp(token, "quit") == 0) {
				printf("Saiu\n");
			}
			/* Caso contrário:

			 Verificar qual o comando;

			 Preparar msg_out;

			 Usar network_send_receive para enviar msg_out para
			 o server e receber msg_resposta.
			 */
			//CASE SIZE
			else if (strcmp(token, "size") == 0) {
				msg_out = (struct message_t *) malloc(sizeof(struct message_t));
				msg_out->opcode = OC_SIZE;
				msg_out->c_type = CT_RESULT;
				msg_out->content.result = 0;

				msg_resposta = network_send_receive(server, msg_out);
				printf("Mensagem Enviada\n\n");
				print_msg(msg_out);
				free_message(msg_out);
				printf("Mensagem Recebida\n\n");
				if (msg_resposta == NULL) {
					printf("Nao houve resposta\n");
				} else {
					print_msg(msg_resposta);
					if (msg_resposta->opcode == OC_RT_ERROR) {
						printf("Houve um erro\n");
					} else {
						printf("resultado: %d\n", msg_resposta->content.result);
					}
					free_message(msg_resposta);
				}
				//CASE GET
			} else if (strcmp(token, "get") == 0) {
				token = strtok(NULL, " ");
				if (token == NULL) {
					printf("Uso: get <chave>\n");
				} else {
					msg_out = (struct message_t *) malloc(
							sizeof(struct message_t));
					msg_out->opcode = OC_GET;
					msg_out->c_type = CT_KEY;
					msg_out->content.key = strdup(token);

					msg_resposta = network_send_receive(server, msg_out);
					printf("Mensagem Enviada\n\n");
					print_msg(msg_out);
					free_message(msg_out);
					printf("Mensagem Recebida\n\n");
					if (msg_resposta == NULL) {
						printf("Nao houve resposta\n");
					} else {
						if (msg_resposta->opcode == OC_RT_ERROR) {
							printf("Tabela vazia ou outro erro\n");
						} else {
							if (msg_resposta->c_type == CT_VALUE) {
								printf("Valor: %s\n",
										msg_resposta->content.data->data);
							} else {
								printf("Chaves:\n");
								int i = 0;
								while (msg_resposta->content.keys[i] != NULL) {
									printf("Key = %s\n",
											msg_resposta->content.keys[i]);
									i++;
								}
							}
						}
						print_msg(msg_resposta);
						free_message(msg_resposta);
					}
				}
				//CASE DEL
			} else if (strcmp(token, "del") == 0) {

				token = strtok(NULL, " ");
				if (token == NULL) {
					printf("Uso: del <chave>\n");
				} else {
					msg_out = (struct message_t *) malloc(
							sizeof(struct message_t));
					msg_out->opcode = OC_DEL;
					msg_out->c_type = CT_KEY;
					msg_out->content.key = strdup(token);

					msg_resposta = network_send_receive(server, msg_out);
					printf("Mensagem Enviada\n\n");
					print_msg(msg_out);
					free_message(msg_out);
					printf("Mensagem Recebida\n\n");
					if (msg_resposta == NULL) {
						printf("Nao houve resposta\n");
					} else {
						if (msg_resposta->opcode == OC_RT_ERROR) {
							printf("Chave nao existe ou outro erro\n");
						} else {
							printf("resultado: %d\n",
									msg_resposta->content.result);
						}
						print_msg(msg_resposta);
						free_message(msg_resposta);
					}
				}
			}
			//CASE PUT
			else if (strcmp(token, "put") == 0) {

				token = strtok(NULL, " ");
				if (token == NULL) {
					printf("Uso: put <chave> <data>\n");
				} else {
					char key[sizeof(input)];
					strcpy(key, token);
					token = strtok(NULL, " ");
					if (token == NULL) {
						printf("Uso: put <chave> <data>\n");
					} else {
						char datastr[sizeof(input)];
						strcpy(datastr, token);
						token = strtok(NULL, " ");
						while (token != NULL) {
							strcat(datastr, " ");
							strcat(datastr, token);
							token = strtok(NULL, " ");
						}
						struct data_t *data;

						data = data_create2(strlen(datastr) + 1,
								datastr);

						msg_out = (struct message_t *) malloc(
								sizeof(struct message_t));
						msg_out->opcode = OC_PUT;
						msg_out->c_type = CT_ENTRY;
						msg_out->content.entry = entry_create(key,
								data);
						data_destroy(data);

						msg_resposta = network_send_receive(server, msg_out);
						printf("Mensagem Enviada\n\n");
						print_msg(msg_out);
						free_message(msg_out);
						printf("Mensagem Recebida\n\n");
						if (msg_resposta == NULL) {
							printf("Nao houve resposta\n");
						} else {
							if (msg_resposta->opcode == OC_RT_ERROR) {
								printf("Chave ja existe ou outro erro\n");
							} else {
								printf("resultado: %d\n",
										msg_resposta->content.result);
							}
							print_msg(msg_resposta);
							free_message(msg_resposta);
						}
					}
				}
				//CASE UPDATE
			} else if (strcmp(token, "update") == 0) {

				token = strtok(NULL, " ");
				if (token == NULL) {
					printf("Uso: update <chave> <data>\n");
				} else {
					char key[sizeof(input)];
					strcpy(key, token);
					token = strtok(NULL, " ");
					if (token == NULL) {
						printf("Uso: update <chave> <data>\n");
					} else {
						char datastr[sizeof(input)];
						strcpy(datastr, token);
						token = strtok(NULL, " ");
						while (token != NULL) {
							strcat(datastr, " ");
							strcat(datastr, token);
							token = strtok(NULL, " ");
						}
						struct data_t *data;

						data = data_create2(strlen(datastr) + 1, datastr);

						msg_out = (struct message_t *) malloc(
								sizeof(struct message_t));
						msg_out->opcode = OC_UPDATE;
						msg_out->c_type = CT_ENTRY;
						msg_out->content.entry = entry_create(key, data);
						data_destroy(data);

						msg_resposta = network_send_receive(server, msg_out);
						printf("Mensagem Enviada\n\n");
						print_msg(msg_out);
						free_message(msg_out);
						printf("Mensagem Recebida\n\n");
						if (msg_resposta == NULL) {
							printf("Nao houve resposta\n");
						} else {
							if (msg_resposta->opcode == OC_RT_ERROR){
								printf("Chave nao existe ou outro erro\n");
							} else {
							printf("resultado: %d\n",
									msg_resposta->content.result);
							}
							print_msg(msg_resposta);
							free_message(msg_resposta);
						}
					}
				}
			} else {
				printf("comando invalido\n");
			}
		}
	}
	return network_close(server);
}

