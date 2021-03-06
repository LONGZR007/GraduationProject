/****************************************************************************************************
 * 描述：DS1302 时钟芯片驱动程序实现
 *
 * 作者：Gavin
 *
 * 版本：v1.0.0    日期：2018-05-10
 *                                                                              大连智海科技有限公司
*****************************************************************************************************/

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "drv_ds1302.h"
#include <string.h>
#include <stdio.h>

/* 管脚操作宏定义 */

#define FALSE    0
#define TRUE     1

//#define DS_SCL_SET(val)    (val ? GPIO_BSR_SET(DS_SCL_PORT, DS_SCL_PIN) : GPIO_BRR_SET(DS_SCL_PORT, DS_SCL_PIN))    // 时钟线设置, FALSE = 低电平, TRUE = 高电平
//#define DS_SDA_SET(val)    (val ? GPIO_BSR_SET(DS_SDA_PORT, DS_SDA_PIN) : GPIO_BRR_SET(DS_SDA_PORT, DS_SDA_PIN))    // 数据线设置, FALSE = 低电平, TRUE = 高电平
//#define DS_SDA_GET()       (GPIO_IDR_GET(DS_SDA_PORT, DS_SDA_PIN))                                                  // 数据线状态获取
//#define DS_RST_SET(val)    (val ? GPIO_BSR_SET(DS_CE_PORT, DS_CE_PIN) : GPIO_BRR_SET(DS_CE_PORT, DS_CE_PIN))        // 复位线设置, FALSE = 低电平, TRUE = 高电平

void DS_SCL_SET(u8 val)  
{    
    if (val)
    GPIO_BSR_SET(DS_SCL_PORT, DS_SCL_PIN) ;
    else
    GPIO_BRR_SET(DS_SCL_PORT, DS_SCL_PIN)   ;
}    

void DS_SDA_SET(u8 val)   
{
    if (val)
    GPIO_BSR_SET(DS_SDA_PORT, DS_SDA_PIN);
    else
    GPIO_BRR_SET(DS_SDA_PORT, DS_SDA_PIN);    // 数据线设置, FALSE = 低电平, TRUE = 高电平
}

u8 DS_SDA_GET(void)   
{
    return (GPIO_IDR_GET(DS_SDA_PORT, DS_SDA_PIN))     ;
}    // 数据线状态获取

void DS_RST_SET(u8 val)   
{
    if(val)
    GPIO_BSR_SET(DS_CE_PORT, DS_CE_PIN) ; 
    else
    GPIO_BRR_SET(DS_CE_PORT, DS_CE_PIN)  ;      // 复位线设置, FALSE = 低电平, TRUE = 高电平
}
/* 内部函数声明 */

static void Send_Byte(u8 Dat);    // 发送单字节数据
static u8   Read_Byte(void);      // 读取单字节数据

/****************************************************************************************************
 * 描述：时钟芯片初始化
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/
	 
void DS_Init(void)
{
	/* 管脚配置 */	
    GPIO_InitTypeDef GPIO_InitStructure;

    /*开启GPIOB和GPIOF的外设时钟*/
    RCC_APB2PeriphClockCmd(DS_SCL_PORT_CLK | DS_CE_PORT_CLK | DS_SDA_PORT_CLK, ENABLE); 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    /*选择要控制的GPIOB引脚*/															   
    GPIO_InitStructure.GPIO_Pin = DS_SCL_PIN;	

    /*设置引脚模式为通用推挽输出*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

    /*设置引脚速率为50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

    /*调用库函数，初始化GPIOB0*/
    GPIO_Init(DS_SCL_PORT, &GPIO_InitStructure);	

    /*选择要控制的GPIOB引脚*/															   
    GPIO_InitStructure.GPIO_Pin = DS_CE_PIN;	

    /*调用库函数，初始化GPIOB0*/
    GPIO_Init(DS_CE_PORT, &GPIO_InitStructure);
    
     /*选择要控制的GPIOB引脚*/															   
    GPIO_InitStructure.GPIO_Pin = DS_SDA_PIN;
    
    /*设置引脚模式为通用推挽输出*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	

    /*调用库函数，初始化GPIOB0*/
    GPIO_Init(DS_SDA_PORT, &GPIO_InitStructure);
}

