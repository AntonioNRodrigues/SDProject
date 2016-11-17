/*
 * Grupo 33
 * Miguel Vale n.39279
 * António Rodrigues n.º40853
 * Ricardo Veloso n.º44842
 */

/*
 Programa cliente para manipular tabela de hash remota.
 Os comandos introduzido no programa não deverão exceder
 80 carateres.

 Uso: table-client <ip servidor>:<porta servidor>
 Exemplo de uso: ./table_client 10.101.148.144:54321
 */

#include "network_client-private.h"
#include "client_stub-private.h"

/*
 * Checks if command line arguments are a valid
 */
int testArgs(int argc, char ** argv) {
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
	struct rtable_t *remote_table;
	char input[81];
	char *token;
	/* Testar os argumentos de entrada */
	if (testArgs(argc, argv) < 0) {
		return -1;
	}
	/* Usar network_connect para estabelecer ligação ao servidor */
	printf("A tentar estabelecer ligacao\n\n");
	remote_table = rtable_bind(argv[1]);

	if (remote_table->server == NULL) {
		free(remote_table);
		printf("Remote server is down\n");
		return -1;
	}
	printf("ligacao estabelecida\n\n");
	/* Fazer ciclo até que o utilizador resolva fazer "quit" */

	printf("***********************************\n");
	printf("*            CLIENT               *\n");
	printf("***********************************\n\n");

	while (strcmp(input, "quit") != 0) {

		printf(">>> "); // Mostrar a prompt para inserção de comando

		/* Receber o comando introduzido pelo utilizador
		 */
		fgets(input, sizeof(input), stdin);

		/*
		 * Just in case no actual input is received
		 * the input prompt is shown again
		 */
		if (input[0] != '\n') {
			input[strlen(input) - 1] = '\0';

			/* Verificar se o comando foi "quit". Em caso afirmativo
			 não há mais nada a fazer a não ser terminar decentemente.
			 */
			token = strtok(input, " ");
			//CASE QUIT
			if (strcmp(token, "quit") == 0) {
				printf("A terminar\n");

				/* Verificar qual o comando; */

				//CASE SIZE
			} else if (strcmp(token, "size") == 0) {

				rtable_size(remote_table);

				//CASE GET
			} else if (strcmp(token, "get") == 0) {

				token = strtok(NULL, " ");
				if (token == NULL) {
					printf("Uso: get <chave>\n");
				}
				else if (strcmp(token, "!") == 0) {
					rtable_get_keys(remote_table);
				} else {
					rtable_get(remote_table, token);
				}
				//CASE DEL

			} else if (strcmp(token, "del") == 0) {

				token = strtok(NULL, " ");
				if (token == NULL) {
					printf("Uso: del <chave>\n");
				} else {
					rtable_del(remote_table, token);
				}
				//CASE PUT
			} else if (strcmp(token, "put") == 0) {

				token = strtok(NULL, " ");
				if (token == NULL) {
					printf("Uso: put <chave> <dados>\n");
				} else {
					char key[sizeof(input)];
					strcpy(key, token);
					token = strtok(NULL, " ");
					if (token == NULL) {
						printf("Uso: put <chave> <dados>\n");
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

						rtable_put(remote_table, key, data);

						data_destroy(data);
					}
				}
				//CASE UPDATE
			} else if (strcmp(token, "update") == 0) {

				token = strtok(NULL, " ");
				if (token == NULL) {
					printf("Uso: update <chave> <dados>\n");
				} else {
					char key[sizeof(input)];
					strcpy(key, token);
					token = strtok(NULL, " ");
					if (token == NULL) {
						printf("Uso: update <chave> <dados>\n");
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

						rtable_update(remote_table, key, data);

						data_destroy(data);
					}
				}
			} else {
				printf("Comando invalido\n");
			}
		}
	}
	return rtable_unbind(remote_table);
}

