#ifndef __PROTOCOL_H
#define	__PROTOCOL_H

#include "stm32f10x.h"


#define MY_ID 0
#define MAX_REV_NUM 256

#define MAX_CAR_NUM 5

typedef union  
{  
    float fdata;  
    unsigned long ldata;  
}FloatLongType;  





struct S_CAR
{
	int8_t status;
	int8_t target_status;
	float target_speed;
	float speed;
	float height;
	float target_height;
};

struct S_COMMUNICATION
{
	uint8_t op_mode;
	uint8_t op_number;
	
	uint8_t signal ;
	
	uint8_t rev_buff[MAX_REV_NUM];
	
	uint8_t send_buff[45];
	
	uint8_t over_time_cnt;
	
	uint16_t rev_cnt;
	uint16_t rev_last_cnt;
	
	struct S_CAR s_car[MAX_CAR_NUM];

	
	
};

extern struct S_COMMUNICATION s_communication;


void Float_to_Byte(float f,unsigned char byte[])  ;
void Byte_to_Float(float *f,unsigned char byte[]) ;


uint8_t protocol_crc(uint8_t * buff,int num);
void protocol_rev(void);
void Proto_send(uint8_t flag );

#endif

