/****************************************************************************************************
 * 描述：指纹应用程序实现
 *
 * 作者：龙泽润
 *
 * 版本：v1.0.0    日期：2018-07-29
 *                                                                              大连智海科技有限公司
****************************************************************************************************/

#include "stm32f10x_systick.h" 
#include "stm32f10x_tim.h"
#include "drv_remote.h"
#include "app_as608.h"
#include "drv_as608.h"
#include "drv_oled.h"

/****************************************************************************************************
 * 描述：指纹录入实现
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

void Add_FR(void)
{
	u8 i=0,ensure ,processnum=0;
	u16 ID;
	while(1)
	{
		OLED_Clear();                   // 清屏 OLED
		switch (processnum)
		{
			case 0:
				i++;
//				LCD_Fill(0,100,lcddev.width,160,WHITE);
//				Show_Str_Mid(0,100,"Pleas touch finger!",16,240);//请按手指
				OLED_ShowString(0, 2, "Pleas touch finger!", 8, 1);    //请按手指
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer1);//生成特征
					if(ensure==0x00)
					{
//						LCD_Fill(0,120,lcddev.width,160,WHITE);
//						Show_Str_Mid(0,120,"Fingerprint correct",16,240);//指纹正确
						i=0;
						processnum=1;//跳到第二步						
					}else ShowErrMessage(ensure);				
				}else ShowErrMessage(ensure);						
				break;
			
			case 1:
				i++;
//				LCD_Fill(0,100,lcddev.width,160,WHITE);
//				Show_Str_Mid(0,100,"Pleas touch finger again!",16,240);//再按一次手指
				OLED_ShowString(0, 2, "Pleas touch finger again!", 8, 1);    //再按一次手指
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer2);//生成特征			
					if(ensure==0x00)
					{
//						LCD_Fill(0,120,lcddev.width,160,WHITE);
//						Show_Str_Mid(0,120,"Fingerprint correct",16,240);//指纹正确
						i=0;
						processnum=2;//跳到第三步
					}else ShowErrMessage(ensure);	
				}else ShowErrMessage(ensure);		
				break;

			case 2:
//				LCD_Fill(0,100,lcddev.width,160,WHITE);
//				Show_Str_Mid(0,100,"Compare twice fingerprint",16,240);//对比两次指纹
				ensure=PS_Match();
				if(ensure==0x00) 
				{
//					LCD_Fill(0,120,lcddev.width,160,WHITE);
//					Show_Str_Mid(0,120,"Twice fingerprint are same",16,240);//两次指纹是一样的
					processnum=3;//跳到第四步
				}
				else 
				{
//					LCD_Fill(0,100,lcddev.width,160,WHITE);
//					Show_Str_Mid(0,100,"Compare fail,pleas touch again!",16,240);//对比失败，请重新按手指
					ShowErrMessage(ensure);
					i=0;
					processnum=0;//跳回第一步		
				}
				Delay_ms(1000);
				break;

			case 3:
//				LCD_Fill(0,100,lcddev.width,160,WHITE);
//				Show_Str_Mid(0,100,"Generate fingerprint template",16,240);//产生一个指纹模板
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
//					LCD_Fill(0,120,lcddev.width,160,WHITE);
//					Show_Str_Mid(0,120,"Generate fingerprint success",16,240);//生成指纹模板成功
					OLED_ShowString(0, 2, "Generate fingerprint success", 8, 1);    // 生成指纹模板成功
					processnum=4;//跳到第五步
				}else {processnum=0;ShowErrMessage(ensure);}
				Delay_ms(1000);
				break;
				
			case 4:	
//				LCD_Fill(0,100,lcddev.width,160,WHITE);
//				Show_Str_Mid(0,100,"Intput ID and save with ENTER!",16,240);//输入ID并按“Enter”保存
				OLED_ShowString(0, 5, "0=<ID<=299", 12, 1);
//				Show_Str_Mid(0,120,"0=< ID <=299",16,240);	
//				do
//					ID=Get_Keyboard_Val();
//				while(!(ID<300));//输入DI必须小于300

				while(!K_P)                     // 输入DI必须小于300
				{
					Using_Matrix_Keyboard();    // 调用矩阵键盘
					OLED_ShowNum(0, 7, Get_Keyboard_Val(), 5, 12, 1);
				}
				
				ID=Get_Keyboard_Val();          // 获取矩阵键盘输入的值
				Matrix_Keyboard_Rst();          // 复位矩阵键盘
				
				ensure=PS_StoreChar(CharBuffer2,ID);    // 储存模板
				if(ensure==0x00) 
				{			
//					LCD_Fill(0,100,lcddev.width,160,WHITE);					
//					Show_Str_Mid(0,120,"Add fingerprint success!!!",16,240);
					OLED_ShowString(0, 2, "Add fingerprint success!!!", 8, 1);    // 添加指纹成功
//					PS_ValidTempleteNum(&ValidN);//读库指纹个数
//					LCD_ShowNum(80,80,AS608Para.PS_max-ValidN,3,16);//显示剩余指纹个数
					Delay_ms(1500);//延时后清除显示	
//					LCD_Fill(0,100,240,160,WHITE);
					return ;
				}else {processnum=0;ShowErrMessage(ensure);}					
				break;				
		}
		Delay_ms(800);
		if(i==5)//超过5次没有按手指则退出
		{
			OLED_Clear();                   // 清屏 OLED
//			LCD_Fill(0,100,lcddev.width,160,WHITE);
			break;	
		}
	}
}

/****************************************************************************************************
 * 描述：刷指纹实现
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

u32 kai = 500;
u32 guan = 950;

void press_FR(void)
{
	SearchResult seach;
	u8 ensure;
//	char *str;
	ensure=PS_GetImage();
	if(ensure==0x00)//获取图像成功 
	{	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //生成特征成功
		{		
			ensure=PS_HighSpeedSearch(CharBuffer1,0,300,&seach);
			
			if(ensure==0x00)//搜索成功
			{				
//				LCD_Fill(0,100,lcddev.width,160,WHITE);
//				Show_Str_Mid(0,100,"Search fingerprint success",16,240);//搜索指纹成功				
//				str=mymalloc(50);
//				sprintf(str,"Match ID:%d  Match score:%d",seach.pageID,seach.mathscore);//显示匹配指纹的ID和分数
//				Show_Str_Mid(0,140,(u8*)str,16,240);
//				myfree(str);
				OLED_ShowString(0, 3, "OK", 8, 1);    // 生成指纹模板成功
				
				/* 开锁处理函数 */
				
				TIM2_CCV(kai);
				Delay_ms(999);
				Delay_ms(999);
				Delay_ms(999);
			}
			else 
				ShowErrMessage(ensure);					
		}
		else
			ShowErrMessage(ensure);
		Delay_ms(1000);//延时后清除显示
		
	/* 关锁处理函数 */
		
		TIM2_CCV(guan);
				
	 OLED_Clear();                   // 清屏 OLED
	}
		
}

