//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK 战舰V3 STM32开发板
//ATK-AS608指纹识别模块驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/3/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights dataerved									  
////////////////////////////////////////////////////////////////////////////////// 	
#include <string.h>	
#include "bsp_usart_dma.h"
#include "drv_as608.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "includes.h"

u32 AS608Addr = 0XFFFFFFFF; //默认

 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 配置中断源：按键1 */
  NVIC_InitStructure.NVIC_IRQChannel = PS_Sta_EXTI_IRQ;
  /* 配置抢占优先级 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 配置子优先级 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* 使能中断通道 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void PS_Sta_IRQDisable(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
/* 关闭中断 */
  NVIC_InitStructure.NVIC_IRQChannel = PS_Sta_EXTI_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void PS_Sta_IRQEnable(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
/* 关闭中断 */
  NVIC_InitStructure.NVIC_IRQChannel = PS_Sta_EXTI_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

//初始化PA1为下拉输入		    
//读摸出感应状态(触摸感应时输出高电平信号)
void PS_StaGPIO_Init(void)
{   
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  
    /* 使能端口时钟 */
	RCC_APB2PeriphClockCmd(PS_Sta_GPIO_CLK,ENABLE);
  
	GPIO_InitStructure.GPIO_Pin=PS_Sta_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_Init(PS_Sta_PORT,&GPIO_InitStructure);
    
  	/* 选择EXTI的信号源 */
  GPIO_EXTILineConfig(PS_Sta_EXTI_PORTSOURCE, PS_Sta_EXTI_PINSOURCE); 
  EXTI_InitStructure.EXTI_Line = PS_Sta_EXTI_LINE;
	
	/* EXTI为中断模式 */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	/* 上升沿中断 */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  /* 使能中断 */	
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  NVIC_Configuration();

  USART_Config();    // 串口初始化
//  USARTx_DMA_Config();
  USARRx_DMA_Config();
//  if (PS_HandShake(0))
//  {
//    printf("指纹模块初始化失败\n");
//  }
//  else
//  {
//    printf("指纹模块初始化成功\n");
//  }
}

//串口发送一个字节
static void MYUSART_SendData(u8 data)
{
	while((USART2->SR&0X40)==0); 
	USART2->DR = data;
}
//发送包头
static void SendHead(void)
{
  MYUSART_SendData(0xEF);
	MYUSART_SendData(0x01);
}
//发送地址
static void SendAddr(void)
{
	MYUSART_SendData(AS608Addr>>24);
	MYUSART_SendData(AS608Addr>>16);
	MYUSART_SendData(AS608Addr>>8);
	MYUSART_SendData(AS608Addr);
}
//发送包标识,
static void SendFlag(u8 flag)
{
	MYUSART_SendData(flag);
}
//发送包长度
static void SendLength(int length)
{
	MYUSART_SendData(length>>8);
	MYUSART_SendData(length);
}
//发送指令码
static void Sendcmd(u8 cmd)
{
	MYUSART_SendData(cmd);
}
//发送校验和
static void SendCheck(u16 check)
{
	MYUSART_SendData(check>>8);
	MYUSART_SendData(check);
}
//判断中断接收的数组有没有应答包
//waittime为等待中断接收数据的时间（单位1ms）
//返回值：数据包首地址
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
//		if(USART2_RX_STA&0X8000)//接收到一次数据
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
    u8 xC = 0;    // 循环计数
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
    
    for(xC=0; xC<len-3; xC++)    // 保存要发送的数据
    {
        Temp[9+xC] = data[xC];
    }

    for(xC=0; xC<len; xC++)     // 计算校验和
    {
        TempNum += Temp[xC+6];
    }
    
    Temp[6+len] = (TempNum >> 8) & 0xFF;            // 加入校验和的低 8 位
    Temp[7+len] = TempNum & 0xFF;     // 加入校验和的高 8 位
    
//    USARTx_DMA_SendData(Temp, len+8);       // 发送数据
}

//录入图像 PS_GetImage
//功能:探测手指，探测到后录入指纹图像存于ImageBuffer。 
//模块返回确认字
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
	SendFlag(0x01);//命令包标识
	SendLength(0x03);
	Sendcmd(0x01);
    temp =  0x01+0x03+0x01;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
