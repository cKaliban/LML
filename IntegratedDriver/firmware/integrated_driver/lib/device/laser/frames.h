/*
 * frames.h
 *
 *  Created on: 13.01.2020
 *      Author: ckali
 */

#ifndef FRAMES_H_
#define FRAMES_H_

#include "stdint.h"

//typedef uint16_t frame;
typedef enum frames{
	PID_START = 0xD0D0,
	PID_RESET = 0xCEDE,
	PID_STOP = 0xDEAD,
	PID_IDN = 0x1D1D,

//	PID_SET = 0xAD,
//	PID_TP = 0x5E,
//	PID_KP = 0xC9,
//	PID_KD = 0xCD,
//	PID_KI = 0xC1,
//	PID_OI = 0x01,
//	PID_OM = 0x03,

	PID_SET_TP = 0xAD5E,
	PID_SET_KP = 0xADC9,
	PID_SET_KD = 0xADCD,
	PID_SET_KI = 0xADC1,
	PID_SET_PERIOD = 0xADFF,
	PID_SET_OI = 0xAD01,
	PID_SET_OM = 0xAD03
}frame;


uint16_t frames__packFrames_uint16(uint8_t*, uint8_t);
float frames__packFrames_float(uint8_t*, uint8_t);

//uint16_t frames__getTp(uint8_t*);
//uint16_t frames__getTp_tol(uint8_t*);
//float frames__getKp(uint8_t*);
//float frames__getKi(uint8_t*);
//float frames__getKd(uint8_t*);



#endif /* FRAMES_H_ */
