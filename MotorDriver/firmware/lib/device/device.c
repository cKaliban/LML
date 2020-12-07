/*
 * device.c
 *
 *      Author: Mateusz Wieczorek
 */

#include "device.h"

device_t stepper_control = {
		.device_name = MOTOR,
//		.device_state = DEBUG_STATE,
		.device_state = IDLE,
		.current_task = TASK_NONE,
		.current_layer = FIRST_LAYER_X,
		.axes = {
				.X_axis = {
						.pulse = &h_pulse,
						.control = &h_control,
						.limit_sw = &h_switch
				},
				.Z_axis = {
						.pulse = &v_pulse,
						.control = &v_control,
						.limit_sw = &v_switch
				}
		}
};
