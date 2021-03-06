/****************************************************************************************************
 * 描述：SI2C 接口硬件驱动层实现
 *
 * 作者：龙泽润
 *
 * 版本：v1.0.0    日期：2018-05-02
 *                                                                              大连智海科技有限公司
****************************************************************************************************/

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "..\User\bsp\led\bsp_led.h" 
#include "drv_si2c.h"
#include "drv_oled.h"

/****************************************************************************************************
 * 描述：SI2C1 总线初始化
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

void SI2C_Init(void)
{
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启GPIOB和GPIOF的外设时钟*/
		RCC_APB2PeriphClockCmd( SI2C_SCL_GPIO_CLK | SI2C_SDA_GPIO_CLK, ENABLE); 

		/*选择要控制的GPIOB引脚*/															   
		GPIO_InitStructure.GPIO_Pin = SI2C_SCL_PIN;	

		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;   

		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*调用库函数，初始化GPIOB0*/
		GPIO_Init(SI2C_SCL_PORT, &GPIO_InitStructure);	
  
    /*选择要控制的GPIOB引脚*/															   
		GPIO_InitStructure.GPIO_Pin = SI2C_SDA_PIN;	
  
    /*调用库函数，初始化GPIOB0*/
		GPIO_Init(SI2C_SDA_PORT, &GPIO_InitStructure);
}

void SI2C_SCL_SET(void)  	
{	
  digitalHi(SI2C_SCL_PORT, SI2C_SCL_PIN);
//	GPIO_OUT_SET(GPIOF, 15);   // SCL 输出置位
}

void SI2C_SCL_RST(void)    
{
  digitalLo(SI2C_SCL_PORT, SI2C_SCL_PIN);
//	GPIO_OUT_RST(GPIOF, 15); // SCL 输出复位
}

void SI2C_SDA_SET(void)    
{
  digitalHi(SI2C_SDA_PORT, SI2C_SDA_PIN);
//	GPIO_OUT_SET(GPIOF, 14); // SDA 输出置位
} 

void SI2C_SDA_RST(void)    
{
  digitalLo(SI2C_SDA_PORT, SI2C_SDA_PIN);
//	GPIO_OUT_RST(GPIOF, 14);    // SDA 输出复位
}

u8 SI2C_SDA_GET(void)    
{
	return (SI2C_SDA_PORT->IDR >> 11) & 1;    // SDA 输入获取
}

/****************************************************************************************************
 * 描述：配置 SDA 为输出
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

void SI2C_SDA_OUT(void)
{
//	GPIO_Config(GPIOA, SI2C_SDA, GPIO_MODE_GP_PP, GPIO_SPEED_50M);    // SDA 管脚, 通用推挽输出, 速度 50 MHz
}

/****************************************************************************************************
 * 描述：配置 SDA 为输入
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

void SI2C_SDA_IN(void)
{
//	GPIO_Config(GPIOA, SI2C_SDA, GPIO_MODE_IPU, 0);    // SDA 管脚, 上拉输入
}

/****************************************************************************************************
 * 描述：SI2C 产生起始信号
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

void SI2C_Start(void)
{
	SI2C_SDA_OUT();    // SDA 配置为输出
	SI2C_SCL_SET();    // 拉高时钟线
	SI2C_SDA_SET();    // SDA 为高
	SI2C_SDA_RST();    // SDA 为低
	SI2C_SCL_RST();    // 拉低时钟线，等待数据发送
}

/****************************************************************************************************
 * 描述：SI2C 产生停止信号
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

void SI2C_Stop(void)
{
	SI2C_SDA_OUT();    // SDA 配置为输出
	
	SI2C_SCL_SET();    // 拉高时钟线
	SI2C_SDA_RST();    // SDA 为低
	SI2C_SDA_SET();    // SDA 为高
//    SI2C_SCL_RST();    // 拉高时钟线

}

/****************************************************************************************************
 * 描述：等待应答信号
 *
 * 参数：无
 *
 * 返回：1：接收成功    0：接收失败
****************************************************************************************************/

u8 SI2C_Wait_ACK(void)
{
//	u8 ACKErrTime = 0;    // 循坏计数
	
//	SI2C_SDA_IN();        // SDA 配置为输入  
	
	SI2C_SCL_SET();       // 拉高时钟线

//	while(SI2C_SDA_GET() == 1)
	{

//		if(ACKErrTime++ > 250)    // 超时未接收到应答
//		{
////			SI2C_Stop();          // 产生停止信号
//			
//			return 0;             // 返回 0 接收应答失败
//		}
	}
	
	SI2C_SCL_RST();               // 拉低时钟线

	return 1;                     // 返回 1 接收应答成功
} 

