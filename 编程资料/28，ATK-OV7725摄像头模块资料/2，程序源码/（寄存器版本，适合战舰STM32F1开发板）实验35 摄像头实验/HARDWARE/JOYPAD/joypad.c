#include  "joypad.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������V3
//��Ϸ�ֱ����� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/1/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//��ʼ���ֱ��ӿ�.	 
void JOYPAD_Init(void)
{	
 	RCC->APB2ENR|=1<<3;//��ʹ������PORTBʱ��    
 	RCC->APB2ENR|=1<<5;//��ʹ������PORTDʱ��    	
	GPIOB->CRH&=0XFFFF00FF;
	GPIOB->CRH|=0X00003800;     
	GPIOB->ODR|=3<<10; 
	GPIOD->CRL&=0XFFFF0FFF;	//PD3�������
	GPIOD->CRL|=0X00003000;     
	GPIOD->ODR|=1<<3; 
}
//�ֱ��ӳٺ���
//t:Ҫ�ӳٵ�ʱ��
void JOYPAD_Delay(u16 t)
{
	while(t--);
}
//��ȡ�ֱ�����ֵ.
//FC�ֱ����������ʽ:
//ÿ��һ������,���һλ����,���˳��:
//A->B->SELECT->START->UP->DOWN->LEFT->RIGHT.
//�ܹ�8λ,������C��ť���ֱ�,����C��ʵ�͵���A+Bͬʱ����.
//������1,�ɿ���0.
//����ֵ:
//[7]:��
//[6]:��
//[5]:��
//[4]:��
//[3]:Start
//[2]:Select
//[1]:B
//[0]:A
u8 JOYPAD_Read(void)
{
	vu8 temp=0;
 	u8 t;
	JOYPAD_LAT=1;	//���浱ǰ״̬
 	JOYPAD_Delay(80);
	JOYPAD_LAT=0; 
	for(t=0;t<8;t++)
	{
		temp>>=1;	 
		if(JOYPAD_DAT==0)temp|=0x80;//LOAD֮�󣬾͵õ���һ������
		JOYPAD_CLK=1;			   	//ÿ��һ�����壬�յ�һ������
		JOYPAD_Delay(80);
		JOYPAD_CLK=0;	
		JOYPAD_Delay(80); 
	}
	return temp;
}





