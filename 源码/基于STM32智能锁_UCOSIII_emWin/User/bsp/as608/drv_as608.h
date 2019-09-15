#ifndef __AS608_H
#define __AS608_H
#include <stdio.h>
#include "stm32f10x.h" 
#include "stm32f10x_gpio.h" 

#define CharBuffer1 0x01
#define CharBuffer2 0x02

extern u32 AS608Addr;//模块地址


#define PS_Sta_GPIO_CLK          (RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO)
#define PS_Sta_GPIO_PIN           GPIO_Pin_1
#define PS_Sta_EXTI_PORTSOURCE    GPIO_PortSourceGPIOA
#define PS_Sta_EXTI_PINSOURCE     GPIO_PinSource1
#define PS_Sta_EXTI_LINE          EXTI_Line1
#define PS_Sta_EXTI_IRQ           EXTI1_IRQn

#define PS_Sta_IRQHandler         EXTI1_IRQHandler

#define PS_Sta_PIN     GPIO_Pin_1
#define PS_Sta_PORT    GPIOA

#define PS_Sta   (GPIO_ReadInputDataBit(PS_Sta_PORT, PS_Sta_PIN))    //读指纹模块状态引脚

typedef struct  
{
	u16 pageID;//指纹ID
	u16 mathscore;//匹配得分
}SearchResult;

typedef struct
{
	u16 PS_max;//指纹最大容量
	u8  PS_level;//安全等级
	u32 PS_addr;
	u8  PS_size;//通讯数据包大小
	u8  PS_N;//波特率基数N
}SysPara;

void PS_Sta_IRQDisable(void);
void PS_Sta_IRQEnable(void);

void PS_StaGPIO_Init(void);//初始化PA6读状态引脚
	
u8 PS_GetImage(void); //录入图像 
 
u8 PS_GenChar(u8 BufferID);//生成特征 

u8 PS_Match(void);//精确比对两枚指纹特征 

u8 PS_Search(u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p);//搜索指纹 
 
u8 PS_RegModel(void);//合并特征（生成模板） 
 
u8 PS_StoreChar(u8 BufferID,u16 PageID);//储存模板 

u8 PS_DeletChar(u16 PageID,u16 N);//删除模板 

u8 PS_Empty(void);//清空指纹库 

u8 PS_WriteReg(u8 RegNum,u8 DATA);//写系统寄存器 
 
u8 PS_ReadSysPara(SysPara *p); //读系统基本参数 

u8 PS_SetAddr(u32 addr);  //设置模块地址 

u8 PS_WriteNotepad(u8 NotePageNum,u8 *content);//写记事本 

u8 PS_ReadNotepad(u8 NotePageNum,u8 *note);//读记事 

u8 PS_HighSpeedSearch(u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p);//高速搜索 
  
u8 PS_ValidTempleteNum(u16 *ValidN);//读有效模板个数 

u8 PS_HandShake(u32 *PS_Addr); //与AS608模块握手

const char *EnsureMessage(u8 ensure);//确认码错误信息解析

#endif

