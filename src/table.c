#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "table-private.h"

int key_hash(char *key, int l){

  /* Verificar se key é NULL */

  /* l tem valor válido? */

  return soma % l;
}

struct table_t *table_create(int n) {

  /* n tem valor válido? */

  /* Alocar memória para struct table_t */

  /* Alocar memória para array de listas com n entradas 
     que ficará referenciado na struct table_t alocada. 

     Inicializar listas.

     Inicializar atributos da tabela.
  */

  return new_table;
}

void table_destroy(struct table_t *table) {

  /* table é NULL? 

     Libertar memória das listas.

     Libertar memória da tabela.

  */

  
}

int table_put(struct table_t *table, char * key, struct data_t *value) {

  /* Verificar valores de entrada */

  /* Criar entry com par chave/valor */

  /* Executar hash para determinar onde inserir a entry na tabela */

  /* Inserir entry na tabela */

}

int table_update(struct table_t *table, char * key, struct data_t *value) {

}

struct data_t *table_get(struct table_t *table, char * key){

}

int table_del(struct table_t *table, char *key){

}

/* Esta é dada! Ao estilo C! */
int table_size(struct table_t *table) {
	return table == NULL ? -1 : table->/*atributo*/;
}

char **table_get_keys(struct table_t *table) {

}

void table_free_keys(char **keys) {

}

