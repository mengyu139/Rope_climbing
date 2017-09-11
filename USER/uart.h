#ifndef __USART_H
#define	__USART_H

#include "stm32f10x.h"



#define UART_OVER_TIME 4




void USART2_Config(void);
void usart_send_byte(uint8_t c);
void usart_send_string( uint8_t * s );

void task_uart_detect(void);
void send_some_bytes(uint8_t * buff , int num);

void Response_to_uart_command(void);


#endif
