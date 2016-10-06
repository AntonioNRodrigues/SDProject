#include "data.h"
#include "entry.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <assert.h>

/* Sem comentários! Não há tempo... */

int testCreate() {
	char *key = "123abc";
	char *data_s = "1234567890abc";
	struct data_t *value;
	struct entry_t *entry;
	int result;

	printf("Módulo entry -> teste entry_create:");

	if ((value = data_create2(strlen(data_s)+1, data_s)) == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	assert(entry_create(NULL, value) == NULL);
	result = (entry_create(NULL, value) == NULL);

	assert(entry_create(key, NULL) == NULL);
	result = result && (entry_create(key, NULL) == NULL);

	if ((entry = entry_create(key, value)) == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	result = result && (entry->key != key) && (entry->value != value)
			&& (strcmp(entry->key, key) == 0)
			&& (value->datasize == entry->value->datasize)
			&& (memcmp(value->data, entry->value->data, value->datasize) == 0);

	data_destroy(value);
	entry_destroy(entry);

	printf(" %s\n", result ? "passou" : "não passou");
	return result;
}

int testDup() {
	char *key = "123abc";
	char *data_s = "1234567890abc";
	struct data_t *value;
	struct entry_t *entry, *entry2;
	int result;

	printf("Módulo entry -> teste entry_dup:");

	if ((value = data_create2(strlen(data_s)+1, data_s)) == NULL)
		error(1, errno, "  O teste não pode prosseguir");
	if ((entry = entry_create(key, value)) == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	assert(entry_dup(NULL) == NULL);
	result = (entry_dup(NULL) == NULL);

	if ((entry2 = entry_dup(entry)) == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	result = result && (entry->key != entry2->key)
			&& (strcmp(entry->key, entry2->key) == 0)
			&& (entry->value != entry2->value)
			&& (entry->value->datasize == entry2->value->datasize)
			&& (memcmp(entry->value->data, entry2->value->data, entry->value->datasize) == 0);

	data_destroy(value);
	entry_destroy(entry);
	entry_destroy(entry2);

	printf(" %s\n", result ? "passou" : "não passou");
	return result;
}

int testDestroy() {
	entry_destroy(NULL);
	return 1;
}

int main() {
	int score = 0;

	printf("\nIniciando o teste do módulo entry\n");

	score += testCreate();

	score += testDup();

	printf("Módulo entry -> teste entry_destroy:");
	score += testDestroy();
	printf(" passou\n");

	printf("\nResultados do teste do módulo entry: %d em 3\n\n",score);
	return score;
}
