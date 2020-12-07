/*
 * driver.c
 *
 *  Created on: 06.01.2020
 *      Author: ckali
 */
#include "pulsegen.h"

// Debugging macros
#define PULSE_NUMBER	1
#define PERIOD			20000


pulse_config_t h_pulse;
pulse_config_t v_pulse;


/*	IMPLEMENTATION DETAILS			*/
static float Rcoeff(uint32_t, uint32_t, uint32_t);
static float initialPeriod(uint32_t, uint32_t, uint32_t);
static float nextPeriod_ver_a(float, float);
//static float nextPeriod_ver_b(float, float);


/*	HORIZONTAL STATIC FUNCTIONS		*/
//TODO: pack init into single function
static void port__H_Init(void);
static void pulse__H_Init(void);
static void pulse__H_Enable(void);
static void pulse__H_Disable(void);
static void pulse__H_Lock(void);
static void pulse__H_Unlock(void);
static void pulse__H_Home(void);
//static void pulse__H_Calibrate(void);
//static void pulse__H_SinglePass(void);
static void pulse__H_GenOnePulse(uint16_t);
static uint16_t pulse__H_acc(uint16_t, float);
static void pulse__H_Traverse(uint16_t);
static void pulse__H_deacc(uint16_t, float);
static void pulse__H_Toggle(void);


/*	VERTICAL STATIC FUNCTIONS		*/
static void port__V_Init(void);
static void pulse__V_Init(void);
static void pulse__V_Enable(void);
static void pulse__V_Disable(void);
static void pulse__V_Lock(void);
static void pulse__V_Unlock(void);
static void pulse__V_Home(void);
static void pulse__V_GenOnePulse(uint16_t);
static void pulse__V_Clean(void);



/*	GLOBAL WRAPPERS IMPLEMENTATION	*/
void pulse__Init(pulse_config_t * self, enum axis_t axis){
	self->max_frq = 1000;
	self->lower_limit = 0;
	self->upper_limit = 10000;
	self->axis = axis;
	self->status = NOT_CALIBRATED;
	self->busy = PULSE_IDLE;

	if(self->axis == HORIZONTAL){
		self->portSetup = &port__H_Init;
		self->timerSetup = &pulse__H_Init;

		self->enable = &pulse__H_Enable;
		self->disable = &pulse__H_Disable;
		self->lock = &pulse__H_Lock;
		self->unlock = &pulse__H_Unlock;

		self->calibrate = &pulse__H_Calibrate;
		self->home = &pulse__H_Home;
		self->run = &pulse__H_SinglePass;
		self->special = &pulse__H_Toggle;
	}
	else if (self->axis == VERTICAL) {
		self->portSetup = &port__V_Init;
		self->timerSetup = &pulse__V_Init;

		self->enable = &pulse__V_Enable;
		self->disable = &pulse__V_Disable;
		self->lock = &pulse__V_Lock;
		self->unlock = &pulse__V_Unlock;

		self->calibrate = &pulse__V_Calibrate;
		self->home = &pulse__V_Home;
		self->run = &pulse__V_SinglePass;
		self->special = &pulse__V_Clean;

		self->first_layer_depth = 100;
		self->default_layer_depth = 100;
	}
	else{
		for(;;);
	}
}


