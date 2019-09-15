/****************************************************************************************************
 * 描述：驱动层配置文件
 *
 * 作者：Gavin
 *
 * 版本：v1.0.0    日期：2018-04-12
 *                                                                              大连智海科技有限公司
****************************************************************************************************/

#ifndef DRV_CFG_H
#define DRV_CFG_H


/* 操作系统支持, 1 = 启用, 0 = 禁用 */

#define RTOS    0

#if RTOS
#include "ucos_ii.h"    // 使用 uC/OS-II 系统
#endif

/* 芯片类型定义 */

#if !defined (STM32F10X_LD) && !defined (STM32F10X_LD_VL) && \
	!defined (STM32F10X_MD) && !defined (STM32F10X_MD_VL) && \
	!defined (STM32F10X_HD) && !defined (STM32F10X_HD_VL) && \
	!defined (STM32F10X_XL) && !defined (STM32F10X_CL) 

//#define STM32F10X_LD       // STM32F101/102/103    Flash =  16K And   32K
//#define STM32F10X_LD_VL    // STM32F100            Flash =  16K And   32K
  #define STM32F10X_MD       // STM32F101/102/103    Flash =  64K And  128K
//#define STM32F10X_MD_VL    // STM32F100            Flash =  64K And  128K
//#define STM32F10X_HD       // STM32F101/103        Flash = 256K And  512K
//#define STM32F10X_HD_VL    // STM32F100            Flash = 256K And  512K
//#define STM32F10X_XL       // STM32F101/103        Flash = 768K And 1024K
//#define STM32F10X_CL       // STM32F105/107
#endif

/* 时钟参数定义 */

#ifdef STM32F10X_CL
#define HSE_FREQ    25000000    // 外部高速时钟频率, 单位：Hz
#else
#define HSE_FREQ     8000000    // 外部高速时钟频率, 单位：Hz
#endif
#define LSE_FREQ       32768    // 外部低速时钟频率, 单位：Hz

/* 每秒滴答数定义 */

#if RTOS
#define TICKS_PER_SEC    OS_TICKS_PER_SEC    // 使用系统滴答数设置
#else
#define TICKS_PER_SEC    1000                // 每秒 1000 个滴答(定时器单位：毫秒)
#endif

/* 延时函数定义 */

#if RTOS
#define Delay_ms(val)      OSTimeDlyHMSM(0, 0, 0, val);    // 延时函数, 单位：毫秒
#define Delay_sec(val)     OSTimeDlyHMSM(0, 0, val, 0);    // 延时函数, 单位：秒
#define Delay_min(val)     OSTimeDlyHMSM(0, val, 0, 0);    // 延时函数, 单位：分钟
#define Delay_hour(val)    OSTimeDlyHMSM(val, 0, 0, 0);    // 延时函数, 单位：小时
#else
#define Delay_ms(val)      SysTick_Delay(val)              // 延时函数, 单位：毫秒
#endif

/* 系统中断、节拍处理函数定义 */

#if RTOS
#define OS_IntEnter()    OSIntEnter()    // 进入中断
#define OS_IntExit()     OSIntExit()     // 退出中断
#define OS_TimeTick()    OSTimeTick()    // 系统时间滴答
#endif

/* I2C 接口参数定义 */

#define I2C1_EN    0    // I2C1 接口启用设置, 1 = 启用, 0 = 禁用
#define I2C1_RM    0    // I2C1 重映射设置, 1 = 管脚重映射, 0 = 默认管脚
#define I2C2_EN    0    // I2C2 接口启用设置, 1 = 启用, 0 = 禁用




#endif /* DRV_CFG_H */

/* End Of File */
