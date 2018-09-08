/*
*********************************************************************************************************
*	                                  
*	ģ������ : GUI����������
*	�ļ����� : MainTask.c
*	��    �� : V1.0
*	˵    �� : GUI����������
*		�汾��   ����         ����            ˵��
*		v1.0    2015-08-05  Eric2013  	      �װ�
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __MainTask_H
#define __MainTask_H

#include "GUI.h"
#include "DIALOG.h"
#include "WM.h"
#include "BUTTON.h"
#include "CHECKBOX.h"
#include "DROPDOWN.h"
#include "EDIT.h"
#include "FRAMEWIN.h"
#include "LISTBOX.h"
#include "MULTIEDIT.h"
#include "RADIO.h"
#include "SLIDER.h"
#include "TEXT.h"
#include "PROGBAR.h"
#include "SCROLLBAR.h"
#include "LISTVIEW.h"
#include "GRAPH.h"
#include "MENU.h"
#include "MULTIPAGE.h"
#include "ICONVIEW.h"
#include "TREEVIEW.h"

#include "ff.h"
#include "can_network.h"
#include "demo_fatfs.h"

/*
************************************************************************
*						  FatFs
************************************************************************
*/
extern FRESULT result;
extern FIL file;
extern DIR DirInf_usb;
extern UINT bw;
extern FATFS fs_nand;
extern FATFS fs_usb;

extern void _WriteByte2File(U8 Data, void * p); 
/*
************************************************************************
*						���ⲿ�ļ�����
************************************************************************
*/
extern void MainTask(void);
extern void TOUCH_Calibration(void);
int getChData(char dataNum);
struct _CONFIG_CHANNEL_ getChannel(char chNum);
//void saveVCT(WM_MESSAGE * pMsg);

#endif

/*****************************(END OF FILE) *********************************/
