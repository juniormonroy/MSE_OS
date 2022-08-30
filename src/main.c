/*
 * main.c
 *
 *  Created on: Aug 22, 2022
 *      Author: junior
 */
/*==================[inclusions]=============================================*/

#include <stdlib.h>
#include "main.h"
#include "board.h"
#include "OS_Core.h"
#include "OS_Queue.h"
#include "OS_Semaph.h"
#include "sapi.h"


/*==================[macros and definitions]=================================*/

#define MILISEC		1000

#define PRIORIDAD_0		0
#define PRIORIDAD_1		1
#define PRIORIDAD_2		2



/*==================[Global data declaration]==============================*/

tarea g_sTarea1, g_sTarea2;	//prioridad 0
tarea g_sTarea3, g_sTarea4, g_sTarea5, g_sTarea6, g_sTarea7, g_sTarea8;	//prioridad 3

osCola colaUart;

osSemaforo semTecla1_descendente, semTecla1_ascendente, semTecla2_descendente, semTecla2_ascendente;

typedef struct _mydata my_data;

/*==================[internal functions declaration]=========================*/

void tecla1_down_ISR(void);
void tecla1_up_ISR(void);
void tecla2_down_ISR(void);
void tecla2_up_ISR(void);

static void TEC1_CENTIGRADOS(void);
static void TEC2_FARENHEIT(void);
static void ADC_TEMPERATURA(void);
static void WATCHDOG_LED(void);
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


	IRQ_INIT_HW_OS();

	gpioWrite(LEDB,FALSE);
	gpioWrite(LED1,FALSE);
	gpioWrite(LED2,FALSE);
	gpioWrite(LED3,FALSE);


	adcConfig( ADC_ENABLE ); /* ADC */

	uartConfig( UART_USB, 115200 );
}

static void initSoftware(void)
{
	QUEUE_INIT_OS(&colaUart,sizeof(char));

	SEMAFORO_INIT_OS(&semTecla1_ascendente);
	SEMAFORO_INIT_OS(&semTecla1_descendente);
	SEMAFORO_INIT_OS(&semTecla2_ascendente);
	SEMAFORO_INIT_OS(&semTecla2_descendente);

	INSTALL_IRQ_OS(PIN_INT0_IRQn,tecla1_down_ISR);
	INSTALL_IRQ_OS(PIN_INT1_IRQn,tecla1_up_ISR);
	INSTALL_IRQ_OS(PIN_INT2_IRQn,tecla2_down_ISR);
	INSTALL_IRQ_OS(PIN_INT3_IRQn,tecla2_up_ISR);
}

/*============================================================================*/

int main(void)
{

	initHardware();

	initSoftware();

	INIT_TAREA_OS(
			TEC1_CENTIGRADOS,				//Funcion asociada a la tarea
			&g_sTarea1, 		//handler de la tarea
			"TEC1_CENTIGRADOS", 			//nombre de referencia de la tarea
			NULL, 				//pasaje de parametros a la tarea
			PRIORIDAD_1			//prioridad de la tarea
			);


	INIT_TAREA_OS(
			TEC2_FARENHEIT, 			//Funcion asociada a la tarea
			&g_sTarea2, 		//handler de la tarea
			"TEC2_FARENHEIT", 			//nombre de referencia de la tarea
			NULL, 				//pasaje de parametros a la tarea
			PRIORIDAD_1			//prioridad de la tarea
			);

	INIT_TAREA_OS(
			ADC_TEMPERATURA, 			//Funcion asociada a la tarea
			&g_sTarea3, 		//handler de la tarea
			"ADC_TEMPERATURA", 			//nombre de referencia de la tarea
			NULL, 				//pasaje de parametros a la tarea
			PRIORIDAD_0			//prioridad de la tarea
			);

	INIT_TAREA_OS(
			WATCHDOG_LED, 			//Funcion asociada a la tarea
			&g_sTarea4, 		//handler de la tarea
			"WATCHDOG_LED", 			//nombre de referencia de la tarea
			NULL, 				//pasaje de parametros a la tarea
			PRIORIDAD_2			//prioridad de la tarea
			);

	INICIALIZACION_OS();

	while (1)
	{
	}
}


