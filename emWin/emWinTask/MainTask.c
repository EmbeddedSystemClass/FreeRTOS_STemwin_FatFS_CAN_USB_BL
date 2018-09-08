/*
*********************************************************************************************************
*	                                  
*	模块名称 : 外置XBF格式字库到SPI FLASH
*	文件名称 : MainTask.c
*	版    本 : V1.0
*	说    明 : 支持点阵：
*                1. 宋体16点阵，24点阵，32点阵的unicode编码完整字体。
*              字库存储方式：
*                1. 字库存储到SPI FLASH里面。
*              实验步骤如下：
*                1. 将字库文件font.bin放到SD卡根目录中，系统上电后会将其加载到SPI FLASH里面。
*                2. 板子上电后会有一个加载进度指示，存储到SPI FLASH成功后会显示一个对话框。
*                3. 字体文件为7.28MB，SPI FLASH为8MB，加载过程稍有些慢，请耐心等待。
*              
*	修改记录 :
*		版本号   日期         作者          说明
*		V1.0    2016-07-16   Eric2013  	    首版    
*                                     
*	Copyright (C), 2015-2020, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "MainTask.h"
#include "includes.h"
#include "Logo-Cell.c"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "demo_fatfs.h"
#include "DROPDOWN.h"

extern TaskHandle_t xHandleTaskUserIF;
extern TaskHandle_t xHandleTaskMsgPro;
extern char flag_correct ;
//extern QueueHandle_t xQueue1;
//extern QueueHandle_t xQueue2;

WM_HWIN hWinInfo;    /* 信息窗口句柄 */
WM_HWIN hWinInfoSave;    /* 信息窗口句柄 */
WM_HWIN  hWinMain;   
WM_HWIN hWinSet;    
WM_HWIN hWinLast;    /* 信息窗口句柄 */

BUTTON_SKINFLEX_PROPS BUTTON_pProps;
DROPDOWN_SKINFLEX_PROPS DROPDOWN_pProps;

//uint8_t mode_flag=0;
char pro_num = 0;

extern char cfgname[40][16];
extern uint8_t BL_data[8];
extern CONFIG_PARA cfg_para;
extern struct _CONFIG_DATA cfg_data_tmp;
extern struct _CONFIG_CHANNEL_ cfg_ch_tmp;
extern struct _CONFIG_CHANNEL cfg_allch_tmp;
extern void OP_JiaoZhun(void);

//char proname[640][16];
char currentPro = 0;
uint8_t  channel_num = 0;
char  cfg_flag = 0;
char file_name[22]={0};
CONFIG_PIC  picArray;
char change_pro = 0;
char change_pic = 0;
char change_ch = 0;
char change_data = 0;
char save_flag = 1;
char setflag = 0;
char pageflag = 0;
char delFlag = 0;

#define ID_Timer1   0
#define ID_Timer2   1
#define LEN_ITEM_TEXT 15

/*
*********************************************************************************************************
*	                                  调用外部字体
*********************************************************************************************************
*/
extern GUI_CONST_STORAGE GUI_FONT GUI_Fontyahei36;
extern GUI_CONST_STORAGE GUI_FONT GUI_Fontyahei30;
extern GUI_CONST_STORAGE GUI_FONT GUI_Fontkai24;
extern GUI_CONST_STORAGE GUI_FONT GUI_Fontyahei72;
extern GUI_CONST_STORAGE GUI_FONT GUI_Fontyahei48;
extern GUI_CONST_STORAGE GUI_FONT GUI_Fontkai20;

WM_HWIN CreateWindow1(void);
void CreateFramewin(void);
void CreateFramewin4(void);
void CreateFramewin5(void);

typedef struct {
  char *   pText;
  int      Width;
  int      Align;
  int    (*fpCompare)(const void * p0, const void * p1);
} COL_PROP;

typedef struct {
  U32 Row;
  U32 Col;
} CELL_EDIT_ORG;

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0		(GUI_ID_USER + 0x00)
#define ID_TEXT_0				(GUI_ID_USER + 0x01)
#define ID_DROPDOWN_0		(GUI_ID_USER + 0x02)
#define ID_TEXT_1				(GUI_ID_USER + 0x03)
#define ID_TEXT_2				(GUI_ID_USER + 0x04)
#define ID_TEXT_3				(GUI_ID_USER + 0x05)
#define ID_TEXT_4				(GUI_ID_USER + 0x06)
#define ID_TEXT_5				(GUI_ID_USER + 0x07)
#define ID_TEXT_6				(GUI_ID_USER + 0x08)
#define ID_TEXT_7				(GUI_ID_USER + 0x09)
#define ID_TEXT_8				(GUI_ID_USER + 0x0A)
#define ID_TEXT_9				(GUI_ID_USER + 0x0B)
#define ID_TEXT_10			(GUI_ID_USER + 0x0C)
#define ID_TEXT_11			(GUI_ID_USER + 0x0D)
#define ID_TEXT_12			(GUI_ID_USER + 0x0E)
#define ID_EDIT_0				(GUI_ID_USER + 0x0F)
#define ID_EDIT_1				(GUI_ID_USER + 0x10)
#define ID_EDIT_2				(GUI_ID_USER + 0x11)
#define ID_TEXT_13			(GUI_ID_USER + 0x12)
#define ID_TEXT_14			(GUI_ID_USER + 0x13)
#define ID_TEXT_15			(GUI_ID_USER + 0x14)
#define ID_DROPDOWN_1		(GUI_ID_USER + 0x15)   
#define ID_DROPDOWN_2		(GUI_ID_USER + 0x16)
#define ID_TEXT_16			(GUI_ID_USER + 0x17)   
#define ID_TEXT_17			(GUI_ID_USER + 0x18)   
#define ID_TEXT_18			(GUI_ID_USER + 0x19)   
#define ID_TEXT_19			(GUI_ID_USER + 0x1A) 
#define ID_EDIT_3				(GUI_ID_USER + 0x1F)
#define ID_EDIT_4				(GUI_ID_USER + 0x20)
#define ID_EDIT_5				(GUI_ID_USER + 0x21)
#define ID_EDIT_6				(GUI_ID_USER + 0x22)
#define ID_EDIT_7				(GUI_ID_USER + 0x23)
#define ID_EDIT_8				(GUI_ID_USER + 0x24)
#define ID_EDIT_9				(GUI_ID_USER + 0x25)
#define ID_EDIT_10			(GUI_ID_USER + 0x26)
#define ID_EDIT_11			(GUI_ID_USER + 0x27)
#define ID_EDIT_12			(GUI_ID_USER + 0x28)
#define ID_EDIT_13			(GUI_ID_USER + 0x29)
#define ID_EDIT_14			(GUI_ID_USER + 0x2A)
#define ID_LISTVIEW_0		(GUI_ID_USER + 0x2B)
#define ID_BUTTON_0			(GUI_ID_USER + 0x2C)
#define ID_BUTTON_1			(GUI_ID_USER + 0x2D)		
#define ID_BUTTON_2			(GUI_ID_USER + 0x2E)		
#define ID_BUTTON_3			(GUI_ID_USER + 0x2F)		
#define ID_BUTTON_4			(GUI_ID_USER + 0x30)

#define WM_LIMIT 				(WM_USER + 0x00)

static const GUI_WIDGET_CREATE_INFO _aDialogCreateInfo[] = 
{
	{ FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 250, 180, 300, 120, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_0, 40, 30, 220, 24, 0, 0x64, 0 },
};

static void _cbDialogInfo(WM_MESSAGE * pMsg) 
{
	WM_HWIN hItem;

	switch (pMsg->MsgId) 
	{
		/* 显示armfly的logo*/
		case WM_PAINT:
			//GUI_DrawBitmap(&bmLogo_armflySmall, 50,130);
			break;
		
		case WM_INIT_DIALOG:
			
			/* 初始化框架窗口 */
			hItem = pMsg->hWin;
			FRAMEWIN_SetFont(hItem, &GUI_Fontkai24);
			FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			FRAMEWIN_SetText(hItem, "删除信息");
			FRAMEWIN_SetTextColor(hItem, 0x00000000);
		
			/* 初始化ID_TEXT_0到ID_TEXT_3*/
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
			TEXT_SetFont(hItem, &GUI_Fontkai24);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetText(hItem, "正在删除···");
			break;
			
		default:
			WM_DefaultProc(pMsg);
	}
}

void CreateFramewin(void) 
{
	hWinInfo = GUI_CreateDialogBox(_aDialogCreateInfo, GUI_COUNTOF(_aDialogCreateInfo), _cbDialogInfo, WM_HBKWIN, 0, 0);
}

void DeleteFramewin(void)
{
	WM_DeleteWindow(hWinInfo);
}

static const GUI_WIDGET_CREATE_INFO _aDialogCreateInfo2[] = 
{
	{ FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 250, 180, 300, 120, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_0, 40, 30, 220, 24, 0, 0x64, 0 },
};

static void _cbDialogInfo2(WM_MESSAGE * pMsg) 
{
	WM_HWIN hItem;

	switch (pMsg->MsgId) 
	{
		/* 显示armfly的logo*/
		case WM_PAINT:
			//GUI_DrawBitmap(&bmLogo_armflySmall, 50,130);
			break;
		
		case WM_INIT_DIALOG:
			
			/* 初始化框架窗口 */
			hItem = pMsg->hWin;
			FRAMEWIN_SetFont(hItem, &GUI_Fontkai24);
			FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			FRAMEWIN_SetText(hItem, "保存信息");
			FRAMEWIN_SetTextColor(hItem, 0x00000000);
		
			/* 初始化ID_TEXT_0到ID_TEXT_3*/
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
			TEXT_SetFont(hItem, &GUI_Fontkai24);
			TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			TEXT_SetText(hItem, "正在保存···");
			break;
			
		default:
			WM_DefaultProc(pMsg);
	}
}

void CreateFramewin3(void) 
{
	hWinInfoSave = GUI_CreateDialogBox(_aDialogCreateInfo, GUI_COUNTOF(_aDialogCreateInfo2), _cbDialogInfo2, WM_HBKWIN, 0, 0);
}

void DeleteFramewin3(void)
{
	WM_DeleteWindow(hWinInfoSave);
}

