/*
 * Semaphore.h
 *
 *  Created on: Aug 26, 2022
 *      Author: junior
 */

#ifndef MSE_ISO_MSE_OS_INC_SEMAPHORE_H_
#define MSE_ISO_MSE_OS_INC_SEMAPHORE_H_

#include "OS_Core.h"


typedef struct _sem
{
	tarea* 	task;
	bool 	    taked;
} sem_t;



void SEMAPH_INIT_OS(sem_t* sem);

void SEMAPH_TAKE_OS(sem_t* sem);

void SEMAPH_GIVE_OS(sem_t* sem);


#endif /* MSE_ISO_MSE_OS_INC_SEMAPHORE_H_ */
