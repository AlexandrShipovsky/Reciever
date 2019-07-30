/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _UART_H
#define _UART_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x.h"
#include "stm32f10x_usart.h"

#define baudrate 9600

void UART_Init(void);
void Send_UART_Str(USART_TypeDef *USARTx, uint8_t *string);

#endif //_UART.H
