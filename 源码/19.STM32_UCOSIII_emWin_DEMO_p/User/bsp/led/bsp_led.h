#ifndef __LED_H
#define	__LED_H


#include "stm32f10x.h"


/* 定义LED连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的LED引脚 */
#define macLED1_GPIO_PORT    	GPIOD			              /* GPIO端口 */
#define macLED1_GPIO_CLK 	    RCC_APB2Periph_GPIOD		/* GPIO端口时钟 */
#define macLED1_GPIO_PIN		  GPIO_Pin_11			          /* LED引脚号 */

/* 定义LED连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的LED引脚 */
#define macLED_GPIO_PORT    	GPIOE			              /* GPIO端口 */
#define macLED_GPIO_CLK 	    RCC_APB2Periph_GPIOE		/* GPIO端口时钟 */
#define macLED_GPIO_PIN		    GPIO_Pin_6		          /* LED引脚号 */


/* 带参宏，可以像内联函数一样使用 */
#define LED1(a)	if (a)	\
					GPIO_SetBits(macLED1_GPIO_PORT,macLED1_GPIO_PIN);\
					else		\
					GPIO_ResetBits(macLED1_GPIO_PORT,macLED1_GPIO_PIN)



/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)			{p->BSRR=i;}			//设置为高电平		
#define digitalLo(p,i)			{p->BRR=i;}				//输出低电平
#define digitalToggle(p,i)		{p->ODR ^=i;}			//输出反转状态


/* 定义控制IO的宏 */
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
