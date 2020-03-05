//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK ս��V3 STM32������
//ATK-AS608ָ��ʶ��ģ����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/3/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights dataerved									  
////////////////////////////////////////////////////////////////////////////////// 	
#include <string.h>	
#include "bsp_usart_dma.h"
#include "drv_as608.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "includes.h"

u32 AS608Addr = 0XFFFFFFFF; //Ĭ��

 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* �����ж�Դ������1 */
  NVIC_InitStructure.NVIC_IRQChannel = PS_Sta_EXTI_IRQ;
  /* ������ռ���ȼ� */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* ���������ȼ� */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* ʹ���ж�ͨ�� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void PS_Sta_IRQDisable(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
/* �ر��ж� */
  NVIC_InitStructure.NVIC_IRQChannel = PS_Sta_EXTI_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void PS_Sta_IRQEnable(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
/* �ر��ж� */
  NVIC_InitStructure.NVIC_IRQChannel = PS_Sta_EXTI_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

//��ʼ��PA1Ϊ��������		    
//��������Ӧ״̬(������Ӧʱ����ߵ�ƽ�ź�)
void PS_StaGPIO_Init(void)
{   
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  
    /* ʹ�ܶ˿�ʱ�� */
	RCC_APB2PeriphClockCmd(PS_Sta_GPIO_CLK,ENABLE);
  
	GPIO_InitStructure.GPIO_Pin=PS_Sta_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_Init(PS_Sta_PORT,&GPIO_InitStructure);
    
  	/* ѡ��EXTI���ź�Դ */
  GPIO_EXTILineConfig(PS_Sta_EXTI_PORTSOURCE, PS_Sta_EXTI_PINSOURCE); 
  EXTI_InitStructure.EXTI_Line = PS_Sta_EXTI_LINE;
	
	/* EXTIΪ�ж�ģʽ */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	/* �������ж� */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  /* ʹ���ж� */	
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  NVIC_Configuration();

  USART_Config();    // ���ڳ�ʼ��
//  USARTx_DMA_Config();
  USARRx_DMA_Config();
//  if (PS_HandShake(0))
//  {
//    printf("ָ��ģ���ʼ��ʧ��\n");
//  }
//  else
//  {
//    printf("ָ��ģ���ʼ���ɹ�\n");
//  }
}

//���ڷ���һ���ֽ�
static void MYUSART_SendData(u8 data)
{
	while((USART2->SR&0X40)==0); 
	USART2->DR = data;
}
//���Ͱ�ͷ
static void SendHead(void)
{
  MYUSART_SendData(0xEF);
	MYUSART_SendData(0x01);
}
//���͵�ַ
static void SendAddr(void)
{
	MYUSART_SendData(AS608Addr>>24);
	MYUSART_SendData(AS608Addr>>16);
	MYUSART_SendData(AS608Addr>>8);
	MYUSART_SendData(AS608Addr);
}
//���Ͱ���ʶ,
static void SendFlag(u8 flag)
{
	MYUSART_SendData(flag);
}
//���Ͱ�����
static void SendLength(int length)
{
	MYUSART_SendData(length>>8);
	MYUSART_SendData(length);
}
//����ָ����
static void Sendcmd(u8 cmd)
{
	MYUSART_SendData(cmd);
}
//����У���
static void SendCheck(u16 check)
{
	MYUSART_SendData(check>>8);
	MYUSART_SendData(check);
}
//�ж��жϽ��յ�������û��Ӧ���
//waittimeΪ�ȴ��жϽ������ݵ�ʱ�䣨��λ1ms��
//����ֵ�����ݰ��׵�ַ
//static u8 *JudgeStr(u16 waittime)
//{
//	char *data;
//	u8 str[8];
//	str[0]=0xef;str[1]=0x01;str[2]=AS608Addr>>24;
//	str[3]=AS608Addr>>16;str[4]=AS608Addr>>8;
//	str[5]=AS608Addr;str[6]=0x07;str[7]='\0';
//	USART2_RX_STA=0;
//	while(--waittime)
//	{
//		
//		if(USART2_RX_STA&0X8000)//���յ�һ������
//		{
//			USART2_RX_STA=0;
//			data=strstr((const char*)USART2_RX_BUF,(const char*)str);
//			if(data)
//				return (u8*)data;	
//		}
//	}
//	return 0;
//}

void AS608_Send_Data(u8 Flag, u8 len, u8 cmd, u8 *data)
{
    u8 xC = 0;    // ѭ������
    u16 TempNum = 0;
    static u8 Temp[50];
    
    Temp[0] = 0xEF;
    Temp[1] = 0x01;
    Temp[2] = (u8)(AS608Addr>>24);
    Temp[3] = (u8)(AS608Addr>>16);
    Temp[4] = (u8)(AS608Addr>>8);
    Temp[5] = (u8)AS608Addr;
    Temp[6] = Flag;
    Temp[7] = len;
    Temp[8] = cmd;
    
    for(xC=0; xC<len-3; xC++)    // ����Ҫ���͵�����
    {
        Temp[9+xC] = data[xC];
    }

    for(xC=0; xC<len; xC++)     // ����У���
    {
        TempNum += Temp[xC+6];
    }
    
    Temp[6+len] = (TempNum >> 8) & 0xFF;            // ����У��͵ĵ� 8 λ
    Temp[7+len] = TempNum & 0xFF;     // ����У��͵ĸ� 8 λ
    
//    USARTx_DMA_SendData(Temp, len+8);       // ��������
}

//¼��ͼ�� PS_GetImage
//����:̽����ָ��̽�⵽��¼��ָ��ͼ�����ImageBuffer�� 
//ģ�鷵��ȷ����
u8 PS_GetImage(void)
{
    u8  ensure;
	u8  *data;
    OS_ERR      err;
	OS_MSG_SIZE msg_size;
  u16 temp;
   CPU_SR_ALLOC();

  OS_CRITICAL_ENTER();
  
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x01);
    temp =  0x01+0x03+0x01;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
//    AS608_Send_Data(0x01, 0x03, 0x01, 0);

//	data=JudgeStr(2000);
    /* ������Ϣ���� queue ����Ϣ */
    data = OSQPend ((OS_Q         *)&queue,                //��Ϣ����ָ��
                    (OS_TICK       )999,                     //�ȴ�ʱ��Ϊ����
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //���û�л�ȡ���ź����͵ȴ�
                    (OS_MSG_SIZE  *)&msg_size,             //��ȡ��Ϣ���ֽڴ�С
                    (CPU_TS       *)0,                     //��ȡ������ʱ��ʱ���
                    (OS_ERR       *)&err);                 //���ش���
    
    if ( err == OS_ERR_NONE )                              //������ճɹ�
    {
        if(data)
            ensure=data[9];
        else
            ensure=0xff;
    }
    else
        ensure=0xff;
	
	return ensure;
}

//�������� PS_GenChar
//����:��ImageBuffer�е�ԭʼͼ������ָ�������ļ�����CharBuffer1��CharBuffer2			 
//����:BufferID --> charBuffer1:0x01	charBuffer1:0x02												
//ģ�鷵��ȷ����
u8 PS_GenChar(u8 BufferID)
{
	u8  *data;
    OS_ERR      err;
	OS_MSG_SIZE msg_size;
  u16 temp;
  CPU_SR_ALLOC();
    
  OS_CRITICAL_ENTER();
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x04);
	Sendcmd(0x02);
	MYUSART_SendData(BufferID);
	temp = 0x01+0x04+0x02+BufferID;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
  
//	data=JudgeStr(2000);
    
//    AS608_Send_Data(0x01, 0x04, 0x02, &BufferID);
    
    /* ������Ϣ���� queue ����Ϣ */
    data = OSQPend ((OS_Q         *)&queue,                //��Ϣ����ָ��
                    (OS_TICK       )999,                     //�ȴ�ʱ��Ϊ����
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //���û�л�ȡ���ź����͵ȴ�
                    (OS_MSG_SIZE  *)&msg_size,             //��ȡ��Ϣ���ֽڴ�С
                    (CPU_TS       *)0,                     //��ȡ������ʱ��ʱ���
                    (OS_ERR       *)&err);                 //���ش���
    
    if ( err == OS_ERR_NONE )                              //������ճɹ�
    {
        if(data)
            return data[9];    //  OK
        else
            return 0xff;     // ����
    }
    else
        return 0xff;
}

