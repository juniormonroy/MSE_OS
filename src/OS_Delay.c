/*
 * OS_Delay.c
 *
 *  Created on: Aug 26, 2022
 *      Author: junior
 */
#include "OS_Delay.h"


void DELAY_OS(uint32_t ticks)
{
	tarea* tarea_actual;

	if(GET_ESTADO_SISTEMA_OS() == OS_IRQ_RUN)
	{
		SET_ERROR_OS(ERR_OS_DELAY_FROM_ISR,DELAY_OS);
	}

	if(ticks > 0)
	{
		TASK_ENTER_CRITICAL_OS();


		tarea_actual = GET_CURRENT_TASK_OS();
		tarea_actual->ticks_bloqueados = ticks;


		TASK_EXIT_CRITICAL_OS();


		while (tarea_actual->ticks_bloqueados > 0)
		{
			tarea_actual->estado = TAREA_BLOCKED;
			CPU_YIELD_OS();
		}
	}
}
