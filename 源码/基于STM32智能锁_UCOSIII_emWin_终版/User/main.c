/*
******************************************************************************
* @file    bsp_led.c
* @author  野火
* @version V1.0
* @date    2015-xx-xx
* @brief   led应用函数接口
******************************************************************************
* @attention
*
* 实验平台:野火 iSO STM32 开发板 
* 论坛    :http://www.firebbs.cn
* 淘宝    :https://fire-stm32.taobao.com
*
******************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/
static  OS_TCB	 AppTaskStartTCB;		     //定义任务控制块
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
static	CPU_STK	 AppTaskStartStk[APP_TASK_START_STK_SIZE];	   //定义任务堆栈
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

OS_Q queue;                             // 声明消息队列

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

uint8_t key_flag=1;//1：允许按键截图，0：禁用按键截图
uint8_t IsCal =0;

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	OS_ERR err;	
	
	/* 初始化"uC/OS-III"内核 */  
	OSInit(&err);	
	
	/*创建任务*/
	OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,              // 任务控制块指针          
               (CPU_CHAR   *)"App Task Start",		          // 任务名称
               (OS_TASK_PTR )AppTaskStart, 	                  // 任务代码指针
               (void       *)0,			                      // 传递给任务的参数parg
               (OS_PRIO     )APP_TASK_START_PRIO,			  // 任务优先级
               (CPU_STK    *)&AppTaskStartStk[0],	          // 任务堆栈基地址
               (CPU_STK_SIZE)APP_TASK_START_STK_SIZE/10,	  // 堆栈剩余警戒线
               (CPU_STK_SIZE)APP_TASK_START_STK_SIZE,		  // 堆栈大小
               (OS_MSG_QTY  )1u,			                      // 可接收的最大消息队列数
               (OS_TICK     )0u,			                      // 时间片轮转时间
               (void       *)0,			                      // 任务控制块扩展信息
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | 
                              OS_OPT_TASK_STK_CLR),	      // 任务选项
               (OS_ERR     *)&err);		                    // 返回值
	  
  /* 启动多任务系统，控制权交给uC/OS-III */
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
	
	/* 板级初始化 */	
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
        
        /* 密码扫描 */
        PasswordUnlock();
        
        OSTimeDlyHMSM(0, 0, 0, 15,
                      OS_OPT_TIME_HMSM_STRICT,
                      &err);
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskKeypad
*	功能说明: 中文输入法键盘任务
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级：6
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
//        printf("键盘\n");
		if(err == OS_ERR_NONE)
		{
			keypad(msg);
		}
	}
}

