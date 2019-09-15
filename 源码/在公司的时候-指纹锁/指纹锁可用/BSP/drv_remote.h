#include "stm32f10x.h"
#ifndef __RED_H
#define __RED_H 
//#include "sys.h"   
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板V3
//红外遥控解码驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/1/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

#define RDATA 		GPIO_IN_GET(GPIOB, 9) 	//红外数据输入脚PBin(9)

//红外遥控识别码(ID),每款遥控器的该值基本都不一样,但也有一样的.
//我们选用的遥控器识别码为0
#define REMOTE_ID 0  

		   
#define    K_1     (162 == Remote_Scan() ? 1 : 0)    // 162 对应 1		case 162 : str = "1";     break;
#define    K_2     (98  == Remote_Scan() ? 1 : 0)    // 89  对应 3		case 226 : str = "3";     break;
#define    K_3     (226 == Remote_Scan() ? 1 : 0)    // 226 对应 3		case 98  : str = "2";     break;	    
#define    K_4     (34  == Remote_Scan() ? 1 : 0)    // 34  对应 4	    case 34  : str = "4";     break;		 
#define    K_5     (2   == Remote_Scan() ? 1 : 0)    // 2   对应 5		case 2   : str = "5";     break;		  
#define    K_6     (194 == Remote_Scan() ? 1 : 0)    // 194 对应 6		case 194 : str = "6";     break;	   
#define    K_7     (224 == Remote_Scan() ? 1 : 0)    // 224 对应 7		case 224 : str = "7";     break;		  
#define    K_8     (168 == Remote_Scan() ? 1 : 0)    // 168 对应 8		case 168 : str = "8";     break;				   
#define    K_9     (144 == Remote_Scan() ? 1 : 0)    // 144 对应 9		case 144 : str = "9";     break;		    
#define    K_0     (152 == Remote_Scan() ? 1 : 0)    // 152 对应 0		case 104 : str = "*";     break;		  
#define    K_A     (104 == Remote_Scan() ? 1 : 0)    // 104 对应 *		case 152 : str = "0";     break;	   
#define    K_P     (176 == Remote_Scan() ? 1 : 0)    // 176 对应 #		case 176 : str = "#";     break;	 	    
#define    K_UP    (24  == Remote_Scan() ? 1 : 0)    // 24  对应 UP		case 24  : str = "UP";    break;
#define    K_L     (16  == Remote_Scan() ? 1 : 0)    // 16  对应 L		case 16  : str = "LEFT";  break;			   					
#define    K_OK    (56  == Remote_Scan() ? 1 : 0)    // 56  对应 OK		case 56  : str = "OK";    break;	 
#define    K_R     (90  == Remote_Scan() ? 1 : 0)    // 90  对应 R		case 90  : str = "RIGHT"; break;
#define    K_DN    (74  == Remote_Scan() ? 1 : 0)    // 74  对应 DN		case 74  : str = "DOWN";  break;

extern u8 RmtCnt;			//按键按下的次数

void Remote_Init(void);    	//红外传感器接收头引脚初始化
u8 Remote_Scan(void);	    
void Remote_Discern(void);

/****************************************************************************************************
 * 描述：使用矩阵键输入 0-9
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

void Using_Matrix_Keyboard(void);

/****************************************************************************************************
 * 描述：获取矩阵键输入 0-9 的转换值
 *
 * 参数：无
 *
 * 返回：矩阵键盘输入值
****************************************************************************************************/

u32 Get_Keyboard_Val(void);

/****************************************************************************************************
 * 描述：矩阵键输入 0-9 的处理
 *
 * 参数：key：键值（0-9）
 *
 * 返回：无
****************************************************************************************************/

void Matrix_Keyboard(u8 key);

/****************************************************************************************************
 * 描述：矩阵键盘复位
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

void Matrix_Keyboard_Rst(void);

#endif

/* End Of File */
