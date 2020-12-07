#include "periph_conf.h"


//PA5
void PWM__Init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	GPIOA->MODER &= ~(GPIO_MODER_MODER5);
	GPIOA->MODER |= (GPIO_MODER_MODER5_1);

	GPIOA->AFR[0] |= GPIO_AFRL_AFRL5_0;
//	GPIOC->AFR[1] |= GPIO_AFRH_AFRH1_1;

	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	TIM2->CR1 = 0x0000;
//	TIM2->CR2 = 0x0000;
	TIM2->CCER = 0x0000;
	TIM2->PSC = (uint16_t) 50 - 1; // 50Hz line speed -> 1us resolution.

	TIM2->ARR = (uint16_t) 1000 - 1; //1000 us -> 1kHz. Correction due to change on nth cycle

	TIM2->CR1 |= TIM_CR1_ARPE;

	TIM2->CCMR1 = 0x0000;
//	TIM2->CCMR2 = 0x0000;

	TIM2->CCMR1 |= (0x06 << TIM_CCMR1_OC1M_Pos | TIM_CCMR1_OC1PE);
//	TIM2->CCMR2 |= (0x06 << TIM_CCMR2_OC4M_Pos | TIM_CCMR2_OC4PE);

	TIM2->CCR2 = 0;
	TIM2->CCR4 = 0;

	TIM2->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC4E);

	TIM2->BDTR |= TIM_BDTR_MOE;
}


void watch__init(){
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
	TIM7->CR1 &= ~TIM_CR1_CEN;
	TIM7->CR1 = 0x0000;
	//	TIM2->CR2 = 0x0000;
	TIM7->CCER = 0x0000;
	TIM7->PSC = (uint16_t) 50000 - 1; // 50MHz line speed / 50000 -> 1ms resolution.
	TIM7->ARR = (uint16_t) 2000 - 1; // 2000ms - 2s

	TIM7->EGR |= TIM_EGR_UG;
	TIM7->DIER |= TIM_DIER_UIE;
}

inline void watch__start(){
	TIM7->CR1 |= TIM_CR1_CEN;
}

inline void watch__reset(){
	TIM7->CNT = 0;
//	TIM7->CNT = 1;
}

inline void watch__stop(){
	TIM7->CR1 &= ~TIM_CR1_CEN;
	TIM7->CNT = 0;
}
