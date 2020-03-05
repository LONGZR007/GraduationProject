/*
******************************************************************************
* @file    bsp_led.c
* @author  Ұ��
* @version V1.0
* @date    2015-xx-xx
* @brief   ledӦ�ú����ӿ�
******************************************************************************
* @attention
*
* ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
* ��̳    :http://www.firebbs.cn
* �Ա�    :https://fire-stm32.taobao.com
*
******************************************************************************
*/
  
#include "..\User\bsp\led\bsp_led.h"     

 /**
  * @brief  ��ʼ������LED��IO
  * @param  ��
  * @retval ��
  */
void LED_GPIO_Config(void)
{		
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*����GPIOB��GPIOF������ʱ��*/
		RCC_APB2PeriphClockCmd( macLED1_GPIO_CLK, ENABLE); 
    RCC_APB2PeriphClockCmd( macLED_GPIO_CLK, ENABLE); 

		/*ѡ��Ҫ���Ƶ�GPIOB����*/															   
		GPIO_InitStructure.GPIO_Pin = macLED1_GPIO_PIN;	

		/*��������ģʽΪͨ���������*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*������������Ϊ50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*���ÿ⺯������ʼ��GPIOB0*/
		GPIO_Init(macLED1_GPIO_PORT, &GPIO_InitStructure);	
  
    /*ѡ��Ҫ���Ƶ�GPIOB����*/															   
		GPIO_InitStructure.GPIO_Pin = macLED_GPIO_PIN;	
  
    /*���ÿ⺯������ʼ��GPIOB0*/
		GPIO_Init(macLED_GPIO_PORT, &GPIO_InitStructure);
    
    LED_OFF;
}

  

/*********************************************END OF FILE**********************/
