/**
  ******************************************************************************
  * @file    bsp_exti.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   I/O线中断应用bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./BSP/Key/key.h"
#include "includes.h"
#include "app_as608.h"

 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 配置中断源：按键1 */
  NVIC_InitStructure.NVIC_IRQChannel = Open_Bell_INT_EXTI_IRQ;
  /* 配置抢占优先级 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 配置子优先级 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* 使能中断通道 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  配置 IO为EXTI中断口，并设置中断优先级
  * @param  无
  * @retval 无
  */
void EXTI_Key_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/*开启按键GPIO口的时钟*/
	RCC_APB2PeriphClockCmd(Open_INT_GPIO_CLK,ENABLE);
    RCC_APB2PeriphClockCmd(Bell_INT_GPIO_CLK,ENABLE);
												
	/* 配置 NVIC 中断*/
	NVIC_Configuration();
	
/*--------------------------KEY1配置-----------------------------*/
	/* 选择按键用到的GPIO */	
  GPIO_InitStructure.GPIO_Pin = Bell_INT_GPIO_PIN;
  /* 配置为浮空输入 */	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(Bell_INT_GPIO_PORT, &GPIO_InitStructure);

	/* 选择EXTI的信号源 */
  GPIO_EXTILineConfig(Bell_INT_EXTI_PORTSOURCE, Bell_INT_EXTI_PINSOURCE); 
  EXTI_InitStructure.EXTI_Line = Bell_INT_EXTI_LINE;
	
	/* EXTI为中断模式 */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	/* 上升沿中断 */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  /* 使能中断 */	
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
  /*--------------------------KEY2配置-----------------------------*/
	/* 选择按键用到的GPIO */	
  GPIO_InitStructure.GPIO_Pin = Open_INT_GPIO_PIN;
  /* 配置为浮空输入 */	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(Open_INT_GPIO_PORT, &GPIO_InitStructure);

	/* 选择EXTI的信号源 */
  GPIO_EXTILineConfig(Open_INT_EXTI_PORTSOURCE, Open_INT_EXTI_PINSOURCE); 
  EXTI_InitStructure.EXTI_Line = Open_INT_EXTI_LINE;
	
	/* EXTI为中断模式 */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	/* 下降沿中断 */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  /* 使能中断 */	
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}

 /**
  * @brief  配置 矩阵按键
  * @param  无
  * @retval 无
  */

