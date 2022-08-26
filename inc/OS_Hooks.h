/*
 * OS_Hooks.h
 *
 *  Created on: Aug 22, 2022
 *      Author: junior
 */

#ifndef MSE_ISO_MSE_OS_INC_OS_HOOKS_H_
#define MSE_ISO_MSE_OS_INC_OS_HOOKS_H_




void __attribute__((weak)) RETURN_HOOKS_OS(void);

void __attribute__((weak)) TICK_HOOKS_OS(void);

void __attribute__((weak)) ERROR_HOOKS_OS(void *caller, uint32_t error);

void __attribute__((weak)) TASK_IDLE_OS(void);


#endif /* MSE_ISO_MSE_OS_INC_OS_HOOKS_H_ */
