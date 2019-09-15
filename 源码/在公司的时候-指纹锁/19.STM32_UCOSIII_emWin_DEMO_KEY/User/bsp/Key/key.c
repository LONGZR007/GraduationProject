/**
  ******************************************************************************
  * @file    bsp_exti.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   I/O���ж�Ӧ��bsp
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./BSP/Key/key.h"
#include "includes.h"
#include "app_as608.h"

 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* �����ж�Դ������1 */
  NVIC_InitStructure.NVIC_IRQChannel = Open_Bell_INT_EXTI_IRQ;
  /* ������ռ���ȼ� */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* ���������ȼ� */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* ʹ���ж�ͨ�� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  ���� IOΪEXTI�жϿڣ��������ж����ȼ�
  * @param  ��
  * @retval ��
  */
void EXTI_Key_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/*��������GPIO�ڵ�ʱ��*/
	RCC_APB2PeriphClockCmd(Open_INT_GPIO_CLK,ENABLE);
    RCC_APB2PeriphClockCmd(Bell_INT_GPIO_CLK,ENABLE);
												
	/* ���� NVIC �ж�*/
	NVIC_Configuration();
	
/*--------------------------KEY1����-----------------------------*/
	/* ѡ�񰴼��õ���GPIO */	
  GPIO_InitStructure.GPIO_Pin = Bell_INT_GPIO_PIN;
  /* ����Ϊ�������� */	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(Bell_INT_GPIO_PORT, &GPIO_InitStructure);

	/* ѡ��EXTI���ź�Դ */
  GPIO_EXTILineConfig(Bell_INT_EXTI_PORTSOURCE, Bell_INT_EXTI_PINSOURCE); 
  EXTI_InitStructure.EXTI_Line = Bell_INT_EXTI_LINE;
	
	/* EXTIΪ�ж�ģʽ */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	/* �������ж� */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  /* ʹ���ж� */	
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
  /*--------------------------KEY2����-----------------------------*/
	/* ѡ�񰴼��õ���GPIO */	
  GPIO_InitStructure.GPIO_Pin = Open_INT_GPIO_PIN;
  /* ����Ϊ�������� */	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(Open_INT_GPIO_PORT, &GPIO_InitStructure);

	/* ѡ��EXTI���ź�Դ */
  GPIO_EXTILineConfig(Open_INT_EXTI_PORTSOURCE, Open_INT_EXTI_PINSOURCE); 
  EXTI_InitStructure.EXTI_Line = Open_INT_EXTI_LINE;
	
	/* EXTIΪ�ж�ģʽ */
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	/* �½����ж� */
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  /* ʹ���ж� */	
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}

 /**
  * @brief  ���� ���󰴼�
  * @param  ��
  * @retval ��
  */

void Matrix_Buttons_Init(void)
{
    /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /*����GPIOB��GPIOF������ʱ��*/
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOG, ENABLE); 
														   
    /* ���ģʽ���� */ 
    GPIO_InitStructure.GPIO_Mode =GPIO_Mode_Out_PP;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;     
    
    /* �� 1 �������� */
    
    GPIO_InitStructure.GPIO_Pin = Line1_GPIO_PIN;	
    GPIO_Init(Line1_GPIO_PORT, &GPIO_InitStructure);

        
    /* �� 2 �������� */        
    GPIO_InitStructure.GPIO_Pin = Line2_GPIO_PIN;	
    GPIO_Init(Line2_GPIO_PORT, &GPIO_InitStructure);


    /* �� 3 �������� */ 
    GPIO_InitStructure.GPIO_Pin = Line3_GPIO_PIN;	
    GPIO_Init(Line3_GPIO_PORT, &GPIO_InitStructure);

    
    /* ����ģʽ���� */ 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 

    /* ���������� */

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
  * @brief  ɨ����󰴼�
  * @param  ��
  * @retval ���ؼ�ֵ
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

/* ��ȡ�������� */
void Get_Open_Code(u8 *Buff)
{
    SPI_FLASH_BufferRead((u8 *)Buff, OpenPassword_Addr, 7);
//    printf("kai men %s\n", Buff);
}

/* ��ȡ��ʱ�������� */
void Get_Open_TempCode(u8 *Buff)
{
    SPI_FLASH_BufferRead((u8 *)Buff, TemporaryPassword_Addr, 8);
//    printf("kai men %s\n", Buff);
}

/* ɾ����ʱ�������� */
void Del_Open_TempCode(void)
{
    SPI_FLASH_SectorErase(AdministratorPassword_Addr);                             // ��������
    SPI_FLASH_BufferWrite(0, TemporaryPassword_Addr, 1);     // д����ʱ����
}


void PasswordUnlock(void)
{
    OS_ERR err;
    u8 ButtVal = 0;
    u8 Buff[8];
    struct rtc_time time;
    static char Input_Val[17]={'\0'};
    static u8 xC = 0;
    
    
    ButtVal = Matrix_Buttons_Scan();    // ɨ��������
        
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
            RTC_TimeCovr(&time);    // ��ȡ��ǰʱ��
            Get_Open_Code(Buff);    // ����������
            
            if (strstr(Input_Val, (char *)Buff))
            {
                printf("���뿪���ɹ�-%d_%d_%d_%d\n", time.tm_mon, 
                            time.tm_mday, time.tm_hour, time.tm_min);    // �ϱ�ָ�ƿ���ʧ��ʱ��ʹ���
                OSTaskSemPost(&AppTaskLockTCB,
                              (OS_OPT   )OS_OPT_POST_NONE,
                              (OS_ERR  *)&err);
                ERR_Count = 0;    // ����������
            }
            
            Get_Open_TempCode(Buff);    // ����������
            if (Buff[0] == 1 && strstr(Input_Val, (char *)&Buff[1]))
            {
                printf("��ʱ���뿪���ɹ�-%d_%d_%d_%d\n", time.tm_mon, 
                            time.tm_mday, time.tm_hour, time.tm_min);    // �ϱ�ָ�ƿ���ʧ��ʱ��ʹ���
                Del_Open_TempCode();    // �ɹ�������ɾ����ʱ����
                OSTaskSemPost(&AppTaskLockTCB,
                              (OS_OPT   )OS_OPT_POST_NONE,
                              (OS_ERR  *)&err);
                ERR_Count = 0;    // ����������
            }
            else
            {
                /* ���� */
                OLED_Clear();
                ShowErrMessage();	    // ��ʾ������Ϣ
                xC = 0;
                ERR_Count++;            // �������
                printf("���뿪����%d��ʧ��-%d_%d_%d_%d\n", ERR_Count, time.tm_mon, 
                                          time.tm_mday, time.tm_hour, time.tm_min);    // �ϱ�ָ�ƿ���ʧ��ʱ��ʹ���
                /* �ﵽ�����������ץ�� */
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
                Input_Val[xC] = 48;    // �ַ� 0
            }
            else
            {
                Input_Val[xC] = ButtVal + 48;    // ����һ���ַ�
            }
            xC++;
            xC = xC % 17;
            Input_Val[xC] = '\0';
        }
        
        /* ���� */
        OLED_Clear();
        OLED_ShowString(0, 2, (u8 *)Input_Val, 16, 1);
    }
}


/*********************************************END OF FILE**********************/
