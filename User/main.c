/*
*********************************************************************************************************
*
*	ģ������ : ������ģ�顣
*	�ļ����� : main.c
*	��    �� : V1.0
*	˵    �� : ��ʵ����Ҫʵ��FreeRTOS+STemWin+FatFS+USB Host�ۺ�
*              ʵ��Ŀ�ģ�
*                1. ѧϰFreeRTOS+STemWin+FatFS+USB Host�ۺ� 
*                2. �����USB Host��Ҫʵ��U����ش���֧��U���Ȳ�Ρ�
*                   �û����Ը�����Ҫ��usb_usr.c�ļ��еĲ����⺯����
*                   USBH_USR_Configuration_DescAvailable���ߺ���USBH_USR_Init������������־
*                   �γ���⺯����
*                   USBH_USR_DeviceDisconnected
*              ʵ�����ݣ�
*                1. ���°���K1����ͨ�����ڴ�ӡ����ִ�������������115200������λ8����żУ��λ�ޣ�ֹͣλ1��
*                   =================================================
*                   ������      ����״̬ ���ȼ�   ʣ��ջ �������
*                   vTaskUserIF     R       2       251     2
*                   vTaskGUI        R       1       714     1
*                   IDLE            R       0       103     6
*                   vTaskStart      B       5       482     5
*                   vTaskLED        B       3       481     3
*                   vTaskMsgPro     B       4       1937    4
*                   
*                   
*                   ������       ���м���         ʹ����
*                   vTaskUserIF     1565            <1%
*                   vTaskGUI        32567           3%
*                   IDLE            966975          94%
*                   vTaskStart      10286           1%
*                   vTaskLED        0               <1%
*                   vTaskMsgPro     16608           1%
*                  �����������ʹ��SecureCRT��V6���������д�������鿴��ӡ��Ϣ��
*                  ��������ʵ�ֵĹ������£�
*                   vTaskGUI        ����: emWin����
*                   vTaskTaskUserIF ����: �ӿ���Ϣ����	
*                   vTaskLED        ����: LED��˸
*                   vTaskMsgPro     ����: U�����ļ���������
*                   vTaskStart      ����: ��������Ҳ����������ȼ���������ʵ�ְ���ɨ��ʹ������
*                2. ��������״̬�Ķ������£������洮�ڴ�ӡ��ĸB, R, D, S��Ӧ��
*                    #define tskBLOCKED_CHAR		( 'B' )  ����
*                    #define tskREADY_CHAR		    ( 'R' )  ����
*                    #define tskDELETED_CHAR		( 'D' )  ɾ��
*                    #define tskSUSPENDED_CHAR	    ( 'S' )  ����
*                3. ��ʵ���USB Host��Ҫ�Ƕ�U�̵Ĳ�����ͨ�����Զ˵Ĵ������SecureCRT�����
*                   �����ӷ����������ʵ�ֲ���������ʵ����demo_fatfs�ļ����档
*                   	printf("��ѡ���������:\r\n");
*                   	printf("1 - ��ʾ��Ŀ¼�µ��ļ��б�\r\n");
*                   	printf("2 - ����һ�����ļ�armfly.txt\r\n");
*                   	printf("3 - ��armfly.txt�ļ�������\r\n");
*                   	printf("4 - ����Ŀ¼\r\n");
*                   	printf("5 - ɾ���ļ���Ŀ¼\r\n");
*                   	printf("6 - ��д�ļ��ٶȲ���\r\n");
*                   	printf("7 - ����U��\r\n");
*                   	printf("8 - ж��U��\r\n");
*              ע�����
*                1. ��ʵ���Ƽ�ʹ�ô������SecureCRT��Ҫ�����ڴ�ӡЧ�������롣�������
*                   V6��������������С�
*                2. ��ؽ��༭��������������TAB����Ϊ4���Ķ����ļ���Ҫ��������ʾ�����롣
*
*	�޸ļ�¼ :
*		�汾��    ����         ����            ˵��
*       V1.0    2016-03-15   Eric2013    1. ST�̼��⵽V1.6.1�汾
*                                        2. BSP������V1.2
*                                        3. FreeRTOS�汾V8.2.3
*                                        4. STemWin�汾V5.28
*                                        5. FatFS�汾V0.11a
*                                        6. USB��汾V2.2.0
*
*
*	Copyright (C), 2016-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"
#include "MainTask.h"
#include "SysInfoTest.h"

/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
static void vTaskGUI(void *pvParameters);
static void vTaskTaskUserIF(void *pvParameters);
//static void vTaskLED(void *pvParameters);
static void vTaskMsgPro(void *pvParameters);
static void vTaskStart(void *pvParameters);
static void vTaskCom(void *pvParameters);
static void AppTaskCreate (void);
static void AppObjCreate (void);
//static void App_Printf(char *format, ...);
//static void vTimerCallback(xTimerHandle pxTimer);
//static void sendToFPGA(char picNum);
static void picUp(BaseType_t res,uint32_t val);
static void picDown(BaseType_t res,uint32_t val);
/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
TaskHandle_t xHandleTaskGUI = NULL;
TaskHandle_t xHandleTaskUserIF = NULL;
//static TaskHandle_t xHandleTaskLED = NULL;
TaskHandle_t xHandleTaskMsgPro = NULL;
TaskHandle_t xHandleTaskUSBPro = NULL;
TaskHandle_t xHandleTaskStart = NULL;
TaskHandle_t xHandleTaskCom = NULL;
//static SemaphoreHandle_t  xMutex = NULL;
//TimerHandle_t xTimers= NULL;
//QueueHandle_t xQueue1 = NULL;
//QueueHandle_t xQueue2 = NULL;
//QueueHandle_t xQueue3 = NULL;

static char project_number;
//static char picture_num;
static uint8_t cnt_state = 0;
char flag_bl = 0;

extern CanRxMsg g_tCanRxMsg;	/* ���ڽ��� */
//extern char DemoFatFS(void);

