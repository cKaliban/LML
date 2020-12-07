/*
 * subroutines.h
 *
 *  Created on: 06.01.2020
 *      Author: ckali
 */

#ifndef UTILS_SUBROUTINES_H_
#define UTILS_SUBROUTINES_H_

#include "stm32f4xx.h"

#include "../communication/communication.h"
#include "../communication/frames.h"
#include "../device/device.h"
#include "../lib/device/laser/pid.h"
#include "../lib/device/laser/periph_conf.h"

//TODO: find correct margin value
//#define H_MARGIN	400
//#define V_MARGIN	400

void NVIC_Init(void);

void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);

void DMA1_Stream5_IRQHandler(void);

void TIM1_UP_TIM10_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);

void EXTI15_10_IRQHandler(void);
/********************************/
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#endif /* UTILS_SUBROUTINES_H_ */
