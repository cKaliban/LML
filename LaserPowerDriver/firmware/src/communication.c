#include "communication.h"


uint8_t comm_tx_buffer[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t comm_rx_buffer[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint16_t pyro_buffer = 0x0000;


void Comm__Init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

	GPIOA->MODER &= ~(GPIO_MODER_MODER2_Msk | GPIO_MODER_MODER3_Msk);
	GPIOA->MODER |= (0x02 <<GPIO_MODER_MODER2_Pos) | (0x02 <<GPIO_MODER_MODER3_Pos);
	GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR2 | GPIO_OSPEEDER_OSPEEDR3);

	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL2 | GPIO_AFRL_AFRL3);
	GPIOA->AFR[0] |= (GPIO_AFRL_AFRL2_0 | GPIO_AFRL_AFRL2_1 |GPIO_AFRL_AFRL2_2);
	GPIOA->AFR[0] |= (GPIO_AFRL_AFRL3_0 | GPIO_AFRL_AFRL3_1 |GPIO_AFRL_AFRL3_2);

	USART2->CR1 = 0x00000000;
	USART2->CR2 = 0x00000000;
	USART2->CR3 = 0x00000000;

	USART2->CR1 |= USART_CR1_OVER8;

	USART2->BRR = USART_BAUD_115k_OVER8;

	USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;

	DMA1_Stream5->CR |= (DMA_CHANNEL_4 | DMA_CIRCULAR | DMA_PERIPH_TO_MEMORY | DMA_SxCR_MINC);
	DMA1_Stream5->NDTR |= 8;
	DMA1_Stream5->PAR |= (uint32_t) &USART2->DR;
	DMA1_Stream5->M0AR |= (uint32_t) comm_rx_buffer;
	DMA1_Stream5->CR |= DMA_SxCR_TCIE;
	DMA1_Stream5->CR |= DMA_SxCR_EN;
	USART2->CR3 |= USART_CR3_DMAR;
	USART2->CR1 |= USART_CR1_UE;
}

void Comm__Init_noDMA(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

	GPIOA->MODER &= ~(GPIO_MODER_MODER2_Msk | GPIO_MODER_MODER3_Msk);
	GPIOA->MODER |= (0x02 <<GPIO_MODER_MODER2_Pos) | (0x02 <<GPIO_MODER_MODER3_Pos);
	GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR2 | GPIO_OSPEEDER_OSPEEDR3);

	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL2 | GPIO_AFRL_AFRL3);
	GPIOA->AFR[0] |= (GPIO_AFRL_AFRL2_0 | GPIO_AFRL_AFRL2_1 |GPIO_AFRL_AFRL2_2);
	GPIOA->AFR[0] |= (GPIO_AFRL_AFRL3_0 | GPIO_AFRL_AFRL3_1 |GPIO_AFRL_AFRL3_2);

	USART2->CR1 = 0x00000000;
	USART2->CR2 = 0x00000000;
	USART2->CR3 = 0x00000000;

	USART2->CR1 |= USART_CR1_OVER8;

	USART2->BRR = USART_BAUD_115k_OVER8;

	USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;

	USART2->CR1 |= USART_CR1_UE;
}

inline void Comm__PutChar(uint8_t ch)
{
	while ((USART2->SR & USART_SR_TXE) != USART_SR_TXE);
					USART2->DR = ch;
}

void Comm__PutString(uint8_t * str)
{
	while(*str != 0){
		Comm__PutChar(*str);
		str++;
	}
}

//TODO add frames blueprints

void Pyro__Init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB1ENR |= RCC_APB1ENR_UART4EN;

	GPIOA->MODER &= ~(GPIO_MODER_MODER0_Msk | GPIO_MODER_MODER1_Msk);
	GPIOA->MODER |= (0x02 <<GPIO_MODER_MODER0_Pos) | (0x02 <<GPIO_MODER_MODER1_Pos);
	GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR0 | GPIO_OSPEEDER_OSPEEDR1);

	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL0 | GPIO_AFRL_AFRL1);
	GPIOA->AFR[0] |= (GPIO_AFRL_AFRL0_3);
	GPIOA->AFR[0] |= (GPIO_AFRL_AFRL1_3);

	UART4->CR1 = 0x00000000;
	UART4->CR2 = 0x00000000;
	UART4->CR3 = 0x00000000;

	UART4->CR1 |= USART_CR1_OVER8;
	UART4->BRR = USART_BAUD_115k_OVER8;

	UART4->CR1 |= USART_CR1_TE | USART_CR1_RE;

	UART4->CR3 |= USART_CR3_DMAR;
	UART4->CR1 |= USART_CR1_UE;
}

