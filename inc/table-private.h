#ifndef _TABLE_PRIVATE_H
#define _TABLE_PRIVATE_H

#include "table.h"

struct table_t{
	struct list_t **buckets/* continuar definição */;
	int size; /* Dimensão da tabela */
};

int key_hash(char *key, int l);

#endif
