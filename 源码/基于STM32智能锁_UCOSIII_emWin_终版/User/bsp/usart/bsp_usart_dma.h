#ifndef __USARTDMA_H
#define	__USARTDMA_H


#include "stm32f10x.h"
#include <stdio.h>


// 串口工作参数宏定义
#define  DEBUG_USARTx                   USART2
#define  DEBUG_USART_CLK                RCC_APB1Periph_USART2
#define  DEBUG_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
#define  DEBUG_USART_BAUDRATE           57600

// USART GPIO 引脚宏定义
#define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA)
#define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
#define  DEBUG_USART_TX_GPIO_PORT       GPIOA   
#define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_2
#define  DEBUG_USART_RX_GPIO_PORT       GPIOA
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_3

#define  DEBUG_USART_IRQ                USART2_IRQn
#define  DEBUG_USART_IRQHandler         USART2_IRQHandler

// 串口TX对应的DMA请求通道
#define  USART_TX_DMA_CHANNEL     DMA1_Channel7

// 串口RX对应的DMA请求通道
#define  USART_RX_DMA_CHANNEL     DMA1_Channel6
// 外设寄存器地址
#define  USART_DR_ADDRESS        (USART2_BASE+0x04)
// 一次发送的数据量
#define  SENDBUFF_SIZE            20


void USART_Config(void);
void USARTx_DMA_Config(void);
void USARRx_DMA_Config(void);
void USARTx_DMA_SendData(uint8_t *data, uint16_t len);

#endif /* __USARTDMA_H */
