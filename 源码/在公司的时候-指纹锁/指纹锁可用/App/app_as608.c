/****************************************************************************************************
 * ������ָ��Ӧ�ó���ʵ��
 *
 * ���ߣ�������
 *
 * �汾��v1.0.0    ���ڣ�2018-07-29
 *                                                                              �����Ǻ��Ƽ����޹�˾
****************************************************************************************************/

#include "stm32f10x_systick.h" 
#include "stm32f10x_tim.h"
#include "drv_remote.h"
#include "app_as608.h"
#include "drv_as608.h"
#include "drv_oled.h"

/****************************************************************************************************
 * ������ָ��¼��ʵ��
 *
 * ��������
 *
 * ���أ���
****************************************************************************************************/

void Add_FR(void)
{
	u8 i=0,ensure ,processnum=0;
	u16 ID;
	while(1)
	{
		OLED_Clear();                   // ���� OLED
		switch (processnum)
		{
			case 0:
				i++;
//				LCD_Fill(0,100,lcddev.width,160,WHITE);
//				Show_Str_Mid(0,100,"Pleas touch finger!",16,240);//�밴��ָ
				OLED_ShowString(0, 2, "Pleas touch finger!", 8, 1);    //�밴��ָ
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer1);//��������
					if(ensure==0x00)
					{
//						LCD_Fill(0,120,lcddev.width,160,WHITE);
//						Show_Str_Mid(0,120,"Fingerprint correct",16,240);//ָ����ȷ
						i=0;
						processnum=1;//�����ڶ���						
					}else ShowErrMessage(ensure);				
				}else ShowErrMessage(ensure);						
				break;
			
			case 1:
				i++;
//				LCD_Fill(0,100,lcddev.width,160,WHITE);
//				Show_Str_Mid(0,100,"Pleas touch finger again!",16,240);//�ٰ�һ����ָ
				OLED_ShowString(0, 2, "Pleas touch finger again!", 8, 1);    //�ٰ�һ����ָ
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer2);//��������			
					if(ensure==0x00)
					{
//						LCD_Fill(0,120,lcddev.width,160,WHITE);
//						Show_Str_Mid(0,120,"Fingerprint correct",16,240);//ָ����ȷ
						i=0;
						processnum=2;//����������
					}else ShowErrMessage(ensure);	
				}else ShowErrMessage(ensure);		
				break;

			case 2:
//				LCD_Fill(0,100,lcddev.width,160,WHITE);
//				Show_Str_Mid(0,100,"Compare twice fingerprint",16,240);//�Ա�����ָ��
				ensure=PS_Match();
				if(ensure==0x00) 
				{
//					LCD_Fill(0,120,lcddev.width,160,WHITE);
//					Show_Str_Mid(0,120,"Twice fingerprint are same",16,240);//����ָ����һ����
					processnum=3;//�������Ĳ�
				}
				else 
				{
//					LCD_Fill(0,100,lcddev.width,160,WHITE);
//					Show_Str_Mid(0,100,"Compare fail,pleas touch again!",16,240);//�Ա�ʧ�ܣ������°���ָ
					ShowErrMessage(ensure);
					i=0;
					processnum=0;//���ص�һ��		
				}
				Delay_ms(1000);
				break;

			case 3:
//				LCD_Fill(0,100,lcddev.width,160,WHITE);
//				Show_Str_Mid(0,100,"Generate fingerprint template",16,240);//����һ��ָ��ģ��
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
//					LCD_Fill(0,120,lcddev.width,160,WHITE);
//					Show_Str_Mid(0,120,"Generate fingerprint success",16,240);//����ָ��ģ��ɹ�
					OLED_ShowString(0, 2, "Generate fingerprint success", 8, 1);    // ����ָ��ģ��ɹ�
					processnum=4;//�������岽
				}else {processnum=0;ShowErrMessage(ensure);}
				Delay_ms(1000);
				break;
				
			case 4:	
//				LCD_Fill(0,100,lcddev.width,160,WHITE);
//				Show_Str_Mid(0,100,"Intput ID and save with ENTER!",16,240);//����ID������Enter������
				OLED_ShowString(0, 5, "0=<ID<=299", 12, 1);
//				Show_Str_Mid(0,120,"0=< ID <=299",16,240);	
//				do
//					ID=Get_Keyboard_Val();
//				while(!(ID<300));//����DI����С��300

				while(!K_P)                     // ����DI����С��300
				{
					Using_Matrix_Keyboard();    // ���þ������
					OLED_ShowNum(0, 7, Get_Keyboard_Val(), 5, 12, 1);
				}
				
				ID=Get_Keyboard_Val();          // ��ȡ������������ֵ
				Matrix_Keyboard_Rst();          // ��λ�������
				
				ensure=PS_StoreChar(CharBuffer2,ID);    // ����ģ��
				if(ensure==0x00) 
				{			
//					LCD_Fill(0,100,lcddev.width,160,WHITE);					
//					Show_Str_Mid(0,120,"Add fingerprint success!!!",16,240);
					OLED_ShowString(0, 2, "Add fingerprint success!!!", 8, 1);    // ���ָ�Ƴɹ�
//					PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
//					LCD_ShowNum(80,80,AS608Para.PS_max-ValidN,3,16);//��ʾʣ��ָ�Ƹ���
					Delay_ms(1500);//��ʱ�������ʾ	
//					LCD_Fill(0,100,240,160,WHITE);
					return ;
				}else {processnum=0;ShowErrMessage(ensure);}					
				break;				
		}
		Delay_ms(800);
		if(i==5)//����5��û�а���ָ���˳�
		{
			OLED_Clear();                   // ���� OLED
//			LCD_Fill(0,100,lcddev.width,160,WHITE);
			break;	
		}
	}
}