//    AS608_Send_Data(0x01, 0x03, 0x01, 0);

//	data=JudgeStr(2000);
    /* 请求消息队列 queue 的消息 */
    data = OSQPend ((OS_Q         *)&queue,                //消息变量指针
                    (OS_TICK       )999,                     //等待时长为无限
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //如果没有获取到信号量就等待
                    (OS_MSG_SIZE  *)&msg_size,             //获取消息的字节大小
                    (CPU_TS       *)0,                     //获取任务发送时的时间戳
                    (OS_ERR       *)&err);                 //返回错误
    
    if ( err == OS_ERR_NONE )                              //如果接收成功
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

//生成特征 PS_GenChar
//功能:将ImageBuffer中的原始图像生成指纹特征文件存于CharBuffer1或CharBuffer2			 
//参数:BufferID --> charBuffer1:0x01	charBuffer1:0x02												
//模块返回确认字
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
	SendFlag(0x01);//命令包标识
	SendLength(0x04);
	Sendcmd(0x02);
	MYUSART_SendData(BufferID);
	temp = 0x01+0x04+0x02+BufferID;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
  
//	data=JudgeStr(2000);
    
//    AS608_Send_Data(0x01, 0x04, 0x02, &BufferID);
    
    /* 请求消息队列 queue 的消息 */
    data = OSQPend ((OS_Q         *)&queue,                //消息变量指针
                    (OS_TICK       )999,                     //等待时长为无限
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //如果没有获取到信号量就等待
                    (OS_MSG_SIZE  *)&msg_size,             //获取消息的字节大小
                    (CPU_TS       *)0,                     //获取任务发送时的时间戳
                    (OS_ERR       *)&err);                 //返回错误
    
    if ( err == OS_ERR_NONE )                              //如果接收成功
    {
        if(data)
            return data[9];    //  OK
        else
            return 0xff;     // 错误
    }
    else
        return 0xff;
}

//精确比对两枚指纹特征 PS_Match
//功能:精确比对CharBuffer1 与CharBuffer2 中的特征文件 
//模块返回确认字
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
	SendFlag(0x01);//命令包标识
	SendLength(0x03);
	Sendcmd(0x03);
	temp = 0x01+0x03+0x03;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
//	data=JudgeStr(2000);
    
//    AS608_Send_Data(0x01, 0x03, 0x03, 0);
    
    /* 请求消息队列 queue 的消息 */
    data = OSQPend ((OS_Q         *)&queue,                //消息变量指针
                    (OS_TICK       )0,                     //等待时长为无限
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //如果没有获取到信号量就等待
                    (OS_MSG_SIZE  *)&msg_size,             //获取消息的字节大小
                    (CPU_TS       *)0,                     //获取任务发送时的时间戳
                    (OS_ERR       *)&err);                 //返回错误
    
    if ( err == OS_ERR_NONE )                              //如果接收成功
    {
        if(data)
            return data[9];    //  OK
        else
            return 0xff;     // 错误
    }
    else
        return 0xff;
}

