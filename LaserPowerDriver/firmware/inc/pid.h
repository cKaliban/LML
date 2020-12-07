/*
 * pid.h
 *
 *  Created on: 08.01.2020
 *      Author: ckali
 */

#ifndef PID_H_
#define PID_H_

#include "stdint.h"
#include "float.h"
#include "communication.h"
#include "periph_conf.h"

typedef enum status_t{
	PIDS_IDLE, PIDS_RUN, PIDS_RESET, PIDS_LOCK, PIDS_STOP
}status_t;

typedef struct T_pid_t{
	status_t status;
	status_t transition;
	uint16_t Tp_hex;
	uint16_t Tp_tol_high;
	uint16_t Tp_tol_low;

	float Kp;
	float Ki;
	float Kd;

	int16_t err;
	int16_t last_err;
	int16_t epsilon;

	int32_t integral;
	int32_t max_integral;
	int16_t derivative;

	uint16_t period;

	uint16_t out_init;
	uint16_t out;
	uint16_t max_out;
	uint16_t min_out;
	uint32_t stable_count;
}T_pid_t;


extern T_pid_t PWMpid;

void PID__SetStatus(T_pid_t*, status_t);
void PID__SetTransition(T_pid_t*, status_t);

void PWM__Update(uint16_t);
void PWM__Start(void);
void PWM__Stop(void);
void PID__Step(T_pid_t*);
void PID__Reset(T_pid_t*);

void PID__SetPeriod(T_pid_t*, uint16_t);
void PID__SetOutInit(T_pid_t*, uint16_t);
void PID__SetOutMax(T_pid_t*, uint16_t);

void PID__SetTp(T_pid_t*, uint16_t);
void PID__SetTpmax(T_pid_t*, uint16_t);
void PID__SetTPmin(T_pid_t*, uint16_t);

void PID__SetKp(T_pid_t*, float);
void PID__SetKi(T_pid_t*, float);
void PID__SetKd(T_pid_t*, float);

//void PID__step(T_pid_t*);

#endif /* PID_H_ */
