/*
 * OS_Hooks.c
 *
 *  Created on: Aug 22, 2022
 *      Author: junior
 */

#include "OS_Core.h"
#include "OS_Hooks.h"
/*==================[definicion de hooks debiles]=================================*/
void __attribute__((weak)) returnHook(void)
{

	while(1);

}


void __attribute__((weak)) tickHook(void)
{

	__asm volatile( "nop" );

}


void __attribute__((weak)) errorHook(void *caller, uint32_t error)  {



	/*
	 * Revisar el contenido de control_OS.error para obtener informacion. Utilizar os_getError()
	 */




	while(1);
}

