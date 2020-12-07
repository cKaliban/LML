#ifndef UART_H
#define UART_H
#include "stm32f4xx.h"
#include "../lib/utils/delay.h"

#define USART_BAUD_115k_OVER8	0x1B1
#define USART_BAUD_19k_OVER8	0xA24

typedef union comm_buffer_t{
	uint8_t uart_buffer[8];
	uint64_t full;
	struct{
		uint16_t header;
		uint16_t val_u16_a;
		union{
			struct{
				uint16_t val_u16_b;
				uint16_t val_u16_c;
			};
			uint32_t val_32;
			float val_f;
		};
	};
}comm_buffer_t;

extern uint8_t USART_rx_buffer[8];
extern uint8_t USART_tx_buffer[8];
extern comm_buffer_t USB_buffer;
//extern uint8_t comm_rx_buffer[8];
//extern uint8_t comm_tx_buffer[8];

void UARTInit(void);
void USART_PutChar(uint8_t);
void USART_PutString(uint8_t*);

void InComm__Init(void);
void InComm__StartWork(void);
//void InComm__PauseLaser(void);

#endif /* !UART_H */