//搜索指纹 PS_Search
//功能:以CharBuffer1或CharBuffer2中的特征文件搜索整个或部分指纹库.若搜索到，则返回页码。			
//参数:  BufferID @ref CharBuffer1	CharBuffer2
//说明:  模块返回确认字，页码（相配指纹模板）
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
	SendFlag(0x01);//命令包标识
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
    
    /* 请求消息队列 queue 的消息 */
    data = OSQPend ((OS_Q         *)&queue,                //消息变量指针
                    (OS_TICK       )0,                     //等待时长为无限
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //如果没有获取到信号量就等待
                    (OS_MSG_SIZE  *)&msg_size,             //获取消息的字节大小
                    (CPU_TS       *)0,                     //获取任务发送时的时间戳
                    (OS_ERR       *)&err);                 //返回错误
                    
    if ( err == OS_ERR_NONE )                              //如果接收成功
    {
        if(data)
        {
		    p->pageID   =(data[10]<<8)+data[11];
		    p->mathscore=(data[12]<<8)+data[13];	
            return data[9];    //  OK
        }
        else
            return 0xff;     // 错误
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

//合并特征（生成模板）PS_RegModel
//功能:将CharBuffer1与CharBuffer2中的特征文件合并生成 模板,结果存于CharBuffer1与CharBuffer2	
//说明:  模块返回确认字
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
	SendFlag(0x01);//命令包标识
	SendLength(0x03);
	Sendcmd(0x05);
	temp = 0x01+0x03+0x05;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
//	data=JudgeStr(2000);
    
//    AS608_Send_Data(0x01, 0x03, 0x05, 0);
    
    /* 请求消息队列 queue 的消息 */
    data = OSQPend ((OS_Q         *)&queue,                //消息变量指针
                    (OS_TICK       )0,                     //等待时长为无限
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //如果没有获取到信号量就等待
                    (OS_MSG_SIZE  *)&msg_size,             //获取消息的字节大小
                    (CPU_TS       *)0,                     //获取任务发送时的时间戳
                    (OS_ERR       *)&err);                 //返回错误
    
    if ( err == OS_ERR_NONE )                              //如果接收成功
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

//储存模板 PS_StoreChar
//功能:将 CharBuffer1 或 CharBuffer2 中的模板文件存到 PageID 号flash数据库位置。			
//参数:  BufferID @ref charBuffer1:0x01	charBuffer1:0x02
//       PageID（指纹库位置号）
//说明:  模块返回确认字
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
	SendFlag(0x01);//命令包标识
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
    /* 请求消息队列 queue 的消息 */
    data = OSQPend ((OS_Q         *)&queue,                //消息变量指针
                    (OS_TICK       )0,                     //等待时长为无限
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //如果没有获取到信号量就等待
                    (OS_MSG_SIZE  *)&msg_size,             //获取消息的字节大小
                    (CPU_TS       *)0,                     //获取任务发送时的时间戳
                    (OS_ERR       *)&err);                 //返回错误
    
    if ( err == OS_ERR_NONE )                              //如果接收成功
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

//删除模板 PS_DeletChar
//功能:  删除flash数据库中指定ID号开始的N个指纹模板
//参数:  PageID(指纹库模板号)，N删除的模板个数。
//说明:  模块返回确认字
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
	SendFlag(0x01);//命令包标识
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
    /* 请求消息队列 queue 的消息 */
    data = OSQPend ((OS_Q         *)&queue,                //消息变量指针
                    (OS_TICK       )0,                     //等待时长为无限
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //如果没有获取到信号量就等待
                    (OS_MSG_SIZE  *)&msg_size,             //获取消息的字节大小
                    (CPU_TS       *)0,                     //获取任务发送时的时间戳
                    (OS_ERR       *)&err);                 //返回错误
    
    if ( err == OS_ERR_NONE )                              //如果接收成功
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

//清空指纹库 PS_Empty
//功能:  删除flash数据库中所有指纹模板
//参数:  无
//说明:  模块返回确认字
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
	SendFlag(0x01);//命令包标识
	SendLength(0x03);
	Sendcmd(0x0D);
	temp = 0x01+0x03+0x0D;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
//	data=JudgeStr(2000);
    
//    AS608_Send_Data(0x01, 0x03, 0x0D, 0);

//	data=JudgeStr(2000);
    /* 请求消息队列 queue 的消息 */
    data = OSQPend ((OS_Q         *)&queue,                //消息变量指针
                    (OS_TICK       )0,                     //等待时长为无限
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //如果没有获取到信号量就等待
                    (OS_MSG_SIZE  *)&msg_size,             //获取消息的字节大小
                    (CPU_TS       *)0,                     //获取任务发送时的时间戳
                    (OS_ERR       *)&err);                 //返回错误
    
    if ( err == OS_ERR_NONE )                              //如果接收成功
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

//写系统寄存器 PS_WriteReg
//功能:  写模块寄存器
//参数:  寄存器序号RegNum:4\5\6
//说明:  模块返回确认字
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
	SendFlag(0x01);//命令包标识
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

    /* 请求消息队列 queue 的消息 */
    data = OSQPend ((OS_Q         *)&queue,                //消息变量指针
                    (OS_TICK       )0,                     //等待时长为无限
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //如果没有获取到信号量就等待
                    (OS_MSG_SIZE  *)&msg_size,             //获取消息的字节大小
                    (CPU_TS       *)0,                     //获取任务发送时的时间戳
                    (OS_ERR       *)&err);                 //返回错误
    
    if ( err == OS_ERR_NONE )                              //如果接收成功
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
//		printf("\r\n设置参数成功！");
//	else
//		printf("\r\n%s",EnsureMessage(ensure));
//	return ensure;
}

