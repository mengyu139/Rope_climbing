#include "clock.h"
#include "led.h"
#include "Motor.h"
#include "Sonic.h"
#include "uart.h"
#include "CAN1.h"
#include "protocal.h"
#include "Advance_control.h"


uint32_t led_cnt = 0;

uint32_t Global_clock = 0;
uint32_t Globale_flag = 0;

uint32_t send_clock  =0;

uint32_t set_target_cnt = 0;
uint16_t Index = 0;

volatile int flag = 0;



extern float s_buff[Discrete_num];

uint8_t control_sweep = 0;


void Clock_init(void)
{
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	
	TIM_InternalClockConfig(TIM1);  
	TIM_DeInit(TIM1);
	
	TIM_TimeBaseStructure.TIM_Period = 200-1;                //2ms定时中断刷新系统
	TIM_TimeBaseStructure.TIM_Prescaler = 720-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, & TIM_TimeBaseStructure);
	
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);  
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE );  
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);


	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM1, ENABLE);  //启动时钟
	
}



//中断服务函数
void TIM1_UP_IRQHandler(void)
{	
	float tmp_output;
	int cur , pre;
	if(TIM_GetITStatus(TIM1,TIM_IT_Update)==SET) 
	{
//		float tmp = 0.5;
		led_cnt++;
		Motor_Clock ++;
		Global_clock++;
		s_sonic.Clock ++;
		send_clock ++;
		s_can_msg.Clock ++;
		
		
		Led_Flash();
		
		
		//update motor deedback data including: speed ,  absolute position , and current?
		task_update_motor_data();
		
		
		//电机控制
		#if 0
		task_motor_control(1);
		#endif
		

		
	
		
		
		//点动模式
		if( s_communication.op_mode == 1 )
		{
			//更新电机数据
			//控制电机
			//发送数据
			
			if( MY_ID == 0 )
			{
				Proto_send(2);
			}
			
			
			//control
			Advanced_control(0);
			
			if( s_communication.s_car[MY_ID].status == 1 )
			{
				tmp_output = s_advanced_control.Us + POSITIVE_COMPASETION;
				if( tmp_output < 0 )
				{
					tmp_output = 0;
				}
			}
			else if( s_communication.s_car[MY_ID].status == -1 )
			{
				tmp_output = s_advanced_control.Us + NEGTIVE_COMPASETION;	
				if( tmp_output > 0 )
				{
					tmp_output = 0;
				}
			}
			
			//output
			motor_out(tmp_output);
			
		}
		
		// track mode
		else if( s_communication.op_mode == 2 )
		{
			set_target_cnt ++;
			#if 1
			if( MY_ID == 0 )
			{
				if( set_target_cnt >= 100 )
				{
					set_target_cnt = 0;
					Set_target(MY_ID, s_buff[Index] );
						
					Index = (Index + 1) % Discrete_num;
				}
			}
			
			#endif
			
			
			
			if(  MY_ID == 0)
			{
				Proto_send(0);
			}
			
			Proto_send(1);
			
			
			//control
			Advanced_control(1);
			
			if( s_communication.s_car[MY_ID].status == 1 )
			{
				tmp_output = s_advanced_control.Us + POSITIVE_COMPASETION;
				if( tmp_output < 0 )
				{
					tmp_output = 0;
				}
			}
			else if( s_communication.s_car[MY_ID].status == -1 )
			{
				tmp_output = s_advanced_control.Us + NEGTIVE_COMPASETION;	
				if( tmp_output > 0 )
				{
					tmp_output = 0;
				}
			}
			
			//output
			motor_out(tmp_output);
			
			
			
			
		}
		

		
		
		
		
		
		
	
		

		
		//检测串口接受数据
		task_uart_detect();
		
		

		if( Global_clock >= 8000 )//
		{
			Globale_flag = 1;
			
		}
		
		
		
		
	  TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}

} 
