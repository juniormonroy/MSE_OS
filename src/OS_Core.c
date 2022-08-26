/*
 * OS_Core.c
 *
 *  Created on: Aug 22, 2022
 *      Author: junior
 */

#include "sapi.h"
#include "OS_Core.h"
#include "OS_Hooks.h"
#include <string.h>

#define MAX_TASK_LIST 8
/*==================[definicion de variables globales]=================================*/

static osControl control_OS;

//----------------------------------------------------------------------------------


/*==================[definicion de funciones de OS]=================================*/

void INIT_TAREA_OS(void *entryPoint, tarea *task, const char * const taskName, void * const Parameter, uint32_t prioridad)
{
	static uint8_t id = 0;



	if(control_OS.cantidad_Tareas < MAX_TASK_COUNT)  {

		task->stack[STACK_SIZE/4 - XPSR] = INIT_XPSR;					//necesario para bit thumb
		task->stack[STACK_SIZE/4 - PC_REG] = (uint32_t)entryPoint;		//direccion de la tarea (ENTRY_POINT)
		task->stack[STACK_SIZE/4 - LR] = (uint32_t)RETURN_HOOKS_OS;			//Retorno de la tarea (no deberia darse)


		task->stack[STACK_SIZE/4 - LR_PREV_VALUE] = EXEC_RETURN;

		task->stack_pointer = (uint32_t) (task->stack + STACK_SIZE/4 - FULL_STACKING_SIZE);


		task->entry_point = entryPoint;
		task->id = id;
		task->estado = TAREA_READY;
		strcpy(task->name , taskName);
		task->ticks_bloqueados = 0;
		task->prioridad = prioridad;

		control_OS.listaTareas[id] = task;
		control_OS.cantidad_Tareas++;

		id++;
	}

	else {

		control_OS.error = ERR_OS_CANT_TAREAS;
		ERROR_HOOKS_OS(INIT_TAREA_OS, control_OS.error);
	}


}



void INICIALIZACION_OS(void)  {

	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS)-1);


	control_OS.estado_sistema = OS_FROM_RESET;
	control_OS.tarea_actual = NULL;
	control_OS.tarea_siguiente = NULL;




	for (uint8_t i = 0; i < MAX_TASK_COUNT; i++)  {
		if(i>=control_OS.cantidad_Tareas)
			control_OS.listaTareas[i] = NULL;
	}
}





int32_t GET_ERROR_OS(void)  {
	return control_OS.error;
}


//##################


static bool scheduler(void)
{
	uint32_t i_task;
	tarea *aux_p_task;
	int i, j;
	uint8_t  aux_p = 0;
	uint32_t current_ticks;

	if (control_OS.estado_sistema == OS_FROM_RESET)
	{
		i_task = 1;
		/* Set current task as first configure task */
		control_OS.tarea_actual = control_OS.listaTareas[i_task-1];
	}
	else
	{
		/*The scheduler can be called from an exception given by Systick in Handler mode,
		 * or by the function os_cpu_yield in Thread mode */
		if (control_OS.estado_sistema == OS_SCHEDULING)
		{
			return RETURN_FAIL;
		}
		else
		{
			SET_STATE_OS(OS_SCHEDULING);
		}

		aux_p = 255;

		if(control_OS.tarea_actual->id < control_OS.cantidad_Tareas)
			j = control_OS.tarea_actual->id;
		else
			j = 0;

		for(i=0;i<control_OS.cantidad_Tareas;i++)
		{
			if(control_OS.listaTareas[j]-> estado == TAREA_BLOCKED)
			{
				if(control_OS.listaTareas[j]->ticks_bloqueados != 0)
				{
					current_ticks = GET_TICK_COUNT_OS();
					if(current_ticks >= control_OS.listaTareas[j]->ticks_bloqueados)
					{
						control_OS.listaTareas[j]->estado = TAREA_READY;
						control_OS.listaTareas[j]->ticks_bloqueados = 0;
					}
				}
			}

			if(control_OS.listaTareas[j]->estado == TAREA_READY)
			{
				if(aux_p > control_OS.listaTareas[j]->prioridad)
				{
					aux_p = control_OS.listaTareas[j]->prioridad;
					i_task = j;
				}
			}

			j++;
			if(j >= control_OS.cantidad_Tareas)
				j = 0;

		}

		if(aux_p == 255)
		{
			if(control_OS.tarea_actual->estado == TAREA_RUNNING)
				control_OS.tarea_siguiente = control_OS.tarea_actual;
			else
				control_OS.tarea_siguiente = &TaskIdle;
		}
		else
		{
			control_OS.tarea_siguiente = control_OS.listaTareas[i_task];
		}

		if(control_OS.tarea_actual->estado == TAREA_BLOCKED)
		{
			SET_STATE_OS(OS_NORMAL_RUN);
			os_set_pendSV();
		}

		SET_STATE_OS(OS_NORMAL_RUN);
	}

	return RETURN_OK;
}

