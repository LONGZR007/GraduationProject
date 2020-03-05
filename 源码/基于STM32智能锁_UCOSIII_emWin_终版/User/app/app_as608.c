/****************************************************************************************************
 * ������ָ��Ӧ�ó���ʵ��
 *
 * ���ߣ�������
 *
 * �汾��v1.0.0    ���ڣ�2018-07-29
 *                                                                              �����Ǻ��Ƽ����޹�˾
****************************************************************************************************/

#include "stm32f10x_tim.h"
#include "app_as608.h"
#include "drv_as608.h"
#include "includes.h"

/****************************************************************************************************
 * ������ˢָ��ʵ��
 *
 * ��������
 *
 * ���أ���
****************************************************************************************************/

u8 ERR_Count = 0;     // �������

void press_FR(void)
{ 
    OS_ERR err;
    struct rtc_time time;
	SearchResult seach;
	u8 ensure;
    /* ��ȡ��ǰʱ�� */
    RTC_TimeCovr(&time);
	ensure=PS_GetImage();
	if(ensure==0x00)//��ȡͼ��ɹ� 
	{	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //���������ɹ�
		{		
			ensure=PS_HighSpeedSearch(CharBuffer1,0,300,&seach);
			
			if(ensure==0x00)//�����ɹ�
			{				
                ERR_Count = 0;    // �����������
                printf("ָ��: %d �ɹ�����-%d_%d_%d_%d\n", seach.pageID,
                        time.tm_mon,time.tm_mday, time.tm_hour, time.tm_min);    // �ϱ��ɹ�������ָ�Ʊ�ź�ʱ��
                /* ���������ź������������� */
                OSTaskSemPost(&AppTaskLockTCB,
                             (OS_OPT   )OS_OPT_POST_NONE,
                             (OS_ERR  *)&err);
			}
			else
            {
                ERR_Count++;            // �������
                printf("ָ�ƿ�����%d��ʧ��-%d_%d_%d_%d\n", ERR_Count, time.tm_mon, 
                                          time.tm_mday, time.tm_hour, time.tm_min);    // �ϱ�ָ�ƿ���ʧ��ʱ��ʹ���
                ShowErrMessage();	    // ��ʾ������Ϣ
                if (ERR_Count >= ERR_MAX)
                {
                    OSTaskSemPost(&AppTaskShotTCB,
                              (OS_OPT   )OS_OPT_POST_NONE,
                              (OS_ERR  *)&err);
                }
            }				
		}
		else
        {
            ERR_Count++;            // �������
            printf("ָ�ƿ�����%d��ʧ��-%d_%d_%d_%d\n", ERR_Count, time.tm_mon, time.tm_mday,
                                                                  time.tm_hour, time.tm_min);    // �ϱ�ָ�ƿ���ʧ��ʱ��ʹ���
            ShowErrMessage();	    // ��ʾ������Ϣ
            if (ERR_Count >= ERR_MAX)
            {
                OSTaskSemPost(&AppTaskShotTCB,
                          (OS_OPT   )OS_OPT_POST_NONE,
                          (OS_ERR  *)&err);
            }
        }
	}
    else
    {
        ERR_Count++;            // �������
        printf("ָ�ƿ�����%d��ʧ��-%d_%d_%d_%d\n", ERR_Count, time.tm_mon, time.tm_mday,
                                                             time.tm_hour, time.tm_min);    // �ϱ�ָ�ƿ���ʧ��ʱ��ʹ���
        ShowErrMessage();	    // ��ʾ������Ϣ
        if (ERR_Count >= ERR_MAX)
        {
            OSTaskSemPost(&AppTaskShotTCB,
                (OS_OPT   )OS_OPT_POST_NONE,
                (OS_ERR  *)&err);
        }
    }
		
}

/****************************************************************************************************
 * ��������ʾȷ���������Ϣ
 *
 * ������ensure��������
 *
 * ���أ���
****************************************************************************************************/

void ShowErrMessage(void)
{
    /* ����ʧ�� */
    OLED_ShowCHinese(0, 2, 2, 1);
    OLED_ShowCHinese(16, 2, 3, 1);
    OLED_ShowCHinese(32, 2, 4, 1);
    OLED_ShowCHinese(48, 2, 5, 1);
}

/****************************************************************************************************
 * ������������ʾ��ʾ��Ϣ
 *
 * ��������
 *
 * ���أ���
****************************************************************************************************/

void ShowRightMessage(void)
{
    /* �����ɹ� */
    OLED_ShowCHinese(0, 2, 2, 1);
    OLED_ShowCHinese(16, 2, 3, 1);
    OLED_ShowCHinese(32, 2, 0, 1);
    OLED_ShowCHinese(48, 2, 1, 1);
}