/****************************************************************************************************
 * 描述：BCD转十进制
 *
 * 参数：dt = 日期时间指针(BCD 编码, 依次为年(2位)、月、日、时、分、秒、周)
 *
 * 返回：无
****************************************************************************************************/
	 
void BCD_Metric_convert(struct rtc_time* time)
{
    time->tm_sec  = (time->tm_sec  >> 4) * 10 + (time->tm_sec  & 0xF);
    time->tm_min  = (time->tm_min  >> 4) * 10 + (time->tm_min  & 0xF);
    time->tm_hour = (time->tm_hour >> 4) * 10 + (time->tm_hour & 0xF);
    time->tm_mday = (time->tm_mday >> 4) * 10 + (time->tm_mday & 0xF);
    time->tm_mon  = (time->tm_mon  >> 4) * 10 + (time->tm_mon  & 0xF);
    time->tm_year = (time->tm_year >> 4) * 10 + (time->tm_year & 0xF);
    time->tm_wday = (time->tm_wday >> 4) * 10 + (time->tm_wday & 0xF);
    
}

/****************************************************************************************************
 * 描述：十进制转BCD
 *
 * 参数：dt = 日期时间指针(十进制, 依次为年(2位)、月、日、时、分、秒、周)
 *
 * 返回：无
****************************************************************************************************/
	 
void Metric_BCD_convert(struct rtc_time* time)
{
    time->tm_sec  = ((time->tm_sec  % 100) / 10 << 4) + (time->tm_sec  % 10);
    time->tm_min  = ((time->tm_min  % 100) / 10 << 4) + (time->tm_min  % 10);
    time->tm_hour = ((time->tm_hour % 100) / 10 << 4) + (time->tm_hour % 10);
    time->tm_mday = ((time->tm_mday % 100) / 10 << 4) + (time->tm_mday % 10);
    time->tm_mon  = ((time->tm_mon  % 100) / 10 << 4) + (time->tm_mon  % 10);
    time->tm_year = ((time->tm_year % 100) / 10 << 4) + (time->tm_year % 10);
    time->tm_wday = ((time->tm_wday % 100) / 10 << 4) + (time->tm_wday % 10);
}

/****************************************************************************************************
 * 描述：日期时间设置
 *
 * 参数：dt = 日期时间指针(BCD 编码, 依次为年(2位)、月、日、时、分、秒、周)
 *
 * 返回：无
****************************************************************************************************/
	 
void DS_DateTime_Set(struct rtc_time* time)
{
	DS_RST_SET(TRUE);     // 拉高复位线
	DS_WP_SET(FALSE);     // 解除写保护
	Send_Byte(0xBE);      // 发送命令字
	Send_Byte(time->tm_sec);       // 设置秒
	Send_Byte(time->tm_min);       // 设置分
	Send_Byte(time->tm_hour);      // 设置小时
	Send_Byte(time->tm_mday);      // 设置日期
	Send_Byte(time->tm_mon);       // 设置月份
	Send_Byte(time->tm_wday);      // 设置星期
	Send_Byte(time->tm_year);      // 设置年
	DS_WP_SET(TRUE);      // 设置写保护
	DS_RST_SET(FALSE);    // 拉低复位线
}

/****************************************************************************************************
 * 描述：日期年设置
 *
 * 参数：Year = 2 位年份(BCD 编码)
 *
 * 返回：无
****************************************************************************************************/

void DS_Year_Set(u8 Year)
{
	DS_RST_SET(TRUE);     // 拉高复位线
	DS_WP_SET(FALSE);     // 解除写保护
	Send_Byte(0x8C);      // 发送命令字
	Send_Byte(Year);      // 发送年设置值
	DS_WP_SET(TRUE);      // 设置写保护
	DS_RST_SET(FALSE);    // 拉低复位线
}

