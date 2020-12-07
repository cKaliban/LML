#ifndef DELAY_H
#define DELAY_H

#include "stm32f4xx.h"

void delayInit(void);
void delay_us(uint16_t);
void delay_ms(uint16_t);

#endif /* !DELAY_H */