/****************************************************************************************************
 * 描述：显示确认码错误信息
 *
 * 参数：ensure：错误码
 *
 * 返回：无
****************************************************************************************************/

void ShowErrMessage(u8 ensure)
{
	OLED_ShowNum_NL(0, 0, ensure, 16, 1);    // 显示错误码
}

/****************************************************************************************************
 * 描述：按键任务
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

void AS608_Task(void)
{
	static u8 K1_flag = 0;    // 按键 1 标志
	static u8 K2_flag = 0;    // 按键 2 标志
	static u8 K4_flag = 0;    // 按键 1 标志
	static u8 K7_flag = 0;    // 按键 2 标志
	static u8 K6_flag = 0;    // 按键 1 标志
	static u8 K9_flag = 0;    // 按键 2 标志
	
	
	if(K_1 ^ K1_flag )             // 按键 1 状态改变
	{ 
		K1_flag = K_1;             // 更新按键状态
		
		if (K_1)                   // 按键 1 按下
		{
			Add_FR();    // 指纹录入
		}
	}
	
	if(K_2 ^ K2_flag )             // 按键 2 状态改变
	{ 
		K2_flag = K_2;             // 更新按键状态
		
		if (K_2)                   // 按键 2 按下
		{
			PS_Empty();    // 清空指纹库
			OLED_ShowString(0, 2, "OK", 8, 1);    // 生成指纹模板成功
			Delay_ms(1000);
			OLED_Clear();                   // 清屏 OLED
		}
	}

	if (PS_Sta)
	{
		press_FR();    // 刷指纹
	}
	
	/* 设置开锁 */
	
	if(K_4 ^ K4_flag )             // 按键 4 状态改变
	{ 
		K4_flag = K_4;             // 更新按键状态
		
		if (K_4)                   // 按键 1 按下
		{
			kai += 50;
			TIM2_CCV(kai);
		}
	}
	
	if(K_7 ^ K7_flag )             // 按键 7 状态改变
	{ 
		K7_flag = K_7;             // 更新按键状态
		
		if (K_7)                   // 按键 7 按下
		{
			kai -= 50;
			TIM2_CCV(kai);
		}
	}
	
	if(K_6 ^ K6_flag )             // 按键 4 状态改变
	{ 
		K6_flag = K_6;             // 更新按键状态
		
		if (K_6)                   // 按键 1 按下
		{
			guan += 50;
			TIM2_CCV(guan);
		}
	}
	
	if(K_9 ^ K9_flag )             // 按键 7 状态改变
	{ 
		K9_flag = K_9;             // 更新按键状态
		
		if (K_9)                   // 按键 7 按下
		{
			guan -= 50;
			TIM2_CCV(guan);
		}
	}
	
	OLED_ShowNum(0, 7, kai, 5, 12, 1);
	OLED_ShowNum(64, 7, guan, 5, 12, 1);
}

/* End Of File */