///*
//*********************************************************************************************************
//*	函 数 名: _WriteByte2File()
//*	功能说明: 
//*	形    参：  	
//*	返 回 值: 
//*********************************************************************************************************
//*/
//static void _WriteByte2File(uint8_t Data, void * p) 
//{
//	result = f_write(p, &Data, 1, &bw);
//}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCom
*	功能说明: 
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级：3
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
		//阻塞任务直到拍照通知
        OSTaskSemPend((OS_TICK )0,                    // 如果这个参数设置为0就一直等待下去
                      (OS_OPT )OS_OPT_PEND_BLOCKING,  // 如果没有信号量可用就等待
                      (CPU_TS *)&Ts,                   // 这个参数是指向存放信号量被提交、强制解除等待状态、或者信号量被删除的时间戳        
                      (OS_ERR *)&err);
        
        if (ERR_Count >= ERR_MAX)
        {
            Camera_Display();
        }
        
        /* 获取当前时间 */
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
		/* 创建截图 */
		/* 向SD卡绘制BMP图片 */
        OSSchedLock(&err);
		ILI9341_GramScan(1);
        Screen_Shot(0, 0, 240, 320 ,buf);
		ILI9341_GramScan(2);
		OSSchedUnlock(&err);
		/* 创建完成后关闭file */
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
*	函 数 名: AppTaskUserIF
*	功能说明: LED4闪烁
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级：2
*********************************************************************************************************
*/
static void AppTaskUserapp(void *p_arg)
{
	(void)p_arg;		/* 避免编译器告警 */
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
*	函 数 名: AppTaskLock
*	功能说明: 开锁任务
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级：3
*********************************************************************************************************
*/

static void AppTaskLock(void *p_arg)
{
  
  OS_ERR  err;
	CPU_TS 	Ts;
  
  (void)p_arg;		/* 避免编译器告警 */

  while(1)
  {
    //阻塞任务直到通知开锁
    OSTaskSemPend((OS_TICK )0,                                // 如果这个参数设置为0就一直等待下去
                 (OS_OPT )OS_OPT_PEND_BLOCKING,  // 如果没有信号量可用就等待
                 (CPU_TS *)&Ts,                   // 这个参数是指向存放信号量被提交、强制解除等待状态、或者信号量被删除的时间戳        
                 (OS_ERR *)&err);
    
    Unlocking();   // 开锁
	  
	/* 清屏 */
    OLED_Clear();
    ShowRightMessage();
    
    OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err);
    
    /* 清屏 */
    OLED_Clear();
    CloseLock();   // 关锁
  }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskLock
*	功能说明: 刷指纹任务
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级：3
*********************************************************************************************************
*/

static void AppTaskBrush(void *p_arg)
{
  
  OS_ERR  err;
	CPU_TS 	Ts;
  
  (void)p_arg;		/* 避免编译器告警 */

  while(1)
  {
    // 阻塞任务直到通知刷指纹
    OSTaskSemPend((OS_TICK )0,                                // 如果这个参数设置为0就一直等待下去
                 (OS_OPT )OS_OPT_PEND_BLOCKING,  // 如果没有信号量可用就等待
                 (CPU_TS *)&Ts,                   // 这个参数是指向存放信号量被提交、强制解除等待状态、或者信号量被删除的时间戳        
                 (OS_ERR *)&err);
    
    press_FR();
  }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskLock
*	功能说明: 门铃任务
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级：3
*********************************************************************************************************
*/

static void AppTaskBell(void *p_arg)
{
  
  OS_ERR  err;
	CPU_TS 	Ts;
  
  (void)p_arg;		/* 避免编译器告警 */

  while(1)
  {
    //阻塞任务直到通知响门铃
    OSTaskSemPend((OS_TICK )0,                     // 如果这个参数设置为0就一直等待下去
                  (OS_OPT )OS_OPT_PEND_BLOCKING,   // 如果没有信号量可用就等待
                  (CPU_TS *)&Ts,                   // 这个参数是指向存放信号量被提交、强制解除等待状态、或者信号量被删除的时间戳        
                  (OS_ERR *)&err);
    
    macBEEP_ON();   // 
    
    OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT,&err);
    
    macBEEP_OFF();   // 
  }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskWiFi
*	功能说明: WiFi处理任务
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级：
*********************************************************************************************************
*/

static  void   AppTaskWiFi(void *p_arg)
{
    OS_ERR  err;

    (void)p_arg;		/* 避免编译器告警 */

    while(1)
    {
        WiFi_ConnectionDetection();    // 连接状态检测
        WiFi_Receiving_Process();      // 处理接收到的数据
        
        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参：p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
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
	
  //创建一个按键，按键的具体细节如无兴趣可不管

	//创建一个信号量，按键按下的时候post一个信号量给其他任务
//	OSSemCreate ((OS_SEM *)&SEM_SHOT,     //指向信号量变量的指针
//			   (CPU_CHAR    *)"SEM_SHOT",    //信号量的名字
//			   (OS_SEM_CTR   )0,             //信号量这里是指示事件发生，所以赋值为0，表示事件还没有发生
//			   (OS_ERR      *)&err);         //错误类型
    
//  OSSemCreate ((OS_SEM *)&SEM_Lock,     //指向信号量变量的指针
//			   (CPU_CHAR    *)"SEM_Lock",    //信号量的名字
//			   (OS_SEM_CTR   )0,             //信号量这里是指示事件发生，所以赋值为0，表示事件还没有发生
//			   (OS_ERR      *)&err);         //错误类型  
  
    		/* 创建消息队列 queue */
  OSQCreate ((OS_Q         *)&queue,            //指向消息队列的指针
               (CPU_CHAR     *)"Queue For Test",  //队列的名字
               (OS_MSG_QTY    )2,                //最多可存放消息的数目
               (OS_ERR       *)&err);             //返回错误类型
}

/**************************************拍照用*********************************************************/

FIL bmpfsrc; 
FRESULT bmpres;

#define GETR_FROM_RGB16(RGB565)  ((unsigned char)(( ((unsigned short int )RGB565) >>11)<<3))		  	//返回8位 R
#define GETG_FROM_RGB16(RGB565)  ((unsigned char)(( ((unsigned short int )(RGB565 & 0x7ff)) >>5)<<2)) 	//返回8位 G
#define GETB_FROM_RGB16(RGB565)  ((unsigned char)(( ((unsigned short int )(RGB565 & 0x1f))<<3)))       	//返回8位 B

/**
 * @brief  设置ILI9341的截取BMP图片
 * @param  x ：截取区域的起点X坐标 
 * @param  y ：截取区域的起点Y坐标 
 * @param  Width ：区域宽度
 * @param  Height ：区域高度 
 * @retval 无
  *   该参数为以下值之一：
  *     @arg 0 :截图成功
  *     @arg -1 :截图失败
 */
int Screen_Shot( uint16_t x, uint16_t y, uint16_t Width, uint16_t Height, char * filename)
{
	/* bmp  文件头 54个字节 */
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
	
	
	/* 宽*高 +补充的字节 + 头部信息 */
	file_size = (long)Width * (long)Height * 3 + Height*(Width%4) + 54;		

	/* 文件大小 4个字节 */
	header[2] = (unsigned char)(file_size &0x000000ff);
	header[3] = (file_size >> 8) & 0x000000ff;
	header[4] = (file_size >> 16) & 0x000000ff;
	header[5] = (file_size >> 24) & 0x000000ff;
	
	/* 位图宽 4个字节 */
	width=Width;	
	header[18] = width & 0x000000ff;
	header[19] = (width >> 8) &0x000000ff;
	header[20] = (width >> 16) &0x000000ff;
	header[21] = (width >> 24) &0x000000ff;
	
	/* 位图高 4个字节 */
	height = Height;
	header[22] = height &0x000000ff;
	header[23] = (height >> 8) &0x000000ff;
	header[24] = (height >> 16) &0x000000ff;
	header[25] = (height >> 24) &0x000000ff;
		
	/* 新建一个文件 */
	bmpres = f_open( &bmpfsrc , (char*)filename, FA_CREATE_ALWAYS | FA_WRITE );
	
	/* 新建文件之后要先关闭再打开才能写入 */
	f_close(&bmpfsrc);
		
	bmpres = f_open( &bmpfsrc , (char*)filename,  FA_OPEN_EXISTING | FA_WRITE);

	if ( bmpres == FR_OK )
	{    
		/* 将预先定义好的bmp头部信息写进文件里面 */
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
				
			if( ucAlign )				/* 如果不是4字节对齐 */
				bmpres = f_write ( & bmpfsrc, kk, sizeof(unsigned char) * ( ucAlign ), & mybw );

		}/* 截屏完毕 */

		f_close(&bmpfsrc); 
		
		return 0;
		
	}	
	else/* 截屏失败 */
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
    
	/* ov7725 gpio 初始化 */
	Ov7725_GPIO_Config();
	/* ov7725 寄存器配置初始化 */
	while(count<10)
	{		
		if(Ov7725_Init())break;
		OSTimeDlyHMSM(0, 0, 0, 500, OS_OPT_TIME_HMSM_STRICT, &err);
		count++;
	}
	while(count==10)
	{		
        
	}
	
	/* ov7725 场信号线初始化 */
	VSYNC_Init();
	Ov7725_vsync = 0;
	while(1)
	{
		if( Ov7725_vsync == 2 )
		{			
			OSSchedLock(&err);
			ILI9341_GramScan(2);
			bsp_DelayUS(10);
			FIFO_PREPARE;  			/*FIFO准备*/
			/*采集并显示*/
			for(i = 0; i < 240*320; i++)
			{
				READ_FIFO_PIXEL(Camera_Data);		/* 从FIFO读出一个rgb565像素到Camera_Data变量 */
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
