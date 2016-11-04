#include <stdlib.h>
#include <string.h>

#include "table_skel-private.h"

struct table_t *tabela;

int table_skel_init(int n_lists) {
	if (n_lists <= 0) {
		return -1;
	}
	tabela = table_create(n_lists);
	if (tabela == NULL) {
		return -1;
	}
	return 0;
}
int table_skel_destroy() {
	table_destroy(tabela);

	return tabela == NULL ? 0 : -1;
}

struct message_t *invoke(struct message_t *msg_in) {
	return NULL;
}