/*	HORIZONTAL FUNCTIONS IMPLEMENTATION */
//	TODO change to static,provide only structure access by pointer
//	TODO when possible pack into single function with pointer arguments
static void port__H_Init(){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER &= ~GPIO_MODER_MODER8;
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD8_Msk);
	GPIOA->MODER |= GPIO_MODER_MODER8_1;
	GPIOA->OTYPER |= GPIO_OTYPER_OT8;
	GPIOA->AFR[1] |= GPIO_AFRH_AFRH0_0;
}
static void pulse__H_Init(){
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	uint16_t Prescaler = (uint16_t) (SystemCoreClock / 1000000) - 1; // 100MHz APB2 bus
	TIM1->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
	TIM1->CR1 |= TIM_CR1_DIR;
	TIM1->CR1 &= ~TIM_CR1_CKD;

	TIM1->SMCR = RESET;
	TIM1->CR1 |= TIM_CR1_OPM;
	TIM1->CCMR1 &= (uint16_t)~TIM_CCMR1_OC1M;
	TIM1->CCMR1 &= (uint16_t)~TIM_CCMR1_CC1S;
//	TIM1->CCMR1 |= TIM_CCMR1_OC1M; // PWM2 for now
	TIM1->CCMR1 |= (0x06 << TIM_CCMR1_OC1M_Pos); // PWM1 as alternative

	TIM1->CCER &= (uint16_t)~TIM_CCER_CC1P;
	TIM1->CCER |= TIM_CCER_CC1P;
//	TIM1->CCER &= ~(TIM_CCER_CC1P);
	TIM1->CCER |= TIM_CCER_CC1E;

	TIM1->CR1 |= TIM_CR1_ARPE;
	TIM1->BDTR |= TIM_BDTR_OSSI; // TODO: determine if this better solution than just lock output on pins.
	TIM1->BDTR |= TIM_BDTR_LOCK;
	TIM1->CCMR1 |= TIM_CCMR1_OC1PE;

	TIM1->ARR = PERIOD - 1;
	TIM1->CCR1 = PULSE;
	TIM1->PSC = Prescaler;
	TIM1->RCR = 0;

	TIM1->EGR = TIM_EGR_UG;
	TIM1->DIER |= TIM_DIER_UIE;
	TIM1->BDTR |= TIM_BDTR_MOE;
//	TIM1->CR1 |= TIM_CR1_CEN; /* Testing only */
}
static inline void pulse__H_Lock(){
	TIM1->BDTR &= ~(TIM_BDTR_MOE);
}
static inline void pulse__H_Unlock(){
	TIM1->BDTR |= TIM_BDTR_MOE;
}
static inline void pulse__H_Enable(){
	h_pulse.busy = PULSE_BUSY;
	TIM1->CR1 |= TIM_CR1_CEN;
	if(h_control.current_direction == RIGHT)
		h_pulse.current_position--;
	if(h_control.current_direction == LEFT)
		h_pulse.current_position++;
}
static inline void pulse__H_Disable(){
	TIM1->CR1 &= ~TIM_CR1_CEN;
	TIM1->EGR |= TIM_EGR_UG;
}
static inline void pulse__H_GenOnePulse(uint16_t period){
	TIM1->ARR = period;
}
void pulse__H_ConstPulse(uint32_t steps, uint16_t period){ // TODO test version with loop only
	while(h_pulse.busy != PULSE_IDLE);
	pulse__H_GenOnePulse(period);
	pulse__H_Enable();

	for(uint32_t i = 1; i < steps; i++){
		while(h_pulse.busy != PULSE_IDLE);
		pulse__H_GenOnePulse(period);
		pulse__H_Enable();
	}
}
void pulse__H_Calibrate(){ // TODO find optimal calibration speed
	pulse__H_ConstPulse(1, 10000);
}
void pulse__H_Home(){ // TODO find optimal homing speed
	if(h_pulse.current_position > h_pulse.lower_limit){
		h_control.setDir(RIGHT);
		pulse__H_ConstPulse(1,10000);
	}
	else if(h_pulse.current_position < h_pulse.lower_limit){
		h_control.setDir(LEFT);
		pulse__H_ConstPulse(1,10000);
	}
}
void pulse__H_SinglePass(uint32_t Vmax){
	float R_acc = -Rcoeff(Vmax, 1000000, h_pulse.cc1_limit - h_pulse.lower_limit);
	float R_deacc;
	uint16_t init_period = initialPeriod(Vmax, 1000000,h_pulse.cc1_limit - h_pulse.lower_limit);
//	R_deacc = Rcoeff(Vmax, 1000000, h_pulse.upper_limit - h_pulse.cc2_limit);
	R_deacc = -R_acc;

	//-----------
	uint16_t transition_period = pulse__H_acc(init_period, R_acc);
//	pulseHConstPulse(h_pulse.cc2_limit - h_pulse.cc1_limit, transition_period);
	pulse__H_Traverse(transition_period);
	pulse__H_deacc(transition_period, R_deacc);
}
static uint16_t pulse__H_acc(uint16_t period, float R){
	uint16_t new_period = period;
	float last_period_f = 0.0;
	float next_period_f =  period;
	uint16_t next_period = (uint16_t) next_period_f;

	if(h_control.current_direction == LEFT){
		while(h_pulse.current_position < h_pulse.cc1_limit){
			pulse__H_GenOnePulse(new_period);
			pulse__H_Enable();
			new_period = next_period;
			last_period_f = next_period_f;
			next_period_f = nextPeriod_ver_a(last_period_f, R);
			next_period = (uint16_t) next_period_f;
			while(h_pulse.busy != PULSE_IDLE);
		}
	}
	if(h_control.current_direction == RIGHT){
		while(h_pulse.current_position > h_pulse.cc2_limit){
			pulse__H_GenOnePulse(new_period);
			pulse__H_Enable();
			new_period = next_period;
			last_period_f = next_period_f;
			next_period_f = nextPeriod_ver_a(last_period_f, R);
			next_period = (uint16_t) next_period_f;
			while(h_pulse.busy != PULSE_IDLE);
		}
	}
	return new_period;
}

