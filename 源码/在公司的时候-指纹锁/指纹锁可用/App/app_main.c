/****************************************************************************************************
 * 描述：应用程序入口实现
 *
 * 作者：Gavin
 *
 * 版本：v1.0.0    日期：2018-04-12
 *                                                                              大连智海科技有限公司
****************************************************************************************************/

#include "app_task.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_nvic.h"
#include "stm32f10x_systick.h"

/* 内部变量声明 */

#if RTOS
static OS_STK RT_STK[RT_SS];      // 实时任务堆栈
static OS_STK SEC_STK[SEC_SS];    // 1 秒周期任务堆栈
#else
#include "stm32f10x_rtc.h"
#endif

/****************************************************************************************************
 * 描述：应用程序入口函数
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

int main(void)
{
	Init_Task();    // 调用初始化任务
	
#if RTOS
	OSInit();                                                      // 操作系统初始化
	OSTaskCreate(RT_Task, NULL, &RT_STK[RT_SS - 1], RT_TP);        // 创建实时任务
	OSTaskCreate(SEC_Task, NULL, &SEC_STK[SEC_SS - 1], SEC_TP);    // 创建 1 秒周期任务
	OSStart();                                                     // 启动操作系统
#else
	while (1)
	{
		RT_Task(NULL);         // 调用实时任务
		
		if (RTC_SECF_GET())    // 秒中断
		{
			RTC_SECF_RST();    // 清除秒中断标志
			SEC_Task(NULL);    // 调用秒任务
		}
	}
}

#endif

/* End Of File */