//��ȷ�ȶ���öָ������ PS_Match
//����:��ȷ�ȶ�CharBuffer1 ��CharBuffer2 �е������ļ� 
//ģ�鷵��ȷ����
u8 PS_Match(void)
{
	u8  *data;
    OS_ERR      err;
	OS_MSG_SIZE msg_size;
  u16 temp;
CPU_SR_ALLOC();
  
  OS_CRITICAL_ENTER();
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x03);
	temp = 0x01+0x03+0x03;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
//	data=JudgeStr(2000);
    
//    AS608_Send_Data(0x01, 0x03, 0x03, 0);
    
    /* ������Ϣ���� queue ����Ϣ */
    data = OSQPend ((OS_Q         *)&queue,                //��Ϣ����ָ��
                    (OS_TICK       )0,                     //�ȴ�ʱ��Ϊ����
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //���û�л�ȡ���ź����͵ȴ�
                    (OS_MSG_SIZE  *)&msg_size,             //��ȡ��Ϣ���ֽڴ�С
                    (CPU_TS       *)0,                     //��ȡ������ʱ��ʱ���
                    (OS_ERR       *)&err);                 //���ش���
    
    if ( err == OS_ERR_NONE )                              //������ճɹ�
    {
        if(data)
            return data[9];    //  OK
        else
            return 0xff;     // ����
    }
    else
        return 0xff;
}

