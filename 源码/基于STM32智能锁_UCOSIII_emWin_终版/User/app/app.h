#ifndef	_APP_H_
#define	_APP_H_

#include "includes.h"
#include "GUI.h"
#include "DIALOG.h"
#include "MESSAGEBOX.h"
#include  <os.h>	
#include  <stm32f10x.h>


#define APPBKCOLOR  		GUI_LIGHTGRAY
#define APPTEXTCOLOR    GUI_MAGENTA

/************ KeyInput **************/
extern OS_TCB   AppTaskKeypadTCB;
extern uint8_t  keypadflag;
extern WM_HWIN  KEYBOARD;
void DeleteWindow_KEYBOARD(void);

extern GUI_CONST_STORAGE GUI_BITMAP bmPhone;
extern GUI_CONST_STORAGE GUI_BITMAP bmmessages;

extern GUI_CONST_STORAGE GUI_BITMAP bmkey;
extern GUI_CONST_STORAGE GUI_BITMAP bmrgbled;
extern GUI_CONST_STORAGE GUI_BITMAP bmadc;
extern GUI_CONST_STORAGE GUI_BITMAP bmeeprom;

extern GUI_CONST_STORAGE GUI_BITMAP bmclock;
extern GUI_CONST_STORAGE GUI_BITMAP bmmusic;
extern GUI_CONST_STORAGE GUI_BITMAP bmrecorder;
extern GUI_CONST_STORAGE GUI_BITMAP bmusb;

extern GUI_CONST_STORAGE GUI_BITMAP bmwifi;
extern GUI_CONST_STORAGE GUI_BITMAP bmnet;
extern GUI_CONST_STORAGE GUI_BITMAP bmWeather;
extern GUI_CONST_STORAGE GUI_BITMAP bmcamera;


extern GUI_CONST_STORAGE GUI_BITMAP bmcalculator;
extern GUI_CONST_STORAGE GUI_BITMAP bmpicture;
extern GUI_CONST_STORAGE GUI_BITMAP bmnote;
extern GUI_CONST_STORAGE GUI_BITMAP bmbrowser;


//extern uint8_t Flag_ICON000;
//extern void FUN_ICON000Clicked(void);

//extern uint8_t Flag_ICON001;
//extern void FUN_ICON001Clicked(void);

extern uint8_t Flag_ICON100;
extern void FUN_ICON100Clicked(void);
//extern void FUN_ICON107Clicked(void);

extern uint8_t Flag_ICON101;
extern void FUN_ICON101Clicked(void);

extern uint8_t Flag_ICON102;
extern void FUN_ICON102Clicked(void);

//extern uint8_t Flag_ICON103;
//extern void FUN_ICON103Clicked(void);

//extern uint8_t Flag_ICON104;
//extern void FUN_ICON104Clicked(void);

//extern uint8_t Flag_ICON105;
//extern void FUN_ICON105Clicked(void);

//extern uint8_t Flag_ICON106;
//extern void FUN_ICON106Clicked(void);

extern uint8_t Flag_ICON107;
extern void FUN_ICON107Clicked(void);

//extern uint8_t Flag_ICON108;
//extern void FUN_ICON108Clicked(void);

//extern uint8_t Flag_ICON109;
//extern void FUN_ICON109Clicked(void);

//extern uint8_t Flag_ICON110;
//extern void FUN_ICON110Clicked(void);

extern uint8_t Flag_ICON111;
extern void FUN_ICON111Clicked(void);

//extern uint8_t Flag_ICON200;
//extern void FUN_ICON200Clicked(void);

extern uint8_t Flag_ICON201;
extern void FUN_ICON201Clicked(void);

//extern uint8_t Flag_ICON202;
//extern void FUN_ICON202Clicked(void);

//extern uint8_t Flag_ICON203;
//extern void FUN_ICON203Clicked(void);

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void UserAPP(void);
extern uint8_t UserApp_Flag;
#endif	//_APP_H_
