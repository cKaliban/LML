#include "control_signals.h"

static void control__H_Init(void);
static void control__H_Enable(void);
static void control__H_Disable(void);
static void control__H_SetDir(dir_t);
//static void control__HChangeDir(void);

static void control__V_Init(void);
static void control__V_Enable(void);
static void control__V_Disable(void);
static void control__V_SetDir(dir_t);
//static void control__VChangeDir(void);



control_signal_t h_control;
control_signal_t v_control;



void control__Init(control_signal_t * self, axis_t axis){
	self->axis = axis;
	if(self->axis == HORIZONTAL){
		self->portSetup = &control__H_Init;
		self->enable = &control__H_Enable;
		self->disable = &control__H_Disable;
		self->setDir = &control__H_SetDir;
		self->setDir(RIGHT);
//		self->current_direction = RIGHT;
//		self->changeDir = &control__HChangeDir;
	}
	else if (self->axis == VERTICAL) {
		self->portSetup = &control__V_Init;
		self->enable = &control__V_Enable;
		self->disable = &control__V_Disable;
		self->setDir = &control__V_SetDir;
//		self->changeDir = &control__VChangeDir;
		self->setDir(UP);
//		self->current_direction = UP;
	}
	else{
		for(;;);
	}
}

static void control__H_Init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	GPIOB->MODER &= ~(GPIO_MODER_MODE15_Msk | GPIO_MODER_MODE14_Msk);
	GPIOB->MODER |= ((0x01 << GPIO_MODER_MODE15_Pos) | (0x01 << GPIO_MODER_MODE14_Pos));
//	GPIOB->OTYPER |= GPIO_OTYPER_OT15;
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT15);
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT14);
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD15_Msk | GPIO_PUPDR_PUPD14_Msk);

	GPIOB->BSRR |= (GPIO_BSRR_BR_15 | GPIO_BSRR_BR_14);
}
static inline void control__H_Enable(void){
//	GPIOB->BSRR |= GPIO_BSRR_BS14;
	GPIOB->BSRR |= GPIO_BSRR_BR14;
}
static inline void control__H_Disable(void){
//	GPIOB->BSRR |= GPIO_BSRR_BR14;
	GPIOB->BSRR |= GPIO_BSRR_BS14;
}
static inline void control__H_SetDir(dir_t direction){
	if(direction == LEFT){
//		GPIOB->BSRR |= GPIO_BSRR_BS15;
		GPIOB->BSRR |= GPIO_BSRR_BR15;
		h_control.current_direction = LEFT;
	}
	if(direction == RIGHT){
//		GPIOB->BSRR |= GPIO_BSRR_BR15;
		GPIOB->BSRR |= GPIO_BSRR_BS15;
		h_control.current_direction = RIGHT;
	}
}
//static inline void control__HChangeDir(void){
//	if(h_control.current_direction == LEFT){
//		GPIOB->BSRR |= GPIO_BSRR_BR15;
//		h_control.current_direction = RIGHT;
//	}
//	else if(h_control.current_direction == RIGHT){
//		GPIOB->BSRR |= GPIO_BSRR_BS15;
//		h_control.current_direction = LEFT;
//	}
//}

static void control__V_Init(){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	GPIOB->MODER &= ~(GPIO_MODER_MODE1_Msk | GPIO_MODER_MODE2_Msk);
	GPIOB->MODER |= ((0x01 << GPIO_MODER_MODE1_Pos) | (0x01 << GPIO_MODER_MODE2_Pos));
//	GPIOB->OTYPER |= GPIO_OTYPER_OT2;
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT2);
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT1);
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD1_Msk | GPIO_PUPDR_PUPD2_Msk);

	GPIOB->BSRR |= (GPIO_BSRR_BR_1 | GPIO_BSRR_BR_2);
}
static inline void control__V_Enable(void){
//	GPIOB->BSRR |= GPIO_BSRR_BS1;
	GPIOB->BSRR |= GPIO_BSRR_BR1;
}
static inline void control__V_Disable(void){
//	GPIOB->BSRR |= GPIO_BSRR_BR1;
	GPIOB->BSRR |= GPIO_BSRR_BS1;
}
//static inline void control__VChangeDir(void){
//	GPIOB->ODR ^= GPIO_ODR_OD2;
//}
static inline void control__V_SetDir(dir_t direction){
	if(direction == UP){
		GPIOB->BSRR |= GPIO_BSRR_BS2;
		v_control.current_direction = UP;
	}
	if(direction == DOWN){
		GPIOB->BSRR |= GPIO_BSRR_BR2;
		v_control.current_direction = DOWN;
	}
}
