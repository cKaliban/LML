#include "limit_switches.h"

switch_control_t h_switch;
switch_control_t v_switch;



void rightSwitchInit(void);
void leftSwitchInit(void);
void upperSwitchInit(void);
void lowerSwitchInit(void);

void limitSwitchInit(void){
	rightSwitchInit();
	leftSwitchInit();
	upperSwitchInit();
	lowerSwitchInit();
}

void debug_XSwitchInit(void){
	rightSwitchInit();
	leftSwitchInit();
}

void debug_YSwitchInit(void){
	upperSwitchInit();
	lowerSwitchInit();
}


// TODO: pack the initialization into single function

//------------------------------------

void rightSwitchInit(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	GPIOC->MODER &= ~GPIO_MODER_MODER0_Msk;
	GPIOC->MODER |= (0x00 << GPIO_MODER_MODER0_Pos);
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD0;

	SYSCFG->EXTICR[0] &= ~ SYSCFG_EXTICR1_EXTI0_Msk;
	SYSCFG->EXTICR[0] |=   SYSCFG_EXTICR1_EXTI0_PC;

	EXTI->IMR |= EXTI_IMR_IM0;

	EXTI->RTSR &= ~EXTI_RTSR_TR0;
	EXTI->FTSR |= EXTI_FTSR_TR0;
}

void leftSwitchInit(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	GPIOC->MODER &= ~GPIO_MODER_MODER1_Msk;
	GPIOC->MODER |= (0x00 << GPIO_MODER_MODER1_Pos);
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD1;

	SYSCFG->EXTICR[0] &= ~ SYSCFG_EXTICR1_EXTI1_Msk;
	SYSCFG->EXTICR[0] |=   SYSCFG_EXTICR1_EXTI1_PC;

	EXTI->IMR |= EXTI_IMR_IM1;

	EXTI->RTSR &= ~EXTI_RTSR_TR1;
	EXTI->FTSR |= EXTI_FTSR_TR1;
}

//------------------------------------

void upperSwitchInit(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	GPIOC->MODER &= ~GPIO_MODER_MODER2_Msk;
	GPIOC->MODER |= (0x00 << GPIO_MODER_MODER2_Pos);
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD2;

	SYSCFG->EXTICR[0] &= ~ SYSCFG_EXTICR1_EXTI2_Msk;
	SYSCFG->EXTICR[0] |=   SYSCFG_EXTICR1_EXTI2_PC;

	EXTI->IMR |= EXTI_IMR_IM2;

	EXTI->RTSR &= ~EXTI_RTSR_TR2;
	EXTI->FTSR |= EXTI_FTSR_TR2;
}

void lowerSwitchInit(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	GPIOC->MODER &= ~GPIO_MODER_MODER3_Msk;
	GPIOC->MODER |= (0x00 << GPIO_MODER_MODER3_Pos);
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD3;

	SYSCFG->EXTICR[0] &= ~ SYSCFG_EXTICR1_EXTI3_Msk;
	SYSCFG->EXTICR[0] |=   SYSCFG_EXTICR1_EXTI3_PC;

	EXTI->IMR |= EXTI_IMR_IM3;

	EXTI->RTSR &= ~EXTI_RTSR_TR3;
	EXTI->FTSR |= EXTI_FTSR_TR3;
}
