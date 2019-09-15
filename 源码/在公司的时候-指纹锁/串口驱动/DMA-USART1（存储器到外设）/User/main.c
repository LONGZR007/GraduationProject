
// DMA �洢�������裨���ڣ����ݴ���ʵ��

#include "stm32f10x.h"
#include "bsp_usart_dma.h"
#include "drv_ds1302.h"
#include "bsp_led.h"
u32 AS608Addr = 0XFFFFFFFF; //Ĭ��
u8  TempData[12];
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num);
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
    Usart_SendArray(USART2 ,Temp, len+8);
}
static void Delay(__IO u32 nCount); 


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



/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */

struct rtc_time tm;
int main(void)
{
  u16 temp;
  /* ��ʼ��USART */
  USART_Config(); 

  /* ����ʹ��DMAģʽ */
  USARTx_DMA_Config();
  USARRx_DMA_Config();
  
  /* ����RGB��ɫ�� */
  LED_GPIO_Config();

//  printf("\r\n USART1 DMA TX ���� \r\n");
  SendHead();
  SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x01);
    temp =  0x01+0x03+0x01;
	SendCheck(temp);
  /*Ϊ��ʾDMA�������ж�CPU���ܴ����������飬����ʹ��DMA�������ݣ����ǳ���
  *��ʱ�����п��ܻᵼ�µ��Զ˴��ڵ������ֻῨ��������ҷɵ������
  *���DMA�����е�ѭ��ģʽ��Ϊ����ģʽ*/		
  
  /* USART1 �� DMA����TX���� */
//    DS_Init();
//    TempData[0] = 0xEF;
//    TempData[1] = 0x01;
//    TempData[2] = (AS608Addr>>24)&0xFF;
//    TempData[3] = (AS608Addr>>16)&0xFF;
//    TempData[4] = (AS608Addr>>8)&0xFF;
//    TempData[5] = AS608Addr&0xFF;
//    TempData[6] = 0X01;
//    TempData[7] = 0X00;
//    TempData[8] = 0X00;
//    
////    USARTx_DMA_SendData(TempData, 9);
////  USARTx_DMA_SendData(SendBuff, SENDBUFF_SIZE);
//  /* ��ʱCPU�ǿ��еģ����Ը����������� */  
//  //����ͬʱ����LED
//  Delay(0xFFFFFF);
//  AS608_Send_Data(0x01, 0x03, 0x01, 0);
//  Delay(0xFFFFFF);
  while(1)
  {
//      DS_DateTime_Get(&tm);
//    LED1_TOGGLE
    Delay(0xFFFFF);
  }
}

static void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
