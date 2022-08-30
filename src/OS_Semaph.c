/*
 * Semaphore.c
 *
 *  Created on: Aug 26, 2022
 *      Author: junior
 */

#include "OS_Semaph.h"


void SEMAFORO_INIT_OS(osSemaforo* sem)
{
	sem->tomado = true;
	sem->tarea_asociada = NULL;
}

void SEMAFORO_TAKE_OS(osSemaforo* sem)
{
	bool Salir = false;
	tarea* tarea_actual;

	while (!Salir)
	{

		if(sem->tomado)
		{
			TASK_ENTER_CRITICAL_OS();

			tarea_actual = GET_CURRENT_TASK_OS();
			tarea_actual->estado = TAREA_BLOCKED;
			sem->tarea_asociada = tarea_actual;

			TASK_EXIT_CRITICAL_OS();
			CPU_YIELD_OS();
		}
		else
		{
			sem->tomado = true;
			Salir = true;
		}

	}
}

void SEMAFORO_GIVE_OS(osSemaforo* sem)
{


	if (sem->tomado == true &&	sem->tarea_asociada != NULL)
	{
		sem->tomado = false;
		sem->tarea_asociada->estado = TAREA_READY;


		if (GET_ESTADO_SISTEMA_OS() == OS_IRQ_RUN)
			 SET_SCHEDULE_FROM_ISR_OS(true);
	}
}