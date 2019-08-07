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

/** 
  * Парсер пакета ( с кодировкой Рида-Соломона исправляющий 2 ошибки(можно изменить))
  * Возвращает количество исправленных ошибок
  * Возвращает -1 Если не удалось исправить все ошибки
  */
int pars(protVstructure *prot, uint8_t *str)
{
  int nErr; // Количество найденных ошибок
  WordToByte word;
  str++;                             // Пропускаем стартовый байт
  c_form(NumbOfErr, ProtLength - 1); // Будем исправлять 2 ошибки, в буфере длиной ProtLength - 1 байт (за вычетом стартового)
  nErr = c_decode(str);              // Теперь str длиной 11 содержит 8 байт исходной информации
  if (nErr == -1)                    // Если не удалось исправить вернуть -1
  {
    return nErr;
  }
  prot->fst = *str; // Парсинг строки
  str++;
  prot->snd = *str;
  str++;
  prot->trd = *str;
  str++;
  word.byte[3] = *str;
  str++;
  word.byte[2] = *str;
  str++;
  word.byte[1] = *str;
  str++;
  word.byte[0] = *str;
  prot->crc = word.word;
  return nErr;
}
/** 
  * Объединяет значения структуры протокола в строку
  * Кодирует информационное сообщение
  */
void UnitBuf(protVstructure *prot, uint8_t *buf)
{
  uint8_t i = 0;
  WordToByte crc;
  crc.word = prot->crc;
  buf[i] = StartByte;
  i++;
  buf[i] = prot->fst;
  i++;
  buf[i] = prot->snd;
  i++;
  buf[i] = prot->trd;
  i++;
  buf[i] = crc.byte[3];
  i++;
  buf[i] = crc.byte[2];
  i++;
  buf[i] = crc.byte[1];
  i++;
  buf[i] = crc.byte[0];
  // Кодируем информацию без стартового байта
  c_form(NumbOfErr, ProtLength - 1); // Будем исправлять 2 ошибки, в буфере длиной ProtLength - 1 байт (за вычетом стартового)
  c_code(&buf[1]);                   // Тепрь buf длиной 12 содержит 1 стартовый байт 4 кодовых байта 3 информационных и 4 байта CRC32
}