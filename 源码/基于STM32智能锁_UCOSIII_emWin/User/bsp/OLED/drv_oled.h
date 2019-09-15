/****************************************************************************************************
 * 描述：OLED 硬件驱动层声明
 *
 * 作者：龙泽润
 *
 * 版本：v1.0.0    日期：2018-04-25
 *                                                                              大连智海科技有限公司
****************************************************************************************************/

#include "stm32f10x.h"

#ifndef STM32F10X_OLED_H
#define STM32F10X_OLED_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
/* 写类型定义 */
	 
#define OLED_CMD  0	   // 写命令
#define OLED_DATA 1	   // 写数据

/* 暂时 */
	 
#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	
	 
/****************************************************************************************************
 * 描述：初始化SSD1306声明	
 *
 * 参数：无
 *
 * 返回：无
****************************************************************************************************/

void OLED_Init(void);
	 
/****************************************************************************************************
 * 描述：发送写命令声明
 *
 * 参数：Command = 命令
 *
 * 返回：无
****************************************************************************************************/

void Write_IIC_Command(u8 Command);

/****************************************************************************************************
 * 描述：发送数据声明
 *
 * 参数：Data = 数据
 *
 * 返回：无
****************************************************************************************************/

void Write_IIC_Data(u8 Data, u8 aga);

/****************************************************************************************************
 * 描述：发送数据 / 命令
 *
 * 参数：dat = 数据
 * 参数：cmd = 发送的是数据还是命令
 *
 * 返回：无
****************************************************************************************************/

void OLED_WR_Byte(u8 dat,u8 cmd,u8 aga);

/****************************************************************************************************
 * 描述：显示一个小数
 *
 * 参数：x, y:起点坐标
 * 参数：num：数值
 * 参数：vir:小数点的有效位
 * 参数：size2：字体大小
 *
 * 返回：无
****************************************************************************************************/

void OLED_ShowDec(u8 x,u8 y,float num,u8 vir,u8 size2,u8 aga);

/****************************************************************************************************
 * 描述：显示一个整数可以不指定长度
 *
 * 参数：x, y:起点坐标
 * 参数：num：数值
 * 参数：size2：字体大小
 *
 * 返回：无
****************************************************************************************************/

void OLED_ShowNum_NL(u8 x,u8 y,u32 num, u8 size2,u8 aga);

/* 未整理的函数声明 */

void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size,u8 aga);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size,u8 aga);	 
void OLED_Set_Pos(u8 x, u8 y);
void OLED_ShowCHinese(u8 x,u8 y,u8 no,u8 aga);
void OLED_DrawBMP(u8 x0, u8 y0,u8 x1, u8 y1,u8 BMP[]);
void fill_picture(unsigned char fill_Data);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2,u8 aga);

void SI2C_SCL_SET(void);
void SI2C_SCL_RST(void);
void SI2C_SDA_SET(void);
void SI2C_SDA_RST(void); 
u8 SI2C_SDA_GET(void);

/* 选择 I2C 的开关 */

#define SE_I2C1    0    // I2C 使用选择, = 1 使用 I2C1; = 0 使用 SI2C

#ifdef __cplusplus
}
#endif

#endif /* STM32F10X_OLED_H */

/* End Of File */
