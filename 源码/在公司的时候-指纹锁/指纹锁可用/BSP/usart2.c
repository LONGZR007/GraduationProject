#include "stm32f10x_rcc.h"
#include "stm32f10x_nvic.h"
#include "stm32f10x_systick.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "usart2.h"
#include "stdarg.h"


//////////////////////////////////////////////////////////////////////////////////	   
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK Mini STM32开发板
//串口2驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/3/29
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved	
//********************************************************************************
//修改说明
//无
////////////////////////////////////////////////////////////////////////////////// 	

//串口接收缓存区 	
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.
u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 

/* TIM3 定时配置 */

extern vu16 USART2_RX_STA; 
//定时器3中断服务程序	 
void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//溢出中断
	{
		USART2_RX_STA|=1<<15;	//标记接收完成	
		TIM3->CR1->CEN = 0;		//关闭定时器4
	}				   
	TIM3->SR&=~(1<<0);//清除中断标志位 	    
}
//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器4!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3时钟使能    
 	TIM3->ARR=arr;  	//设定计数器自动重装值 
	TIM3->PSC=psc;  	//预分频器设置
	TIM3->DIER|=1<<0;   //允许更新中断				
	TIM3->CR1->CEN = 1;    //使能定时器4
	NVIC_Config(TIM3_IRQn, 1, 3,ENABLE);//抢占1，子优先级3
	
//  MY_NVIC_Init(0,2,TIM4_IRQn,2);//抢占1，子优先级3，组2									 
}


//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
vu16 USART2_RX_STA=0;
void USART2_IRQHandler(void)
{
	u8 res;	      
	if(USART2->SR&(1<<5))//接收到数据
	{	 
		res=USART2->DR; 			 
		if((USART2_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
		{ 
			if(USART2_RX_STA<USART2_MAX_RECV_LEN)	//还可以接收数据
			{
				TIM3->CNT=0;         				//计数器清空
				if(USART2_RX_STA==0) 				//使能定时器4的中断 
				{
					TIM3->CR1->CEN = 1;     			//使能定时器4
				}
				USART2_RX_BUF[USART2_RX_STA++]=res;	//记录接收到的值	 
			}else 
			{
				USART2_RX_STA|=1<<15;				//强制标记接收完成
			} 
		}
	}  				 											 
}
//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率 
void usart2_init(u32 pclk1,u32 bound)
{  	 
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk1*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   				//使能PORTA口时钟 
	RCC->APB1ENR|=1<<17;  				//使能串口2时钟 
	GPIOA->CRL&=0XFFFF00FF;				//PA2\PA3IO状态设置
	GPIOA->CRL|=0X00008B00;				//IO状态设置
	
	RCC->APB1RSTR|=1<<17;   			//复位串口2
	RCC->APB1RSTR&=~(1<<17);			//停止复位	
	//波特率设置
// 	USART2->BRR=(pclk1*1000000)/(bound);// 波特率设置	
	USART2->BRR=mantissa;
	USART2->CR1|=0X200C;  				//1位停止,无校验位.
	//使能接收中断 
	USART2->CR1|=1<<5;    				//接收缓冲区非空中断使能	
	NVIC_Config(USART2_IRQn, 0, 1,ENABLE);//抢占1，子优先级3	
//	MY_NVIC_Init(0,1,USART2_IRQn,2);//组2
	TIM3_Int_Init(99,7199);				//10ms中断
	TIM3->CR1->CEN = 1;						//关闭定时器4
	USART2_RX_STA=0;							//清零
}

//串口2,printf 函数
//确保一次发送数据不超过USART2_MAX_SEND_LEN字节
void u2_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
		while((USART2->SR&0X40)==0);			//循环发送,直到发送完毕   
		USART2->DR=USART2_TX_BUF[j];  
	} 
}





