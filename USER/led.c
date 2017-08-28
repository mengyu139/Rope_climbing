
#include "led.h"
#include "clock.h"
#include "Config.h"
/*
 * ��������LED_GPIO_Config
 * ����  ������LED�õ���I/O��
 * ����  ����
 * ���  ����
 */
void LED_GPIO_Config(void)
{		
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����GPIOC������ʱ��*/
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
		  

	/* �ر�����led��	*/
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
