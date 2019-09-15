#include "drv_remote.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_nvic.h"
#include "stm32f10x_tim.h"
#include "drv_oled.h"
#include "stm32f10x_systick.h"

/* �����ֵ������ */

static u8 KeyValueCache[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};    // �����ֵ
static u8 Key_Num = 0;                                           // ������������������

//����ң�س�ʼ��
//����IO�Լ���ʱ��4�����벶��
void Remote_Init(void)    			  
{									   
	RCC->APB1ENR|=1<<2;   	//TIM4 ʱ��ʹ�� 
	RCC->APB2ENR|=1<<3;    	//ʹ��PORTBʱ�� 

	GPIOB->CRH&=0XFFFFFF0F;	//PB9 ����  
	GPIOB->CRH|=0X00000080;	//��������     
	GPIOB->ODR|=1<<9;		//PB9 ����
	  
 	TIM4->ARR=10000;  		//�趨�������Զ���װֵ ���10ms���  
	TIM4->PSC=71;  			//Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.	 

	TIM4->CCMR2|=1<<8;		//CC4S=01 	ѡ������� IC4ӳ�䵽TI4��
 	TIM4->CCMR2|=3<<12;  	//IC4F=0011 ���������˲��� 8����ʱ��ʱ�������˲�
 	TIM4->CCMR2|=0<<10;  	//IC4PS=00 	���������Ƶ,����Ƶ 

	TIM4->CCER|=0<<13; 		//CC4P=0	�����ز���
	TIM4->CCER|=1<<12; 		//CC4E=1 	�������������ֵ������Ĵ�����
	TIM4->DIER|=1<<4;   	//����CC4IE�����ж�				
	TIM4->DIER|=1<<0;   	//��������ж�				
	TIM4->CR1->CEN=0x01;    	//ʹ�ܶ�ʱ��4
//  	MY_NVIC_Init(1,3,TIM4_IRQn,2);//��ռ1�������ȼ�3����2
	NVIC_Config(TIM4_IRQn, 1, 3,ENABLE);//��ռ1�������ȼ�3
}

//ң��������״̬
//[7]:�յ����������־
//[6]:�õ���һ��������������Ϣ
//[5]:����	
//[4]:����������Ƿ��Ѿ�������								   
//[3:0]:�����ʱ��
u8 	RmtSta=0;	  	  
u16 Dval;		//�½���ʱ��������ֵ
u32 RmtRec=0;	//������յ�������	   		    
u8  RmtCnt=0;	//�������µĴ���	  
//��ʱ��4�жϷ������	 
void TIM4_IRQHandler(void)
{ 		    	 
	u16 tsr;
	tsr=TIM4->SR;
	if(tsr&0X01)                                    //���
	{
		if(RmtSta&0x80)								//�ϴ������ݱ����յ���
		{	
			RmtSta&=~0X10;							//ȡ���������Ѿ���������
			if((RmtSta&0X0F)==0X00)RmtSta|=1<<6;	//����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ�
			if((RmtSta&0X0F)<14)RmtSta++;
			else
			{
				RmtSta&=~(1<<7);					//���������ʶ
				RmtSta&=0XF0;						//��ռ�����	
			}								 	   	
		}							    
	}
	if(tsr&(1<<4))                                  //CC4IE�ж�
	{	  
		if(RDATA)                                   //�����ز���
		{
  			TIM4->CCER|=1<<13; 						//CC4P=1	����Ϊ�½��ز���
			TIM4->CNT=0;							//��ն�ʱ��ֵ
			RmtSta|=0X10;							//����������Ѿ�������
		}else                                       //�½��ز���
		{
			Dval=TIM4->CCR4;						//��ȡCCR4Ҳ������CC4IF��־λ
  			TIM4->CCER&=~(1<<13);					//CC4P=0	����Ϊ�����ز���
			if(RmtSta&0X10)							//���һ�θߵ�ƽ���� 
			{
 				if(RmtSta&0X80)                     //���յ���������
				{
					
					if(Dval>300&&Dval<800)			//560Ϊ��׼ֵ,560us
					{
						RmtRec<<=1;					//����һλ.
						RmtRec|=0;					//���յ�0	   
					}else if(Dval>1400&&Dval<1800)	//1680Ϊ��׼ֵ,1680us
					{
						RmtRec<<=1;					//����һλ.
						RmtRec|=1;					//���յ�1
					}else if(Dval>2200&&Dval<2600)	//�õ�������ֵ���ӵ���Ϣ 2500Ϊ��׼ֵ2.5ms
					{
						RmtCnt++; 					//������������1��
						RmtSta&=0XF0;				//��ռ�ʱ��		
					}
 				}else if(Dval>4200&&Dval<4700)		//4500Ϊ��׼ֵ4.5ms
				{
					RmtSta|=1<<7;					//��ǳɹ����յ���������
					RmtCnt=0;						//�����������������
				}						 
			}
			RmtSta&=~(1<<4);
		}				 		     	    					   
	}
	TIM4->SR=0;//����жϱ�־λ 	    
}

