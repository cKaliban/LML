/* UART and DMA Configuration:
Pins A2 and A3 are connected to serial port emulator provided by St-link, allowing
for communication through built-in USB port
*/
#include "communication.h"


uint8_t USART_rx_buffer[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t USART_tx_buffer[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

comm_buffer_t USB_buffer;

void UARTInit(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

	GPIOA->MODER &= ~(GPIO_MODER_MODER2_Msk | GPIO_MODER_MODER3_Msk);
	GPIOA->MODER |= (0x02 <<GPIO_MODER_MODER2_Pos) | (0x02 <<GPIO_MODER_MODER3_Pos);
	GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR2 | GPIO_OSPEEDER_OSPEEDR3);

	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL2 | GPIO_AFRL_AFRL3);
	GPIOA->AFR[0] |= (GPIO_AFRL_AFRL2_0 | GPIO_AFRL_AFRL2_1 |GPIO_AFRL_AFRL2_2);
	GPIOA->AFR[0] |= (GPIO_AFRL_AFRL3_0 | GPIO_AFRL_AFRL3_1 |GPIO_AFRL_AFRL3_2);

	delay_ms(100);

	USART2->CR1 = 0x00000000;
	USART2->CR2 = 0x00000000;
	USART2->CR3 = 0x00000000;

	USART2->CR1 |= USART_CR1_OVER8;

	USART2->BRR = USART_BAUD_115k_OVER8;

	USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;

	DMA1_Stream5->CR |= (DMA_CHANNEL_4 | DMA_CIRCULAR | DMA_PERIPH_TO_MEMORY | DMA_SxCR_MINC);
	DMA1_Stream5->NDTR |= 8;
	DMA1_Stream5->PAR |= (uint32_t) &USART2->DR;
	DMA1_Stream5->M0AR |= (uint32_t) USART_rx_buffer;
	DMA1_Stream5->CR |= DMA_SxCR_TCIE;
	DMA1_Stream5->CR |= DMA_SxCR_EN;
	USART2->CR3 |= USART_CR3_DMAR;
	USART2->CR1 |= USART_CR1_UE;

//	delay_ms(500);
//	delay_ms(500);
//	delay_ms(500);
//	delay_ms(500);
}

void USART_PutChar(uint8_t ch)
{
	while ((USART2->SR & USART_SR_TXE) != USART_SR_TXE);
					USART2->DR = ch;
}

void USART_PutString(uint8_t * str)
{
	while(*str != 0){
		USART_PutChar(*str);
		str++;
	}
}


void InComm__Init(){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	GPIOC->MODER &= ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9);
	GPIOC->MODER |= (GPIO_MODER_MODE8_0 | GPIO_MODER_MODE9_0);
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9);
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9);

	GPIOC->BSRR |= GPIO_BSRR_BR_8;
	GPIOC->BSRR |= GPIO_BSRR_BR_9;
}


inline void InComm__StartWork(){
	GPIOC->BSRR |= GPIO_BSRR_BS_8;
	delay_ms(400);
	GPIOC->BSRR |= GPIO_BSRR_BR_8;
//	GPIOC->BSRR |= GPIO_BSRR_BR_9;
}


//inline void InComm__PauseLaser(){
//	GPIOC->BSRR |= GPIO_BSRR_BS_9;
//	GPIOC->BSRR |= GPIO_BSRR_BR_8;
//}
