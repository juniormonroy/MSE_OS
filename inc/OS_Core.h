/*
 * OS_Core.h
 *
 *  Created on: 15 Agosto. 2022
 *      Author: Junior Monroy
 */

#ifndef OS_INC_MSE_OS_CORE_H_
#define OS_INC_MSE_OS_CORE_H_


#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "board.h"
#include "sapi.h"



//Tamaño del stack predefinido para cada tarea expresado en bytes
#define STACK_SIZE 256

//----------------------------------------------------------------------------------




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

//----------------------------------------------------------------------------------



//Valores necesarios para registros del stack frame inicial
#define INIT_XPSR 	1 << 24				//xPSR.T = 1
#define EXEC_RETURN	0xFFFFFFF9			//retornar a modo thread con MSP, FPU no utilizada

//----------------------------------------------------------------------------------



//Definiciones varias
#define STACK_FRAME_SIZE			8
#define FULL_STACKING_SIZE 			17	//16 core registers + valor previo de LR
#define TASK_NAME_SIZE				10	//tamaño array correspondiente al nombre
#define MAX_TASK_COUNT				8	//cantidad maxima de tareas para este OS
#define MAX_PRIORITY		0			//maxima prioridad que puede tener una tarea
#define MIN_PRIORITY		3			//minima prioridad que puede tener una tarea
#define PRIORITY_COUNT		(MIN_PRIORITY-MAX_PRIORITY)+1	//cantidad de prioridades asignables
#define QUEUE_HEAP_SIZE		64			//cantidad de bytes reservados por cada cola definida



//definicion codigos de error y warning de OS
#define ERR_OS_CANT_TAREAS		-1
#define ERR_OS_SCHEDULING		-2
#define ERR_OS_DELAY_FROM_ISR	-3
//
#define ERROR					-4
#define RETURN_FAIL  			-5
#define RETURN_OK 	 			 1
#define MEMORY_ERROR			-6
#define TASK_RETURN_ERROR		-7
#define ISR_ERROR				-8
//
#define WARN_OS_QUEUE_FULL_ISR	-100
#define WARN_OS_QUEUE_EMPTY_ISR	-101



/*==================[definicion de datos para el OS]=================================*/


//Definicion de los estados posibles para las tareas
enum _estadoTarea
{
	TAREA_READY,
	TAREA_RUNNING,
	TAREA_BLOCKED
};
typedef enum _estadoTarea estadoTarea;



//Definicion de los estados posibles de nuestro OS
enum _estadoOS
{
	OS_FROM_RESET,				//inicio luego de un reset
	OS_SCHEDULING,				//el OS esta efectuando un scheduling
	OS_NORMAL_RUN,				//estado del sistema corriendo una tarea
	OS_IRQ_RUN					//El OS esta corriendo un Handler

};
typedef enum _estadoOS estadoOS;


//Definicion de la estructura para cada tarea
struct _tarea
{
	uint32_t stack[STACK_SIZE/4];
	uint32_t stack_pointer;
	void *entry_point;
	estadoTarea estado;
	uint8_t prioridad;
	uint8_t id;
	uint32_t ticks_bloqueados;					//cantidad de ticks que la tarea debe permanecer bloqueada
	char name[TASK_NAME_SIZE];
	void * Parameter;					//pasaje de parametro a funcion
};
typedef struct _tarea tarea;



//Definicion de la estructura de control para el sistema operativo
struct _osControl
{
	tarea *listaTareas[MAX_TASK_COUNT];			//array de punteros a tareas
	int32_t error;								//variable que contiene el ultimo error generado
	uint8_t cantidad_Tareas;					//cantidad de tareas definidas por el usuario
	estadoOS estado_sistema;					//Informacion sobre el estado del OS
	//estadoOS estado_previo;					//Informacion sobre el estado del OS anterior
	tarea *tarea_actual;				//definicion de puntero para tarea actual
	tarea *tarea_siguiente;			//definicion de puntero para tarea siguiente
	//uint32_t os_tickcount;
	//uint32_t os_criticalcount;
	uint8_t cantTareas_prioridad[PRIORITY_COUNT];	//cada posicion contiene cuantas tareas tienen la misma prioridad
	bool cambioContextoNecesario;
	bool schedulingFromIRQ;						//esta bandera se utiliza para la atencion a interrupciones
	int16_t contador_critico;					//Contador de secciones criticas solicitadas


};
typedef struct _osControl osControl;



/*==================[definicion de prototipos]=================================*/

void INIT_TAREA_OS(void *entryPoint, tarea *task, const char * const taskName, void * const Parameter, uint8_t prioridad);
void INICIALIZACION_OS(void);
int32_t GET_ERROR_OS(void);

void TASK_ENTER_CRITICAL_OS(void);
void TASK_EXIT_CRITICAL_OS(void);

void CPU_YIELD_OS(void);

void SET_ERROR_OS(int32_t err, void* caller);

tarea* GET_CURRENT_TASK_OS(void);
estadoOS GET_ESTADO_SISTEMA_OS(void);
void SET_ESTADO_SISTEMA_OS(estadoOS estado);
void SET_SCHEDULE_FROM_ISR_OS(bool value);
bool GET_SCHEDULE_FROM_ISR_OS(void);

#endif /* OS_INC_MSE_OS_CORE_H_ */
