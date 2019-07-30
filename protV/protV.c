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

/* Includes ------------------------------------------------------------------*/
#include "protV.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Парсер строки байт str в элементы протокола
  * @param  None
  * @retval None
  */
void pars(protVstructure *prot, uint8_t *str)
{
  WordToByte word;
  prot->fst = *str;
  str++;
  prot->snd = *str;
  str++;
  prot->trd = *str;
  str++;
  word.byte[0] = *str;
  str++;
  word.byte[1] = *str;
  str++;
  word.byte[2] = *str;
  str++;
  word.byte[3] = *str;
  prot->crc = word.word;
}

/** 
  *   Флаг стартового байта (StartByte)
  */
_Bool FlagStartByte(uint8_t *str)
{
  if (*str == StartByte)
  {
    return 1;
  }
  return 0;
}
