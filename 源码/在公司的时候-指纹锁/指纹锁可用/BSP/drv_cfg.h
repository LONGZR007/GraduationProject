/****************************************************************************************************
 * �����������������ļ�
 *
 * ���ߣ�Gavin
 *
 * �汾��v1.0.0    ���ڣ�2018-04-12
 *                                                                              �����Ǻ��Ƽ����޹�˾
****************************************************************************************************/

#ifndef DRV_CFG_H
#define DRV_CFG_H


/* ����ϵͳ֧��, 1 = ����, 0 = ���� */

#define RTOS    0

#if RTOS
#include "ucos_ii.h"    // ʹ�� uC/OS-II ϵͳ
#endif

/* оƬ���Ͷ��� */

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

/* ʱ�Ӳ������� */

#ifdef STM32F10X_CL
#define HSE_FREQ    25000000    // �ⲿ����ʱ��Ƶ��, ��λ��Hz
#else
#define HSE_FREQ     8000000    // �ⲿ����ʱ��Ƶ��, ��λ��Hz
#endif
#define LSE_FREQ       32768    // �ⲿ����ʱ��Ƶ��, ��λ��Hz

/* ÿ��δ������� */

#if RTOS
#define TICKS_PER_SEC    OS_TICKS_PER_SEC    // ʹ��ϵͳ�δ�������
#else
#define TICKS_PER_SEC    1000                // ÿ�� 1000 ���δ�(��ʱ����λ������)
#endif

/* ��ʱ�������� */

#if RTOS
#define Delay_ms(val)      OSTimeDlyHMSM(0, 0, 0, val);    // ��ʱ����, ��λ������
#define Delay_sec(val)     OSTimeDlyHMSM(0, 0, val, 0);    // ��ʱ����, ��λ����
#define Delay_min(val)     OSTimeDlyHMSM(0, val, 0, 0);    // ��ʱ����, ��λ������
#define Delay_hour(val)    OSTimeDlyHMSM(val, 0, 0, 0);    // ��ʱ����, ��λ��Сʱ
#else
#define Delay_ms(val)      SysTick_Delay(val)              // ��ʱ����, ��λ������
#endif

/* ϵͳ�жϡ����Ĵ��������� */

#if RTOS
#define OS_IntEnter()    OSIntEnter()    // �����ж�
#define OS_IntExit()     OSIntExit()     // �˳��ж�
#define OS_TimeTick()    OSTimeTick()    // ϵͳʱ��δ�
#endif

/* I2C �ӿڲ������� */

#define I2C1_EN    0    // I2C1 �ӿ���������, 1 = ����, 0 = ����
#define I2C1_RM    0    // I2C1 ��ӳ������, 1 = �ܽ���ӳ��, 0 = Ĭ�Ϲܽ�
#define I2C2_EN    0    // I2C2 �ӿ���������, 1 = ����, 0 = ����




#endif /* DRV_CFG_H */

/* End Of File */
