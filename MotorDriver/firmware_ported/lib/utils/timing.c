/*
 * timing.c
 *
 *  Created on: 06.01.2020
 *      Author: ckali
 */
#include "timing.h"


inline void start_timer(){
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

inline void stop_timer(){
	DWT->CTRL &= ~(DWT_CTRL_CYCCNTENA_Msk);
}

inline uint32_t get_timer(){
	return DWT->CYCCNT;
}

inline void reset_timer(){
	DWT->CYCCNT = 0;
}

inline void init_timer(){
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
}


