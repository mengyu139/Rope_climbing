#ifndef __ADVANCE_CONTROL_H
#define __ADVANCE_CONTROL_H 

#include "stm32f10x.h"
	
	
struct S_ADVANCE
{
	float q1;
	float q2;
	
	float q_d_1;
	float q_d_2;
	
	float delta_q1;
	float delta_q2;
	
	float diff_q;
	float diff_q_buff[2];
	
	float q_pre;
	
	
	float delta_ita;
	
	
	float s;
	
	float Ks;
	float Kq;
	float Alpha_s;
	float Alpha_q;
	
	
	float Us;
	
}	;
extern struct S_ADVANCE s_advanced_control;

void Advanced_control_init(void);

void Advanced_control(uint8_t mode);


#endif


