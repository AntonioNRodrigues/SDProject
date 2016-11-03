#include <stdlib.h>
#include <string.h>
#include "client_stub-private.h"

struct rtable_t *rtable_bind(const char *address_port) {
	if (address_port == NULL) {
		return NULL;
	}
	struct rtable_t *remote_table = (struct rtable_t*) malloc(
			sizeof(struct rtable_t));
	if (remote_table == NULL) {
		return NULL;
	}
	remote_table->server = network_connect(address_port);
	if (remote_table->server == NULL) {
		free(remote_table);
		return NULL;
	}

	return remote_table;

}

int rtable_unbind(struct rtable_t *rtable) {

}

int rtable_put(struct rtable_t *rtable, char *key, struct data_t *value) {

}

int rable_update(struct rtable_t *rtable, char *key, struct data_t *value) {

}

struct data_t *rtable_get(struct rtable_t *table, char *key) {

}
int rtable_del(struct rtable_t *table, char *key) {

}

int rtable_size(struct rtable_t *rtable) {

}

char **rtable_get_keys(struct rtable_t *rtable) {

}

void rtable_free_keys(char **keys) {

}
