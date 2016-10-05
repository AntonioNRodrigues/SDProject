#ifndef _LISTPRIVATE_H
#define _LISTPRIVATE_H

#include "entry.h"
#include "list.h"
#include "data.h"

struct node_t {
	struct node_t *next;
	struct entry_t *entry;
};

struct list_t {
	int size;
	struct node_t *head;
};
/**
 * function to create a node
 */
struct node_t *create_node(struct entry_t *entry);
/**
 * function to create a duplicate of the node
 */
struct node_t *dup_node(struct node_t *n);
/**
 * function to destroy a node and its content
 */
void destroy_node(struct node_t *n);

#endif
