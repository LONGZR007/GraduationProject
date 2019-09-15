#ifndef __LED_H
#define	__LED_H


#include "stm32f10x.h"


/* ����LED���ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����Ĵ��뼴�ɸı���Ƶ�LED���� */
#define macLED1_GPIO_PORT    	GPIOD			              /* GPIO�˿� */
#define macLED1_GPIO_CLK 	    RCC_APB2Periph_GPIOD		/* GPIO�˿�ʱ�� */
#define macLED1_GPIO_PIN		  GPIO_Pin_11			          /* LED���ź� */

/* ����LED���ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����Ĵ��뼴�ɸı���Ƶ�LED���� */
#define macLED_GPIO_PORT    	GPIOE			              /* GPIO�˿� */
#define macLED_GPIO_CLK 	    RCC_APB2Periph_GPIOE		/* GPIO�˿�ʱ�� */
#define macLED_GPIO_PIN		    GPIO_Pin_6		          /* LED���ź� */


/* ���κ꣬��������������һ��ʹ�� */
#define LED1(a)	if (a)	\
					GPIO_SetBits(macLED1_GPIO_PORT,macLED1_GPIO_PIN);\
					else		\
					GPIO_ResetBits(macLED1_GPIO_PORT,macLED1_GPIO_PIN)



/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)			{p->BSRR=i;}			//����Ϊ�ߵ�ƽ		
#define digitalLo(p,i)			{p->BRR=i;}				//����͵�ƽ
#define digitalToggle(p,i)		{p->ODR ^=i;}			//�����ת״̬


/* �������IO�ĺ� */
#define LED1_TOGGLE		digitalToggle(macLED1_GPIO_PORT,macLED1_GPIO_PIN)
#define LED1_OFF		  digitalLo(macLED1_GPIO_PORT,macLED1_GPIO_PIN)
#define LED1_ON			  digitalHi(macLED1_GPIO_PORT,macLED1_GPIO_PIN)

#define LED_TOGGLE		digitalToggle(macLED_GPIO_PORT,macLED_GPIO_PIN)
#define LED_OFF		    digitalHi(macLED_GPIO_PORT,macLED_GPIO_PIN)
#define LED_ON			  digitalLo(macLED_GPIO_PORT,macLED_GPIO_PIN)

#define Unlocking()     LED_ON
#define CloseLock()     LED_OFF

void LED_GPIO_Config(void);

#endif /* __LED_H */
