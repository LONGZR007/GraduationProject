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

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/
static  OS_TCB	 AppTaskStartTCB;		     //����������ƿ�
        OS_TCB   AppTaskShotTCB;
static  OS_TCB   AppTaskUserappTCB;
        OS_TCB   AppTaskKeypadTCB;
        OS_TCB   AppTaskLockTCB;
        OS_TCB   AppTaskBellTCB;
        OS_TCB   AppTaskBrushTCB;
        OS_TCB   AppTaskWiFiTCB;
/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/
static	CPU_STK	 AppTaskStartStk[APP_TASK_START_STK_SIZE];	   //���������ջ
static  CPU_STK  AppTaskCOMStk[APP_TASK_SHOT_STK_SIZE];
static  CPU_STK  AppTaskLockStk[APP_TASK_Lock_STK_SIZE];
static  CPU_STK  AppTaskBellStk[APP_TASK_Bell_STK_SIZE];
static  CPU_STK  AppTaskBrushStk[APP_TASK_Brush_STK_SIZE];
static  CPU_STK  AppTaskWiFiStk[APP_TASK_Brush_STK_SIZE];
__align(8) static  CPU_STK  AppTaskUserappStk[APP_TASK_USERAPP_STK_SIZE];
__align(8) static  CPU_STK  AppTaskKeypadStk[APP_TASK_KEYPAD_STK_SIZE];
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

OS_Q queue;                             // ������Ϣ����

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static  void   AppTaskStart(void *p_arg);
static  void   AppTaskCreate(void);
static  void   AppObjCreate (void);
static  void   AppTaskShot(void *p_arg);
static  void   AppTaskUserapp(void *p_arg);
static  void   AppTaskWiFi(void *p_arg);


/*********************************************************************
*
*       data
*
**********************************************************************
*/
/*---------- kernel objects ----------*/
//static  OS_SEM   SEM_SHOT;
// OS_SEM   SEM_Lock;

/*---------- FatFS data ----------*/
FIL 	file;				/* file objects */
FRESULT result; 
UINT 	bw;  			/* File R/W count */

uint8_t key_flag=1;//1����������ͼ��0�����ð�����ͼ
uint8_t IsCal =0;

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	OS_ERR err;	
	
	/* ��ʼ��"uC/OS-III"�ں� */  
	OSInit(&err);	
	
	/*��������*/
	OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,              // ������ƿ�ָ��          
               (CPU_CHAR   *)"App Task Start",		          // ��������
               (OS_TASK_PTR )AppTaskStart, 	                  // �������ָ��
               (void       *)0,			                      // ���ݸ�����Ĳ���parg
               (OS_PRIO     )APP_TASK_START_PRIO,			  // �������ȼ�
               (CPU_STK    *)&AppTaskStartStk[0],	          // �����ջ����ַ
               (CPU_STK_SIZE)APP_TASK_START_STK_SIZE/10,	  // ��ջʣ�ྯ����
               (CPU_STK_SIZE)APP_TASK_START_STK_SIZE,		  // ��ջ��С
               (OS_MSG_QTY  )1u,			                      // �ɽ��յ������Ϣ������
               (OS_TICK     )0u,			                      // ʱ��Ƭ��תʱ��
               (void       *)0,			                      // ������ƿ���չ��Ϣ
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | 
                              OS_OPT_TASK_STK_CLR),	      // ����ѡ��
               (OS_ERR     *)&err);		                    // ����ֵ
	  
  /* ����������ϵͳ������Ȩ����uC/OS-III */
  OSStart(&err);                                       
}

