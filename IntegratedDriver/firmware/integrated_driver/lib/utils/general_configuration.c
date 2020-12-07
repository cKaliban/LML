#include "general_configuration.h"

//===================================
/* Main clock and delay timers configuration */
void mainClockInit(void){
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
}


void buttonInit(){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	GPIOC->MODER &= ~GPIO_MODER_MODER13_Msk;
	GPIOC->MODER |= (0x00 << GPIO_MODER_MODER13_Pos);
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13;

//	SYSCFG->EXTICR[0] &= ~ SYSCFG_EXTICR1_EXTI13_Msk;
//	SYSCFG->EXTICR[0] |=   SYSCFG_EXTICR1_EXTI13_PC;
	SYSCFG->EXTICR[3] &= ~ SYSCFG_EXTICR4_EXTI13_Msk;
	SYSCFG->EXTICR[3] |=   SYSCFG_EXTICR4_EXTI13_PC;

	EXTI->IMR |= EXTI_IMR_IM13;
//	EXTI->RTSR |= EXTI_RTSR_TR13;
	EXTI->FTSR |= EXTI_FTSR_TR13;
}
