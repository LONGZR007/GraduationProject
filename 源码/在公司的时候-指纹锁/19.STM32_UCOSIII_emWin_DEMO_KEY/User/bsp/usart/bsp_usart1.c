/**
  ******************************************************************************
  * @file    bsp_usart1.c
  * @author  Ұ��
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ����c��printf������usart�˿�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "..\User\bsp\usart\bsp_usart1.h"
#include "includes.h"

uint8_t Usart1RxBuff[RXBUFF_SIZE];
void USAR1Rx_DMA_Config(void);

 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* ����USARTΪ�ж�Դ */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQ;
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
  * @brief  USART1 GPIO ����,����ģʽ���á�115200 8-N-1
  * @param  ��
  * @retval ��
  */
void USART1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	USART_TypeDef *uart;

	/* ��1������GPIO��USART������ʱ�� */
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

	/* ��2������USART Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = macUSART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(macUSART_TX_PORT, &GPIO_InitStructure);

	/* ��3������USART Rx��GPIO����Ϊ��������ģʽ
		����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
		���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò���
	*/
	GPIO_InitStructure.GPIO_Pin = macUSART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(macUSART_RX_PORT, &GPIO_InitStructure);
	/*  ��2���Ѿ����ˣ�����ⲽ���Բ���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	*/
	
	/* ��4�������ô���Ӳ������ */
	uart = macUSARTx;	
	USART_InitStructure.USART_BaudRate = macUSART_BAUD_RATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(uart, &USART_InitStructure);
	USART_ITConfig(uart, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	/* 
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������
	*/
  // �����ж����ȼ�����
	NVIC_Configuration();
	
	// ʹ�ܴ��ڿ����ж�
	USART_ITConfig(macUSARTx, USART_IT_IDLE, ENABLE);	
	USART_Cmd(uart, ENABLE);		/* ʹ�ܴ��� */ 

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* �巢����Ǳ�־��Transmission Complete flag */
  
  USAR1Rx_DMA_Config();
  /* �ر� 8266 ���Թ��� */
  
  
}

void USAR1Rx_DMA_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;
	
    // ����DMAʱ��
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // ����DMAԴ��ַ���������ݼĴ�����ַ*/
    DMA_InitStructure.DMA_PeripheralBaseAddr = USART1_DR_ADDRESS;
    // �ڴ��ַ(Ҫ����ı�����ָ��)
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Usart1RxBuff;
    // ���򣺴����赽�ڴ�
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    // �����С	
    DMA_InitStructure.DMA_BufferSize = RXBUFF_SIZE;
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
    DMA_Init(USART1_RX_DMA_CHANNEL, &DMA_InitStructure);		
    // ʹ��DMA
    DMA_Cmd (USART1_RX_DMA_CHANNEL,ENABLE);
    
    /* USART1 �� DMA����Rx���� */
    USART_DMACmd(macUSARTx, USART_DMAReq_Rx, ENABLE);    // ����һ�δ��ڽ���
}

///�ض���c�⺯��printf��USART1
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ�USART1 */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///�ض���c�⺯��scanf��USART1
int fgetc(FILE *f)
{
		/* �ȴ�����1�������� */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}

u8 USART1_RX_Len = 0;

void USART1_IRQHandler(void)
{
    OSIntEnter();    // �����ж�
    if (USART1_RX_Len == 0)
    {
        if(USART_GetITStatus(macUSARTx, USART_IT_IDLE)!=RESET)    // ���߿����ж�
        {
            USART_DMACmd(macUSARTx, USART_DMAReq_Tx, DISABLE);    // ���� DMA ����
            
            USART1_RX_Len = RXBUFF_SIZE - DMA_GetCurrDataCounter(USART1_RX_DMA_CHANNEL);    // ���㴮�ڽ��յ��ĳ���
            
            /* ��DMA��־λ */
            DMA_ClearFlag(DMA1_FLAG_TC5);
            
          
            
            
    //        // ���ô��ڿ����ж�
    //        USART_ITConfig(macUSARTx, USART_IT_IDLE, DISABLE);
            // ʹ�� DMA
       DMA_Cmd (USART1_RX_DMA_CHANNEL,ENABLE);

    //    /* USART1 �� DMA ����Rx���� */
        USART_DMACmd(macUSARTx, USART_DMAReq_Rx, ENABLE);    // ������һ�δ��ڽ���
            
            // ��������жϱ�־λ
        }
    }
    if(USART_GetITStatus(macUSARTx, USART_IT_IDLE)!=RESET)    // ���߿����ж�
    {
        USART_ClearFlag(macUSARTx, USART_IT_IDLE);     // ��������ж�
        USART_ReceiveData(macUSARTx);    // �� DR�� ������ջ������ǿձ�־
    }
    OSIntExit();    // �˳��ж�
}

void USART1_Start_Rx(void)
{
    USART1_RX_Len = 0;
    USART_DMACmd(macUSARTx, USART_DMAReq_Tx, DISABLE);    // ���� DMA ����
    /* �������� DMA */
    DMA_Cmd (USART1_RX_DMA_CHANNEL,DISABLE);                  // ���� DMA ��EF 01 00 00 00 00 00 07
    USART1_RX_DMA_CHANNEL->CNDTR = RXBUFF_SIZE;
    
    // ʹ�� DMA
    DMA_Cmd (USART1_RX_DMA_CHANNEL,ENABLE);

    /* USART1 �� DMA ����Rx���� */
    USART_DMACmd(macUSARTx, USART_DMAReq_Rx, ENABLE);    // ������һ�δ��ڽ���
}
/*********************************************END OF FILE**********************/
