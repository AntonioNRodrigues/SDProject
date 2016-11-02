#ifndef _CLIENT_STUB_H
#define _CLIENT_STUB_H

#include "data.h"

/* Remote table. A definir pelo grupo em client_stub-private.h 
 */
struct rtable_t; 

/* Função para estabelecer uma associação entre o cliente e uma tabela
 * remota num servidor.
 * address_port é uma string no formato <hostname>:<port>.
 * retorna NULL em caso de erro .
 */
struct rtable_t *rtable_bind(const char *address_port);

/* Termina a associação entre o cliente e a tabela remota, e liberta
 * toda a memória local. 
 * Retorna 0 se tudo correr bem e -1 em caso de erro.
 */
int rtable_unbind(struct rtable_t *rtable);

/* Função para adicionar um par chave valor na tabela remota.
 * Devolve 0 (ok) ou -1 (problemas).
 */
int rtable_put(struct rtable_t *rtable, char *key, struct data_t *value);

/* Função para substituir na tabela remota, o valor associado à chave key.
 * Devolve 0 (OK) ou -1 em caso de erros.
 */
int rable_update(struct rtable_t *rtable, char *key, struct data_t *value);

/* Função para obter da tabela remota o valor associado à chave key.
 * Devolve NULL em caso de erro.
 */
struct data_t *rtable_get(struct rtable_t *table, char *key);

/* Função para remover um par chave valor da tabela remota, especificado 
 * pela chave key.
 * Devolve: 0 (OK) ou -1 em caso de erros.
 */
int rtable_del(struct rtable_t *table, char *key);

/* Devolve número de pares chave/valor na tabela remota.
 */
int rtable_size(struct rtable_t *rtable);

/* Devolve um array de char * com a cópia de todas as keys da
 * tabela remota, e um último elemento a NULL.
 */
char **rtable_get_keys(struct rtable_t *rtable);

/* Liberta a memória alocada por table_get_keys().
 */
void rtable_free_keys(char **keys);

#endif
