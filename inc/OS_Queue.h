/*
 * OS_Queue.h
 *
 *  Created on: Aug 26, 2022
 *      Author: junior
 */

#ifndef MSE_ISO_MSE_OS_INC_OS_QUEUE_H_
#define MSE_ISO_MSE_OS_INC_OS_QUEUE_H_

#include "OS_Core.h"

#define QUEUE_HEAP_SIZE		64	//cantidad de bytes reservados por cada cola definida



//Definicion de la estructura para las colas
struct _cola
{
	uint8_t data[QUEUE_HEAP_SIZE];
	tarea* tarea_asociada;
	uint16_t indice_head;
	uint16_t indice_tail;
	uint16_t size_elemento;
};
typedef struct _cola osCola;

void QUEUE_INIT_OS(osCola* cola, uint16_t datasize);

void QUEUE_WRITE_OS(osCola* cola, void* dato);

void QUEUE_READ_OS(osCola* cola, void* dato);

#endif /* MSE_ISO_MSE_OS_INC_OS_QUEUE_H_ */
