/*
******************************************************************************
* @file    bsp_led.c
* @author  野火
* @version V1.0
* @date    2015-xx-xx
* @brief   led应用函数接口
******************************************************************************
* @attention
*
* 实验平台:野火 iSO STM32 开发板 
* 论坛    :http://www.firebbs.cn
* 淘宝    :https://fire-stm32.taobao.com
*
******************************************************************************
*/
/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                           MASTER INCLUDES
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                         Evaluation Board
*
* Filename      : includes.h
* Version       : V1.00
* Programmer(s) : EHS
*                 DC
*********************************************************************************************************
*/

#ifndef  INCLUDES_PRESENT
#define  INCLUDES_PRESENT

//#ifndef SOFTRESET
//#define SOFTRESET
//#endif
#define CALADD  510*4096

/*
*********************************************************************************************************
*                                         STANDARD LIBRARIES
*********************************************************************************************************
*/

#include  <stdarg.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <math.h>
#include  <string.h>


/*
*********************************************************************************************************
*                                              LIBRARIES
*********************************************************************************************************
*/

#include  <cpu.h>
#include  <lib_def.h>
#include  <lib_ascii.h>
#include  <lib_math.h>
#include  <lib_mem.h>
#include  <lib_str.h>


/*
*********************************************************************************************************
*                                              APP / BSP
*********************************************************************************************************
*/
#include  "..\User\bsp\bsp.h"
#include  "..\User\bsp\led\bsp_led.h"  
#include  "..\User\bsp\Key\key.h"
#include  "..\User\bsp\Beep\beep.h" 
#include  "..\User\bsp\TouchPad\TouchPad.h"
#include  "..\User\bsp\spi_flash\fatfs_flash_spi.h"
#include  "..\User\bsp\usart\bsp_usart1.h"
#include  "..\User\bsp\lcd\bsp_ili9341_lcd.h"
#include  "..\User\bsp\lcd\bsp_xpt2046_lcd.h"
#include  "..\User\bsp\ExSRAM\bsp_fsmc_sram.h"
#include  "..\User\bsp\rtc\bsp_date.h"
#include  "..\User\bsp\rtc\bsp_rtc.h"
#include  "..\User\bsp\ov7725\bsp_ov7725.h"
#include  "..\User\bsp\sdcard\bsp_sdio_sdcard.h"
#include  "..\User\bsp\ESP8266\bsp_esp8266.h"
#include  "drv_ds1302.h"
#include  "drv_as608.h"
#include  "app_as608.h"
#include  "ff.h"
#include  "drv_oled.h"
#include  "app_cfg.h"
#include  "app.h"
#include  <cm_backtrace.h>
/*
*********************************************************************************************************
*                                                 OS
*********************************************************************************************************
*/

#include  <os.h>	
#include "os_type.h"
#include "os_cfg_app.h"
/*
*********************************************************************************************************
*                                                 ST
*********************************************************************************************************
*/

#include  <stm32f10x.h>

/*
*********************************************************************************************************
*                                               emWin
*********************************************************************************************************
*/
#include "GUI.H"
#include "DIALOG.h"
#include "common.h"

extern OS_Q queue;                             // 声明消息队列

extern  OS_TCB   AppTaskLockTCB;             // 开锁任务控制块
//extern  OS_SEM   SEM_Lock;
extern  OS_TCB   AppTaskBellTCB;
extern  OS_TCB   AppTaskBrushTCB;
extern  OS_TCB   AppTaskShotTCB;
extern  OS_TCB   AppTaskWiFiTCB;

/*
*********************************************************************************************************
*                                               emWin
*********************************************************************************************************
*/

#define AdministratorPassword_Addr    (4095*4096)
#define OpenPassword_Addr             (4095*4096 + 7)
#define FingerprintNumber_Addr        (4094*4096)
#define FingerprintNumber_Addr        (4094*4096)
#define TemporaryPassword_Addr        (4093*4096)
/*
*********************************************************************************************************
*                                              FUNCTION
*********************************************************************************************************
*/
void Touch_MainTask(void);
void bsp_DelayUS(uint32_t _ulDelayTime);
void Soft_Reset(void);
/*
*********************************************************************************************************
*                                            INCLUDES END
*********************************************************************************************************
*/


#endif

