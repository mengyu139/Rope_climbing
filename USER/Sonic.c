#include "Sonic.h"
#include "delay.h"


struct S_SONIC s_sonic;


void Sonic_para_init(void)
{
	s_sonic.exti_flag = 0;
	
	s_sonic.buff_index = 0;
	
	s_sonic.Clock = 0;
	
	s_sonic.distance_buff[0]=0;
	s_sonic.distance_buff[1]=0;
	s_sonic.distance_buff[2]=0;
	s_sonic.distance_buff[3]=0;
	s_sonic.distance_buff[4]=0;
	
	s_sonic.distance = 0;
	
	
	
}


// pb8 : echo  pb9 : trig
static void Sonic_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	/* config the NVIC(PB8) */

  /* 配置P[A|B|C|D|E]8为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	/* EXTI line gpio config(PB8) */	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	 // 上拉输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* EXTI line(PB8) mode config */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource8); 
  EXTI_InitStructure.EXTI_Line = EXTI_Line8;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure); 

}

void Sonic_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_InternalClockConfig(TIM3);  
	TIM_DeInit(TIM3);
	
	TIM_TimeBaseStructure.TIM_Period = 0xffff;                //2ms定时中断刷新系统
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, & TIM_TimeBaseStructure);
	
	//TIM_ClearITPendingBit(TIM6, TIM_IT_Update);  
	//TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE );  
	
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);  //启动时钟
	
	Sonic_gpio_init();
	
	EXTI_choose_rise();
	
	Sonic_para_init();
	
	CLOSE_TRIG;
}


void clear_exti_flag(void)
{
		EXTI_ClearITPendingBit(EXTI_Line8);
		EXTI_ClearFlag(EXTI_Line8);
}

void Enable_EXTI(void)
{
	EXTI->IMR = (EXTI->IMR | (1<<8));
	EXTI->EMR = (EXTI->EMR | (1<<8));
}

void Disable_EXTI(void)
{
	EXTI->IMR = (EXTI->IMR & (~(1<<8)));
	EXTI->EMR = (EXTI->EMR & (~(1<<8)));
}

void EXTI_choose_rise(void)
{
	EXTI->RTSR = EXTI->RTSR | ( 1<<8 );// ENABLE rise
	EXTI->FTSR = EXTI->FTSR & (~( 1<<8 ));// disable falling
}

void EXTI_choose_fall(void)
{
	EXTI->FTSR = EXTI->FTSR | ( 1<<8 );// ENABLE falling
	EXTI->RTSR = EXTI->RTSR & (~( 1<<8 ));// disable rise
}

void delay_nus(uint16_t time)
{    
   u16 i=0;  
   while(time--)
   {
      i=10;  //????
      while(i--) ;    
   }
}


void Sonic_start(void)
{
	//while(  GPIO_ReadOutputDataBit( GPIOB,  9) );
	
	if( GPIO_ReadOutputDataBit( GPIOB,  9) )
	{
		
	}
	else
	{
		START_TRIG;
		
		clear_exti_flag();
		s_sonic.exti_flag=0;
		
		//choose rising
		EXTI->RTSR = EXTI->RTSR | ( 1<<8 );// ENABLE rise
		EXTI->FTSR = EXTI->FTSR & (~( 1<<8 ));// disable falling
		
		//ENABLE EXTI
		EXTI->IMR = (EXTI->IMR | (1<<8));
		EXTI->EMR = (EXTI->EMR | (1<<8));
		
		delay_nus(20);
		
		CLOSE_TRIG;
	}

}


float Sonic_update(void)
{
	int i = 0;
	int max_index=0;
	int min_index = 0;
	
	float sum = 0;
	
	for( i=1;i<5;i++ )
	{
		if( s_sonic.distance_buff[i] > s_sonic.distance_buff[ max_index ] )
		{
			max_index = i;
		}
		
		if( s_sonic.distance_buff[i] < s_sonic.distance_buff[ min_index ] )
		{
			min_index = i;
		}
		
		sum += s_sonic.distance_buff[i];
	}
	sum += s_sonic.distance_buff[0];
	sum = sum - s_sonic.distance_buff[ max_index ] - s_sonic.distance_buff[ min_index ];
	sum = sum / 3;
	return sum;
}



//void task_Sonic_update(void)
//{
//	if( s_sonic.Clock >= 40 )
//	{
//		s_sonic.Clock = 0;
//		
//		s_sonic.distance = Sonic_update();
//		
//	}
//}