//读系统基本参数 PS_ReadSysPara
//功能:  读取模块的基本参数（波特率，包大小等)
//参数:  无
//说明:  模块返回确认字 + 基本参数（16bytes）
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
	SendFlag(0x01);//命令包标识
	SendLength(0x03);
	Sendcmd(0x0F);
	temp = 0x01+0x03+0x0F;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
//	data=JudgeStr(1000);
    
//    AS608_Send_Data(0x01, 0x03, 0x0F, 0);

    /* 请求消息队列 queue 的消息 */
    data = OSQPend ((OS_Q         *)&queue,                //消息变量指针
                    (OS_TICK       )0,                     //等待时长为无限
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //如果没有获取到信号量就等待
                    (OS_MSG_SIZE  *)&msg_size,             //获取消息的字节大小
                    (CPU_TS       *)0,                     //获取任务发送时的时间戳
                    (OS_ERR       *)&err);                 //返回错误
    
    if ( err == OS_ERR_NONE )                              //如果接收成功
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
//		printf("\r\n模块最大指纹容量=%d",p->PS_max);
//		printf("\r\n对比等级=%d",p->PS_level);
//		printf("\r\n地址=%x",p->PS_addr);
//		printf("\r\n波特率=%d",p->PS_N*9600);
//	}
//	else 
//			printf("\r\n%s",EnsureMessage(ensure));
//	return ensure;
}

//设置模块地址 PS_SetAddr
//功能:  设置模块地址
//参数:  PS_addr
//说明:  模块返回确认字
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
	SendFlag(0x01);//命令包标识
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
	AS608Addr=PS_addr;//发送完指令，更换地址
  OS_CRITICAL_EXIT();
  
//  data=JudgeStr(2000);
//    TempData[0] = PS_addr>>24;
//    TempData[1] = PS_addr>>16;
//    TempData[2] = PS_addr>>8;
//    TempData[3] = PS_addr;
//    
//    AS608_Send_Data(0x01, 0x07, 0x15, TempData);

    /* 请求消息队列 queue 的消息 */
    data = OSQPend ((OS_Q         *)&queue,                //消息变量指针
                    (OS_TICK       )0,                     //等待时长为无限
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //如果没有获取到信号量就等待
                    (OS_MSG_SIZE  *)&msg_size,             //获取消息的字节大小
                    (CPU_TS       *)0,                     //获取任务发送时的时间戳
                    (OS_ERR       *)&err);                 //返回错误
    
    if ( err == OS_ERR_NONE )                              //如果接收成功
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
//		printf("\r\n设置地址成功！");
//	else
//		printf("\r\n%s",EnsureMessage(ensure));
//	return ensure;
}

//功能： 模块内部为用户开辟了256bytes的FLASH空间用于存用户记事本,
//	该记事本逻辑上被分成 16 个页。
//参数:  NotePageNum(0~15),Byte32(要写入内容，32个字节)
//说明:  模块返回确认字
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
	SendFlag(0x01);//命令包标识
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

    /* 请求消息队列 queue 的消息 */
    data = OSQPend ((OS_Q         *)&queue,                //消息变量指针
                    (OS_TICK       )0,                     //等待时长为无限
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //如果没有获取到信号量就等待
                    (OS_MSG_SIZE  *)&msg_size,             //获取消息的字节大小
                    (CPU_TS       *)0,                     //获取任务发送时的时间戳
                    (OS_ERR       *)&err);                 //返回错误
    
    if ( err == OS_ERR_NONE )                              //如果接收成功
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
//读记事PS_ReadNotepad
//功能：  读取FLASH用户区的128bytes数据
//参数:  NotePageNum(0~15)
//说明:  模块返回确认字+用户信息
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
	SendFlag(0x01);//命令包标识
	SendLength(0x04);
	Sendcmd(0x19);
	MYUSART_SendData(NotePageNum);
	temp = 0x01+0x04+0x19+NotePageNum;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
//  data=JudgeStr(2000);
    