//����������
//����ֵ:
//	 0,û���κΰ�������
//����,���µİ�����ֵ.
u8 Remote_Scan(void)
{        
	u8 sta=0;       
    u8 t1,t2;  
	if(RmtSta&(1<<6))//�õ�һ��������������Ϣ��
	{ 
	    t1=RmtRec>>24;			//�õ���ַ��
	    t2=(RmtRec>>16)&0xff;	//�õ���ַ���� 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)//����ң��ʶ����(ID)����ַ 
	    { 
	        t1=RmtRec>>8;
	        t2=RmtRec; 	
	        if(t1==(u8)~t2)sta=t1;//��ֵ��ȷ	 
//			OLED_ShowNum(0, 3 ,sta, 6,16,1);//��ʾ��ֵ
		}   
		if((sta==0)||((RmtSta&0X80)==0))//�������ݴ���/ң���Ѿ�û�а�����
		{
		 	RmtSta&=~(1<<6);//������յ���Ч������ʶ
			RmtCnt=0;		//�����������������
		}
	}  
    return sta;
}

void Remote_Discern(void)
{
	static u8 Sign_2 = 0;    // ���� 1 ��־
	static u8 Sign_1 = 0;    // ���� 2 ��־
    static u32 xC = 1080;
	
	if (K_UP == 1)           // ���� S1 ����
	{
		Sign_1 = 1;          // �������±�־
	}
	else if (Sign_1 == 1)    // ���� S1 �̰�����
	{
		Sign_1 = 0;          // ��λ����
		
		/* ���ΰ��� */

		xC += 100;
	}

	if (K_DN == 1)           // ���� S1 ����
	{
		Sign_2 = 1;          // �������±�־
	}
	else if (Sign_2 == 1)    // ���� S1 �̰�����
	{
		Sign_2 = 0;          // ��λ����
		
		/* ���ΰ��� */

		xC -= 100;
	}

	OLED_ShowNum(0, 0 ,xC, 6,16,1);
	
	TIM2_CCV(xC);
	
	if(K_1)
	{
		OLED_ShowString(0, 4,"K_1 ", 16,1);	//��ʾSYMBOL
	}
	
	
	else if (K_2)
	{
		OLED_ShowString(0, 4,"K_2 ", 16,1);	//��ʾSYMBOL
	}
	else if (K_3)
	{
		OLED_ShowString(0, 4,"K_3 ", 16,1);	//��ʾSYMBOL
	}
	else if (K_4)
	{
		OLED_ShowString(0, 4,"K_4 ", 16,1);	//��ʾSYMBOL
	}
	else if (K_5)
	{
		OLED_ShowString(0, 4,"K_5 ", 16,1);	//��ʾSYMBOL
	}
	else if (K_6)
	{
		OLED_ShowString(0, 4,"K_6 ", 16,1);	//��ʾSYMBOL
	}
	else if (K_7)
	{
		OLED_ShowString(0, 4,"K_7 ", 16,1);	//��ʾSYMBOL
	}
	else if (K_8)
	{
		OLED_ShowString(0, 4,"K_8 ", 16,1);	//��ʾSYMBOL
	}
	else if (K_9)
	{
		OLED_ShowString(0, 4,"K_9 ", 16,1);	//��ʾSYMBOL
	}
	else if (K_0)
	{
		OLED_ShowString(0, 4,"K_0 ", 16,1);	//��ʾSYMBOL
	}
	else if (K_A)
	{
		OLED_Display_Off();                 // �ر� OLED
		OLED_ShowString(0, 4,"K_* ", 16,1);	// ��ʾSYMBOL
	}
	else if (K_P)
	{
		OLED_Display_On();                  // �� OLED
		OLED_ShowString(0, 4,"K_# ", 16,1);	// ��ʾSYMBOL
	}
	else if (K_OK)
	{
		OLED_Clear();                       // ����
		OLED_ShowString(0, 4,"K_OK", 16,1);	// ��ʾSYMBOL
	}
	else if (K_UP)
	{
		OLED_ShowString(0, 4,"K_UP", 16,1);	// ��ʾSYMBOL
	}
	else if (K_R)
	{
		OLED_ShowString(0, 4,"K_R ", 16,1);	//��ʾSYMBOL
	}
	else if (K_L)
	{
		OLED_ShowString(0, 4,"K_L ", 16,1);	//��ʾSYMBOL
	}
	else if (K_DN)
	{
		OLED_ShowString(0, 4,"K_DN", 16,1);	//��ʾSYMBOL
	}
}

/****************************************************************************************************
 * ������ʹ�þ�������� 0-9
 *
 * ��������
 *
 * ���أ���
****************************************************************************************************/