//#################


void SysTick_Handler(void)  {



	scheduler();



	TICK_HOOKS_OS();


	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;


	__ISB();


	__DSB();
}



uint32_t getContextoSiguiente(uint32_t sp_actual)  {
	uint32_t sp_siguiente;




	if (control_OS.estado_sistema == OS_FROM_RESET)  {
		sp_siguiente = control_OS.tarea_actual->stack_pointer;
		control_OS.tarea_actual->estado = TAREA_RUNNING;
		control_OS.estado_sistema = OS_NORMAL_RUN;
	}


	else {
		control_OS.tarea_actual->stack_pointer = sp_actual;
		control_OS.tarea_actual->estado = TAREA_READY;

		sp_siguiente = control_OS.tarea_siguiente->stack_pointer;

		control_OS.tarea_actual = control_OS.tarea_siguiente;
		control_OS.tarea_actual->estado = TAREA_RUNNING;
	}

	return sp_siguiente;
}


void TASK_ENTER_CRITICAL_OS(void)
{
	__disable_irq();
	control_OS.os_criticalcount++;
}
void TASK_EXIT_CRITICAL_OS(void)
{
	if(control_OS.os_criticalcount > 0)
		control_OS.os_criticalcount--;
	if(control_OS.os_criticalcount == 0 )
	{
		__enable_irq();
	}
}




tarea* GET_CURRENT_TASK_OS(void)
{
	return control_OS.tarea_actual;
}

void BLOCK_CURRENT_TASK_OS(void)
{
	control_OS.tarea_actual-> estado = TAREA_BLOCKED;
	TASK_EXIT_CRITICAL_OS();
	CPU_YIELD_OS();
}


void CPU_YIELD_OS(void)
{
	scheduler();
}


void UNBLOCK_CURRENT_TASK_OS(tarea* task)
{
	task->estado = TAREA_READY;
}



uint32_t GET_TICK_COUNT_OS( void )
{
	uint32_t aux_ticks;

	/* Critical section required if running on a 16 bit processor. */
	TASK_ENTER_CRITICAL_OS();
	{
		aux_ticks = control_OS.os_tickcount;
	}
	TASK_EXIT_CRITICAL_OS();

	return aux_ticks;
}


void SET_CURRENT_TASK_TICK_OS( uint32_t ticks_block)
{
	control_OS.tarea_actual->ticks_bloqueados = ticks_block;
}
void SET_STATE_OS(estadoOS state)
{
	control_OS.estado_previo = control_OS.estado_sistema;
	//osControl.sys_previous_state = control_OS.sys_state;
	control_OS.estado_sistema = state;
}
void SET_PREVIOUS_STATE_OS(void)
{
	estadoOS state;
	state = control_OS.estado_sistema;
	control_OS.estado_sistema = control_OS.estado_previo;
	control_OS.estado_previo =  state;
}



void os_set_error(void *caller, uint16_t error)  {
	control_OS.error = error;
	ERROR_HOOKS_OS(caller, error) ;
}


void INIT_STRUCT_CONTROL_OS(void)
{
	int i;

	control_OS.estado_previo = OS_FROM_RESET;
	control_OS.estado_sistema = OS_FROM_RESET;
	control_OS.tarea_actual = NULL;
	control_OS.tarea_siguiente = NULL;
	control_OS.error = 0;
	control_OS.cantidad_Tareas = 0;

	for (i = control_OS.cantidad_Tareas; i < MAX_TASK_LIST; i++)
	{
		control_OS.listaTareas[i] = NULL;
	}
}

static void os_set_pendSV(void)
{

	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;


	__ISB();


	__DSB();
}
