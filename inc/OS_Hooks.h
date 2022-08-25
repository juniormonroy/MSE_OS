/*
 * OS_Hooks.h
 *
 *  Created on: Aug 22, 2022
 *      Author: junior
 */

#ifndef MSE_ISO_MSE_OS_INC_OS_HOOKS_H_
#define MSE_ISO_MSE_OS_INC_OS_HOOKS_H_



void __attribute__((weak)) returnHook(void);

void __attribute__((weak)) tickHook(void);

void __attribute__((weak)) errorHook(void *caller, uint32_t error);

void __attribute__((weak)) taskIDLE(void);


#endif /* MSE_ISO_MSE_OS_INC_OS_HOOKS_H_ */