/*
*********************************************************************************************************
*	                       GUI_WIDGET_CREATE_INFOÀàÐÍÊý×é
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate4[] = 
{
	{ FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 800, 480, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_0, 170, 10, 700, 48, 0, 0x64, 0 },
  { DROPDOWN_CreateIndirect, "Dropdown", ID_DROPDOWN_0, 250, 100, 210, 29, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_1, 60, 100, 160, 35, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_2, 60, 160, 160, 35, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_3, 60, 220, 160, 35, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_4, 60, 280, 160, 35, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_5, 60, 340, 160, 35, 0, 0x64, 0 },

  { TEXT_CreateIndirect, "Text", ID_TEXT_6, 250, 160, 160, 35, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_7, 250, 220, 160, 35, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_8, 250, 280, 160, 35, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_9, 250, 340, 160, 35, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_10, 150, 440, 700, 35, 0, 0x64, 0 },
  
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 550, 100, 170, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_1, 550, 160, 170, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_2, 550, 220, 170, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_3, 550, 280, 170, 35, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_4, 550, 340, 170, 35, 0, 0x0, 0 },
};


static void _cbDialog4(WM_MESSAGE * pMsg) 
{
	WM_HWIN hItem;
	int     NCode;
	int     Id;
	char buf[16]={0},buf1[16];
	char fileName[22]={0}; 
	int temp;
	float temp_f;
	unsigned char i;
//	char pro_num = 0;
	BaseType_t xResult;
	uint32_t ulValue;

	switch (pMsg->MsgId) {
	case WM_INIT_DIALOG:
		//
		// Initialization of 'Window'
		//
		hItem = pMsg->hWin;
//		WINDOW_SetBkColor(hItem, 0x00D8E9EC);
		FRAMEWIN_SetTitleVis(hItem, 0);
		FRAMEWIN_SetClientColor(hItem,0x00D8E9EC);
//			WM_SetFocus(hItem);
		//
		// Initialization of 'Dropdown'
		//
	{
		DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);    //先设置成默认效果
		DROPDOWN_GetSkinFlexProps(&DROPDOWN_pProps, DROPDOWN_SKINFLEX_PI_FOCUSSED); //读取数据到结构体BUTTON_pProps中  
		DROPDOWN_pProps.aColorFrame[0] = GUI_LIGHTBLUE;    //设置颜色  
		DROPDOWN_pProps.aColorFrame[1] = GUI_LIGHTBLUE;  
		DROPDOWN_pProps.aColorFrame[2] = GUI_LIGHTBLUE;
		DROPDOWN_pProps.aColorLower[0] = GUI_LIGHTBLUE;
		DROPDOWN_pProps.aColorLower[1] = GUI_LIGHTBLUE;
		DROPDOWN_pProps.aColorUpper[0] = GUI_LIGHTBLUE;
		DROPDOWN_pProps.aColorUpper[1] = GUI_LIGHTBLUE;		
		DROPDOWN_SetSkinFlexProps(&DROPDOWN_pProps, DROPDOWN_SKINFLEX_PI_FOCUSSED);
		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
			WM_SetFocus(hItem);
		DROPDOWN_SetFont(hItem, GUI_FONT_32B_1);
		DROPDOWN_SetAutoScroll(hItem, 1);
//		DROPDOWN_AddString(hItem, "工程1");
//		DROPDOWN_AddString(hItem, "工程2");
//		DROPDOWN_AddString(hItem, "工程3");
//		len = max_pro_num;
//		ViewRootDir(FS_VOLUME_SD,proname);
		if(cfg_flag)
		{
			cfg_flag = 0;
			memset(file_name,0,sizeof(file_name));
			sprintf(file_name, "proname");
//			ReadFileData(FS_VOLUME_SD,file_name,&cfgname,sizeof(cfgname));
			ReadFileData(FS_VOLUME_SD,file_name,&currentPro,1);
		}
		for(i=0;cfgname[i][0] != '\0';i++)
		{
			sprintf(buf, "%d%s", i,":");
			strcpy(buf1,cfgname[i]);
			strcat(buf,buf1);
			DROPDOWN_AddString(hItem, buf);
		}
//			memset(file_name,0,sizeof(file_name));
//			sprintf(file_name, "jiaozhun");
//			ReadFileData(FS_VOLUME_SD,file_name,&cfgname,sizeof(cfgname));
//		correctPara();
		if(i>6)
			DROPDOWN_SetListHeight(hItem, 32*6);
		else
			DROPDOWN_SetListHeight(hItem, 32*i);
//		DROPDOWN_SetSel(hItem,cfgname[0][15]);
		DROPDOWN_SetSel(hItem,currentPro);
	}
		//
		// Initialization of 'Text'
		//
	{
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
//		TEXT_SetFont(hItem, &XBF_Font32);
		TEXT_SetFont(hItem, &GUI_Fontyahei48);
		TEXT_SetText(hItem, "上海帆声图像科技有限公司");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, &GUI_Fontyahei36);
		TEXT_SetText(hItem, "选择工程:");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, &GUI_Fontyahei36);
		TEXT_SetText(hItem, "当前工程:");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, &GUI_Fontyahei36);
		TEXT_SetText(hItem, "当前图片:");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, &GUI_Fontyahei36);
		TEXT_SetText(hItem, "背光电压:");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, &GUI_Fontyahei36);
		TEXT_SetText(hItem, "背光电流:");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
		TEXT_SetFont(hItem, GUI_FONT_32B_1);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_7);
		TEXT_SetFont(hItem, GUI_FONT_32B_1);		
		memset(fileName,0,sizeof(fileName));
		sprintf(fileName,"%s00",cfgname[0]);
		ReadFileData(FS_VOLUME_SD,fileName,&picArray,sizeof(CONFIG_PIC));
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
		TEXT_SetFont(hItem, GUI_FONT_32B_1);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_9);
		TEXT_SetFont(hItem, GUI_FONT_32B_1);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_10);
		TEXT_SetFont(hItem, GUI_FONT_24_1);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetText(hItem, "Shanghai Freesense Image Technology Co,.Ltd.          V4014.01.02");
	}	
    //
    // Initialization of 'Button'
    //
	{		
		BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);    //先设置成默认效果
		BUTTON_GetSkinFlexProps(&BUTTON_pProps, BUTTON_SKINFLEX_PI_FOCUSSED); //读取数据到结构体BUTTON_pProps中  
		BUTTON_pProps.aColorFrame[0] = GUI_LIGHTBLUE;    //设置颜色  
		BUTTON_pProps.aColorFrame[1] = GUI_LIGHTBLUE;  
		BUTTON_pProps.aColorFrame[2] = GUI_LIGHTBLUE;
		BUTTON_pProps.aColorLower[0] = GUI_LIGHTBLUE;
		BUTTON_pProps.aColorLower[1] = GUI_LIGHTBLUE;
		BUTTON_pProps.aColorUpper[0] = GUI_LIGHTBLUE;
		BUTTON_pProps.aColorUpper[1] = GUI_LIGHTBLUE;		
		BUTTON_SetSkinFlexProps(&BUTTON_pProps, BUTTON_SKINFLEX_PI_FOCUSSED);
		
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem, &GUI_Fontyahei36);
    BUTTON_SetText(hItem, "加载配置");
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetFont(hItem, &GUI_Fontyahei36);
    BUTTON_SetText(hItem, "删除");
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
    BUTTON_SetFont(hItem, &GUI_Fontyahei36);
    BUTTON_SetText(hItem, "复位");
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
    BUTTON_SetFont(hItem, &GUI_Fontyahei36);
    BUTTON_SetText(hItem, "倒配置到U盘");
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
    BUTTON_SetFont(hItem, &GUI_Fontyahei36);
    BUTTON_SetText(hItem, "设置");
		break;
	}
		
	case WM_PAINT:
	{		
		GUI_SetColor(0x00FFFFFF);
		GUI_DrawHLine(60,20,780);
		GUI_DrawHLine(430,20,780);
		GUI_DrawVLine(500,70,420);
		GUI_SetColor(0x00ACA899);
		GUI_DrawHLine(59,19,779);
		GUI_DrawHLine(429,19,779);
		GUI_DrawVLine(499,70,420);
	}
		break;
			
	case WM_TIMER:
		if(WM_GetTimerId(pMsg->Data.v) == ID_Timer1)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
			if(WM_HasFocus(hItem))
				xTaskNotify(xHandleTaskUserIF,BIT_21,eSetBits);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
			xResult = xTaskNotifyWait(0x00000000,0xFFFFFFFF,&ulValue,xMaxBlockTime);
			if( xResult == pdPASS )
			{
				if((ulValue & BIT_23) != 0)
					pro_num = ulValue & 0xffff;
				DROPDOWN_SetSel(hItem,pro_num);
			}
			pro_num = DROPDOWN_GetSel(hItem);
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
			TEXT_SetText(hItem, cfgname[pro_num]);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_7);
			if(change_pro)
			{
				change_pro = 0;
				memset(fileName,0,sizeof(fileName));
				sprintf(fileName,"%s00",cfgname[pro_num]);
				ReadFileData(FS_VOLUME_SD,fileName,&picArray,sizeof(CONFIG_PIC));
//				cfgname[0][15]=pro_num;
				currentPro = pro_num;
				CreateNewFile(0,0,1);
			}
			sprintf(buf, "%d--", picArray.picNum);
			strcpy(buf1,picArray.picName);
			strcat(buf,buf1);
			TEXT_SetText(hItem, buf);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
			temp=((unsigned int)BL_data[0]<<8)|((unsigned int)BL_data[1]);    //BL 	U
			temp_f=temp/10.0;
//			temp_f=temp/10;
//			temp_f=(temp+5)/10;
			if(temp_f<0)
				temp_f=0;
//			if((temp_f > 0)  && (temp_f != cfg_para.voltage))
//			{
//				temp_f = cfg_para.voltage/100;
//			}
			sprintf(buf, "%.2f", temp_f);
			strcat(buf," V");
			TEXT_SetText(hItem, buf);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_9);
			temp=((unsigned int)BL_data[2]<<8)|((unsigned int)BL_data[3]);    //BL 	I
//			temp_f=(2.979*temp/10240-0.5)*1000; 
			if(temp<0)
				temp=0;
//			if(temp>cfg_para.current/100)
//				temp = cfg_para.current/100;
			sprintf(buf, "%d", temp);
			strcat(buf," mA");
			TEXT_SetText(hItem, buf);			
				
			if(WM_IsWindow(hWinInfo) && delFlag)
			{
				delFlag = 0;
				GUI_Delay(300);
				DeleteFramewin();
				WM_SetFocus(hWinMain);
			}
		}
		/* 重启定时器 */
		WM_RestartTimer(pMsg->Data.v, 100);
		break;
				
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
    
		switch (Id) {
			case ID_DROPDOWN_0: // Notifications sent by 'Dropdown'
				switch (NCode) {
				case WM_NOTIFICATION_CLICKED:
					break;
				case WM_NOTIFICATION_RELEASED:
					break;
				case WM_NOTIFICATION_SEL_CHANGED:
					change_pro = 1;
					break;
				}
				break;
			case ID_BUTTON_0: // Notifications sent by 'Button' 
			  switch(NCode) {
			  case WM_NOTIFICATION_CLICKED:
				break;
			  case WM_NOTIFICATION_RELEASED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
				pro_num = DROPDOWN_GetSel(hItem);	
				xTaskNotify(xHandleTaskMsgPro,BIT_16 | pro_num,eSetValueWithOverwrite);
				GUI_Delay(2);
				xTaskNotify(xHandleTaskMsgPro,BIT_19,eSetBits);
				GUI_Delay(2);
				break;
			  }
			  break;
			case ID_BUTTON_1: // Notifications sent by 'Button' 
			  switch(NCode) {
			  case WM_NOTIFICATION_CLICKED:
				break;
			  case WM_NOTIFICATION_RELEASED:
				if(!WM_IsWindow(hWinInfo))
				{
					CreateFramewin();
				}
				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
				pro_num = DROPDOWN_GetSel(hItem);
				memset(buf,0,sizeof(buf));
				memset(buf1,0,sizeof(buf1));
				DROPDOWN_GetItemText(hItem,pro_num,buf,sizeof(buf));
				strncpy(buf1,buf+2,strlen(buf)-2);
				for(i=pro_num;cfgname[i][0] != '\0';i++)
				{
						strcpy(cfgname[i],cfgname[i+1]);
				}
				DROPDOWN_DeleteItem(hItem,pro_num);
				DeleteDirFile(FS_VOLUME_SD,buf1);
//				CreateNewFile(0,0,1);
				delFlag = 1;
				break;
			  }
			  break;
			case ID_BUTTON_2: // Notifications sent by 'Button' 
			  switch(NCode) {
			  case WM_NOTIFICATION_CLICKED:
				break;
			  case WM_NOTIFICATION_RELEASED:
				__set_FAULTMASK(1);      // 关闭所有中端
				NVIC_SystemReset();// 复位
				break;
			  }
			  break;
			case ID_BUTTON_3: // Notifications sent by 'Button' 
			  switch(NCode) {
			  case WM_NOTIFICATION_CLICKED:
				break;
			  case WM_NOTIFICATION_RELEASED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
				pro_num = DROPDOWN_GetSel(hItem);
				xTaskNotify(xHandleTaskMsgPro,BIT_16 | pro_num,eSetValueWithOverwrite);
				GUI_Delay(2);
//				memset(buf,0,sizeof(buf));
//				memset(file_name,0,sizeof(file_name));
//				strncpy(buf,cfgname[pro_num],strlen(cfgname[pro_num]));
//				sprintf(file_name, "%s%02d",buf, 0);
//				xTaskNotify(xHandleTaskMsgPro,BIT_18,eSetBits);
//				GUI_Delay(10);
				xTaskNotify(xHandleTaskMsgPro,BIT_22,eSetBits);
				GUI_Delay(10);
				break;
			  }
			  break;
			case ID_BUTTON_4: // Notifications sent by 'Button'
			  switch(NCode) {
			  case WM_NOTIFICATION_CLICKED:
				break;
			  case WM_NOTIFICATION_RELEASED:
//				WM_DeleteTimer(ID_Timer1);
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_4));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_5));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_8));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_9));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_10));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4));
//							GUI_EndDialog(pMsg->hWin, 0);
//				DeleteFramewin(pMsg->hWin);
//				pMsg->hWin = 0;
//							CreateFramewin5();
//				DeleteFramewin(hWinMain);
//				hWinSet=CreateFramewin3();
//				WM_Paint(hWinSet);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
			WM_SetFocus(hItem);	
				WM_HideWindow(hWinMain);
				WM_ShowWindow(hWinSet);
			WM_SetFocus(hWinSet);
			pageflag = 1;
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