//����ָ�� PS_Search
//����:��CharBuffer1��CharBuffer2�е������ļ����������򲿷�ָ�ƿ�.�����������򷵻�ҳ�롣			
//����:  BufferID @ref CharBuffer1	CharBuffer2
//˵��:  ģ�鷵��ȷ���֣�ҳ�루����ָ��ģ�壩
u8 PS_Search(u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p)
{
	u8  *data;
    OS_ERR      err;
	OS_MSG_SIZE msg_size;
  u16 temp;
  CPU_SR_ALLOC();
  
    OS_CRITICAL_ENTER();
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x08);
	Sendcmd(0x04);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(StartPage>>8);
	MYUSART_SendData(StartPage);
	MYUSART_SendData(PageNum>>8);
	MYUSART_SendData(PageNum);
	temp = 0x01+0x08+0x04+BufferID
	+(StartPage>>8)+(u8)StartPage
	+(PageNum>>8)+(u8)PageNum;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
//	data=JudgeStr(2000);

//    TempData[0] = BufferID;
//    TempData[1] = StartPage>>8;
//    TempData[2] = StartPage;
//    TempData[3] = PageNum>>8;
//    TempData[4] = PageNum;
//    
//    AS608_Send_Data(0x01, 0x08, 0x04, TempData);
    
    /* ������Ϣ���� queue ����Ϣ */
    data = OSQPend ((OS_Q         *)&queue,                //��Ϣ����ָ��
                    (OS_TICK       )0,                     //�ȴ�ʱ��Ϊ����
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //���û�л�ȡ���ź����͵ȴ�
                    (OS_MSG_SIZE  *)&msg_size,             //��ȡ��Ϣ���ֽڴ�С
                    (CPU_TS       *)0,                     //��ȡ������ʱ��ʱ���
                    (OS_ERR       *)&err);                 //���ش���
                    
    if ( err == OS_ERR_NONE )                              //������ճɹ�
    {
        if(data)
        {
		    p->pageID   =(data[10]<<8)+data[11];
		    p->mathscore=(data[12]<<8)+data[13];	
            return data[9];    //  OK
        }
        else
            return 0xff;     // ����
    }
    else
        return 0xff;
//	if(data)
//	{
//		ensure = data[9];
//		p->pageID   =(data[10]<<8)+data[11];
//		p->mathscore=(data[12]<<8)+data[13];	
//	}
//	else
//		ensure = 0xff;
//	return ensure;	
}

//�ϲ�����������ģ�壩PS_RegModel
//����:��CharBuffer1��CharBuffer2�е������ļ��ϲ����� ģ��,�������CharBuffer1��CharBuffer2	
//˵��:  ģ�鷵��ȷ����
u8 PS_RegModel(void)
{
    u8  ensure;
	u8  *data;
    OS_ERR      err;
	OS_MSG_SIZE msg_size;
  u16 temp;
  CPU_SR_ALLOC();
    
  OS_CRITICAL_ENTER();
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x05);
	temp = 0x01+0x03+0x05;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
//	data=JudgeStr(2000);
    
//    AS608_Send_Data(0x01, 0x03, 0x05, 0);
    
    /* ������Ϣ���� queue ����Ϣ */
    data = OSQPend ((OS_Q         *)&queue,                //��Ϣ����ָ��
                    (OS_TICK       )0,                     //�ȴ�ʱ��Ϊ����
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //���û�л�ȡ���ź����͵ȴ�
                    (OS_MSG_SIZE  *)&msg_size,             //��ȡ��Ϣ���ֽڴ�С
                    (CPU_TS       *)0,                     //��ȡ������ʱ��ʱ���
                    (OS_ERR       *)&err);                 //���ش���
    
    if ( err == OS_ERR_NONE )                              //������ճɹ�
    {
        if(data)
            ensure=data[9];
        else
            ensure=0xff;
    }
    else
        ensure=0xff;
	
	return ensure;	
}

//����ģ�� PS_StoreChar
//����:�� CharBuffer1 �� CharBuffer2 �е�ģ���ļ��浽 PageID ��flash���ݿ�λ�á�			
//����:  BufferID @ref charBuffer1:0x01	charBuffer1:0x02
//       PageID��ָ�ƿ�λ�úţ�
//˵��:  ģ�鷵��ȷ����
u8 PS_StoreChar(u8 BufferID,u16 PageID)
{
    u8  ensure;
	u8  *data;
    OS_ERR      err;
	OS_MSG_SIZE msg_size;
    u16 temp;
  CPU_SR_ALLOC();
  
  OS_CRITICAL_ENTER();
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x06);
	Sendcmd(0x06);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(PageID>>8);
	MYUSART_SendData(PageID);
	temp = 0x01+0x06+0x06+BufferID
	+(PageID>>8)+(u8)PageID;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
  
