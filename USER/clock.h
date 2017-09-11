#ifndef __CLOCK_H
#define	__CLOCK_H

#include "stm32f10x.h"

void Clock_init(void);


extern uint32_t led_cnt;
extern uint32_t Global_clock;
extern uint32_t send_clock;
extern uint32_t Globale_flag;


#define Discrete_num  500
//#define  T  15.0
#define AMPHILE 0.5


#endif