void CreateFramewin4(void) 
{
//	WM_HWIN hWin;

	hWinMain = GUI_CreateDialogBox(_aDialogCreate4, GUI_COUNTOF(_aDialogCreate4), _cbDialog4, WM_HBKWIN, 0, 0);
	WM_CreateTimer(WM_GetClientWindow(hWinMain),ID_Timer1,10,0);
//	return hWin;
}

static void refreshBlSet(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);
	EDIT_SetFloatMode(hItem,(float)cfg_para.voltage/100,0,30,2,2);
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_1);
	EDIT_SetFloatMode(hItem,cfg_para.current/100,0,300,0,2);
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_2);
	EDIT_SetFloatMode(hItem,cfg_para.totalTime,0,999999,0,2);
}

static void refreshZU(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_3);
	EDIT_SetFloatMode(hItem,(float)cfg_para.cfg_ch_zu.ch_zu1_h/100-31,-20,20,2,2);		
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_4);
	EDIT_SetFloatMode(hItem,(float)cfg_para.cfg_ch_zu.ch_zu1_l/100-31,-20,20,2,2);
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_5);
	EDIT_SetFloatMode(hItem,(float)cfg_para.cfg_ch_zu.ch_zu2_h/100-31,-20,20,2,2);		
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_6);
	EDIT_SetFloatMode(hItem,(float)cfg_para.cfg_ch_zu.ch_zu2_l/100-31,-20,20,2,2);
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_7);
	EDIT_SetFloatMode(hItem,(float)cfg_para.cfg_ch_zu.ch_zu3_h/100-31,-20,20,2,2);		
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_8);
	EDIT_SetFloatMode(hItem,(float)cfg_para.cfg_ch_zu.ch_zu3_l/100-31,-20,20,2,2);
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_9);
	EDIT_SetFloatMode(hItem,(float)cfg_para.cfg_ch_zu.ch_zu4_h/100-31,-20,20,2,2);		
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_10);
	EDIT_SetFloatMode(hItem,(float)cfg_para.cfg_ch_zu.ch_zu4_l/100-31,-20,20,2,2);
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_11);
	EDIT_SetFloatMode(hItem,(float)cfg_para.cfg_ch_zu.ch_zu5_h/100-31,-20,20,2,2);		
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_12);
	EDIT_SetFloatMode(hItem,(float)cfg_para.cfg_ch_zu.ch_zu5_l/100-31,-20,20,2,2);
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_13);
	EDIT_SetFloatMode(hItem,(float)cfg_para.cfg_ch_zu.ch_zu6_h/100-31,-20,20,2,2);		
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_14);
	EDIT_SetFloatMode(hItem,(float)cfg_para.cfg_ch_zu.ch_zu6_l/100-31,-20,20,2,2);
}

static void getChName(char num,char *name)
{	
	switch(num)
	{
		case 0:
			strcpy(name,cfg_para.cfg_ch.cfg_ch1.ch_name);
			break;
		case 1:
			strcpy(name,cfg_para.cfg_ch.cfg_ch2.ch_name);
			break;
		case 2:
			strcpy(name,cfg_para.cfg_ch.cfg_ch3.ch_name);
			break;
		case 3:
			strcpy(name,cfg_para.cfg_ch.cfg_ch4.ch_name);
			break;
		case 4:
			strcpy(name,cfg_para.cfg_ch.cfg_ch5.ch_name);
			break;
		case 5:
			strcpy(name,cfg_para.cfg_ch.cfg_ch6.ch_name);
			break;
		case 6:
			strcpy(name,cfg_para.cfg_ch.cfg_ch7.ch_name);
			break;
		case 7:
			strcpy(name,cfg_para.cfg_ch.cfg_ch8.ch_name);
			break;
		case 8:
			strcpy(name,cfg_para.cfg_ch.cfg_ch9.ch_name);
			break;
		case 9:
			strcpy(name,cfg_para.cfg_ch.cfg_ch10.ch_name);
			break;
		
		case 10:
			strcpy(name,cfg_para.cfg_ch.cfg_ch11.ch_name);
			break;
		case 11:
			strcpy(name,cfg_para.cfg_ch.cfg_ch12.ch_name);
			break;
		case 12:
			strcpy(name,cfg_para.cfg_ch.cfg_ch13.ch_name);
			break;
		case 13:
			strcpy(name,cfg_para.cfg_ch.cfg_ch14.ch_name);
			break;
		case 14:
			strcpy(name,cfg_para.cfg_ch.cfg_ch15.ch_name);
			break;
		case 15:
			strcpy(name,cfg_para.cfg_ch.cfg_ch16.ch_name);
			break;
		case 16:
			strcpy(name,cfg_para.cfg_ch.cfg_ch17.ch_name);
			break;
		case 17:
			strcpy(name,cfg_para.cfg_ch.cfg_ch18.ch_name);
			break;
		case 18:
			strcpy(name,cfg_para.cfg_ch.cfg_ch19.ch_name);
			break;
		case 19:
			strcpy(name,cfg_para.cfg_ch.cfg_ch20.ch_name);
			break;
		
		case 20:
			strcpy(name,cfg_para.cfg_ch.cfg_ch21.ch_name);
			break;
		case 21:
			strcpy(name,cfg_para.cfg_ch.cfg_ch22.ch_name);
			break;
		case 22:
			strcpy(name,cfg_para.cfg_ch.cfg_ch23.ch_name);
			break;
		case 23:
			strcpy(name,cfg_para.cfg_ch.cfg_ch24.ch_name);
			break;
		case 24:
			strcpy(name,cfg_para.cfg_ch.cfg_ch25.ch_name);
			break;
		case 25:
			strcpy(name,cfg_para.cfg_ch.cfg_ch26.ch_name);
			break;
		case 26:
			strcpy(name,cfg_para.cfg_ch.cfg_ch27.ch_name);
			break;
		case 27:
			strcpy(name,cfg_para.cfg_ch.cfg_ch28.ch_name);
			break;
		case 28:
			strcpy(name,cfg_para.cfg_ch.cfg_ch29.ch_name);
			break;
		case 29:
			strcpy(name,cfg_para.cfg_ch.cfg_ch30.ch_name);
			break;		
		
		case 30:
			strcpy(name,cfg_para.cfg_ch.cfg_ch31.ch_name);
			break;
		case 31:
			strcpy(name,cfg_para.cfg_ch.cfg_ch32.ch_name);
			break;
		case 32:
			strcpy(name,cfg_para.cfg_ch.cfg_ch33.ch_name);
			break;
		case 33:
			strcpy(name,cfg_para.cfg_ch.cfg_ch34.ch_name);
			break;
		case 34:
			strcpy(name,cfg_para.cfg_ch.cfg_ch35.ch_name);
			break;
		case 35:
			strcpy(name,cfg_para.cfg_ch.cfg_ch36.ch_name);
			break;
		case 36:
			strcpy(name,cfg_para.cfg_ch.cfg_ch37.ch_name);
			break;
	}
}

