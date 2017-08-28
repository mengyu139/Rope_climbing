
#include "led.h"
#include "clock.h"
#include "Config.h"
/*
 * 函数名：LED_GPIO_Config
 * 描述  ：配置LED用到的I/O口
 * 输入  ：无
 * 输出  ：无
 */
void LED_GPIO_Config(void)
{		
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启GPIOC的外设时钟*/
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE); 

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	
	if( GREEN_BOARD == 1 )
	{
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	else
	{
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;	
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	}
		  

	/* 关闭所有led灯	*/
	LED = 1;
}


void Led_Flash(void)
{
	static int flag = 0;
	
	if( led_cnt >= 500 )
	{
		led_cnt = 0;
		
		LED = ~LED;
	
		
	}
	
	
	
}


/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
