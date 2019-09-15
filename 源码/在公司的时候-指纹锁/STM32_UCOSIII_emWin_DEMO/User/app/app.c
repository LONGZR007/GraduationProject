/*
******************************************************************************
* @file    bsp_led.c
* @author  Ұ��
* @version V1.0
* @date    2015-xx-xx
* @brief   ledӦ�ú����ӿ�
******************************************************************************
* @attention
*
* ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
* ��̳    :http://www.firebbs.cn
* �Ա�    :https://fire-stm32.taobao.com
*
******************************************************************************
*/
#include "includes.h"
#include  "app.h"

#define BK_COLOR       GUI_BLACK
#define MIDWIN_COLOR   0X303030
#define BOTWIN_COLOR   0X101010
#define ICON_COLOR     GUI_LIGHTCYAN
#define TEXT_COLOR		 GUI_WHITE
/*
*********************************************************************************************************
*                                         �궨�� 
*********************************************************************************************************
*/
#define ICONVIEW_Width     48   /* �ؼ�ICONVIEW�Ŀ� */  
#define ICONVIEW_Height    48   /* �ؼ�ICONVIEW�ĸߣ�����ͼ���Y������ */  
#define ICONVIEW_XSpace    10   /* �ؼ�ICONVIEW��X������ */ 
#define ICONVIEW_YSpace    12   /* �ؼ�ICONVIEW��Y������ */ 
#define ICONVIEW_XPos	     3    /* �ؼ�ICONVIEW��X�����������*/
#define VIRTUAL_WIN_NUM	   2    /* icon��ҳ�� */
#define BOTWIN_YSpace	     62   /* ��Ļ�·����ִ���Y������*/
#define MIDWIN_yPos		     20   /* �м䴰��Y������ʼλ�ü��������ֿ��ø߶�*/
#define MIDWIN_xPos		     0 	  /* �м䴰��X������ʼλ��*/

/*
*********************************************************************************************************
*                                      ����
*********************************************************************************************************
*/ 
WM_HWIN hFrameClose=0;
GUI_XBF_DATA XBF_Data;
GUI_FONT     XBF_Font;
/*ʱ��ṹ��*/
extern struct rtc_time systmtime;
extern char TimeDisplay;

uint8_t UserApp_Flag=0;//��������־

uint8_t Flag_ICON000  = 0;   /* ICONVIEW�ؼ����µı�־��0��ʾδ���£�1��ʾ���� */
uint8_t Flag_ICON001  = 0;

uint8_t Flag_ICON100  = 0;
uint8_t Flag_ICON101  = 0;
uint8_t Flag_ICON102  = 0;
uint8_t Flag_ICON103  = 0;

uint8_t Flag_ICON104  = 0;
uint8_t Flag_ICON105  = 0;
uint8_t Flag_ICON106  = 0;
uint8_t Flag_ICON107  = 0;

uint8_t Flag_ICON108  = 0;
uint8_t Flag_ICON109  = 0;
uint8_t Flag_ICON110  = 0;
uint8_t Flag_ICON111  = 0;

uint8_t Flag_ICON200  = 0;
uint8_t Flag_ICON201  = 0;
uint8_t Flag_ICON202  = 0;
uint8_t Flag_ICON203  = 0;


/* ��������ICONVIEWͼ��Ĵ��� */
typedef struct {
  const GUI_BITMAP * pBitmap;
  const char       * pText;
} BITMAP_ITEM;

typedef struct WIN_PARA{			//����ʹ�õ����û���������������ڻص�������ʹ��
	int xSizeLCD, ySizeLCD;			//LCD��Ļ�ߴ�
	int xPosWin,  yPosWin;			//���ڵ���ʼλ��
	int xSizeWin, ySizeWin;			//���ڳߴ�	
	int xSizeBM,  ySizeBM;
	int ySizeBotWin;
	
	WM_HWIN hWinBot;				//���ƴ��ڵľ�����ײ���
	WM_HWIN hWinMid;				//����ʾ���ľ�����в���
	
}WIN_PARA;
WIN_PARA WinPara;			//�û�ʹ�õĴ��ڶ�������

