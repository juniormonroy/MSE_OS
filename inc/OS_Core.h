/*
 * OS_Core.h
 *
 *  Created on: 15 Agosto. 2022
 *      Author: Junior Monroy
 */

#include <stdint.h>
#include <stdbool.h>
#include "board.h"

//Tamaño del stack predefinido para cada tarea expresado en bytes
#define STACK_SIZE 256


//Posiciones dentro del stack de los registros que lo conforman
#define XPSR			1
#define PC_REG			2
#define LR				3
#define R12				4
#define R3				5
#define R2				6
#define R1				7
#define R0				8
#define LR_PREV_VALUE	9
#define R4				10
#define R5				11
#define R6				12
#define R7				13
#define R8				14
#define R9				15
#define R10 			16
#define R11 			17


//Valores necesarios para registros del stack frame inicial
#define INIT_XPSR 	1 << 24				//xPSR.T = 1
#define EXEC_RETURN	0xFFFFFFF9			//retornar a modo thread con MSP, FPU no utilizada


//Definiciones varias
#define STACK_FRAME_SIZE			8
#define FULL_STACKING_SIZE 			17	//16 core registers + valor previo de LR
#define TASK_NAME_SIZE				10	//tamaño array correspondiente al nombre
#define MAX_TASK_COUNT				8	//cantidad maxima de tareas para este OS

#define ERROR		-1
#define RETURN_FAIL  0
#define RETURN_OK 	 1

//definicion codigos de error de OS
#define ERR_OS_CANT_TAREAS		-2
#define MEMORY_ERROR			-3
#define TASK_RETURN_ERROR		-4
#define ISR_ERROR				-5


//definicion de datos para el OS
//Definicion de los estados posibles para las tareas
typedef enum _estadoTarea
{
	TAREA_READY,
	TAREA_RUNNING,
	TAREA_BLOCKED
}estadoTarea;




//Definicion de los estados posibles de nuestro OS
typedef enum _estadoOS
{
	OS_FROM_RESET,
	OS_SCHEDULING,
	OS_NORMAL_RUN,
	OS_IRQ
}estadoOS;




//Definicion de la estructura para cada tarea
struct _tarea  {
	uint32_t stack[STACK_SIZE/4];
	uint32_t stack_pointer;
	void *entry_point;
	estadoTarea estado;
	uint32_t prioridad;
	uint32_t id;
	uint32_t ticks_bloqueados;
	char name[TASK_NAME_SIZE];
};
typedef struct _tarea tarea;

tarea TaskIdle;


//Definicion de la estructura de control para el sistema operativo
struct _osControl  {
	tarea *listaTareas[MAX_TASK_COUNT];			//array de punteros a tareas
	int32_t error;								//variable que contiene el ultimo error generado
	uint32_t cantidad_Tareas;					//cantidad de tareas definidas por el usuario para cada prioridad
	estadoOS estado_sistema;					//Informacion sobre el estado del OS
	estadoOS estado_previo;					//Informacion sobre el estado del OS anterior
	tarea *tarea_actual;				//definicion de puntero para tarea actual
	tarea *tarea_siguiente;			//definicion de puntero para tarea siguiente
	uint32_t os_tickcount;
	uint32_t os_criticalcount;
};
typedef struct _osControl osControl;






//definicion de prototipos
void INIT_TAREA_OS(void *entryPoint, tarea *task, const char * taskName, void * const Parameter, uint32_t prioridad);
void INICIALIZACION_OS(void);
int32_t GET_ERROR_OS(void);


void TASK_ENTER_CRITICAL_OS(void);
void TASK_EXIT_CRITICAL_OS(void);


void CPU_YIELD_OS(void);


uint32_t GET_TICK_COUNT_OS( void );
void SET_CURRENT_TASK_TICK_OS( uint32_t ticks_block);


tarea* GET_CURRENT_TASK_OS(void);
void BLOCK_CURRENT_TASK_OS(void);
void UNBLOCK_CURRENT_TASK_OS(tarea* task);


void SET_STATE_OS(estadoOS state);
void SET_PREVIOUS_STATE_OS(void);

void INIT_STRUCT_CONTROL_OS(void);

static void os_set_pendSV(void);
