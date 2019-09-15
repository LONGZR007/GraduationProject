#include "drv_remote.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_nvic.h"
#include "stm32f10x_tim.h"
#include "drv_oled.h"
#include "stm32f10x_systick.h"

/* 矩阵键值缓存区 */

static u8 KeyValueCache[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};    // 储存键值
static u8 Key_Num = 0;                                           // 保存矩阵键盘输入数量

//红外遥控初始化
//设置IO以及定时器4的输入捕获
void Remote_Init(void)    			  
{									   
	RCC->APB1ENR|=1<<2;   	//TIM4 时钟使能 
	RCC->APB2ENR|=1<<3;    	//使能PORTB时钟 

	GPIOB->CRH&=0XFFFFFF0F;	//PB9 输入  
	GPIOB->CRH|=0X00000080;	//上拉输入     
	GPIOB->ODR|=1<<9;		//PB9 上拉
	  
 	TIM4->ARR=10000;  		//设定计数器自动重装值 最大10ms溢出  
	TIM4->PSC=71;  			//预分频器,1M的计数频率,1us加1.	 

	TIM4->CCMR2|=1<<8;		//CC4S=01 	选择输入端 IC4映射到TI4上
 	TIM4->CCMR2|=3<<12;  	//IC4F=0011 配置输入滤波器 8个定时器时钟周期滤波
 	TIM4->CCMR2|=0<<10;  	//IC4PS=00 	配置输入分频,不分频 

	TIM4->CCER|=0<<13; 		//CC4P=0	上升沿捕获
	TIM4->CCER|=1<<12; 		//CC4E=1 	允许捕获计数器的值到捕获寄存器中
	TIM4->DIER|=1<<4;   	//允许CC4IE捕获中断				
	TIM4->DIER|=1<<0;   	//允许更新中断				
	TIM4->CR1->CEN=0x01;    	//使能定时器4
//  	MY_NVIC_Init(1,3,TIM4_IRQn,2);//抢占1，子优先级3，组2
	NVIC_Config(TIM4_IRQn, 1, 3,ENABLE);//抢占1，子优先级3
}

//遥控器接收状态
//[7]:收到了引导码标志
//[6]:得到了一个按键的所有信息
//[5]:保留	
//[4]:标记上升沿是否已经被捕获								   
//[3:0]:溢出计时器
u8 	RmtSta=0;	  	  
u16 Dval;		//下降沿时计数器的值
u32 RmtRec=0;	//红外接收到的数据	   		    
u8  RmtCnt=0;	//按键按下的次数	  
//定时器4中断服务程序	 
void TIM4_IRQHandler(void)
{ 		    	 
	u16 tsr;
	tsr=TIM4->SR;
	if(tsr&0X01)                                    //溢出
	{
		if(RmtSta&0x80)								//上次有数据被接收到了
		{	
			RmtSta&=~0X10;							//取消上升沿已经被捕获标记
			if((RmtSta&0X0F)==0X00)RmtSta|=1<<6;	//标记已经完成一次按键的键值信息采集
			if((RmtSta&0X0F)<14)RmtSta++;
			else
			{
				RmtSta&=~(1<<7);					//清空引导标识
				RmtSta&=0XF0;						//清空计数器	
			}								 	   	
		}							    
	}
	if(tsr&(1<<4))                                  //CC4IE中断
	{	  
		if(RDATA)                                   //上升沿捕获
		{
  			TIM4->CCER|=1<<13; 						//CC4P=1	设置为下降沿捕获
			TIM4->CNT=0;							//清空定时器值
			RmtSta|=0X10;							//标记上升沿已经被捕获
		}else                                       //下降沿捕获
		{
			Dval=TIM4->CCR4;						//读取CCR4也可以清CC4IF标志位
  			TIM4->CCER&=~(1<<13);					//CC4P=0	设置为上升沿捕获
			if(RmtSta&0X10)							//完成一次高电平捕获 
			{
 				if(RmtSta&0X80)                     //接收到了引导码
				{
					
					if(Dval>300&&Dval<800)			//560为标准值,560us
					{
						RmtRec<<=1;					//左移一位.
						RmtRec|=0;					//接收到0	   
					}else if(Dval>1400&&Dval<1800)	//1680为标准值,1680us
					{
						RmtRec<<=1;					//左移一位.
						RmtRec|=1;					//接收到1
					}else if(Dval>2200&&Dval<2600)	//得到按键键值增加的信息 2500为标准值2.5ms
					{
						RmtCnt++; 					//按键次数增加1次
						RmtSta&=0XF0;				//清空计时器		
					}
 				}else if(Dval>4200&&Dval<4700)		//4500为标准值4.5ms
				{
					RmtSta|=1<<7;					//标记成功接收到了引导码
					RmtCnt=0;						//清除按键次数计数器
				}						 
			}
			RmtSta&=~(1<<4);
		}				 		     	    					   
	}
	TIM4->SR=0;//清除中断标志位 	    
}

