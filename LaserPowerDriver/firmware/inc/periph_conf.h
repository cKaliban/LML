#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_
#include "stm32f4xx.h"
#include "timing_rough.h"


void MainClockInit(void);

void PWM__Init(void); //TIM3, PC7 and PC6

void NVIC_Init(void);


#endif
