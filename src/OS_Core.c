/*
 * OS_Core.c
 *
 *  Created on: 15 Agosto. 2022
 *      Author: Junior Monroy
 */

#include "OS_Core.h"
#include "OS_Hooks.h"



static osControl control_OS;

/*==================[definicion de funciones de OS]=================================*/
void os_InitTarea(void *entryPoint, tarea *task, const char * taskName, void * const Parameter, uint32_t id, uint32_t prioridad)
{
	//static uint8_t id = 0;				//el id sera correlativo a medida que se generen mas tareas

	if(control_OS.cantidad_Tareas < MAX_TASK_COUNT)
	{

		task->stack[STACK_SIZE/4 - XPSR] = INIT_XPSR;					//necesario para bit thumb
		task->stack[STACK_SIZE/4 - PC_REG] = (uint32_t)entryPoint;		//direccion de la tarea (ENTRY_POINT)
		task->stack[STACK_SIZE/4 - LR] = (uint32_t)returnHook;			//Retorno de la tarea (no deberia darse)


		task->stack[STACK_SIZE/4 - LR_PREV_VALUE] = EXEC_RETURN;

		task->stack_pointer = (uint32_t) (task->stack + STACK_SIZE/4 - FULL_STACKING_SIZE);


		task->entry_point = entryPoint;
		task->id = id;
		task->estado = TAREA_READY;


		control_OS.listaTareas[id] = task;
		control_OS.cantidad_Tareas++;

		id++;
	}

	else
	{

		control_OS.error = ERR_OS_CANT_TAREAS;
		//errorHook(os_InitTarea, error);
	}
}


void os_Init(void)
{

	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS)-1);

	control_OS.estado_previo = OS_FROM_RESET;
	control_OS.estado_sistema = OS_FROM_RESET;
	control_OS.tarea_actual = NULL;
	control_OS.tarea_siguiente = NULL;
	control_OS.error = 0;
	control_OS.cantidad_Tareas = 0;

	for (uint8_t i = 0; i < MAX_TASK_COUNT; i++)
	{

		if(i>=control_OS.cantidad_Tareas)
			control_OS.listaTareas[i] = NULL;

	}



}


int32_t os_getError(void)
{
	return control_OS.error;
}


static void scheduler(void)
{
	uint8_t indice;		//variable auxiliar para legibilidad

	if (control_OS.estado_sistema == OS_FROM_RESET)  {
		control_OS.tarea_actual = (tarea*) control_OS.listaTareas[0];
	}
	else {
		indice = control_OS.tarea_actual->id+1;
		if(indice < control_OS.cantidad_Tareas)  {
			control_OS.tarea_siguiente = (tarea*) control_OS.listaTareas[indice];
		}
		else  {
			control_OS.tarea_siguiente = (tarea*) control_OS.listaTareas[0];
		}
	}

}


void SysTick_Handler(void)
{

	scheduler();

	tickHook();

	//Se setea el bit correspondiente a la excepcion PendSV
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;

	//Instruction Synchronization Barrier
	__ISB();

	 //Data Synchronization Barrier
	__DSB();
}

uint32_t getContextoSiguiente(uint32_t sp_actual)
{
	uint32_t sp_siguiente;

	if (control_OS.estado_sistema == OS_FROM_RESET)
	{
		sp_siguiente = control_OS.tarea_actual->stack_pointer;
		control_OS.tarea_actual->estado = TAREA_RUNNING;
		control_OS.estado_sistema = OS_NORMAL_RUN;
	}

	else
	{
		control_OS.tarea_actual->stack_pointer = sp_actual;
		control_OS.tarea_actual->estado = TAREA_READY;

		sp_siguiente = control_OS.tarea_siguiente->stack_pointer;

		control_OS.tarea_actual = control_OS.tarea_siguiente;
		control_OS.tarea_actual->estado = TAREA_RUNNING;
	}

	return sp_siguiente;
}
