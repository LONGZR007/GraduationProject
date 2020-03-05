#ifndef __AS608_H
#define __AS608_H
#include <stdio.h>
#include "stm32f10x.h" 
#include "stm32f10x_gpio.h" 

#define CharBuffer1 0x01
#define CharBuffer2 0x02

extern u32 AS608Addr;//ģ���ַ


#define PS_Sta_GPIO_CLK          (RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO)
#define PS_Sta_GPIO_PIN           GPIO_Pin_1
#define PS_Sta_EXTI_PORTSOURCE    GPIO_PortSourceGPIOA
#define PS_Sta_EXTI_PINSOURCE     GPIO_PinSource1
#define PS_Sta_EXTI_LINE          EXTI_Line1
#define PS_Sta_EXTI_IRQ           EXTI1_IRQn

#define PS_Sta_IRQHandler         EXTI1_IRQHandler

#define PS_Sta_PIN     GPIO_Pin_1
#define PS_Sta_PORT    GPIOA

#define PS_Sta   (GPIO_ReadInputDataBit(PS_Sta_PORT, PS_Sta_PIN))    //��ָ��ģ��״̬����

typedef struct  
{
	u16 pageID;//ָ��ID
	u16 mathscore;//ƥ��÷�
}SearchResult;

typedef struct
{
	u16 PS_max;//ָ���������
	u8  PS_level;//��ȫ�ȼ�
	u32 PS_addr;
	u8  PS_size;//ͨѶ���ݰ���С
	u8  PS_N;//�����ʻ���N
}SysPara;

void PS_Sta_IRQDisable(void);
void PS_Sta_IRQEnable(void);

void PS_StaGPIO_Init(void);//��ʼ��PA6��״̬����
	
u8 PS_GetImage(void); //¼��ͼ�� 
 
u8 PS_GenChar(u8 BufferID);//�������� 

u8 PS_Match(void);//��ȷ�ȶ���öָ������ 

u8 PS_Search(u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p);//����ָ�� 
 
u8 PS_RegModel(void);//�ϲ�����������ģ�壩 
 
u8 PS_StoreChar(u8 BufferID,u16 PageID);//����ģ�� 

u8 PS_DeletChar(u16 PageID,u16 N);//ɾ��ģ�� 

u8 PS_Empty(void);//���ָ�ƿ� 

u8 PS_WriteReg(u8 RegNum,u8 DATA);//дϵͳ�Ĵ��� 
 
u8 PS_ReadSysPara(SysPara *p); //��ϵͳ�������� 

u8 PS_SetAddr(u32 addr);  //����ģ���ַ 

u8 PS_WriteNotepad(u8 NotePageNum,u8 *content);//д���±� 

u8 PS_ReadNotepad(u8 NotePageNum,u8 *note);//������ 

u8 PS_HighSpeedSearch(u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p);//�������� 
  
u8 PS_ValidTempleteNum(u16 *ValidN);//����Чģ����� 

u8 PS_HandShake(u32 *PS_Addr); //��AS608ģ������

const char *EnsureMessage(u8 ensure);//ȷ���������Ϣ����

#endif

