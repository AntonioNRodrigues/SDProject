/* Remote table. A definir pelo grupo em client_stub-private.h 
 */
struct rtable_t;

struct rtable_t *rtable_bind(const char *address_port){

}

int rtable_unbind(struct rtable_t *rtable){

}

int rtable_put(struct rtable_t *rtable, char *key, struct data_t *value){

}

int rable_update(struct rtable_t *rtable, char *key, struct data_t *value){

}

struct data_t *rtable_get(struct rtable_t *table, char *key){

}
int rtable_del(struct rtable_t *table, char *key){

}

int rtable_size(struct rtable_t *rtable){

}

char **rtable_get_keys(struct rtable_t *rtable){

}

void rtable_free_keys(char **keys){

}
