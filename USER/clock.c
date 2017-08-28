#include "clock.h"
#include "led.h"
#include "Motor.h"
#include "Sonic.h"
#include "uart.h"
#include "CAN1.h"
#include "protocal.h"
uint32_t led_cnt = 0;

uint32_t Global_clock = 0;
uint32_t send_clock  =0;

volatile int flag = 0;

uint16_t Index = 0;

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
		#if 1
		task_motor_control(1);
		#endif
		
		
		//超声波启动
		
		if( s_sonic.Clock >= 15 )//30ms
		{
			s_sonic.Clock = 0;
			//Sonic_start();
		}
		
		
		//set target and share info through CAN
		if( s_can_msg.Clock % 2 == 0 )
		{
			//ONLY master set transmit the target to others
			if( MY_ID == 0 )
			{
				Proto_send(0);
				
				if( control_sweep == 1 )
				{
					s_can_msg.tx_buff[0] = 0xff;
					CAN1_Send( 0x202 , s_can_msg.tx_buff );
				}
			}
		}
		else
		{
			Proto_send(1);
			
			
		}
		
		
		//超声波更新数值
		//task_Sonic_update();
		
		//检测串口接受数据
		//task_uart_detect();
		
		
		
		
		
		if( Global_clock >= 4000 )//20ms
		{
			Global_clock = 0;
			
			
			//set_target( 1, -2.0 );
			#if 0
			if( flag == 0 )
			{
				flag = 1;
				set_target( 1,0.5 );
				GPIO_SetBits(GPIOA, GPIO_Pin_8);	
			}
			else if( flag == 1 )
			{
				flag = 2;
				set_target( -1,0 );
				GPIO_ResetBits(GPIOA, GPIO_Pin_8);	
			}
			else if( flag == 2 )
			{
				flag = 3;
				set_target( -1,-0.5);
				GPIO_SetBits(GPIOA, GPIO_Pin_8);	
			}
			else 
			{
				flag = 0;
				
				set_target( +1,0);
				GPIO_ResetBits(GPIOA, GPIO_Pin_8);	
			}
			#endif

			
		}
		
		
		
		
	  TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}

} 