/****************************************************************************************************
 * 描述：日期月设置
 *
 * 参数：Mon = 月份(BCD 编码, 1 - 12)
 *
 * 返回：无
****************************************************************************************************/

void DS_Month_Set(u8 Mon)
{
	DS_RST_SET(TRUE);     // 拉高复位线
	DS_WP_SET(FALSE);     // 解除写保护
	Send_Byte(0x88);      // 发送命令字
	Send_Byte(Mon);       // 发送月设置值
	DS_WP_SET(TRUE);      // 设置写保护
	DS_RST_SET(FALSE);    // 拉低复位线
}

/****************************************************************************************************
 * 描述：日期日设置
 *
 * 参数：Day = 日(BCD 编码, 1 - 31)
 *
 * 返回：无
****************************************************************************************************/

void DS_Day_Set(u8 Day)
{
	DS_RST_SET(TRUE);     // 拉高复位线
	DS_WP_SET(FALSE);     // 解除写保护
	Send_Byte(0x86);      // 发送命令字
	Send_Byte(Day);       // 发送日设置值
	DS_WP_SET(TRUE);      // 设置写保护
	DS_RST_SET(FALSE);    // 拉低复位线
}

/****************************************************************************************************
 * 描述：时间时设置
 *
 * 参数：Hour = 时(BCD 编码, 0 - 23(24 小时制)/1 - 12, Bit 5 = 上午/下午(1 = 下午, 0 = 上午)(12 小时制))
 *
 * 返回：无
****************************************************************************************************/

void DS_Hour_Set(u8 Hour)
{
	DS_RST_SET(TRUE);     // 拉高复位线
	DS_WP_SET(FALSE);     // 解除写保护
	Send_Byte(0x84);      // 发送命令字
	Send_Byte(Hour);      // 发送时设置值
	DS_WP_SET(TRUE);      // 设置写保护
	DS_RST_SET(FALSE);    // 拉低复位线
}

/****************************************************************************************************
 * 描述：时间分设置
 *
 * 参数：Min = 分(BCD 编码, 0 - 59)
 *
 * 返回：无
****************************************************************************************************/

void DS_Minute_Set(u8 Min)
{
	DS_RST_SET(TRUE);     // 拉高复位线
	DS_WP_SET(FALSE);     // 解除写保护
	Send_Byte(0x82);      // 发送命令字
	Send_Byte(Min);       // 发送分设置值
	DS_WP_SET(TRUE);      // 设置写保护
	DS_RST_SET(FALSE);    // 拉低复位线
}

/****************************************************************************************************
 * 描述：时间秒设置
 *
 * 参数：Sec = 秒(BCD 编码, 0 - 59)
 *
 * 返回：无
****************************************************************************************************/

void DS_Second_Set(u8 Sec)
{
	DS_RST_SET(TRUE);     // 拉高复位线
	DS_WP_SET(FALSE);     // 解除写保护
	Send_Byte(0x80);      // 发送命令字
	Send_Byte(Sec);       // 发送秒设置值
	DS_WP_SET(TRUE);      // 设置写保护
	DS_RST_SET(FALSE);    // 拉低复位线
}

/****************************************************************************************************
 * 描述：日期周设置
 *
 * 参数：Week = 周(BCD 编码, 1 - 7)
 *
 * 返回：无
****************************************************************************************************/

void DS_Week_Set(u8 Week)
{
	DS_RST_SET(TRUE);     // 拉高复位线
	DS_WP_SET(FALSE);     // 解除写保护
	Send_Byte(0x8A);      // 发送命令字
	Send_Byte(Week);      // 发送秒设置值
	DS_WP_SET(TRUE);      // 设置写保护
	DS_RST_SET(FALSE);    // 拉低复位线
}