struct _CONFIG_CHANNEL_ getChannel(char chNum)
{
	struct _CONFIG_CHANNEL_ cfgChTemp;
	switch(chNum)
		{
			case 0:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch1;
				break;
			case 1:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch2;
				break;
			case 2:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch3;
				break;
			case 3:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch4;
				break;			
			case 4:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch5;
				break;
			case 5:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch6;
				break;
			case 6:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch7;
				break;
			case 7:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch8;
				break;			
			case 8:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch9;
				break;
			case 9:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch10;
				break;
			
			case 10:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch11;
				break;			
			case 11:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch12;
				break;			
			case 12:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch13;
				break;
			case 13:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch14;
				break;
			case 14:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch15;
				break;
			case 15:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch16;
				break;			
			case 16:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch17;
				break;
			case 17:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch18;
				break;
			case 18:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch19;
				break;
			case 19:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch20;
				break;
			
			case 20:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch21;
				break;			
			case 21:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch22;
				break;
			case 22:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch23;
				break;
			case 23:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch24;
				break;
			case 24:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch25;
				break;
			case 25:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch26;
				break;			
			case 26:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch27;
				break;
			case 27:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch28;
				break;
			case 28:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch29;
				break;
			case 29:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch30;
				break;
			
			case 30:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch31;
				break;			
			case 31:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch32;
				break;
			case 32:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch33;
				break;
			case 33:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch34;
				break;
			case 34:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch35;
				break;
			case 35:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch36;
				break;			
			case 36:
				cfgChTemp = cfg_para.cfg_ch.cfg_ch37;
				break;
		}
		return cfgChTemp;
}

static void calculateChData(WM_HWIN hItem,char ch_num)
{
  char    i,j;
//  char    NumRows;
  char * apText[6];
  char   acText[6][10] = {0};
	int datatmp=0;
	char flag;
	
	char type = 0;
	char level = 0;
	unsigned short hl_time =0;
	float voltage =0;
//	char loopNum = 0;
	char rep_times =0;
	char lp_addr = 0;
	
	if(ch_num<24)
		flag = 0;
	else
		flag = 1;
	
	for(i=0;i<16;i++)
	{
		datatmp = getChData(i);
		if(datatmp==0)
			break;
		else //if((datatmp==1) && (flag==0))
		{
			type = datatmp >> 31;
			if(type==0)
			{
				if(flag==0){
					level = (datatmp >> 30)  & 0x01;
					hl_time = datatmp & 0xffff;
				}
				else{
					hl_time = (datatmp >> 15)  & 0xffff;
					voltage = (datatmp & 0x7fff)/100.0-31;
				}
			}else{
//				loopNum = (datatmp >> 20)  & 0x07;
				lp_addr = (datatmp >> 16)  & 0x0f;
				rep_times = datatmp & 0x03ff;
			}
			
			memset(acText,0,sizeof(acText));
//			NumRows = LISTVIEW_GetNumRows(hItem);
			sprintf(acText[0],"%d",i);
			if(type){
				strcpy(acText[1],"Loop");
				sprintf(acText[4],"%d",lp_addr);
				sprintf(acText[5],"%d",rep_times);
			}
			else{
				strcpy(acText[1],"SetV");
				if(flag)
					sprintf(acText[2],"%.2f",voltage);
				else{
//					getZuValue(ch_num,&levelH,&levelL);
//					if(level)
						sprintf(acText[2],"%d",level);
//					else
//						sprintf(acText[2],"%.2f",levelL);
				}
				sprintf(acText[3],"%d",hl_time);
			}

			for (j = 0; j < 6; j++) {
				apText[j] = acText[j];
			}
			LISTVIEW_AddRow(hItem, (const GUI_ConstString *)apText);
		}
	}
}

static void addRow(WM_HWIN hItem) 
{
//	memset(&cfg_ch_tmp,0,sizeof(cfg_ch_tmp));
	cfg_ch_tmp = getChannel(channel_num);
	cfg_data_tmp = cfg_ch_tmp.cfg_data;
	calculateChData(hItem,channel_num);
}

static void cmpChNum(WM_HWIN hItem)
{
	char i;
	
	for(i=0;i<37;i++)
		DROPDOWN_SetItemDisabled(hItem, i,1);
	
	if(cfg_para.cfg_ch.cfg_ch1.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 0,0);
	if(cfg_para.cfg_ch.cfg_ch2.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 1,0);
	if(cfg_para.cfg_ch.cfg_ch3.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 2,0);
	if(cfg_para.cfg_ch.cfg_ch4.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 3,0);
	if(cfg_para.cfg_ch.cfg_ch5.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 4,0);
	if(cfg_para.cfg_ch.cfg_ch6.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 5,0);
	if(cfg_para.cfg_ch.cfg_ch7.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 6,0);
	if(cfg_para.cfg_ch.cfg_ch8.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 7,0);
	if(cfg_para.cfg_ch.cfg_ch9.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 8,0);
	if(cfg_para.cfg_ch.cfg_ch10.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 9,0);
	
	if(cfg_para.cfg_ch.cfg_ch11.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 10,0);
	if(cfg_para.cfg_ch.cfg_ch12.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 11,0);
	if(cfg_para.cfg_ch.cfg_ch13.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 12,0);
	if(cfg_para.cfg_ch.cfg_ch14.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 13,0);
	if(cfg_para.cfg_ch.cfg_ch15.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 14,0);
	if(cfg_para.cfg_ch.cfg_ch16.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 15,0);
	if(cfg_para.cfg_ch.cfg_ch17.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 16,0);
	if(cfg_para.cfg_ch.cfg_ch18.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 17,0);
	if(cfg_para.cfg_ch.cfg_ch19.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 18,0);
	if(cfg_para.cfg_ch.cfg_ch20.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 19,0);
	
	
	if(cfg_para.cfg_ch.cfg_ch21.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 20,0);
	if(cfg_para.cfg_ch.cfg_ch22.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 21,0);
	if(cfg_para.cfg_ch.cfg_ch23.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 22,0);
	if(cfg_para.cfg_ch.cfg_ch24.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 23,0);
	if(cfg_para.cfg_ch.cfg_ch25.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 24,0);
	if(cfg_para.cfg_ch.cfg_ch26.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 25,0);
	if(cfg_para.cfg_ch.cfg_ch27.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 26,0);
	if(cfg_para.cfg_ch.cfg_ch28.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 27,0);
	if(cfg_para.cfg_ch.cfg_ch29.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 28,0);
	if(cfg_para.cfg_ch.cfg_ch30.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 29,0);
	
	if(cfg_para.cfg_ch.cfg_ch31.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 30,0);
	if(cfg_para.cfg_ch.cfg_ch32.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 31,0);
	if(cfg_para.cfg_ch.cfg_ch33.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 32,0);
	if(cfg_para.cfg_ch.cfg_ch34.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 33,0);
	if(cfg_para.cfg_ch.cfg_ch35.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 34,0);
	if(cfg_para.cfg_ch.cfg_ch36.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 35,0);
	if(cfg_para.cfg_ch.cfg_ch37.cfg_data.ch_data1 != 0)
		DROPDOWN_SetItemDisabled(hItem, 36,0);
}

static void changePic(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
//	char pro_num = 0;
	char pic_num = 0;
	char buf[16]={0};
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
	pro_num = DROPDOWN_GetSel(hItem);
	hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_1);
	pic_num = DROPDOWN_GetSel(hItem);
	memset(buf,0,sizeof(buf));
	memset(file_name,0,sizeof(file_name));
	strncpy(buf,cfgname[pro_num],strlen(cfgname[pro_num]));
	sprintf(file_name, "%s%02d", buf,pic_num);
//				ReadFileData(FS_VOLUME_SD,file_name,&cfg_para,sizeof(cfg_para));
	xTaskNotify(xHandleTaskMsgPro,BIT_18,eSetBits);
	change_ch = 1;
}

static void saveVCT(WM_MESSAGE * pMsg)
{	
	WM_HWIN hItem;
	float hl_temp;
  int   hlValTemp;
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);
	hl_temp = EDIT_GetFloatValue(hItem);
	hlValTemp = hl_temp*100;
//	if(hlValTemp != cfg_para.voltage)
		cfg_para.voltage = hlValTemp;
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_1);
	hl_temp = EDIT_GetFloatValue(hItem);
	hlValTemp = hl_temp*100;
//	if(hlValTemp != cfg_para.current)
		cfg_para.current = hlValTemp;	
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_2);
	hlValTemp = EDIT_GetFloatValue(hItem);
//	if(hlValTemp != cfg_para.totalTime)
		cfg_para.totalTime = hlValTemp;
}

static void saveZU(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	float hl_temp;
  int   hlValTemp;
	
//	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);
//	hl_temp = EDIT_GetFloatValue(hItem);
//	hlValTemp = hl_temp*100;
////	if(hlValTemp != cfg_para.voltage)
//		cfg_para.voltage = hlValTemp;
//	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_1);
//	hl_temp = EDIT_GetFloatValue(hItem);
//	hlValTemp = hl_temp*100;
////	if(hlValTemp != cfg_para.current)
//		cfg_para.current = hlValTemp;
	saveVCT(pMsg);
			
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_3);
	hl_temp = EDIT_GetFloatValue(hItem);
	hlValTemp = (hl_temp+31)*100;
//	if(hlValTemp != cfg_para.cfg_ch_zu.ch_zu1_h)
		cfg_para.cfg_ch_zu.ch_zu1_h = hlValTemp;
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_4);
	hl_temp = EDIT_GetFloatValue(hItem);
	hlValTemp = (hl_temp+31)*100;
//	if(hlValTemp != cfg_para.cfg_ch_zu.ch_zu1_l)
		cfg_para.cfg_ch_zu.ch_zu1_l = hlValTemp;
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_5);
	hl_temp = EDIT_GetFloatValue(hItem);
	hlValTemp = (hl_temp+31)*100;
//	if(hlValTemp != cfg_para.cfg_ch_zu.ch_zu2_h)
		cfg_para.cfg_ch_zu.ch_zu2_h = hlValTemp;
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_6);
	hl_temp = EDIT_GetFloatValue(hItem);
	hlValTemp = (hl_temp+31)*100;
