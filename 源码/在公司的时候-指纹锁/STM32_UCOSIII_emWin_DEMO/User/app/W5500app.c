﻿/**************************************************************************************
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
static uint8_t netmode=0;// 0:TCP Clent 1:TCP Server 2:UDP
static uint8_t IsConnect=0;
static uint8_t IsNetInit=0;
static WM_HWIN hPend;
static 	uint16_t messagesnetwork[2];
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateNetwork[] = {
  { FRAMEWIN_CreateIndirect, "W5500", 0, 0, 0, 240, 320, 0, 0x0, 0 },
  { DROPDOWN_CreateIndirect, "Mode", GUI_ID_DROPDOWN0, 30, 35, 110, 60, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Port", GUI_ID_EDIT0, 180, 4, 45, 22, 0, 0x5, 0 },
  { EDIT_CreateIndirect, "IP", GUI_ID_EDIT1, 30, 4, 105, 22, 0, 0xf, 0 },
  { BUTTON_CreateIndirect, "Connect", GUI_ID_BUTTON0, 160, 35, 60, 35, 0, 0x0, 0 },
  { MULTIEDIT_CreateIndirect, "Post", GUI_ID_MULTIEDIT0, 5, 85, 160, 80, 0, 0x0, 0 },
  { MULTIEDIT_CreateIndirect, "Pend", GUI_ID_MULTIEDIT1, 5, 190, 220, 100, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Send", GUI_ID_BUTTON1, 170, 95, 55, 25, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Clear", GUI_ID_BUTTON2, 170, 130, 55, 25, 0, 0x0, 0 },
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
//eg:text: 192.168.1.10 --> ip[0]=192,ip[1]=168,ip[2]=1,ip[3]=10.
static uint8_t TextToIP(uint8_t *text,uint8_t *ip)
{
	uint8_t i,j,tmp[3]={0};
	int8_t dot[5]={0};
	i=0;
	j=0;
	while((i<15)&&(text[i]!='\0'))
	{
		if(!((text[i]>='0'&&text[i]<='9')||(text[i]=='.')))
			return 1;
		if(text[i]=='.')
		{				
			++j;	
			dot[j]=i;				
			if(j>3||i==0)return 1;
		}			
		++i;		
	}
	if(j<3) return 1;
	dot[4]=i;
	if(((dot[2]-dot[1])<=1)||((dot[3]-dot[2])<=1)||((dot[4]-dot[3])<=1)) return 1;	
	dot[0]=-1;
	for(i=0;i<4;++i)
	{
		j=dot[i+1]-dot[i]-1;
		if(j==1){tmp[0]='0';tmp[1]='0';tmp[2]=text[dot[i]+1];}
		else if(j==2){tmp[0]='0';tmp[1]=text[dot[i]+1];tmp[2]=text[dot[i]+2];}
		else if(j==3){tmp[0]=text[dot[i]+1];tmp[1]=text[dot[i]+2];tmp[2]=text[dot[i]+3];}
		ip[i]=100*(tmp[0]-'0')+10*(tmp[1]-'0')+tmp[2]-'0';
	}
	return 0;
}

/**
*@brief		TCP Server回环演示函数。
*@param		无
*@return	无
*/
void do_tcp_server(void)
{	
	uint16 len=0;  
	switch(getSn_SR(SOCK_TCPS))											            	/*获取socket的状态*/
	{
		case SOCK_CLOSED:													                  /*socket处于关闭状态*/
			socket(SOCK_TCPS ,Sn_MR_TCP,local_port,Sn_MR_ND);	        /*打开socket*/
		  break;     
    
		case SOCK_INIT:														                  /*socket已初始化状态*/
			listen(SOCK_TCPS);												                /*socket建立监听*/
		  break;
		
		case SOCK_ESTABLISHED:												              /*socket处于连接建立状态*/
		
			if(getSn_IR(SOCK_TCPS) & Sn_IR_CON)
			{
				setSn_IR(SOCK_TCPS, Sn_IR_CON);								          /*清除接收中断标志位*/
			}
			len=getSn_RX_RSR(SOCK_TCPS);									            /*定义len为已接收数据的长度*/
			if(len>0)
			{
				recv(SOCK_TCPS,comdata,len);								              	/*接收来自Client的数据*/
				comdata[len]=0x00; 											                  /*添加字符串结束符*/
				//printf("%s\r\n",buff);
				send(SOCK_TCPS,comdata,len);									              /*向Client发送数据*/
				com_gbk2utf8((const char *)comdata,(char *)comdata);
				if(MULTIEDIT_GetTextSize(hPend)>=1000)
						MULTIEDIT_SetText(hPend,"");
				MULTIEDIT_AddText(hPend,(char *)comdata);
		  }
		  break;
		
		case SOCK_CLOSE_WAIT:												                /*socket处于等待关闭状态*/
			close(SOCK_TCPS);
		  break;
	}
}

