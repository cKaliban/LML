#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_
#include "stm32f4xx.h"
#include "timing_rough.h"


//void MainClockInit(void);
void watch__init(void);
void watch__start(void);
void watch__reset(void);
void watch__stop(void);
void PWM__Init(void); //TIM2, PA5

void freq_measurement_init(void);
uint16_t freq_measurement_read(void);
//void NVIC_Init(void);


#endif