/* ��������ICONVIEW0ͼ��Ĵ��� */
static const BITMAP_ITEM _aBitmapItem0[] = {
	{&bmPhone,    "Phone" 		},
//	{&bmmessages, "Message"	}
};
/* ��������ICONVIEW1ͼ��Ĵ��� */
static const BITMAP_ITEM _aBitmapItem1[] = {
  {&bmkey,  	  "KEY"		    },
//  {&bmrgbled,	  "Breathing"	}, 
//  {&bmadc,  	  "ADC"				},
//	{&bmeeprom,   "EEPROM" 		},
	
  {&bmclock,      "Clock" 		},
//  {&bmmusic,    "Music" 		},
//	{&bmrecorder, "Recorder"	},
//	{&bmusb,  	  "USB"		    },
	
	{&bmwifi,	  "Wifi"},	
//	{&bmnet,   	  "Network" 	},	
//	{&bmWeather,  "Humiture"  }, 
  {&bmcamera,     "Camera"		}, 
};

/* ��������ICONVIEW2ͼ��Ĵ��� */
static const BITMAP_ITEM _aBitmapItem2[] = {
//	{&bmcalculator,"Calculator" },
	{&bmpicture,   "Picture" 		},
	{&bmnote,      "Note" 		  },
	{&bmbrowser,   "Browser"		}, 	
};

//void FUN_ICON000Clicked(void)  {printf("FUN_ICON000Clicked\n");}
//void FUN_ICON001Clicked(void)  {printf("FUN_ICON001Clicked\n");}

//void FUN_ICON100Clicked(void)  {printf("FUN_ICON100Clicked\n");}
//void FUN_ICON101Clicked(void)  {printf("FUN_ICON101Clicked\n");}
//void FUN_ICON102Clicked(void)  {printf("FUN_ICON102Clicked\n");}
//void FUN_ICON103Clicked(void)  {printf("FUN_ICON103Clicked\n");}

//void FUN_ICON104Clicked(void)  {printf("FUN_ICON104Clicked\n");}
//void FUN_ICON105Clicked(void)  {printf("FUN_ICON105Clicked\n");}
//void FUN_ICON106Clicked(void)  {printf("FUN_ICON106Clicked\n");}
//void FUN_ICON107Clicked(void)  {printf("FUN_ICON107Clicked\n");}

//void FUN_ICON108Clicked(void)  {printf("FUN_ICON108Clicked\n");}
//void FUN_ICON109Clicked(void)  {printf("FUN_ICON109Clicked\n");}
//void FUN_ICON110Clicked(void)  {printf("FUN_ICON110Clicked\n");}
//void FUN_ICON111Clicked(void)  {printf("FUN_ICON111Clicked\n");}

//void FUN_ICON200Clicked(void)  {printf("FUN_ICON200Clicked\n");}
//void FUN_ICON201Clicked(void)  {printf("FUN_ICON201Clicked\n");}
//void FUN_ICON202Clicked(void)  {printf("FUN_ICON202Clicked\n");}
//void FUN_ICON203Clicked(void)  {printf("FUN_ICON203Clicked\n");}