static void pulse__H_deacc(uint16_t period, float R){
	uint16_t new_period = period;
	float last_period_f = period;
	float next_period_f =  period;
	uint16_t next_period = (uint16_t) next_period_f;
	while(h_pulse.busy != PULSE_IDLE);

	if(h_control.current_direction == LEFT){
		while(h_pulse.current_position < h_pulse.upper_limit){
			pulse__H_GenOnePulse(new_period);
			pulse__H_Enable();
			new_period = next_period;
			last_period_f = next_period_f;
			next_period_f = nextPeriod_ver_a(last_period_f, R);
			next_period = (uint16_t) next_period_f;
			if(next_period > 65000)
				next_period = 65000;
			while(h_pulse.busy != PULSE_IDLE);
		}
	}
	if(h_control.current_direction == RIGHT){
		while(h_pulse.current_position > h_pulse.lower_limit){
			pulse__H_GenOnePulse(new_period);
			pulse__H_Enable();
			new_period = next_period;
			last_period_f = next_period_f;
			next_period_f = nextPeriod_ver_a(last_period_f, R);
			next_period = (uint16_t) next_period_f;
			if(next_period > 65000)
				next_period = 65000;
			while(h_pulse.busy != PULSE_IDLE);
		}
	}

}
static void pulse__H_Traverse(uint16_t traverse_period){
	while((h_pulse.current_position <= h_pulse.cc2_limit) &&
			(h_pulse.current_position >= h_pulse.cc1_limit)){
		pulse__H_GenOnePulse(traverse_period);
		pulse__H_Enable();
		while(h_pulse.busy != PULSE_IDLE);
	}
}

//static void pulse__H_Toggle(){
//	uint32_t Vmax = h_pulse.max_frq;
//	if(h_pulse.current_position == h_pulse.lower_limit){
//		h_control.setDir(LEFT);
//		h_pulse.run(Vmax);
////		stepper_control.axes.X_axis.pulse->run(Vmax);
////		stepper_control.axes.X_axis.control->setDir(LEFT);
////		stepper_control.device_state = READY;
////		stepper_control.current_task = TASK_NONE;
//	}
//	else if(h_pulse.current_position == h_pulse.upper_limit){
////		stepper_control.axes.X_axis.control->setDir(RIGHT);
////		stepper_control.axes.X_axis.pulse->run(stepper_control.axes.X_axis.pulse->max_frq);
//		h_control.setDir(RIGHT);
//		h_pulse.run(Vmax);
////		stepper_control.device_state = FINISHED;
////		stepper_control.current_task = TASK_NONE;
//	}
//}

static void pulse__H_Toggle(){
	while(h_pulse.current_position != h_pulse.center_pos){
		if(h_pulse.current_position > h_pulse.center_pos){
			h_control.setDir(RIGHT);
			pulse__H_ConstPulse(1,10000);
		}
		else if(h_pulse.current_position < h_pulse.center_pos){
			h_control.setDir(LEFT);
			pulse__H_ConstPulse(1,10000);
		}
	}
}