//    TempData[0] = BufferID;
//    TempData[1] = PageID >> 8;
//    TempData[2] = PageID;
    
//    AS608_Send_Data(0x01, 0x06, 0x06, TempData);

//	data=JudgeStr(2000);
    /* ������Ϣ���� queue ����Ϣ */
    data = OSQPend ((OS_Q         *)&queue,                //��Ϣ����ָ��
                    (OS_TICK       )0,                     //�ȴ�ʱ��Ϊ����
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //���û�л�ȡ���ź����͵ȴ�
                    (OS_MSG_SIZE  *)&msg_size,             //��ȡ��Ϣ���ֽڴ�С
                    (CPU_TS       *)0,                     //��ȡ������ʱ��ʱ���
                    (OS_ERR       *)&err);                 //���ش���
    
    if ( err == OS_ERR_NONE )                              //������ճɹ�
    {
        if(data)
            ensure=data[9];
        else
            ensure=0xff;
    }
    else
        ensure=0xff;
	
	return ensure;
}

//ɾ��ģ�� PS_DeletChar
//����:  ɾ��flash���ݿ���ָ��ID�ſ�ʼ��N��ָ��ģ��
//����:  PageID(ָ�ƿ�ģ���)��Nɾ����ģ�������
//˵��:  ģ�鷵��ȷ����
u8 PS_DeletChar(u16 PageID,u16 N)
{
    u8  ensure;
	u8  *data;
    OS_ERR      err;
	OS_MSG_SIZE msg_size;
   u16 temp;
  CPU_SR_ALLOC();
  
  OS_CRITICAL_ENTER();
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x07);
	Sendcmd(0x0C);
	MYUSART_SendData(PageID>>8);
	MYUSART_SendData(PageID);
	MYUSART_SendData(N>>8);
	MYUSART_SendData(N);
	temp = 0x01+0x07+0x0C
	+(PageID>>8)+(u8)PageID
	+(N>>8)+(u8)N;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
//	data=JudgeStr(2000);

//    TempData[0] = PageID>>8;
//    TempData[1] = PageID;
//    TempData[2] = N>>8;
//    TempData[3] = N;
//    
//    AS608_Send_Data(0x01, 0x07, 0x0C, TempData);

//	data=JudgeStr(2000);
    /* ������Ϣ���� queue ����Ϣ */
    data = OSQPend ((OS_Q         *)&queue,                //��Ϣ����ָ��
                    (OS_TICK       )0,                     //�ȴ�ʱ��Ϊ����
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //���û�л�ȡ���ź����͵ȴ�
                    (OS_MSG_SIZE  *)&msg_size,             //��ȡ��Ϣ���ֽڴ�С
                    (CPU_TS       *)0,                     //��ȡ������ʱ��ʱ���
                    (OS_ERR       *)&err);                 //���ش���
    
    if ( err == OS_ERR_NONE )                              //������ճɹ�
    {
        if(data)
            ensure=data[9];
        else
            ensure=0xff;
    }
    else
        ensure=0xff;
	
	return ensure;
}

//���ָ�ƿ� PS_Empty
//����:  ɾ��flash���ݿ�������ָ��ģ��
//����:  ��
//˵��:  ģ�鷵��ȷ����
u8 PS_Empty(void)
{
    u8  ensure;
	u8  *data;
    OS_ERR      err;
	OS_MSG_SIZE msg_size;
  u16 temp;
  CPU_SR_ALLOC();
  
  OS_CRITICAL_ENTER();
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x0D);
	temp = 0x01+0x03+0x0D;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
//	data=JudgeStr(2000);
    
//    AS608_Send_Data(0x01, 0x03, 0x0D, 0);

//	data=JudgeStr(2000);
    /* ������Ϣ���� queue ����Ϣ */
    data = OSQPend ((OS_Q         *)&queue,                //��Ϣ����ָ��
                    (OS_TICK       )0,                     //�ȴ�ʱ��Ϊ����
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //���û�л�ȡ���ź����͵ȴ�
                    (OS_MSG_SIZE  *)&msg_size,             //��ȡ��Ϣ���ֽڴ�С
                    (CPU_TS       *)0,                     //��ȡ������ʱ��ʱ���
                    (OS_ERR       *)&err);                 //���ش���
    
    if ( err == OS_ERR_NONE )                              //������ճɹ�
    {
        if(data)
            ensure=data[9];
        else
            ensure=0xff;
    }
    else
        ensure=0xff;
	
	return ensure;
}

