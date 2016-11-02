#ifndef _TABLE_SKEL_H
#define _TABLE_SKEL_H

#include "message.h"

/* Inicia o skeleton da tabela.
 * O main() do servidor deve chamar esta função antes de poder usar a
 * função invoke(). O parâmetro n_lists define o número de listas a
 * serem usadas pela tabela mantida no servidor.
 * Retorna 0 (OK) ou -1 (erro, por exemplo OUT OF MEMORY)
 */
int table_skel_init(int n_lists);

/* Libertar toda a memória e recursos alocados pela função anterior.
 */
int table_skel_destroy();

/* Executa uma operação (indicada pelo opcode na msg_in) e retorna o
 * resultado numa mensagem de resposta ou NULL em caso de erro.
 */
struct message_t *invoke(struct message_t *msg_in);

#endif
