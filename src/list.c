#include <stdlib.h>
#include <string.h>
#include "entry.h"

/* Cria uma nova lista. Em caso de erro, retorna NULL.
 */
struct list *list_create() {
	list *l = (list *) malloc(sizeof(list));
	if (l == NULL)
		return NULL;
	l->head = NULL;
	l->size = 0;

	return l;
}

/* Elimina uma lista, libertando *toda* a memoria utilizada pela
 * lista.
 */
void list_destroy(struct list_t *list) {
	if (list != NULL) {
		//node pointer to a tempNode
		node *temp;
		while (list->head != NULL) {
			temp = list->head;
			list->head = temp->next;
			destroyNode(temp);
		}
		free(list);
	}
}

/* Adiciona uma entry na lista. Como a lista deve ser ordenada, 
 * a nova entry deve ser colocada no local correto.
 * Retorna 0 (OK) ou -1 (erro)
 */
int list_add(struct list_t *list, struct entry_t *entry);

/* Elimina da lista um elemento com a chave key. 
 * Retorna 0 (OK) ou -1 (erro)
 */
int list_remove(struct list_t *list, char* key);

/* Obtem um elemento da lista que corresponda à chave key. 
 * Retorna a referência do elemento na lista (ou seja, uma alteração
 * implica alterar o elemento na lista). 
 * --->SE NAO ESTIVER PRESENTE O QUE DEVOLVE!!!!!!!!!<------
 */
struct entry_t *list_get(struct list_t *list, char *key) {
	if (list == NULL || key == NULL)
		return NULL;

	node *current_node = list->head;

	while (current_node != NULL) {
		if (strcmp(current_node->entry->key, key) == 0) {
			return current_node->entry;
		}
		current_node = current_node->next;
	}
	return NULL;

}
/* Retorna o tamanho (numero de elementos) da lista 
 * Retorna -1 em caso de erro.  */
int list_size(struct list_t *list) {
	return (list == NULL) ? -1 : list->size;
}

/* Devolve um array de char * com a cópia de todas as keys da 
 * tabela, e um último elemento a NULL.
 */
char **list_get_keys(struct list_t *list) {
	if (list == NULL || list->head == NULL) {
		return NULL;
	}
	char **list_keys = (char **) malloc(sizeof(char *) * (list->size + 1));
	node * current_node = list->head;
	int i = 0;
	while (current_node != NULL) {
		list_keys[i] = strdup(current_node->entry->key);
		i++;

		//revert cycle to delete all memory if strdup fails

		current_node = current_node->next;
	}
	list_keys[i + 1] = NULL;
	return list_keys;
}

/* Liberta a memoria reservada por list_get_keys.
 */
void list_free_keys(char **keys) {
	if (keys != NULL) {
		int i = 0;
		while (keys[i] != NULL) {
			free(keys[i]);
			i++;
		}
		free(keys);
	}

}

/**
 *
 */
node *create_empty_node() {
	node *no = (node *) malloc(sizeof(node));
	no->entry = NULL;
	no->next = NULL;
	return no;
}

/**
 * function to create a node
 */
node *create_node(struct entry_t *entry) {
	if (entry == NULL) {
		return NULL;
	}
	node *no = (node *) malloc(sizeof(node));
	if (no == NULL) {
		return NULL;
	}
	no->entry = entry_dup(entry);
	no->next = NULL;
	return no;
}
/**
 * function to create a duplicate of the node
 */
node *dup_node(node *n) {
	if (n == NULL)
		return NULL;

	node *dupNode = create_node(n->entry);

	if (dupNode == NULL)
		return NULL;

	dupNode->next = n->next;

	return dupNode;
}
/**
 * function to destroy a node and its content
 */
void destroy_node(node *n) {
	if (n != NULL) {
		entry_destroy(n->entry);
		free(n);
	}
}
