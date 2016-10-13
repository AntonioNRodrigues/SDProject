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
		while (key[i] != '\0') {
			soma += (int) key[i];
			i++;
		}
	} else {
		for (i = 0; i < 3; i++) {
			soma += (int) key[i];
		}
		for (j = size_key - 1; j < (size_key - 3); j--) {
			soma += (int) key[j];
		}
	}
	return soma % l;
}

struct table_t *table_create(int n) {

	/* n tem valor válido? */
	if (n <= 0)
		return NULL;

	/* Alocar memória para struct table_t */
	struct table_t * new_table = (struct table_t *) malloc(
			sizeof(struct table_t));
	if (new_table == NULL)
		return NULL;

	/* Alocar memória para array de listas com n entradas
	 que ficará referenciado na struct table_t alocada.
	 Inicializar listas.
	 Inicializar atributos da tabela.
	 */

	new_table->buckets = (struct list_t **) malloc(sizeof(struct list_t *) * n);
	if (new_table->buckets == NULL) {
		table_destroy(new_table);
		return NULL;
	}
	int i, j = 0;
	for (i = 0; i < n; i++) {
		new_table->buckets[i] = list_create();
		if (new_table->buckets[i]) {
			//revert cycle to freeSpace already allocated
			j = i - 1;
			while (j != 0) {
				list_destroy(new_table->buckets[j]);
				j -= 1;
			}
			table_destroy(new_table);
			return NULL;
		}
	}
	new_table->size = n;

	return new_table;
}

void table_destroy(struct table_t *table) {
	if (table != NULL) {
		int var = 0;
		for (var = 0; var < table->size; var++) {
			list_destroy(table->buckets[var]);
		}
		free(table->buckets);
		free(table);
	}
}

int table_put(struct table_t *table, char * key, struct data_t *value) {

	/* Verificar valores de entrada */
	if (table == NULL || key == NULL || value == NULL) {
		return -1;
	}
	/* Criar entry com par chave/valor */
	struct entry_t *new_entry = entry_create(strdup(key), data_dup(value));
	if (new_entry == NULL) {
		return -1;
	}
	/* Executar hash para determinar onde inserir a entry na tabela */
	int place_entry = key_hash(key, table->size);

	/* Inserir entry na tabela */
	int v = list_add(table->buckets[place_entry], new_entry);

	if (v == 0) {
		table->quantity_keys++;
	}
	return v;

}

int table_update(struct table_t *table, char * key, struct data_t *value) {
	//WRONG ------------------->
	return table_put(table, key, value);
}

struct data_t *table_get(struct table_t *table, char * key) {
	if (table == NULL || key == NULL)
		return NULL;

	return list_get(table->buckets[key_hash(key, table->size)], key)->value;
}

int table_del(struct table_t *table, char *key) {
	if (table == NULL || key == NULL)
		return -1;

	return list_remove(table->buckets[key_hash(key, table->size)], key);
}

/* Esta é dada! Ao estilo C! */
int table_size(struct table_t *table) {
	return table == NULL ? -1 : table->size;
}

char **table_get_keys(struct table_t *table) {
	if (table == NULL)
		return NULL;

	char **table_keys = (char **) malloc(sizeof(char *) * table->quantity_keys);
	if (table_keys == NULL)
		return NULL;
	int index_table, index_keys = 0;

	while (index_table < table->size) {
		int temp_size_list = table->buckets[index_table]->size;
		char ** temp_list = list_get_keys(table->buckets[index_table]);

		for (int i =0; i<temp_size_list; i++){
			table_keys[index_table] = temp_list[i];
			index_table++;
		}

	}
	return table_keys;
}

void table_free_keys(char **keys) {
	list_free_keys(keys);
}

