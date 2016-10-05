#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <error.h>
#include <errno.h>
#include <assert.h>

#include "list-private.h"

int testListDestroy() {
	printf("Módulo list -> teste destrói lista:");
	list_destroy(NULL);
	printf(" passou\n");
	return 1;
}

int testListFreeKeys() {
	printf("Módulo list -> teste destrói keys:");
	list_free_keys(NULL);
	printf(" passou\n");
	return 1;
}

int testListaVazia() {
	struct list_t *list;
	int result;

	printf("Módulo list -> teste lista vazia:");

	if ((list = list_create()) == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	result = (list != NULL) && (list_size(list) == 0);

	list_destroy(list);
	
	printf(" %s\n", result ? "passou" : "não passou");

	return result;
}

int testAddCabeca() {
	int result;
	struct list_t *list;

	struct entry_t *entry;
	struct entry_t *entry2;
	struct data_t *data;

	printf("Módulo list -> teste adicionar cabeça:");

	if ((list = list_create()) == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	if ((data = data_create(5)) == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	if ((entry = entry_create("abc", data)) == NULL)
		error(1, errno, "  O teste não pode prosseguir");


	memcpy(entry->value->data, "abc1", 5);

	assert(list_add(NULL, entry) < 0);
	result = (list_add(NULL, entry) < 0);

	assert(list_add(list, NULL) < 0);
	result = result && (list_add(list, NULL) < 0);

	result = result && (list_add(list, entry) == 0);

	entry2 = list_get(list, "abc");

	result = result &&
		 entry2 != entry && 
                 list_size(list) == 1 &&
		 strcmp(entry->key, entry2->key) == 0;

	entry_destroy(entry);
	data_destroy(data);
	list_destroy(list);

	printf(" %s\n",result ? "passou" : "não passou");
	return result;
}

int testAddVarios() {
	int result,i,keysize;
	struct list_t *list;
	struct entry_t *entry[1024];
	struct entry_t *aux;
	struct data_t *data;
	char key[16];

	printf("Módulo list -> teste adicionar vários:");

	if ((list = list_create()) == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	for(i=0; i<1024; i++) {
		sprintf(key, "keyabc-%d",i);
		keysize = strlen(key) + 1;

		if ((data = data_create(keysize)) == NULL)
			error(1, errno, "  O teste não pode prosseguir");

		memcpy(data->data, key, keysize);
		if ((entry[i] = entry_create(key, data)) == NULL)
			error(1, errno, "  O teste não pode prosseguir");

		data_destroy(data);
		list_add(list, entry[i]);
	}

	assert(list_size(list) == 1024);
	result = (list_size(list) == 1024);

	for(i=0; i<1024; i++) {
		assert(list_get(list, entry[i]->key) != entry[i]);
		aux = list_get(list, entry[i]->key);
		result = result &&
			 (aux != entry[i]) &&
			 strcmp(aux->key, entry[i]->key) == 0;
		entry_destroy(entry[i]);
	}

	list_destroy(list);

	printf(" %s\n", result ? "passou" : "não passou");
	return result;
}


int testRemoveCabeca() {
	int result;
	struct list_t *list;
	struct entry_t *e1, *e2, *e3, *entry;
	struct data_t *data;

	printf("Módulo list -> teste remover cabeça:");

	if ((list = list_create()) == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	if ((data = data_create(5)) == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	e1 = entry_create("abc", data);
	e2 = entry_create("def", data);
	e3 = entry_create("ghi", data);

	if (e1 == NULL || e2 == NULL || e3 == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	memcpy(e1->value->data, "abc1", 5);
	memcpy(e2->value->data, "def1", 5);
	memcpy(e3->value->data, "ghi1", 5);

	list_add(list,e1);
	list_add(list,e2);
	list_add(list,e3);

	assert(list_remove(NULL, "ghi") < 0);
	result = (list_remove(NULL, "abc") < 0);

	assert(list_remove(list, NULL) < 0);
	result = result && (list_remove(list, NULL) < 0);

	result = result &&
		 list_remove(list, "ghi") == 0 &&
		 list_size(list) == 2;
 
	entry = list_get(list, "def");
	result = result &&
		 entry != e2 &&
		 strcmp(entry->key, e2->key) == 0;

	entry = list_get(list, "abc");
	result = result &&
		 entry != e1 &&
		 strcmp(entry->key, e1->key) == 0;

	list_destroy(list);
	entry_destroy(e1);
	entry_destroy(e2);
	entry_destroy(e3);
	data_destroy(data);

	printf(" %s\n", result ? "passou" : "não passou");
	return result;
}

int testRemoveCauda() {
	int result;
	struct list_t *list;
	struct entry_t *e1, *e2, *e3, *entry;
	struct data_t *data;

	printf("Módulo list -> teste remover cauda:");

	if ((list = list_create()) == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	if ((data = data_create(5)) == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	e1 = entry_create("abc", data);
	e2 = entry_create("def", data);
	e3 = entry_create("ghi", data);

	if (e1 == NULL || e2 == NULL || e3 == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	memcpy(e1->value->data, "abc1", 5);
	memcpy(e2->value->data, "def1", 5);
	memcpy(e3->value->data, "ghi1", 5);

	list_add(list,e1);
	list_add(list,e2);
	list_add(list,e3);

	result = list_remove(list, "abc") == 0 &&
		 list_size(list) == 2;

	entry = list_get(list, "def");
	result = result &&
		 entry != e2 &&
		 strcmp(entry->key, e2->key) == 0;

	entry = list_get(list, "ghi");
	result = result &&
		 entry != e3 &&
		 strcmp(entry->key, e3->key) == 0;

	list_destroy(list);
	entry_destroy(e1);
	entry_destroy(e2);
	entry_destroy(e3);
	data_destroy(data);

	printf(" %s\n", result ? "passou" : "não passou");
	return result;
}

int testRemoveMeio() {
	int result;
	struct list_t *list;
	struct entry_t *e1, *e2, *e3, *entry;
	struct data_t *data;

	printf("Módulo list -> teste remover meio:");

	if ((list = list_create()) == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	if ((data = data_create(5)) == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	e1 = entry_create("abc", data);
	e2 = entry_create("def", data);
	e3 = entry_create("ghi", data);

	if (e1 == NULL || e2 == NULL || e3 == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	memcpy(e1->value->data, "abc1", 5);
	memcpy(e2->value->data, "def1", 5);
	memcpy(e3->value->data, "ghi1", 5);

	list_add(list,e1);
	list_add(list,e2);
	list_add(list,e3);

	result = list_remove(list, "def") == 0 &&
		 list_size(list) == 2;

	entry = list_get(list, "ghi");
	result = result &&
		 entry != e3 &&
		 strcmp(entry->key, e3->key) == 0;

	entry = list_get(list, "abc");
	result = result &&
		 entry != e1 &&
		 strcmp(entry->key, e1->key) == 0;

	list_destroy(list);
	entry_destroy(e1);
	entry_destroy(e2);
	entry_destroy(e3);
	data_destroy(data);

	printf(" %s\n", result ? "passou" : "não passou");
	return result;
}

int testGetKeys() {
	int result;
	struct list_t *list;
	struct entry_t *e1, *e2, *e3;
	struct data_t *data;
	char **keys;

	printf("Módulo list -> teste busca chaves:");

	if ((list = list_create()) == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	if ((data = data_create(5)) == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	assert(list_get_keys(NULL) == NULL);
	result = (list_get_keys(NULL) == NULL);

	e1 = entry_create("abc", data);
	e2 = entry_create("def", data);
	e3 = entry_create("ghi", data);

	if (e1 == NULL || e2 == NULL || e3 == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	memcpy(e1->value->data, "abc1", 5);
	memcpy(e2->value->data, "def1", 5);
	memcpy(e3->value->data, "ghi1", 5);

	list_add(list,e1);
	list_add(list,e2);
	list_add(list,e3);

	if ((keys = list_get_keys(list)) == NULL)
		error(1, errno, "  O teste não pode prosseguir");

	result = strcmp(keys[0], e3->key) == 0 && keys[0] != e3->key &&
                 strcmp(keys[1], e2->key) == 0 && keys[1] != e2->key && 
                 strcmp(keys[2], e1->key) == 0 && keys[2] != e1->key && 
                 keys[3] == NULL;

	list_free_keys(keys);
	list_destroy(list);
	entry_destroy(e1);
	entry_destroy(e2);
	entry_destroy(e3);
	data_destroy(data);

	printf(" %s\n", result ? "passou" : "não passou");
	return result;
}

int main() {
	int score = 0;

	printf("\nIniciando o teste do módulo list\n\n");

	score += testListDestroy();

	score += testListFreeKeys();

	score += testListaVazia();

	score += testAddCabeca();

	score += testAddVarios();

	score += testRemoveCabeca();

	score += testRemoveCauda();

	score += testRemoveMeio();

	score += testGetKeys();

	printf("\nResultados do teste do modulo list: %d em 9\n",score);

	return score;
}