//	if(hlValTemp != cfg_para.cfg_ch_zu.ch_zu2_l)
		cfg_para.cfg_ch_zu.ch_zu2_l = hlValTemp;
			
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_7);
	hl_temp = EDIT_GetFloatValue(hItem);
	hlValTemp = (hl_temp+31)*100;
//	if(hlValTemp != cfg_para.cfg_ch_zu.ch_zu3_h)
		cfg_para.cfg_ch_zu.ch_zu3_h = hlValTemp;
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_8);
	hl_temp = EDIT_GetFloatValue(hItem);
	hlValTemp = (hl_temp+31)*100;
//	if(hlValTemp != cfg_para.cfg_ch_zu.ch_zu3_l)
		cfg_para.cfg_ch_zu.ch_zu3_l = hlValTemp;
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_9);
	hl_temp = EDIT_GetFloatValue(hItem);
	hlValTemp = (hl_temp+31)*100;
//	if(hlValTemp != cfg_para.cfg_ch_zu.ch_zu4_h)
		cfg_para.cfg_ch_zu.ch_zu4_h = hlValTemp;
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_10);
	hl_temp = EDIT_GetFloatValue(hItem);
	hlValTemp = (hl_temp+31)*100;
//	if(hlValTemp != cfg_para.cfg_ch_zu.ch_zu4_l)
		cfg_para.cfg_ch_zu.ch_zu4_l = hlValTemp;
			
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_11);
	hl_temp = EDIT_GetFloatValue(hItem);
	hlValTemp = (hl_temp+31)*100;
//	if(hlValTemp != cfg_para.cfg_ch_zu.ch_zu5_h)
		cfg_para.cfg_ch_zu.ch_zu5_h = hlValTemp;
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_12);
	hl_temp = EDIT_GetFloatValue(hItem);
	hlValTemp = (hl_temp+31)*100;
//	if(hlValTemp != cfg_para.cfg_ch_zu.ch_zu5_l)
		cfg_para.cfg_ch_zu.ch_zu5_l = hlValTemp;
	
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_13);
	hl_temp = EDIT_GetFloatValue(hItem);
	hlValTemp = (hl_temp+31)*100;
//	if(hlValTemp != cfg_para.cfg_ch_zu.ch_zu6_h)
		cfg_para.cfg_ch_zu.ch_zu6_h = hlValTemp;
	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_14);
	hl_temp = EDIT_GetFloatValue(hItem);
	hlValTemp = (hl_temp+31)*100;
//	if(hlValTemp != cfg_para.cfg_ch_zu.ch_zu6_l)
		cfg_para.cfg_ch_zu.ch_zu6_l = hlValTemp;
	
//	hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_2);
//	hlValTemp = EDIT_GetFloatValue(hItem);
////	if(hlValTemp != cfg_para.totalTime)
//		cfg_para.totalTime = hlValTemp;
}

static void saveChData(int data,char num)
{
	switch(num) 
	{
		case 0:
			cfg_data_tmp.ch_data1 = data;
			break;
		case 1:
			cfg_data_tmp.ch_data2 = data;
			break;
		case 2:
			cfg_data_tmp.ch_data3 = data;
			break;
		case 3:
			cfg_data_tmp.ch_data4 = data;
			break;
		case 4:
			cfg_data_tmp.ch_data5 = data;
			break;
		case 5:
			cfg_data_tmp.ch_data6 = data;
			break;
		case 6:
			cfg_data_tmp.ch_data7 = data;
			break;
		case 7:
			cfg_data_tmp.ch_data8 = data;
			break;
		
		case 8:
			cfg_data_tmp.ch_data9 = data;
			break;
		case 9:
			cfg_data_tmp.ch_data10 = data;
			break;
		case 10:
			cfg_data_tmp.ch_data11 = data;
			break;
		case 11:
			cfg_data_tmp.ch_data12 = data;
			break;
		case 12:
			cfg_data_tmp.ch_data13 = data;
			break;
		case 13:
			cfg_data_tmp.ch_data14 = data;
			break;
		case 14:
			cfg_data_tmp.ch_data15 = data;
			break;
		case 15:
			cfg_data_tmp.ch_data16 = data;
			break;
	}
}

static void zuheSetV(char type,short level,short hl_time,char num,char flag)
{
	int temp=0;
	if(flag==0)
		temp = type<<31 | level<<30 | hl_time;
	else
		temp = type<<31 | hl_time<<15 | level;
	saveChData(temp,num);
}

static void zuheLoop(char type,char loopnum,char lp_addr,char rep_times,char num)
{
	int temp=0;
	temp = type<<31 | (loopnum-1)<<20 | lp_addr<<16 | rep_times;
	saveChData(temp,num);
}

static void saveData(WM_HWIN hItem)
{
	char rowNum=0,i;
	char buf[10];
	char ch_flag;
	
	char type = 0;
	char level = 0;
	short hl_time =0,lev_vol =0;
	float voltage =0;
	char loopNum = 0;
	char rep_times =0;
	char lp_addr = 0;
	
	loopNum = 0;
	memset(&cfg_data_tmp,0,sizeof(cfg_data_tmp));
	
	rowNum = LISTVIEW_GetNumRows(hItem);
	for(i=0;i<rowNum;i++)
	{
		LISTVIEW_GetItemText(hItem,1,i,buf,10);
		if(strcmp(buf,"SetV")==0)
		{
			if(channel_num<24)
			{
				type = 0;
				LISTVIEW_GetItemText(hItem,2,i,buf,10);
				level = atoi(buf);
				LISTVIEW_GetItemText(hItem,3,i,buf,10);
				hl_time = atoi(buf);
				ch_flag = 0;
				zuheSetV(type,level,hl_time,i,ch_flag);
			}else{
				type = 0;
				LISTVIEW_GetItemText(hItem,2,i,buf,10);
				voltage = atof(buf);
				LISTVIEW_GetItemText(hItem,3,i,buf,10);
				hl_time = atoi(buf);
				ch_flag = 1;
				lev_vol = (short)((voltage+31.0)*100.0);
				zuheSetV(type,lev_vol,hl_time,i,ch_flag);
			}
		}
		else
		{
			type = 1;
			LISTVIEW_GetItemText(hItem,4,i,buf,10);
			lp_addr = atoi(buf);
			LISTVIEW_GetItemText(hItem,5,i,buf,10);
			rep_times = atoi(buf);
			loopNum++;
			zuheLoop(type,loopNum,lp_addr,rep_times,i);
		}
	}
	cfg_ch_tmp.cfg_data = cfg_data_tmp;
	chSave(channel_num);
}

static void delBlank(char *buf)
{
	char i,j=0;
	for(i=0;buf[i] != '\0';i++)
	{
		if(buf[i] != ' ')
			buf[j++] = buf[i];
	}
	buf[j] = '\0';
}

static void _cbList(WM_MESSAGE * pMsg) {
  int NCode;

  switch (pMsg->MsgId) {
  case WM_NOTIFY_PARENT:
    NCode = pMsg->Data.v;
    if (NCode == WM_NOTIFICATION_CHILD_DELETED) {
      WM_SendMessage(WM_GetParent(pMsg->hWin), pMsg);
      break;
    }
    LISTVIEW_Callback(pMsg);
    break;
  default:
    LISTVIEW_Callback(pMsg);
  }
}

static void _cbCellEdit(WM_MESSAGE * pMsg) {
  CELL_EDIT_ORG   CellEditOrg;
  WM_KEY_INFO   * pKeyInfo;
  WM_HWIN         hList;
  char            acText[LEN_ITEM_TEXT];
//  char i,pos=0;
//  char mylen=0;
//  int     NCode;
  WM_MESSAGE Message;
//  char posflag = 0;
//  char position = 0;

  switch (pMsg->MsgId) {
  case WM_KEY:
    pKeyInfo = (WM_KEY_INFO *)pMsg->Data.p;
    if (pKeyInfo->PressedCnt == 0) {
      if (pKeyInfo->Key == GUI_KEY_ENTER) {
        WM_SetFocus(WM_GetParent(pMsg->hWin));
        WM_DeleteWindow(pMsg->hWin);
        break;
      }
    }
    EDIT_Callback(pMsg);
    if (pKeyInfo->Key == GUI_KEY_LEFT || pKeyInfo->Key == GUI_KEY_RIGHT) 
		{
	//		WM_SendMessageNoPara(WM_GetClientWindow(pMsg->hWin),WM_LIMIT);
			//WM_SendMessageNoPara(pMsg->hWin,WM_LIMIT);
			Message.MsgId = WM_LIMIT;
			Message.Data.v = 0;
		}
			if (pKeyInfo->Key == GUI_KEY_UP || pKeyInfo->Key == GUI_KEY_DOWN) 
		{
			Message.MsgId = WM_LIMIT;
			Message.Data.v = 1;
		}
		WM_SendMessage(pMsg->hWin, &Message);
    break;
	case WM_LIMIT:
		EDIT_GetText(pMsg->hWin, acText, LEN_ITEM_TEXT);
		if(setflag)
		{
			EDIT_GetUserData(pMsg->hWin, &CellEditOrg, sizeof(CELL_EDIT_ORG));
			hList = WM_GetParent(pMsg->hWin);
			if(strncmp(acText,"SetV",4)==0)
			{
				EDIT_SetText(pMsg->hWin,"Loop");
				LISTVIEW_SetItemText(hList,CellEditOrg.Col+1,CellEditOrg.Row,"");
				LISTVIEW_SetItemText(hList,CellEditOrg.Col+2,CellEditOrg.Row,"");
				LISTVIEW_SetItemText(hList,CellEditOrg.Col+3,CellEditOrg.Row,"0");
				LISTVIEW_SetItemText(hList,CellEditOrg.Col+4,CellEditOrg.Row,"0");
			}
			else{
				EDIT_SetText(pMsg->hWin,"SetV");
				LISTVIEW_SetItemText(hList,CellEditOrg.Col+1,CellEditOrg.Row,"0.00");
				LISTVIEW_SetItemText(hList,CellEditOrg.Col+2,CellEditOrg.Row,"0");
				LISTVIEW_SetItemText(hList,CellEditOrg.Col+3,CellEditOrg.Row,"");
				LISTVIEW_SetItemText(hList,CellEditOrg.Col+4,CellEditOrg.Row,"");
			}
		}
		else{
//			mylen = strlen(acText);
//			NCode = pMsg->Data.v;
//			if(NCode)
//			{
//				EDIT_GetUserData(pMsg->hWin, &CellEditOrg, sizeof(CELL_EDIT_ORG));
//				if(CellEditOrg.Col == 2)
//				{
//					posflag = 1;
//					for(position=0;position<mylen & acText[position]!='.';position++);
//				}
//				for(i=0;i<mylen;i++)
//				{
//					if(acText[i]<2)
//						acText[i] = 0;
//					if((acText[0]==44)||(acText[0]==46))
//					{
//						acText[0] = 45;
//					}
//					if(posflag && ((acText[i]==45)||(acText[i]==47)))
//					{
//						if(position<mylen)
//						{
//							if(position==i)
//								acText[i] = 46;
//						}
//						else
//							acText[i] = 46;
//					}
//					if((acText[i]==45)||(acText[i]==46))
//						continue;
//					if((acText[i]<48)&&(acText[i]>10))
//						acText[i] = 48;
//					if(acText[i]>57)
//						acText[i] = 57;
//				}
//				posflag = 0;
//			}
//			pos=EDIT_GetCursorCharPos(pMsg->hWin);
//			EDIT_SetText(pMsg->hWin, acText);
//			EDIT_SetSel(pMsg->hWin,pos,pos);
		}
		break;
  case WM_SET_FOCUS:
    if (pMsg->Data.v == 0) {
      WM_DeleteWindow(pMsg->hWin);
    } else {
      EDIT_Callback(pMsg);
    }
    break;
  case WM_DELETE:
    EDIT_GetUserData(pMsg->hWin, &CellEditOrg, sizeof(CELL_EDIT_ORG));
    hList = WM_GetParent(pMsg->hWin);
    EDIT_GetText(pMsg->hWin, acText, LEN_ITEM_TEXT);
		delBlank(acText);
    LISTVIEW_SetItemTextSorted(hList, CellEditOrg.Col, CellEditOrg.Row, acText);
		setflag = 0;
    break;
  default:
    EDIT_Callback(pMsg);
  }
}