/*
*********************************************************************************************************
*	�� �� ��: _cbBkWindow
*	����˵��: ���洰�ڵĻص����� 
*	��    �Σ�WM_MESSAGE * pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _cbBkWindow(WM_MESSAGE * pMsg) 
{
	WM_HWIN hText;
	char text_buffer[20]={0};
	switch (pMsg->MsgId) 
	{
		case WM_TIMER://����ʱ����ʾ����Ϣ��ÿ�����޸�һ��ʱ��
			if(!TimeDisplay||UserApp_Flag)
			{
				WM_RestartTimer(pMsg->Data.v, 250);
				break;
			}
			/* ��ȡtext��� */      
			hText = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1);
			
			/* ת��rtcֵ������ʱ�� */
			RTC_TimeCovr(&systmtime);
  
			/* ת�����ַ��� */
			sprintf(text_buffer,"%02d:%02d:%02d",systmtime.tm_hour,systmtime.tm_min,systmtime.tm_sec);
			/* ���ʱ�� */
			TEXT_SetText(hText,text_buffer);
			WM_RestartTimer(pMsg->Data.v, 250);
		break;
		/* �ػ���Ϣ*/
		case WM_PAINT:		
				GUI_SetBkColor(BK_COLOR);
				GUI_Clear();
				ICONVIEW_SetSel(WM_GetDialogItem(WinPara.hWinMid, GUI_ID_ICONVIEW2),-1);
				ICONVIEW_SetSel(WM_GetDialogItem(WinPara.hWinMid, GUI_ID_ICONVIEW1),-1);
				ICONVIEW_SetSel(WM_GetDialogItem(WinPara.hWinBot, GUI_ID_ICONVIEW0),-1);			
		break;
			
	 default:
		WM_DefaultProc(pMsg);
		break;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: _cbMidWin
