/*
 * Semaphore.h
 *
 *  Created on: Aug 26, 2022
 *      Author: junior
 */

#ifndef MSE_ISO_MSE_OS_INC_SEMAPHORE_H_
#define MSE_ISO_MSE_OS_INC_SEMAPHORE_H_

#include "OS_Core.h"


//Definicion de la estructura para los semaforos
struct _semaforo
{
	tarea* tarea_asociada;
	bool tomado;
};
typedef struct _semaforo osSemaforo;


void SEMAFORO_INIT_OS(osSemaforo* sem);
void SEMAFORO_TAKE_OS(osSemaforo* sem);
void SEMAFORO_GIVE_OS(osSemaforo* sem);



#endif /* MSE_ISO_MSE_OS_INC_SEMAPHORE_H_ */
