/*
 * OS_IRQ.c
 *
 *  Created on: Aug 26, 2022
 *      Author: junior
 */

#include "OS_IRQ.h"


#define TEC1_PORT_NUM   0
#define TEC1_BIT_VAL    4

#define TEC2_PORT_NUM   0
#define TEC2_BIT_VAL    8

static void* isr_vector_usuario[CANT_IRQ];	//vector de punteros a funciones para nuestras interrupciones


void IRQ_INIT_HW_OS(void)
{
	/////////////////////////tec 1 ///////////////////////////////////////////////////
//Seteamos la interrupcion 0 para el flanco descendente en la tecla 1
	Chip_SCU_GPIOIntPinSel( 0, TEC1_PORT_NUM, TEC1_BIT_VAL );
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 0 ) ); // INT0 flanco descendente
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( 0 ) );
	Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH( 0 ) );


//Seteamos la interrupcion 1 para el flanco ascendente en la tecla 1
	Chip_SCU_GPIOIntPinSel( 1, TEC1_PORT_NUM, TEC1_BIT_VAL );
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 1 ) ); // INT1 flanc
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( 1 ) );
	Chip_PININT_EnableIntHigh( LPC_GPIO_PIN_INT, PININTCH( 1 ) );


	/////////////////////////tec 2 ///////////////////////////////////////////////////
//Seteamos la interrupcion 2 para el flanco descendente en la tecla 2
	Chip_SCU_GPIOIntPinSel( 2, TEC2_PORT_NUM, TEC2_BIT_VAL );
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 2 ) ); // INT0 flanco descendente
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( 2 ) );
	Chip_PININT_EnableIntLow( LPC_GPIO_PIN_INT, PININTCH( 2 ) );


 //Seteamos la interrupcion 3 para el flanco ascendente en la tecla 2
	Chip_SCU_GPIOIntPinSel( 3, TEC2_PORT_NUM, TEC2_BIT_VAL );
	Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH( 3 ) ); // INT1 flanc
	Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH( 3 ) );
	Chip_PININT_EnableIntHigh( LPC_GPIO_PIN_INT, PININTCH( 3 ) );




}

bool INSTALL_IRQ_OS(LPC43XX_IRQn_Type irq, void* usr_isr)
{
	bool Ret = 0;


	if (isr_vector_usuario[irq] == NULL)
	{
		isr_vector_usuario[irq] = usr_isr;
		NVIC_ClearPendingIRQ(irq);
		NVIC_EnableIRQ(irq);
		Ret = true;
	}

	return Ret;
}

bool REMOVE_IRQ_OS(LPC43XX_IRQn_Type irq)
{
	bool Ret = 0;

	if (isr_vector_usuario[irq] != NULL)
	{
		isr_vector_usuario[irq] = NULL;
		NVIC_ClearPendingIRQ(irq);
		NVIC_DisableIRQ(irq);
		Ret = true;
	}

	return Ret;
}


static void os_IRQHandler(LPC43XX_IRQn_Type IRQn)
{
	estadoOS estadoPrevio_OS;
	void (*funcion_usuario)(void);


	estadoPrevio_OS = GET_ESTADO_SISTEMA_OS();



	SET_ESTADO_SISTEMA_OS(OS_IRQ_RUN);


	funcion_usuario = isr_vector_usuario[IRQn];
	funcion_usuario();


	SET_ESTADO_SISTEMA_OS(estadoPrevio_OS);



	NVIC_ClearPendingIRQ(IRQn);



	if (GET_SCHEDULE_FROM_ISR_OS())
	{
		SET_SCHEDULE_FROM_ISR_OS(false);
		CPU_YIELD_OS();
	}
}

/*==================[interrupt service routines]=============================*/

