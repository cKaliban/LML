/*
 * axis_type.h
 *
 *  Created on: 06.01.2020
 *      Author: ckali
 */

#ifndef DEVICE_AXIS_TYPES_H_
#define DEVICE_AXIS_TYPES_H_
#include "stdint.h"

typedef enum axis_t {HORIZONTAL, VERTICAL}axis_t;

typedef enum dir_t{
	LEFT, RIGHT,
	UP, DOWN
}dir_t;


typedef enum axis_state_t{
	NOT_CALIBRATED, CALIBRATED
}axis_state_t;


#endif /* DEVICE_AXIS_TYPES_H_ */