/****************************************************************************************************
 * 描述：时间模式设置
 *
 * 参数：Mode = 时间模式(TRUE = 12 小时制式, FALSE = 24 小时制式)
 *
 * 返回：无
****************************************************************************************************/

void DS_TimeMode_Set(u8 Mode)
{
	u8 hour = DS_Hour_Get();    // 获取当前小时
	
	if ((hour >> 7) != Mode)    // 模式需要设置
	{
		if (Mode)    // 转换为 12 小时制
		{
			if (hour >= 0x12)     // 小时大于 12(下午)
			{
				hour -=  hour > 0x12 ? 0x12 : 0;
				hour &= 0x20;
			}
			else if (hour == 0)    // 凌晨 0 点
			{
				hour = 0x12;
			}
			
			hour &= 0x80;    // 设置 12 小时制
		}
		else         // 转换为 24 小时制
		{
			if (hour & 0x20)    // 下午
			{
				if ((hour & 0x1F) < 0x12)
				{
					hour += 0x12;    // 小时转换
				}
				
				hour &= 0x1F;
			}
			else if (hour == 0x12)
			{
				hour = 0;
			}
		}
			
		DS_Hour_Set(hour);      // 设置模式
	}
}

/****************************************************************************************************
 * 描述：涓流充电设置
 *
 * 参数：TCS = 充电功能选择设置(参见数据结构定义)
 * 参数：DS  = 二极管选择设置(参见数据结构定义)
 * 参数：RS  = 电阻选择设置(参见数据结构定义)
 *
 * 返回：无
****************************************************************************************************/

void DS_TC_Set(ds_tcs_t TCS, ds_ds_t DS, ds_rs_t RS)
{
	DS_RST_SET(TRUE);            // 拉高复位线
	DS_WP_SET(FALSE);            // 解除写保护
	Send_Byte(0x90);             // 发送命令字
	Send_Byte(TCS | DS | RS);    // 发送涓流充电设置值
	DS_WP_SET(TRUE);             // 设置写保护
	DS_RST_SET(FALSE);           // 拉低复位线
}

/****************************************************************************************************
 * 描述：涓流充电设置获取
 *
 * 参数：无
 *
 * 返回：涓流充电设置值(Bit 0-1 = 电阻选择设置, Bit 2-3 = 二极管选择设置, Bit 4-7 = 涓流充电选择)
****************************************************************************************************/

u8 DS_TC_Get(void)
{
	u8 Dat = 0;    // 读取数据
	
	DS_RST_SET(TRUE);     // 拉高复位线
	Send_Byte(0x91);      // 发送命令字
	Dat = Read_Byte();    // 读取接收数据
	DS_RST_SET(FALSE);    // 拉低复位线
	
	return Dat;    // 返回读取的数据
}

/****************************************************************************************************
 * 描述：时间获取
 *
 * 参数：dt = 接收日期时间的指针(BCD 编码, 依次为年(2位)、月、日、时、分、秒、周)
 *
 * 返回：无
****************************************************************************************************/
	 
void DS_DateTime_Get(struct rtc_time* time)//u8 *dt
{
	DS_RST_SET(TRUE);       // 拉高复位线
	DS_WP_SET(FALSE);       // 解除写保护
	Send_Byte(0xBF);        // 发送命令字
	time->tm_sec  = Read_Byte();    // 读秒
	time->tm_min  = Read_Byte();    // 读分
	time->tm_hour = Read_Byte();    // 读小时
	time->tm_mday = Read_Byte();    // 读日期
	time->tm_mon  = Read_Byte();    // 读月份
	time->tm_wday = Read_Byte();    // 读星期
	time->tm_year = Read_Byte();    // 读年
//	dt[5] = Read_Byte();    // 读取秒
//	dt[4] = Read_Byte();    // 读取分
//	dt[3] = Read_Byte();    // 读取时
//	dt[2] = Read_Byte();    // 读取日
//	dt[1] = Read_Byte();    // 读取月
//	dt[6] = Read_Byte();    // 读取周
//	dt[0] = Read_Byte();    // 读取年
	DS_WP_SET(TRUE);        // 设置写保护
	DS_RST_SET(FALSE);      // 拉低复位线
}

