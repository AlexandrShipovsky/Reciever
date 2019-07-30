/**
  ******************************************************************************
  * @file    protV/protV.h
  * @author  Alexandr Shipovsky
  * @version V0.0.1
  * @date    5-august-2019
  * @brief   Main program body
  ******************************************************************************
  * @attention
  * Библиотека для работы 
  * 
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PROTV_H
#define __PROTV_H

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
/* Exported types ------------------------------------------------------------*/
/** 
  *   Cтруктура протокола 
  */
typedef struct
{
  uint8_t fst;  // Первый байт данных
  uint8_t snd;  // Второй байт данных
  uint8_t trd;  // Третий байт данных
  uint32_t crc; // Контрольная сумма
} protVstructure;
/** 
  *   Объединение типов uint32_t и 4 байта
  */
 typedef union
 {
   uint32_t word;
   uint8_t byte[4];
 } WordToByte;
 
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define StartByte ((uint8_t)0x63)

/* Exported functions ------------------------------------------------------- */
/** 
  *   Парсер строки байт в элементы протокола
  */
void pars(protVstructure *prot, uint8_t *str);
/** 
  *   Флаг стартового байта (StartByte)
  */
_Bool FlagStartByte(uint8_t *str);
/**
  * @}
  */
#endif /* __PROTV_H */