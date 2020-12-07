/*
 * random.c
 *
 *  Created on: 23.03.2020
 *      Author: ckali
 */
#include "random.h"

//void TM_RNG_Init(void) {
//	/* Enable RNG clock source */
//	RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;
//
//	/* RNG Peripheral enable */
//	RNG->CR |= RNG_CR_RNGEN;
//}
//
//void TM_RNG_DeInit(void) {
//	/* Disable RNG peripheral */
//	RNG->CR &= ~RNG_CR_RNGEN;
//
//	/* Disable RNG clock source */
//	RCC->AHB2ENR &= ~RCC_AHB2ENR_RNGEN;
//}
//
//uint32_t TM_RNG_Get(void) {
//	/* Wait until one RNG number is ready */
//	while (!(RNG->SR & (RNG_SR_DRDY)));
//
//	/* Get a 32-bit Random number */
//	return RNG->DR;
//}

