#ifndef DEVICE_CONTROL_SIGNALS_H_
#define DEVICE_CONTROL_SIGNALS_H_

#include "stm32f4xx.h"
#include "axis_types.h"



typedef struct control_signal_t{
	axis_t axis;
	dir_t current_direction;
	void (*portSetup)(void);
	void (*enable)(void);
	void (*disable)(void);
	void (*setDir)(dir_t);
//	void (*changeDir)(void);
}control_signal_t;

extern control_signal_t h_control;
extern control_signal_t v_control;

void control__Init(control_signal_t*, axis_t);

#endif /* DEVICE_MOTOR_CONTROL */
