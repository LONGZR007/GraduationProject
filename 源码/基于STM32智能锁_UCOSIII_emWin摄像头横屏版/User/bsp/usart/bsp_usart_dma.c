
#include "bsp_usart_dma.h"
#include "includes.h"

//uint8_t *SendBuff;
uint8_t RxBuff[SENDBUFF_SIZE];

 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 嵌套向量中断控制器组选择 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* 配置USART为中断源 */
    NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
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
  * @brief  USART GPIO 配置,工作参数配置
  * @param  无
  * @retval 无
  */
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// 打开串口GPIO的时钟
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	
	// 打开串口外设的时钟
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

	// 将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  // 将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	// 配置串口的工作参数
	// 配置波特率
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
	// 配置 针数据字长
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// 配置停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// 配置校验位
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	// 配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 完成串口的初始化配置
	USART_Init(DEBUG_USARTx, &USART_InitStructure);

	// 串口中断优先级配置
	NVIC_Configuration();
	
	// 使能串口空闲中断
	USART_ITConfig(DEBUG_USARTx, USART_IT_IDLE, ENABLE);	
	
	// 使能串口
	USART_Cmd(DEBUG_USARTx, ENABLE);	    
}

/**
  * @brief  USARTx TX DMA 配置，内存到外设(USART1->DR)
  * @param  无
  * @retval 无
  */
//void USARTx_DMA_Config(void)
//{
//    DMA_InitTypeDef DMA_InitStructure;

//    // 开启DMA时钟
//    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
//    // 设置DMA源地址：串口数据寄存器地址*/
//    DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_ADDRESS;
//    // 内存地址(要传输的变量的指针)
//    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SendBuff;
//    // 方向：从内存到外设	
//    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
//    // 传输大小	
//    DMA_InitStructure.DMA_BufferSize = SENDBUFF_SIZE;
//    // 外设地址不增	    
//    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//    // 内存地址自增
//    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//    // 外设数据单位	
//    DMA_InitStructure.DMA_PeripheralDataSize = 
//    DMA_PeripheralDataSize_Byte;
//    // 内存数据单位
//    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 
//    // DMA模式，一次或者循环模式
//    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
//    //DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
//    // 优先级：中	
//    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
//    // 禁止内存到内存的传输
//    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//    // 配置DMA通道		   
//    DMA_Init(USART_TX_DMA_CHANNEL, &DMA_InitStructure);		
//    // 使能DMA
//    DMA_Cmd (USART_TX_DMA_CHANNEL,ENABLE);
//}

void USARRx_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	
    // 开启DMA时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // 设置DMA源地址：串口数据寄存器地址*/
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_ADDRESS;
    // 内存地址(要传输的变量的指针)
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)RxBuff;
    // 方向：从外设到内存
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    // 传输大小	
    DMA_InitStructure.DMA_BufferSize = SENDBUFF_SIZE;
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
    DMA_Init(USART_RX_DMA_CHANNEL, &DMA_InitStructure);		
    // 使能DMA
    DMA_Cmd (USART_RX_DMA_CHANNEL,ENABLE);
    
    /* USART1 向 DMA发出Rx请求 */
    USART_DMACmd(DEBUG_USARTx, USART_DMAReq_Rx, ENABLE);    // 开启一次串口接收
    
    /* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART2, USART_FLAG_TC);     /* 清发送外城标志，Transmission Complete flag */
}

//void USARTx_DMA_SendData(uint8_t *data, uint16_t len)
//{
//    // 失能DMA
//    DMA_Cmd (USART_TX_DMA_CHANNEL,DISABLE);
//    USART_TX_DMA_CHANNEL->CMAR = (uint32_t)data;
//    DMA_SetCurrDataCounter(USART_TX_DMA_CHANNEL, len);
//    // 使能DMA
//    DMA_Cmd (USART_TX_DMA_CHANNEL,ENABLE);
//    
//    
//    
//    /* USART1 向 DMA发出TX请求 */
//    USART_DMACmd(DEBUG_USARTx, USART_DMAReq_Tx, ENABLE);
//}



void DEBUG_USART_IRQHandler(void)
{
    OS_ERR      err;
    uint16_t Len;
    OSIntEnter();    // 进入中断
//    printf("进入串口中断\n");
    if(USART_GetITStatus(DEBUG_USARTx, USART_IT_IDLE)!=RESET)    // 总线空闲中断
    {
        USART_DMACmd(DEBUG_USARTx, USART_DMAReq_Tx, DISABLE);    // 禁用 DMA 传输
        
        Len = SENDBUFF_SIZE - DMA_GetCurrDataCounter(USART_RX_DMA_CHANNEL);    // 计算串口接收到的长度
//        printf("进入串口中断：%d\n", Len);
        /* 发布消息到消息队列 queue */
        OSQPost ((OS_Q       *)&queue,                             // 消息变量指针
                (uint8_t     *)RxBuff,                             // 要发送的数据的指针，将内存块首地址通过队列“发送出去”
                (OS_MSG_SIZE  )Len,                                // 数据字节大小
                (OS_OPT       )OS_OPT_POST_FIFO | OS_OPT_POST_ALL, // 先进先出和发布给全部任务的形式
                (OS_ERR      *)&err);	                           // 返回错误类型
        
        /* 清DMA标志位 */
        DMA_ClearFlag(DMA1_FLAG_TC5);
        /* 重新配置 DMA */
        DMA_Cmd (USART_RX_DMA_CHANNEL,DISABLE);                  // 禁用 DMA 流EF 01 00 00 00 00 00 07
        USART_RX_DMA_CHANNEL->CNDTR = SENDBUFF_SIZE;
        
        // 使能 DMA
        DMA_Cmd (USART_RX_DMA_CHANNEL,ENABLE);
        
        /* USART1 向 DMA 发出Rx请求 */
        USART_DMACmd(DEBUG_USARTx, USART_DMAReq_Rx, ENABLE);    // 开启下一次串口接收
      
        USART_ClearFlag(DEBUG_USARTx, USART_IT_IDLE);     // 清除空闲中断
        // 清除空闲中断标志位
        USART_ReceiveData(DEBUG_USARTx);    // 读 DR， 清除接收缓冲区非空标志
    }
    
    OSIntExit();    // 退出中断
}






