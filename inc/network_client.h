#ifndef _NETWORK_CLIENT_H
#define _NETWORK_CLIENT_H

#include "message-private.h"

//struct server_t; //a definir pelo grupo em network_client-private.h

/* Esta fun��o deve:
 *    - estabelecer a liga��o com o servidor;
 *    - address_port � uma string no formato <hostname>:<port>
 *      (exemplo: 10.10.10.10:10000)
 *    - retornar toda a informac�o necess�ria (e.g., descritor da
 *      socket) na estrutura server_t
 */
struct server_t *network_connect(const char *address_port);


/* Esta fun��o deve
 *   - Obter o descritor da liga��o (socket) da estrutura server_t;
 *   - enviar a mensagem msg ao servidor;
 *   - receber uma resposta do servidor;
 *   - retornar a mensagem obtida como resposta ou NULL em caso
 *     de erro.
 */
struct message_t *network_send_receive(struct server_t *server, struct message_t *msg);


/* A funcao network_close() deve fechar a liga��o estabelecida por
 * network_connect(). Se network_connect() alocou memoria, a fun��o
 * deve libertar essa memoria.
 */
int network_close(struct server_t *server);

#endif

