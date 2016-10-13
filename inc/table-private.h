#ifndef _TABLE_PRIVATE_H
#define _TABLE_PRIVATE_H

#include "table.h"

struct table_t {
	struct list_t **buckets/* continuar definição */;
	int size; /* Dimensão da tabela */
	/*
	 * quantitie of keys that are in the system.
	 * each list has its on size and this value the sum of each sizes
	 * since the table_t has an array with size size of list_t*/
	int quantity_keys;
};

int key_hash(char *key, int l);

#endif
