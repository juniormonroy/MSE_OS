/*==================[inclusions]=============================================*/

#include "main.h"
#include "board.h"
#include "OS_Core.h"
#include "OS_Hooks.h"
#include "sapi.h"

/*==================[macros and definitions]=================================*/

#define MILISEC		1000

/*==================[Global data declaration]==============================*/

tarea g_sTarea1, g_sTarea2, g_sTarea3, g_sTarea4, g_sTarea5, g_sTarea6, g_sTarea7, g_sTarea8;


int flag;

/*==================[internal functions declaration]=========================*/

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
}


/*==================[Definicion de tareas para el OS]==========================*/
void tarea1(void)
{

	uint32_t i = 0;
	flag = 0;
	while (1)
	{


		if(flag == 0)
		{
			gpioWrite(LEDB,ON);
			for(i = 0;i < 900000;i++);
			gpioWrite(LEDB,OFF);
			for(i = 0;i < 900000;i++);

			flag = 1;
		}


	}
}

void tarea2(void)
{
	uint32_t j = 0;


	while (1)
	{
		if(flag == 1)
		{
			gpioWrite(LED1,ON);
			for(j = 0;j < 900000;j++);
			gpioWrite(LED1,OFF);
			for(j = 0;j < 900000;j++);
			flag = 2;
		}

	}
}

void tarea3(void)
{
	uint32_t k = 0;


	while (1) {
		if(flag == 2)
		{
			gpioWrite(LED2,ON);
			for(k = 0;k < 900000;k++);
			gpioWrite(LED2,OFF);
			for(k = 0;k < 900000;k++);
			flag = 3;
		}

	}
}

void tarea4(void)
{
	uint32_t l = 0;


	while (1) {

		if(flag == 3)
		{
			gpioWrite(LED3,ON);
			for(l = 0;l < 900000;l++);
			gpioWrite(LED3,OFF);
			for(l = 0;l < 900000;l++);
			flag = 0;
		}

	}
}


/*============================================================================*/

int main(void)
{

	initHardware();

	os_InitTarea(
			tarea1,		//Funcion asociada a la tarea
			&g_sTarea1, //handler de la tarea
			"TAREA01", 	//nombre de referencia de la tarea
			NULL, 		//pasaje de parametros a la tarea
			0			//prioridad de la tarea
			);


	os_InitTarea(
			tarea2, 	//Funcion asociada a la tarea
			&g_sTarea2, //handler de la tarea
			"TAREA02", 	//nombre de referencia de la tarea
			NULL, 		//pasaje de parametros a la tarea
			0			//prioridad de la tarea
			);

	os_InitTarea(
			tarea3, 	//Funcion asociada a la tarea
			&g_sTarea3, //handler de la tarea
			"TAREA03", 	//nombre de referencia de la tarea
			NULL, 		//pasaje de parametros a la tarea
			0			//prioridad de la tarea
			);

	os_InitTarea(
			tarea4, 	//Funcion asociada a la tarea
			&g_sTarea4, //handler de la tarea
			"TAREA04", 	//nombre de referencia de la tarea
			NULL, 		//pasaje de parametros a la tarea
			0			//prioridad de la tarea
			);


	os_Init();

	while (1) {
	}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
