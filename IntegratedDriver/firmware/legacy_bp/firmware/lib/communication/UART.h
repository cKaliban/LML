#ifndef UART_H
#define UART_H
#include "stm32f4xx.h"

#define USART_BAUD_115k_OVER8	0x1B1
#define USART_BAUD_19k_OVER8	0xA24

extern uint8_t USART_rx_buffer[8];
extern uint8_t USART_tx_buffer[8];

//extern uint8_t comm_rx_buffer[8];
//extern uint8_t comm_tx_buffer[8];

void UARTInit(void);
void USART_PutChar(uint8_t);
void USART_PutString(uint8_t*);

void InComm__Init(void);
void InComm__StartLaser(void);
void InComm__PauseLaser(void);

#endif /* !UART_H */
