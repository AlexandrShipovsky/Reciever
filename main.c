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
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "UART/UART.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_crc.h"
#include <stdio.h>
#include "protV/protV.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// Состояния реле
#define RelayON_1 ((uint8_t)(0xFF << 6)) // 0x11000000
#define RelayON_2 ((uint8_t)(0xFF >> 6)) // 0x00000011
#define RelayOFF ((uint8_t)(0x18))       // 0x00011000
// Каналы реле
#define RelayPORT GPIOA
#define Relay_11 GPIO_Pin_0
#define Relay_12 GPIO_Pin_1
#define Relay_21 GPIO_Pin_2
#define Relay_22 GPIO_Pin_3
#define Relay_31 GPIO_Pin_4
#define Relay_32 GPIO_Pin_5
// Пин LED
#define LEDpin GPIO_Pin_13
// Размер буфера
#define sizebuf 33
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t buf[sizebuf]; // Кольцевой буфер
protVstructure prot;  // Структура протокола
int NerrPkg = 0;      // Количество битых пакетов
int TruePkg = 0;      // Количество принятых правильных пакетов
int Nbyte = 0;        // Количество исправленных байт

/* Private function prototypes -----------------------------------------------*/
void Delay_ms(uint32_t ms);
void DMA_ini(void);
void SwitchRelay(void);
ErrorStatus RCC_ini(void);
void GPIO_ini(void);
/* Private functions ---------------------------------------------------------*/
// Программная задержка
void Delay_ms(uint32_t ms)
{
  for (uint32_t i = 0; i < ms; i++)
  {
  }
}
// Настройки тактирования
ErrorStatus RCC_ini(void)
{
  RCC_DeInit();              //Сброс настроек
  RCC_HSEConfig(RCC_HSE_ON); //Включаем внешний кварцевый резонатор
  ErrorStatus HSEStartUpStatus;
  HSEStartUpStatus = RCC_WaitForHSEStartUp(); /* Ждем пока HSE будет готов */
  if (HSEStartUpStatus == ERROR)
  {
    return HSEStartUpStatus;
  }
  /* HCLK = SYSCLK */ /* Смотри на схеме AHB Prescaler. Частота не делится (RCC_SYSCLK_Div1) */
  RCC_HCLKConfig(RCC_SYSCLK_Div1);
  /* PCLK2 = HCLK */ /* Смотри на схеме APB2 Prescaler. Частота не делится (RCC_HCLK_Div1)  */
  RCC_PCLK2Config(RCC_HCLK_Div1);
  /* PCLK1 = HCLK/2 */ /* Смотри на схеме APB1 Prescaler. Частота делится на 2 (RCC_HCLK_Div2)
	потому что на выходе APB1 должно быть не более 36МГц (смотри схему тактирования) */
  RCC_PCLK1Config(RCC_HCLK_Div2);
  /* PLLCLK = 8MHz * 9 = 72 MHz */
  /* Указываем PLL от куда брать частоту (RCC_PLLSource_HSE_Div1) и на сколько ее умножать (RCC_PLLMul_9) */
  /* PLL может брать частоту с кварца как есть (RCC_PLLSource_HSE_Div1) или поделенную на 2 (RCC_PLLSource_HSE_Div2). Смотри схему */
  RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
  /* Включаем PLL */
  RCC_PLLCmd(ENABLE);
  /* Ждем пока PLL будет готов */
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {
  };
  /* Переключаем системное тактирование на PLL */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
  /* Ждем пока переключиться */
  while (RCC_GetSYSCLKSource() != 0x08)
  {
  };
  return HSEStartUpStatus;
}
// Настройка DMA
void DMA_ini(void)
{
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  DMA_DeInit(DMA1_Channel5); // Сброс настроек DMA
  DMA_InitTypeDef DMA_InitStruct;
  DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t) & (USART1->DR);   // Адрес данных UART
  DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)buf;                   // Адрес буфера памяти
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
// Управление реле
void SwitchRelay(void)
{
  uint8_t FT;
  // Реле 11
  FT = (prot.fst >> 6);
  if (FT)
  { 
    GPIO_ResetBits(GPIOC, LEDpin); ////////////////
    GPIO_SetBits(RelayPORT, Relay_11);
  }
  else
  {
    GPIO_ResetBits(RelayPORT, Relay_11);
    GPIO_SetBits(GPIOC, LEDpin); ///////////////
  };
  // Реле 12
  FT = (prot.fst << 6);
  if (FT)
  {   
    GPIO_SetBits(RelayPORT, Relay_12);
        
  }
  else
  {
    GPIO_ResetBits(RelayPORT, Relay_12);
  }
  // Реле 21
  FT = (prot.snd >> 6);
  if (FT)
  {
    GPIO_SetBits(RelayPORT, Relay_21);
    
  }
  else
  {
    GPIO_ResetBits(RelayPORT, Relay_21);
  }
  // Реле 22
  FT = (prot.snd << 6);
  if (FT)
  {
    GPIO_SetBits(RelayPORT, Relay_22);
  }
  else
  {
    GPIO_ResetBits(RelayPORT, Relay_22);
  }
  // Реле 31
  FT = (prot.trd >> 6);
  if (FT)
  {
    GPIO_SetBits(RelayPORT, Relay_31);
  }
  else
  {
    GPIO_ResetBits(RelayPORT, Relay_31);
  }
  // Реле 32
  FT = (prot.trd << 6);
  if (FT)
  {
    GPIO_SetBits(RelayPORT, Relay_32);
  }
  else
  {
    GPIO_ResetBits(RelayPORT, Relay_32);
  }
}
// Настройка GPIO
void GPIO_ini(void)
{
  // Настройка LED
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitTypeDef PIN_INIT;
  PIN_INIT.GPIO_Pin = LEDpin;
  PIN_INIT.GPIO_Speed = GPIO_Speed_10MHz;
  PIN_INIT.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOC, &PIN_INIT);

  // Настройка пинов реле
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  PIN_INIT.GPIO_Pin = Relay_11 | Relay_12 | Relay_21 | Relay_22 | Relay_31 | Relay_32;
  PIN_INIT.GPIO_Speed = GPIO_Speed_10MHz;
  PIN_INIT.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(RelayPORT, &PIN_INIT);
}
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  ErrorStatus RCCStatus;
  RCCStatus = RCC_ini();
  UART_Init();
  DMA_ini();
  GPIO_ini();
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);

  /*!< At this stage the microcontroller setting
     */

  if (RCCStatus) // Проверка настроек тактирования
  {
    //Send_UART_Str(USART1, "I'm ready!\n\rRCC SUCCESS\n\r");
    //printf("I'm ready!\n\rRCC SUCCESS\n\r");
  }
  else
  {
    //Send_UART_Str(USART1, "I'm ready!\n\rRCC ERROR\n\r");
    //printf("I'm ready!\n\rRCC ERROR\n\r");
  }

  int i = 0, j = 0;
  uint32_t crc;    // Переменная для хранения CRC
  WordToByte word; // Объединение для расчета CRC
  int nErr;        // Количество исправленных ошибок
  uint8_t pkg[12]; // Пакет для обработки
  while (1)
  {

    if (FlagStartByte(&buf[i])) // Если i-й байт - стартовый байт
    {
      Delay_ms(180000);                //  Ждем пока придет остальная часть пакета
      for (j = 0; j < ProtLength; j++) // Копируем пакет из кольцевого буфера
      {
        pkg[j] = buf[i];
        i++;
        if (i >= (uint8_t)sizebuf)
        {
          i = 0; // Обнуляем инкремент кольцевого буфера
        }
      }
      j--;
      i--;                
      nErr = pars(&prot, pkg); // Парсим
      // Объединяем 4 байта протокола в uint32_t для расчета CRC
      word.byte[3] = StartByte;
      word.byte[2] = prot.fst;
      word.byte[1] = prot.snd;
      word.byte[0] = prot.trd;
      // Считаем аппаратно CRC
      CRC_ResetDR();
      CRC_CalcCRC(word.word);
      crc = CRC_GetCRC();
      if ((crc == prot.crc) & !(nErr == -1))
      {
        // Данные корректные, можно работать

        // Включаем/отключаем реле
        SwitchRelay();

        //Nbyte = nErr + Nbyte; // Число исправленных байт
        //nErr = 0;
        crc = 0;
        //TruePkg += 1; // Число исправленных пакетов

        if (j > i)
        {
          buf[sizebuf - j + i] = 0x00; // Стираем стартовый байт (остальное сотрет DMA)
        }
        else
        {
          buf[i - j] = 0x00; // Стираем стартовый байт (остальное сотрет DMA)
        }
      }
      else
      {
        //NerrPkg += 1;
      }

    } // if FLAG

    i++;
    if (i == sizebuf)
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