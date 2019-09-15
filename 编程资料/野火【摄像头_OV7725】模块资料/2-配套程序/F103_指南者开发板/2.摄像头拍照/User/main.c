/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����ͷ����ov7725��������
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "./ov7725/bsp_ov7725.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./led/bsp_led.h"   
#include "./usart/bsp_usart.h"
#include "./key/bsp_key.h"  
#include "./systick/bsp_SysTick.h"
#include "./bmp/bsp_bmp.h"
#include "ff.h"


extern uint8_t Ov7725_vsync;

unsigned int Task_Delay[NumOfTask]; 

extern OV7725_MODE_PARAM cam_mode;

FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FRESULT res_sd;                /* �ļ�������� */


/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void) 	
{		
	
	float frame_count = 0;
	uint8_t retry = 0;

	/* Һ����ʼ�� */
	ILI9341_Init();
	ILI9341_GramScan ( 3 );
	
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);

  ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* ��������ʾȫ�� */
	
	/********��ʾ�ַ���ʾ��*******/
  ILI9341_DispStringLine_EN(LINE(0),"BH OV7725 Test Demo");

	USART_Config();
	LED_GPIO_Config();
	Key_GPIO_Config();
	SysTick_Init();
	
	/*����sd�ļ�ϵͳ*/
	res_sd = f_mount(&fs,"0:",1);
	if(res_sd != FR_OK)
	{
		printf("\r\n�������������Ѹ�ʽ����fat��ʽ��SD����\r\n");
	}
	
	printf("\r\n ** OV7725����ͷʵʱҺ����ʾ����** \r\n"); 
	
	/* ov7725 gpio ��ʼ�� */
	OV7725_GPIO_Config();
	
	LED_BLUE;
	/* ov7725 �Ĵ���Ĭ�����ó�ʼ�� */
	while(OV7725_Init() != SUCCESS)
	{
		retry++;
		if(retry>5)
		{
			printf("\r\nû�м�⵽OV7725����ͷ\r\n");
			ILI9341_DispStringLine_EN(LINE(2),"No OV7725 module detected!");
			while(1);
		}
	}


	/*��������ͷ����������ģʽ*/
	OV7725_Special_Effect(cam_mode.effect);
	/*����ģʽ*/
	OV7725_Light_Mode(cam_mode.light_mode);
	/*���Ͷ�*/
	OV7725_Color_Saturation(cam_mode.saturation);
	/*���ն�*/
	OV7725_Brightness(cam_mode.brightness);
	/*�Աȶ�*/
	OV7725_Contrast(cam_mode.contrast);
	/*����Ч��*/
	OV7725_Special_Effect(cam_mode.effect);
	
	/*����ͼ�������ģʽ��С*/
	OV7725_Window_Set(cam_mode.cam_sx,
														cam_mode.cam_sy,
														cam_mode.cam_width,
														cam_mode.cam_height,
														cam_mode.QVGA_VGA);

	/* ����Һ��ɨ��ģʽ */
	ILI9341_GramScan( cam_mode.lcd_scan );
	
	
	
	ILI9341_DispStringLine_EN(LINE(2),"OV7725 initialize success!");
	printf("\r\nOV7725����ͷ��ʼ�����\r\n");
	
	Ov7725_vsync = 0;
	
	while(1)
	{
		/*���յ���ͼ�������ʾ*/
		if( Ov7725_vsync == 2 )
		{
			frame_count++;
			FIFO_PREPARE;  			/*FIFO׼��*/					
			ImagDisp(cam_mode.lcd_sx,
								cam_mode.lcd_sy,
								cam_mode.cam_width,
								cam_mode.cam_height);			/*�ɼ�����ʾ*/
			
			Ov7725_vsync = 0;			
//			LED1_TOGGLE;

		}
		
		/*��ⰴ��*/
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
		{		
			static uint8_t name_count = 0;
			char name[40];
			
			//�������ý�ͼ���֣���ֹ�ظ���ʵ��Ӧ���п���ʹ��ϵͳʱ����������
			name_count++; 
			sprintf(name,"0:photo_%d.bmp",name_count);

			LED_BLUE;
			printf("\r\n���ڽ�ͼ...");
			
			/*��ͼ�������ú�Һ����ʾ����ͽ�ͼ����*/
			ILI9341_GramScan ( cam_mode.lcd_scan );			
			
			if(Screen_Shot(0,0,LCD_X_LENGTH,LCD_Y_LENGTH,name) == 0)
			{
				printf("\r\n��ͼ�ɹ���");
				LED_GREEN;
			}
			else
			{
				printf("\r\n��ͼʧ�ܣ�");
				LED_RED;
			}
		}
		/*��ⰴ��*/
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
			/*LED��ת*/
//			LED3_TOGGLE;			
			
				/*��̬��������ͷ��ģʽ��
			    ����Ҫ�������ʹ�ô��ڡ��û���������ѡ���ȷ�ʽ�޸���Щ������
			    �ﵽ��������ʱ��������ͷģʽ��Ŀ��*/
			
				cam_mode.QVGA_VGA = 0,	//QVGAģʽ
				cam_mode.cam_sx = 0,
				cam_mode.cam_sy = 0,	

				cam_mode.cam_width = 320,
				cam_mode.cam_height = 240,

				cam_mode.lcd_sx = 0,
				cam_mode.lcd_sy = 0,
				cam_mode.lcd_scan = 3, //LCDɨ��ģʽ�����������ÿ���1��3��5��7ģʽ

				//���¿ɸ����Լ�����Ҫ������������Χ���ṹ�����Ͷ���	
				cam_mode.light_mode = 0,//�Զ�����ģʽ
				cam_mode.saturation = 0,	
				cam_mode.brightness = 0,
				cam_mode.contrast = 0,
				cam_mode.effect = 1,		//�ڰ�ģʽ
			
			/*��������ͷ����д������*/
			OV7725_Special_Effect(cam_mode.effect);
			/*����ģʽ*/
			OV7725_Light_Mode(cam_mode.light_mode);
			/*���Ͷ�*/
			OV7725_Color_Saturation(cam_mode.saturation);
			/*���ն�*/
			OV7725_Brightness(cam_mode.brightness);
			/*�Աȶ�*/
			OV7725_Contrast(cam_mode.contrast);
			/*����Ч��*/
			OV7725_Special_Effect(cam_mode.effect);
			
			/*����ͼ�������ģʽ��С*/
			OV7725_Window_Set(cam_mode.cam_sx,
																cam_mode.cam_sy,
																cam_mode.cam_width,
																cam_mode.cam_height,
																cam_mode.QVGA_VGA);

			/* ����Һ��ɨ��ģʽ */
			ILI9341_GramScan( cam_mode.lcd_scan );
		}
		
		/*ÿ��һ��ʱ�����һ��֡��*/
		if(Task_Delay[0] == 0)  
		{			
			printf("\r\nframe_ate = %.2f fps\r\n",frame_count/10);
			frame_count = 0;
			Task_Delay[0] = 10000;
		}
		
	}
}




/*********************************************END OF FILE**********************/