/*
*********************************************************************************************************
*	                       GUI_WIDGET_CREATE_INFOÀàÐÍÊý×é
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate5[] = 
{
	{ FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 800, 480, 0, 0x64, 0 },
	{ TEXT_CreateIndirect, "Text", ID_TEXT_0, 480, 170, 80, 28, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_1, 480, 210, 140, 28, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_2, 480, 250, 140, 28, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_3, 480, 290, 140, 28, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_4, 480, 330, 140, 28, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_5, 480, 370, 140, 28, 0, 0x64, 0 },
	
	{ DROPDOWN_CreateIndirect, "Dropdown", ID_DROPDOWN_0, 630, 170, 130, 29, 0, 0x0, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_0, 630, 210, 80, 28, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_1, 630, 250, 80, 28, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_2, 630, 290, 80, 28, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_6, 720, 210, 40, 28, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_7, 720, 250, 40, 28, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_8, 720, 290, 40, 28, 0, 0x64, 0 },
  { DROPDOWN_CreateIndirect, "Dropdown", ID_DROPDOWN_1, 630, 330, 130, 28, 0, 0x0, 0 },
  { DROPDOWN_CreateIndirect, "Dropdown", ID_DROPDOWN_2, 630, 370, 130, 28, 0, 0x0, 0 },	
	{ BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 480, 410, 100, 28, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "Button", ID_BUTTON_1, 630, 410, 130, 28, 0, 0x0, 0 },
	
  { TEXT_CreateIndirect, "Text", ID_TEXT_9, 360, 8, 80, 28, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_10, 48, 35, 100, 28, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_11, 168, 35, 100, 28, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_12, 280, 35, 100, 28, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_13, 400, 35, 110, 28, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_14, 520, 35, 110, 28, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_15, 640, 35, 110, 28, 0, 0x64, 0 },

  { TEXT_CreateIndirect, "Text", ID_TEXT_16, 20, 60, 30, 28, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_17, 20, 100, 30, 28, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_18, 760, 60, 30, 28, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_19, 760, 100, 30, 28, 0, 0x64, 0 },
	
  { EDIT_CreateIndirect, "Edit", ID_EDIT_3, 50, 60, 75, 28, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_4, 50, 100, 75, 28, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_5, 170, 60, 75, 28, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_6, 170, 100, 75, 28, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_7, 290, 60, 75, 28, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_8, 290, 100, 75, 28, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_9, 410, 60, 80, 28, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_10, 410, 100, 80, 28, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_11, 530, 60, 80, 28, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_12, 530, 100, 80, 28, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_13, 650, 60, 80, 28, 0, 0x64, 0 },
  { EDIT_CreateIndirect, "Edit", ID_EDIT_14, 650, 100, 80, 28, 0, 0x64, 0 },
	
  { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0, 10, 160, 430, 305, 0, 0x0, 0 },
	
};

void InitDialog(WM_MESSAGE * pMsg)
{	
	WM_HWIN hItem;
	
	//
	// Initialization Framewin
	//
	hItem = pMsg->hWin;
	FRAMEWIN_SetTitleVis(hItem, 0);
	FRAMEWIN_SetClientColor(hItem,0x00D8E9EC);
//		WINDOW_SetBkColor(hItem, 0x00D8E9EC);
	//
	// Initialization of 'Text'
	//
	{
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, &GUI_Fontkai24);
		TEXT_SetText(hItem, "工程名:");	
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, &GUI_Fontkai24);
		TEXT_SetText(hItem, "设置电压:");		
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, &GUI_Fontkai24);
		TEXT_SetText(hItem, "设置电流:");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, &GUI_Fontkai24);
		TEXT_SetText(hItem, "设置总时间:");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, &GUI_Fontkai24);
		TEXT_SetText(hItem, "设置图片:");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, &GUI_Fontkai24);
		TEXT_SetText(hItem, "设置通道:");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, GUI_FONT_24_1);
		TEXT_SetText(hItem, "V");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_7);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, GUI_FONT_24_1);
		TEXT_SetText(hItem, "mA");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, GUI_FONT_24_1);
		TEXT_SetText(hItem, "us");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_9);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, &GUI_Fontkai24);
		TEXT_SetText(hItem, "时钟组");

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_10);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, GUI_FONT_20_1);
		TEXT_SetText(hItem, "CH0~CH3");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_11);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, GUI_FONT_20_1);
		TEXT_SetText(hItem, "CH4~CH7");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_12);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, GUI_FONT_20_1);
		TEXT_SetText(hItem, "CH8~CH11");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_13);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, GUI_FONT_20_1);
		TEXT_SetText(hItem, "CH12~CH15");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_14);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, GUI_FONT_20_1);
		TEXT_SetText(hItem, "CH16~CH19");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_15);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, GUI_FONT_20_1);
		TEXT_SetText(hItem, "CH20~CH23");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_16);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, GUI_FONT_24_1);
		TEXT_SetText(hItem, "H");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_17);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, GUI_FONT_24_1);
		TEXT_SetText(hItem, "L");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_18);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, GUI_FONT_24_1);
		TEXT_SetText(hItem, "V");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_19);
		TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		TEXT_SetFont(hItem, GUI_FONT_24_1);
		TEXT_SetText(hItem, "V");
	}
	//
	// Initialization of 'Dropdown'
	//
	{	
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
		DROPDOWN_SetFont(hItem, GUI_FONT_24_1);
		DROPDOWN_SetAutoScroll(hItem, 1);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_1);
		DROPDOWN_SetFont(hItem, GUI_FONT_24_1);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_SetListHeight(hItem, 24*5);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_2);
		DROPDOWN_SetFont(hItem, GUI_FONT_24_1);
		DROPDOWN_SetAutoScroll(hItem, 1);
		DROPDOWN_SetListHeight(hItem, 24*3);
	}	
	//
	// Initialization of 'Edit'
	//
	{	
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_0);
		EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
		EDIT_SetFont(hItem, GUI_FONT_24_1);	
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_1);
		EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
		EDIT_SetFont(hItem, GUI_FONT_24_1);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_2);
		EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
		EDIT_SetFont(hItem, GUI_FONT_24_1);
	
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_3);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetFont(hItem, GUI_FONT_24_1);	
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_4);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetFont(hItem, GUI_FONT_24_1);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_5);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetFont(hItem, GUI_FONT_24_1);	
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_6);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetFont(hItem, GUI_FONT_24_1);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_7);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetFont(hItem, GUI_FONT_24_1);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_8);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetFont(hItem, GUI_FONT_24_1);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_9);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetFont(hItem, GUI_FONT_24_1);	
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_10);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetFont(hItem, GUI_FONT_24_1);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_11);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetFont(hItem, GUI_FONT_24_1);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_12);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetFont(hItem, GUI_FONT_24_1);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_13);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetFont(hItem, GUI_FONT_24_1);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_14);
		EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetFont(hItem, GUI_FONT_24_1);
	}
	//
	// Initialization of 'Listview'
	//
	{
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
		LISTVIEW_SetFont(hItem, GUI_FONT_20_1);
		WM_SetCallback(hItem, _cbList);
		LISTVIEW_SetGridVis(hItem, 1);
		LISTVIEW_SetRowHeight(hItem, 30);
		HEADER_SetFont(LISTVIEW_GetHeader(hItem), &GUI_Fontkai20);
		LISTVIEW_SetHeaderHeight(hItem, 35);
		LISTVIEW_EnableCellSelect(hItem, 1);
		LISTVIEW_SetAutoScrollV(hItem, 1);
		LISTVIEW_AddColumn(hItem, 45, "地址", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 55, "命令", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 70, "电压(V)", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 85, "时间(us)", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 85, "重复地址", GUI_TA_HCENTER | GUI_TA_VCENTER);
		LISTVIEW_AddColumn(hItem, 90, "重复次数", GUI_TA_HCENTER | GUI_TA_VCENTER);
	}	
	//
	// 按钮控件设置
	//
	{
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
		BUTTON_SetFont(hItem, &GUI_Fontkai24);
		BUTTON_SetText(hItem, "保存");
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
		BUTTON_SetFont(hItem, &GUI_Fontkai24);
		BUTTON_SetText(hItem, "返回主页面");
	}
}


static void _cbDialog5(WM_MESSAGE * pMsg) 
{
	WM_HWIN hItem;
  WM_KEY_INFO    * pKeyInfo;
	int     NCode;
	int     Id;
  static WM_HWIN   hCellEdit = 0;
  CELL_EDIT_ORG    CellEditOrg;
  GUI_RECT         ItemRect;
  char             acItemText[LEN_ITEM_TEXT],acCommand[LEN_ITEM_TEXT];
	char i,len=0;
	char buf[16]={0},buf1[16];
//	char pro_num = 0;
	char pic_num = 0;
  int  lis_tmp;
	char row_num = 0;

	switch (pMsg->MsgId)
	{
		case WM_INIT_DIALOG:
			InitDialog(pMsg);
		//
		// Initialization of 'Dropdown'
		//
		{				
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
			if(cfg_flag)
			{
//				cfg_flag = 0;
				memset(file_name,0,sizeof(file_name));
				sprintf(file_name, "proname");
				ReadFileData(FS_VOLUME_SD,file_name,&cfgname,sizeof(cfgname));
			}
			for(i=0;cfgname[i][0] != '\0';i++)
			{
				sprintf(buf, "%d%s", i,":");
				strcpy(buf1,cfgname[i]);
				strcat(buf,buf1);
				DROPDOWN_AddString(hItem, buf);
			}
			if(i>6)
				DROPDOWN_SetListHeight(hItem, 24*6);
			else
				DROPDOWN_SetListHeight(hItem, 24*i);
			
			sprintf(file_name, "%s00", cfgname[0]);
			ReadFileData(FS_VOLUME_SD,file_name,&cfg_para,sizeof(cfg_para));
			
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_1);
			len = 1;
			for(i=0;i<len;i++)
			{
				memset(file_name,0,sizeof(file_name));
				sprintf(file_name,"%s%02d",cfgname[pro_num],i);
				ReadFileData(FS_VOLUME_SD,file_name,&picArray,sizeof(CONFIG_PIC));
				memset(buf1,0,sizeof(buf1));
				sprintf(buf1, "%d:%s", picArray.picNum,picArray.picName);
				DROPDOWN_AddString(hItem, buf1);
				len = picArray.picMaxN;
			}
			memset(file_name,0,sizeof(file_name));
			sprintf(file_name,"%s00",cfgname[0]);
			ReadFileData(FS_VOLUME_SD,file_name,&picArray,sizeof(CONFIG_PIC));
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_2);
			for(i=0;i<37;i++)
			{
				memset(buf,0,sizeof(buf));
				getChName(i,buf);
	//			sprintf(buf, "%s%d:%s", "CH",i,buf1);
				DROPDOWN_AddString(hItem, buf);
				DROPDOWN_SetItemDisabled(hItem, i,1);
			}
			change_ch = 1;
		}
			//
			// Initialization of 'Text'
			//
		{
		}		
		//
		// Initialization of 'Edit'
		//
	{		
		refreshBlSet(pMsg);
		refreshZU(pMsg);  
	}
	//
	// Initialization of 'Listview'
	//
	{
		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
		addRow(hItem);
	}	
		
		case WM_PAINT:
		{
			GUI_SetColor(0x00FFFFFF);//RGB(172,168,153)
			GUI_DrawRect(10,20,780,140);
			GUI_DrawRect(460,160,780,460);
			GUI_SetColor(0x00ACA899);//RGB(172,168,153)
			GUI_DrawRect(9,19,779,139);
			GUI_DrawRect(459,159,779,459);

			GUI_SetColor(0x00D8E9EC);//RGB(236,233,216)
			GUI_DrawHLine(20, 355, 435);
			GUI_DrawHLine(19, 354, 434);

			GUI_SetColor(GUI_BLACK);
		}
			break;
		
//	case MSG_DeleteInfo:
//		if(WM_IsWindow(hWinSet))
//		{
//			DeleteFramewin(hWinSet);
//			hWinMain = CreateWindow1();
//		}
//		break;
  case WM_KEY:
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
    if (pMsg->hWinSrc == hItem) {
      pKeyInfo = (WM_KEY_INFO *)pMsg->Data.p;
			
			if(WM_HasFocus(hItem) && ((pKeyInfo->Key == GUI_KEY_TAB) || (pKeyInfo->Key == GUI_KEY_BACKTAB))){
				lis_tmp=LISTVIEW_GetSel(hItem);
				if(lis_tmp>0){
					LISTVIEW_SetSel(hItem,LISTVIEW_GetSel(hItem));
					LISTVIEW_SetSelCol(hItem,LISTVIEW_GetSelCol(hItem));
				}else{
					LISTVIEW_SetSel(hItem,0);
					LISTVIEW_SetSelCol(hItem,2);
				}
			}
      if (pKeyInfo->PressedCnt == 0) {
        if (pKeyInfo->Key == GUI_KEY_ENTER) {
          if (hCellEdit == 0) {
            CellEditOrg.Row = LISTVIEW_GetSel(hItem);
            CellEditOrg.Col = LISTVIEW_GetSelCol(hItem);
						
						LISTVIEW_GetItemText(hItem,1,CellEditOrg.Row,acCommand,LEN_ITEM_TEXT);
						if(CellEditOrg.Col == 0 )
							break;
						if(CellEditOrg.Col == 1)
						{
							if(CellEditOrg.Row == 0)
								break;
							else
								setflag = 1;
						}
						if(!strcmp("SetV",acCommand)){
							if(CellEditOrg.Col == 4 || CellEditOrg.Col == 5)
								break;
						}else{
							if(CellEditOrg.Col == 2 || CellEditOrg.Col == 3)
								break;
						}
						
            LISTVIEW_GetItemRect(hItem, CellEditOrg.Col, CellEditOrg.Row, &ItemRect);
            LISTVIEW_GetItemTextSorted(hItem, CellEditOrg.Col, CellEditOrg.Row, acItemText, LEN_ITEM_TEXT);
            hCellEdit = EDIT_CreateUser(ItemRect.x0, ItemRect.y0, ItemRect.x1 - ItemRect.x0 + 2, ItemRect.y1 - ItemRect.y0 + 1, hItem, WM_CF_SHOW, 0, 0, LEN_ITEM_TEXT, sizeof(CELL_EDIT_ORG));
            EDIT_SetUserData(hCellEdit, &CellEditOrg, sizeof(CELL_EDIT_ORG));
            WIDGET_SetEffect(hCellEdit, &WIDGET_Effect_Simple);
						EDIT_SetTextAlign(hCellEdit, GUI_TA_HCENTER | GUI_TA_VCENTER);
						EDIT_SetFont(hCellEdit, GUI_FONT_20_1);	
            WM_SetCallback(hCellEdit, _cbCellEdit);
            WM_SetFocus(hCellEdit);
						if(CellEditOrg.Col==1)
							EDIT_SetText(hCellEdit, acItemText);
						else if(CellEditOrg.Col==2){
							if(channel_num<24)
								EDIT_SetFloatMode(hCellEdit,atoi(acItemText),0,1,0,2);
							else
								EDIT_SetFloatMode(hCellEdit,atof(acItemText),-31,31,2,2);
						}
						else
							EDIT_SetFloatMode(hCellEdit,atoi(acItemText),0,60000,0,2);
          }
        }
      }
    }
    break;
			
	case WM_TIMER:
		if(WM_GetTimerId(pMsg->Data.v) == ID_Timer2)
		{
			if(pageflag)
			{
				pageflag = 0;
				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
//				DROPDOWN_SetSel(hItem,cfgname[0][15]);
				DROPDOWN_SetSel(hItem,currentPro);
				changePic(pMsg);
			}
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_1);
			switch(change_pic)
			{
				case 1:
					row_num = DROPDOWN_GetNumItems(hItem);
					for(i=row_num;i>0;i--)
						DROPDOWN_DeleteItem(hItem,i-1);
					change_pic = 2;
					break;
				case 2:
					pro_num = DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0));
					len = 1;
					memset(file_name,0,sizeof(file_name));
					for(i=0;i<len;i++)
					{
						sprintf(file_name,"%s%02d",cfgname[pro_num],i);
						ReadFileData(FS_VOLUME_SD,file_name,&picArray,sizeof(CONFIG_PIC));
						memset(buf1,0,sizeof(buf1));
						sprintf(buf1, "%d:%s", picArray.picNum,picArray.picName);
//								DROPDOWN_AddString(hItem, buf);
						DROPDOWN_InsertString(hItem, buf1,picArray.picNum);
						len = picArray.picMaxN;
					}
					DROPDOWN_DeleteItem(hItem,picArray.picNum+1);
					DROPDOWN_SetSel(hItem,0);
					change_pic = 3;
					memset(file_name,0,sizeof(file_name));
					sprintf(file_name,"%s00",cfgname[pro_num]);
					ReadFileData(FS_VOLUME_SD,file_name,&picArray,sizeof(CONFIG_PIC));
					break;
				case 3:
					change_pic = 0;
					changePic(pMsg);
					refreshBlSet(pMsg);
					refreshZU(pMsg);
					hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_2);
					channel_num = DROPDOWN_GetSel(hItem);
					break;
			}
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_2);			
			switch(change_ch)
			{
				case 1:
					row_num = DROPDOWN_GetNumItems(hItem);
					for(i=row_num;i>0;i--)
						DROPDOWN_DeleteItem(hItem,i-1);
					change_ch = 2;
					break;
				case 2:
					change_ch = 3;
					for(i=0;i<37;i++)
					{
						memset(buf,0,sizeof(buf));
						getChName(i,buf); 
//						DROPDOWN_AddString(hItem, buf);
						DROPDOWN_InsertString(hItem, buf,i);
						DROPDOWN_SetItemDisabled(hItem, i,1);
					}
					break;
				case 3:
					change_ch = 0;
					cmpChNum(hItem);
					channel_num = DROPDOWN_GetSel(hItem);
					change_data = 1;
					refreshBlSet(pMsg);
					refreshZU(pMsg);
					break;
			}
			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
			switch(change_data)
			{
				case 1:
					row_num = LISTVIEW_GetNumRows(hItem);
					for(i=row_num;i>0;i--)
						LISTVIEW_DeleteRow(hItem,i-1);
					change_data = 2;
					break;
				case 2:
					addRow(hItem);
					change_data = 0;
					break;
			}
			
			if(WM_IsWindow(hWinInfoSave) && delFlag)
			{
				delFlag = 0;
				GUI_Delay(300);
				DeleteFramewin3();
				WM_SetFocus(hWinSet);
			}
		}
		/* 重启定时器 */
		WM_RestartTimer(pMsg->Data.v, 400);
		break;
		
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (NCode) {
			case WM_NOTIFICATION_CHILD_DELETED:
				if (pMsg->hWinSrc == hCellEdit) {
					hCellEdit = 0;
				}
				break;
			}
		
			switch(Id) 
			{
		case ID_DROPDOWN_0: // Notifications sent by 'Dropdown'
			switch (NCode) {
			case WM_NOTIFICATION_CLICKED:
				break;
			case WM_NOTIFICATION_RELEASED:
//				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
//				WM_SetFocus(hItem);
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
				pro_num = DROPDOWN_GetSel(hItem);
				xTaskNotify(xHandleTaskMsgPro,BIT_16 | pro_num,eSetValueWithOverwrite);
				GUI_Delay(2);
//				picture_num=0;
				change_pic = 1;
				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_1);
				DROPDOWN_InsertString(hItem, "",0);
//				Message.MsgId = ID_DROPDOWN_1;
//				Message.Data.v = WM_NOTIFICATION_SEL_CHANGED;
//				WM_SendMessage(pMsg->hWinSrc, &Message);
				change_data = 1;
				break;
			}
			break;
		case ID_DROPDOWN_1: // Notifications sent by 'Dropdown'
			switch (NCode) {
			case WM_NOTIFICATION_CLICKED:
				break;
			case WM_NOTIFICATION_RELEASED:
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				changePic(pMsg);
				break;
			}
			break;
		case ID_DROPDOWN_2: // Notifications sent by 'Dropdown'
			switch (NCode) {
			case WM_NOTIFICATION_CLICKED:
				break;
			case WM_NOTIFICATION_RELEASED:
				break;
			case WM_NOTIFICATION_SEL_CHANGED:
				hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_2);
				channel_num = DROPDOWN_GetSel(hItem);
