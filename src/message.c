#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#include "message-private.h"

void free_message(struct message_t *msg){

  /* Verificar se msg é NULL */
  if (msg==NULL){
    return;
  }
  /* Se msg->c_type for:
      VALOR, libertar msg->content.data
      ENTRY, libertar msg->content.entry_create
      CHAVES, libertar msg->content.keys
      CHAVE, libertar msg->content.key
  */
  if (msg->c_type == CT_VALUE) {
    free(msg->content.data);
  } else if (msg->c_type == CT_ENTRY) {
    free(msg->content.entry);
  } else if (msg->c_type == CT_KEYS) {
    free(msg->content.keys);
  } else if (msg->c_type == CT_KEY) {
    free(msg->content.key);
  } else if (msg->c_type == CT_RESULT {
    free(msg->content.result);
  }
  /* libertar msg */
  free(msg->opcode);
  free(msg->c_type);
  free(msg->content);
  free(msg);
}

int message_to_buffer(struct message_t *msg, char **msg_buf){
  
  
  /* Verificar se msg é NULL */
  if (msg == NULL){
    return -1;
  }
  /* Consoante o msg->c_type, determinar o tamanho do vetor de bytes
     que tem de ser alocado antes de serializar msg
  */
  int buffer_size;
  
  if (msg->c_type == CT_RESULT){
      buffer_size += _SHORT*2 + _INT;
  }else if (msg->c_type == CT_VALUE){
      buffer_size += _SHORT*2 + _INT + msg->content.data->datasize);
    }else if (msg->c_type == CT_KEYS){
      char **temp_keys = msg->content.keys;
      for (int i;i<strlen(temp_keys);i+1){
        buffer_size += strlen(temp_keys[i]);
      }
      buffer_size += _SHORT*3 + _INT);
    }else if (msg->c_type == CT_KEY){
      buffer_size += _SHORT*3 + strlen(msg->content.key);
    }else if (msg->c_type == CT_ENTRY){
      buffer_size += _SHORT*3 + _INT + msg->content.data->datasize);
    }
  /* Alocar quantidade de memória determinada antes 
     *msg_buf = ....
  */
  *msg_buf = (char *) malloc(buffer_size);
  /* Inicializar ponteiro auxiliar com o endereço da memória alocada */
  
  char *ptr;
  
  ptr = *msg_buf;
  
  uint16_t short_value;
  uint32_t long_value;
  
  short_value = htons(msg->opcode);
  memcpy(ptr, &short_value, _SHORT);
  ptr += _SHORT;

  short_value = htons(msg->c_type);
  memcpy(ptr, &short_value, _SHORT);
  ptr += _SHORT;
  
  /* Consoante o conteúdo da mensagem, continuar a serialização da mesma */
  
  switch(msg->c_type) {
    case CT_RESULT :
      long_value = htonl(msg->content.result);
      memcpy(ptr, &long_value, _INT);
      ptr += _INT;
      break;
    case CT_KEY :
      long_value = h
  return buffer_size;
}

struct message_t *buffer_to_message(char *msg_buf, int msg_size){

  /* Verificar se msg_buf é NULL */

  /* msg_size tem tamanho mínimo ? */

  /* Alocar memória para uma struct message_t */

  /* Recuperar o opcode e c_type */
  memcpy(&short_aux, msg_buf, _SHORT);
  msg->opcode = ntohs(short_aux);
  msg_buf += _SHORT;

  memcpy(&short_aux, msg_buf, _SHORT);
  msg->c_type = ntohs(short_aux);
  msg_buf += _SHORT;

  /* A mesma coisa que em cima mas de forma compacta, ao estilo C! */
  msg->opcode = ntohs(*(short *) msg_buf++);
  msg->c_type = ntohs(*(short *) ++msg_buf);
  msg_buf += _SHORT;

  /* O opcode e c_type são válidos? */

  /* Consoante o c_type, continuar a recuperação da mensagem original */

  return msg;
}