void Using_Matrix_Keyboard(void)
{
	static u8 K1_flag = 0;    // ���� 1 ��־
	static u8 K2_flag = 0;    // ���� 2 ��־
	static u8 K3_flag = 0;    // ���� 3 ��־
	static u8 K4_flag = 0;    // ���� 4 ��־
	static u8 K5_flag = 0;    // ���� 5 ��־
	static u8 K6_flag = 0;    // ���� 6 ��־
	static u8 K7_flag = 0;    // ���� 7 ��־
	static u8 K8_flag = 0;    // ���� 8 ��־
	static u8 K9_flag = 0;    // ���� 9 ��־
	static u8 K0_flag = 0;    // ���� 0 ��־
	static u8 KA_flag = 0;    // ���� * ��־

	if(K_1 ^ K1_flag )             // ���� 1 ״̬�ı�
	{ 
		K1_flag = K_1;             // ���°���״̬
		
		if (K_1)                   // ���� 1 ����
		{
			Matrix_Keyboard(1);    // ���þ��󰴼�������
		}
	}
	
	if(K_2 ^ K2_flag )             // ���� 2 ״̬�ı�
	{ 
		K2_flag = K_2;             // ���°���״̬
		
		if (K_2)                   // ���� 2 ����
		{
			Matrix_Keyboard(2);    // ���þ��󰴼�������
		}
	}
	
	if(K_3 ^ K3_flag )             // ���� 3 ״̬�ı�
	{ 
		K3_flag = K_3;             // ���°���״̬
		
		if (K_3)                   // ���� 3 ����
		{
			Matrix_Keyboard(3);    // ���þ��󰴼�������
		}
	}
	
	if(K_4 ^ K4_flag )             // ���� 4 ״̬�ı�
	{ 
		K4_flag = K_4;             // ���°���״̬
		
		if (K_4)                   // ���� 4 ����
		{
			Matrix_Keyboard(4);    // ���þ��󰴼�������
		}
	}
	
	if(K_5 ^ K5_flag )             // ���� 5 ״̬�ı�
	{ 
		K5_flag = K_5;             // ���°���״̬
		
		if (K_5)                   // ���� 5 ����
		{
			Matrix_Keyboard(5);    // ���þ��󰴼�������
		}
	}
	
	if(K_6 ^ K6_flag )             // ���� 6 ״̬�ı�
	{ 
		K6_flag = K_6;             // ���°���״̬
		
		if (K_6)                   // ���� 6 ����
		{
			Matrix_Keyboard(6);    // ���þ��󰴼�������
		}
	}
	
	if(K_7 ^ K7_flag )             // ���� 7 ״̬�ı�
	{ 
		K7_flag = K_7;             // ���°���״̬
		
		if (K_7)                   // ���� 7 ����
		{
			Matrix_Keyboard(7);    // ���þ��󰴼�������
		}
	}
	
	if(K_8 ^ K8_flag )             // ���� 8 ״̬�ı�
	{ 
		K8_flag = K_8;             // ���°���״̬
		
		if (K_8)                   // ���� 8 ����
		{
			Matrix_Keyboard(8);    // ���þ��󰴼�������
		}
	}
	
	if(K_9 ^ K9_flag )             // ���� 9 ״̬�ı�
	{ 
		K9_flag = K_9;             // ���°���״̬
		
		if (K_9)                   // ���� 9 ����
		{
			Matrix_Keyboard(9);    // ���þ��󰴼�������
		}
	}
	
	if(K_0 ^ K0_flag )             // ���� 0 ״̬�ı�
	{ 
		K0_flag = K_0;             // ���°���״̬
		
		if (K_0)                   // ���� 0 ����
		{
			Matrix_Keyboard(0);    // ���þ��󰴼�������
		}
	}
	
	if(K_A ^ KA_flag )             // ���� * ״̬�ı�
	{ 
		KA_flag = K_A;             // ���°���״̬
		
		if (K_A)                   // ���� * ����
		{
			Key_Num--;             // ��������������һ
		}
	}
}

/****************************************************************************************************
 * ��������ȡ��������� 0-9 ��ת��ֵ
 *
 * ��������
 *
 * ���أ������������ֵ
****************************************************************************************************/

u32 Get_Keyboard_Val(void)
{ 
	u32 Temp_xC = 1;      // �м����
	u32 Temp = 0;         // ��ʱ����
	u8 xC = Key_Num;      // ��ֵ����
	
	while (xC)            // ��ֵ������Ϊ 0
	{
		Temp += KeyValueCache[--xC] * Temp_xC;    // �ϲ�����ֵ
		Temp_xC *= 10;                            // �м�����۳�
	}
	
	return Temp;          // ���ذ���ֵ
}

/****************************************************************************************************
 * ��������������� 0-9 �Ĵ���
 *
 * ������key����ֵ��0-9��
 *
 * ���أ���
****************************************************************************************************/

void Matrix_Keyboard(u8 key)
{
	Key_Num %= 10;                   // ��ֵ����Խ�紦��
	
	KeyValueCache[Key_Num] = key;    // ����ļ�ֵ
	Key_Num++;                       // ��ֵ��������
}

/****************************************************************************************************
 * ������������̸�λ
 *
 * ��������
 *
 * ���أ���
****************************************************************************************************/

void Matrix_Keyboard_Rst(void)
{
	Key_Num = 0;    // ��λ�������
}

/* End Of File */
