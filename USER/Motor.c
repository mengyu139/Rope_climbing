
#include "Motor.h"
#include "delay.h"
#include "CAN1.h"
#include "protocal.h"
#include "uart.h"
#include "RobotModuleDriver.h"


uint32_t Motor_Clock = 0;

struct S_PID_PARA s_pid_para;


float abs (float a)
{
	if( a<0 )
	{
		return -a;
	}
	else
	{
		return a;
	}
}

void motor_pid_init(void)
{
	s_pid_para.P = 10000;
	s_pid_para.I = 0;
	s_pid_para.D = 0;
	s_pid_para.intergral = 0;
	s_pid_para.target = 0.0; // 3m
	s_pid_para.currentS = 0;
	s_pid_para.out = 0;
	s_pid_para.delta =0;
	s_pid_para.direction = 0;
}
 


void motor_out(float a)
{

	if( a> MOTOR_OUT_MAX )
	{
		a = MOTOR_OUT_MAX;
	}
	else if( a < MOTOR_OUT_MIN )
	{
		a = MOTOR_OUT_MIN;
	}
	
	RoboModule_open_loop_mode_set_data( MY_ID+1 , ( (int16_t)a ) );
	
}


void Set_target( uint8_t Id, float target)
{
		//s_pid_para.direction = direction;
		//s_pid_para.target = target;
	
	if ( target > s_communication.s_car[Id].target_height )
	{
		s_communication.s_car[Id].status = 1;
	}
	else if( target < s_communication.s_car[Id].target_height )
	{
		s_communication.s_car[Id].status = -1;
	}


	s_communication.s_car[Id].target_height = target;
	
	
}

void Motor_init(void)
{
	
	s_robo_module.real_current=0;
	s_robo_module.real_position=0;
	s_robo_module.real_velocity=0;
	
	
	//Robot_module driver init 
	//1.reset 2.set mode 3.config feedback data mode
	RoboModule_reset(MY_ID + 1);
	delay_ms(500);
	RoboModule_set_mode(MY_ID + 1,MOTOR_OPEN_LOOP);//run at open loop mode
	delay_ms(500);
	RoboModule_config(MY_ID + 1,10,0);// feedabck data every 10ms 
  delay_ms(300);
	
	
	motor_out(0);
	
	motor_pid_init();
	
}




//update feeback and target

void task_motor_control(uint8_t flag)
{
	float threshold = 0.01;
	float result_out = 0;
	
	s_pid_para.currentS = s_communication.s_car[MY_ID].height;
	s_pid_para.target = s_communication.s_car[MY_ID].target_height;
	
	if( Motor_Clock >= 5 )//5*2=10ms
	{
		Motor_Clock = 0;
		
		if( abs( s_pid_para.currentS - s_pid_para.target ) < threshold )
		{
			result_out = 0;
			
			//Stop the motor
			s_communication.s_car[MY_ID].status = 0;
			
		}
		else
		{
			s_pid_para.delta = s_pid_para.target - s_pid_para.currentS;
			s_pid_para.intergral += s_pid_para.delta;
			s_pid_para.out = s_pid_para.delta * s_pid_para.P;
		
			#if 1
			if( s_pid_para.out > 0  )
			{
				s_pid_para.out += POSITIVE_COMPASETION;
			}
			else if( s_pid_para.out < 0 )
			{
				s_pid_para.out += NEGTIVE_COMPASETION;
			}
			
		 #endif
			
			//防止震荡，如果向上走，最小输出为0
			if( s_communication.s_car[MY_ID].status == 1)
			{
				if( s_pid_para.out<0 )
				{
					s_pid_para.out = 0;
				}
			}
			//when downward , the most out is 0
			else if( s_communication.s_car[MY_ID].status == -1 )
			{
				if( s_pid_para.out>0 )
				{
					s_pid_para.out = 0;
				}
			}
			
//			else if( s_communication.s_car[MY_ID].status ==0 )
//			{
//				s_pid_para.out = 0;
//			}
			
			result_out = s_pid_para.out;		
		}
		
		if( flag ==1 )
		{
			motor_out( result_out);
		}
		
	}
	
	
	
	
	
	
}
	

