
// DMA 存储器到外设（串口）数据传输实验

#include "stm32f10x.h"
#include "bsp_usart_dma.h"
#include "drv_ds1302.h"
#include "bsp_led.h"
u32 AS608Addr = 0XFFFFFFFF; //默认
u8  TempData[12];
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num);
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
    Usart_SendArray(USART2 ,Temp, len+8);
}
static void Delay(__IO u32 nCount); 


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



/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */

struct rtc_time tm;
int main(void)
{
  u16 temp;
  /* 初始化USART */
  USART_Config(); 

  /* 配置使用DMA模式 */
  USARTx_DMA_Config();
  USARRx_DMA_Config();
  
  /* 配置RGB彩色灯 */
  LED_GPIO_Config();

//  printf("\r\n USART1 DMA TX 测试 \r\n");
  SendHead();
  SendAddr();
	SendFlag(0x01);//命令包标识
	SendLength(0x03);
	Sendcmd(0x01);
    temp =  0x01+0x03+0x01;
	SendCheck(temp);
  /*为演示DMA持续运行而CPU还能处理其它事情，持续使用DMA发送数据，量非常大，
  *长时间运行可能会导致电脑端串口调试助手会卡死，鼠标乱飞的情况，
  *或把DMA配置中的循环模式改为单次模式*/		
  
  /* USART1 向 DMA发出TX请求 */
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
//  /* 此时CPU是空闲的，可以干其他的事情 */  
//  //例如同时控制LED
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

static void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