*	����˵��: 
*	��    �Σ�WM_MESSAGE * pMsg
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _cbMidWin(WM_MESSAGE * pMsg) 
{
	int NCode, Id;
	WM_MOTION_INFO * pInfo;
	switch (pMsg->MsgId) 
	{
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
			NCode = pMsg->Data.v;                 /* Notification code */
			switch (Id) 
			{
				case GUI_ID_ICONVIEW1:
					switch (NCode) 
					{
						/* ICON�ؼ������Ϣ */
						case WM_NOTIFICATION_CLICKED:							
							break;
						
						/* ICON�ؼ��ͷ���Ϣ */
						case WM_NOTIFICATION_RELEASED: 
							UserApp_Flag = 1;
							tpad_flag=0;
							/* ����Ӧѡ�� */
							switch(ICONVIEW_GetSel(pMsg->hWinSrc))
							{
								/* KEY  ******************************************************************/
								case 0:	
									Flag_ICON100 = 1;
									FUN_ICON100Clicked();			
									break;	
								
								/* Breathing ***********************************************************************/
								case 1:
									Flag_ICON101 = 1;
									FUN_ICON101Clicked();
									break;
								
								/* ADC *********************************************************************/
								case 2:
									Flag_ICON102 = 1;
//									FUN_ICON102Clicked();	
									break;
								
								/* EEPROM ********************************************************************/
								case 3:
									Flag_ICON103 = 1;
//									FUN_ICON103Clicked();
									break;
								
								/* Clock **********************************************************************/
								case 4:
									Flag_ICON104 = 1;
									FUN_ICON104Clicked();
									break;
								
								/* Music ********************************************************************/
								case 5:
									Flag_ICON105 = 1;
//									FUN_ICON105Clicked();
									break;
								
								/* Recorder ******************************************************************/
								case 6:					
									Flag_ICON106 = 1;
									FUN_ICON106Clicked();    // 
									break;
								
								/* USB *******************************************************************/
								case 7:
									Flag_ICON107 = 1;
//									FUN_ICON107Clicked();
									break;
								
								/* Wifi ******************************************************************/
								case 8:
									Flag_ICON108 = 1;
									FUN_ICON108Clicked();
									break;
								 
								 /* Network ******************************************************************/
								case 9:
									Flag_ICON109 = 1;
//									FUN_ICON109Clicked();//W5500
									break;
								 
								 /* Humiture ******************************************************************/
								case 10:
									Flag_ICON110 = 1;
//									FUN_ICON110Clicked();
									break;
								 
								/* Camera *******************************************************************/
								case 11:
									Flag_ICON111 = 1;
									FUN_ICON111Clicked();
									break;
								default:break;
								}
							 break;
						}
					break;
				case GUI_ID_ICONVIEW2:
					switch (NCode) 
					{
						/* ICON�ؼ������Ϣ */
						case WM_NOTIFICATION_CLICKED:							
							break;
						
						/* ICON�ؼ��ͷ���Ϣ */
						case WM_NOTIFICATION_RELEASED: 
							UserApp_Flag = 1;
							tpad_flag=0;
							/* ����Ӧѡ�� */
							switch(ICONVIEW_GetSel(pMsg->hWinSrc))
							{			
								
								/* Calculator *******************************************************************/
								case 0:
									Flag_ICON200 = 1;
									FUN_ICON200Clicked();
									break;
								
								/* Picture ***************************************************************/
								case 1:
									Flag_ICON201 = 1;
									FUN_ICON201Clicked();
									break;

								/* Note ***************************************************************/
								case 2:
									Flag_ICON202 = 1;
									FUN_ICON202Clicked();
									break;		
								/* Browser ***************************************************************/
								case 3:
									Flag_ICON203 = 1;
									FUN_ICON203Clicked();
									break;	
								default:break;
							}	
						 break;
					}
				break;
			}
			break;
		
		case WM_MOTION:
			 pInfo = (WM_MOTION_INFO *)pMsg->Data.p;
			 if (pInfo) 
			 {
				//printf("mooton_message=%d\n",pInfo->Cmd);
				 switch (pInfo->Cmd) 
				 {				    
					case WM_MOTION_INIT:
						pInfo->SnapX = 240;		//���ض���
						pInfo->SnapY = 0;
						//pInfo->Flags = WM_CF_MOTION_X | WM_MOTION_MANAGE_BY_WINDOW;
						pInfo->Period = 500;
					break;
					
					case WM_MOTION_MOVE:
						WM_DisableWindow(WM_GetDialogItem(pMsg->hWin,GUI_ID_ICONVIEW1));
						WM_DisableWindow(WM_GetDialogItem(pMsg->hWin,GUI_ID_ICONVIEW2));
					break;							
					case WM_MOTION_GETPOS:
						//printf("\n dx = %d",pInfo->dx);
						//printf(" dy = %d",pInfo->dy);
						//printf(" xpos = %d\n",pInfo->xPos);
						//printf(" ypos = %d\n",pInfo->yPos);
					break;								
					default:
						break;						
				  }
					/* �ƶ����� */
					if (pInfo->FinalMove || ((pInfo->dx<=2)&&(pInfo->dx>=-2)))
					{
						WM_EnableWindow(WM_GetDialogItem(pMsg->hWin,GUI_ID_ICONVIEW1));
						WM_EnableWindow(WM_GetDialogItem(pMsg->hWin,GUI_ID_ICONVIEW2));
					}
			 }	
			
			break;
			
		/* �ػ���Ϣ*/
		case WM_PAINT:
			{
				GUI_SetBkColor(MIDWIN_COLOR);
				GUI_Clear();
			}			
		break;			
	 default:
		WM_DefaultProc(pMsg);
		break;
	}
}

/**
  * @brief  _cbBotWin,�������ص�����
  * @param  none
  * @retval none
  */
