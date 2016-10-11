#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "table-private.h"

int key_hash(char *key, int l) {
	/* Verificar se key é NULL */
	if (key == NULL || l <= 0)
		return -1;

	int soma, i, j = 0;
	int size_key = strlen(key); // strlen excludes the '/0'

	if (size_key <= 5) {
		while (strcmp(key[i], '\0') != 0) {
			soma += key[i];
			i++;
		}
	} else {
		for (i = 0; i < 3; i++) {
			soma += key[i];
		}
		for (j = size_key - 1; j < (size_key - 3); j--) {
			soma += key[j];
		}
	}
	return soma % l;
}

struct table_t *table_create(int n) {

	/* n tem valor válido? */
	if (n <= 0)
		return NULL;

	/* Alocar memória para struct table_t */
	struct table_t * new_table = (struct table_t *) malloc(sizeof(struct table_t));
	if(new_table == NULL)
		return NULL;

	/* Alocar memória para array de listas com n entradas
	 que ficará referenciado na struct table_t alocada.





	 Inicializar listas.

	 Inicializar atributos da tabela.
	 */

	return new_table;
}

void table_destroy(struct table_t *table) {

	/* table é NULL?

	 Libertar memória das listas.

	 Libertar memória da tabela.

	 */

}

int table_put(struct table_t *table, char * key, struct data_t *value) {

	/* Verificar valores de entrada */

	/* Criar entry com par chave/valor */

	/* Executar hash para determinar onde inserir a entry na tabela */

	/* Inserir entry na tabela */

}

int table_update(struct table_t *table, char * key, struct data_t *value) {

}

struct data_t *table_get(struct table_t *table, char * key) {

}

int table_del(struct table_t *table, char *key) {

}

/* Esta é dada! Ao estilo C! */
int table_size(struct table_t *table) {
return table == NULL ? -1 : table->/*atributo*/;
}

char **table_get_keys(struct table_t *table) {

}

void table_free_keys(char **keys) {

}

