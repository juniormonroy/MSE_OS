/*
 * OS_Core.c
 *
 *  Created on: Aug 22, 2022
 *      Author: junior
 */
#include "OS_Core.h"
#include "OS_Hooks.h"
#include "OS_Queue.h"
#include "OS_Semaph.h"

/*==================[definicion de variables globales]=================================*/

static osControl control_OS;
static tarea tareaIdle;

//----------------------------------------------------------------------------------

/*==================[definicion de prototipos static]=================================*/
static void INIT_TASK_IDLE_OS(void);
static void setPendSV(void);
static void PRIORITY_ORDER_OS(void);



/*==================[definicion de funciones de OS]=================================*/

void INIT_TAREA_OS(void *entryPoint, tarea *task, const char * const taskName, void * Param, uint8_t prioridad)
{
	static uint8_t id = 0;

	if(control_OS.cantidad_Tareas < MAX_TASK_COUNT)
	{

		task->stack[STACK_SIZE/4 - XPSR] = INIT_XPSR;					//necesario para bit thumb
		task->stack[STACK_SIZE/4 - PC_REG] = (uint32_t)entryPoint;		//direccion de la tarea (ENTRY_POINT)
		task->stack[STACK_SIZE/4 - LR] = (uint32_t)RETURN_HOOKS_OS;			//Retorno de la tarea (no deberia darse)


		task->stack[STACK_SIZE/4 - LR_PREV_VALUE] = EXEC_RETURN;

		task->stack_pointer = (uint32_t) (task->stack + STACK_SIZE/4 - FULL_STACKING_SIZE);


		task->entry_point = entryPoint;
		task->id = id;
		task->estado = TAREA_READY;
		task->prioridad = prioridad;

		strcpy(task->name , taskName);
		task->Parameter = Param;


		control_OS.listaTareas[id] = task;
		control_OS.cantidad_Tareas++;
		control_OS.cantTareas_prioridad[prioridad]++;

		id++;
	}

	else
	{

		SET_ERROR_OS(ERR_OS_CANT_TAREAS,INIT_TAREA_OS);
	}
}

void INICIALIZACION_OS(void)
{

	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS)-1);

	INIT_TASK_IDLE_OS();

	control_OS.estado_sistema = OS_FROM_RESET;
	control_OS.tarea_actual = NULL;
	control_OS.tarea_siguiente = NULL;


	for (uint8_t i = 0; i < MAX_TASK_COUNT; i++)
	{
		if(i>=control_OS.cantidad_Tareas)
			control_OS.listaTareas[i] = NULL;
	}

	PRIORITY_ORDER_OS();
}


int32_t GET_ERROR_OS(void)
{
	return control_OS.error;
}


static void INIT_TASK_IDLE_OS(void)
{
	tareaIdle.stack[STACK_SIZE/4 - XPSR] = INIT_XPSR;					//necesario para bit thumb
	tareaIdle.stack[STACK_SIZE/4 - PC_REG] = (uint32_t)TASK_IDLE_OS;		//direccion de la tarea (ENTRY_POINT)
	tareaIdle.stack[STACK_SIZE/4 - LR] = (uint32_t)RETURN_HOOKS_OS;			//Retorno de la tarea (no deberia darse)


	tareaIdle.stack[STACK_SIZE/4 - LR_PREV_VALUE] = EXEC_RETURN;
	tareaIdle.stack_pointer = (uint32_t) (tareaIdle.stack + STACK_SIZE/4 - FULL_STACKING_SIZE);

	strcpy(tareaIdle.name , NULL);
	tareaIdle.Parameter = NULL;

	tareaIdle.entry_point = TASK_IDLE_OS;
	tareaIdle.id = 0xFF;
	tareaIdle.estado = TAREA_READY;
	tareaIdle.prioridad = 0xFF;
}


static void scheduler(void)
{
	static uint8_t indicePrioridad[PRIORITY_COUNT];		//indice de tareas a ejecutar segun prioridad
	uint8_t indiceArrayTareas = 0;
	uint8_t prioridad_actual = MAX_PRIORITY;			//Maxima prioridad al iniciar
	uint8_t cantBloqueadas_prioActual = 0;
	bool salir = false;
	uint8_t cant_bloqueadas = 0;
	estadoOS estado_anterior;


	if (control_OS.estado_sistema == OS_FROM_RESET)
	{
		control_OS.tarea_actual = (tarea*) &tareaIdle;
		memset(indicePrioridad,0,sizeof(uint8_t) * PRIORITY_COUNT);
		control_OS.estado_sistema = OS_NORMAL_RUN;
	}

	if (control_OS.estado_sistema == OS_SCHEDULING)
	{
		return;
	}

	control_OS.estado_sistema = OS_SCHEDULING;

	while(!salir)
	{

		indiceArrayTareas = 0;

		if(control_OS.cantTareas_prioridad[prioridad_actual] > 0)
		{

			indicePrioridad[prioridad_actual] %= control_OS.cantTareas_prioridad[prioridad_actual];

			for (int i=0; i<prioridad_actual;i++)
			{
				indiceArrayTareas += control_OS.cantTareas_prioridad[i];
			}
			indiceArrayTareas += indicePrioridad[prioridad_actual];



			if(( (tarea*)control_OS.listaTareas[indiceArrayTareas] )->estado == TAREA_READY)
			{
				control_OS.tarea_siguiente = (tarea*) control_OS.listaTareas[indiceArrayTareas];
				control_OS.cambioContextoNecesario = true;
				indicePrioridad[prioridad_actual]++;
				salir = true;


			}
			else if(( (tarea*)control_OS.listaTareas[indiceArrayTareas] )->estado == TAREA_BLOCKED)
			{
				cant_bloqueadas++;
				cantBloqueadas_prioActual++;
				indicePrioridad[prioridad_actual]++;
				if (cant_bloqueadas == control_OS.cantidad_Tareas)
				{
					control_OS.tarea_siguiente = &tareaIdle;
					control_OS.cambioContextoNecesario = true;
					salir = true;
				}
				else
				{
					if(cantBloqueadas_prioActual == control_OS.cantTareas_prioridad[prioridad_actual])
					{
						cantBloqueadas_prioActual = 0;
						indicePrioridad[prioridad_actual] = 0;
						prioridad_actual++;
					}
				}


			}
			else if(( (tarea*)control_OS.listaTareas[indiceArrayTareas] )->estado == TAREA_RUNNING)
			{

				indicePrioridad[prioridad_actual]++;
				control_OS.cambioContextoNecesario = false;
				salir = true;


			}
			else
			{
				SET_ERROR_OS(ERR_OS_SCHEDULING,scheduler);
			}

		}
		else
		{
			indicePrioridad[prioridad_actual] = 0;
			prioridad_actual++;
		}
	}

	control_OS.estado_sistema = OS_NORMAL_RUN;


	if(control_OS.cambioContextoNecesario)
		setPendSV();
}

