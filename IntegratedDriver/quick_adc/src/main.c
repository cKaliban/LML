/**
  ******************************************************************************

  * @file    main.c
  * @author  Mateusz Wieczorek
  * @version V1.0
  * @brief   Main function of motor control device
  ******************************************************************************
*/

#include "stm32f4xx.h"
#include "stdint.h"


void clk_init(void);
void adc_init(void);
void uart_init(void);


int main(void)
{
	/*	DEVICE CONFIGURATION	*/
	clk_init();
	adc_init();

	for(;;){
	}
}


void NVIC_Init(){

}

void adc_init(){
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;



}

void uart_init(){

}

void clk_init(void){
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
