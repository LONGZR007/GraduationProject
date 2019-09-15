#ifndef __EXTI_H
#define	__EXTI_H


#include "stm32f10x.h"


//引脚定义
#define Bell_INT_GPIO_PORT         GPIOF
#define Bell_INT_GPIO_CLK          (RCC_APB2Periph_GPIOF|RCC_APB2Periph_AFIO)
#define Bell_INT_GPIO_PIN          GPIO_Pin_14
#define Bell_INT_EXTI_PORTSOURCE   GPIO_PortSourceGPIOF
#define Bell_INT_EXTI_PINSOURCE    GPIO_PinSource14
#define Bell_INT_EXTI_LINE         EXTI_Line14

#define Open_INT_GPIO_PORT         GPIOB
#define Open_INT_GPIO_CLK          (RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO)
#define Open_INT_GPIO_PIN          GPIO_Pin_12
#define Open_INT_EXTI_PORTSOURCE   GPIO_PortSourceGPIOB
#define Open_INT_EXTI_PINSOURCE    GPIO_PinSource12
#define Open_INT_EXTI_LINE         EXTI_Line12

#define Open_Bell_INT_EXTI_IRQ          EXTI15_10_IRQn
#define Open_Bell_IRQHandler            EXTI15_10_IRQHandler

/* 矩阵按键 */

#define Line3_GPIO_PORT              GPIOB
#define Line3_GPIO_CLK               RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO
#define Line3_GPIO_PIN               GPIO_Pin_9
#define Line3_INT_EXTI_PINSOURCE     GPIO_PinSource9
#define Line3_INT_EXTI_LINE          EXTI_Line9

#define Line2_GPIO_PORT              GPIOB
#define Line2_GPIO_CLK               RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO
#define Line2_GPIO_PIN               GPIO_Pin_8
#define Line2_INT_EXTI_PINSOURCE     GPIO_PinSource8
#define Line2_INT_EXTI_LINE          EXTI_Line8

#define Line1_GPIO_PORT              GPIOB
#define Line1_GPIO_CLK               RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO
#define Line1_GPIO_PIN               GPIO_Pin_7

#define Row1_GPIO_PORT           GPIOG
#define Row1_GPIO_CLK            RCC_APB2Periph_GPIOG
#define Row1_GPIO_PIN            GPIO_Pin_9

#define Row2_GPIO_PORT           GPIOG
#define Row2_GPIO_CLK            RCC_APB2Periph_GPIOG
#define Row2_GPIO_PIN            GPIO_Pin_10

#define Row3_GPIO_PORT           GPIOG
#define Row3_GPIO_CLK            RCC_APB2Periph_GPIOG
#define Row3_GPIO_PIN            GPIO_Pin_13

#define Row4_GPIO_PORT           GPIOG
#define Row4_GPIO_CLK            RCC_APB2Periph_GPIOG
#define Row4_GPIO_PIN            GPIO_Pin_14

void EXTI_Key_Config(void);
void Matrix_Buttons_Init(void);
u8 Matrix_Buttons_Scan(void);
void PasswordUnlock(void);
void Del_Open_TempCode(void);

#endif /* __EXTI_H */
