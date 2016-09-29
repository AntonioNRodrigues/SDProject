#include <stdlib.h>
#include <string.h>
#include "entry.h"
#include "list.h"
#include "list-private.h"

/* Cria uma nova lista. Em caso de erro, retorna NULL.
 */
struct list_t *list_create() {
	struct list_t *l = (struct list_t*) malloc(sizeof(struct list_t));
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
		struct node_t *temp;
		while (list->head != NULL) {
			temp = list->head;
			list->head = temp->next;
			destroy_node(temp);
		}
		free(list);
	}
}

/* Adiciona uma entry na lista. Como a lista deve ser ordenada, 
 * a nova entry deve ser colocada no local correto.
 * Retorna 0 (OK) ou -1 (erro)
 */
int list_add(struct list_t *list, struct entry_t *entry) {
	//if the list comes null return -1
	if (list == NULL) {
		return -1;
	}
	//create a new node with the entry
	struct node_t *new_node = create_node(entry);
	if (new_node == NULL) {
		return -1;
	}
	//if the list is empty put he node in the list
	if (list->head == NULL) {
		list->head = new_node;
		list->size = 1;
		return 0;
	} else {
		//pointer of node to the front of the list
		struct node_t *current = list->head;
		//if the next has only um element
		//pointer of node to the back of the list

		while (current != NULL) {
			//if the key is in already in the list
			if (strcmp(current->entry->key, new_node->entry->key) == 0) {
				//update the value of the list
				free(new_node);
				return -1;
			}
			//add the node in the front of the list
			if (strcmp(current->entry->key, new_node->entry->key) > 0) {
				new_node->next = current;
				list->head = new_node;
				list->size = list->size + 1;
				return 0;

			}
			//add in the bottom of the list
			if (current->next == NULL) {
				current->next = new_node;
				list->size = list->size + 1;
				return 0;
			}
			//is the key is the wright position insert
			if (strcmp(current->entry->key, new_node->entry->key) < 0
					&& strcmp(current->next->entry->key, new_node->entry->key)
							> 0) {
				struct node_t *temp = current->next;
				current->next = new_node;
				new_node->next = temp;
				list->size = list->size + 1;
				return 0;
			}
			//iterate over the list
			else {
				current = current->next;
			}
		}
	}
	return -1;
}

/* Elimina da lista um elemento com a chave key. 
 * Retorna 0 (OK) ou -1 (erro)
 */
int list_remove(struct list_t *list, char* key) {
	//if the list is empty or == NULL or or the key is NULL
	if (list == NULL || key == NULL) {
		return -1;
	}
	struct node_t *current = list->head;

	struct node_t *temp;

	//if the key is in the head
	if (strcmp(current->entry->key, key) == 0) {
		list->head = current->next;
		destroy_node(current);
		list->size = list->size -1;
		return 0;
	}
	// if the key its not in the head iterate over the list
	while (current != NULL) {
		if (current->next == NULL) {
			return -1;
		}
		if (strcmp(current->next->entry->key, key) == 0) {
			temp = current->next;
			current->next = temp->next;
			destroy_node(temp);
			list->size = list->size -1;
			return 0;
		}
		current = current->next;
	}
	return -1;

}

/* Obtem um elemento da lista que corresponda à chave key. 
 * Retorna a referência do elemento na lista (ou seja, uma alteração
 * implica alterar o elemento na lista). 
 */
struct entry_t *list_get(struct list_t *list, char *key) {
	if (list == NULL || key == NULL)
		return NULL;

	struct node_t *current_node = list->head;

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
	struct node_t* current_node = list->head;
	int i = 0;
	while (current_node != NULL) {
		list_keys[i] = strdup(current_node->entry->key);
		i++;
		printf("%s ", current_node->entry->key);
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
struct node_t *create_empty_node() {
	struct node_t *no = (struct node_t *) malloc(sizeof(struct node_t));
	no->entry = NULL;
	no->next = NULL;
	return no;
}

/**
 * function to create a node
 */
struct node_t *create_node(struct entry_t *entry) {
	if (entry == NULL) {
		return NULL;
	}
	struct node_t *no = (struct node_t *) malloc(sizeof(struct node_t));
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
struct node_t *dup_node(struct node_t *n) {
	if (n == NULL)
		return NULL;

	struct node_t *dupNode = create_node(n->entry);

	if (dupNode == NULL)
		return NULL;

	dupNode->next = n->next;

	return dupNode;
}
/**
 * function to destroy a node and its content
 */
void destroy_node(struct node_t *n) {
	if (n != NULL) {
		entry_destroy(n->entry);
		free(n);
	}
}