void Matrix_Buttons_Init(void)
{
    /*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /*开启GPIOB和GPIOF的外设时钟*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOG, ENABLE); 
														   
    /* 输出模式配置 */ 
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;     
    
    /* 列 1 配置输入 */
    
    GPIO_InitStructure.GPIO_Pin = Line1_GPIO_PIN;	
    GPIO_Init(Line1_GPIO_PORT, &GPIO_InitStructure);

        
    /* 列 2 配置输入 */        
    GPIO_InitStructure.GPIO_Pin = Line2_GPIO_PIN;	
    GPIO_Init(Line2_GPIO_PORT, &GPIO_InitStructure);


    /* 列 3 配置输入 */ 
    GPIO_InitStructure.GPIO_Pin = Line3_GPIO_PIN;	
    GPIO_Init(Line3_GPIO_PORT, &GPIO_InitStructure);

    
    /* 输入模式配置 */ 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 

    /* 行配置输入 */

    GPIO_InitStructure.GPIO_Pin = Row1_GPIO_PIN;	
    GPIO_Init(Row1_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = Row2_GPIO_PIN;	
    GPIO_Init(Row2_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = Row3_GPIO_PIN;	
    GPIO_Init(Row3_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = Row4_GPIO_PIN;	
    GPIO_Init(Row4_GPIO_PORT, &GPIO_InitStructure);
}

u16 Read_PIN(void)
{
    return GPIOG->IDR;
}

 /**
  * @brief  扫描矩阵按键
  * @param  无
  * @retval 返回键值
  */

u8 Matrix_Buttons_Scan(void)
{
	u8 xC = 0;
    u8 yC = 0;
    u16 DR = 0;
    static u8 Matrix_Buttons[4][3];
    u8 NUM[4][3] = {{1,   2,  3},
                    {4,   5,  6},
                    {7,   8,  9},
                    {10, 11, 12}};
    
    for(xC=0; xC<3; xC++)
    {
        DR = Line1_GPIO_PORT->ODR;
        DR = DR & (~(7 << 7));
        Line1_GPIO_PORT->ODR = DR | (1 << (7 + xC));
        
        DR = Read_PIN();
        
        if ((DR >> 9) & 1)
        {
            Matrix_Buttons[0][xC]++;
        }
        
        if ((DR >> 10) & 1)
        {
            Matrix_Buttons[1][xC]++;
        }
        
        if ((DR >> 13) & 1)
        {
            Matrix_Buttons[2][xC]++;
        }
        
        if ((DR >> 14) & 1)
        {
            Matrix_Buttons[3][xC]++;
        }
    }
    
    for(yC=0; yC<4; yC++)
    {
        for(xC=0; xC<3; xC++)
        {
            if (Matrix_Buttons[yC][xC] > 10)
            {
                Matrix_Buttons[yC][xC] = 0;
                
                return NUM[yC][xC];
            }
        }
    }
    
    return 0;
}

/* 获取开门密码 */
void Get_Open_Code(u8 *Buff)
{
    SPI_FLASH_BufferRead((u8 *)Buff, OpenPassword_Addr, 7);
//    printf("kai men %s\n", Buff);
}

/* 获取临时开门密码 */
void Get_Open_TempCode(u8 *Buff)
{
    SPI_FLASH_BufferRead((u8 *)Buff, TemporaryPassword_Addr, 8);
//    printf("kai men %s\n", Buff);
}

/* 删除临时开门密码 */
void Del_Open_TempCode(void)
{
    SPI_FLASH_SectorErase(AdministratorPassword_Addr);                             // 擦除扇区
    SPI_FLASH_BufferWrite(0, TemporaryPassword_Addr, 1);     // 写入临时密码
}


void PasswordUnlock(void)
{
    OS_ERR err;
    u8 ButtVal = 0;
    u8 Buff[8];
    struct rtc_time time;
    static char Input_Val[17]={'\0'};
    static u8 xC = 0;
    
    
    ButtVal = Matrix_Buttons_Scan();    // 扫描矩阵键盘
        
    if (ButtVal)
    {
        if (ButtVal == 10)
        {
            if (xC)
            {
                xC--;
                Input_Val[xC] = '\0';
            }
        }
        else if (ButtVal == 12)
        {
            RTC_TimeCovr(&time);    // 获取当前时间
            Get_Open_Code(Buff);    // 读开门密码
            
            if (strstr(Input_Val, (char *)Buff))
            {
                printf("密码开锁成功-%d_%d_%d_%d\n", time.tm_mon, 
                            time.tm_mday, time.tm_hour, time.tm_min);    // 上报指纹开锁失败时间和次数
                OSTaskSemPost(&AppTaskLockTCB,
                              (OS_OPT   )OS_OPT_POST_NONE,
                              (OS_ERR  *)&err);
                ERR_Count = 0;    // 清零错误次数
            }
            
            Get_Open_TempCode(Buff);    // 读开门密码
            if (Buff[0] == 1 && strstr(Input_Val, (char *)&Buff[1]))
            {
                printf("临时密码开锁成功-%d_%d_%d_%d\n", time.tm_mon, 
                            time.tm_mday, time.tm_hour, time.tm_min);    // 上报指纹开锁失败时间和次数
                Del_Open_TempCode();    // 成功开锁，删除临时密码
                OSTaskSemPost(&AppTaskLockTCB,
                              (OS_OPT   )OS_OPT_POST_NONE,
                              (OS_ERR  *)&err);
                ERR_Count = 0;    // 清零错误次数
            }
            else
            {
                /* 清屏 */
                OLED_Clear();
                ShowErrMessage();	    // 显示错误信息
                xC = 0;
                ERR_Count++;            // 错误计数
                printf("密码开锁第%d次失败-%d_%d_%d_%d\n", ERR_Count, time.tm_mon, 
                                          time.tm_mday, time.tm_hour, time.tm_min);    // 上报指纹开锁失败时间和次数
                /* 达到最大错误次数，抓拍 */
                if (ERR_Count >= ERR_MAX)
                {
                    OSTaskSemPost(&AppTaskShotTCB,
                              (OS_OPT   )OS_OPT_POST_NONE,
                              (OS_ERR  *)&err);
                }
                Input_Val[0] = '\0';
                return;
            }
            xC = 0;
            Input_Val[0] = '\0';
        }
        else
        {
            if (ButtVal == 11)
            {
                Input_Val[xC] = 48;    // 字符 0
            }
            else
            {
                Input_Val[xC] = ButtVal + 48;    // 保存一个字符
            }
            xC++;
            xC = xC % 17;
            Input_Val[xC] = '\0';
        }
        
        /* 清屏 */
        OLED_Clear();
        OLED_ShowString(0, 2, (u8 *)Input_Val, 16, 1);
    }
}


/*********************************************END OF FILE**********************/
