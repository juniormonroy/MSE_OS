/*
 * main.c
 *
 *  Created on: Aug 22, 2022
 *      Author: junior
 */
/*==================[inclusions]=============================================*/

#include "main.h"
#include "board.h"
#include "sapi.h"
#include "OS_Core.h"
#include "OS_Hooks.h"
//#include "OS_Queue.h"



/*==================[macros and definitions]=================================*/

#define MILISEC		1000


/*==================[Global data declaration]==============================*/

tarea g_sTarea1, g_sTarea2, g_sTarea3, g_sTarea4, g_sTarea5, g_sTarea6, g_sTarea7, g_sTarea8;


int flag;

/*==================[internal functions declaration]=========================*/

static void tarea1(void);
static void tarea2(void);
static void tarea3(void);
static void tarea4(void);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/** @brief hardware initialization function
 *	@return none
 */
static void initHardware(void)
{
	Board_Init();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / MILISEC);		//systick 1ms
	uartConfig(UART_USB, 115200);

	IRQ_INIT_OS();

	/* Init led*/
	gpioWrite(LEDB,FALSE);
	gpioWrite(LED1,FALSE);
	gpioWrite(LED2,FALSE);
	gpioWrite(LED3,FALSE);


	QUEUE_INIT_OS();
	init_sems();
	INIT_STRUCT_CONTROL_OS();


}





/*============================================================================*/

int main(void)
{

	initHardware();

	INIT_TAREA_OS(
			tarea1,		//Funcion asociada a la tarea
			&g_sTarea1, //handler de la tarea
			"TAREA01", 	//nombre de referencia de la tarea
			NULL, 		//pasaje de parametros a la tarea
			0			//prioridad de la tarea
			);


	INIT_TAREA_OS(
			tarea2, 	//Funcion asociada a la tarea
			&g_sTarea2, //handler de la tarea
			"TAREA02", 	//nombre de referencia de la tarea
			NULL, 		//pasaje de parametros a la tarea
			0			//prioridad de la tarea
			);

	INIT_TAREA_OS(
			tarea3, 	//Funcion asociada a la tarea
			&g_sTarea3, //handler de la tarea
			"TAREA03", 	//nombre de referencia de la tarea
			NULL, 		//pasaje de parametros a la tarea
			0			//prioridad de la tarea
			);

	INIT_TAREA_OS(
			tarea4, 	//Funcion asociada a la tarea
			&g_sTarea4, //handler de la tarea
			"TAREA04", 	//nombre de referencia de la tarea
			NULL, 		//pasaje de parametros a la tarea
			0			//prioridad de la tarea
			);


	INICIALIZACION_OS();

	while (1) {
	}
}



/*==================[Definicion de tareas para el OS]==========================*/
static void tarea1(void)
{


	while (1)
	{

			gpioWrite(LEDB,ON);
			DELAY_OS( 500);

			gpioWrite(LEDB,OFF);
			DELAY_OS(500);

	}
}

static void tarea2(void)
{



	while (1)
	{

			gpioWrite(LED1,ON);
			DELAY_OS(500);
			gpioWrite(LED1,OFF);
			DELAY_OS(500);


	}
}

static void tarea3(void)
{


	while (1) {

			gpioWrite(LED2,ON);
			DELAY_OS(500);
			gpioWrite(LED2,OFF);
			DELAY_OS(500);


	}
}

static void tarea4(void)
{



	while (1) {


			gpioWrite(LED3,ON);
			DELAY_OS(500);
			gpioWrite(LED3,OFF);
			DELAY_OS(500);


	}
}
/** @} doxygen end group definition */

/*==================[end of file]============================================*/