//处理红外键盘
//返回值:
//	 0,没有任何按键按下
//其他,按下的按键键值.
u8 Remote_Scan(void)
{        
	u8 sta=0;       
    u8 t1,t2;  
	if(RmtSta&(1<<6))//得到一个按键的所有信息了
	{ 
	    t1=RmtRec>>24;			//得到地址码
	    t2=(RmtRec>>16)&0xff;	//得到地址反码 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)//检验遥控识别码(ID)及地址 
	    { 
	        t1=RmtRec>>8;
	        t2=RmtRec; 	
	        if(t1==(u8)~t2)sta=t1;//键值正确	 
//			OLED_ShowNum(0, 3 ,sta, 6,16,1);//显示键值
		}   
		if((sta==0)||((RmtSta&0X80)==0))//按键数据错误/遥控已经没有按下了
		{
		 	RmtSta&=~(1<<6);//清除接收到有效按键标识
			RmtCnt=0;		//清除按键次数计数器
		}
	}  
    return sta;
}

void Remote_Discern(void)
{
	static u8 Sign_2 = 0;    // 按键 1 标志
	static u8 Sign_1 = 0;    // 按键 2 标志
    static u32 xC = 1080;
	
	if (K_UP == 1)           // 按键 S1 按下
	{
		Sign_1 = 1;          // 按键按下标志
	}
	else if (Sign_1 == 1)    // 按键 S1 短按弹起
	{
		Sign_1 = 0;          // 复位按键
		
		/* 单次按下 */

		xC += 100;
	}

	if (K_DN == 1)           // 按键 S1 按下
	{
		Sign_2 = 1;          // 按键按下标志
	}
	else if (Sign_2 == 1)    // 按键 S1 短按弹起
	{
		Sign_2 = 0;          // 复位按键
		
		/* 单次按下 */

		xC -= 100;
	}

	OLED_ShowNum(0, 0 ,xC, 6,16,1);
	
	TIM2_CCV(xC);
	
	if(K_1)
	{
		OLED_ShowString(0, 4,"K_1 ", 16,1);	//显示SYMBOL
	}
	
	
	else if (K_2)
	{
		OLED_ShowString(0, 4,"K_2 ", 16,1);	//显示SYMBOL
	}
	else if (K_3)
	{
		OLED_ShowString(0, 4,"K_3 ", 16,1);	//显示SYMBOL
	}
	else if (K_4)
	{
		OLED_ShowString(0, 4,"K_4 ", 16,1);	//显示SYMBOL
	}
	else if (K_5)
	{
		OLED_ShowString(0, 4,"K_5 ", 16,1);	//显示SYMBOL
	}
	else if (K_6)
	{
		OLED_ShowString(0, 4,"K_6 ", 16,1);	//显示SYMBOL
	}
	else if (K_7)
	{
		OLED_ShowString(0, 4,"K_7 ", 16,1);	//显示SYMBOL
	}
	else if (K_8)
	{
		OLED_ShowString(0, 4,"K_8 ", 16,1);	//显示SYMBOL
	}
	else if (K_9)
	{
		OLED_ShowString(0, 4,"K_9 ", 16,1);	//显示SYMBOL
	}
	else if (K_0)
	{
		OLED_ShowString(0, 4,"K_0 ", 16,1);	//显示SYMBOL
	}
	else if (K_A)
	{
		OLED_Display_Off();                 // 关闭 OLED
		OLED_ShowString(0, 4,"K_* ", 16,1);	// 显示SYMBOL
	}
	else if (K_P)
	{
		OLED_Display_On();                  // 打开 OLED
		OLED_ShowString(0, 4,"K_# ", 16,1);	// 显示SYMBOL
	}
	else if (K_OK)
	{
		OLED_Clear();                       // 清屏
		OLED_ShowString(0, 4,"K_OK", 16,1);	// 显示SYMBOL
	}
	else if (K_UP)
	{
		OLED_ShowString(0, 4,"K_UP", 16,1);	// 显示SYMBOL
	}
	else if (K_R)
	{
		OLED_ShowString(0, 4,"K_R ", 16,1);	//显示SYMBOL
	}
	else if (K_L)
	{
		OLED_ShowString(0, 4,"K_L ", 16,1);	//显示SYMBOL
	}
	else if (K_DN)
	{
		OLED_ShowString(0, 4,"K_DN", 16,1);	//显示SYMBOL
	}
}

