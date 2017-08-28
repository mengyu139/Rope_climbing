
#include "stm32f10x.h"
#include "led.h"
#include <math.h>
#include <stdio.h>
#include "delay.h"
#include "Motor.h"
#include "uart.h"
#include "clock.h"

#include "drv_spi.h"
#include "drv_RF24L01.h"
#include "Sonic.h"
#include "protocal.h"
#include "CAN1.h"
#include "sys.h"



//1.³¬Éù²¨   ok 
//2.PWM  1 channel   ok
//3.±àÂëÆ÷ AB   ok 
//4.SPI ÎÞÏßÄ£¿é ok
//5. Í¸Ã÷´®¿Ú  
//6. ¶ÁÈ¡³¬Éù²¨ ok


extern uint8_t control_sweep ;
float s_buff[Discrete_num];

uint8_t send_buff[10];




void SendValue(int32_t v)
{
	if( v >0 )
	{
		send_buff[0] = '+';
		v = v;
	}
	else
	{
		send_buff[0] = '-';
		v = -v;
	}
	
	send_buff[1] = v /1000000 + 0x30;
	send_buff[2] = v%1000000/100000 + 0x30;
	send_buff[3] = v%100000/ 10000 + 0x30;
	send_buff[4] = v%10000/1000 + 0x30;
	send_buff[5] = v%1000/100 + 0x30;
	send_buff[6] = v%100/10 + 0x30;
	send_buff[7] = v%10 + 0x30;
	
	send_some_bytes( send_buff, 8 );
}

void Send_data(uint8_t type , int32_t v , int end)
{
	usart_send_byte(type);
	SendValue(v);
	
	if( end )
	{
		usart_send_byte('\n');
	}
	else
	{
		usart_send_byte(' ');
	}
}



void init_distance(void)
{
	int i;
	for ( i =0 ;i < Discrete_num;i ++)
	{
		s_buff[ i ] = AMPHILE * sin( 2 * 3.1415926 / Discrete_num * i  );
	}
}

uint16_t target = 1500;

uint16_t tim6_cnt = 0;

uint8_t exti_switch = 0;

float x = 0;
uint8_t x_buff[4];
	

uint8_t robot_data[8]={0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};

int main(void)
{	
	
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);


	delay_init();
	LED_GPIO_Config();// PB14

	
//	motor_pid_init();
	
	
	CAN1_Configuration();// RX PB8  TX PB9
	delay_ms(100);
	
	
	Motor_init();
	
	
	Clock_init();

	init_distance();
	//Sonic_init();// PB8  PB9
	
	USART2_Config();//PA2 TX  PA3  RX
	
	delay_ms(100);
	

	#if 0
	if( MY_ID == 0 )
	{
		s_communication.s_car[0].target_height = 1.5;
		s_communication.s_car[0].height = 1.2;
		s_communication.s_car[0].speed = 0.2;
	}
	if( MY_ID == 1 ) 
	{
		s_communication.s_car[1].height = 1.3;
		s_communication.s_car[1].speed = 0.3;
	}
	#endif
	
	
	
	while(1)
	{	
		
		#if 1
		if( MY_ID == 0 )
		{
			Set_target(MY_ID,x);
			//control_sweep = 0;
		}
		
		
	
		
		//RoboModule_open_loop_mode_set_data(MOTOR_ALL,x);
		
		#endif
		
		
		
		delay_ms(500);
		
		//PBout(14)= ~PBout(14);
	  
	
	
		
	}


	
	
}
