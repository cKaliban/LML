/*
 * frames.h
 *
 *  Created on: 05.01.2020
 *      Author: ckali
 */

#ifndef COMMUNICATION_FRAMES_H_
#define COMMUNICATION_FRAMES_H_
#include "stdint.h"

typedef enum q_frame_t{
    FRAME_IDN = 0x1D1D,
	FRAME_SUBMIT = 0x5EED,
	FRAME_CALIBRATE = 0x5EAB,
	FRAME_HOME = 0xAB00,
	FRAME_TEST = 0xCECC,
	FRAME_STOP = 0xDEAD,

//    # X control
	FRAME_CALIBRATE_X = 0xAA5E,
	FRAME_HOME_X = 0xAA00,
	FRAME_TEST_X = 0xAACE,
	FRAME_TOGGLE = 0xAA66,

//    # Y control
	FRAME_CALIBRATE_Z = 0xBB5E,
	FRAME_HOME_Z = 0xBB00,
	FRAME_TEST_Z = 0xBBCE,
	FRAME_CLEAN = 0xBB66,

//    # Other
	FRAME_NEXT_LAYER = 0x6060,
	FRAME_FINISH_WORK = 0xF1FF,
}q_frame_t;

typedef struct query_t{
	uint16_t inquiry;
	uint16_t response;
}query_t;

uint16_t frames__packFrames_uint16(uint8_t* , uint8_t);


#endif /* COMMUNICATION_FRAMES_H_ */
