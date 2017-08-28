#include "CAN1.h"
#include "led.h"
#include "protocal.h"
#include "delay.h"
#include "Config.h"

struct S_CAN_MSG s_can_msg;
	
	

/****************************************************************************************
  * @函数描述：  CAN初始化
  * @入口参数：  无.
  * @返回值  :   无.
****************************************************************************************/
void CAN1_GPIOConfig(void)
{
	
 	GPIO_InitTypeDef GPIO_InitStructure;
   	
	if( GREEN_BOARD==0 )
	{
		/*外设时钟设置*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

		/*IO设置*/
		GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
		/* Configure CAN pin: RX */									               // PB8
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	             // 上拉输入
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		/* Configure CAN pin: TX */									               // PB9
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		         // 复用推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	}
	else
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);      
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;                                     //Rx
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;                                     //Tx
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
	}
 
}




void CAN1_NVIC_Configuration(void)
{
  	NVIC_InitTypeDef NVIC_InitStructure;

  	NVIC_InitStructure.NVIC_IRQChannel=USB_LP_CAN1_RX0_IRQn;;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
}
	
	
void CAN1_Configuration(void)
{
	int i=0;
	
	uint32_t filter_id = 0;
	uint32_t filter_mask = 0;
	
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);    //注意，挂在APB1

	CAN1_GPIOConfig();
	CAN1_NVIC_Configuration();
	
	/************************模式与波特率设置*************/
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	
	CAN_InitStructure.CAN_TTCM=DISABLE; 
	CAN_InitStructure.CAN_ABOM=DISABLE;
	CAN_InitStructure.CAN_AWUM=DISABLE; 
	CAN_InitStructure.CAN_NART=DISABLE;
	CAN_InitStructure.CAN_RFLM=DISABLE; 
	CAN_InitStructure.CAN_TXFP=DISABLE;
	
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;   //CAN_Mode_LoopBack：回环模式，CAN_Mode_Normal:正常模式
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_1tq; 
	CAN_InitStructure.CAN_Prescaler = 9;            //500K通信速度
	CAN_Init(CAN1,&CAN_InitStructure);
	
	/********************** 0 *过滤器设置********************/
	CAN_FilterInitStructure.CAN_FilterNumber = 0;    //0号过滤器
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; 
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; 
	
	//想要只接受 0x2?? 的数据，即STM32之间共享数据的 can数据
	filter_id = (0x200 << 21);
	CAN_FilterInitStructure.CAN_FilterIdHigh = (uint16_t)(filter_id >> 16);         //关闭所有屏蔽
	CAN_FilterInitStructure.CAN_FilterIdLow = (uint16_t)filter_id; 
	
	filter_mask = (0x200 << 21);
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (uint16_t)(filter_mask >> 16); 
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = (uint16_t)filter_mask;
	
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;    //传入FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; 
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	
		/********************* 1 **过滤器设置********************/
	CAN_FilterInitStructure.CAN_FilterNumber = 1;    //1号过滤器
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; 
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; 
	
	//想要只接受 0x2?? 的数据，即STM32之间共享数据的 can数据
	filter_id = (0X00B | ( (MY_ID+1)<<4 ))<<21;
	
	CAN_FilterInitStructure.CAN_FilterIdHigh = (uint16_t)(filter_id >> 16);         //关闭所有屏蔽
	CAN_FilterInitStructure.CAN_FilterIdLow = (uint16_t)filter_id; 
	
	filter_mask = 0XFFF<<21;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (uint16_t)(filter_mask >> 16); 
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = (uint16_t)filter_mask;
	
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);         //开启接收中断
	
	for( i=0;i<8;i++ )
	{
		s_can_msg.tx_buff[i]=0;
	}
	s_can_msg.Clock=0;
	
	
	
	
}
void CAN1_Send(uint32_t ID,u8 *Data)       
{  
	u8 TransmitMailbox=0,i=0;  
	CanTxMsg TxMessage;  
	
	TxMessage.StdId = ID;            
	TxMessage.ExtId = 0x0000;
	TxMessage.IDE = CAN_ID_STD; 
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.DLC = 8; 
	
	for(i=0;i<8;i++)
	{
		TxMessage.Data[i] = *Data++; 
	}
	
	TransmitMailbox=CAN_Transmit(CAN1,&TxMessage);
	
	while((CAN_TransmitStatus(CAN1,TransmitMailbox)!=CANTXOK)&&(i!=0xFF))
	{
		i++;
	}

}



void USB_LP_CAN1_RX0_IRQHandler(void)
{

	CAN_Receive(CAN1,CAN_FIFO0,&s_can_msg.RxMessage);
	
//	if( s_can_msg.RxMessage.StdId == 0x201 )
//	{
//			if( s_can_msg.rev_flag == 1 )
//			{
//				s_can_msg.rev_flag = 0;
//				GPIO_ResetBits(GPIOA, GPIO_Pin_8);	
//			}
//			else
//			{
//				s_can_msg.rev_flag = 1;
//				GPIO_SetBits(GPIOA, GPIO_Pin_8);	
//			}
//	}
	
	protocol_rev();
	
} 


