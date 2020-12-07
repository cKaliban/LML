/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f4xx.h"
#include "stm32f4xx_nucleo.h"
#include "stdint.h"

#include "periph_conf.h"
#include "communication.h"
#include "pid.h"



int main(void)
{
	//INIT STATE
	/* HARDWARE INIT */
	MainClockInit();
	Comm__Init();
	InComm__StartInit();
	InComm__StopInit();
	Pyro__Init();
	PWM__Init();
	NVIC_Init();
	/* !HARDWARE INIT */

	// IDLE STATE
	PWM__Start();
	for(;;){
		// START STATE
		if(PWMpid.status == PIDS_IDLE && PWMpid.transition == PIDS_RESET){
			PID__Reset(&PWMpid);
			PWMpid.transition = PIDS_IDLE;
		}
		if(PWMpid.status == PIDS_IDLE && PWMpid.transition == PIDS_RUN){
			PWM__Update(PWMpid.out_init);
			PWMpid.status = PIDS_RUN;
		}
		// RESET RUN
		if(PWMpid.status == PIDS_RUN && PWMpid.transition == PIDS_RESET){
			PID__Reset(&PWMpid);
			PWMpid.transition = PIDS_RUN;
		}
		// RUN STATE
		if(PWMpid.status == PIDS_RUN){
			if(PWMpid.stable_count < 1000){
//				PID__Step(&PWMpid);
//				__NOP();
			}
			else
				PWMpid.status = PIDS_LOCK; // LOCK STATE
//				__NOP();

		}
		// RESET LOCK
		if(PWMpid.status == PIDS_LOCK && PWMpid.transition == PIDS_RESET){
			PID__Reset(&PWMpid);
			PWMpid.status = PIDS_RUN;
			PWMpid.transition = PIDS_RUN;
		}
		if(PWMpid.transition == PIDS_STOP){
			TIM3->CCR2 = 1;
			TIM3->CCR4 = 1;
		//	TIM3->CR1 &= ~(TIM_CR1_CEN);
//			uint8_t str[] = "STEPPERD";
//			uint8_t str3[] = "STOPPED\0";
//			Comm__PutString(str3);
			PID__Reset(&PWMpid);
			PWMpid.status = PIDS_IDLE;
			PWMpid.transition = PIDS_IDLE;
		}
	}
}



