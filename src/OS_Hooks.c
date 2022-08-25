/*
 * OS_Hooks.c
 *
 *  Created on: Aug 22, 2022
 *      Author: junior
 */


#include "sapi.h"
#include "OS_Core.h"
#include "OS_Hooks.h"


void __attribute__((weak)) returnHook(void)
{
	while(1)
	{
		__asm("wfi");
	};
}



void __attribute__((weak)) tickHook(void)
{
	__asm volatile( "nop" );
	//__asm("wfi");
}


void __attribute__((weak)) errorHook(void *caller, uint32_t error)
{


	if (error == ERR_OS_CANT_TAREAS)
	{
		uartWriteString(UART_USB, "ERROR CANTIDAD DE TAREAS");
	}
	else if (error == MEMORY_ERROR)
	{
		uartWriteString(UART_USB, "ERROR DE MEMORIA");
	}
	else if (error == TASK_RETURN_ERROR)
	{
		uartWriteString(UART_USB, "ERROR DE LA TAREA");
	}
	else if (error == ISR_ERROR)
	{
		uartWriteString(UART_USB, "ERROR DE INTERRUPCION");
	}
	else

	{
		while(1);
	}


}


void __attribute__((weak)) taskIDLE(void)
{

	while(1)
	{
		__asm("wfi");
	};

}