static void _cbBotWin(WM_MESSAGE * pMsg)
{
	int NCode, Id;
	switch (pMsg->MsgId) 
	{			 		 
		case WM_PAINT:	                             //�ػ汳��	
			GUI_SetBkColor(BOTWIN_COLOR);
			GUI_Clear();
		break;
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
			NCode = pMsg->Data.v;                 /* Notification code */
		switch (Id) 
		{
			case GUI_ID_ICONVIEW0:
				switch (NCode) 
				{
					/* ICON�ؼ������Ϣ */
					case WM_NOTIFICATION_CLICKED:					
						break;
					
					/* ICON�ؼ��ͷ���Ϣ */
					case WM_NOTIFICATION_RELEASED: 
							UserApp_Flag = 1;
						tpad_flag=0;
						/* ����Ӧѡ�� */
						switch(ICONVIEW_GetSel(pMsg->hWinSrc))
						{
							/* Phone *******************************************************************/
							case 0:	
								Flag_ICON000 = 1;
								FUN_ICON000Clicked();			
								break;	
							
							/* Message ***********************************************************************/
							case 1:
								Flag_ICON001 = 1;
								FUN_ICON001Clicked();
								break;
						}	
				 break;
			}
		break;
	}
	break;		
	default:		
		WM_DefaultProc(pMsg);
	break;		 
	}
}
/**
  * @brief  CreateTopWin�����������Ĵ���
  * @param  none
  * @retval none
  */
static void CreateTopWin(void)
{
	WM_HWIN hText;
	/* ������ "wildfire OS "�ı� */
	hText = TEXT_CreateEx(0, 2, 60 , 16, WM_HBKWIN, WM_CF_SHOW, GUI_TA_LEFT|TEXT_CF_VCENTER, GUI_ID_TEXT0, "Wildfire OS");
	TEXT_SetFont(hText, GUI_FONT_10_ASCII);
	TEXT_SetTextColor(hText,TEXT_COLOR);
	
	/* ״̬����ʱ����ʾ�ı� */
	hText = TEXT_CreateEx(WinPara.xSizeLCD/2-25,2,50,16,WM_HBKWIN,WM_CF_SHOW,GUI_TA_HCENTER|TEXT_CF_VCENTER,GUI_ID_TEXT1,"11:56:00");
	//TEXT_SetBkColor(hText,GUI_INVALID_COLOR);
	TEXT_SetTextColor(hText,TEXT_COLOR);
	TEXT_SetFont(hText,GUI_FONT_8_ASCII);
}
/**
  * @brief  CreateMidWin�������м�Ĵ���
  * @param  none
  * @retval none
  */