//    AS608_Send_Data(0x01, 0x04, 0x19, &NotePageNum);

    /* 请求消息队列 queue 的消息 */
    data = OSQPend ((OS_Q         *)&queue,                //消息变量指针
                    (OS_TICK       )0,                     //等待时长为无限
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //如果没有获取到信号量就等待
                    (OS_MSG_SIZE  *)&msg_size,             //获取消息的字节大小
                    (CPU_TS       *)0,                     //获取任务发送时的时间戳
                    (OS_ERR       *)&err);                 //返回错误
    
    if ( err == OS_ERR_NONE )                              //如果接收成功
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
//高速搜索PS_HighSpeedSearch
//功能：以 CharBuffer1或CharBuffer2中的特征文件高速搜索整个或部分指纹库。
//		  若搜索到，则返回页码,该指令对于的确存在于指纹库中 ，且登录时质量
//		  很好的指纹，会很快给出搜索结果。
//参数:  BufferID， StartPage(起始页)，PageNum（页数）
//说明:  模块返回确认字+页码（相配指纹模板）
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
	SendFlag(0x01);//命令包标识
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

    /* 请求消息队列 queue 的消息 */
    data = OSQPend ((OS_Q         *)&queue,                //消息变量指针
                    (OS_TICK       )999,                     //等待时长为无限
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //如果没有获取到信号量就等待
                    (OS_MSG_SIZE  *)&msg_size,             //获取消息的字节大小
                    (CPU_TS       *)0,                     //获取任务发送时的时间戳
                    (OS_ERR       *)&err);                 //返回错误
    
    if ( err == OS_ERR_NONE )                              //如果接收成功
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

//读有效模板个数 PS_ValidTempleteNum
//功能：读有效模板个数
//参数: 无
//说明: 模块返回确认字+有效模板个数ValidN
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
	SendFlag(0x01);//命令包标识
	SendLength(0x03);
	Sendcmd(0x1d);
	temp = 0x01+0x03+0x1d;
	SendCheck(temp);
  OS_CRITICAL_EXIT();
//  data=JudgeStr(2000);
    
//    AS608_Send_Data(0x01, 0x03, 0x1d, 0);

    /* 请求消息队列 queue 的消息 */
    data = OSQPend ((OS_Q         *)&queue,                //消息变量指针
                    (OS_TICK       )0,                     //等待时长为无限
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  //如果没有获取到信号量就等待
                    (OS_MSG_SIZE  *)&msg_size,             //获取消息的字节大小
                    (CPU_TS       *)0,                     //获取任务发送时的时间戳
                    (OS_ERR       *)&err);                 //返回错误
    
    if ( err == OS_ERR_NONE )                              //如果接收成功
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
//		printf("\r\n有效指纹个数=%d",(data[10]<<8)+data[11]);
//	}
//	else
//		printf("\r\n%s",EnsureMessage(ensure));
//	return ensure;
}

//与AS608握手 PS_HandShake
//参数: PS_Addr地址指针
//说明: 模块返新地址（正确地址）	
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
//    printf("发送数据给指纹\n");
    /* 请求消息队列 queue 的消息 */
    data = OSQPend ((OS_Q         *)&queue,                    // 消息变量指针
                    (OS_TICK       )0,                      // 等待 1000ms
                    (OS_OPT        )OS_OPT_PEND_BLOCKING,  // 如果没有获取到信号量就等待
                    (OS_MSG_SIZE  *)&msg_size,                 // 获取消息的字节大小
                    (CPU_TS       *)0,                         // 获取任务发送时的时间戳
                    (OS_ERR       *)&err);                     // 返回错误
//    printf("%d  %d   %d\n",data[0],data[1],data[6]);
    if ( err == OS_ERR_NONE )                                  //如果接收成功
    {
        if(	data[0]==0XEF && data[1]==0X01 && data[6]==0X07) //判断是不是模块返回的应答包	
        {
            //*PS_Addr=(data[2]<<24) + (data[3]<<16) + (data[4]<<8) + (data[5]);
            return 0;
        }
    }
    else
        ensure=1;
	
	return ensure;
    

//	Delay_ms(200);
//	if(USART2_RX_STA&0X8000)//接收到数据
//	{		
//		if(//判断是不是模块返回的应答包				
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

//模块应答包确认码信息解析
//功能：解析确认码错误信息返回信息
//参数: ensure
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