//дϵͳ�Ĵ��� PS_WriteReg
//����:  дģ��Ĵ���
//����:  �Ĵ������RegNum:4\5\6
//˵��:  ģ�鷵��ȷ����
u8 PS_WriteReg(u8 RegNum,u8 DATA)
{
    u8  ensure;
	u8  *data;
    OS_ERR      err;
	OS_MSG_SIZE msg_size;
  u16 temp;
  CPU_SR_ALLOC();
  
  OS_CRITICAL_ENTER();
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x05);
	Sendcmd(0x0E);
	MYUSART_SendData(RegNum);
	MYUSART_SendData(DATA);
	temp = RegNum+DATA+0x01+0x05+0x0E;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
//	data=JudgeStr(2000);
//    TempData[0] = RegNum;
//    TempData[1] = DATA;
//    
//    AS608_Send_Data(0x01, 0x05, 0x0E, TempData);

    /* ������Ϣ���� queue ����Ϣ */
    data = OSQPend ((OS_Q         *)&queue,                //��Ϣ����ָ��
                    (OS_TICK       )0,                     //�ȴ�ʱ��Ϊ����
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //���û�л�ȡ���ź����͵ȴ�
                    (OS_MSG_SIZE  *)&msg_size,             //��ȡ��Ϣ���ֽڴ�С
                    (CPU_TS       *)0,                     //��ȡ������ʱ��ʱ���
                    (OS_ERR       *)&err);                 //���ش���
    
    if ( err == OS_ERR_NONE )                              //������ճɹ�
    {
        if(data)
            ensure=data[9];
        else
            ensure=0xff;
    }
    else
        ensure=0xff;
	
	return ensure;
    
//	if(data)
//		ensure=data[9];
//	else
//		ensure=0xff;
//	if(ensure==0)
//		printf("\r\n���ò����ɹ���");
//	else
//		printf("\r\n%s",EnsureMessage(ensure));
//	return ensure;
}

//��ϵͳ�������� PS_ReadSysPara
//����:  ��ȡģ��Ļ��������������ʣ�����С��)
//����:  ��
//˵��:  ģ�鷵��ȷ���� + ����������16bytes��
u8 PS_ReadSysPara(SysPara *p)
{
    u8  ensure;
	u8  *data;
    OS_ERR      err;
	OS_MSG_SIZE msg_size;
  u16 temp;
  CPU_SR_ALLOC();
  
  OS_CRITICAL_ENTER();
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x0F);
	temp = 0x01+0x03+0x0F;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
//	data=JudgeStr(1000);
    
//    AS608_Send_Data(0x01, 0x03, 0x0F, 0);

    /* ������Ϣ���� queue ����Ϣ */
    data = OSQPend ((OS_Q         *)&queue,                //��Ϣ����ָ��
                    (OS_TICK       )0,                     //�ȴ�ʱ��Ϊ����
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //���û�л�ȡ���ź����͵ȴ�
                    (OS_MSG_SIZE  *)&msg_size,             //��ȡ��Ϣ���ֽڴ�С
                    (CPU_TS       *)0,                     //��ȡ������ʱ��ʱ���
                    (OS_ERR       *)&err);                 //���ش���
    
    if ( err == OS_ERR_NONE )                              //������ճɹ�
    {
        if(data)
        {
            ensure = data[9];
            p->PS_max = (data[14]<<8)+data[15];
            p->PS_level = data[17];
            p->PS_addr=(data[18]<<24)+(data[19]<<16)+(data[20]<<8)+data[21];
            p->PS_size = data[23];
            p->PS_N = data[25];
        }
        else
            ensure=0xff;
    }
    else
        ensure=0xff;
	
	return ensure;
    
//	if(data)
//	{
//		ensure = data[9];
//		p->PS_max = (data[14]<<8)+data[15];
//		p->PS_level = data[17];
//		p->PS_addr=(data[18]<<24)+(data[19]<<16)+(data[20]<<8)+data[21];
//		p->PS_size = data[23];
//		p->PS_N = data[25];
//	}		
//	else
//		ensure=0xff;
//	if(ensure==0x00)
//	{
//		printf("\r\nģ�����ָ������=%d",p->PS_max);
//		printf("\r\n�Աȵȼ�=%d",p->PS_level);
//		printf("\r\n��ַ=%x",p->PS_addr);
//		printf("\r\n������=%d",p->PS_N*9600);
//	}
//	else 
//			printf("\r\n%s",EnsureMessage(ensure));
//	return ensure;
}

//����ģ���ַ PS_SetAddr
//����:  ����ģ���ַ
//����:  PS_addr
//˵��:  ģ�鷵��ȷ����
u8 PS_SetAddr(u32 PS_addr)
{
    u8  ensure;
	u8  *data;
    OS_ERR      err;
	OS_MSG_SIZE msg_size;
  u16 temp;
CPU_SR_ALLOC();
    
  OS_CRITICAL_ENTER();
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x07);
	Sendcmd(0x15);
	MYUSART_SendData(PS_addr>>24);
	MYUSART_SendData(PS_addr>>16);
	MYUSART_SendData(PS_addr>>8);
	MYUSART_SendData(PS_addr);
	temp = 0x01+0x07+0x15
	+(u8)(PS_addr>>24)+(u8)(PS_addr>>16)
	+(u8)(PS_addr>>8) +(u8)PS_addr;				
	SendCheck(temp);
	AS608Addr=PS_addr;//������ָ�������ַ
  OS_CRITICAL_EXIT();
  
