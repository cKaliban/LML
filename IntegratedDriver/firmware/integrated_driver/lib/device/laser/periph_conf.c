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


void subwatch__init(){

//	TIM4->CR1 &= ~TIM_CR1_CEN;
//	TIM4->CR1 = 0x0000;
//	//	TIM2->CR2 = 0x0000;
//	TIM4->CCER = 0x0000;
//	TIM4->PSC = (uint16_t) 50000 - 1; // 50MHz line speed / 50000 -> 1ms resolution.
//	TIM4->ARR = (uint16_t) 10 - 1; // 2000ms - 2s
//
//	TIM4->EGR |= TIM_EGR_UG;
//	TIM4->DIER |= TIM_DIER_UIE;
}


void freq_measurement_init(){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	GPIOB->MODER &= ~(GPIO_MODER_MODER7);
	GPIOB->MODER |= (GPIO_MODER_MODER7_1);
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR7);
//	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR7_0; // BE CAREFUL WITH THIS
	GPIOB->AFR[0] |= GPIO_AFRL_AFRL7_1;


	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	TIM4->CCER = 0x0000;

	TIM4->SMCR &= ~(TIM_SMCR_TS);
	TIM4->SMCR |= (TIM_SMCR_TS_1 | TIM_SMCR_TS_2);

	TIM4->SMCR &= ~(TIM_SMCR_SMS);
	TIM4->SMCR |= TIM_SMCR_SMS_2;
	TIM4->CCMR1 |= TIM_CCMR1_CC2S_0; // Should be incorrect, but what do I know?
//	TIM4->CCMR1 |= (0x1 << TIM_CCMR1_IC2F_Pos);
	TIM4->CCER |= (0x1 << TIM_CCER_CC2P_Pos);
	TIM4->CCER |= TIM_CCER_CC2E;
	TIM4->PSC = (uint16_t) 50 - 1; // 50MHz line speed / 50 -> 1us resolution.
	TIM4->ARR = (uint16_t) 500 - 1 ; //
	TIM4->CR1 |= TIM_CR1_URS;
	TIM4->DIER |= TIM_DIER_UIE;

	TIM4->EGR |= TIM_EGR_UG;

	TIM4->CR1 |= TIM_CR1_CEN;

}


inline uint16_t freq_measurement_read(){
	uint16_t period = (uint16_t) TIM4->CCR2;
	return period;
}

