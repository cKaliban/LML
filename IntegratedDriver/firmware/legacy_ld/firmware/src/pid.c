/*
 * pid.c
 *
 *  Created on: 08.01.2020
 *      Author: ckali
 */

#include "pid.h"

T_pid_t PWMpid = {
		.status = PIDS_IDLE,
		.transition = PIDS_IDLE,

		.Tp_hex = 1000, // Tp = 0
//		.Tp_tol = 1,
		.Tp_tol_high = 1010,
		.Tp_tol_low = 990,
		.Kp = 0.8f,
		.Ki = 0.01f,
		.Kd = 0.001f,

		.err = 0,
		.last_err = 0,
		.epsilon = 10,
		.integral = 0,
		.max_integral = 10000,
		.derivative = 0,

		.period = 999,
		.out_init = 40,
		.out = 40,
		.max_out = 40,
		.min_out = 1, //force tickle at least

		.stable_count = 0
};


void PID__Step(T_pid_t * pid){
//	USART_PutChar(0x01);
	pid->last_err = pid->err;
	Pyro__Query(&pyro_buffer);
	if(pyro_buffer < pid->Tp_tol_high && pyro_buffer > pid->Tp_tol_low)
		pid->stable_count++;
	else
		pid->stable_count = 0;
	pid->err =(int16_t)pid->Tp_hex - pyro_buffer;

	if(pid->err > pid->epsilon || pid->err < -pid->epsilon) // DISABLE IF NEEDED
		pid->integral = pid->integral + pid->err;
//	else
//		pid->integral = 0;

	if(pid->integral > pid->max_integral)
		pid->integral = pid->max_integral;
	else if(pid->integral < -pid->max_integral)
		pid->integral = -pid->max_integral;

	pid->derivative = pid->err - pid->last_err;

	int16_t out = (int16_t)  (pid->Kp * pid->err) + (pid->Ki * pid->integral) + (pid->Kd * pid->derivative);
	if(out > pid->max_out)
		pid->out = pid->max_out;
	else if (out < pid->min_out)
		pid->out = pid->min_out;
	else
		pid->out = (uint16_t) out;
	if(pid->status == PIDS_RUN)
		PWM__Update(pid->out);
	else
		pid->out = 1;
}

void PWM__Start(void){
	TIM3->CCR2 = 1;
	TIM3->CCR4 = 1;
	TIM3->CR1 |= TIM_CR1_CEN;
	// Ugly, but works for now.
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);

	PWM__Update(PWMpid.out_init);
}
inline void PWM__Stop(void){
	PWMpid.transition = PIDS_STOP;
}

inline void PWM__Update(uint16_t pwm_duty){
	TIM3->CCR2 = pwm_duty;
	TIM3->CCR4 = pwm_duty; //TODO: eliminate if not necessary
}

void PID__Reset(T_pid_t* pid){
	pid->integral = 0;
	pid->derivative = 0;
	pid->err = 0;
	pid->last_err = 0;
	pid->stable_count = 0;
//	uint8_t str[] = "STOPPEDD";
//	uint8_t str4[] = "PIDDRST\0";
//	Comm__PutString(str4);
//	pid->status = PIDS_RUN;
}

void PID__SetStatus(T_pid_t* pid, status_t status){
	pid->status = status;
}
void PID__SetTransition(T_pid_t* pid, status_t status){
	pid->transition = status;
}

void PID__SetPeriod(T_pid_t* pid, uint16_t period){
	pid->period = period;
	TIM3->ARR = period;
}
void PID__SetOutInit(T_pid_t* pid, uint16_t o_in){
	pid->out_init = o_in;
}
void PID__SetOutMax(T_pid_t* pid, uint16_t o_max){
	pid->max_out = o_max;
}

void PID__SetTp(T_pid_t*pid, uint16_t Tp){
	pid->Tp_hex = Tp;
}
void PID__SetTpmax(T_pid_t* pid, uint16_t Tp_max){
	pid->Tp_tol_high = Tp_max;
}

void PID__SetTPmin(T_pid_t* pid, uint16_t Tp_min){
	pid->Tp_tol_low = Tp_min;
}

void PID__SetKp(T_pid_t* pid, float Kp){
	pid->Kp = Kp;
}
void PID__SetKi(T_pid_t* pid, float Ki){
	pid->Ki = Ki;

}void PID__SetKd(T_pid_t* pid, float Kd){
	pid->Kd = Kd;
}






//void PID__step(T_pid_t * pid){
//	USART__PutChar(0x01);
//	pid->last_err = pid->err;
//	UART_ReadTemp_v1(&pyro_buffer);
//	pid->err =(int16_t)pid->Tk_hex - pyro_buffer;
//	pid->integral = pid->integral + pid->err;
//	pid->derivative = pid->err - pid->last_err;
//
//	int16_t out = (int16_t)  (pid->Kp * pid->err) + (pid->Ki * pid->integral) + (pid->Kd * pid->derivative);
//	if(out > pid->max_out)
//		pid->out = pid->max_out;
//	else if (out < pid->min_out)
//		pid->out = pid->min_out;
//	else
//		pid->out = (uint8_t) out;
//	PWM3__update(pid->out);
//}
