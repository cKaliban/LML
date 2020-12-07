#include "periph_conf.h"


void MainClockInit(void){
	// Assumed: PSU 2.7-3.3V, clock 100MHz
	// Source: 8MHz Bypass from St-Link v2
		FLASH->ACR =
		            FLASH_ACR_LATENCY_3WS
		          | FLASH_ACR_PRFTEN
		          | FLASH_ACR_ICEN
		          | FLASH_ACR_DCEN;

		RCC->CFGR = RCC_CFGR_PPRE2_DIV2
		        | RCC_CFGR_PPRE1_DIV4;

		RCC->CR |= RCC_CR_HSEBYP;
		RCC->CR |= RCC_CR_HSEON;

		while((RCC->CR & RCC_CR_HSERDY) != RCC_CR_HSERDY);

		RCC->PLLCFGR =
		          RCC_PLLCFGR_PLLSRC_HSE                // HSE = 8MHz
		        | (4 << 0)                              // M = 4
		        | (200 << 6)                            // N = 200
		        | (1 << 16)                             // P = 4
		        | (7 << 24);

		RCC->CR |= RCC_CR_PLLON;

		while((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY);

		RCC->CFGR &= ~RCC_CFGR_SW;
		RCC->CFGR |= RCC_CFGR_SW_PLL;

		while((RCC->CFGR & RCC_CFGR_SW_PLL) != RCC_CFGR_SW_PLL);

		SystemCoreClockUpdate();
		SysTick->CTRL |= SYSTICK_CLKSOURCE_HCLK_DIV8;
}


//PC7, PC9
void PWM__Init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	GPIOC->MODER &= ~(GPIO_MODER_MODER7 | GPIO_MODER_MODER9);
	GPIOC->MODER |= (GPIO_MODER_MODER7_1 | GPIO_MODER_MODER9_1);

	GPIOC->AFR[0] |= GPIO_AFRL_AFRL7_1;
	GPIOC->AFR[1] |= GPIO_AFRH_AFRH1_1;

	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	TIM3->CR1 = 0x0000;
	TIM3->CR2 = 0x0000;
	TIM3->CCER = 0x0000;
	TIM3->PSC = (uint16_t) 50 - 1; // 50Hz line speed -> 1us resolution.

	TIM3->ARR = (uint16_t) 1000 - 1; //100 us -> 10kHz. Correction due to change on nth cycle

	TIM3->CR1 |= TIM_CR1_ARPE;

	TIM3->CCMR1 = 0x0000;
	TIM3->CCMR2 = 0x0000;

	TIM3->CCMR1 |= (0x06 << TIM_CCMR1_OC2M_Pos | TIM_CCMR1_OC2PE);
	TIM3->CCMR2 |= (0x06 << TIM_CCMR2_OC4M_Pos | TIM_CCMR2_OC4PE);

	TIM3->CCR2 = 0;
	TIM3->CCR4 = 0;

	TIM3->CCER |= (TIM_CCER_CC2E | TIM_CCER_CC4E);

	TIM3->BDTR |= TIM_BDTR_MOE;
}


void NVIC_Init(void){
	NVIC_SetPriority(DMA1_Stream5_IRQn, 0);
	NVIC_SetPriority(EXTI0_IRQn, 1);
	NVIC_SetPriority(EXTI1_IRQn, 1);

	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI1_IRQn);
	NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}



