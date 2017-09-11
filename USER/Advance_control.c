#include "Advance_control.h"
#include "protocal.h"
#include "Motor.h"
#include "delay.h"
#include "CAN1.h"



struct S_ADVANCE s_advanced_control;

void Advanced_control_init(void)
{
	s_advanced_control.q_d_1 =0;
	s_advanced_control.q_d_2 = 0;
	s_advanced_control.delta_q1 = 0;
	s_advanced_control.delta_q2 = 0;
	s_advanced_control.delta_ita = 0;
	s_advanced_control.Us = 0;
	
	s_advanced_control.Kq = 20000;
	s_advanced_control.Ks = 1;
	s_advanced_control.Alpha_s = 60000;
	s_advanced_control.Alpha_q  =0;
	
	s_advanced_control.diff_q = 0;
	s_advanced_control.q_pre = 0;
}


void Advanced_control(uint8_t mode)
{
	if( MY_ID == 0)
	{
		//
		s_advanced_control.q1 = s_communication.s_car[0].height;
		s_advanced_control.q_d_1 = s_communication.s_car[0].target_height;
		
		s_advanced_control.q2 = s_communication.s_car[2].height;
		s_advanced_control.q_d_2 = s_communication.s_car[0].target_height;
	}
		
	else
	{
		s_advanced_control.q1 = s_communication.s_car[2].height;
		s_advanced_control.q_d_1 = s_communication.s_car[2].target_height;
		
		s_advanced_control.q2 = s_communication.s_car[0].height;
		s_advanced_control.q_d_2 = s_communication.s_car[2].target_height;
	}
	
	
	s_advanced_control.delta_q1 =  s_advanced_control.q1 - s_advanced_control.q_d_1  ;
	s_advanced_control.delta_q2 =  s_advanced_control.q2 - s_advanced_control.q_d_2   ;
	
	s_advanced_control.delta_ita = 2 * ( s_advanced_control.delta_q1 - s_advanced_control.delta_q2 );
	
	
	s_advanced_control.diff_q_buff[0] = s_advanced_control.diff_q_buff[1];
	s_advanced_control.diff_q_buff[1] = s_robo_module.real_position;
	
	//s_advanced_control.diff_q = s_advanced_control.q1 - s_advanced_control.q_pre;
	s_advanced_control.diff_q = s_advanced_control.diff_q_buff[1] - s_advanced_control.diff_q_buff[0];
	
	
	
	s_advanced_control.q_pre = s_advanced_control.q1;
	
	
	s_advanced_control.s = (s_advanced_control.Alpha_s * s_advanced_control.delta_ita );//s_advanced_control.diff_q + 
	
	
	if( mode == 0)
	{
		s_advanced_control.Us =  0  - s_advanced_control.Kq * s_advanced_control.delta_q1;
	}
	else
	{
		s_advanced_control.Us =  -s_advanced_control.Ks * s_advanced_control.s  - s_advanced_control.Kq * s_advanced_control.delta_q1;
	}
	
}