void SysTick_Handler(void)
{
	uint8_t i;
	tarea* task;		//variable para legibilidad

	i = 0;

	while (control_OS.listaTareas[i] != NULL)
	{
		task = (tarea*)control_OS.listaTareas[i];

		if( task->ticks_bloqueados > 0 )
		{
			if((--task->ticks_bloqueados == 0) && (task->estado == TAREA_BLOCKED))
			{
				task->estado = TAREA_READY;
			}
		}

		i++;
	}


	scheduler();


	TICK_HOOKS_OS();
}

static void setPendSV(void)
{

	control_OS.cambioContextoNecesario = false;

	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;

	__ISB();

	__DSB();
}

uint32_t getContextoSiguiente(uint32_t sp_actual)
{
	uint32_t sp_siguiente;

	control_OS.tarea_actual->stack_pointer = sp_actual;

	if (control_OS.tarea_actual->estado == TAREA_RUNNING)
		control_OS.tarea_actual->estado = TAREA_READY;

	sp_siguiente = control_OS.tarea_siguiente->stack_pointer;

	control_OS.tarea_actual = control_OS.tarea_siguiente;
	control_OS.tarea_actual->estado = TAREA_RUNNING;
	control_OS.estado_sistema = OS_NORMAL_RUN;

	return sp_siguiente;
}

void CPU_YIELD_OS(void)
{
	scheduler();
}

tarea* GET_CURRENT_TASK_OS(void)
{
	return control_OS.tarea_actual;
}

estadoOS GET_ESTADO_SISTEMA_OS(void)
{
	return control_OS.estado_sistema;
}

void SET_ESTADO_SISTEMA_OS(estadoOS estado)
{
	control_OS.estado_sistema = estado;
}

void SET_SCHEDULE_FROM_ISR_OS(bool value)
{
	control_OS.schedulingFromIRQ = value;
}

bool GET_SCHEDULE_FROM_ISR_OS(void)
{
	return control_OS.schedulingFromIRQ;
}

void SET_ERROR_OS(int32_t err, void* caller)
{
	control_OS.error = err;
	ERROR_HOOKS_OS(caller, err);
}

void os_setWarning(int32_t warn)
{
	control_OS.error = warn;
}

inline void TASK_ENTER_CRITICAL_OS()
{
	__disable_irq();
	control_OS.contador_critico++;
}

inline void TASK_EXIT_CRITICAL_OS()
{
	if (--control_OS.contador_critico <= 0)
	{
		control_OS.contador_critico = 0;
		__enable_irq();
	}
}

static void PRIORITY_ORDER_OS(void)
{
	int32_t p;
	tarea* aux;

	// Crear una pila auxiliar
	int32_t stack[MAX_TASK_COUNT];

	// inicializar la parte superior de la pila
	int32_t top = -1;
	int32_t l = 0;
	int32_t h = control_OS.cantidad_Tareas - 1;

	//empujar los valores iniciales de l y h para apilar (índices a estructuras de tareas)
	stack[++top] = l;
	stack[++top] = h;

	// Sigue apareciendo de la pila mientras no está vacía
	while (top >= 0)
	{
		// Pop h and l
		// Meter h and l
		h = stack[top--];
		l = stack[top--];

		//Coloque el elemento de pivote en su posición correcta
		// en una matriz ordenada

		tarea** arr = control_OS.listaTareas;
		tarea* x = arr[h];

		int32_t i = (l - 1);

		for (int j = l; j <= h - 1; j++)
		{
			if (arr[j]->prioridad <= x->prioridad)
			{
				i++;

				//intercambiar(&arr[i], &arr[j]);
				aux = arr[i];
				arr[i] = arr[j];
				arr[j] = aux;
			}
		}

		//intercambiar(&arr[i + 1], &arr[h]);
		aux = arr[i+1];
		arr[i+1] = arr[h];
		arr[h] = aux;

		p = (i + 1);

		// Si hay elementos en el lado izquierdo del pivote,
		// luego empuja el lado izquierdo para apilar
		if (p - 1 > l)
		{
			stack[++top] = l;
			stack[++top] = p - 1;
		}

		// Si hay elementos en el lado derecho del pivote,
		// luego empuja el lado derecho para apilar
		if (p + 1 < h)
		{
			stack[++top] = p + 1;
			stack[++top] = h;
		}
	}
}