/**
*@brief		UDP测试程序
*@param		无
*@return	无
*/
void do_udp(void)
{                                                              
	uint16 len=0;
	switch(getSn_SR(SOCK_UDPS))                                                /*获取socket的状态*/
	{
		case SOCK_CLOSED:                                                        /*socket处于关闭状态*/
			socket(SOCK_UDPS,Sn_MR_UDP,local_port,0);                              /*初始化socket*/
		  break;
		
		case SOCK_UDP:                                                           /*socket初始化完成*/
			delay_ms(10);
			if(getSn_IR(SOCK_UDPS) & Sn_IR_RECV)
			{
				setSn_IR(SOCK_UDPS, Sn_IR_RECV);                                     /*清接收中断*/
			}
			if((len=getSn_RX_RSR(SOCK_UDPS))>0)                                    /*接收到数据*/
			{
				recvfrom(SOCK_UDPS,comdata, len, remote_ip,&remote_port);               /*W5500接收计算机发送来的数据*/
				comdata[len-8]=0x00;                                                    /*添加字符串结束符*/
				//printf("%s\r\n",buff);                                               /*打印接收缓存*/ 
				sendto(SOCK_UDPS,comdata,len-8, remote_ip, remote_port);                /*W5500把接收到的数据发送给Remote*/
				comdata[len-8]='\0';
				com_gbk2utf8((const char *)comdata,(char *)comdata);
				if(MULTIEDIT_GetTextSize(hPend)>=1000)
						MULTIEDIT_SetText(hPend,"");
				MULTIEDIT_AddText(hPend,(char *)comdata);
			}
			break;
	}

}
/**
*@brief		TCP Client回环演示函数。
*@param		无
*@return	无
*/
void do_tcp_client(void)
{	
   uint16 len=0;	

	switch(getSn_SR(SOCK_TCPC))								  				         /*获取socket的状态*/
	{
		case SOCK_CLOSED:											        		         /*socket处于关闭状态*/
			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND);
		  break;
		
		case SOCK_INIT:													        	         /*socket处于初始化状态*/
			connect(SOCK_TCPC,remote_ip,remote_port);                /*socket连接服务器*/ 
		  break;
		
		case SOCK_ESTABLISHED: 												             /*socket处于连接建立状态*/
			if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
			{
				setSn_IR(SOCK_TCPC, Sn_IR_CON); 							         /*清除接收中断标志位*/
			}
		
			len=getSn_RX_RSR(SOCK_TCPC); 								  	         /*定义len为已接收数据的长度*/
			if(len>0)
			{
				recv(SOCK_TCPC,comdata,len); 							   		         /*接收来自Server的数据*/
				comdata[len]=0x00;  											                 /*添加字符串结束符*/
				//printf("%s\r\n",buff);
				send(SOCK_TCPC,comdata,len);								     	         /*向Server发送数据*/
				com_gbk2utf8((const char *)comdata,(char *)comdata);
				if(MULTIEDIT_GetTextSize(hPend)>=1000)
						MULTIEDIT_SetText(hPend,"");
				MULTIEDIT_AddText(hPend,(char *)comdata);
			}		  
		  break;
			
		case SOCK_CLOSE_WAIT: 											    	         /*socket处于等待关闭状态*/
			close(SOCK_TCPC);
		  break;

	}
}

// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialogNetwork(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
	uint8_t iptext[16]={0};
	OS_ERR      err;
	uint16_t i=0;
  // USER END

  switch (pMsg->MsgId) {
	case WM_DELETE:
		OS_INFO("Networkapp delete\n");
		IsConnect=0;//关闭连接
		Flag_ICON109 = 0;
		UserApp_Flag = 0;
		keypadflag=0;
		WM_DeleteWindow(KEYBOARD);
		tpad_flag=0;
	break;
  case WM_INIT_DIALOG:
    //
    // Initialization of 'W5500'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetTextColor(hItem,GUI_DARKGRAY);
    FRAMEWIN_SetFont(hItem, GUI_FONT_16B_ASCII);
    FRAMEWIN_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		hFrameClose=FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
    FRAMEWIN_SetTitleHeight(hItem, 20);
		WM_DisableWindow(hFrameClose);
    //
    // Initialization of 'Mode'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_DROPDOWN0);
    DROPDOWN_SetFont(hItem, GUI_FONT_16B_ASCII);
		DROPDOWN_SetTextAlign(hItem,GUI_TA_CENTER);
    DROPDOWN_AddString(hItem, "TCP Client");
    DROPDOWN_AddString(hItem, "TCP Server");    
    DROPDOWN_AddString(hItem, "UDP");
		WM_BringToTop(hItem);
    //
    // Initialization of 'Port'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0);
    EDIT_SetFont(hItem, GUI_FONT_16B_ASCII);
    EDIT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		EDIT_SetUlongMode(hItem,remote_port,1,65535);
    //
    // Initialization of 'IP'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1);
		sprintf((char *)iptext,"%d.%d.%d.%d",remote_ip[0],remote_ip[1],remote_ip[2],remote_ip[3]);
    EDIT_SetText(hItem,(char *)iptext);
    EDIT_SetFont(hItem, GUI_FONT_16B_ASCII);
    EDIT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		WM_DisableWindow(hItem);
    //
    // Initialization of 'Connect'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0);
    BUTTON_SetFont(hItem, &XBF_Font);
    BUTTON_SetText(hItem, "连接");
		WM_DisableWindow(hItem);
    //
    // Initialization of 'Post'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0);
    MULTIEDIT_SetText(hItem, "Hello World");
    MULTIEDIT_SetFont(hItem, &XBF_Font);
    MULTIEDIT_SetTextColor(hItem, MULTIEDIT_CI_EDIT, 0x00FF0000);
		MULTIEDIT_EnableBlink(hItem,500,1);
		MULTIEDIT_SetAutoScrollV(hItem,1);
		MULTIEDIT_SetAutoScrollH(hItem,1);
    //
    // Initialization of 'Pend'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT1);
		hPend=hItem;
		MULTIEDIT_SetFont(hItem, &XBF_Font);
    MULTIEDIT_SetText(hItem, "");
		MULTIEDIT_SetAutoScrollV(hItem,1);
		MULTIEDIT_SetAutoScrollH(hItem,1);
		MULTIEDIT_SetReadOnly(hItem,1);
		MULTIEDIT_SetMaxNumChars(hItem,1024);
    //
    // Initialization of 'Send'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON1);
    BUTTON_SetFont(hItem, &XBF_Font);
    BUTTON_SetText(hItem, "发送");
		WM_DisableWindow(hItem);
    //
    // Initialization of 'Clear'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON2);
    BUTTON_SetFont(hItem, GUI_FONT_16B_ASCII);
    BUTTON_SetText(hItem, "Clear");
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case GUI_ID_DROPDOWN0: // Notifications sent by 'Mode'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)				
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
				netmode=DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, GUI_ID_DROPDOWN0));
				i=0;
				while(i<16)
				{
					iptext[i]='\0';
					++i;
				}
				switch (netmode)
				{
					case 1:
							sprintf((char *)iptext,"%d.%d.%d.%d",local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
							EDIT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1),(char *)iptext);
							EDIT_SetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0),local_port);
							WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1));              
						break;
					case 0:
					case 2:
							WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1));
							sprintf((char *)iptext,"%d.%d.%d.%d",remote_ip[0],remote_ip[1],remote_ip[2],remote_ip[3]);
							EDIT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1),(char *)iptext);
							EDIT_SetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0),remote_port);
						break;
				}
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_EDIT0: // Notifications sent by 'Port'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
				messagesnetwork[0]=WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0);
				messagesnetwork[1]=GUI_ID_EDIT0;
				if(!keypadflag)OSTaskQPost(&AppTaskKeypadTCB,
																(void *)&messagesnetwork,
																2,
																OS_OPT_POST_FIFO,
																&err);
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
				if(netmode==1)
					local_port=EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0));
				else
					remote_port=EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0));
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_EDIT1: // Notifications sent by 'IP'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
				messagesnetwork[0]=WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1);
				messagesnetwork[1]=GUI_ID_EDIT1;
				if(!keypadflag)OSTaskQPost(&AppTaskKeypadTCB,
																(void *)&messagesnetwork,
																2,
																OS_OPT_POST_FIFO,
																&err);
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
				if(netmode!=1)
				{
					i=0;
					while(i<16)
					{
						iptext[i]='\0';
						++i;
					}
					EDIT_GetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1),(char *)iptext,15);
					TextToIP(iptext,remote_ip);					
				}
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON0: // Notifications sent by 'Connect'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
				if(keypadflag)WM_DeleteWindow(KEYBOARD);
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				if(IsConnect==0)
				{
					IsConnect=1;//启动连接
					BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0), "断开");
					WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1));
					WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0));
					WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_DROPDOWN0));
					//if(netmode!=1)WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON1));
          WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON1));
				}
				else
				{
					if(netmode!=1)WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1));
					WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON1));					
					WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0));
					WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_DROPDOWN0));
					BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0), "连接");
					remote_port=5000;//调整远端端口为默认值
					local_port=6000;//调整本地端口为默认值
					IsConnect=0;//关闭连接
				}
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_MULTIEDIT0: // Notifications sent by 'Post'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
				messagesnetwork[0]=WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0);
				messagesnetwork[1]=GUI_ID_MULTIEDIT0;
				if(!keypadflag)OSTaskQPost(&AppTaskKeypadTCB,
																(void *)&messagesnetwork,
																2,
																OS_OPT_POST_FIFO,
																&err);
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
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
//    case GUI_ID_MULTIEDIT1: // Notifications sent by 'Pend'
//      switch(NCode) {
//      case WM_NOTIFICATION_CLICKED:
//        // USER START (Optionally insert code for reacting on notification message)
//        // USER END
//        break;
//      case WM_NOTIFICATION_RELEASED:
//        // USER START (Optionally insert code for reacting on notification message)
//        // USER END
//        break;
//      case WM_NOTIFICATION_VALUE_CHANGED:
//        // USER START (Optionally insert code for reacting on notification message)
//        // USER END
//        break;
//      // USER START (Optionally insert additional code for further notification handling)
//      // USER END
//      }
//      break;
    case GUI_ID_BUTTON1: // Notifications sent by 'Send'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
				if(keypadflag)WM_DeleteWindow(KEYBOARD);
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
//				if(netmode==1)break;
				com_data2null(comdata,2048);
				MULTIEDIT_GetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),(char *)comdata,2048);
				com_utf82gbk((const char *)comdata,(char *)comdata);
        switch(netmode)
        {
          case 0:
            send(SOCK_TCPC,comdata,strlen((const char *)comdata));								     	         /*向Server发送数据*/
            break;
          case 1:
            send(SOCK_TCPS,comdata,strlen((const char *)comdata));
           break;
          case 2:
            sendto(SOCK_UDPS,comdata,strlen((const char *)comdata), remote_ip, remote_port);    
           break;
            
        }
