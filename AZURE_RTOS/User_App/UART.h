#ifndef __UART_H__
#define __UART_H__
#include "main.h"

void PrintfInit();
void PushBuff(uint8_t *addr, uint32_t len);
uint32_t PopBuff(uint8_t * addr);

void PrintfDec(uint32_t vlu);
void PrintfHex(uint8_t *addr, uint32_t len);
void Printf(uint8_t *addr, uint32_t len);

void UART1TaskCreate();

#endif