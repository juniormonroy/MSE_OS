/*
 * OS_Delay.c
 *
 *  Created on: Aug 26, 2022
 *      Author: junior
 */

#include "OS_Delay.h"

#define TickRateMS 1

void DELAY_OS(uint32_t time_ms)
{
	uint32_t aux_ticks;
	uint32_t current_ticks;

	TASK_ENTER_CRITICAL_OS();

	current_ticks = GET_TICK_COUNT_OS();

	aux_ticks = current_ticks + time_ms /TickRateMS;

	SET_CURRENT_TASK_TICK_OS(aux_ticks);

	BLOCK_CURRENT_TASK_OS();

}


