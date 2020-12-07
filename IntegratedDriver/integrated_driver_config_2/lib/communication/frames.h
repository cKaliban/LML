/*
 * frames.h
 *
 *  Created on: 05.01.2020
 *      Author: ckali
 */

#ifndef FRAMES_H_
#define FRAMES_H_
#include "stdint.h"

typedef enum q_frame_t{
//    FRAME_IDN = 0x1D1D,
	FRAME_SUBMIT = 0x5EED,
//	FRAME_CALIBRATE = 0x5EAB,
//	FRAME_HOME = 0xAB00,
//	FRAME_TEST = 0xCECC,
//	FRAME_STOP = 0xDEAD,
//
////    # X control
//	FRAME_CALIBRATE_X = 0xAA5E,
//	FRAME_HOME_X = 0xAA00,
//	FRAME_TEST_X = 0xAACE,
	FRAME_TOGGLE = 0xAA66,
//
////    # Y control
//	FRAME_CALIBRATE_Z = 0xBB5E,
//	FRAME_HOME_Z = 0xBB00,
//	FRAME_TEST_Z = 0xBBCE,
	FRAME_CLEAN = 0xBB66,

//    # Other

}q_frame_t;

typedef struct query_t{
	uint16_t inquiry;
	uint16_t response;
}query_t;

uint16_t frames__packFrames_uint16(uint8_t* , uint8_t);




typedef enum header_t{
	FRAME_NEXT_LAYER = 0x6060,
	FRAME_FINISH_WORK = 0xF1FF,

	FRAME_IDN = 0x1D1D,
	FRAME_REQUEST = 0x5EED,
//	FRAME_DEBUG = 0x0000,
//	FRAME_ACTIVE = 0x0000,
	FRAME_STOP = 0xDEAD,

    FRAME_CALIBRATE = 0x5EAB,
	FRAME_CALIBRATE_X = 0x5EAA,
	FRAME_CALIBRATE_Z = 0x5EBB,

	FRAME_TEST = 0xCEAB,
	FRAME_TEST_X = 0xCEAA,
	FRAME_TEST_Z = 0xCEBB,

	FRAME_HOME = 0x00AB,
	FRAME_HOME_X = 0x00AA,
	FRAME_HOME_Z = 0x00BB,

//	Platform settings
//	FRAME_PID_TEMP = 0xAD5E,
    FRAME_PLATFORM_SET = 0xADAA,
//	FRAME_FIRST_DEPTH = 0xADB0,
//	FRAME_DEFAULT_DEPTH = 0xADBB,
/*	Laser settings */
//	Laser mode
//	FRAME_MODE_CONST = 0X0000,
//	FRAME_MODE_PID = 0X0000,
//	PID settings
	FRAME_PROC_TEMP = 0xAD5E,
//	FRAME_TEMP_TOL = 0x0000,
	FRAME_PID_KP = 0xADC9,
	FRAME_PID_KD = 0xADCD,
	FRAME_PID_KI = 0xADC1,
//	PWM settings
	FRAME_PWM_SET = 0xADFF,
//	FRAME_PWM_INIT = 0xAD01,
//	FRAME_PWM_MAX = 0xAD03

}header_t;

#endif /* FRAMES_H_ */
