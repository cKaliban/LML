/*
 * device.h
 *
 *      Author: Mateusz Wieczorek
 */

#ifndef DEVICE_DEVICE_H_
#define DEVICE_DEVICE_H_
#include "stdio.h"

#include "control_signals.h"
#include "device_state.h"
#include "limit_switches.h"
#include "pulsegen.h"

typedef enum layer_t{
	FIRST_LAYER_X, FIRST_LAYER_Z ,DEFAULT_LAYER
}layer_t;


typedef struct axis_control_t{
	/*DEPRECATED*/
	uint32_t edge_pos_min;
	uint32_t edge_pos_max;
	uint32_t limit_pos_min;
	uint32_t limit_pos_max;
	/*!DEPRECATED*/

	pulse_config_t * pulse;
	control_signal_t * control;
	switch_control_t * limit_sw;
}axis_control_t;


typedef enum task_t{
	TASK_NONE,

	TASK_TEST,
	TASK_TEST_X,
	TASK_STRESSTEST_X,
	TASK_TEST_Z,
	TASK_STRESSTEST_Z,


	TASK_NEXT_LAYER,
	TASK_FINISH_WORK,

	TASK_HOME,
	TASK_HOME_X,
	TASK_HOME_Z,

	TASK_CLEAN,
	TASK_TOGGLE,

	TASK_FAILED
}task_t;


typedef struct device_t{
	const dev_name_t device_name;
	dev_state_t device_state;
	task_t current_task;
	layer_t current_layer;
	struct axes{
		axis_control_t X_axis;
		axis_control_t Z_axis;
	}axes;
}device_t;

extern device_t stepper_control;



#endif /* DEVICE_DEVICE_H_ */