/****************************************************************************************************
 * ������ˢָ��ʵ��
 *
 * ��������
 *
 * ���أ���
****************************************************************************************************/

u32 kai = 500;
u32 guan = 950;

void press_FR(void)
{
	SearchResult seach;
	u8 ensure;
//	char *str;
	ensure=PS_GetImage();
	if(ensure==0x00)//��ȡͼ��ɹ� 
	{	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //���������ɹ�
		{		
			ensure=PS_HighSpeedSearch(CharBuffer1,0,300,&seach);
			
			if(ensure==0x00)//�����ɹ�
			{				
//				LCD_Fill(0,100,lcddev.width,160,WHITE);
//				Show_Str_Mid(0,100,"Search fingerprint success",16,240);//����ָ�Ƴɹ�				
//				str=mymalloc(50);
//				sprintf(str,"Match ID:%d  Match score:%d",seach.pageID,seach.mathscore);//��ʾƥ��ָ�Ƶ�ID�ͷ���
//				Show_Str_Mid(0,140,(u8*)str,16,240);
//				myfree(str);
				OLED_ShowString(0, 3, "OK", 8, 1);    // ����ָ��ģ��ɹ�
				
				/* ���������� */
				
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
		Delay_ms(1000);//��ʱ�������ʾ
		
	/* ���������� */
		
		TIM2_CCV(guan);
				
	 OLED_Clear();                   // ���� OLED
	}
		
}

/****************************************************************************************************
 * ��������ʾȷ���������Ϣ
 *
 * ������ensure��������
 *
 * ���أ���
****************************************************************************************************/

void ShowErrMessage(u8 ensure)
{
	OLED_ShowNum_NL(0, 0, ensure, 16, 1);    // ��ʾ������
}

/****************************************************************************************************
 * ��������������
 *
 * ��������
 *
 * ���أ���
****************************************************************************************************/

void AS608_Task(void)
{
	static u8 K1_flag = 0;    // ���� 1 ��־
	static u8 K2_flag = 0;    // ���� 2 ��־
	static u8 K4_flag = 0;    // ���� 1 ��־
	static u8 K7_flag = 0;    // ���� 2 ��־
	static u8 K6_flag = 0;    // ���� 1 ��־
	static u8 K9_flag = 0;    // ���� 2 ��־
	
	
	if(K_1 ^ K1_flag )             // ���� 1 ״̬�ı�
	{ 
		K1_flag = K_1;             // ���°���״̬
		
		if (K_1)                   // ���� 1 ����
		{
			Add_FR();    // ָ��¼��
		}
	}
	
	if(K_2 ^ K2_flag )             // ���� 2 ״̬�ı�
	{ 
		K2_flag = K_2;             // ���°���״̬
		
		if (K_2)                   // ���� 2 ����
		{
			PS_Empty();    // ���ָ�ƿ�
			OLED_ShowString(0, 2, "OK", 8, 1);    // ����ָ��ģ��ɹ�
			Delay_ms(1000);
			OLED_Clear();                   // ���� OLED
		}
	}

	if (PS_Sta)
	{
		press_FR();    // ˢָ��
	}
	
	/* ���ÿ��� */
	
	if(K_4 ^ K4_flag )             // ���� 4 ״̬�ı�
	{ 
		K4_flag = K_4;             // ���°���״̬
		
		if (K_4)                   // ���� 1 ����
		{
			kai += 50;
			TIM2_CCV(kai);
		}
	}
	
	if(K_7 ^ K7_flag )             // ���� 7 ״̬�ı�
	{ 
		K7_flag = K_7;             // ���°���״̬
		
		if (K_7)                   // ���� 7 ����
		{
			kai -= 50;
			TIM2_CCV(kai);
		}
	}
	
	if(K_6 ^ K6_flag )             // ���� 4 ״̬�ı�
	{ 
		K6_flag = K_6;             // ���°���״̬
		
		if (K_6)                   // ���� 1 ����
		{
			guan += 50;
			TIM2_CCV(guan);
		}
	}
	
	if(K_9 ^ K9_flag )             // ���� 7 ״̬�ı�
	{ 
		K9_flag = K_9;             // ���°���״̬
		
		if (K_9)                   // ���� 7 ����
		{
			guan -= 50;
			TIM2_CCV(guan);
		}
	}
	
	OLED_ShowNum(0, 7, kai, 5, 12, 1);
	OLED_ShowNum(64, 7, guan, 5, 12, 1);
}

/* End Of File */