static void CreateMidWin(void)
{
	uint8_t i=0;
	WM_HWIN MIDWIN;
	WinPara.hWinMid= WM_CreateWindowAsChild(
											WinPara.xPosWin,											
											WinPara.yPosWin,
											WinPara.xSizeWin,
											WinPara.ySizeWin,	
											WM_HBKWIN, 
											WM_CF_SHOW | WM_CF_MOTION_X, 
											_cbMidWin, 
											sizeof(WIN_PARA *)
											);
/*-------------------------------------------------------------------------------------*/
					/*��ָ��λ�ô���ָ���ߴ��ICONVIEW1 С����*/
					MIDWIN=ICONVIEW_CreateEx(
											 ICONVIEW_XPos, 					/* С���ߵ��������أ��ڸ������У�*/
											 0, 								/* С���ߵ��������أ��ڸ������У�*/
											 WinPara.xSizeLCD-ICONVIEW_XPos,	/* С���ߵ�ˮƽ�ߴ磨��λ�����أ�*/
											 3*(WinPara.ySizeBM+20), 			/* С���ߵĴ�ֱ�ߴ磨��λ�����أ�*/
											 WinPara.hWinMid, 				    /* �����ڵľ�������Ϊ0 ������С���߽���Ϊ���棨�������ڣ����Ӵ��� */
											 WM_CF_SHOW | WM_CF_HASTRANS,       /* ���ڴ�����ǡ�ΪʹС���������ɼ���ͨ��ʹ�� WM_CF_SHOW */ 
											 0,//ICONVIEW_CF_AUTOSCROLLBAR_V, 	/* Ĭ����0�����������ʵ������������ֱ������ */
											 GUI_ID_ICONVIEW1, 			        /* С���ߵĴ���ID */
											 ICONVIEW_Width, 				    /* ͼ���ˮƽ�ߴ� */
											 ICONVIEW_Height+10);				/* ͼ��Ĵ�ֱ�ߴ� */
											 
		/* ��ICONVIEW С���������ͼ�� */
		for (i = 0; i < GUI_COUNTOF(_aBitmapItem1); i++) 
		{
			ICONVIEW_AddBitmapItem(MIDWIN, _aBitmapItem1[i].pBitmap,_aBitmapItem1[i].pText);
			ICONVIEW_SetTextColor(MIDWIN,i,TEXT_COLOR);
		}	
		/* ����С���ߵı���ɫ 32 λ��ɫֵ��ǰ8 λ������alpha��ϴ���Ч��*/
		ICONVIEW_SetBkColor(MIDWIN, ICONVIEW_CI_SEL, ICON_COLOR | 0x80000000);
		/* �������� */
		ICONVIEW_SetFont(MIDWIN, &GUI_Font10_ASCII);
		/* ���ó�ʼѡ���ͼ��Ϊ -1 (��ʾ��δѡ��)*/
		ICONVIEW_SetSel(MIDWIN,-1);
		/* ����ͼ����x ��y �����ϵļ�ࡣ*/
		ICONVIEW_SetSpace(MIDWIN, GUI_COORD_X, ICONVIEW_XSpace);
		ICONVIEW_SetSpace(MIDWIN, GUI_COORD_Y, ICONVIEW_YSpace);
	//	ICONVIEW_SetSpace(hWin, GUI_COORD_Y, ICONVIEW_YSpace);
		/* ���ö��뷽ʽ ��5.22�汾�����¼���� */
		ICONVIEW_SetIconAlign(MIDWIN, ICONVIEW_IA_HCENTER|ICONVIEW_IA_TOP);
		
/*-------------------------------------------------------------------------------------*/
		/*��ָ��λ�ô���ָ���ߴ��ICONVIEW2С����*/
		MIDWIN=ICONVIEW_CreateEx(
								 WinPara.xSizeLCD+ICONVIEW_XPos,	/* С���ߵ��������أ��ڸ������У�*/
								 0, 								/* С���ߵ��������أ��ڸ������У�*/
								 WinPara.xSizeLCD-ICONVIEW_XPos,	/* С���ߵ�ˮƽ�ߴ磨��λ�����أ�*/
								 2*(WinPara.ySizeBM+20),			/* С���ߵĴ�ֱ�ߴ磨��λ�����أ�*/
								 WinPara.hWinMid, 				    /* �����ڵľ�������Ϊ0 ������С���߽���Ϊ���棨�������ڣ����Ӵ��� */
								 WM_CF_SHOW | WM_CF_HASTRANS,       /* ���ڴ�����ǡ�ΪʹС���������ɼ���ͨ��ʹ�� WM_CF_SHOW */ 
								 0,//ICONVIEW_CF_AUTOSCROLLBAR_V, 	/* Ĭ����0�����������ʵ������������ֱ������ */
								 GUI_ID_ICONVIEW2, 			        /* С���ߵĴ���ID */
								 ICONVIEW_Width, 				    /* ͼ���ˮƽ�ߴ� */
								 ICONVIEW_Height+10);				/* ͼ��Ĵ�ֱ�ߴ� */
		/*-------------------------------------------------------------------------------------*/						 
		/* ��ICONVIEW С���������ͼ�� */
		for (i = 0; i < GUI_COUNTOF(_aBitmapItem2); i++) 
		{
			ICONVIEW_AddBitmapItem(MIDWIN, _aBitmapItem2[i].pBitmap,_aBitmapItem2[i].pText);
			ICONVIEW_SetTextColor(MIDWIN,i,TEXT_COLOR);
		}	
		
		/* ����С���ߵı���ɫ 32 λ��ɫֵ��ǰ8 λ������alpha��ϴ���Ч��*/
		ICONVIEW_SetBkColor(MIDWIN, ICONVIEW_CI_SEL, ICON_COLOR | 0x80000000);
		/* �������� */
		ICONVIEW_SetFont(MIDWIN, &GUI_Font10_ASCII);
		/* ���ó�ʼѡ���ͼ��Ϊ -1 (��ʾ��δѡ��)*/
		ICONVIEW_SetSel(MIDWIN,-1);
		/* ����ͼ����x ��y �����ϵļ�ࡣ*/
		ICONVIEW_SetSpace(MIDWIN, GUI_COORD_X, ICONVIEW_XSpace);
		ICONVIEW_SetSpace(MIDWIN, GUI_COORD_Y, ICONVIEW_YSpace);
	//	ICONVIEW_SetSpace(hWin, GUI_COORD_Y, ICONVIEW_YSpace);
		/* ���ö��뷽ʽ ��5.22�汾�����¼���� */
		ICONVIEW_SetIconAlign(MIDWIN, ICONVIEW_IA_HCENTER|ICONVIEW_IA_TOP);
}