void DAC_IRQHandler(void){os_IRQHandler(         DAC_IRQn         );}
void M0APP_IRQHandler(void){os_IRQHandler(       M0APP_IRQn       );}
void DMA_IRQHandler(void){os_IRQHandler(         DMA_IRQn         );}
void FLASH_EEPROM_IRQHandler(void){os_IRQHandler(RESERVED1_IRQn   );}
void ETH_IRQHandler(void){os_IRQHandler(         ETHERNET_IRQn    );}
void SDIO_IRQHandler(void){os_IRQHandler(        SDIO_IRQn        );}
void LCD_IRQHandler(void){os_IRQHandler(         LCD_IRQn         );}
void USB0_IRQHandler(void){os_IRQHandler(        USB0_IRQn        );}
void USB1_IRQHandler(void){os_IRQHandler(        USB1_IRQn        );}
void SCT_IRQHandler(void){os_IRQHandler(         SCT_IRQn         );}
void RIT_IRQHandler(void){os_IRQHandler(         RITIMER_IRQn     );}
void TIMER0_IRQHandler(void){os_IRQHandler(      TIMER0_IRQn      );}
void TIMER1_IRQHandler(void){os_IRQHandler(      TIMER1_IRQn      );}
void TIMER2_IRQHandler(void){os_IRQHandler(      TIMER2_IRQn      );}
void TIMER3_IRQHandler(void){os_IRQHandler(      TIMER3_IRQn      );}
void MCPWM_IRQHandler(void){os_IRQHandler(       MCPWM_IRQn       );}
void ADC0_IRQHandler(void){os_IRQHandler(        ADC0_IRQn        );}
void I2C0_IRQHandler(void){os_IRQHandler(        I2C0_IRQn        );}
void SPI_IRQHandler(void){os_IRQHandler(         I2C1_IRQn        );}
void I2C1_IRQHandler(void){os_IRQHandler(        SPI_INT_IRQn     );}
void ADC1_IRQHandler(void){os_IRQHandler(        ADC1_IRQn        );}
void SSP0_IRQHandler(void){os_IRQHandler(        SSP0_IRQn        );}
void SSP1_IRQHandler(void){os_IRQHandler(        SSP1_IRQn        );}
void UART0_IRQHandler(void){os_IRQHandler(       USART0_IRQn      );}
void UART1_IRQHandler(void){os_IRQHandler(       UART1_IRQn       );}
void UART2_IRQHandler(void){os_IRQHandler(       USART2_IRQn      );}
void UART3_IRQHandler(void){os_IRQHandler(       USART3_IRQn      );}
void I2S0_IRQHandler(void){os_IRQHandler(        I2S0_IRQn        );}
void I2S1_IRQHandler(void){os_IRQHandler(        I2S1_IRQn        );}
void SPIFI_IRQHandler(void){os_IRQHandler(       RESERVED4_IRQn   );}
void SGPIO_IRQHandler(void){os_IRQHandler(       SGPIO_INT_IRQn   );}
void GPIO0_IRQHandler(void){os_IRQHandler(       PIN_INT0_IRQn    );}
void GPIO1_IRQHandler(void){os_IRQHandler(       PIN_INT1_IRQn    );}
void GPIO2_IRQHandler(void){os_IRQHandler(       PIN_INT2_IRQn    );}
void GPIO3_IRQHandler(void){os_IRQHandler(       PIN_INT3_IRQn    );}
void GPIO4_IRQHandler(void){os_IRQHandler(       PIN_INT4_IRQn    );}
void GPIO5_IRQHandler(void){os_IRQHandler(       PIN_INT5_IRQn    );}
void GPIO6_IRQHandler(void){os_IRQHandler(       PIN_INT6_IRQn    );}
void GPIO7_IRQHandler(void){os_IRQHandler(       PIN_INT7_IRQn    );}
void GINT0_IRQHandler(void){os_IRQHandler(       GINT0_IRQn       );}
void GINT1_IRQHandler(void){os_IRQHandler(       GINT1_IRQn       );}
void EVRT_IRQHandler(void){os_IRQHandler(        EVENTROUTER_IRQn );}
void CAN1_IRQHandler(void){os_IRQHandler(        C_CAN1_IRQn      );}
void ADCHS_IRQHandler(void){os_IRQHandler(       ADCHS_IRQn       );}
void ATIMER_IRQHandler(void){os_IRQHandler(      ATIMER_IRQn      );}
void RTC_IRQHandler(void){os_IRQHandler(         RTC_IRQn         );}
void WDT_IRQHandler(void){os_IRQHandler(         WWDT_IRQn        );}
void M0SUB_IRQHandler(void){os_IRQHandler(       M0SUB_IRQn       );}
void CAN0_IRQHandler(void){os_IRQHandler(        C_CAN0_IRQn      );}
void QEI_IRQHandler(void){os_IRQHandler(         QEI_IRQn         );}