inline void Pyro__PutChar(uint8_t ch)
{
	while ((UART4->SR & USART_SR_TXE) != USART_SR_TXE);
					UART4->DR = ch;
}

static inline void Pyro__RequestTemp(){
	Pyro__PutChar(0x01);
}

static inline void Pyro__Receive(uint16_t* rx_buffer)
{
//	*rx_buffer = 0x0000;
//	__disable_irq();
	while (!(UART4->SR & USART_SR_RXNE) && (PWMpid.status = PIDS_RUN)){
		if(PWMpid.status != PIDS_RUN){
			*rx_buffer = 0x0000;
			break;
		}
	}
	*rx_buffer = (uint8_t)(UART4->DR & 0xFF) << 8;
	while (!(UART4->SR & USART_SR_RXNE) && (PWMpid.status = PIDS_RUN)){
		if(PWMpid.status != PIDS_RUN){
			*rx_buffer = 0x0000;
			break;
		}
	}
	*rx_buffer |= (uint8_t)(UART4->DR & 0xFF) << 0;
//	__enable_irq();
}

inline void Pyro__Query(uint16_t* rx_buffer){
	Pyro__RequestTemp();
	Pyro__Receive(rx_buffer);

}

void DMA1_Stream5_IRQHandler(){
//	if ((DMA1->HIFCR & DMA_HIFCR_CTCIF5_Msk) != 0){
		uint16_t status_frame = frames__packFrames_uint16(comm_rx_buffer,0);
		switch(status_frame)
		{
		case PID_IDN: ;
//			uint8_t str[] = "STEPPERD";
//			uint8_t str[] = "LASERPD\0";
//			Comm__PutString(str);
			break;
		case PID_STOP:
			PWM__Stop();
			break;
		case PID_START:
			PID__SetTransition(&PWMpid, PIDS_RUN);
			break;
		case PID_RESET:
			PID__SetTransition(&PWMpid, PIDS_RESET);
			break;
		case PID_SET_TP: ;
			uint16_t temp = frames__packFrames_uint16(comm_rx_buffer, 2);
			PID__SetTp(&PWMpid, temp);
			temp = frames__packFrames_uint16(comm_rx_buffer, 4);
			PID__SetTPmin(&PWMpid, temp);
			temp = frames__packFrames_uint16(comm_rx_buffer, 6);
			PID__SetTpmax(&PWMpid, temp);
			break;
		case PID_SET_PERIOD: ;
			uint16_t period = frames__packFrames_uint16(comm_rx_buffer, 2);
			PID__SetPeriod(&PWMpid, period);
			break;
		case PID_SET_OM: ;
			uint16_t out_max = frames__packFrames_uint16(comm_rx_buffer, 2);
			PID__SetOutMax(&PWMpid, out_max);
			break;
		case PID_SET_OI: ;
			uint16_t out_init = frames__packFrames_uint16(comm_rx_buffer, 2);
			PID__SetOutInit(&PWMpid, out_init);
			break;
		case PID_SET_KP: ;
			float Kp = frames__packFrames_float(comm_rx_buffer, 2);
			PID__SetKp(&PWMpid, Kp);
			break;
		case PID_SET_KI:;
			float Ki = frames__packFrames_float(comm_rx_buffer, 2);
			PID__SetKi(&PWMpid, Ki);
			break;
		case PID_SET_KD:;
			float Kd = frames__packFrames_float(comm_rx_buffer, 2);
			PID__SetKd(&PWMpid, Kd);
//			uint8_t str[] = "STOPPEDD";
//			uint8_t str2[] = "UPDATED\0";
//			Comm__PutString(str2);
			break;
		}
		DMA1->HIFCR |= DMA_HIFCR_CTCIF5;
//	}
}

