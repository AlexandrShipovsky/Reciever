/**
  ******************************************************************************
  * @file    main.c 
  * @author  Shipovsky
  * @version V
  * @date    00-April-2019
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "UART/UART.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_crc.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// Каналы реле
#define RelayPORT GPIOA
#define Relay_1 GPIO_Pin_0
#define Relay_2 GPIO_Pin_1
#define Relay_3 GPIO_Pin_2
#define Relay_4 GPIO_Pin_3
#define Relay_5 GPIO_Pin_4
#define Relay_6 GPIO_Pin_5
// Пин LED
#define LEDpin GPIO_Pin_13
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char buf[4]; // Кольцевой буфер
/* Private function prototypes -----------------------------------------------*/
void Delay_ms(uint32_t ms);
void DMA_Ini(void);
/* Private functions ---------------------------------------------------------*/
// Программная задержка
void Delay_ms(uint32_t ms)
{
  for (uint32_t i = 0; i < ms; i++)
  {
  }
}
// Настройка DMA
void DMA_ini(void)
{
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  DMA_DeInit(DMA1_Channel5); // Сброс настроек DMA
  DMA_InitTypeDef DMA_InitStruct;
  DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t) & (USART1->DR);   // Адрес данных UART
  DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&buf[0];               // Адрес буфера памяти
  DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;                      // Направление передачи от переферии в память
  DMA_InitStruct.DMA_BufferSize = sizeof(buf);                         // Размер буфера
  DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // Отключить инкремент адреса данных переферии
  DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // Включить инкремент адреса памяти
  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // Байт данных для переферии
  DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // Байт данных для памяти
  DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;                         // Циклический режим работы
  DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;                   // Приоритет
  DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;                            // Отключить режим "из памяти в память"
  DMA_Init(DMA1_Channel5, &DMA_InitStruct);

  DMA_Cmd(DMA1_Channel5, ENABLE); // Включить DMA канал 5
}
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

  UART_Init();
  DMA_ini();
  /*!< At this stage the microcontroller setting
     */

  // Настройка LED
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitTypeDef PIN_INIT;
  PIN_INIT.GPIO_Pin = LEDpin;
  PIN_INIT.GPIO_Speed = GPIO_Speed_10MHz;
  PIN_INIT.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOC, &PIN_INIT);

  //Send_UART_Str(USART1, "I'm ready!\n\r");
  uint8_t i = 0;
  while (1)
  {

    if (buf[i] == 'w')
    {
      GPIO_SetBits(GPIOC, LEDpin);
      buf[i] = '\0';
    }
    else if (buf[i] == 's')
    {
      GPIO_ResetBits(GPIOC, LEDpin);
      buf[i] = '\0';
    }
    i++;
    if (i == sizeof(buf))
    {
      i = 0;
    }
  } // END_WHILE
} // END_MAIN

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */

/******************* (C) COPYRIGHT 2019 Biruch *****END OF FILE****/