//				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
//				rows = LISTVIEW_GetNumRows(hItem);				
//				for(i=rows;i>0;i--)
//					LISTVIEW_DeleteRow(hItem,i-1);				
//				addRow(hItem);
				change_data = 1;			
				break;
			}
			break;
				case ID_BUTTON_0: 
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							if(!WM_IsWindow(hWinInfo))
							{
								CreateFramewin3();
							}
							hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
							pro_num = DROPDOWN_GetSel(hItem);
							hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_1);
							pic_num = DROPDOWN_GetSel(hItem);
							memset(file_name,0,sizeof(file_name));						
							sprintf(file_name, "%s%02d",cfgname[pro_num], pic_num);
							xTaskNotify(xHandleTaskMsgPro,BIT_18,eSetBits);
							GUI_Delay(10);
							saveZU(pMsg);
							hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
							cfg_allch_tmp = cfg_para.cfg_ch;
							saveData(hItem);
							cfg_para.cfg_ch = cfg_allch_tmp;
				//			CreateNewFile(project_number,picture_num);
							xTaskNotify(xHandleTaskMsgPro,BIT_16 | pro_num,eSetValueWithOverwrite);
							GUI_Delay(10);
							xTaskNotify(xHandleTaskMsgPro,BIT_17 | pic_num,eSetValueWithOverwrite);
							GUI_Delay(10);
							xTaskNotify(xHandleTaskMsgPro,BIT_20,eSetBits);
							GUI_Delay(10);
							xTaskNotify(xHandleTaskMsgPro,BIT_24,eSetBits);
							GUI_Delay(10);
							delFlag = 1;
							break;
					}
					break;
				case ID_BUTTON_1: 
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
//							GUI_EndDialog(pMsg->hWin, 0);
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_1));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_2));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_3));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_4));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_5));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_8));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_9));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_10));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_11));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_12));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_13));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_14));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_15));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_16));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_17));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_18));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_19));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_EDIT_0));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_EDIT_1));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_EDIT_2));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_EDIT_3));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_EDIT_4));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_EDIT_5));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_EDIT_6));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_EDIT_7));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_EDIT_8));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_EDIT_9));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_EDIT_10));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_EDIT_11));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_EDIT_12));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_EDIT_13));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_EDIT_14));
//				DeleteFramewin(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0));
//				DeleteFramewin(pMsg->hWin);
//				pMsg->hWin = 0;
//							CreateFramewin3();
//							DeleteFramewin(hWinSet);
//							hWinMain=CreateWindow1();
//							WM_Paint(hWinMain);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
			WM_SetFocus(hItem);	
							WM_HideWindow(hWinSet);
							WM_ShowWindow(hWinMain);
			WM_SetFocus(hWinMain);
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

