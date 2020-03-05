
#include "bsp_usart_dma.h"
#include "includes.h"

//uint8_t *SendBuff;
uint8_t RxBuff[SENDBUFF_SIZE];

 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Ƕ�������жϿ�������ѡ�� */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* ����USARTΪ�ж�Դ */
    NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
    /* �������ȼ�*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    /* �����ȼ� */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    /* ʹ���ж� */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /* ��ʼ������NVIC */
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  USART GPIO ����,������������
  * @param  ��
  * @retval ��
  */
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// �򿪴���GPIO��ʱ��
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	
	// �򿪴��������ʱ��
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

	// ��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  // ��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	// ���ô��ڵĹ�������
	// ���ò�����
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
	// ���� �������ֳ�
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// ����ֹͣλ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// ����У��λ
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	// ����Ӳ��������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// ���ù���ģʽ���շ�һ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// ��ɴ��ڵĳ�ʼ������
	USART_Init(DEBUG_USARTx, &USART_InitStructure);

	// �����ж����ȼ�����
	NVIC_Configuration();
	
	// ʹ�ܴ��ڿ����ж�
	USART_ITConfig(DEBUG_USARTx, USART_IT_IDLE, ENABLE);	
	
	// ʹ�ܴ���
	USART_Cmd(DEBUG_USARTx, ENABLE);	    
}

/**
  * @brief  USARTx TX DMA ���ã��ڴ浽����(USART1->DR)
  * @param  ��
  * @retval ��
  */
//void USARTx_DMA_Config(void)
//{
//    DMA_InitTypeDef DMA_InitStructure;

//    // ����DMAʱ��
//    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
//    // ����DMAԴ��ַ���������ݼĴ�����ַ*/
//    DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_ADDRESS;
//    // �ڴ��ַ(Ҫ����ı�����ָ��)
//    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SendBuff;
//    // ���򣺴��ڴ浽����	
//    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
//    // �����С	
//    DMA_InitStructure.DMA_BufferSize = SENDBUFF_SIZE;
//    // �����ַ����	    
//    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//    // �ڴ��ַ����
//    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//    // �������ݵ�λ	
//    DMA_InitStructure.DMA_PeripheralDataSize = 
//    DMA_PeripheralDataSize_Byte;
//    // �ڴ����ݵ�λ
//    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 
//    // DMAģʽ��һ�λ���ѭ��ģʽ
//    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
//    //DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
//    // ���ȼ�����	
//    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
//    // ��ֹ�ڴ浽�ڴ�Ĵ���
//    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//    // ����DMAͨ��		   
//    DMA_Init(USART_TX_DMA_CHANNEL, &DMA_InitStructure);		
//    // ʹ��DMA
//    DMA_Cmd (USART_TX_DMA_CHANNEL,ENABLE);
//}

void USARRx_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	
    // ����DMAʱ��
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // ����DMAԴ��ַ���������ݼĴ�����ַ*/
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART_DR_ADDRESS;
    // �ڴ��ַ(Ҫ����ı�����ָ��)
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)RxBuff;
    // ���򣺴����赽�ڴ�
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    // �����С	
    DMA_InitStructure.DMA_BufferSize = SENDBUFF_SIZE;
    // �����ַ����	    
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    // �ڴ��ַ����
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    // �������ݵ�λ	
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    // �ڴ����ݵ�λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 
    // DMAģʽ��һ�λ���ѭ��ģʽ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
    //DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
    // ���ȼ�����	
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; 
    // ��ֹ�ڴ浽�ڴ�Ĵ���
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    // ����DMAͨ��		   
    DMA_Init(USART_RX_DMA_CHANNEL, &DMA_InitStructure);		
    // ʹ��DMA
    DMA_Cmd (USART_RX_DMA_CHANNEL,ENABLE);
    
    /* USART1 �� DMA����Rx���� */
    USART_DMACmd(DEBUG_USARTx, USART_DMAReq_Rx, ENABLE);    // ����һ�δ��ڽ���
    
    /* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART2, USART_FLAG_TC);     /* �巢����Ǳ�־��Transmission Complete flag */
}

//void USARTx_DMA_SendData(uint8_t *data, uint16_t len)
//{
//    // ʧ��DMA
//    DMA_Cmd (USART_TX_DMA_CHANNEL,DISABLE);
//    USART_TX_DMA_CHANNEL->CMAR = (uint32_t)data;
//    DMA_SetCurrDataCounter(USART_TX_DMA_CHANNEL, len);
//    // ʹ��DMA
//    DMA_Cmd (USART_TX_DMA_CHANNEL,ENABLE);
//    
//    
//    
//    /* USART1 �� DMA����TX���� */
//    USART_DMACmd(DEBUG_USARTx, USART_DMAReq_Tx, ENABLE);
//}



void DEBUG_USART_IRQHandler(void)
{
    OS_ERR      err;
    uint16_t Len;
    OSIntEnter();    // �����ж�
//    printf("���봮���ж�\n");
    if(USART_GetITStatus(DEBUG_USARTx, USART_IT_IDLE)!=RESET)    // ���߿����ж�
    {
        USART_DMACmd(DEBUG_USARTx, USART_DMAReq_Tx, DISABLE);    // ���� DMA ����
        
        Len = SENDBUFF_SIZE - DMA_GetCurrDataCounter(USART_RX_DMA_CHANNEL);    // ���㴮�ڽ��յ��ĳ���
//        printf("���봮���жϣ�%d\n", Len);
        /* ������Ϣ����Ϣ���� queue */
        OSQPost ((OS_Q       *)&queue,                             // ��Ϣ����ָ��
                (uint8_t     *)RxBuff,                             // Ҫ���͵����ݵ�ָ�룬���ڴ���׵�ַͨ�����С����ͳ�ȥ��
                (OS_MSG_SIZE  )Len,                                // �����ֽڴ�С
                (OS_OPT       )OS_OPT_POST_FIFO | OS_OPT_POST_ALL, // �Ƚ��ȳ��ͷ�����ȫ���������ʽ
                (OS_ERR      *)&err);	                           // ���ش�������
        
        /* ��DMA��־λ */
        DMA_ClearFlag(DMA1_FLAG_TC5);
        /* �������� DMA */
        DMA_Cmd (USART_RX_DMA_CHANNEL,DISABLE);                  // ���� DMA ��EF 01 00 00 00 00 00 07
        USART_RX_DMA_CHANNEL->CNDTR = SENDBUFF_SIZE;
        
        // ʹ�� DMA
        DMA_Cmd (USART_RX_DMA_CHANNEL,ENABLE);
        
        /* USART1 �� DMA ����Rx���� */
        USART_DMACmd(DEBUG_USARTx, USART_DMAReq_Rx, ENABLE);    // ������һ�δ��ڽ���
      
        USART_ClearFlag(DEBUG_USARTx, USART_IT_IDLE);     // ��������ж�
        // ��������жϱ�־λ
        USART_ReceiveData(DEBUG_USARTx);    // �� DR�� ������ջ������ǿձ�־
    }
    
    OSIntExit();    // �˳��ж�
}