/****************************************************************************************************
 * 描述：日期年获取
 *
 * 参数：无
 *
 * 返回：2 位年份(BCD 编码)
****************************************************************************************************/

u8 DS_Year_Get(void)
{
	u8 Dat = 0;    // 读取数据
	
	DS_RST_SET(TRUE);     // 拉高复位线
	Send_Byte(0x8D);      // 发送命令字
	Dat = Read_Byte();    // 读取接收数据
	DS_RST_SET(FALSE);    // 拉低复位线
	
	return Dat;    // 返回读取的数据
}

/****************************************************************************************************
 * 描述：日期月获取
 *
 * 参数：无
 *
 * 返回：月份(BCD 编码, 1 - 12)
****************************************************************************************************/

u8 DS_Month_Get(void)
{
	u8 Dat = 0;    // 读取数据
	
	DS_RST_SET(TRUE);     // 拉高复位线
	Send_Byte(0x89);      // 发送命令字
	Dat = Read_Byte();    // 读取接收数据
	DS_RST_SET(FALSE);    // 拉低复位线
	
	return Dat;    // 返回读取的数据
}

/****************************************************************************************************
 * 描述：日期日获取
 *
 * 参数：无
 *
 * 返回：日(BCD 编码, 1 - 31)
****************************************************************************************************/

u8 DS_Day_Get(void)
{
	u8 Dat = 0;    // 读取数据
	
	DS_RST_SET(TRUE);     // 拉高复位线
	Send_Byte(0x87);      // 发送命令字
	Dat = Read_Byte();    // 读取接收数据
	DS_RST_SET(FALSE);    // 拉低复位线
	
	return Dat;    // 返回读取的数据
}

/****************************************************************************************************
 * 描述：时间时获取
 *
 * 参数：无
 *
 * 返回：小时(BCD 编码, 0 - 23(24 小时制)/1 - 12, Bit 5 = 上午/下午(1 = 下午, 0 = 上午)(12 小时制))
****************************************************************************************************/

u8 DS_Hour_Get(void)
{
	u8 Dat = 0;    // 读取数据
	
	DS_RST_SET(TRUE);     // 拉高复位线
	Send_Byte(0x85);      // 发送命令字
	Dat = Read_Byte();    // 读取接收数据
	DS_RST_SET(FALSE);    // 拉低复位线
	
	return Dat & 0x3F;    // 返回读取的数据
}

/****************************************************************************************************
 * 描述：时间分获取
 *
 * 参数：无
 *
 * 返回：分钟(BCD 编码, 0 - 59)
****************************************************************************************************/

u8 DS_Minute_Get(void)
{
	u8 Dat = 0;    // 读取数据
	
	DS_RST_SET(TRUE);     // 拉高复位线
	Send_Byte(0x83);      // 发送命令字
	Dat = Read_Byte();    // 读取接收数据
	DS_RST_SET(FALSE);    // 拉低复位线
	
	return Dat;    // 返回读取的数据
}

/****************************************************************************************************
 * 描述：时间秒获取
 *
 * 参数：无
 *
 * 返回：秒钟(BCD 编码, 0 - 59)
****************************************************************************************************/

u8 DS_Second_Get(void)
{
	u8 Dat = 0;    // 读取数据
	
	DS_RST_SET(TRUE);     // 拉高复位线
	Send_Byte(0x81);      // 发送命令字
	Dat = Read_Byte();    // 读取接收数据
	DS_RST_SET(FALSE);    // 拉低复位线
	
	return Dat;    // 返回读取的数据
}

/****************************************************************************************************
 * 描述：日期周获取
 *
 * 参数：无
 *
 * 返回：周(BCD 编码, 0 - 7)
****************************************************************************************************/

