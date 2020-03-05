#ifndef __COMMON_H_
#define __COMMON_H_

#include "DIALOG.h"
#include  <stdint.h>
#include  <stdarg.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <math.h>
/**************************************************************************************
*
*       Defines
*
***************************************************************************************
*/

#define COMDATA_SIZE			1024*5	

/*********************************************************************
*
*       tpeydef
*
**********************************************************************
*/
/* WAV�ļ�ͷ��ʽ */
typedef __packed struct
{ 
	uint32_t	riff;							/* = "RIFF"	0x46464952*///4
	uint32_t	size_8;						/* ���¸���ַ��ʼ���ļ�β�����ֽ���	*///4
	uint32_t	wave;							/* = "WAVE" 0x45564157*///4
	
	uint32_t	fmt;							/* = "fmt " 0x20746d66*///4
	uint32_t	fmtSize;					/* ��һ���ṹ��Ĵ�С(һ��Ϊ16) *///4	 
	uint16_t	wFormatTag;				/* ���뷽ʽ,һ��Ϊ1	*///2
	uint16_t	wChannels;				/* ͨ������������Ϊ1��������Ϊ2 *///2
	uint32_t	dwSamplesPerSec;	/* ������ *///4
	uint32_t	dwAvgBytesPerSec;	/* ÿ���ֽ���(= ������ �� ÿ���������ֽ���) *///4
	uint16_t	wBlockAlign;			/* ÿ���������ֽ���(=����������/8*ͨ����) *///2
	uint16_t	wBitsPerSample;		/* ����������(ÿ��������Ҫ��bit��) *///2
																			   
	uint32_t	data;							/* = "data" 0x61746164*///4
	uint32_t	datasize;					/* �����ݳ��� *///4
} WavHead;


/*********************************************************************
*
*       data
*
**********************************************************************
*/
extern uint8_t comdata[COMDATA_SIZE];


/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
void com_data2null(uint8_t *data,uint32_t length);

void com_gbk2utf8(const char *src, char *str);
void com_utf82gbk(const char *src, char *str);
void com_createdir(const char *dir);

void ErrorDialog(WM_HWIN hParent,char *string1,char *string2);
#endif //end of __COMMON_H_