void tecla1_down_ISR(void)
{
	SEMAFORO_GIVE_OS(&semTecla1_descendente);
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 0 ) );
}

void tecla1_up_ISR(void)
{
	SEMAFORO_GIVE_OS(&semTecla1_ascendente);
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 1 ) );
}

void tecla2_down_ISR(void)
{
	SEMAFORO_GIVE_OS(&semTecla2_descendente);
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 2 ) );
}

void tecla2_up_ISR(void)
{
	SEMAFORO_GIVE_OS(&semTecla2_ascendente);
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 3 ) );
}

/*==================[Definicion de tareas para el OS]==========================*/
static void TEC1_CENTIGRADOS(void)
{
	char msg[25];
	uint8_t i;

	strcpy(msg,"C");

	while (1)
	{

		SEMAFORO_TAKE_OS(&semTecla1_descendente);
		gpioWrite(LEDB,TRUE);
		gpioWrite(LED1,FALSE);

		i = 0;
		while(msg[i] != NULL)
		{
			QUEUE_WRITE_OS(&colaUart,(msg + i));
			i++;
		}
	}
}

static void TEC2_FARENHEIT(void)
{
	char msg[25];
	uint8_t i;

	strcpy(msg,"F");

	while (1)
	{

		SEMAFORO_TAKE_OS(&semTecla2_descendente);
		gpioWrite(LEDB,FALSE);
		gpioWrite(LED1,TRUE);

		i = 0;
		while(msg[i] != NULL)  {
			QUEUE_WRITE_OS(&colaUart,(msg + i));
			i++;
		}
	}
}


static void ADC_TEMPERATURA(void)//LLAMA AL ADC E IMPRIME LA TEMPERATURA EN F O C DEPENDIENDO DE COLA
{								 //SI COLA ENVIA C ES CENTIGRADOS, F FARENHEIT
	char aux;
	uint16_t muestras = 0, TEMP = 0;//VARIABLE MUESTRAS
	static char uartBuff[10];//BUFFER DE SALIDA PARA EL UART USB

	while(1)
	{
		QUEUE_READ_OS(&colaUart,&aux);

		if (aux == 67) // "C" EN ASCII ES 67
		{
			TASK_ENTER_CRITICAL_OS();
			muestras = 1023 - adcRead( CH2 );//LLAMA A FUNCION ADC EN CH2, DEVUELVE EL VALOR EN CUENTAS 0-1023
			TEMP = muestras/25.4 - 3;//CONVERSION A G
			itoa( TEMP, uartBuff, 10 ); // base 10, llama a funcion conversion, v es el valor convertid
			uartWriteString(UART_USB,"Temperatura de ");
			uartWriteString(UART_USB,uartBuff);
			uartWriteString(UART_USB," Grados Centigrados\n");
			TASK_EXIT_CRITICAL_OS();
		}
		if (aux == 70) // "F" EN ASCII ES 70
		{
			TASK_ENTER_CRITICAL_OS();
			muestras = 1023 - adcRead( CH2 );//LLAMA A FUNCION ADC EN CH2, DEVUELVE EL VALOR EN CUENTAS 0-1023
			TEMP = (muestras/25.4 - 3)*1.8 + 32;
			itoa( TEMP, uartBuff, 10 ); // base 10, llama a funcion conversion, v es el valor convertid
			uartWriteString(UART_USB,"Temperatura de ");
			uartWriteString(UART_USB,uartBuff);
			uartWriteString(UART_USB," Grados Farenheit\n");
			TASK_EXIT_CRITICAL_OS();
		}


	}
}

static void WATCHDOG_LED(void)//TAREA QUE INDICA LED HEARTBEAT VERDE
{
	while(1)
	{
		gpioWrite(LED3,true);
		DELAY_OS(100);
		gpioWrite(LED3,false);
		DELAY_OS(100);
	}
}


/*==================[end of file]============================================*/