extern void can_LedCtrl(void);
extern void can_demo(void);

extern WM_HWIN  hWinMain;
extern WM_HWIN  hWinSet;

extern CONFIG_PARA cfg_para;
extern char file_name[22];
extern CONFIG_PIC  picArray;
extern char cfgname[40][16];
extern uint8_t BL_data[8];
extern struct roundbutton_struct ROUND_BUTTON;
extern char  cfg_flag;

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/

int main(void)
{
	/* 
	  ����������ǰ��Ϊ�˷�ֹ��ʼ��STM32����ʱ���жϷ������ִ�У������ֹȫ���ж�(����NMI��HardFault)��
	  �������ĺô��ǣ�
	  1. ��ִֹ�е��жϷ����������FreeRTOS��API������
	  2. ��֤ϵͳ�������������ܱ���ж�Ӱ�졣
	  3. �����Ƿ�ر�ȫ���жϣ���Ҹ����Լ���ʵ��������ü��ɡ�
	  ����ֲ�ļ�port.c�еĺ���prvStartFirstTask�л����¿���ȫ���жϡ�ͨ��ָ��cpsie i������__set_PRIMASK(1)
	  ��cpsie i�ǵ�Ч�ġ�
     */

	 __set_PRIMASK(1);
	
//	delay_ms(1000);
	
	/* Ӳ����ʼ�� */
	bsp_Init(); 
	
	/* 1. ��ʼ��һ����ʱ���жϣ����ȸ��ڵδ�ʱ���жϣ������ſ��Ի��׼ȷ��ϵͳ��Ϣ ��������Ŀ�ģ�ʵ����
		  Ŀ�в�Ҫʹ�ã���Ϊ������ܱȽ�Ӱ��ϵͳʵʱ�ԡ�
	   2. Ϊ����ȷ��ȡFreeRTOS�ĵ�����Ϣ�����Կ��ǽ�����Ĺر��ж�ָ��__set_PRIMASK(1); ע�͵��� 
	*/
//	vSetupSysInfoTest();
	EXTI_Config();
  
	/* �������� */
	AppTaskCreate();

	/* ��������ͨ�Ż��� */
	AppObjCreate();
	
  /* �������ȣ���ʼִ������ */
  vTaskStartScheduler();

	/* 
	  ���ϵͳ���������ǲ������е�����ģ����е����Ｋ�п��������ڶ�ʱ��������߿��������
	  heap�ռ䲻����ɴ���ʧ�ܣ���Ҫ�Ӵ�FreeRTOSConfig.h�ļ��ж����heap��С��
	  #define configTOTAL_HEAP_SIZE	      ( ( size_t ) ( 17 * 1024 ) )
	*/
	while(1);
}