//				if(netmode==0)
//						send(SOCK_TCPC,comdata,strlen((const char *)comdata));								     	         /*向Server发送数据*/
//        else if(netmode==2)
//						sendto(SOCK_UDPS,comdata,strlen((const char *)comdata), remote_ip, remote_port);                /*W5500把接收到的数据发送给Remote*/
				MULTIEDIT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),"");
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON2: // Notifications sent by 'Clear'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				MULTIEDIT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT1),"");
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
	case WM_PAINT:                                           
    GUI_SetBkColor(APPBKCOLOR);
		GUI_SetColor(APPTEXTCOLOR);
		GUI_Clear();
		GUI_DispStringAt("IP:",5,5);
		GUI_DispStringAt("端口:",140,5);
		GUI_DispStringAt("发送内容:",5,65);
		GUI_DispStringAt("接收内容:",5,170);
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
*       CreateW5500
*/
void FUN_ICON109Clicked(void)
{
	WM_HWIN hWin;
	OS_INFO("Networkapp create\n");
	netmode=0;//默认是TCP Client
	IsConnect=0;//默认不启动连接
	IsNetInit=0;
	com_data2null(comdata,COMDATA_SIZE);
	hWin=GUI_CreateDialogBox(_aDialogCreateNetwork, GUI_COUNTOF(_aDialogCreateNetwork), _cbDialogNetwork, WM_HBKWIN, 0, 0);
	GUI_Delay(10); 
	gpio_for_w5500_config();	         	/*初始化MCU相关引脚*/
	reset_w5500();					            /*硬复位W5500*/
	set_w5500_mac();                    /*配置MAC地址*/
  IsNetInit=set_w5500_ip();           /*配置IP地址*/
	
  socket_buf_init(txsize, rxsize);    /*初始化8个Socket的发送接收缓存大小*/
	WM_EnableWindow(hFrameClose);
	while(IsNetInit)
	{
		GUI_ExecCreatedDialog(MESSAGEBOX_Create("\r\n The W5500 drive cannot work! \r\n","error",GUI_MESSAGEBOX_CF_MODAL));
		while(1)
		{			
			if(tpad_flag)WM_DeleteWindow(hWin);
			if(!Flag_ICON109)return;
			GUI_Delay(10);
		}
	}
	WM_EnableWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON0));
	while(Flag_ICON109)
	{
		if(IsConnect && (IsNetInit==0))
		{
			switch (netmode)
			{
				case 0://TCP Client
						do_tcp_client();
					break;        
				case 1://TCP Server
						do_tcp_server();
					break;
				case 2://UDP
						do_udp();
					break;
			}
		}
		if(tpad_flag)WM_DeleteWindow(hWin);
		GUI_Delay(10); 
	}
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
