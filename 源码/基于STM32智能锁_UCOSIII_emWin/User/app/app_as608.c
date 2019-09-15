/****************************************************************************************************
 * 描述：指纹应用程序实现
 *
 * 作者：龙泽润
 *
 * 版本：v1.0.0    日期：2018-07-29
 *                                                                              大连智海科技有限公司
****************************************************************************************************/

#include "stm32f10x_tim.h"
#include "app_as608.h"
#include "drv_as608.h"
#include "includes.h"

/****************************************************************************************************
 * 描述：刷指纹实现
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

u8 ERR_Count = 0;     // 错误计数

void press_FR(void)
{ 
    OS_ERR err;
    struct rtc_time time;
	SearchResult seach;
	u8 ensure;
    /* 获取当前时间 */
    RTC_TimeCovr(&time);
	ensure=PS_GetImage();
	if(ensure==0x00)//获取图像成功 
	{	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //生成特征成功
		{		
			ensure=PS_HighSpeedSearch(CharBuffer1,0,300,&seach);
			
			if(ensure==0x00)//搜索成功
			{				
                ERR_Count = 0;    // 错误计数清零
                printf("指纹: %d 成功开锁-%d_%d_%d_%d\n", seach.pageID,
                        time.tm_mon,time.tm_mday, time.tm_hour, time.tm_min);    // 上报成功开锁的指纹编号和时间
                /* 发送任务信号量给开门任务 */
                OSTaskSemPost(&AppTaskLockTCB,
                             (OS_OPT   )OS_OPT_POST_NONE,
                             (OS_ERR  *)&err);
			}
			else
            {
                ERR_Count++;            // 错误计数
                printf("指纹开锁第%d次失败-%d_%d_%d_%d\n", ERR_Count, time.tm_mon, 
                                          time.tm_mday, time.tm_hour, time.tm_min);    // 上报指纹开锁失败时间和次数
                ShowErrMessage();	    // 显示错误信息
                if (ERR_Count >= ERR_MAX)
                {
                    OSTaskSemPost(&AppTaskShotTCB,
                              (OS_OPT   )OS_OPT_POST_NONE,
                              (OS_ERR  *)&err);
                }
            }				
		}
		else
        {
            ERR_Count++;            // 错误计数
            printf("指纹开锁第%d次失败-%d_%d_%d_%d\n", ERR_Count, time.tm_mon, time.tm_mday,
                                                                  time.tm_hour, time.tm_min);    // 上报指纹开锁失败时间和次数
            ShowErrMessage();	    // 显示错误信息
            if (ERR_Count >= ERR_MAX)
            {
                OSTaskSemPost(&AppTaskShotTCB,
                          (OS_OPT   )OS_OPT_POST_NONE,
                          (OS_ERR  *)&err);
            }
        }
	}
    else
    {
        ERR_Count++;            // 错误计数
        printf("指纹开锁第%d次失败-%d_%d_%d_%d\n", ERR_Count, time.tm_mon, time.tm_mday,
                                                             time.tm_hour, time.tm_min);    // 上报指纹开锁失败时间和次数
        ShowErrMessage();	    // 显示错误信息
        if (ERR_Count >= ERR_MAX)
        {
            OSTaskSemPost(&AppTaskShotTCB,
                (OS_OPT   )OS_OPT_POST_NONE,
                (OS_ERR  *)&err);
        }
    }
		
}

/****************************************************************************************************
 * 描述：显示确认码错误信息
 *
 * 参数：ensure：错误码
 *
 * 返回：无
****************************************************************************************************/

void ShowErrMessage(void)
{
    /* 开锁失败 */
    OLED_ShowCHinese(0, 2, 2, 1);
    OLED_ShowCHinese(16, 2, 3, 1);
    OLED_ShowCHinese(32, 2, 4, 1);
    OLED_ShowCHinese(48, 2, 5, 1);
}

/****************************************************************************************************
 * 描述：门外显示提示信息
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

void ShowRightMessage(void)
{
    /* 开锁成功 */
    OLED_ShowCHinese(0, 2, 2, 1);
    OLED_ShowCHinese(16, 2, 3, 1);
    OLED_ShowCHinese(32, 2, 0, 1);
    OLED_ShowCHinese(48, 2, 1, 1);
}

