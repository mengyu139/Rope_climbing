/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "uart.h"
#include "Sonic.h"

uint16_t exti_flag = 0;
	
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
	
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}



uint8_t crc;
uint16_t distance;



void USART2_IRQHandler(void)
{
	//USART_ReceiveData( USART2 );
	
	if (USART_GetITStatus(USART2, USART_IT_ORE) == SET)  
	{  
			USART_ClearITPendingBit(USART2,USART_IT_ORE);      
			USART_ReceiveData( USART2 );  

	}
		
		
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{ 	
	  int i;
		

		USART_ClearFlag(USART2,USART_FLAG_RXNE);
		USART_ClearITPendingBit(USART2,USART_FLAG_RXNE);
			
	  	//printf("%c",c);    //将接受到的数据直接返回打印
	} 
	 
}


void EXTI9_5_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line8) != RESET) //确保是否产生了EXTI Line8中断
  {
		exti_flag ++;
		
		//rising exti
		if( s_sonic.exti_flag == 0 )
		{
			s_sonic.exti_flag = 1;
			
			//choose falling exti
			EXTI->FTSR = EXTI->FTSR | ( 1<<8 );// ENABLE falling
			EXTI->RTSR = EXTI->RTSR & (~( 1<<8 ));// disable rise
			
			TIM3->CNT = 0;
			
		}
		
		
		//falling exti
		else
		{
			s_sonic.exti_flag = 0;
			
			s_sonic.distance_tmp = ((float)TIM3->CNT /1000000.0 * 340.0 /2.0 * 100.0); // to 'cm'
			
			if( s_sonic.distance_tmp < 600 )
			{
				s_sonic.buff_index = (s_sonic.buff_index + 1) % 5;
				
				s_sonic.distance_buff[s_sonic.buff_index] = s_sonic.distance_tmp;
				
				s_sonic.distance = Sonic_update();
				//s_sonic.distance = s_sonic.distance_tmp;
			}
			
			
			
			
			//choose rising exti
			EXTI->RTSR = EXTI->RTSR | ( 1<<8 );// ENABLE rise
			EXTI->FTSR = EXTI->FTSR & (~( 1<<8 ));// disable falling
			
			//disable EXTI
			EXTI->IMR = (EXTI->IMR & (~(1<<8)));
			EXTI->EMR = (EXTI->EMR & (~(1<<8)));
				
			
		}

		
		EXTI_ClearITPendingBit(EXTI_Line8);
		EXTI_ClearFlag(EXTI_Line8);
  }  
}




/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