/**
  * @brief  CreateBotWin�������ײ��Ĵ���
  * @param  none
  * @retval none
  */
static void CreateBotWin(void)
{
	uint8_t i;
	WM_HWIN BOTWIN;
	WinPara.hWinBot= WM_CreateWindowAsChild(
											0,											
											WinPara.ySizeLCD-WinPara.ySizeBotWin,	//�ײ�λ��
											WinPara.xSizeLCD,
											WinPara.ySizeBotWin,	//�ײ�ʣ����
											WM_HBKWIN, 
											WM_CF_SHOW,
											_cbBotWin, 
											sizeof(WIN_PARA *)
											);
					/*��ָ��λ�ô���ָ���ߴ��ICONVIEW С����*/
					BOTWIN=ICONVIEW_CreateEx(
											 62,                					      /* С���ߵ��������أ��ڸ������У�*/
											 0, 								                /* С���ߵ��������أ��ڸ������У�*/
											 116,                               /* С���ߵ�ˮƽ�ߴ磨��λ�����أ�*/
											 WinPara.ySizeBotWin,				        /* С���ߵĴ�ֱ�ߴ磨��λ�����أ�*/
											 WinPara.hWinBot, 				          /* �����ڵľ�������Ϊ0 ������С���߽���Ϊ���棨�������ڣ����Ӵ��� */
											 WM_CF_SHOW | WM_CF_HASTRANS,       /* ���ڴ�����ǡ�ΪʹС���������ɼ���ͨ��ʹ�� WM_CF_SHOW */ 
											 0,//ICONVIEW_CF_AUTOSCROLLBAR_V, 	/* Ĭ����0�����������ʵ������������ֱ������ */
											 GUI_ID_ICONVIEW0, 			            /* С���ߵĴ���ID */
											 ICONVIEW_Width, 				            /* ͼ���ˮƽ�ߴ� */
											 ICONVIEW_Height);					        /* ͼ��Ĵ�ֱ�ߴ� */
	WM_BringToTop(WinPara.hWinBot);
	/* ��ICONVIEW С���������ͼ�� */
	for (i = 0; i < GUI_COUNTOF(_aBitmapItem0); i++) 
	{
		ICONVIEW_AddBitmapItem(BOTWIN, _aBitmapItem0[i].pBitmap,"");
	}	
	/* ����С���ߵı���ɫ 32 λ��ɫֵ��ǰ8 λ������alpha��ϴ���Ч��*/
	ICONVIEW_SetBkColor(BOTWIN, ICONVIEW_CI_SEL, ICON_COLOR | 0x80000000);
	/* �������� */
//	ICONVIEW_SetFont(hWinicon0, &GUI_Font10_ASCII);
	/* ���ó�ʼѡ���ͼ��Ϊ -1 (��ʾ��δѡ��)*/
	ICONVIEW_SetSel(BOTWIN,-1);
	/* ����ͼ����x ��y �����ϵļ�ࡣ*/
	ICONVIEW_SetSpace(BOTWIN, GUI_COORD_X, ICONVIEW_XSpace);
//	ICONVIEW_SetSpace(hWin, GUI_COORD_Y, ICONVIEW_YSpace);
	/* ���ö��뷽ʽ ��5.22�汾�����¼���� */
	ICONVIEW_SetIconAlign(BOTWIN, ICONVIEW_IA_HCENTER|ICONVIEW_IA_TOP);
}
/*
*********************************************************************************************************
*
*       _cbGetData
*
* Function description
*   Callback function for getting font data
*
* Parameters:
*   Off      - Position of XBF file to be read
*   NumBytes - Number of requested bytes
*   pVoid    - Application defined pointer
*   pBuffer  - Pointer to buffer to be filled by the function
*
* Return value:
*   0 on success, 1 on error
*********************************************************************************************************
*/
static int _cbGetData(U32 Offset, U16 NumBytes, void * pVoid, void * pBuffer)
{
	SPI_FLASH_BufferRead(pBuffer,60*4096+Offset,NumBytes);
	return 0;
}

