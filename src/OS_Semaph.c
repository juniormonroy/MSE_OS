/*
 * Semaphore.c
 *
 *  Created on: Aug 26, 2022
 *      Author: junior
 */


#include "OS_Semaph.h"

sem_t semLed;
sem_t semWrite;

void SEMAPH_INIT_OS(sem_t* sem)
{
	sem->taked = TRUE;
	sem->task = NULL;
}

/* */
void SEMAPH_TAKE_OS(sem_t* sem)
{
	TASK_ENTER_CRITICAL_OS();
	if(sem->taked)
	{
		sem->task = GET_CURRENT_TASK_OS();
		BLOCK_CURRENT_TASK_OS();
	}
	else
	{
		sem->taked = TRUE;
	}
	TASK_EXIT_CRITICAL_OS();
}

/* */
void SEMAPH_GIVE_OS(sem_t* sem)
{
	if(sem->taked)
	{
		sem->taked = FALSE;
		if(sem->task != NULL)
		{
			UNBLOCK_CURRENT_TASK_OS(sem->task);
		}
	}
}

void init_sems(void)
{
	SEMAPH_INIT_OS(&semLed);
	SEMAPH_INIT_OS(&semWrite);
}