/****************************************************************************************************
 * 描述：获取空闲ID号
 *
 * 参数：无
 *
 * 返回：空闲 ID
****************************************************************************************************/

u8 AS608_GET_Idle_ID(void)
{
    u32 ID_Bufff[7];    // ID 缓冲区
    u8 Idle_ID = 0;
    u8 xC = 0;
    u8 yC = 0;
    
    SPI_FLASH_BufferRead((uint8_t *)ID_Bufff, FingerprintNumber_Addr, 7*4);    // 读全部ID
    
    for(xC=0; xC<7; xC++)    // 遍历数组
    {
        for(yC=0; yC<32; yC++)    // 遍历全部每一位
        {
            if (!((ID_Bufff[xC] >> yC) & 1))    // 空闲 ID
            {
                Idle_ID = xC * 32 + yC;         // 计算空闲 ID 号
                return Idle_ID;                 // 返回空闲 ID 号
            }
        }
    }
    
    return 0;    // 没有空闲 ID 返回第一个
}

/****************************************************************************************************
 * 描述：获取最后一个ID号
 *
 * 参数：无
 *
 * 返回：空闲 ID
****************************************************************************************************/

u16 AS608_GET_Last_ID(void)
{
    u32 ID_Bufff[7];    // ID 缓冲区
    u8 Last_ID = 0;
    u8 xC = 0;
    u8 yC = 0;
    
    SPI_FLASH_BufferRead((uint8_t *)ID_Bufff, FingerprintNumber_Addr, 7*4);    // 读全部ID
    
    for(xC=0; xC<7; xC++)    // 遍历数组
    {
        for(yC=0; yC<32; yC++)    // 遍历全部每一位
        {
            if (((ID_Bufff[xC] >> yC) & 1))    // 空闲 ID
            {
                Last_ID = xC * 32 + yC;         // 计算有效 ID 号
            }
        }
    }
    
    return Last_ID;    // 没有空闲 ID 返回第一个
}

/****************************************************************************************************
 * 描述：清除全部 ID 号
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

void AS608_Clear_All_ID(void)
{
    u32 ID_Bufff[7];    // ID 缓冲区
    
    memset(ID_Bufff, 0, 7*4);    // 初始化数组为 0
    
    SPI_FLASH_SectorErase(FingerprintNumber_Addr);                 // 擦除 ID 使用的扇区
    
    SPI_FLASH_BufferWrite((uint8_t *)ID_Bufff, FingerprintNumber_Addr, 7*4);    // 清除全部 ID
}

/****************************************************************************************************
 * 描述：设置 ID 号
 *
 * 参数：ID；要设置的 ID 号
 *
 * 返回：无
****************************************************************************************************/

void AS608_SET_ID(u16 ID)
{
    u32 ID_Bufff[7];    // ID 缓冲区
    u8 xC = 0;
    u8 yC = 0;
    
    SPI_FLASH_BufferRead((uint8_t *)ID_Bufff, FingerprintNumber_Addr, 7*4);    // 读全部ID

    xC = ID / 32;
    yC = ID % 32;
    ID_Bufff[xC] |= 1 << yC;                                       // 设置一个 ID
    
    SPI_FLASH_SectorErase(FingerprintNumber_Addr);                 // 擦除 ID 使用的扇区
    
    SPI_FLASH_BufferWrite((uint8_t *)ID_Bufff, FingerprintNumber_Addr, 7*4);    // 清除全部 ID
}

/****************************************************************************************************
 * 描述：删除 ID 号
 *
 * 参数：ID：要删除的 ID 号
 *
 * 返回：无
****************************************************************************************************/

void AS608_Del_ID(u16 ID)
{
    u32 ID_Bufff[7];    // ID 缓冲区
    u8 xC = 0;
    u8 yC = 0;
    
    SPI_FLASH_BufferRead((uint8_t *)ID_Bufff, FingerprintNumber_Addr, 7*4);    // 读全部ID

    xC = ID / 32;
    yC = ID % 32;
    ID_Bufff[xC] &= ~(1 << yC);                                       // 设置一个 ID
    
    SPI_FLASH_SectorErase(FingerprintNumber_Addr);                 // 擦除 ID 使用的扇区
    
    SPI_FLASH_BufferWrite((uint8_t *)ID_Bufff, FingerprintNumber_Addr, 7*4);    // 清除全部 ID
}


/* End Of File */
