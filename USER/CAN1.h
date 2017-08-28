
#ifndef _CAN1_H_
#define _CAN1_H_

#include "stm32f10x.h"

#define MY_ID1 0X106
#define MY_ID2 (MY_ID1 + 1)


struct S_CAN_MSG 
{
	uint8_t tx_buff[8];
	uint8_t rev_flag;
	CanRxMsg RxMessage;
	
	uint32_t Clock;
	
};

extern struct S_CAN_MSG s_can_msg;

void CAN1_Configuration(void);
void CAN1_GPIOConfig(void);
void CAN1_Send(uint32_t ID,u8 *Data);
void CAN1_NVIC_Configuration(void);

extern int32_t can_upload_cnt;

extern uint16_t CAN_ID_1 ;
extern uint16_t CAN_ID_2 ;


#endif