//  data=JudgeStr(2000);
//    TempData[0] = PS_addr>>24;
//    TempData[1] = PS_addr>>16;
//    TempData[2] = PS_addr>>8;
//    TempData[3] = PS_addr;
//    
//    AS608_Send_Data(0x01, 0x07, 0x15, TempData);

    /* ������Ϣ���� queue ����Ϣ */
    data = OSQPend ((OS_Q         *)&queue,                //��Ϣ����ָ��
                    (OS_TICK       )0,                     //�ȴ�ʱ��Ϊ����
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //���û�л�ȡ���ź����͵ȴ�
                    (OS_MSG_SIZE  *)&msg_size,             //��ȡ��Ϣ���ֽڴ�С
                    (CPU_TS       *)0,                     //��ȡ������ʱ��ʱ���
                    (OS_ERR       *)&err);                 //���ش���
    
    if ( err == OS_ERR_NONE )                              //������ճɹ�
    {
        if(data)
        {
            ensure=data[9];
            AS608Addr = PS_addr;
        }
        else
            ensure=0xff;
    }
    else
        ensure=0xff;
	
	return ensure;
    
//	if(data)
//		ensure=data[9];
//	else
//		ensure=0xff;	
//		AS608Addr = PS_addr;
//	if(ensure==0x00)
//		printf("\r\n���õ�ַ�ɹ���");
//	else
//		printf("\r\n%s",EnsureMessage(ensure));
//	return ensure;
}

//���ܣ� ģ���ڲ�Ϊ�û�������256bytes��FLASH�ռ����ڴ��û����±�,
//	�ü��±��߼��ϱ��ֳ� 16 ��ҳ��
//����:  NotePageNum(0~15),Byte32(Ҫд�����ݣ�32���ֽ�)
//˵��:  ģ�鷵��ȷ����
u8 PS_WriteNotepad(u8 NotePageNum,u8 *Byte32)
{
    u8  ensure,i;
	u8  *data;
    OS_ERR      err;
	OS_MSG_SIZE msg_size;
  u16 temp;
  CPU_SR_ALLOC();
  
  OS_CRITICAL_ENTER();
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(36);
	Sendcmd(0x18);
	MYUSART_SendData(NotePageNum);
	for(i=0;i<32;i++)
	 {
		 MYUSART_SendData(Byte32[i]);
		 temp += Byte32[i];
	 }
  temp =0x01+36+0x18+NotePageNum+temp;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
//  data=JudgeStr(2000);
    
//    AS608_Send_Data(0x01, 36, 0x18, Byte32);

    /* ������Ϣ���� queue ����Ϣ */
    data = OSQPend ((OS_Q         *)&queue,                //��Ϣ����ָ��
                    (OS_TICK       )0,                     //�ȴ�ʱ��Ϊ����
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //���û�л�ȡ���ź����͵ȴ�
                    (OS_MSG_SIZE  *)&msg_size,             //��ȡ��Ϣ���ֽڴ�С
                    (CPU_TS       *)0,                     //��ȡ������ʱ��ʱ���
                    (OS_ERR       *)&err);                 //���ش���
    
    if ( err == OS_ERR_NONE )                              //������ճɹ�
    {
        if(data)
            ensure=data[9];
        else
            ensure=0xff;
    }
    else
        ensure=0xff;
	
	return ensure;
}
//������PS_ReadNotepad
//���ܣ�  ��ȡFLASH�û�����128bytes����
//����:  NotePageNum(0~15)
//˵��:  ģ�鷵��ȷ����+�û���Ϣ
u8 PS_ReadNotepad(u8 NotePageNum,u8 *Byte32)
{
    u8  ensure, i;
	u8  *data;
    OS_ERR      err;
	OS_MSG_SIZE msg_size;
  u16 temp;
  CPU_SR_ALLOC();
    
  OS_CRITICAL_ENTER();
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x04);
	Sendcmd(0x19);
	MYUSART_SendData(NotePageNum);
	temp = 0x01+0x04+0x19+NotePageNum;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
//  data=JudgeStr(2000);
    