/****************************************************************************************************
 * ��������ȡ����ID��
 *
 * ��������
 *
 * ���أ����� ID
****************************************************************************************************/

u8 AS608_GET_Idle_ID(void)
{
    u32 ID_Bufff[7];    // ID ������
    u8 Idle_ID = 0;
    u8 xC = 0;
    u8 yC = 0;
    
    SPI_FLASH_BufferRead((uint8_t *)ID_Bufff, FingerprintNumber_Addr, 7*4);    // ��ȫ��ID
    
    for(xC=0; xC<7; xC++)    // ��������
    {
        for(yC=0; yC<32; yC++)    // ����ȫ��ÿһλ
        {
            if (!((ID_Bufff[xC] >> yC) & 1))    // ���� ID
            {
                Idle_ID = xC * 32 + yC;         // ������� ID ��
                return Idle_ID;                 // ���ؿ��� ID ��
            }
        }
    }
    
    return 0;    // û�п��� ID ���ص�һ��
}

/****************************************************************************************************
 * ��������ȡ���һ��ID��
 *
 * ��������
 *
 * ���أ����� ID
****************************************************************************************************/

u16 AS608_GET_Last_ID(void)
{
    u32 ID_Bufff[7];    // ID ������
    u8 Last_ID = 0;
    u8 xC = 0;
    u8 yC = 0;
    
    SPI_FLASH_BufferRead((uint8_t *)ID_Bufff, FingerprintNumber_Addr, 7*4);    // ��ȫ��ID
    
    for(xC=0; xC<7; xC++)    // ��������
    {
        for(yC=0; yC<32; yC++)    // ����ȫ��ÿһλ
        {
            if (((ID_Bufff[xC] >> yC) & 1))    // ���� ID
            {
                Last_ID = xC * 32 + yC;         // ������Ч ID ��
            }
        }
    }
    
    return Last_ID;    // û�п��� ID ���ص�һ��
}

/****************************************************************************************************
 * ���������ȫ�� ID ��
 *
 * ��������
 *
 * ���أ���
****************************************************************************************************/

void AS608_Clear_All_ID(void)
{
    u32 ID_Bufff[7];    // ID ������
    
    memset(ID_Bufff, 0, 7*4);    // ��ʼ������Ϊ 0
    
    SPI_FLASH_SectorErase(FingerprintNumber_Addr);                 // ���� ID ʹ�õ�����
    
    SPI_FLASH_BufferWrite((uint8_t *)ID_Bufff, FingerprintNumber_Addr, 7*4);    // ���ȫ�� ID
}

/****************************************************************************************************
 * ���������� ID ��
 *
 * ������ID��Ҫ���õ� ID ��
 *
 * ���أ���
****************************************************************************************************/

void AS608_SET_ID(u16 ID)
{
    u32 ID_Bufff[7];    // ID ������
    u8 xC = 0;
    u8 yC = 0;
    
    SPI_FLASH_BufferRead((uint8_t *)ID_Bufff, FingerprintNumber_Addr, 7*4);    // ��ȫ��ID

    xC = ID / 32;
    yC = ID % 32;
    ID_Bufff[xC] |= 1 << yC;                                       // ����һ�� ID
    
    SPI_FLASH_SectorErase(FingerprintNumber_Addr);                 // ���� ID ʹ�õ�����
    
    SPI_FLASH_BufferWrite((uint8_t *)ID_Bufff, FingerprintNumber_Addr, 7*4);    // ���ȫ�� ID
}

/****************************************************************************************************
 * ������ɾ�� ID ��
 *
 * ������ID��Ҫɾ���� ID ��
 *
 * ���أ���
****************************************************************************************************/

void AS608_Del_ID(u16 ID)
{
    u32 ID_Bufff[7];    // ID ������
    u8 xC = 0;
    u8 yC = 0;
    
    SPI_FLASH_BufferRead((uint8_t *)ID_Bufff, FingerprintNumber_Addr, 7*4);    // ��ȫ��ID

    xC = ID / 32;
    yC = ID % 32;
    ID_Bufff[xC] &= ~(1 << yC);                                       // ����һ�� ID
    
    SPI_FLASH_SectorErase(FingerprintNumber_Addr);                 // ���� ID ʹ�õ�����
    
    SPI_FLASH_BufferWrite((uint8_t *)ID_Bufff, FingerprintNumber_Addr, 7*4);    // ���ȫ�� ID
}


/* End Of File */
