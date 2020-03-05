/**
  ******************************************************************************
  * @file    bsp_usart1.c
  * @author  野火
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   重现c库printf函数到usart端口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "..\User\bsp\usart\bsp_usart1.h"
#include "includes.h"

uint8_t Usart1RxBuff[RXBUFF_SIZE];
void USAR1Rx_DMA_Config(void);

 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 配置USART为中断源 */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQ;
    /* 抢断优先级*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    /* 子优先级 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    /* 使能中断 */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* 初始化配置NVIC */
    NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  USART1 GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */
void USART1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	USART_TypeDef *uart;

	/* 第1步：打开GPIO和USART部件的时钟 */
	macUSART_APBxClock_FUN(macUSART_CLK, ENABLE);
	macUSART_GPIO_APBxClock_FUN(macUSART_GPIO_CLK, ENABLE);
  macUSART_GPIO_APBxClock_FUN(RCC_APB2Periph_GPIOE, ENABLE);
  
  /* 8266 */
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  GPIO_ResetBits(GPIOE,GPIO_Pin_0);
  GPIO_SetBits(GPIOE,GPIO_Pin_1);
  
  /* 8266 */

	/* 第2步：将USART Tx的GPIO配置为推挽复用模式 */
	GPIO_InitStructure.GPIO_Pin = macUSART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(macUSART_TX_PORT, &GPIO_InitStructure);

	/* 第3步：将USART Rx的GPIO配置为浮空输入模式
		由于CPU复位后，GPIO缺省都是浮空输入模式，因此下面这个步骤不是必须的
		但是，我还是建议加上便于阅读，并且防止其它地方修改了这个口线的设置参数
	*/
	GPIO_InitStructure.GPIO_Pin = macUSART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(macUSART_RX_PORT, &GPIO_InitStructure);
	/*  第2步已经做了，因此这步可以不做
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	*/
	
	/* 第4步：设置串口硬件参数 */
	uart = macUSARTx;	
	USART_InitStructure.USART_BaudRate = macUSART_BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(uart, &USART_InitStructure);
	USART_ITConfig(uart, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
	/* 
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		注意: 不要在此处打开发送中断
		发送中断使能在SendUart()函数打开
	*/
  // 串口中断优先级配置
	NVIC_Configuration();
	
	// 使能串口空闲中断
	USART_ITConfig(macUSARTx, USART_IT_IDLE, ENABLE);	
	USART_Cmd(uart, ENABLE);		/* 使能串口 */ 

	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* 清发送外城标志，Transmission Complete flag */
  
  USAR1Rx_DMA_Config();
  /* 关闭 8266 回显功能 */
  
  
}

void USAR1Rx_DMA_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;
	
    // 开启DMA时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // 设置DMA源地址：串口数据寄存器地址*/
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_DR_ADDRESS;
    // 内存地址(要传输的变量的指针)
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Usart1RxBuff;
    // 方向：从外设到内存
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    // 传输大小	
    DMA_InitStructure.DMA_BufferSize = RXBUFF_SIZE;
    // 外设地址不增	    
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    // 内存地址自增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    // 外设数据单位	
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    // 内存数据单位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 
    // DMA模式，一次或者循环模式
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
    //DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
    // 优先级：中	
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
    // 禁止内存到内存的传输
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    // 配置DMA通道		   
    DMA_Init(USART1_RX_DMA_CHANNEL, &DMA_InitStructure);		
    // 使能DMA
    DMA_Cmd (USART1_RX_DMA_CHANNEL,ENABLE);
    
    /* USART1 向 DMA发出Rx请求 */
    USART_DMACmd(macUSARTx, USART_DMAReq_Rx, ENABLE);    // 开启一次串口接收
}

///重定向c库函数printf到USART1
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到USART1 */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///重定向c库函数scanf到USART1
int fgetc(FILE *f)
{
		/* 等待串口1输入数据 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}

u8 USART1_RX_Len = 0;

void USART1_IRQHandler(void)
{
    OSIntEnter();    // 进入中断
    if (USART1_RX_Len == 0)
    {
        if(USART_GetITStatus(macUSARTx, USART_IT_IDLE)!=RESET)    // 总线空闲中断
        {
            USART_DMACmd(macUSARTx, USART_DMAReq_Tx, DISABLE);    // 禁用 DMA 传输
            
            USART1_RX_Len = RXBUFF_SIZE - DMA_GetCurrDataCounter(USART1_RX_DMA_CHANNEL);    // 计算串口接收到的长度
            
            /* 清DMA标志位 */
            DMA_ClearFlag(DMA1_FLAG_TC5);
            
          
            
            
    //        // 禁用串口空闲中断
    //        USART_ITConfig(macUSARTx, USART_IT_IDLE, DISABLE);
            // 使能 DMA
       DMA_Cmd (USART1_RX_DMA_CHANNEL,ENABLE);

    //    /* USART1 向 DMA 发出Rx请求 */
        USART_DMACmd(macUSARTx, USART_DMAReq_Rx, ENABLE);    // 开启下一次串口接收
            
            // 清除空闲中断标志位
        }
    }
    if(USART_GetITStatus(macUSARTx, USART_IT_IDLE)!=RESET)    // 总线空闲中断
    {
        USART_ClearFlag(macUSARTx, USART_IT_IDLE);     // 清除空闲中断
        USART_ReceiveData(macUSARTx);    // 读 DR， 清除接收缓冲区非空标志
    }
    OSIntExit();    // 退出中断
}

void USART1_Start_Rx(void)
{
    USART1_RX_Len = 0;
    USART_DMACmd(macUSARTx, USART_DMAReq_Tx, DISABLE);    // 禁用 DMA 传输
    /* 重新配置 DMA */
    DMA_Cmd (USART1_RX_DMA_CHANNEL,DISABLE);                  // 禁用 DMA 流EF 01 00 00 00 00 00 07
    USART1_RX_DMA_CHANNEL->CNDTR = RXBUFF_SIZE;
    
    // 使能 DMA
    DMA_Cmd (USART1_RX_DMA_CHANNEL,ENABLE);

    /* USART1 向 DMA 发出Rx请求 */
    USART_DMACmd(macUSARTx, USART_DMAReq_Rx, ENABLE);    // 开启下一次串口接收
}
/*********************************************END OF FILE**********************/
