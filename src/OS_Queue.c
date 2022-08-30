/*
 * OS_Queue.c
 *
 *  Created on: Aug 26, 2022
 *      Author: junior
 */

#include "OS_Queue.h"


void QUEUE_INIT_OS(osCola* cola, uint16_t datasize)
{
	cola->indice_head = 0;
	cola->indice_tail = 0;
	cola->tarea_asociada = NULL;
	cola->size_elemento = datasize;
}

void QUEUE_WRITE_OS(osCola* cola, void* dato)
{
	uint16_t index_h;					//variable para legibilidad
	uint16_t elementos_total;		//variable para legibilidad
	tarea* tarea_actual;

	index_h = cola->indice_head * cola->size_elemento;
	elementos_total = QUEUE_HEAP_SIZE / cola->size_elemento;

	if(((cola->indice_head == cola->indice_tail) && cola->tarea_asociada != NULL) &&
		cola->tarea_asociada->estado == TAREA_BLOCKED)
	{

		cola->tarea_asociada->estado = TAREA_READY;

		if (GET_ESTADO_SISTEMA_OS() == OS_IRQ_RUN)
			SET_SCHEDULE_FROM_ISR_OS(true);

	}

	if (GET_ESTADO_SISTEMA_OS() == OS_IRQ_RUN &&
			(cola->indice_head + 1) % elementos_total == cola->indice_tail)
	{

		os_setWarning(WARN_OS_QUEUE_FULL_ISR);
		return;		//operacion abortada
	}

	while((cola->indice_head + 1) % elementos_total == cola->indice_tail)
	{

		TASK_ENTER_CRITICAL_OS();

		tarea_actual = GET_CURRENT_TASK_OS();
		tarea_actual->estado = TAREA_BLOCKED;
		cola->tarea_asociada = tarea_actual;

		TASK_EXIT_CRITICAL_OS();
		CPU_YIELD_OS();
	}


	memcpy(cola->data+index_h,dato,cola->size_elemento);
	cola->indice_head = (cola->indice_head + 1) % elementos_total;
	cola->tarea_asociada = NULL;
}

void QUEUE_READ_OS(osCola* cola, void* dato)
{
	uint16_t elementos_total;		//variable para legibilidad
	uint16_t index_t;					//variable para legibilidad
	tarea* tarea_actual;


	index_t = cola->indice_tail * cola->size_elemento;
	elementos_total = QUEUE_HEAP_SIZE / cola->size_elemento;


	if((( (cola->indice_head + 1) % elementos_total == cola->indice_tail) &&
			cola->tarea_asociada != NULL) &&
			cola->tarea_asociada->estado == TAREA_BLOCKED)
	{

		cola->tarea_asociada->estado = TAREA_READY;


		if (GET_ESTADO_SISTEMA_OS() == OS_IRQ_RUN)
			SET_SCHEDULE_FROM_ISR_OS(true);
	}


	if (GET_ESTADO_SISTEMA_OS() == OS_IRQ_RUN && cola->indice_head == cola->indice_tail)
	{
		os_setWarning(WARN_OS_QUEUE_EMPTY_ISR);
		return;		//operacion abortada
	}

	while(cola->indice_head == cola->indice_tail)
	{
		TASK_ENTER_CRITICAL_OS();


		tarea_actual = GET_CURRENT_TASK_OS();
		tarea_actual->estado = TAREA_BLOCKED;
		cola->tarea_asociada = tarea_actual;

		TASK_EXIT_CRITICAL_OS();
		CPU_YIELD_OS();
	}

	memcpy(dato,cola->data+index_t,cola->size_elemento);
	cola->indice_tail = (cola->indice_tail + 1) % elementos_total;
	cola->tarea_asociada = NULL;

}

