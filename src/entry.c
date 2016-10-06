/*
 * Grupo 33
 * Miguel Vale n.39279
 * António Rodrigues n.º40853
 * Ricardo Veloso n.º44842
 */

#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "entry.h"

/* Função que cria um novo par {chave, valor} (isto é, que inicializa
 * a estrutura e aloca a memória necessária).
 */
struct entry_t *entry_create(char *key, struct data_t *data) {
	if (key == NULL || data == NULL) {
		return NULL;
	}
	struct entry_t *newEntry = (struct entry_t *) malloc(
			sizeof(struct entry_t));
	if (newEntry == NULL) {
		data_destroy(data);
		free(key);
		return NULL;
	}
	newEntry->key = strdup(key);

	if (newEntry->key == NULL) {
		free(newEntry);
		// use entry_destroy instead of free;
	}
	newEntry->value = data_dup(data);

	if (newEntry->value == NULL) {
		free(newEntry->key);
		free(newEntry);
	}

	return newEntry;
}

/* Função que destrói um par {chave-valor} e liberta toda a memória.
 */
void entry_destroy(struct entry_t *entry) {
	if (entry != NULL) {
		data_destroy(entry->value);
		free(entry->key);
		free(entry);
	}
}

/* Função que duplica um par {chave, valor}.
 */
struct entry_t *entry_dup(struct entry_t *entry) {
	if (entry == NULL) {
		return NULL;
	}
	struct entry_t *entryDup = entry_create(entry->key, entry->value);
	if (entryDup == NULL) {
		free(entryDup->key);
		data_destroy(entryDup->value);
		free(entryDup);
		return NULL;
	}

	return entryDup;
}
