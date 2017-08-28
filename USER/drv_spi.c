/**
  ******************************************************************************
  * @author  泽耀科技 ASHINING
  * @version V3.0
  * @date    2016-10-08
  * @brief   SPI配置C文件
  ******************************************************************************
  * @attention
  *
  * 官网	:	http://www.ashining.com
  * 淘宝	:	https://shop105912646.taobao.com
  * 阿里巴巴:	https://cdzeyao.1688.com
  ******************************************************************************
  */



#include "drv_spi.h"


#ifndef __USE_SOFT_SPI_INTERFACE__




uint8_t SPI1_TX_Buff[SPI_TX_SIZE]={0xE1,0x20|0xa0,'C','D','E','F','G','H'};

/*******************************************************************************
* Function Name  : SPI1_DMA_Configuration
* Description    : 配置SPI1_RX的DMA通道2，SPI1_TX的DMA通道3
* Input          : None
* Output         : None
* Return         : None
* Attention             : 
*******************************************************************************/

#define SPI1_DR_Addr ( (u32)0x4001300C )  

void SPI1_DMA_Configuration( void )
{
  DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1 , ENABLE );
	
    /* DMA1 Channel3 (triggered by SPI1 Tx event) Config */
  DMA_DeInit(DMA1_Channel3);  
  DMA_InitStructure.DMA_PeripheralBaseAddr = SPI1_DR_Addr;                          //设置  接收外设(0x4001300C) 地址(源地址)
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)*SPI1_TX_Buff;                    //设置 SRAM 存储地址(源地址)
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                                //传输方向 内存-外设
  DMA_InitStructure.DMA_BufferSize = SPI_TX_SIZE;                           //设置 SPI1 接收长度
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                  //外设地址增量(不变)
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                           //内存地址增量(变化)
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;           //外设传输宽度(字节)
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                   //内存传输宽度(字节)
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                                     //传输方式,一次传输完停止,不重新加载
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                           //中断方式-高(三级)
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                      //内存到内存方式禁止
  DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	
  //DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);                                   //开启 DMA1_Channel3 传输完成中断
  //DMA_ITConfig(DMA1_Channel3, DMA_IT_TE, ENABLE);                                   //开启 DMA1_Channel3 传输错误中断
  /* Enable SPI1 DMA TX request */
	
	SPI_I2S_DMACmd(SPI1,  SPI_I2S_DMAReq_Tx, ENABLE);
//	SPI1->CR2 |= 1<<1;  
  DMA_Cmd(DMA1_Channel3, ENABLE);    //开启 DMA 通道 DMA1_Channel3


  //SPI1->CR2 |= 1<<1;                                                                //发送缓冲区DMA使能
	
                                              
}


void SPI1_TX_DMA_restart(void)
{
	DMA1_Channel3->CCR &= (uint16_t)(~DMA_CCR1_EN);//close dma
	DMA1_Channel3->CMAR = (uint32_t)SPI1_TX_Buff;
	DMA1_Channel3->CNDTR = SPI_TX_SIZE;
	DMA1_Channel3->CCR |= DMA_CCR1_EN;// enable
}


void SPI1_TX_DMA_Send(void)
{
	 DMA_Cmd(DMA1_Channel3, ENABLE);
}


/** 硬件SPI */
#define SPI_WAIT_TIMEOUT			((uint16_t)0xFFFF)

/**
  * @brief :SPI初始化(硬件)
  * @param :无
  * @note  :无
  * @retval:无
  */ 