/****************************************************************************************************
 * 描述：产生应答信号
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

void SI2C_ACK(void)
{	
	SI2C_SCL_RST();    // 拉低时钟线
	
	SI2C_SDA_OUT();    // SDA 配置为输出
	
	SI2C_SDA_RST();    // 复位 SDA
	Delay_us(2);       // 延时 2us
	SI2C_SCL_SET();    // 拉高时钟线
	Delay_us(2);       // 延时 2us
	SI2C_SCL_RST();    // 拉低时钟线
}

/****************************************************************************************************
 * 描述：不产生应答信号
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

void SI2C_NACK(void)
{
	SI2C_SCL_RST();    // 拉低时钟线
	
	SI2C_SDA_OUT();    // SDA 配置为输出
	
	SI2C_SDA_SET();    // SDA 置高
	Delay_us(2);       // 延时 2us
	SI2C_SCL_SET();    // 拉高时钟线
	Delay_us(2);       // 延时 2us
	SI2C_SCL_RST();    // 拉低时钟线
}

/****************************************************************************************************
 * 描述：SI2C 发送一个字节
 *
 * 参数：txd：发送的数据
 *
 * 返回：无
****************************************************************************************************/

void SI2C_Send_Byte(u8 txd)
{                        
    u8 Count;          // 循环计数
	
	SI2C_SDA_OUT();    // SDA 配置为输出	    
	SI2C_SCL_RST();    // 拉低时钟线开始数据传输
	
    for(Count=8; Count>0; Count--)
    {
		if (txd & (1 << (Count - 1)))
		{
			SI2C_SDA_SET();    // SDA 置高
		}
		else
		{
			SI2C_SDA_RST();    // SDA 置低
		}
		SI2C_SCL_SET();        // 拉高时钟线
		SI2C_SCL_RST();        // 拉低时钟线
    }	 
}

/****************************************************************************************************
 * 描述：SI2C 发送一个字节(反显)
 *
 * 参数：txd：发送的数据
 *
 * 返回：无
****************************************************************************************************/

void SI2C_Send_Against_Byte(u8 txd)
{                        
    u8 Count;          // 循环计数
	
	SI2C_SDA_OUT();    // SDA 配置为输出	    
	SI2C_SCL_RST();    // 拉低时钟线开始数据传输
	
    for(Count=8; Count>0; Count--)
    {
		if (txd & (1 << (Count - 1)))
		{
			SI2C_SDA_RST();    // SDA 置低
		}
		else
		{
			SI2C_SDA_SET();    // SDA 置高
		}
		SI2C_SCL_SET();        // 拉高时钟线
		SI2C_SCL_RST();        // 拉低时钟线
    }	 
}

/****************************************************************************************************
 * 描述：SI2C 接收一个字节
 *
 * 参数：ack = 1，发送ACK
 * 参数：ack = 0，发送NACK 
 *
 * 返回：SI2C 接收到的数据
****************************************************************************************************/

u8 SI2C_Read_Byte(u8 ack)
{
	u8 Count;        // 循环计数
	u8 Receive = 0;    // 储存接收的数据
	
	SI2C_SDA_IN();     // SDA 配置为输入
	
    for(Count=0;Count<8;Count++ )
	{
		SI2C_SCL_RST();        // 拉低时钟线
		Delay_us(2);           // 延时 2us
		SI2C_SCL_SET();        // 拉高时钟线
        Receive <<= 1;         // 将接收数据右移一位
		
        if(SI2C_SDA_GET() == 1)
		{
			Receive++;         // 接收位为 1
		}
		
		Delay_us(1);           // 延时 1 us
    }			
	
    if (!ack)
        SI2C_NACK();           // 发送NACK
    else
        SI2C_ACK();            // 发送ACK
	
    return Receive;            // 返回接收的数据
}

/****************************************************************************************************
 * 描述：微秒延时函数
 *
 * 参数：time：时长
 *
 * 返回：无
****************************************************************************************************/

void Delay_us(u16 time)
{    
   u16 i = 0;  
   while(time--)
   {
      i = 1;          // 自己定义
      while(i--);
   }
}

/* End Of File */