//    AS608_Send_Data(0x01, 0x04, 0x19, &NotePageNum);

    /* ������Ϣ���� queue ����Ϣ */
    data = OSQPend ((OS_Q         *)&queue,                //��Ϣ����ָ��
                    (OS_TICK       )0,                     //�ȴ�ʱ��Ϊ����
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //���û�л�ȡ���ź����͵ȴ�
                    (OS_MSG_SIZE  *)&msg_size,             //��ȡ��Ϣ���ֽڴ�С
                    (CPU_TS       *)0,                     //��ȡ������ʱ��ʱ���
                    (OS_ERR       *)&err);                 //���ش���
    
    if ( err == OS_ERR_NONE )                              //������ճɹ�
    {
        if(data)
        {
            ensure=data[9];
            for(i=0;i<32;i++)
            {
                Byte32[i]=data[10+i];
            }
        }
        else
            ensure=0xff;
    }
    else
        ensure=0xff;
	
	return ensure;
    
//	if(data)
//	{
//		ensure=data[9];
//		for(i=0;i<32;i++)
//		{
//			Byte32[i]=data[10+i];
//		}
//	}
//	else
//		ensure=0xff;
//	return ensure;
}
//��������PS_HighSpeedSearch
//���ܣ��� CharBuffer1��CharBuffer2�е������ļ��������������򲿷�ָ�ƿ⡣
//		  �����������򷵻�ҳ��,��ָ����ڵ�ȷ������ָ�ƿ��� ���ҵ�¼ʱ����
//		  �ܺõ�ָ�ƣ���ܿ�������������
//����:  BufferID�� StartPage(��ʼҳ)��PageNum��ҳ����
//˵��:  ģ�鷵��ȷ����+ҳ�루����ָ��ģ�壩
u8 PS_HighSpeedSearch(u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p)
{
    u8  ensure;
	u8  *data;
    OS_ERR      err;
	OS_MSG_SIZE msg_size;
  u16 temp;
  CPU_SR_ALLOC();
  
  OS_CRITICAL_ENTER();
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x08);
	Sendcmd(0x1b);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(StartPage>>8);
	MYUSART_SendData(StartPage);
	MYUSART_SendData(PageNum>>8);
	MYUSART_SendData(PageNum);
	temp = 0x01+0x08+0x1b+BufferID
	+(StartPage>>8)+(u8)StartPage
	+(PageNum>>8)+(u8)PageNum;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
//	data=JudgeStr(2000);
    
//    TempData[0] = BufferID;
//    TempData[1] = StartPage>>8;
//    TempData[2] = StartPage;
//    TempData[3] = PageNum>>8;
//    TempData[4] = PageNum;
//    
//    AS608_Send_Data(0x01, 0x08, 0x1b, TempData);

    /* ������Ϣ���� queue ����Ϣ */
    data = OSQPend ((OS_Q         *)&queue,                //��Ϣ����ָ��
                    (OS_TICK       )999,                     //�ȴ�ʱ��Ϊ����
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //���û�л�ȡ���ź����͵ȴ�
                    (OS_MSG_SIZE  *)&msg_size,             //��ȡ��Ϣ���ֽڴ�С
                    (CPU_TS       *)0,                     //��ȡ������ʱ��ʱ���
                    (OS_ERR       *)&err);                 //���ش���
    
    if ( err == OS_ERR_NONE )                              //������ճɹ�
    {
        if(data)
        {
            ensure=data[9];
            p->pageID 	=(data[10]<<8) +data[11];
            p->mathscore=(data[12]<<8) +data[13];
        }
        else
            ensure=0xff;
    }
    else
        ensure=0xff;
	
	return ensure;
    
// 	if(data)
//	{
//		ensure=data[9];
//		p->pageID 	=(data[10]<<8) +data[11];
//		p->mathscore=(data[12]<<8) +data[13];
//	}
//	else
//		ensure=0xff;
//	return ensure;
}

//����Чģ����� PS_ValidTempleteNum
//���ܣ�����Чģ�����
//����: ��
//˵��: ģ�鷵��ȷ����+��Чģ�����ValidN
u8 PS_ValidTempleteNum(u16 *ValidN)
{
    u8  ensure;
	u8  *data;
    OS_ERR      err;
	OS_MSG_SIZE msg_size;
  u16 temp;
  CPU_SR_ALLOC();
  
  OS_CRITICAL_ENTER();
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x1d);
	temp = 0x01+0x03+0x1d;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
//  data=JudgeStr(2000);
    
//    AS608_Send_Data(0x01, 0x03, 0x1d, 0);

    /* ������Ϣ���� queue ����Ϣ */
    data = OSQPend ((OS_Q         *)&queue,                //��Ϣ����ָ��
                    (OS_TICK       )0,                     //�ȴ�ʱ��Ϊ����
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //���û�л�ȡ���ź����͵ȴ�
                    (OS_MSG_SIZE  *)&msg_size,             //��ȡ��Ϣ���ֽڴ�С
                    (CPU_TS       *)0,                     //��ȡ������ʱ��ʱ���
                    (OS_ERR       *)&err);                 //���ش���
    
    if ( err == OS_ERR_NONE )                              //������ճɹ�
    {
        if(data)
        {
            ensure=data[9];
            *ValidN = (data[10]<<8) +data[11];
        }
        else
            ensure=0xff;
    }
    else
        ensure=0xff;
	
	return ensure;
    
