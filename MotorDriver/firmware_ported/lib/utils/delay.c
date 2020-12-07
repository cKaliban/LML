#include "delay.h"

void delayInit(void){
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	delay_ms(1);
}


void delay_us(uint16_t delay){
	uint16_t Prescaler = (uint16_t) (SystemCoreClock / 2/ 1000000) - 1;
	TIM6->SR = 0;
	TIM6->PSC = Prescaler;
	TIM6->ARR = delay;
	TIM6->CR1 |= TIM_CR1_CEN;
	while(!(TIM6->SR & TIM_SR_UIF));
}

void delay_ms(uint16_t delay){
	uint16_t Prescaler = (uint16_t) (SystemCoreClock / 2 / 1000) - 1;
	TIM6->CNT = 0;
	TIM6->SR = 0;
	TIM6->PSC = Prescaler;
	TIM6->ARR = delay;
	TIM6->CR1 |= TIM_CR1_CEN;
	TIM6->SR = 0;
	while(!(TIM6->SR & TIM_SR_UIF));
}
