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
	printf("TABLE CREATE\n");
	/* n tem valor válido? */
	if (n <= 0) {
		return NULL;
	}
	/* Alocar memória para struct table_t */
	struct table_t *new_table = (struct table_t *) malloc(
			sizeof(struct table_t));
	if (new_table == NULL)
		return NULL;

	/* Alocar memória para array de listas com n entradas
	 que ficará referenciado na struct table_t alocada.
	 Inicializar listas.
	 Inicializar atributos da tabela.
	 */

	new_table->buckets = (struct list_t **) malloc((sizeof(struct list_t *)) * n);

	if (new_table->buckets == NULL) {
		table_destroy(new_table);
		return NULL;
	}

	int i, j = 0;
	for (i = 0; i < n; i++) {
		new_table->buckets[i] = list_create();
		if (new_table->buckets[i]	== NULL) {
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
	new_table->quantity_entry = 0;

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
	printf("TABLE PUT :: %s \n", key);
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
	int resulted_value = list_add(table->buckets[place_entry], new_entry);

	if (resulted_value == 0) {
		table->quantity_entry++;
	}
	return resulted_value;

}

/* Função para substituir na tabela, o valor associado à chave key.
 * Os dados de entrada desta função deverão ser copiados.
 * Devolve 0 (OK) ou -1 (out of memory, outros erros)
 */
int table_update(struct table_t *table, char * key, struct data_t *value) {
	/* Verificar valores de entrada */
	if (table == NULL || key == NULL || value == NULL) {
		return -1;
	}
	struct data_t *new_data = data_dup(value);
	if (new_data == NULL)
		return -1;

	//index on the array os list_t to insert the entry
	int index_entry = key_hash(key, table->size);

	//entry to be updated
	struct entry_t *entry_to_update = list_get(table->buckets[index_entry],
			key);

	//the entry with this key does not exist
	if (entry_to_update == NULL) {
		free(new_data);
		return -1;
	}
	//destroy the old value
	data_destroy(entry_to_update->value);
	//update the value
	entry_to_update->value = data_dup(new_data);

	return 0;
}

struct data_t *table_get(struct table_t *table, char * key) {
	if (table == NULL || key == NULL)
		return NULL;
	//index of the entry
	int index_entry = key_hash(key, table->size);

	//get the entry
	struct entry_t *entry = list_get(table->buckets[index_entry], key);
	if (entry == NULL)
		return NULL;

	return data_dup(entry->value);
}
/* Função para remover um par chave valor da tabela, especificado
 * pela chave key, libertando a memória associada a esse par.
 * Devolve: 0 (OK), -1 (nenhum tuplo encontrado; outros erros)
 */
int table_del(struct table_t *table, char *key) {
	if (table == NULL || key == NULL)
		return -1;

	//index of the entry
	int index_entry = key_hash(key, table->size);
	//operation success 0 or error -1
	int ret_value = list_remove(table->buckets[index_entry], key);
	//update the number of entries in case of success
	if (ret_value == 0) {
		table->quantity_entry--;
	}
	return ret_value;
}

/* Esta é dada! Ao estilo C! */
/**
 * number of pairs key->value of the table
 */
int table_size(struct table_t *table) {
	return table == NULL ? -1 : table->quantity_entry;
}

char **table_get_keys(struct table_t *table) {
	if (table == NULL)
		return NULL;

	char **table_keys = (char **) malloc(
			sizeof(char *) * table->quantity_entry);
	if (table_keys == NULL)
		return NULL;

	int index_table = 0;
	int index_total = 0;

	//iterate over the table
	while (index_table < table->size) {
		//get the list in each index of the table
		char **temp = list_get_keys(table->buckets[index_table]);
		int index_temp = 0;
		// iterate over the list until the NULL
		while (temp != NULL) {
			table_keys[index_total] = strdup(temp[index_temp]);
			//the strdup failed
			if (table_keys[index_total] == NULL) {
				table_free_keys(table_keys);
				return NULL;
			}
			index_total++;
			index_temp++;
		}
		index_table++;

	}
	table_keys[index_total] = NULL;
	return table_keys;
}

void table_free_keys(char **keys) {
	list_free_keys(keys);
}