/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static void  AppTaskStart(void *p_arg)
{
    OS_ERR err;
    
   (void)p_arg;
	
	/* �弶��ʼ�� */	
    BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();
	BSP_Tick_Init();

//    Mem_Init();                                                 /* Initialize Memory Management Module                  */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif
    
//    OS_INFO("Creating Application Tasks...\n\r");
    AppTaskCreate();                                            /* Create Application Tasks                             */
//    OS_INFO("Creating Application Events...\n\r");
    AppObjCreate();                                             /* Create Application Objects  */

    while (DEF_TRUE) {/* Task body, always written as an infinite loop.       */
        GUI_TOUCH_Exec();
        
        /* ����ɨ�� */
        PasswordUnlock();
        
        OSTimeDlyHMSM(0, 0, 0, 15,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskKeypad
*	����˵��: �������뷨��������
*	��    �Σ�p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ����6
*********************************************************************************************************
*/
extern void keypad(uint16_t *message);
static  void   AppTaskKeypad(void *p_arg)
{
	OS_ERR   err;
	uint16_t *msg;
	OS_MSG_SIZE	msg_size;
	CPU_TS			ts;
	(void)p_arg;
	OS_DEBUG_FUNC();
	while(1)
	{
		msg = OSTaskQPend(0,
						OS_OPT_PEND_BLOCKING,
						&msg_size,
						&ts,
						&err);
//        printf("����\n");
		if(err == OS_ERR_NONE)
		{
			keypad(msg);
		}
	}
}

///*
//*********************************************************************************************************
//*	�� �� ��: _WriteByte2File()
//*	����˵��: 
//*	��    �Σ�  	
//*	�� �� ֵ: 
//*********************************************************************************************************
//*/
//static void _WriteByte2File(uint8_t Data, void * p) 
//{
//	result = f_write(p, &Data, 1, &bw);
//}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCom
*	����˵��: 
*	��    �Σ�p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ����3
*********************************************************************************************************
*/

int Screen_Shot( uint16_t x, uint16_t y, uint16_t Width, uint16_t Height, char * filename);
void Camera_Display(void);
WM_HWIN hWinCamera;
static void AppTaskShot(void *p_arg)
{	
	OS_ERR  err;
	CPU_TS 	Ts;
	uint8_t i;
	char 	buf[20];
    struct rtc_time time;
	(void)p_arg;
	OS_DEBUG_FUNC();
	while(1)
	{
		//��������ֱ������֪ͨ
        OSTaskSemPend((OS_TICK )0,                    // ��������������Ϊ0��һֱ�ȴ���ȥ
                      (OS_OPT )OS_OPT_PEND_BLOCKING,  // ���û���ź������þ͵ȴ�
                      (CPU_TS *)&Ts,                   // ���������ָ�����ź������ύ��ǿ�ƽ���ȴ�״̬�������ź�����ɾ����ʱ���        
                      (OS_ERR *)&err);
        
        if (ERR_Count >= ERR_MAX)
        {
            Camera_Display();
        }
        
        /* ��ȡ��ǰʱ�� */
		RTC_TimeCovr(&time);
        
		i = 0;
        
		while(i<0xff)
		{
			sprintf((char *)buf,"1:/%d_%d_%d_%d_%d.bmp",time.tm_mon,time.tm_mday, time.tm_hour, time.tm_min,i);
			result=f_open(&file,(const TCHAR*)buf,FA_READ);
			if(result==FR_NO_FILE)break;	
			else
				f_close(&file);
			i++;
		}
		
//		printf("start shot picture\n\r");
		/* ������ͼ */
		/* ��SD������BMPͼƬ */
        OSSchedLock(&err);
		ILI9341_GramScan(1);
        Screen_Shot(0, 0, 240, 320 ,buf);
		ILI9341_GramScan(2);
		OSSchedUnlock(&err);
		/* ������ɺ�ر�file */
//		printf("shot picture success\n\r");
        if (ERR_Count >= ERR_MAX)
        {
            WM_DeleteWindow(hWinCamera);
        }
		
	}  
}

void hardwareerr(void)
{
	OS_ERR  err;
	if(IsCal==0xE0)
	{
		printf("FLASH_FATFS mount fail!!!\n");
		GUI_Clear();
		GUI_DispStringHCenterAt("FLASH_FATFS mount fail", 120,120);
		LCD_BK_EN;
		while(1)
		{
			macBEEP_ON();
			OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
			macBEEP_OFF();	
			OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
		}
	}
	else if(IsCal==0xFF)
	{
		Touch_MainTask();
	}
}
/*
*********************************************************************************************************
*	�� �� ��: AppTaskUserIF
*	����˵��: LED4��˸
*	��    �Σ�p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ����2
*********************************************************************************************************
*/
static void AppTaskUserapp(void *p_arg)
{
	(void)p_arg;		/* ����������澯 */
	if(IsCal!=0x55)
	{
		hardwareerr();
	}
	while (1) 
	{   		
		UserAPP();
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskLock
*	����˵��: ��������
*	��    �Σ�p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ����3
*********************************************************************************************************
*/

static void AppTaskLock(void *p_arg)
{
  
  OS_ERR  err;
	CPU_TS 	Ts;
  
  (void)p_arg;		/* ����������澯 */

  while(1)
  {
    //��������ֱ��֪ͨ����
    OSTaskSemPend((OS_TICK )0,                                // ��������������Ϊ0��һֱ�ȴ���ȥ
                 (OS_OPT )OS_OPT_PEND_BLOCKING,  // ���û���ź������þ͵ȴ�
                 (CPU_TS *)&Ts,                   // ���������ָ�����ź������ύ��ǿ�ƽ���ȴ�״̬�������ź�����ɾ����ʱ���        
                 (OS_ERR *)&err);
    
    Unlocking();   // ����
	  
	/* ���� */
    OLED_Clear();
    ShowRightMessage();
    
    OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err);
    
    /* ���� */
    OLED_Clear();
    CloseLock();   // ����
  }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskLock
*	����˵��: ˢָ������
*	��    �Σ�p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ����3
*********************************************************************************************************
*/

static void AppTaskBrush(void *p_arg)
{
  
  OS_ERR  err;
	CPU_TS 	Ts;
  
  (void)p_arg;		/* ����������澯 */

  while(1)
  {
    // ��������ֱ��֪ͨˢָ��
    OSTaskSemPend((OS_TICK )0,                                // ��������������Ϊ0��һֱ�ȴ���ȥ
                 (OS_OPT )OS_OPT_PEND_BLOCKING,  // ���û���ź������þ͵ȴ�
                 (CPU_TS *)&Ts,                   // ���������ָ�����ź������ύ��ǿ�ƽ���ȴ�״̬�������ź�����ɾ����ʱ���        
                 (OS_ERR *)&err);
    
    press_FR();
  }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskLock
*	����˵��: ��������
*	��    �Σ�p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ����3
*********************************************************************************************************
*/

static void AppTaskBell(void *p_arg)
{
  
  OS_ERR  err;
	CPU_TS 	Ts;
  
  (void)p_arg;		/* ����������澯 */

  while(1)
  {
    //��������ֱ��֪ͨ������
    OSTaskSemPend((OS_TICK )0,                     // ��������������Ϊ0��һֱ�ȴ���ȥ
                  (OS_OPT )OS_OPT_PEND_BLOCKING,   // ���û���ź������þ͵ȴ�
                  (CPU_TS *)&Ts,                   // ���������ָ�����ź������ύ��ǿ�ƽ���ȴ�״̬�������ź�����ɾ����ʱ���        
                  (OS_ERR *)&err);
    
    macBEEP_ON();   // 
    
    OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err);
    
    macBEEP_OFF();   // 
  }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskWiFi
*	����˵��: WiFi��������
*	��    �Σ�p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ����
*********************************************************************************************************
*/

static  void   AppTaskWiFi(void *p_arg)
{
    OS_ERR  err;

    (void)p_arg;		/* ����������澯 */

    while(1)
    {
        WiFi_ConnectionDetection();    // ����״̬���
        WiFi_Receiving_Process();      // ������յ�������
        
        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    �Σ�p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void  AppTaskCreate(void)
{
	OS_ERR      err;
	
	/***********************************/
	OSTaskCreate((OS_TCB       *)&AppTaskShotTCB,            
                 (CPU_CHAR     *)"App Task SHOT",
                 (OS_TASK_PTR   )AppTaskShot, 
                 (void         *)0,
                 (OS_PRIO       )APP_TASK_SHOT_PRIO,
                 (CPU_STK      *)&AppTaskCOMStk[0],
                 (CPU_STK_SIZE  )APP_TASK_SHOT_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_TASK_SHOT_STK_SIZE,
                 (OS_MSG_QTY    )2,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);
	
	/***********************************/
	OSTaskCreate((OS_TCB       *)&AppTaskUserappTCB,             
                 (CPU_CHAR     *)"App Task Userapp",
                 (OS_TASK_PTR   )AppTaskUserapp, 
                 (void         *)0,
                 (OS_PRIO       )APP_TASK_USERAPP_PRIO,
                 (CPU_STK      *)&AppTaskUserappStk[0],
                 (CPU_STK_SIZE  )APP_TASK_USERAPP_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_TASK_USERAPP_STK_SIZE,
                 (OS_MSG_QTY    )0,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);		

	/***********************************/
	OSTaskCreate((OS_TCB       *)&AppTaskKeypadTCB,             
                 (CPU_CHAR     *)"App Task Keypad",
                 (OS_TASK_PTR   )AppTaskKeypad, 
                 (void         *)0,
                 (OS_PRIO       )APP_TASK_KEYPAD_PRIO,
                 (CPU_STK      *)&AppTaskKeypadStk[0],
                 (CPU_STK_SIZE  )APP_TASK_KEYPAD_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_TASK_KEYPAD_STK_SIZE,
                 (OS_MSG_QTY    )2,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);	
  /***********************************/
	OSTaskCreate((OS_TCB       *)&AppTaskLockTCB,             
                 (CPU_CHAR     *)"App Task Lock",
                 (OS_TASK_PTR   )AppTaskLock, 
                 (void         *)0,
                 (OS_PRIO       )APP_TASK_Lock_PRIO,
                 (CPU_STK      *)&AppTaskLockStk[0],
                 (CPU_STK_SIZE  )APP_TASK_Lock_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_TASK_Lock_STK_SIZE,
                 (OS_MSG_QTY    )2,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);	
  /***********************************/
	OSTaskCreate((OS_TCB       *)&AppTaskBellTCB,             
                 (CPU_CHAR     *)"App Task Bell",
                 (OS_TASK_PTR   )AppTaskBell, 
                 (void         *)0,
                 (OS_PRIO       )APP_TASK_Bell_PRIO,
                 (CPU_STK      *)&AppTaskBellStk[0],
                 (CPU_STK_SIZE  )APP_TASK_Bell_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_TASK_Bell_STK_SIZE,
                 (OS_MSG_QTY    )2,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);	
  /***********************************/
	OSTaskCreate((OS_TCB       *)&AppTaskBrushTCB,             
                 (CPU_CHAR     *)"App Task Brush",
                 (OS_TASK_PTR   )AppTaskBrush, 
                 (void         *)0,
                 (OS_PRIO       )APP_TASK_Brush_PRIO,
                 (CPU_STK      *)&AppTaskBrushStk[0],
                 (CPU_STK_SIZE  )APP_TASK_Brush_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_TASK_Brush_STK_SIZE,
                 (OS_MSG_QTY    )2,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);	
                  /***********************************/
	OSTaskCreate((OS_TCB       *)&AppTaskWiFiTCB,             
                 (CPU_CHAR     *)"App Task WiFi",
                 (OS_TASK_PTR   )AppTaskWiFi, 
                 (void         *)0,
                 (OS_PRIO       )APP_TASK_WiFi_PRIO,
                 (CPU_STK      *)&AppTaskWiFiStk[0],
                 (CPU_STK_SIZE  )APP_TASK_WiFi_STK_SIZE / 10,
                 (CPU_STK_SIZE  )APP_TASK_WiFi_STK_SIZE,
                 (OS_MSG_QTY    )2,
                 (OS_TICK       )0,
                 (void         *)0,
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR       *)&err);	
                 
            
}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION EVENTS
*
* Description:  This function creates the application kernel objects.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppObjCreate (void)
{
	OS_ERR      err;
	
  //����һ�������������ľ���ϸ��������Ȥ�ɲ���

	//����һ���ź������������µ�ʱ��postһ���ź�������������
//	OSSemCreate ((OS_SEM *)&SEM_SHOT,     //ָ���ź���������ָ��
//			   (CPU_CHAR    *)"SEM_SHOT",    //�ź���������
//			   (OS_SEM_CTR   )0,             //�ź���������ָʾ�¼����������Ը�ֵΪ0����ʾ�¼���û�з���
//			   (OS_ERR      *)&err);         //��������
    
//  OSSemCreate ((OS_SEM *)&SEM_Lock,     //ָ���ź���������ָ��
//			   (CPU_CHAR    *)"SEM_Lock",    //�ź���������
//			   (OS_SEM_CTR   )0,             //�ź���������ָʾ�¼����������Ը�ֵΪ0����ʾ�¼���û�з���
//			   (OS_ERR      *)&err);         //��������  
  
    		/* ������Ϣ���� queue */
  OSQCreate ((OS_Q         *)&queue,            //ָ����Ϣ���е�ָ��
               (CPU_CHAR     *)"Queue For Test",  //���е�����
               (OS_MSG_QTY    )2,                //���ɴ����Ϣ����Ŀ
               (OS_ERR       *)&err);             //���ش�������
}

/**************************************������*********************************************************/

FIL bmpfsrc; 
FRESULT bmpres;

#define GETR_FROM_RGB16(RGB565)  ((unsigned char)(( ((unsigned short int )RGB565) >>11)<<3))		  	//����8λ R
#define GETG_FROM_RGB16(RGB565)  ((unsigned char)(( ((unsigned short int )(RGB565 & 0x7ff)) >>5)<<2)) 	//����8λ G
#define GETB_FROM_RGB16(RGB565)  ((unsigned char)(( ((unsigned short int )(RGB565 & 0x1f))<<3)))       	//����8λ B

/**
 * @brief  ����ILI9341�Ľ�ȡBMPͼƬ
 * @param  x ����ȡ��������X���� 
 * @param  y ����ȡ��������Y���� 
 * @param  Width ��������
 * @param  Height ������߶� 
 * @retval ��
  *   �ò���Ϊ����ֵ֮һ��
  *     @arg 0 :��ͼ�ɹ�
  *     @arg -1 :��ͼʧ��
 */
int Screen_Shot( uint16_t x, uint16_t y, uint16_t Width, uint16_t Height, char * filename)
{
	/* bmp  �ļ�ͷ 54���ֽ� */
	unsigned char header[54] =
	{
		0x42, 0x4d, 0, 0, 0, 0, 
		0, 0, 0, 0, 54, 0, 
		0, 0, 40,0, 0, 0, 
		0, 0, 0, 0, 0, 0, 
		0, 0, 1, 0, 24, 0, 
		0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 
		0, 0, 0
	};
	
	int i;
	int j;
	long file_size;     
	long width;
	long height;
	unsigned char r,g,b;	
	unsigned int mybw;
	unsigned int read_data;
	char kk[4]={0,0,0,0};
	
	uint8_t ucAlign;//
	
	
	/* ��*�� +������ֽ� + ͷ����Ϣ */
	file_size = (long)Width * (long)Height * 3 + Height*(Width%4) + 54;		

	/* �ļ���С 4���ֽ� */
	header[2] = (unsigned char)(file_size &0x000000ff);
	header[3] = (file_size >> 8) & 0x000000ff;
	header[4] = (file_size >> 16) & 0x000000ff;
	header[5] = (file_size >> 24) & 0x000000ff;
	
	/* λͼ�� 4���ֽ� */
	width=Width;	
	header[18] = width & 0x000000ff;
	header[19] = (width >> 8) &0x000000ff;
	header[20] = (width >> 16) &0x000000ff;
	header[21] = (width >> 24) &0x000000ff;
	
	/* λͼ�� 4���ֽ� */
	height = Height;
	header[22] = height &0x000000ff;
	header[23] = (height >> 8) &0x000000ff;
	header[24] = (height >> 16) &0x000000ff;
	header[25] = (height >> 24) &0x000000ff;
		
	/* �½�һ���ļ� */
	bmpres = f_open( &bmpfsrc , (char*)filename, FA_CREATE_ALWAYS | FA_WRITE );
	
	/* �½��ļ�֮��Ҫ�ȹر��ٴ򿪲���д�� */
	f_close(&bmpfsrc);
		
	bmpres = f_open( &bmpfsrc , (char*)filename,  FA_OPEN_EXISTING | FA_WRITE);

	if ( bmpres == FR_OK )
	{    
		/* ��Ԥ�ȶ���õ�bmpͷ����Ϣд���ļ����� */
		bmpres = f_write(&bmpfsrc, header,sizeof(unsigned char)*54, &mybw);		
			
		ucAlign = Width % 4;
		
		for(i=0; i<Height; i++)					
		{
			for(j=0; j<Width; j++)  
			{					
				read_data = ILI9341_GetPointPixel ( x + j, y + Height - 1 - i );					
				
				r =  GETR_FROM_RGB16(read_data);
				g =  GETG_FROM_RGB16(read_data);
				b =  GETB_FROM_RGB16(read_data);

				bmpres = f_write(&bmpfsrc, &b,sizeof(unsigned char), &mybw);
				bmpres = f_write(&bmpfsrc, &g,sizeof(unsigned char), &mybw);
				bmpres = f_write(&bmpfsrc, &r,sizeof(unsigned char), &mybw);
			}
				
			if( ucAlign )				/* �������4�ֽڶ��� */
				bmpres = f_write ( & bmpfsrc, kk, sizeof(unsigned char) * ( ucAlign ), & mybw );

		}/* ������� */

		f_close(&bmpfsrc); 
		
		return 0;
		
	}	
	else/* ����ʧ�� */
		return -1;

}

extern uint8_t Ov7725_vsync;
extern void cbCameraWin(WM_MESSAGE * pMsg);

void Camera_Display(void)
{
	OS_ERR     err;
	uint32_t i=0;
	uint16_t Camera_Data;
	uint8_t count=0;
	key_flag=0;
    
	hWinCamera=WM_CreateWindowAsChild(0,0,240,320,WM_HBKWIN,WM_CF_SHOW,cbCameraWin,0);
    
	/* ov7725 gpio ��ʼ�� */
	Ov7725_GPIO_Config();
	/* ov7725 �Ĵ������ó�ʼ�� */
	while(count<10)
	{		
		if(Ov7725_Init())break;
		OSTimeDlyHMSM(0, 0, 0, 500, OS_OPT_TIME_HMSM_STRICT, &err);
		count++;
	}
	while(count==10)
	{		
        
	}
	
	/* ov7725 ���ź��߳�ʼ�� */
	VSYNC_Init();
	Ov7725_vsync = 0;
	while(1)
	{
		if( Ov7725_vsync == 2 )
		{			
			OSSchedLock(&err);
			ILI9341_GramScan(2);
			bsp_DelayUS(10);
			FIFO_PREPARE;  			/*FIFO׼��*/
			/*�ɼ�����ʾ*/
			for(i = 0; i < 240*320; i++)
			{
				READ_FIFO_PIXEL(Camera_Data);		/* ��FIFO����һ��rgb565���ص�Camera_Data���� */
				macFSMC_ILI9341_RAM=Camera_Data;
			}
			OSSchedUnlock(&err);
			Ov7725_vsync = 0;	
            discameraexit();
            return;
		}
        OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_HMSM_STRICT, &err);
	}
}

/*********************************************END OF FILE**********************/