u8 DS_Week_Get(void)
{
	u8 Dat = 0;    // 读取数据
	
	DS_RST_SET(TRUE);     // 拉高复位线
	Send_Byte(0x8B);      // 发送命令字
	Dat = Read_Byte();    // 读取接收数据
	DS_RST_SET(FALSE);    // 拉低复位线
	
	return Dat;    // 返回读取的数据
}

/****************************************************************************************************
 * 描述：时钟芯片内存写入
 *
 * 参数：Addr = 写入地址(0 - 30, 共 31 个)
 * 参数：Dat  = 写入数据指针
 * 参数：Len  = 写入长度(1 - (30 - Addr))
 *
 * 返回：实际写入长度
****************************************************************************************************/

u8 DS_RAM_Write(u8 Addr, u8 *Dat, u8 Len)
{
	u8 Ret = 0;      // 返回值
	u8 xC = Addr;    // 循环计数
	
	if (Addr < 31)       // 地址合法
	{
		Addr *= 2;       // 地址偶数转换
		Addr += 0xC0;    // 命令字计算
		
		DS_RST_SET(TRUE);    // 拉高复位线
		DS_WP_SET(FALSE);    // 解除写保护
		
		for (; Ret < Len && xC < 31; xC++)    // 循环写入数据
		{
			Send_Byte(Addr + Ret * 2);        // 发送命令字
			Send_Byte(Dat[Ret]);              // 发送写入数据
			Ret++;                            // 写入计数
		}
	}
	
	DS_WP_SET(TRUE);      // 设置写保护
	DS_RST_SET(FALSE);    // 拉低复位线
	
	return Ret;           // 函数返回
}

/****************************************************************************************************
 * 描述：时钟芯片内存读取
 *
 * 参数：Addr = 读取地址(0 - 30, 共 31 个)
 * 参数：Dat  = 接收读取数据指针
 * 参数：Len  = 读取长度(1 - (30 - Addr))
 *
 * 返回：实际读取长度
****************************************************************************************************/

u8 DS_RAM_Read(u8 Addr, u8 *Dat, u8 Len)
{
	u8 Ret = 0;      // 返回值
	u8 xC = Addr;    // 循环计数
	
	if (Addr < 31)       // 地址合法
	{
		Addr *= 2;       // 地址偶数转换
		Addr += 0xC1;    // 命令字计算
		
		DS_RST_SET(TRUE);    // 拉高复位线
		
		for (; Ret < Len && xC < 31; xC++)    // 循环读取数据
		{
			Send_Byte(Addr + Ret * 2);        // 发送命令字
			Dat[Ret] = Read_Byte();           // 数据读取
			Ret++;                            // 读取计数
		}
	}
	
	DS_RST_SET(FALSE);    // 拉低复位线
	
	return Ret;           // 函数返回
}

/****************************************************************************************************
 * 描述：发送单字节数据
 *
 * 参数：Dat = 要发送的数据
 *
 * 返回：无
****************************************************************************************************/

void Send_Byte(u8 Dat)
{
	u8 xC;        // 循环计数
	
	for (xC = 0; xC < 8; xC++)       // 遍历所有位
	{
		DS_SCL_SET(FALSE);              // 拉低时钟线
		DS_SDA_SET(Dat & (1 << xC));    // 设置数据线
		DS_SCL_SET(TRUE);               // 拉高时钟线
	}
}

/****************************************************************************************************
 * 描述：读取单字节数据
 *
 * 参数：无
 *
 * 返回：读取的数据
****************************************************************************************************/

u8 Read_Byte(void)
{
	u8 xC, Dat = 0;      // 临时数据
	
	for (xC = 0; xC < 8; xC++)    // 遍历所有位
	{
		DS_SCL_SET(FALSE);        // 拉低时钟线
		
		if (DS_SDA_GET())         // 当前位为高
		{
			Dat |= 1 << xC;       // 设置当前位
		}
		
		DS_SCL_SET(TRUE);         // 拉高时钟线
	}
	
	return Dat;    // 返回读取数据
}


/* End Of File */