void drv_spi_init( void )
{
	GPIO_InitTypeDef	SpiGpioInitStructer;
	SPI_InitTypeDef		SpiInitStructer;
	
	/** SPI引脚配置 */
	RCC_APB2PeriphClockCmd( SPI_CLK_GPIO_CLK | SPI_MISO_GPIO_CLK | SPI_MOSI_GPIO_CLK | SPI_NSS_GPIO_CLK, ENABLE );	//打开端口时钟
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	//SCK MOSI MISO 配置为复用
	SpiGpioInitStructer.GPIO_Speed = GPIO_Speed_10MHz;
	SpiGpioInitStructer.GPIO_Mode = GPIO_Mode_AF_PP;
	
	SpiGpioInitStructer.GPIO_Pin = SPI_CLK_GPIO_PIN;
	GPIO_Init( SPI_CLK_GPIO_PORT, &SpiGpioInitStructer );		//初始化SCK
	
	SpiGpioInitStructer.GPIO_Pin = SPI_MOSI_GPIO_PIN;
	GPIO_Init( SPI_MOSI_GPIO_PORT, &SpiGpioInitStructer );		//初始化MOSI
	
	SpiGpioInitStructer.GPIO_Pin = SPI_MISO_GPIO_PIN;
	GPIO_Init( SPI_MISO_GPIO_PORT, &SpiGpioInitStructer );		//初始化MISO
	
	//NSS配置为推挽输出
	SpiGpioInitStructer.GPIO_Mode = GPIO_Mode_Out_PP;
	SpiGpioInitStructer.GPIO_Pin = SPI_NSS_GPIO_PIN;
	GPIO_Init( SPI_NSS_GPIO_PORT, &SpiGpioInitStructer );		//初始化NSS
	GPIO_SetBits( SPI_NSS_GPIO_PORT, SPI_NSS_GPIO_PIN );		//置高

	/** SPI配置 */
	SPI_I2S_DeInit( SPI_PORT );			//复位SPI
	
	if( SPI1 == SPI_PORT )				
	{
		RCC_APB2PeriphClockCmd( SPI_PORT_CLK, ENABLE );			//SPI1在APB2上，打开相应SPI时钟
	}
	else
	{
		RCC_APB1PeriphClockCmd( SPI_PORT_CLK, ENABLE );			//SPI2 3在APB1上
	}
	
	SPI_Cmd( SPI_PORT, DISABLE );		//关闭SPI外设，配置前关闭
	
	SpiInitStructer.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//双线全双工
	SpiInitStructer.SPI_Mode = SPI_Mode_Master;							//主机模式
	SpiInitStructer.SPI_CPOL = SPI_CPOL_Low;							//空闲状态为低电平 
	SpiInitStructer.SPI_CPHA = SPI_CPHA_1Edge;							//第一个边沿采集数据
	SpiInitStructer.SPI_DataSize = SPI_DataSize_8b;						//8位数据
	SpiInitStructer.SPI_NSS = SPI_NSS_Soft;								//从机软件管理
	SpiInitStructer.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;	//32分频
	SpiInitStructer.SPI_FirstBit = SPI_FirstBit_MSB;					//最高位先发送
	SpiInitStructer.SPI_CRCPolynomial = 7;								//CRC多项式,默认不使用SPI自带CRC	 
	
	SPI_Init( SPI_PORT, &SpiInitStructer );
	
	//添加了DMA初始化
	
	SPI_Cmd( SPI_PORT, ENABLE );
	
	
}

/**
  * @brief :SPI收发一个字节
  * @param :
  *			@TxByte: 发送的数据字节
  * @note  :非堵塞式，一旦等待超时，函数会自动退出
  * @retval:接收到的字节
  */
uint8_t drv_spi_read_write_byte( uint8_t TxByte )
{
	uint8_t l_Data = 0;
	uint16_t l_WaitTime = 0;
	
	while( RESET == SPI_I2S_GetFlagStatus( SPI_PORT, SPI_I2S_FLAG_TXE ) )		//等待发送缓冲区为空
	{
		if( SPI_WAIT_TIMEOUT == ++l_WaitTime )
		{
			break;			//如果等待超时则退出
		}
	}
	l_WaitTime = SPI_WAIT_TIMEOUT / 2;		//重新设置接收等待时间(因为SPI的速度很快，正常情况下在发送完成之后会立即收到数据，等待时间不需要过长)
	SPI_PORT->DR = TxByte;	//发送数据
	
	while( RESET == SPI_I2S_GetFlagStatus( SPI_PORT, SPI_I2S_FLAG_RXNE ) )		//等待接收缓冲区非空
	{
		if( SPI_WAIT_TIMEOUT == ++l_WaitTime )
		{
			break;			//如果等待超时则退出
		}
	}
	
	l_Data = (uint8_t)SPI_PORT->DR;		//读取接收数据
	
	return l_Data;		//返回
}

/**
  * @brief :SPI收发字符串
  * @param :
  *			@ReadBuffer: 接收数据缓冲区地址
  *			@WriteBuffer:发送字节缓冲区地址
  *			@Length:字节长度
  * @note  :非堵塞式，一旦等待超时，函数会自动退出
  * @retval:无
  */
void drv_spi_read_write_string( uint8_t* ReadBuffer, uint8_t* WriteBuffer, uint16_t Length )
{
	GPIO_ResetBits( SPI_NSS_GPIO_PORT, SPI_NSS_GPIO_PIN);			//拉低片选
	while( Length-- )
	{
		*ReadBuffer = drv_spi_read_write_byte( *WriteBuffer );		//收发数据
		ReadBuffer++;
		WriteBuffer++;				//读写地址加1
	}
	GPIO_SetBits( SPI_NSS_GPIO_PORT, SPI_NSS_GPIO_PIN);				//拉高片选
}

