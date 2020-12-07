/*
 * timing.h
 *
 *  Created on: 06.01.2020
 *      Author: ckali
 */
#include "stm32f4xx.h"
#include "stdio.h"

#ifndef UTILS_TIMING_H_
#define UTILS_TIMING_H_

void init_timer(void);
void reset_timer(void);
void start_timer(void);
void stop_timer(void);
uint32_t get_timer(void);


#endif /* UTILS_TIMING_H_ */
