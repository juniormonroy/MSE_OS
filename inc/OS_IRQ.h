/*
 * OS_IRQ.h
 *
 *  Created on: Aug 26, 2022
 *      Author: junior
 */

#ifndef MSE_ISO_MSE_OS_INC_OS_IRQ_H_
#define MSE_ISO_MSE_OS_INC_OS_IRQ_H_

#include "OS_Core.h"
#include "OS_Hooks.h"
#include "main.h"




void ISR_REGISTER_OS(LPC43XX_IRQn_Type irq, void* pIsr, uint8_t param);

void ISR_UNREGISTER_OS( LPC43XX_IRQn_Type irq);

void IRQ_INIT_OS (void);



#endif /* MSE_ISO_MSE_OS_INC_OS_IRQ_H_ */
