#include <errno.h>

#include "inet.h"
#include "table-private.h"
#include "message-private.h"

int make_server(short port){
  int sfd;
  struct sockaddr_in server;

  if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    perror("Erro ao criar socket");
    return -1;
  }

  server.sin_family = AF_INET;
  server.sin_port = htons(port);  
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sfd, (struct sockaddr *) &server, sizeof(server)) < 0){
      perror("Erro ao fazer bind");
      close(sfd);
      return -1;
  }

  if (listen(sfd, 0) < 0){
      perror("Erro ao executar listen");
      close(sfd);
      return -1;
  }
  return sfd;
}

struct message_t *get_msg_ok(int opcode_in, int res){
  struct message_t *t = (struct message_t *) malloc(sizeof(struct message_t));

  if (t == NULL) return NULL;

  t->opcode = opcode_in + 1;
  t->c_type = CT_RESULT;
  t->content.result = res;
  return t;
}

struct message_t *get_msg_tuple(int opcode_in, struct tuple_t *tp){
  struct message_t *t = (struct message_t *) malloc(sizeof(struct message_t));

  if (t == NULL) return NULL;

  t->opcode = opcode_in + 1;
  t->c_type = CT_TUPLE;
  t->content.tuple = tp;
  return t;
}

int process_message(struct message_t *msg_in, struct table_t *tab, struct message_t **msg_out, struct list_t **results){
  int res;
  struct list_t *lres;

  printf("OPCODE %d\n", msg_in->opcode);
  switch (msg_in->opcode){
    case OC_OUT:
      res = table_put(tab, msg_in->content.tuple);
      if (res == 0){
        *msg_out = get_msg_ok(msg_in->opcode, 0);
        *results = NULL;
      } else
        return res;
      break;
    case OC_IN:
      lres = table_get(tab, msg_in->content.tuple, 0, 1);
      if (lres != NULL){
        if (list_size(lres) == 0){
          *msg_out = get_msg_ok(msg_in->opcode, 0);
          *results = NULL;
        }
        else{
          *msg_out = get_msg_tuple(msg_in->opcode, lres->head->content->value);
          *results = NULL;
        }
        list_destroy(lres);
        free(msg_in);
      } 
      else
        return -1;
      break;
    case OC_SIZE:
      res = table_size(tab);
      if (res >= 0){
        *msg_out = get_msg_ok(msg_in->opcode, res);
        *results = NULL;
        printf("Size is %d\n", (*msg_out)->content.result);
      } 
      else
        return res;
      break;
  }
  return 0;
}

int network_receive_send(int sockfd, struct table_t *tab){
  char *message_out, *message_in;
  int msg_length;
  int message_size, msg_size, res, n_tuples, i;
  struct message_t *msg_in, *msg_out;
  struct list_t *results;

  if ((res = read_all(sockfd, (char *) &msg_length, _INT) <= 0)){
    if (errno == EBADF)
      printf(" * EBADF\n");
    if (errno == EPIPE)
      printf(" * EPIPE\n");
    perror("Error while reading all"); 
    return -1;
  }

  msg_size = ntohl(msg_length);
  printf("MSIZE %d\n", msg_size);

  if ((message_in = (char *) malloc(msg_size)) == NULL){
    printf("Erro ao alocar mem para receber msg\n");
    return -1; 
  }

  if ((res = read_all(sockfd, message_in, msg_size) < 0)){
    free(message_in);
    if (errno == EBADF)
      printf(" * EBADF\n");
    if (errno == EPIPE)
      printf(" * EPIPE\n");
    perror("Error while reading all"); 
    return -1;
    return -1;
  }

  if ((msg_in = buffer_to_message(message_in, msg_size)) == NULL){
    free(message_in);
    printf("Erro ao desserializar a mensagem\n");
    return -1;
  }

  if ((res = process_message(msg_in, tab, &msg_out, &results)) < 0){
    free_message(msg_in);
    free(message_in);
    printf("An error ocurred while processing a message.\n");
    return res;
  }

  free_message(msg_in);
  free(message_in);

  if ((message_size = message_to_buffer(msg_out, &message_out)) < 0){
    printf("Erro ao serializar menssagem\n");
    return message_size;
  }

  msg_size = htonl(message_size);
  if ((res = write_all(sockfd, (char *) &msg_size, _INT)) < 0){
    free_message(msg_out);
    free(message_out);
    return res;
  }
  if ((res = write_all(sockfd, message_out, message_size)) < 0){
    free_message(msg_out);
    free(message_out);
    return res;
  }

  free_message(msg_out);
  free(message_out);

  if (results != NULL){
    n_tuples = list_size(results);
    
    for (i = 0; i < n_tuples; i++){
      
    }
  }

  return 0;
}

int main(int argc, char **argv){
  int wsock, connsock, res, client_on = 1;
  struct sockaddr_in client;
  socklen_t size_client;
  struct table_t *table;

  if (argc != 2){
    printf("Uso: ./server <porto_servidor>\n");
    printf("Exemplo de uso: ./server 12345\n");
    return -1;
  }

  if ((wsock = make_server(atoi(argv[1]))) < 0) return -1;

  if ((table = table_create(100)) == NULL){
    res = close(wsock);
    return res;
  }

  while ((connsock = accept(wsock, (struct sockaddr *) &client, &size_client)) != -1) {
    printf(" * Client is connected!\n");
    while (client_on){
      if (network_receive_send(connsock, table) < 0){
        printf("maybe Enviar opcode erro\n");
        client_on = 0;
        close(connsock);
      }
    }
  }
}