//	if(data)
//	{
//		ensure=data[9];
//		*ValidN = (data[10]<<8) +data[11];
//	}		
//	else
//		ensure=0xff;
//	
//	if(ensure==0x00)
//	{
//		printf("\r\n��Чָ�Ƹ���=%d",(data[10]<<8)+data[11]);
//	}
//	else
//		printf("\r\n%s",EnsureMessage(ensure));
//	return ensure;
}

//��AS608���� PS_HandShake
//����: PS_Addr��ַָ��
//˵��: ģ�鷵�µ�ַ����ȷ��ַ��	
u8 PS_HandShake(u32 *PS_Addr)
{	
    u8  ensure;
	u8  *data;
    OS_ERR      err;
	OS_MSG_SIZE msg_size;
  CPU_SR_ALLOC();
    
  OS_CRITICAL_ENTER();
  SendHead();
	SendAddr();
	MYUSART_SendData(0X01);
	MYUSART_SendData(0X00);
	MYUSART_SendData(0X00);
  OS_CRITICAL_EXIT();
//    TempData[0] = 0xEF;
//    TempData[1] = 0x01;
//    TempData[2] = AS608Addr>>24;
//    TempData[3] = AS608Addr>>16;
//    TempData[4] = AS608Addr>>8;
//    TempData[5] = AS608Addr;
//    TempData[6] = 0X01;
//    TempData[7] = 0X00;
//    TempData[8] = 0X00;
    
  
//    USARTx_DMA_SendData(TempData, 9);
//    printf("�������ݸ�ָ��\n");
    /* ������Ϣ���� queue ����Ϣ */
    data = OSQPend ((OS_Q         *)&queue,                    // ��Ϣ����ָ��
                    (OS_TICK       )0,                      // �ȴ� 1000ms
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  // ���û�л�ȡ���ź����͵ȴ�
                    (OS_MSG_SIZE  *)&msg_size,                 // ��ȡ��Ϣ���ֽڴ�С
                    (CPU_TS       *)0,                         // ��ȡ������ʱ��ʱ���
                    (OS_ERR       *)&err);                     // ���ش���
//    printf("%d  %d   %d\n",data[0],data[1],data[6]);
    if ( err == OS_ERR_NONE )                                  //������ճɹ�
    {
        if(	data[0]==0XEF && data[1]==0X01 && data[6]==0X07) //�ж��ǲ���ģ�鷵�ص�Ӧ���	
        {
            //*PS_Addr=(data[2]<<24) + (data[3]<<16) + (data[4]<<8) + (data[5]);
            return 0;
        }
    }
    else
        ensure=1;
	
	return ensure;
    

//	Delay_ms(200);
//	if(USART2_RX_STA&0X8000)//���յ�����
//	{		
//		if(//�ж��ǲ���ģ�鷵�ص�Ӧ���				
//					USART2_RX_BUF[0]==0XEF
//				&&USART2_RX_BUF[1]==0X01
//				&&USART2_RX_BUF[6]==0X07
//			)
//			{
//				*PS_Addr=(USART2_RX_BUF[2]<<24) + (USART2_RX_BUF[3]<<16)
//								+(USART2_RX_BUF[4]<<8) + (USART2_RX_BUF[5]);
//				USART2_RX_STA=0;
//				return 0;
//			}
//		USART2_RX_STA=0;					
//	}
//	return 1;		
}

//ģ��Ӧ���ȷ������Ϣ����
//���ܣ�����ȷ���������Ϣ������Ϣ
//����: ensure
const char *EnsureMessage(u8 ensure) 
{
	const char *p;
	switch(ensure)
	{
		case  0x00:
			p="00H";break;		
		case  0x01:
			p="01H";break;
		case  0x02:
			p="02H";break;
		case  0x03:
			p="03H";break;
		case  0x04:
			p="04H";break;
		case  0x05:
			p="05H";break;
		case  0x06:
			p="06H";break;
		case  0x07:
			p="07H";break;
		case  0x08:
			p="08H";break;
		case  0x09:
			p="09H";break;
		case  0x0a:
			p="0AH";break;
		case  0x0b:
			p="0BH";
		case  0x10:
			p="10H";break;
		case  0x11:
			p="11H";break;	
		case  0x15:
			p="15H";break;
		case  0x18:
			p="18H";break;
		case  0x19:
			p="19H";break;
		case  0x1a:
			p="1AH";break;
		case  0x1b:
			p="1AH";break;
		case  0x1c:
			p="1CH";break;
		case  0x1f:
			p="1FH";break;
		case  0x20:
			p="20H";break;
		default :
			p="Error";break;
	}
 return p;	
}





