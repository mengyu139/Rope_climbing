#include "uart.h"
#include "protocal.h"




void Uart_para_init(void)
{
	int i=0;
	s_communication.over_time_cnt=0;
	s_communication.rev_cnt=0;
	s_communication.rev_last_cnt=0;
	s_communication.signal = 0;
	
	for(i=0;i<MAX_REV_NUM;i++)
	{
		s_communication.rev_buff[i] = 0;
	}
	
	for(i=0;i<45;i++)
	{
		s_communication.send_buff[i] = 0;
	}
	
	for(i=0;i<4;i++)
	{
		s_communication.s_car[i].speed = 0;
		s_communication.s_car[i].target_speed = 0;
		s_communication.s_car[i].status = 0;
		s_communication.s_car[i].target_status = 0;
		s_communication.s_car[i].height=0;
		s_communication.s_car[i].target_height=0;
	}
	
	
	
}

void DMA_Uart_config(void)
{
	DMA_InitTypeDef   DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	//DMA_DeInit(DMA1_Channel6);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)s_communication.rev_buff;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = MAX_REV_NUM;//256
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//  内存自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);
	//DMA_ITConfig(DMA1_Channel5,DMA_IT_TC, ENABLE);//使能或者失能指定的通道x中断(DMA1 通道5)
	DMA_Cmd(DMA1_Channel6, ENABLE);
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
}



void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}




void USART2_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	//NVIC_Configuration();
	
	/* USART2 GPIO config */
	/* Configure USART1 Tx (PA.02) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
	/* Configure USART1 Rx (PA.3) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	//USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	

	DMA_Uart_config();
	
//	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	//USART_ITConfig(USART2, USART_IT_ORE, ENABLE);
	 
	USART_Cmd(USART2, ENABLE);
	
	Uart_para_init();
}


void usart_send_byte(uint8_t c)
{
		while ((USART2->SR & USART_FLAG_TC) == (uint16_t)RESET);
		USART_SendData(USART2,c);
}

void usart_send_string( uint8_t * s )
{
	int i=0;
	while( *(s+i) != 0 )
	{
		usart_send_byte( *(s+i) );
		i++;
	}
		
}






void task_uart_detect(void)
{
	int i;
	s_communication.rev_cnt = MAX_REV_NUM - DMA_GetCurrDataCounter(DMA1_Channel6);
	
	// no data
	if( s_communication.rev_cnt == 0 )
	{
		s_communication.rev_cnt=0;
		s_communication.rev_last_cnt=0;
		s_communication.over_time_cnt=0;
		s_communication.signal=0;
		return;
	}

	else
	{
		if( s_communication.rev_cnt == s_communication.rev_last_cnt )
		{
			s_communication.over_time_cnt ++;
			
			//over time
			if( s_communication.over_time_cnt > UART_OVER_TIME )
			{
				//1. shutdown dma
				DMA1_Channel6->CCR &= (uint16_t)(~DMA_CCR1_EN);
				
				
				//2. protocol process
				//protocol_rev();
				
				//x. clear rev buff
				for(i=0;i<s_communication.rev_cnt;i++)
				{
					s_communication.rev_buff[i] = 0;
				}
				s_communication.over_time_cnt=0;
				s_communication.rev_last_cnt=0;
				s_communication.rev_cnt=0;
				
				//x. restart dma
				DMA1_Channel6->CMAR = (uint32_t)s_communication.rev_buff;
				DMA1_Channel6->CNDTR = MAX_REV_NUM;
				DMA1_Channel6->CCR |= DMA_CCR1_EN;
			}
			
			
		}
		else
		{
			s_communication.rev_last_cnt = s_communication.rev_cnt;
			return;
		}
		
	}
	
}

  
void send_some_bytes(uint8_t * buff , int num)
{
	int i;
	
	for( i=0;i<num;i++ )
	{
		usart_send_byte( (*(buff+i)) );
	}
}


