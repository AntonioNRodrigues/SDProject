
#include <stdlib.h>
#include <string.h>
#include "data.h"

/* Função que cria um novo elemento de dados data_t e reserva a memória
 * necessária, especificada pelo parâmetro size 
 */
struct data_t *data_create(int size) {
	// size is a invalida value
	if (size <= 0)
		return NULL;
	// allocated space to the newData
	struct data_t *newData = (struct data_t *) malloc(sizeof(struct data_t));
	//if the allocation has gone wrong return NULL
	if (newData == NULL)
		return NULL;
	//populate the values for the newData
	newData->data = malloc(size);

	//If the allocation of sapce for the newData-Data has gone wrong return NULL
	if (newData->data == NULL) {
		free(newData);
		return NULL;
	}
	newData->datasize = size;

	return newData;

}

/* Função idêntica à anterior, mas que inicializa os dados de acordo com
 * o parâmetro data.
 */
struct data_t *data_create2(int size, void * data) {
	// size is a invalida value
	if (data == NULL || size <= 0)
		return NULL;
	// allocated space to the newData
	struct data_t *newData = data_create(size);
	//if the allocation has gone wrong return NULL
	if (newData == NULL) {
		return NULL;
	}

	//populate the values for the newData
	memcpy(newData->data, data, size);

	if(newData->data == NULL){
		data_destroy(newData);
		return NULL;
	}

	return newData;
}

/* Função que destrói um bloco de dados e liberta toda a memória.
 */
void data_destroy(struct data_t *data) {
	if (data != NULL) {
		free(data->data);
		free(data);
	}
}

/* Função que duplica uma estrutura data_t.
 */
struct data_t *data_dup(struct data_t *data) {
	if(data == NULL){
		return NULL;
	}
	struct data_t *dataDup = data_create2(data->datasize, data->data);

	return dataDup;
}
