/**************************************************************************************
* 因为emWin显示只支持UTF-8编码格式的中文，如果希望直接显示在Keil直接输入的中文，      *
*            比如使用：GUI_DispStringHCenterAt("流水灯",110,120);                     *
* 该文件必须以UTF-8编码格式，不然中文无法正常显示。                                   *
*                                                                                     *
* 如果只是个别例程出现中文显示乱码（如果所有例程都无法显示中文可能是字库问题），      *
* 把对应的例程文件(比如LEDapp.c)用电脑的记事本软件打开，然后选择另存为，在弹出对      *
* 话框中“保存(S)"按钮的左边有个"编码(E)"选项，选择"UTF-8",然后同样保存为同名称的      *
* C语言文件(覆盖原来文件)，再编译。                                                   *
*                                                                                     *
* 如果编译工程时出现下面类似错误也是该文件编码格式问题,必须把文件保存为UTF-8格式      *
* 再编译                                                                              *
* ..\..\User\app\LEDapp.c(275): error:  #8: missing closing quote                     *
*        GUI_DispStringHCenterAt("娴?姘?鐏?",110,120);                                *
* ..\..\User\app\LEDapp.c(276): error:  #165: too few arguments in function call      *
*        GUI_DispStringHCenterAt("瑙?鎽?鍋?宸?澶?鎵?闇€瑕?瑙?鎽?鏍?鍑?",110,215);     *
* ..\..\User\app\LEDapp.c(276): error:  #18: expected a ")"                           *
*        GUI_DispStringHCenterAt("瑙?鎽?鍋?宸?澶?鎵?闇€瑕?瑙?鎽?鏍?鍑?",110,215);     *
*                                                                                     *
* 修改文件后编译就出错这是Keil5软件问题(Keil4没这问题)，推荐使用其他程序编辑工具，    *
* 只用Keil5完成编译和下载工作。                                                       *
***************************************************************************************
*                      实验平台: 野火STM32 ISO 开发板                                 *
*                      论    坛: http://www.chuxue123.com                             *
*                      淘    宝: http://firestm32.taobao.com                          *
*                      邮    箱: wildfireteam@163.com                                 *
***************************************************************************************
*/
/**************************************************************************************
*                                                                                     *
*                SEGGER Microcontroller GmbH & Co. KG                                 *
*        Solutions for real time microcontroller applications                         *
*                                                                                     *
***************************************************************************************
*                                                                                     *
* C-file generated by:                                                                *
*                                                                                     *
*        GUI_Builder for emWin version 5.22                                           *
*        Compiled Jul  4 2013, 15:16:01                                               *
*        (c) 2013 Segger Microcontroller GmbH & Co. KG                                *
*                                                                                     *
***************************************************************************************
*                                                                                     *
*        Internet: www.segger.com  Support: support@segger.com                        *
*                                                                                     *
***************************************************************************************
*/
// USER START (Optionally insert additional includes)
#include "includes.h"
#include  "app.h"
// USER END
/**************************************************************************************
*
*       Defines
*
***************************************************************************************
*/

// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
extern int8_t rtc_flag;
extern char TimeDisplay;
static	char text_buffer[30];
static 	struct rtc_time SrcTm;
/*时间结构体*/
extern struct rtc_time systmtime;

// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateClock[] = {
  { FRAMEWIN_CreateIndirect, "Clock", 0, 0, 0, 240, 320, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Time", GUI_ID_TEXT0, 20, 210, 100, 40, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "wday", GUI_ID_TEXT1, 120, 205, 100, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Data", GUI_ID_TEXT2, 120, 225, 100, 20, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Time Setting", GUI_ID_TEXT3, 0, 0, 110, 20, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "Ok", GUI_ID_BUTTON0, 40, 140, 150, 30, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "TIME", GUI_ID_TEXT4, 0, 20, 210, 20, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "Year", GUI_ID_EDIT0, 20, 40, 50, 25, 0, 0x4, 0 },
  { EDIT_CreateIndirect, "Month", GUI_ID_EDIT1, 87, 40, 50, 25, 0, 0x2, 0 },
  { EDIT_CreateIndirect, "Day", GUI_ID_EDIT2, 155, 40, 50, 25, 0, 0x2, 0 },
  { TEXT_CreateIndirect, "Hour", GUI_ID_TEXT5, 0, 70, 210, 20, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "HOUR", GUI_ID_EDIT3, 20, 90, 50, 25, 0, 0x2, 0 },
  { EDIT_CreateIndirect, "Minute", GUI_ID_EDIT4, 87, 90, 50, 25, 0, 0x2, 0 },
  { EDIT_CreateIndirect, "Edit", GUI_ID_EDIT5, 155, 90, 50, 25, 0, 0x2, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialogClock(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
  OS_ERR      err;
	uint16_t messagesclock[2];
  // USER END
  switch (pMsg->MsgId) {
	case WM_TIMER:
		//printf("Clockapp:MY_MESSAGE_RTC\n");
		if(!TimeDisplay)
		{
			WM_RestartTimer(pMsg->Data.v, 250);
			break;
		}
		/* 转换rtc值至北京时间*/
		RTC_TimeCovr(&systmtime);
		/* 转换成字符串 */
		sprintf(text_buffer,"%02d:%02d:%02d",systmtime.tm_hour,systmtime.tm_min,systmtime.tm_sec);
		//printf("time:%s\n",text_buffer);
		/* 获取text句柄 */
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);//00:00:00
		/* 输出时间 */
		TEXT_SetText(hItem,text_buffer);	
		/* 转换成字符串 */
		sprintf(text_buffer,"%04d.%02d.%02d",systmtime.tm_year,systmtime.tm_mon,systmtime.tm_mday);		
		/* 获取text句柄 */
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2);//2015.04.28		
		/* 输出时间 */
		TEXT_SetText(hItem,text_buffer);		
		/* 获取text句柄 */
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1);//Wednesday		
		/* 输出星期 */
		switch(systmtime.tm_wday)
		{
			case 1:
				/* 输出时间 */
				TEXT_SetText(hItem,"Monday");				
			break;		
			case 2:
				/* 输出时间 */
				TEXT_SetText(hItem,"Tuesday");				
			break;								
			case 3:
				/* 输出时间 */
				TEXT_SetText(hItem,"Wednesday");				
			break;											
			case 4:
				/* 输出时间 */
				TEXT_SetText(hItem,"Thursday");				
			break;			
			case 5:
				/* 输出时间 */
				TEXT_SetText(hItem,"Friday");				
			break;														
			case 6:
				/* 输出时间 */
				TEXT_SetText(hItem,"Saturday");				
			break;			
			case 7:
				/* 输出时间 */
				TEXT_SetText(hItem,"Sunday");				
			break;		
			default:
			  break;
		}
	WM_RestartTimer(pMsg->Data.v, 250);
	break;
  case WM_DELETE:
		OS_INFO("Clockapp delete\n");
		Flag_ICON104 = 0;
		UserApp_Flag = 0;
		keypadflag=0;
		WM_DeleteWindow(KEYBOARD);
		tpad_flag=0;
	break;
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Clock'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetTextColor(hItem,GUI_DARKGRAY);
    FRAMEWIN_SetFont(hItem, GUI_FONT_16B_ASCII);
    FRAMEWIN_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
    FRAMEWIN_SetTitleHeight(hItem, 20);
    //
    // Initialization of 'Time'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
    TEXT_SetFont(hItem, GUI_FONT_24B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetText(hItem, "");
    //
    // Initialization of 'wday'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1);
    TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetText(hItem, "");
    //
    // Initialization of 'Data'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2);
    TEXT_SetFont(hItem, GUI_FONT_20B_ASCII);
    TEXT_SetText(hItem, "");
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    //
    // Initialization of 'Time Setting'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT3);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, GUI_FONT_20_ASCII);
    TEXT_SetText(hItem, "Time Setting:");
    //
    // Initialization of 'Ok'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0);
    BUTTON_SetFont(hItem, GUI_FONT_16B_ASCII);
    BUTTON_SetText(hItem, "Setting RTC");
		BUTTON_SetFocussable(hItem, 0);
    //
    // Initialization of 'TIME'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT4);
    TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    TEXT_SetText(hItem, "        Year        Month        Day");
    //
    // Initialization of 'Year'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0);
    EDIT_SetText(hItem, "2015");
    EDIT_SetFont(hItem, GUI_FONT_20_ASCII);
    EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetUlongMode(hItem,2015,1970,2038);
    //
    // Initialization of 'Month'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1);
    EDIT_SetText(hItem, "04");
    EDIT_SetFont(hItem, GUI_FONT_20B_ASCII);
    EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetUlongMode(hItem,04,1,12);
    //
    // Initialization of 'Day'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT2);
    EDIT_SetText(hItem, "28");
    EDIT_SetFont(hItem, GUI_FONT_20B_ASCII);
    EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetUlongMode(hItem,28,1,31);
    //
    // Initialization of 'Hour'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT5);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    TEXT_SetText(hItem, "        Hour       Minute      Second");
    TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);
    //
    // Initialization of 'HOUR'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT3);
    EDIT_SetText(hItem, "11");
    EDIT_SetFont(hItem, GUI_FONT_20B_ASCII);
    EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetUlongMode(hItem,11,0,23);
    //
    // Initialization of 'Minute'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT4);
    EDIT_SetText(hItem, "55");
    EDIT_SetFont(hItem, GUI_FONT_20B_ASCII);
    EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetUlongMode(hItem,55,0,59);
    //
    // Initialization of 'Edit'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT5);
    EDIT_SetText(hItem, "55");
    EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    EDIT_SetFont(hItem, GUI_FONT_20B_ASCII);
		EDIT_SetUlongMode(hItem,55,0,59);
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case GUI_ID_BUTTON0: // Notifications sent by 'Ok'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				SrcTm.tm_year  = EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0));
				SrcTm.tm_mon  = EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1));
				SrcTm.tm_mday = EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT2));
				SrcTm.tm_hour = EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT3));
				SrcTm.tm_min = EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT4));
				SrcTm.tm_sec = EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT5));
				
				Time_Adjust_LCD(&systmtime,&SrcTm);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_EDIT0: // Notifications sent by 'Year'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				messagesclock[0]=WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0);
				messagesclock[1]=GUI_ID_EDIT0;
				if(!keypadflag)OSTaskQPost(&AppTaskKeypadTCB,
																(void *)&messagesclock,
																2,
																OS_OPT_POST_FIFO,
																&err);
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_EDIT1: // Notifications sent by 'Month'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
				// USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				messagesclock[0]=WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1);
				messagesclock[1]=GUI_ID_EDIT1;
				if(!keypadflag)OSTaskQPost(&AppTaskKeypadTCB,
																(void *)&messagesclock,
																2,
																OS_OPT_POST_FIFO,
																&err);
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_EDIT2: // Notifications sent by 'Day'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)				
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				messagesclock[0]=WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT2);
				messagesclock[1]=GUI_ID_EDIT2;
				if(!keypadflag)OSTaskQPost(&AppTaskKeypadTCB,
																(void *)&messagesclock,
																2,
																OS_OPT_POST_FIFO,
																&err);
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_EDIT3: // Notifications sent by 'HOUR'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)				
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				messagesclock[0]=WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT3);
				messagesclock[1]=GUI_ID_EDIT3;
				if(!keypadflag)OSTaskQPost(&AppTaskKeypadTCB,
																(void *)&messagesclock,
																2,
																OS_OPT_POST_FIFO,
																&err);
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_EDIT4: // Notifications sent by 'Minute'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)				
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				messagesclock[0]=WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT4);
				messagesclock[1]=GUI_ID_EDIT4;
				if(!keypadflag)OSTaskQPost(&AppTaskKeypadTCB,
																(void *)&messagesclock,
																2,
																OS_OPT_POST_FIFO,
																&err);
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_EDIT5: // Notifications sent by 'Edit'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)				
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				messagesclock[0]=WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT5);
				messagesclock[1]=GUI_ID_EDIT5;
				if(!keypadflag)OSTaskQPost(&AppTaskKeypadTCB,
																(void *)&messagesclock,
																2,
																OS_OPT_POST_FIFO,
																&err);
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;
  // USER START (Optionally insert additional message handling)
	case WM_PAINT:                                            //重绘背景	
    GUI_SetBkColor(GUI_LIGHTYELLOW);
		GUI_SetColor(0XD0FFFF);
		GUI_Clear();
		GUI_FillRect(0,0,240-1,190);
    break;
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateClock
*/
void FUN_ICON104Clicked(void)
{
	WM_HWIN hWin;
	OS_INFO("Clockapp create\n");
	hWin = GUI_CreateDialogBox(_aDialogCreateClock, GUI_COUNTOF(_aDialogCreateClock), _cbDialogClock, WM_HBKWIN, 0, 0);
	/* 
	 * 创建定时器，其功能是经过指定周期后，向指定窗口发送消息。
	 * 该定时器与指定窗口相关联。 
	 */
	WM_CreateTimer(WM_GetClientWindow(hWin),  /* 接受信息的窗口的句柄 */
	               0, 	         /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。 */
								 250,         /* 周期，此周期过后指定窗口应收到消息*/
								 0);	         /* 留待将来使用，应为0 */
	GUI_Delay(10); 
	if(rtc_flag!=0)
		rtc_flag=RTC_CheckAndConfig(&systmtime);
	if(rtc_flag!=0)
		GUI_MessageBox("\r\n RTC Work Down!!! \r\n","error",GUI_MESSAGEBOX_CF_MOVEABLE);
	else
		WM_EnableWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
	
	while(Flag_ICON104)
	{
		if(tpad_flag)WM_DeleteWindow(hWin);
		GUI_Delay(10); 
	}
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
