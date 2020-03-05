#ifndef __USART1_H
#define	__USART1_H

#include "stm32f10x.h"
#include <stdio.h>

#define             macUSART_BAUD_RATE                       115200

#define             macUSARTx                                USART1
#define             macUSART_APBxClock_FUN                   RCC_APB2PeriphClockCmd
#define             macUSART_CLK                             RCC_APB2Periph_USART1
#define             macUSART_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macUSART_GPIO_CLK                        (RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO)     
#define             macUSART_TX_PORT                         GPIOA   
#define             macUSART_TX_PIN                          GPIO_Pin_9
#define             macUSART_RX_PORT                         GPIOA   
#define             macUSART_RX_PIN                          GPIO_Pin_10
// һ�η��͵�������
#define  RXBUFF_SIZE            20

extern u8 USART1_RX_Len;               // ���ڽӳ���
extern uint8_t Usart1RxBuff[RXBUFF_SIZE];    // ���ڽ��ջ�����

#define  USART1_IRQ                USART1_IRQn
#define  USART1_IRQHandler         USART1_IRQHandler

// ����RX��Ӧ��DMA����ͨ��
#define  USART1_RX_DMA_CHANNEL     DMA1_Channel5
// ����Ĵ�����ַ
#define  USART1_DR_ADDRESS        (USART1_BASE+0x04)

void USART1_Config(void);
void USART1_Start_Rx(void);

#endif /* __USART1_H */
