/*
 * OS_IRQ.h
 *
 *  Created on: Aug 26, 2022
 *      Author: junior
 */

#ifndef MSE_ISO_MSE_OS_INC_OS_IRQ_H_
#define MSE_ISO_MSE_OS_INC_OS_IRQ_H_



#include "OS_Core.h"
#include "board.h"
#include "cmsis_43xx.h"

#define CANT_IRQ	53

extern osControl g_sControl_OS;

void IRQ_INIT_HW_OS(void);
bool INSTALL_IRQ_OS(LPC43XX_IRQn_Type irq, void* usr_isr);
bool REMOVE_IRQ_OS(LPC43XX_IRQn_Type irq);




#endif /* MSE_ISO_MSE_OS_INC_OS_IRQ_H_ */
