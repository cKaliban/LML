/*
 * timing_rough.c
 *
 *  Created on: 09.01.2020
 *      Author: ckali
 */
#include "timing_rough.h"



void delay_ticks(uint32_t ticks)
{
    SysTick->LOAD = ticks;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;

    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
    SysTick->CTRL = 0;
}


// TODO: Overflow, refactor
void alt_delay_ms(uint32_t ms){
   (void) SysTick->CTRL;
   while (ms != 0) {
      ms -= (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) >> SysTick_CTRL_COUNTFLAG_Pos;
   }
}

//void delay_ms(uint32_t ms){
//	delay_ticks(12500 * ms);
//}
