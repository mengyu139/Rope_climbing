#ifndef __SONIC_H
#define	__SONIC_H

#include "stm32f10x.h"

#define START_TRIG GPIO_SetBits(GPIOB, GPIO_Pin_9)
#define CLOSE_TRIG GPIO_ResetBits(GPIOB, GPIO_Pin_9)



void Sonic_init(void);

void clear_exti_flag(void);
void Enable_EXTI(void);
void Disable_EXTI(void);
void EXTI_choose_rise(void);
void EXTI_choose_fall(void);
void Sonic_start(void);
//void task_Sonic_update(void);
float Sonic_update(void);

struct S_SONIC
{
	uint8_t exti_flag;
	
	uint8_t buff_index;
	
	uint32_t Clock;
	
	float distance_buff[5];
	
	
	
	
	float distance;
	float distance_tmp;
	
};


extern struct S_SONIC s_sonic;




#endif