/*
*********************************************************************************************************
*	�� �� ��: vTaskGUI
*	����˵��: emWin����
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 1   (��ֵԽС���ȼ�Խ�ͣ������uCOS�෴)
*********************************************************************************************************
*/
static void vTaskGUI(void *pvParameters)
{
	while (1) 
	{
		MainTask();
	}
}

/*
*********************************************************************************************************
*	�� �� ��: vTaskTaskUserIF
*	����˵��: ������Ϣ����
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 2 
*********************************************************************************************************
*/
static void vTaskTaskUserIF(void *pvParameters)
{
	uint8_t ucKeyCode;
//	uint8_t pcWriteBuffer[500];
	unsigned char sendbuf[8]={0};
	BaseType_t xResult;
	uint32_t ulValue;
	char cnt = 0;
	char swFlag = 0;
	
    while(1)
    {
			xResult = xTaskNotifyWait(0x00000000,0xFFFFFFFF,&ulValue,xMaxBlockTime);
			if( xResult == pdPASS )
			{
				if((ulValue & BIT_21) != 0)
					swFlag = 1;
			}			
			switch(cnt_state)
			{
				case 1:
					cnt_state = 0;
					GUI_SendKeyMsg(GUI_KEY_UP, 1);
					if(swFlag)
					{
						swFlag = 0;
						picUp(xResult,ulValue);
					}
					break;
				case 2:
					cnt_state = 0;
					GUI_SendKeyMsg(GUI_KEY_DOWN, 1);
					if(swFlag)
					{
						swFlag = 0;
						picDown(xResult,ulValue);
					}
					break;
			}
		ucKeyCode = bsp_GetKey();
		if (ucKeyCode != KEY_NONE)
		{
			switch (ucKeyCode)
			{
				case KEY_DOWN_K1:
          GUI_SendKeyMsg(GUI_KEY_TAB, 1);
					GUI_SendKeyMsg(GUI_KEY_TAB, 0);
					break;
				
				case KEY_2_DOWN:
					GUI_SendKeyMsg(GUI_KEY_SPACE, 1);
					break;
				
				case KEY_3_DOWN:
					GUI_SendKeyMsg(GUI_KEY_LEFT, 1);
					if(swFlag)
					{
						swFlag = 0;
						picUp(xResult,ulValue);
					}
					break;
				
				case KEY_DOWN_K4:		
					GUI_SendKeyMsg(GUI_KEY_RIGHT, 1);
					if(swFlag)
					{
						swFlag = 0;
						picDown(xResult,ulValue);
					}
					break;
				
				case KEY_DOWN_K5:
					if(swFlag)
					{
						swFlag = 0;
						
						memset(sendbuf,0,8);
						CAN_SendMessage(0x02030000,sendbuf);//enable play
		
						CAN_SendMessage(0x01030000,sendbuf);//enable backlight out
						memset(BL_data,0,8);
						flag_bl = 0;
					}
				
					GUI_SendKeyMsg(GUI_KEY_BACKTAB, 1);
					GUI_SendKeyMsg(GUI_KEY_BACKTAB, 0);
					break;
				
				case KEY_DOWN_K6:
					GUI_SendKeyMsg(GUI_KEY_ENTER, 1);
					GUI_SendKeyMsg(GUI_KEY_ENTER, 0);
					break;
				
				case KEY_DOWN_K7:
					if(swFlag)
					{
						swFlag = 0;
						picUp(xResult,ulValue);
					}
					break;
				
				case KEY_DOWN_K8:
					if(swFlag)
					{
						swFlag = 0;
						picDown(xResult,ulValue);
					}
					break;
				
				/* �����ļ�ֵ������ */
				default:
					break;
			}
		}
		
		if((cnt++>100) && flag_bl)
		{
			cnt =0 ;
			CAN_SendMessage(0x01020000,0);
		}
		
		vTaskDelay(20);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: vTaskMsgPro
*	����˵��: �ļ���������
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 3  
*********************************************************************************************************
*/
static void vTaskMsgPro(void *pvParameters)
{			
//	FRESULT result;
//	FATFS fs;
//	FIL fileU;
//	DIR DirInf;
//	uint32_t bw;
//	char bufN[8];
//	char path[32];
	char i;
	char FileNamebuf[16]={0};
	BaseType_t xResult;
	uint32_t ulValue;
	char picture_num;
//	char page=0;
	char len=1;
	uint16_t setVol,setCur;
//	int setTime;
	
	while(1)
	{
//			DemoFatFS();
		xResult = xTaskNotifyWait(0x00000000,0xFFFFFFFF,&ulValue,xMaxBlockTime);
		
		if( xResult == pdPASS )
		{
			if((ulValue & BIT_16) != 0)
				project_number = ulValue & 0xffff;
			if((ulValue & BIT_17) != 0)
				picture_num = ulValue & 0xffff;
			if((ulValue & BIT_18) != 0)
			{
				memset(&cfg_para,0,sizeof(cfg_para));
				ReadFileData(FS_VOLUME_SD,file_name,&cfg_para,sizeof(cfg_para));
			}
			if((ulValue & BIT_19) != 0)
			{
				for(i=0;i<len;i++)
				{
					memset(&cfg_para,0,sizeof(cfg_para));
					sprintf(FileNamebuf, "%s%02d",cfgname[project_number], i);
					ReadFileData(FS_VOLUME_SD,FileNamebuf,&cfg_para,sizeof(cfg_para));
					sendToFPGA(i);
					len = cfg_para.picMaxN;
//					vTaskDelay(20);
				}
				flag_bl = 1;
//				GUI_Delay(300);
//				WM_SendMessageNoPara(hWinMain, MSG_DeleteInfo);
			}
			if((ulValue & BIT_20) != 0)
			{
				CreateNewFile(project_number,picture_num,0);
			}
			if((ulValue & BIT_22) != 0)
			{	
				vTaskPrioritySet(NULL, 7);
				for(i=0;i<len;i++)
				{
					memset(&cfg_para,0,sizeof(cfg_para));
					sprintf(FileNamebuf, "%s%02d",cfgname[project_number], i);
					ReadFileData(FS_VOLUME_SD,FileNamebuf,&cfg_para,sizeof(cfg_para));
					len = cfg_para.picMaxN;				
					WriteCfg2U(&cfg_para,i,cfgname[project_number]);
//					vTaskDelay(20);
				}
				vTaskPrioritySet(NULL, 3);
			}
			if((ulValue & BIT_24) != 0)
			{
				setVol = cfg_para.voltage;
				setCur = cfg_para.current;
//				setTime = cfg_para.totalTime;
				
				for(i=0;i<cfg_para.picMaxN;i++)
				{	
					if(i!=picture_num)
					{
						memset(&cfg_para,0,sizeof(cfg_para));
						sprintf(FileNamebuf, "%s%02d",cfgname[project_number], i);
						ReadFileData(FS_VOLUME_SD,FileNamebuf,&cfg_para,sizeof(cfg_para));
						
						cfg_para.voltage = setVol;
						cfg_para.current = setCur;
//						cfg_para.totalTime = setTime;

						CreateNewFile(project_number,i,0);
					}
				}
				memset(&cfg_para,0,sizeof(cfg_para));
				sprintf(FileNamebuf, "%s%02d",cfgname[project_number], picture_num);
				ReadFileData(FS_VOLUME_SD,FileNamebuf,&cfg_para,sizeof(cfg_para));
			}
		}
	
		vTaskDelay(20);	
	}
}

/*
*********************************************************************************************************
*	�� �� ��: vTaskStart
*	����˵��: ��������Ҳ����������ȼ�������Ҫʵ�ְ������ʹ�����⡣
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 4  
*********************************************************************************************************
*/
static void vTaskStart(void *pvParameters)
{	
	uint16_t count=0;
	uint8_t flag=0;
	char cntLed = 0;
	
	while(1)
	{
		bsp_KeyScan();
		
		if(ROUND_BUTTON.Valid_flag)
		{
			flag = 1;
			ROUND_BUTTON.Valid_flag = 0x00;
			if(ROUND_BUTTON.status == LEFT)
			{				
				cnt_state = 1;
			}
			else{		
				cnt_state = 2;
			}
		}
		if(flag)
		{
			if(++count > 20)
			{
				flag = 0;
				EXIT_OPEN;
				count = 0;
			}
		}
		if(cntLed++ > 50)
		{
			cntLed = 0;
			bsp_LedToggle(1);
		}
			
		vTaskDelay(10);	
	}
}


static void vTaskCom(void *pvParameters)
{
	while(1)
	{
		DemoCOM();
	}
}

/*
*********************************************************************************************************
*	�� �� ��: vTaskUSBPro
*	����˵��: U�̼�⼰��ʼ��
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 5  
*********************************************************************************************************
*/
static void vTaskUSBPro(void *pvParameters)
{
//		usbh_OpenMassStorage();
	
    while(1)
    {
			if(HCD_IsDeviceConnected(&USB_OTG_Core))
			{
				USBH_Process(&USB_OTG_Core, &USB_Host);
			}	
			
			vTaskDelay(10);	
    }
}
				
/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{	
	xTaskCreate( vTaskGUI,            /* ������  */
							 "vTaskGUI",          /* ������    */
							 2048,                /* ����ջ��С����λword��Ҳ����4�ֽ� */
							 NULL,                /* �������  */
							 1,                   /* �������ȼ�*/
							 &xHandleTaskGUI );		/* ������  */
	
	xTaskCreate( vTaskTaskUserIF,   	/* ������  */
							 "vTaskUserIF",     	/* ������    */
							 512,               	/* ����ջ��С����λword��Ҳ����4�ֽ� */
							 NULL,              	/* �������  */
							 2,                 	/* �������ȼ�*/
							 &xHandleTaskUserIF );  /* ������  */
	
	xTaskCreate( vTaskMsgPro,     		/* ������  */
							 "vTaskMsgPro",   		/* ������    */
							 1024,             		/* ����ջ��С����λword��Ҳ����4�ֽ� 2048*/
							 NULL,           			/* �������  */
							 3,               		/* �������ȼ�*/
							 &xHandleTaskMsgPro );  /* ������  */	
	
	xTaskCreate( vTaskStart,     		/* ������  */
							 "vTaskStart",   		/* ������    */
							 256,            		/* ����ջ��С����λword��Ҳ����4�ֽ� */
							 NULL,           		/* �������  */
							 4,              		/* �������ȼ�*/
							 &xHandleTaskStart );   /* ������  */	
	
	xTaskCreate( vTaskCom,     		/* ������  */
							 "vTaskCom",   		/* ������    */
							 512,            		/* ����ջ��С����λword��Ҳ����4�ֽ� */
							 NULL,           		/* �������  */
							 5,              		/* �������ȼ�*/
							 &xHandleTaskCom );   /* ������  */
	
	xTaskCreate( vTaskUSBPro,     		/* ������  */
							 "vTaskUSBPro",   		/* ������    */
							 256,             		/* ����ջ��С����λword��Ҳ����4�ֽ� 2048*/
							 NULL,           			/* �������  */
							 6,               		/* �������ȼ�*/
							 &xHandleTaskUSBPro );  /* ������  */
}

static void picUp(BaseType_t res,uint32_t val)
{
	int8_t qietu_num=0;
	char namebuf[16]={0};
	unsigned char sendbuf[8]={0};
//	BaseType_t xResult;
//	uint32_t ulValue;
	
	qietu_num=picArray.picNum;		

			qietu_num--;
			if(qietu_num<0)
				qietu_num=picArray.picMaxN-1;
			CAN_SendMessage(0x02030001 | (qietu_num<<8),sendbuf);
			memset(namebuf,0,sizeof(namebuf));
			strncpy(namebuf,cfgname[project_number],strlen(cfgname[project_number]));
			sprintf(file_name, "%s%02d", namebuf,qietu_num);
			ReadFileData(FS_VOLUME_SD,file_name,&picArray,sizeof(CONFIG_PIC));

}

static void picDown(BaseType_t res,uint32_t val)
{
	int8_t qietu_num=0;
	char namebuf[16]={0};
	unsigned char sendbuf[8]={0};
	
	qietu_num=picArray.picNum;

			qietu_num++;
			if(qietu_num>picArray.picMaxN-1)
				qietu_num=0;
			CAN_SendMessage(0x02030001 | (qietu_num<<8),sendbuf);
			memset(namebuf,0,sizeof(namebuf));
			strncpy(namebuf,cfgname[project_number],strlen(cfgname[project_number]));
			sprintf(file_name, "%s%02d", namebuf,qietu_num);
			ReadFileData(FS_VOLUME_SD,file_name,&picArray,sizeof(CONFIG_PIC));
		
}

/*
*********************************************************************************************************
*	�� �� ��: AppObjCreate
*	����˵��: ��������ͨ�Ż���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppObjCreate (void)
{
//	uint8_t i;
//	const TickType_t  xTimerPer[2] = {50, 100};
	
//	for(i = 0; i < 2; i++)
//	{
//		xTimers = xTimerCreate("Timer",          /* ��ʱ������ */
//								xTimerPer[i],       /* ��ʱ������,��λʱ�ӽ��� */
//								pdTRUE,          /* ������ */
//								(void *) i,      /* ��ʱ��ID */
//								vTimerCallback); /* ��ʱ���ص����� */

//		if(xTimers == NULL)
//		{
//			/* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
//		}
//	}
	
//	/* �����¼���־�� */
//	xCreatedEventGroup = xEventGroupCreate();	
//	if(xCreatedEventGroup == NULL)
//	{
//			/* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
//	}
		
	/* ����10��uint8_t����Ϣ���� */
//	xQueue1 = xQueueCreate(10, sizeof(uint8_t));
//		if( xQueue1 == 0 )
//		{
//				/* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
//		}
//	
//	/* ����10��uint8_t����Ϣ���� */
////		xQueue2 = xQueueCreate(10, sizeof(uint8_t *));
//	xQueue2 = xQueueCreate(10, sizeof(g_tCanRxMsg.Data));
//		if( xQueue2 == 0 )
//		{
//				/* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
//		}
//	xQueue3 = xQueueCreate(10, sizeof(g_tCanRxMsg.Data));
//	
//	/* ���������ź��� */
//    xMutex = xSemaphoreCreateMutex();
//	
//	if(xMutex == NULL)
//    {
//        /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
//    }
}

/*
*********************************************************************************************************
*	�� �� ��: vTimerCallback
*	����˵��: ��ʱ���ص�����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void vTimerCallback(xTimerHandle pxTimer)
{
//	uint32_t ulTimerID;
//	
//	configASSERT(pxTimer);

//	/* ��ȡ�Ǹ���ʱ��ʱ�䵽 */
//	ulTimerID = (uint32_t)pvTimerGetTimerID(pxTimer);	
//	
//	/* ����ʱ��0���� */
//	if(ulTimerID == 0)
//	{
//		can_LedCtrl();
//	}
//	
//	/* ����ʱ��1���� */
//	if(ulTimerID == 1)
//	{
//		bsp_LedToggle(2);
//	}	
}

/*
*********************************************************************************************************
*	�� �� ��: App_Printf
*	����˵��: �̰߳�ȫ��printf��ʽ		  			  
*	��    ��: ͬprintf�Ĳ�����
*             ��C�У����޷��г����ݺ���������ʵ�ε����ͺ���Ŀʱ,������ʡ�Ժ�ָ��������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void  App_Printf(char *format, ...)
{
//    char  buf_str[200 + 1];
//    va_list   v_args;


//    va_start(v_args, format);
//   (void)vsnprintf((char       *)&buf_str[0],
//                   (size_t      ) sizeof(buf_str),
//                   (char const *) format,
//                                  v_args);
//    va_end(v_args);

//	/* �����ź��� */
//	xSemaphoreTake(xMutex, portMAX_DELAY);

//    printf("%s", buf_str);

//   	xSemaphoreGive(xMutex);
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
