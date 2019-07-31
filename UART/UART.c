#include "UART.h"

void UART_Init(void)
{

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;

	/* USART1 configuration ------------------------------------------------------*/
	/* USART1 configured as follow:
	            - BaudRate = 9600 baud
	            - Word Length = 8 Bits
	            - One Stop Bit
	            - No parity
	            - Hardware flow control disabled (RTS and CTS signals)
	            - Receive and transmit enabled
	            - USART Clock disabled
	            - USART CPOL: Clock is active low
	            - USART CPHA: Data is captured on the middle
	            - USART LastBit: The clock pulse of the last data bit is not output to
	                             the SCLK pin
	      */
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);

	/* Enable USART1 */
	USART_Cmd(USART1, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{
	};
}
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/*  */
void Send_UART_Str(USART_TypeDef *USARTx, uint8_t *string)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
	{
	};
	while (*string)
	{

		USART_SendData(USARTx, *string);
		string++;
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
		{
		};

	} //while
} //send_Uart_str
