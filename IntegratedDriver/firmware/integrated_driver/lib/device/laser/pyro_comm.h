#ifndef UART_H_
#define UART_H_
#include "stm32f4xx.h"
#include "frames.h"
#include "pid.h"

#define USART_BAUD_115k_OVER8	0x1B1
#define USART_BAUD_19k_OVER8	0xA24


extern uint16_t pyro_buffer;
extern uint8_t comm_rx_buffer[];
extern uint8_t comm_tx_buffer[];
extern uint16_t max_measured_temp;


void Comm__Init(void);
void Comm__Init_noDMA(void);

void InComm__StopInit(void);
void InComm__StartInit(void);

void Comm__PutChar(uint8_t);
void Comm__PutString(uint8_t*);

void Pyro__Init(void);
void Pyro__PutChar(uint8_t);
void Pyro__Query(uint16_t*);

void DMA1_Stream5_IrqHandler(void); // Incoming frames handler
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);



#endif
