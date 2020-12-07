/*
 * frames.c
 *
 *  Created on: 13.01.2020
 *      Author: ckali
 */

#include "frames.h"


uint16_t frames__packFrames_uint16(uint8_t* buffer, uint8_t i){
	uint16_t converted = 0x0000;
	converted = (buffer[i] << 8) | buffer[i+1];
	return converted;
}

float frames__packFrames_float(uint8_t* buffer, uint8_t i){
	union {
		uint32_t unsigned_long;
		float f_conv;
	}converted;
	converted.unsigned_long = (buffer[i] << 24) | (buffer[i+1] << 16) | (buffer[i+2] << 8) | buffer[i+3];
	return converted.f_conv;
}
