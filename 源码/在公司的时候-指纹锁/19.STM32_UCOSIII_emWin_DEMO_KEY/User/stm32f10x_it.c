/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTI
  
  AL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "includes.h"

extern uint8_t Ov7725_vsync;

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
//void HardFault_Handler(void)
//{
//	printf("HardFault!!!");
//#ifdef SOFTRESET
//	Soft_Reset();
//#endif
//  /* Go to infinite loop when Hard Fault exception occurs */
//  while (1)
//  {
//  }
//}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

void Open_Bell_IRQHandler(void)
{
  OS_ERR      err;
  
  OSIntEnter();    // 进入中断
  
    /* 一键开门按钮*/
	if(EXTI_GetITStatus(Open_INT_EXTI_LINE) != RESET) 
	{
		// 开门	
        OSTaskSemPost(&AppTaskLockTCB,
          (OS_OPT   )OS_OPT_POST_NONE,
          (OS_ERR  *)&err);
    
        //清除中断标志位
        EXTI_ClearITPendingBit(Open_INT_EXTI_LINE);     
	}  
  
    /* 门铃按键 */
	if(EXTI_GetITStatus(Bell_INT_EXTI_LINE) != RESET) 
	{
		// 门铃	
        OSTaskSemPost(&AppTaskBellTCB,
          (OS_OPT   )OS_OPT_POST_NONE,
          (OS_ERR  *)&err);
    
        //清除中断标志位
        EXTI_ClearITPendingBit(Bell_INT_EXTI_LINE);     
	}
    
  OSIntExit();    // 退出中断
}


/* 有手指按下中断 */

void PS_Sta_IRQHandler(void)
{
    OS_ERR      err;

    OSIntEnter();    // 进入中断

    /* 开门指纹按下*/
    if(EXTI_GetITStatus(PS_Sta_PIN) != RESET) 
    {
        // 开门	
        OSTaskSemPost(&AppTaskBrushTCB,
           (OS_OPT   )OS_OPT_POST_NONE,
           (OS_ERR  *)&err);

        //清除中断标志位
        EXTI_ClearITPendingBit(PS_Sta_PIN);     
    }  

    OSIntExit();    // 退出中断
}
                     
void macOV7725_VSYNC_EXTI_INT_FUNCTION(void)
{
    OSIntEnter();    // 进入中断
    
    /* 场中断 */
    if ( EXTI_GetITStatus(macOV7725_VSYNC_EXTI_LINE) != RESET ) 	//检查EXTI_Line0线路上的中断请求是否发送到了NVIC 
	{
        if( Ov7725_vsync == 0 )
        {
            FIFO_WRST_L(); 	                      //拉低使FIFO写(数据from摄像头)指针复位
            FIFO_WE_H();	                        //拉高使FIFO写允许

            Ov7725_vsync = 1;	   	
            FIFO_WE_H();                          //使FIFO写允许
            FIFO_WRST_H();                        //允许使FIFO写(数据from摄像头)指针运动
        }
        else if( Ov7725_vsync == 1 )
        {
            FIFO_WE_L();                          //拉低使FIFO写暂停
            Ov7725_vsync = 2;
        }        
        EXTI_ClearITPendingBit(macOV7725_VSYNC_EXTI_LINE);		    //清除EXTI_Line0线路挂起标志位
	}
    
    OSIntExit();    // 退出中断
}

/*
 * 函数名：SDIO_IRQHandler
 * 描述  ：在SDIO_ITConfig(）这个函数开启了sdio中断	，
 *		     数据传输结束时产生中断
 * 输入  ：无		 
 * 输出  ：无
 */
void SDIO_IRQHandler(void) 
{
	CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();  
	OSIntNestingCtr++;
	CPU_CRITICAL_EXIT();
	
    /* Process All SDIO Interrupt Sources */
    SD_ProcessIRQSrc();
	
	/* 在os_core.c文件里定义,如果有更高优先级的任务就绪了,则执行一次任务切换 */
	OSIntExit();
}


void SD_SDIO_DMA_IRQHANDLER(void)
{
    CPU_SR_ALLOC();

	CPU_CRITICAL_ENTER();  
	OSIntNestingCtr++;
	CPU_CRITICAL_EXIT();
	
    /* Process DMA2 Stream3 or DMA2 Stream6 Interrupt Sources */
    SD_ProcessDMAIRQ();
	
	/* 在os_core.c文件里定义,如果有更高优先级的任务就绪了,则执行一次任务切换 */
	OSIntExit();
}




/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