static void _CreateXBF(void) 
{
	//
	// Create XBF font
	//
	GUI_XBF_CreateFont(&XBF_Font,       // Pointer to GUI_FONT structure in RAM
					   &XBF_Data,         // Pointer to GUI_XBF_DATA structure in RAM
					   GUI_XBF_TYPE_PROP,//GUI_XBF_TYPE_PROP_EXT, 		// Font type to be created
					   _cbGetData,        // Pointer to callback function
					   0);        // Pointer to be passed to GetData function
}

/*
*********************************************************************************************************
*	�� �� ��: UserAPP
*	����˵��: GUI������ 
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void UserAPP(void)
{
	UserApp_Flag=0;
	//׼������3�����ڣ�������ʹ�õ����û���������������ڻص�������ʹ��
	WinPara.xSizeLCD = LCD_GetXSize();				//LCD��Ļ�ߴ�
	WinPara.ySizeLCD = LCD_GetYSize();				//LCD��Ļ�ߴ�
	WinPara.xSizeBM  = ICONVIEW_Width;				//ͼ����
	WinPara.ySizeBM  = ICONVIEW_Height;				//ͼ��߶�
	WinPara.ySizeBotWin=BOTWIN_YSpace;				//�����·����ڸ߶�
	WinPara.xSizeWin = WinPara.xSizeLCD*VIRTUAL_WIN_NUM;						//���ڳߴ�
	WinPara.ySizeWin = WinPara.ySizeLCD-WinPara.ySizeBotWin-WinPara.yPosWin;	//���ڳߴ�
	WinPara.xPosWin	 = MIDWIN_xPos;							//���ڵ���ʼλ��
	WinPara.yPosWin  = MIDWIN_yPos;							//���ڵ���ʼλ��
	WM_SetSize(WM_HBKWIN,WinPara.xSizeLCD,WinPara.ySizeLCD);//���ñ������ڴ�С����֤�м䴰���ƶ�Ч��
	
	/***************************����Ƥ��ɫ*****************************/
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	MULTIPAGE_SetDefaultSkin(MULTIPAGE_SKIN_FLEX);
	/***************************����Ĭ������**********************************/
	_CreateXBF();
	GUI_UC_SetEncodeUTF8();
  GUI_SetDefaultFont(&XBF_Font);
	/*************************************************************************/
	
	WM_MOTION_Enable(1);
	WM_MOTION_SetDefaultPeriod(500);
	WM_SetCallback(WM_HBKWIN, _cbBkWindow);
	WM_CreateTimer(WM_HBKWIN, 0, 250, 0);
	/* ������������ ״̬������������������*/
	CreateMidWin();
	CreateTopWin();
	CreateBotWin();
	GUI_Delay(10);
	LCD_BK_EN;
	while(1)
	{
		GUI_Delay(5);
	}
}
