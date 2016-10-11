#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "data.h"
#include "entry.h"
#include "table-private.h"

int testTableDestroy(){
	printf("Módulo table -> teste table destroy:");
	table_destroy(NULL);
	printf(" passou\n");
	return 1;
}

int testTabelaVazia() {
	struct table_t *table;
	int result;

	printf("Módulo table -> teste table vazia:");

	assert(table_create(0) == NULL);
	result = table_create(0) == NULL;

	result = result && 
		 (table = table_create(5)) != NULL &&
		 table_size(table) == 0;

	table_destroy(table);

	printf(" %s\n", result ? "passou" : "não passou");
	return result;
}

int testPutInexistente() {
	int result, i;
	struct table_t *table;
	char *key[1024];
	struct data_t *data[1024], *d;

	printf("Módulo table -> teste put inexistente:");

	table = table_create(5);

	for(i = 0; i < 1024; i++) {
		key[i] = (char*) malloc(16 * sizeof(char));
		sprintf(key[i], "a/key/b-%d", i);
		data[i] = data_create2(strlen(key[i]) + 1, key[i]);
		table_put(table, key[i], data[i]);
	}

	result = (table_size(table) == 1024);

	for(i=0; i<1024; i++) {
		d = table_get(table, key[i]);

		assert(d->datasize == data[i]->datasize);
		assert(memcmp(d->data, data[i]->data, d->datasize) == 0);
		assert(d->data != data[i]->data);

		result = result &&
			 d->datasize == data[i]->datasize &&
			 memcmp(d->data, data[i]->data, d->datasize) == 0 &&
 			 d->data != data[i]->data;

		data_destroy(d);
		data_destroy(data[i]);
		free(key[i]);
	}

	table_destroy(table);
	
	printf(" %s\n", result ? "passou" : "não passou");
	return result;
}

int testPutExistente() {
	int result, i;
	struct table_t *table;
	char *key[1024];
	struct data_t *data[1024], *data2[1024], *d;

	printf("Modulo table -> teste put existente:");

	table = table_create(5);

	for(i = 0; i < 1024; i++) {
		key[i] = (char*) malloc(16 * sizeof(char));
		sprintf(key[i], "a/key/b-%d", i);
		data[i] = data_create2(strlen(key[i]) + 1, key[i]);
		table_put(table, key[i], data[i]);
	}

	for(i=0; i<1024; i++) {
		data2[i] = data_create2(strlen(key[i]) + 1, key[i]);
		table_put(table, key[i], data[i]);
	}


	assert(table_size(table) == 1024);
	result = (table_size(table) == 1024);

	for(i = 0; i < 1024; i++) {
		free(key[i]);
		data_destroy(data[i]);
		data_destroy(data2[i]);
	}

	table_destroy(table);
	
	printf(" %s\n", result ? "passou" : "não passou");
	return result;
}

int testUpdate() {
	int result, i;
	struct table_t *table;
	char *key[1024];
	char *key2[1024];
	struct data_t *data[1024], *d;

	printf("Módulo table -> teste update:");
	table = table_create(5);

	for(i = 0; i < 1024; i++) {
		key[i] = (char*) malloc(13 * sizeof(char));
		sprintf(key[i], "a/key/b-%d", i);
		key2[i] = (char*) malloc(14 * sizeof(char));
		sprintf(key2[i], "a/key/bb-%d", i);
		data[i] = data_create2(strlen(key[i]) + 1, key[i]);
		table_put(table, key[i], data[i]);
		data_destroy(data[i]);
	}

	for(i = 0; i < 1024; i++) {
		data[i] = data_create2(strlen(key2[i]) + 1, key2[i]);
		table_update(table, key[i], data[i]);
	}

	assert(table_size(table) == 1024);
	result = result && (table_size(table) == 1024);

	for(i = 0; i < 1024; i++) {
		d = table_get(table, key[i]);
		
		result = result &&
			 d->datasize == data[i]->datasize &&
			 memcmp(d->data, data[i]->data, d->datasize) == 0 &&
			 d->data != data[i]->data;

		data_destroy(d);
		data_destroy(data[i]);
		free(key[i]);
		free(key2[i]);
	}

	table_destroy(table);	
	
	printf(" %s\n", result ? "passou" : "não passou");
	return result;
}