/*	VERTICAL FUNCTIONS IMPLEMENTATION */
static void port__V_Init(){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER &= ~GPIO_MODER_MODER6;
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD6_Msk);
	GPIOA->MODER |= GPIO_MODER_MODER6_1;
	GPIOA->OTYPER |= GPIO_OTYPER_OT6;
	GPIOA->AFR[0] |= GPIO_AFRL_AFRL6_1;
}
static void pulse__V_Init(){	//	TODO: check if ticking value is correct
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	uint16_t Prescaler = (uint16_t) (SystemCoreClock / (1000000 / 2)) - 1; // 50MHz APB1
	TIM3->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
	TIM3->CR1 |= TIM_CR1_DIR;
	TIM3->CR1 &= ~TIM_CR1_CKD;

	TIM3->SMCR = RESET;
	TIM3->CR1 |= TIM_CR1_OPM;
	TIM3->CCMR1 &= (uint16_t)~TIM_CCMR1_OC1M;
	TIM3->CCMR1 &= (uint16_t)~TIM_CCMR1_CC1S;
	TIM3->CCMR1 |= (0x06 << TIM_CCMR1_OC1M_Pos); // PWM1
//	TIM3->CCMR1 |= (0x07 << TIM_CCMR1_OC1M_Pos);
	TIM3->CCER &= (uint16_t)~TIM_CCER_CC1P;
	TIM3->CCER |= TIM_CCER_CC1P;
	TIM3->CCER |= TIM_CCER_CC1E;

	TIM3->CR1 |= TIM_CR1_ARPE;
	TIM3->BDTR |= TIM_BDTR_LOCK;
	TIM3->CCMR1 |= TIM_CCMR1_OC1PE;

	TIM3->ARR = PERIOD - 1;
	TIM3->CCR1 = 100 - 1;
	TIM3->PSC = Prescaler;
	TIM3->RCR = 0;

	TIM3->EGR = TIM_EGR_UG;
	TIM3->DIER |= TIM_DIER_UIE;
//	TIM3->BDTR |= TIM_BDTR_MOE;
}
inline inline void pulse__V_Lock(){
//	TIM3->BDTR &= ~(TIM_BDTR_MOE);
}
inline inline void pulse__V_Unlock(){
//	TIM3->BDTR |= TIM_BDTR_MOE;
}
static inline void pulse__V_Enable(){
	v_pulse.busy = PULSE_BUSY;
	TIM3->CR1 |= TIM_CR1_CEN;
	if(v_control.current_direction == UP)
		v_pulse.current_position--;
	if(v_control.current_direction == DOWN)
		v_pulse.current_position++;
}
static inline void pulse__V_Disable(){
	TIM3->CR1 &= ~TIM_CR1_CEN;
	TIM3->EGR |= TIM_EGR_UG;
}
static inline void pulse__V_GenOnePulse(uint16_t period){
	TIM3->ARR = period;
}
void pulse__V_ConstPulse(uint32_t steps, uint16_t period){ // TODO test version with loop only
	while(v_pulse.busy != PULSE_IDLE);
	pulse__V_GenOnePulse(period);
	pulse__V_Enable();

	for(uint32_t i = 1; i < steps; i++){
		while(v_pulse.busy != PULSE_IDLE);
		pulse__V_GenOnePulse(period);
		pulse__V_Enable();
	}
}
void pulse__V_Calibrate(){
	pulse__V_ConstPulse(1, 1000);
}
void pulse__V_Home(){
	if(v_pulse.current_position > v_pulse.lower_limit){
		v_control.setDir(UP);
		pulse__V_ConstPulse(1,2000);
	}
	else if(v_pulse.current_position < v_pulse.lower_limit){
		v_control.setDir(DOWN);
		pulse__V_ConstPulse(1,2000);
	}
}
void pulse__V_SinglePass(uint32_t steps){
	if(steps > 0){
		for(uint32_t i = 0; i < steps; i++)
			pulse__V_ConstPulse(1,2000);
	}
}

void pulse__V_Clean(){

}
/*	RAMP COEFFICIENT CALCULACTION	*/
// ****************************************************
static inline float Rcoeff(uint32_t Vmax, uint32_t Ftim, uint32_t distance){
	float a = (float) Vmax / Ftim;
	float b = 2 * distance;
	float R = (a / b) * a;
	return R;
}
static inline float nextPeriod_ver_a(float p, float m){
	float result = p * (1 + m * p * p);;
	return result;
}
static inline float initialPeriod(uint32_t Vmax, uint32_t F_tim, uint32_t distance){
	uint16_t initial_period;
	uint32_t a = F_tim / Vmax;
	float initial_period_f = a * sqrtf(distance);
	if(initial_period_f < 65000.0)
		initial_period = (uint16_t) initial_period_f;
	else
		initial_period = 65000;
	return initial_period;
}
//static inline float nextPeriod_ver_b(float p, float m){
//	float q = m * p * p;
//	return p * (1 + q + q * q);
//}