// TODO: add to header
void InComm__StopInit(){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	GPIOC->MODER &= ~GPIO_MODER_MODER0_Msk;
	GPIOC->MODER |= (0x00 << GPIO_MODER_MODER0_Pos);
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD0;

	SYSCFG->EXTICR[0] &= ~ SYSCFG_EXTICR1_EXTI0_Msk;
	SYSCFG->EXTICR[0] |=   SYSCFG_EXTICR1_EXTI0_PC;

	EXTI->IMR |= EXTI_IMR_IM0;

	EXTI->RTSR |= EXTI_RTSR_TR0;
	EXTI->FTSR &= ~EXTI_FTSR_TR0;
	// EXTI->RTSR &= ~EXTI_RTSR_TR0;
	// EXTI->FTSR |= EXTI_FTSR_TR0;
}
// TODO: add to header
void InComm__StartInit(){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	GPIOC->MODER &= ~GPIO_MODER_MODER1_Msk;
	GPIOC->MODER |= (0x00 << GPIO_MODER_MODER1_Pos);
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD1;

	SYSCFG->EXTICR[0] &= ~ SYSCFG_EXTICR1_EXTI1_Msk;
	SYSCFG->EXTICR[0] |=   SYSCFG_EXTICR1_EXTI1_PC;

	EXTI->IMR |= EXTI_IMR_IM1;
	EXTI->RTSR |= EXTI_RTSR_TR1;
	EXTI->FTSR &= ~EXTI_FTSR_TR1;
	// EXTI->RTSR &= ~EXTI_RTSR_TR1;
	// EXTI->FTSR |= EXTI_FTSR_TR1;
}




/*	Stop Handler	*/
void EXTI0_IRQHandler(){
	if ((EXTI->PR & EXTI_PR_PR0_Msk) != 0){
		EXTI->PR |= EXTI_PR_PR0;
		PWM__Stop();
//		PWMpid.transition = PIDS_STOP;
//		PWMpid.status = PIDS_IDLE;
	}
}

/*	Start Handler	*/
void EXTI1_IRQHandler(){
	if ((EXTI->PR & EXTI_PR_PR1_Msk) != 0){
		EXTI->PR |= EXTI_PR_PR1;
		PID__SetTransition(&PWMpid, PIDS_RUN);
//		PWMpid.transition = PIDS_RUN;
	}
}























/* TODO: previous version is working, but this should be more correct
void DMA1_Stream5_IRQHandler(){
	if ((DMA1->HIFCR & DMA_HIFCR_CTCIF5_Msk) != 0){
		uint16_t status_frame = frames__packFrames_uint16(comm_rx_buffer,0);
		switch(status_frame)
		{
		case PID_STOP:
			PWM__Stop();
			break;
		case PID_START:
			PID__SetTransition(&PWMpid, PIDS_RUN);
			break;
		case PID_RESET:
			PID__SetTransition(&PWMpid, PIDS_RESET);
			break;
		case PID_SET_TP: ;
			uint16_t temp = frames__packFrames_uint16(comm_rx_buffer, 2);
			PID__SetTp(&PWMpid, temp);
			temp = frames__packFrames_uint16(comm_rx_buffer, 4);
			PID__SetTPmin(&PWMpid, temp);
			temp = frames__packFrames_uint16(comm_rx_buffer, 6);
			PID__SetTpmax(&PWMpid, temp);
			break;
		case PID_SET_PERIOD: ;
			uint16_t period = frames__packFrames_uint16(comm_rx_buffer, 2);
			PID__SetPeriod(&PWMpid, period);
			break;
		case PID_SET_OM: ;
			uint16_t out_max = frames__packFrames_uint16(comm_rx_buffer, 2);
			PID__SetOutMax(&PWMpid, out_max);
			break;
		case PID_SET_OI: ;
			uint16_t out_init = frames__packFrames_uint16(comm_rx_buffer, 2);
			PID__SetOutInit(&PWMpid, out_init);
			break;
		case PID_SET_KP: ;
			float Kp = frames__packFrames_float(comm_rx_buffer, 2);
			PID__SetKp(&PWMpid, Kp);
			break;
		case PID_SET_KI:;
			float Ki = frames__packFrames_float(comm_rx_buffer, 2);
			PID__SetKi(&PWMpid, Ki);
			break;
		case PID_SET_KD:;
			float Kd = frames__packFrames_float(comm_rx_buffer, 2);
			PID__SetKd(&PWMpid, Kd);
			break;
		}
		DMA1->HIFCR &= ~(DMA_HIFCR_CTCIF5);
	}
}
*/