int testDelInexistente() {
	int result, i;
	struct table_t *table;
	char *key;
	struct data_t *data;

	printf("Módulo table -> teste del inexistente:");

	table = table_create(7);

	for(i = 0; i < 1024; i++) {
		key = (char*) malloc(16 * sizeof(char));
		sprintf(key, "a/key/b-%d", i);
		data = data_create2(strlen(key)+1, key);

		table_put(table, key, data);

		free(key);
		data_destroy(data);
	}

	assert(table_size(table) == 1024);
	result = (table_size(table) == 1024);

	result = result &&
		 (table_del(table, "a/key/b-1024") < 0) &&
		 (table_del(table, "abc") < 0);

	assert(table_size(table) == 1024);
	result = result && (table_size(table) == 1024);

	table_destroy(table);
	
	printf(" %s\n", result ? "passou" : "não passou");
	return result;
}

int testDelExistente() {
	int result, i;
	struct table_t *table;
	char *key;
	struct data_t *data, *data2;

	printf("Módulo table -> teste del existente:");

	table = table_create(7);

	for(i=0; i<1024; i++) {
		key = (char*)malloc(16*sizeof(char));
		sprintf(key,"a/key/b-%d",i);
		data = data_create2(strlen(key)+1,key);

		table_put(table,key,data);

		data_destroy(data);
		free(key);
	}

	assert(table_size(table) == 1024);
	result = (table_size(table) == 1024);

	result = result &&
		 ((data = table_get(table,"a/key/b-1023")) != NULL) &&
		 ((data2 = table_get(table,"a/key/b-45")) != NULL);

	data_destroy(data);
	data_destroy(data2);

	result = result &&
		 (table_del(table,"a/key/b-1023") == 0) &&
		 (table_del(table,"a/key/b-45") == 0);

	result = result &&
		 (table_get(table,"a/key/b-1023") == NULL) &&
		 (table_get(table,"a/key/b-45") == NULL);

	assert(table_size(table) == 1022);
	result = result && (table_size(table) == 1022);

	table_destroy(table);
	
	printf(" %s\n", result ? "passou" : "não passou");
	return result;
}

int testGetKeys() {
	int result = 1, i, j, achou;
	struct table_t *table;
	char **keys;
	char *k[4] = {"abc","bcd","cde","def"};
	struct data_t *d;

	printf("Módulo table -> teste sacar chaves:");

	table = table_create(2);
	d = data_create(5);

	table_put(table, k[3], d);
	table_put(table, k[2], d);
	table_put(table, k[1], d);
	table_put(table, k[0], d);

	data_destroy(d);

	keys = table_get_keys(table);
	
	for(i = 0; keys[i] != NULL; i++) {
		achou = 0;
		for(j = 0; j < 4; j++)
			achou = (achou || (strcmp(keys[i], k[j]) == 0));
		result = (result && achou);
	}

	result = result &&
		 (table_size(table) == i);

	table_free_keys(keys);

	table_destroy(table);

	printf(" %s\n", result ? "passou" : "não passou");
	return result;
}

int main() {
	int score = 0;

	printf("\nIniciando o teste do módulo table\n");

	score += testTableDestroy();

	score += testTabelaVazia();

	score += testPutInexistente();

	score += testPutExistente();

	score += testUpdate();

	score += testDelInexistente();

	score += testDelExistente();

	score += testGetKeys();

	printf("Resultados do teste do módulo table: %d em 8\n\n", score);

	return score;
}
