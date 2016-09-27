#ifndef _LIST_H
#define _LIST_H

#include "entry.h"

typedef struct node_t {
	struct node_t *next;
	struct entry_t *entry;
} node;

typedef struct list_t {
	int size;
	struct node_t *head;
} list;
/**
 *create an empty node
 */
node *create_empty_node();
/**
 * function to create a node
 */
node *create_node(struct entry_t *entry);
/**
 * function to create a duplicate of the node
 */
node *dup_node(node *n);
/**
 * function to destroy a node and its content
 */
void destroy_node(node *n);
#endif