/****************************************************************************************************
 * 描述：使用矩阵键输入 0-9
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

void Using_Matrix_Keyboard(void)
{
	static u8 K1_flag = 0;    // 按键 1 标志
	static u8 K2_flag = 0;    // 按键 2 标志
	static u8 K3_flag = 0;    // 按键 3 标志
	static u8 K4_flag = 0;    // 按键 4 标志
	static u8 K5_flag = 0;    // 按键 5 标志
	static u8 K6_flag = 0;    // 按键 6 标志
	static u8 K7_flag = 0;    // 按键 7 标志
	static u8 K8_flag = 0;    // 按键 8 标志
	static u8 K9_flag = 0;    // 按键 9 标志
	static u8 K0_flag = 0;    // 按键 0 标志
	static u8 KA_flag = 0;    // 按键 * 标志

	if(K_1 ^ K1_flag )             // 按键 1 状态改变
	{ 
		K1_flag = K_1;             // 更新按键状态
		
		if (K_1)                   // 按键 1 按下
		{
			Matrix_Keyboard(1);    // 调用矩阵按键处理函数
		}
	}
	
	if(K_2 ^ K2_flag )             // 按键 2 状态改变
	{ 
		K2_flag = K_2;             // 更新按键状态
		
		if (K_2)                   // 按键 2 按下
		{
			Matrix_Keyboard(2);    // 调用矩阵按键处理函数
		}
	}
	
	if(K_3 ^ K3_flag )             // 按键 3 状态改变
	{ 
		K3_flag = K_3;             // 更新按键状态
		
		if (K_3)                   // 按键 3 按下
		{
			Matrix_Keyboard(3);    // 调用矩阵按键处理函数
		}
	}
	
	if(K_4 ^ K4_flag )             // 按键 4 状态改变
	{ 
		K4_flag = K_4;             // 更新按键状态
		
		if (K_4)                   // 按键 4 按下
		{
			Matrix_Keyboard(4);    // 调用矩阵按键处理函数
		}
	}
	
	if(K_5 ^ K5_flag )             // 按键 5 状态改变
	{ 
		K5_flag = K_5;             // 更新按键状态
		
		if (K_5)                   // 按键 5 按下
		{
			Matrix_Keyboard(5);    // 调用矩阵按键处理函数
		}
	}
	
	if(K_6 ^ K6_flag )             // 按键 6 状态改变
	{ 
		K6_flag = K_6;             // 更新按键状态
		
		if (K_6)                   // 按键 6 按下
		{
			Matrix_Keyboard(6);    // 调用矩阵按键处理函数
		}
	}
	
	if(K_7 ^ K7_flag )             // 按键 7 状态改变
	{ 
		K7_flag = K_7;             // 更新按键状态
		
		if (K_7)                   // 按键 7 按下
		{
			Matrix_Keyboard(7);    // 调用矩阵按键处理函数
		}
	}
	
	if(K_8 ^ K8_flag )             // 按键 8 状态改变
	{ 
		K8_flag = K_8;             // 更新按键状态
		
		if (K_8)                   // 按键 8 按下
		{
			Matrix_Keyboard(8);    // 调用矩阵按键处理函数
		}
	}
	
	if(K_9 ^ K9_flag )             // 按键 9 状态改变
	{ 
		K9_flag = K_9;             // 更新按键状态
		
		if (K_9)                   // 按键 9 按下
		{
			Matrix_Keyboard(9);    // 调用矩阵按键处理函数
		}
	}
	
	if(K_0 ^ K0_flag )             // 按键 0 状态改变
	{ 
		K0_flag = K_0;             // 更新按键状态
		
		if (K_0)                   // 按键 0 按下
		{
			Matrix_Keyboard(0);    // 调用矩阵按键处理函数
		}
	}
	
	if(K_A ^ KA_flag )             // 按键 * 状态改变
	{ 
		KA_flag = K_A;             // 更新按键状态
		
		if (K_A)                   // 按键 * 按下
		{
			Key_Num--;             // 按键输入数量减一
		}
	}
}

/****************************************************************************************************
 * 描述：获取矩阵键输入 0-9 的转换值
 *
 * 参数：无
 *
 * 返回：矩阵键盘输入值
****************************************************************************************************/

u32 Get_Keyboard_Val(void)
{ 
	u32 Temp_xC = 1;      // 中间变量
	u32 Temp = 0;         // 临时变量
	u8 xC = Key_Num;      // 键值数量
	
	while (xC)            // 键值数量不为 0
	{
		Temp += KeyValueCache[--xC] * Temp_xC;    // 合并按键值
		Temp_xC *= 10;                            // 中间变量累乘
	}
	
	return Temp;          // 返回按键值
}

/****************************************************************************************************
 * 描述：矩阵键输入 0-9 的处理
 *
 * 参数：key：键值（0-9）
 *
 * 返回：无
****************************************************************************************************/

void Matrix_Keyboard(u8 key)
{
	Key_Num %= 10;                   // 键值数量越界处理
	
	KeyValueCache[Key_Num] = key;    // 输入的键值
	Key_Num++;                       // 键值数量递增
}

/****************************************************************************************************
 * 描述：矩阵键盘复位
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

void Matrix_Keyboard_Rst(void)
{
	Key_Num = 0;    // 复位矩阵键盘
}

/* End Of File */