/** 硬件SPI */
#endif






#ifdef __USE_SOFT_SPI_INTERFACE__

/** 软件SPI */


/**
  * @brief :SPI初始化(软件)
  * @param :无
  * @note  :无
  * @retval:无
  */
void drv_spi_init( void )
{
	GPIO_InitTypeDef	SpiGpioInitStructer;
	
	/** SPI引脚配置 */
	RCC_APB2PeriphClockCmd( SPI_CLK_GPIO_CLK | SPI_MISO_GPIO_CLK | SPI_MOSI_GPIO_CLK | SPI_NSS_GPIO_CLK, ENABLE );	//打开端口时钟
	
	//SCK MOSI NSS配置为推挽输出
	SpiGpioInitStructer.GPIO_Speed = GPIO_Speed_10MHz;
	SpiGpioInitStructer.GPIO_Mode = GPIO_Mode_Out_PP;
	
	SpiGpioInitStructer.GPIO_Pin = SPI_CLK_GPIO_PIN;
	GPIO_Init( SPI_CLK_GPIO_PORT, &SpiGpioInitStructer );		//初始化SCK
	GPIO_ResetBits( SPI_CLK_GPIO_PORT, SPI_CLK_GPIO_PIN);		//初始化状态设置为低
	
	SpiGpioInitStructer.GPIO_Pin = SPI_MOSI_GPIO_PIN;
	GPIO_Init( SPI_MOSI_GPIO_PORT, &SpiGpioInitStructer );		//初始化MOSI
	GPIO_SetBits( SPI_MOSI_GPIO_PORT, SPI_MOSI_GPIO_PIN);		//初始化状态设置为高
	
	SpiGpioInitStructer.GPIO_Pin = SPI_NSS_GPIO_PIN;
	GPIO_Init( SPI_NSS_GPIO_PORT, &SpiGpioInitStructer );		//初始化NSS
	GPIO_SetBits( SPI_NSS_GPIO_PORT, SPI_NSS_GPIO_PIN);			//初始化状态设置为高
	
	//初始化MISO 上拉输入
	SpiGpioInitStructer.GPIO_Mode = GPIO_Mode_IPU;
	SpiGpioInitStructer.GPIO_Pin = SPI_MISO_GPIO_PIN;
	GPIO_Init( SPI_MISO_GPIO_PORT, &SpiGpioInitStructer );		
	GPIO_SetBits( SPI_MISO_GPIO_PORT, SPI_MISO_GPIO_PIN);		//初始化状态设置为高
}

/**
  * @brief :SPI收发一个字节
  * @param :
  *			@TxByte: 发送的数据字节
  * @note  :非堵塞式，一旦等待超时，函数会自动退出
  * @retval:接收到的字节
  */
uint8_t drv_spi_read_write_byte( uint8_t TxByte )
{
	uint8_t i = 0, Data = 0;
	
	spi_set_clk_low( );
	
	for( i = 0; i < 8; i++ )			//一个字节8byte需要循环8次
	{
		/** 发送 */
		if( 0x80 == ( TxByte & 0x80 ))
		{
			spi_set_mosi_hight( );		//如果即将要发送的位为 1 则置高IO引脚
		}
		else
		{
			spi_set_mosi_low( );		//如果即将要发送的位为 0 则置低IO引脚
		}
		TxByte <<= 1;					//数据左移一位，先发送的是最高位
		
		spi_set_clk_high( );			//时钟线置高
		__nop( );
		__nop( );
		
		/** 接收 */
		Data <<= 1;						//接收数据左移一位，先接收到的是最高位
		if( 1 == spi_get_miso( ))
		{
			Data |= 0x01;				//如果接收时IO引脚为高则认为接收到 1
		}
		
		spi_set_clk_low( );				//时钟线置低
		__nop( );
		__nop( );
	}
	
	return Data;		//返回接收到的字节
}

/**
  * @brief :SPI收发字符串
  * @param :
  *			@ReadBuffer: 接收数据缓冲区地址
  *			@WriteBuffer:发送字节缓冲区地址
  *			@Length:字节长度
  * @note  :非堵塞式，一旦等待超时，函数会自动退出
  * @retval:无
  */
void drv_spi_read_write_string( uint8_t* ReadBuffer, uint8_t* WriteBuffer, uint16_t Length )
{
	spi_set_nss_low( );			//片选拉低
	
	while( Length-- )
	{
		*ReadBuffer = drv_spi_read_write_byte( *WriteBuffer );		//收发数据
		ReadBuffer++;
		WriteBuffer++;			//读写地址加1
	}
	
	spi_set_nss_high( );		//片选拉高
}


/** 软件SPI */
#endif