void CreateFramewin5(void) 
{
//	WM_HWIN hWin;

	hWinSet = GUI_CreateDialogBox(_aDialogCreate5, GUI_COUNTOF(_aDialogCreate5), _cbDialog5, WM_HBKWIN, 0, 0);
	WM_CreateTimer(WM_GetClientWindow(hWinSet),ID_Timer2,10,0);
//	return hWin;
}

/***************************************************************************/
 
int getChData(char dataNum)
{	
	int datatmp=0;
	switch(dataNum)
	{
		case 0:
			datatmp = cfg_data_tmp.ch_data1;
		break;
		case 1:
			datatmp = cfg_data_tmp.ch_data2;
		break;
		case 2:
			datatmp = cfg_data_tmp.ch_data3;
		break;
		case 3:
			datatmp = cfg_data_tmp.ch_data4;
		break;
		case 4:
			datatmp = cfg_data_tmp.ch_data5;
		break;
		case 5:
			datatmp = cfg_data_tmp.ch_data6;
		break;
		case 6:
			datatmp = cfg_data_tmp.ch_data7;
		break;
		case 7:
			datatmp = cfg_data_tmp.ch_data8;
		break;
		
		case 8:
			datatmp = cfg_data_tmp.ch_data9;
		break;
		case 9:
			datatmp = cfg_data_tmp.ch_data10;
		break;
		case 10:
			datatmp = cfg_data_tmp.ch_data11;
		break;
		case 11:
			datatmp = cfg_data_tmp.ch_data12;
		break;
		case 12:
			datatmp = cfg_data_tmp.ch_data13;
		break;
		case 13:
			datatmp = cfg_data_tmp.ch_data14;
		break;
		case 14:
			datatmp = cfg_data_tmp.ch_data15;
		break;
		case 15:
			datatmp = cfg_data_tmp.ch_data16;
		break;
	}
	return datatmp;
}



/*
*********************************************************************************************************
*	函 数 名: MainTask
*	功能说明: GUI主函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/

extern void task_create(void);
void MainTask(void) 
{
//	WM_HWIN hWinMain;
	
	/* 初始化 */
	GUI_Init();

	/* 使能UTF-8编码 */     
	GUI_UC_SetEncodeUTF8(); 
		
	GUI_SetBkColor(GUI_BLACK);
	GUI_SetColor(GUI_WHITE);	
	
	GUI_SetFont(&GUI_Fontyahei72);
	GUI_DispStringAt("CELL信号发生器", (LCD_GetXSize() - 5*72-54)/2 , (LCD_GetYSize() - 72)/2);

	GUI_SetFont(&GUI_Fontyahei30);
	GUI_DispStringAt("帆声图像", (LCD_GetXSize() - 4*30)/2+10 , LCD_GetYSize() - 30 - 25);
	GUI_SetFont(GUI_FONT_20_1);
	GUI_DispStringAt("@Freesense Image", (LCD_GetXSize() - 17*10)/ 2 , LCD_GetYSize() - 20);
	
	GUI_DrawBitmap(&bmLogoCell, (LCD_GetXSize() - bmLogoCell .XSize)/2 , LCD_GetYSize()/2 - 72 - 80);
	
	GUI_SetFont(&GUI_Fontyahei36);
	GUI_DispStringAt("正在初始化系统●●●", (LCD_GetXSize() - 8*36)/2 + 8, (LCD_GetYSize()/2 + 36 + 30));	
		
	ulTaskNotifyTake(pdTRUE,portMAX_DELAY); /* 无限等待 */
	GUI_Delay(800);
  if(File_Init())
	{
		GUI_SetColor(GUI_LIGHTBLUE);
		GUI_DispStringAt("正在解析配置文件●●●", (LCD_GetXSize() - 8*36)/2 + 8, (LCD_GetYSize()/2 + 36 + 30));	
		vTaskPrioritySet(NULL, 7);
		DemoFatFS();
		vTaskPrioritySet(NULL, 1);
//		cfg_flag = 1;
	}
	else
	{
		GUI_SetColor(GUI_RED);
		GUI_DispStringAt("没有找到U盘或SD卡！！！", (LCD_GetXSize() - 8*36)/2 , LCD_GetYSize()/2 + 36 + 30);
//		cfg_flag = 1;
		readPro(0);

		GUI_Delay(1000);
	}
 OP_JiaoZhun();
	 
	/*
	 关于多缓冲和窗口内存设备的设置说明
	   1. 使能多缓冲是调用的如下函数，用户要在LCDConf_Lin_Template.c文件中配置了多缓冲，调用此函数才有效：
		  WM_MULTIBUF_Enable(1);
	   2. 窗口使能使用内存设备是调用函数：WM_SetCreateFlags(WM_CF_MEMDEV);
	   3. 如果emWin的配置多缓冲和窗口内存设备都支持，二选一即可，且务必优先选择使用多缓冲，实际使用
		  STM32F429BIT6 + 32位SDRAM + RGB565/RGB888平台测试，多缓冲可以有效的降低窗口移动或者滑动时的撕裂
		  感，并有效的提高流畅性，通过使能窗口使用内存设备是做不到的。
	   4. 所有emWin例子默认是开启三缓冲。
	*/
	WM_MULTIBUF_Enable(1);
//	task_create();

	/*
       触摸校准函数默认是注释掉的，电阻屏需要校准，电容屏无需校准。如果用户需要校准电阻屏的话，执行
	   此函数即可，会将触摸校准参数保存到EEPROM里面，以后系统上电会自动从EEPROM里面加载。
	*/
    //TOUCH_Calibration();
	
	/* 将字库从SD卡中加载到SPI FLASH里面，加载一次即可，以后使用注释掉此函数 */
//	LoadFontLib();
	
//	/* 创建XBF字体 */
//    GUI_SetXBF();
	
	/* 调用此函数会自动的刷新桌面窗口 */
	WM_SetDesktopColor(GUI_BLUE);

	/* 创建对话框 */
//	hWinMain=GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbDialog, 0, 0, 0);

		CreateFramewin5();
		WM_HideWindow(hWinSet);
		CreateFramewin4();
	
//	WM_CreateTimer(WM_GetClientWindow(hWinMain), /* 接受信息的窗口的句柄 */
//				   ID_Timer1, 	         /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。ID_TimerGraph */
//				   10,                       /* 周期，此周期过后指定窗口应收到消息*/
//				   0);	                     /* 留待将来使用，应为0 */
	
	while(1) 
	{
		GUI_Delay(20);
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
