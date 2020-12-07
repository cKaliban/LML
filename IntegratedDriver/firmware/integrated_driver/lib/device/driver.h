/*
 * driver.h
 *
 *  Created on: 06.01.2020
 *      Author: ckali
 */

// TODO: Come back for refactoring
#ifndef DEVICE_DRIVER_H_
#define DEVICE_DRIVER_H_

#include "pulsegen.h"
#include "control_signals.h"
#include "limit_switches.h"


typedef struct axis{
	uint32_t edge_pos_min;
	uint32_t edge_pos_max;
	uint32_t limit_pos_min;
	uint32_t limit_pos_max;

	uint32_t current_pos;

	pulse_config_t * pulse;
	control_signal_t * control;
	switch_control_t * limit_sw;
}axis_control_t;


typedef struct driver_t{
	axis_control_t X_axis;
	axis_control_t Z_axis;
}driver_t;



#endif /* DEVICE_DRIVER_H_ */
