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
static void AppTaskCreate (void);
static void AppObjCreate (void);
//static void App_Printf(char *format, ...);
//static void vTimerCallback(xTimerHandle pxTimer);
static void sendToFPGA(char picNum);
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
static SemaphoreHandle_t  xMutex = NULL;
TimerHandle_t xTimers= NULL;
QueueHandle_t xQueue1 = NULL;
QueueHandle_t xQueue2 = NULL;
QueueHandle_t xQueue3 = NULL;

static const TickType_t xMaxBlockTime = pdMS_TO_TICKS(100); /* �������ȴ�ʱ��Ϊ500ms */
static char project_number;
//static char picture_num;
static uint8_t cnt_state = 0;
static char flag_bl = 0;

extern CanRxMsg g_tCanRxMsg;	/* ���ڽ��� */
//extern char DemoFatFS(void);

extern void can_LedCtrl(void);
extern void can_demo(void);

extern WM_HWIN  hWinMain;

extern CONFIG_PARA cfg_para;
extern char file_name[22];
extern CONFIG_PIC  picArray;
extern struct _CONFIG_CHANNEL_ cfg_ch_tmp;
extern struct _CONFIG_DATA cfg_data_tmp;
extern char cfgname[8][16];
extern uint8_t BL_data[8];
extern struct roundbutton_struct ROUND_BUTTON;

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
	
    while(1)
    {
			xResult = xTaskNotifyWait(0x00000000,0xFFFFFFFF,&ulValue,xMaxBlockTime);
			switch(cnt_state)
			{
				case 1:
					cnt_state = 0;
					GUI_SendKeyMsg(GUI_KEY_UP, 1);
					if( xResult == pdPASS )
					{
						if((ulValue & BIT_21) != 0)
							picUp(xResult,ulValue);
					}
					break;
				case 2:
					cnt_state = 0;
					GUI_SendKeyMsg(GUI_KEY_DOWN, 1);
					if( xResult == pdPASS )
					{
						if((ulValue & BIT_21) != 0)
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
					if( xResult == pdPASS )
					{
						if((ulValue & BIT_21) != 0)
							picUp(xResult,ulValue);
					}
					break;
				
				case KEY_DOWN_K4:		
					GUI_SendKeyMsg(GUI_KEY_RIGHT, 1);
					if( xResult == pdPASS )
					{
						if((ulValue & BIT_21) != 0)
							picDown(xResult,ulValue);
					}
					break;
				
				case KEY_DOWN_K5:
					if( xResult == pdPASS )
					{
						if((ulValue & BIT_21) != 0)
						{
							memset(sendbuf,0,8);
							CAN_SendMessage(0x02030000,sendbuf);//enable play
			
							CAN_SendMessage(0x01030000,sendbuf);//enable backlight out
							memset(BL_data,0,8);
							flag_bl = 0;
						}
					}
				
					GUI_SendKeyMsg(GUI_KEY_BACKTAB, 1);
					break;
				
				case KEY_DOWN_K6:
					GUI_SendKeyMsg(GUI_KEY_ENTER, 1);
					GUI_SendKeyMsg(GUI_KEY_ENTER, 0);
					break;
				
//				case KEY_DOWN_K7:
//					if( xResult == pdPASS )
//					{
//						if((ulValue & BIT_21) != 0)
//							picUp(xResult,ulValue);
//					}
//					break;
//				
//				case KEY_DOWN_K8:
//					if( xResult == pdPASS )
//					{
//						if((ulValue & BIT_21) != 0)
//							picDown(xResult,ulValue);
//					}
//					break;
//				
//				case KEY_DOWN_K9:
//					
//					break;
				
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
//	FIL file;
//	DIR DirInf;
//	uint32_t bw;
//	char bufN[8];
//	char path[32];
	char i;
	char FileNamebuf[16]={0};
	BaseType_t xResult;
	uint32_t ulValue;
	char picture_num;
	
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
//				xTaskNotifyGive(xHandleTaskGUI);
			}
			if((ulValue & BIT_19) != 0)
			{
				for(i=cfg_para.picMaxN;i>0;i--)
				{
					memset(&cfg_para,0,sizeof(cfg_para));
					sprintf(FileNamebuf, "%s%02d",file_name, i-1);
					ReadFileData(FS_VOLUME_SD,FileNamebuf,&cfg_para,sizeof(cfg_para));
					sendToFPGA(i-1);
					vTaskDelay(20);
				}
				flag_bl = 1;
				WM_SendMessageNoPara(hWinMain, MSG_DeleteInfo);
			}
			if((ulValue & BIT_20) != 0)
			{
				CreateNewFile(project_number,picture_num,0);
//				xTaskNotifyGive(xHandleTaskGUI);
			}
			if((ulValue & BIT_22) != 0)
			{					
				WriteCfg2U(project_number);
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
			
		vTaskDelay(10);	
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
							 512,             		/* ����ջ��С����λword��Ҳ����4�ֽ� 2048*/
							 NULL,           			/* �������  */
							 3,               		/* �������ȼ�*/
							 &xHandleTaskMsgPro );  /* ������  */	
	
	
	xTaskCreate( vTaskStart,     		/* ������  */
							 "vTaskStart",   		/* ������    */
							 512,            		/* ����ջ��С����λword��Ҳ����4�ֽ� */
							 NULL,           		/* �������  */
							 4,              		/* �������ȼ�*/
							 &xHandleTaskStart );   /* ������  */
	
	xTaskCreate( vTaskUSBPro,     		/* ������  */
							 "vTaskUSBPro",   		/* ������    */
							 512,             		/* ����ջ��С����λword��Ҳ����4�ֽ� 2048*/
							 NULL,           			/* �������  */
							 5,               		/* �������ȼ�*/
							 &xHandleTaskUSBPro );  /* ������  */	
}

static void sendSigData(char picNum)
{
	#define deal_num(x)		(2 - x / 16.7647)/4.096*1024
	int signalID = 0x02010000;
	int dataID = 0x02020000;
	unsigned char sendbuf[8]={0};
	char i,j;
	unsigned int datatmp1=0,datatmp2=0;
	float voltage;
	char flag=0;
	
	for(i=0;i<37;i++)
	{
		cfg_ch_tmp = getChannel(i);
		cfg_data_tmp = cfg_ch_tmp.cfg_data;
		if(i >= 24)
		{			
			for(j = 0 ; j < 8 ; j++)
			{
				datatmp1 = getChData(2*j + 0);
				flag = datatmp1>>31;
				if(flag == 0)
				{
					if(datatmp1 != 0)
					{
						voltage = (datatmp1 & 0x7fff);
						voltage = voltage / 100.0 - 31;
						datatmp2 = deal_num(voltage);
						datatmp1 = (datatmp2 & 0x7fff) | (datatmp1 & (~0x7fff));
						sendbuf[3]=datatmp1>>24;
						sendbuf[2]=datatmp1>>16 & 0xff;
						sendbuf[1]=datatmp1>>8 & 0xff;
						sendbuf[0]=datatmp1 & 0xff;
					}else{
						sendbuf[3]=0x00;
						sendbuf[2]=0x00;
						sendbuf[1]=0x00;
						sendbuf[0]=0x00;
					}
				}else{
					sendbuf[3]=datatmp1>>24;
					sendbuf[2]=datatmp1>>16 & 0xff;
					sendbuf[1]=datatmp1>>8 & 0xff;
					sendbuf[0]=datatmp1 & 0xff;
				}
				
				datatmp1 = getChData(2*j + 1);
				flag = datatmp1>>31;
				if(flag ==0)
				{
					if(datatmp1 != 0)
					{
						voltage = (datatmp1 & 0x7fff);
						voltage = voltage / 100.0 - 31;
						datatmp2 = deal_num(voltage);
						datatmp1 = (datatmp2 & 0x7fff) | (datatmp1 & (~0x7fff));
						sendbuf[7]=datatmp1>>24;
						sendbuf[6]=datatmp1>>16 & 0xff;
						sendbuf[5]=datatmp1>>8 & 0xff;
						sendbuf[4]=datatmp1 & 0xff;
					}else{
						sendbuf[7]=0x00;
						sendbuf[6]=0x00;
						sendbuf[5]=0x00;
						sendbuf[4]=0x00;
					}
				}else{					
					sendbuf[7]=datatmp1>>24;
					sendbuf[6]=datatmp1>>16 & 0xff;
					sendbuf[5]=datatmp1>>8 & 0xff;
					sendbuf[4]=datatmp1 & 0xff;
				}
				CAN_SendMessage(dataID | picNum<<8 | ((i-24)<<3) | j,sendbuf);
				vTaskDelay(1);
				if( *((unsigned int *)(sendbuf + 4)) == 0)
					break;
				
			}
		}
		else
		{			
			for(j = 0 ; j < 8 ; j++)
			{
				datatmp1 = getChData(2*j + 0);
				sendbuf[3]=datatmp1>>24;
				sendbuf[2]=datatmp1>>16 & 0xff;
				sendbuf[1]=datatmp1>>8 & 0xff;
				sendbuf[0]=datatmp1 & 0xff;
				
				datatmp1 = getChData(2*j + 1);
				sendbuf[7]=datatmp1>>24;
				sendbuf[6]=datatmp1>>16 & 0xff;
				sendbuf[5]=datatmp1>>8 & 0xff;
				sendbuf[4]=datatmp1 & 0xff;
				
				CAN_SendMessage(signalID | picNum<<8 | (i<<3) | j,sendbuf);	
				vTaskDelay(1);
				if( *((unsigned int *)(sendbuf + 4)) == 0)
					break;
			}
		}
	 }
}

static void sendToFPGA(char picNum)
{
	#define deal_num(x) (2 - ((x / 100.0) - 31) /10.0) * 1024 / 4.096
	int ZU_ID1 = 0x02000000;
	int ZU_ID2 = 0x02000001;
	int ZU_ID3 = 0x02000002;
	int ZU_ID4 = 0x02000003;
	int BL_ID  = 0x01010000;
	int BL_EN  = 0x01030000;
	short sg[4] ;
	unsigned char sendbuf[8]={0};
	
	sg[0] = deal_num(cfg_para.cfg_ch_zu.ch_zu1_h);
	sg[1] = deal_num(cfg_para.cfg_ch_zu.ch_zu1_l);
	sg[2] = deal_num(cfg_para.cfg_ch_zu.ch_zu2_h);
	sg[3] = deal_num(cfg_para.cfg_ch_zu.ch_zu2_l);
	sendbuf[0] = sg[0]>>8;
	sendbuf[1] = sg[0] & 0xff;
	sendbuf[2] = sg[1]>>8;
	sendbuf[3] = sg[1] & 0xff;
	sendbuf[4] = sg[2]>>8;
	sendbuf[5] = sg[2] & 0xff;
	sendbuf[6] = sg[3]>>8;
	sendbuf[7] = sg[3] & 0xff;
	CAN_SendMessage(ZU_ID1 | picNum<<8,sendbuf);
	
	sg[0] = deal_num(cfg_para.cfg_ch_zu.ch_zu3_h);
	sg[1] = deal_num(cfg_para.cfg_ch_zu.ch_zu3_l);
	sg[2] = deal_num(cfg_para.cfg_ch_zu.ch_zu4_h);
	sg[3] = deal_num(cfg_para.cfg_ch_zu.ch_zu4_l);
	sendbuf[0] = sg[0]>>8;
	sendbuf[1] = sg[0] & 0xff;
	sendbuf[2] = sg[1]>>8;
	sendbuf[3] = sg[1] & 0xff;
	sendbuf[4] = sg[2]>>8;
	sendbuf[5] = sg[2] & 0xff;
	sendbuf[6] = sg[3]>>8;
	sendbuf[7] = sg[3] & 0xff;
	CAN_SendMessage(ZU_ID2 | picNum<<8,sendbuf);
	
  sg[0] = deal_num(cfg_para.cfg_ch_zu.ch_zu5_h);
	sg[1] = deal_num(cfg_para.cfg_ch_zu.ch_zu5_l);
	sg[2] = deal_num(cfg_para.cfg_ch_zu.ch_zu6_h);
	sg[3] = deal_num(cfg_para.cfg_ch_zu.ch_zu6_l);
	sendbuf[0] = sg[0]>>8;
	sendbuf[1] = sg[0] & 0xff;
	sendbuf[2] = sg[1]>>8;
	sendbuf[3] = sg[1] & 0xff;
	sendbuf[4] = sg[2]>>8;
	sendbuf[5] = sg[2] & 0xff;
	sendbuf[6] = sg[3]>>8;
	sendbuf[7] = sg[3] & 0xff;
	CAN_SendMessage(ZU_ID3 | picNum<<8,sendbuf);
	
	vTaskDelay(1);
	
	memset(sendbuf,0,8);
	sendbuf[0] = cfg_para.totalTime>>24;
	sendbuf[1] = (cfg_para.totalTime>>16) & 0xff;
	sendbuf[2] = (cfg_para.totalTime>>8) & 0xff;
	sendbuf[3] = cfg_para.totalTime & 0xff;
	CAN_SendMessage(ZU_ID4 | picNum<<8,sendbuf);
	vTaskDelay(2);
	memset(sendbuf,0,8);
	sendbuf[0] = (cfg_para.voltage/10)>>8;
	sendbuf[1] = (cfg_para.voltage/10) & 0xff;
	sendbuf[2] = (cfg_para.current/100)>>8;
	sendbuf[3] = (cfg_para.current/100) & 0xff;
	sendbuf[4] = (cfg_para.current/100 +150) >> 8;
	sendbuf[5] = (cfg_para.current/100 +150) & 0xff;
	
	CAN_SendMessage(BL_ID,sendbuf);
	vTaskDelay(5);
	
	memset(sendbuf,0,8);
	sendbuf[0] = 0x01;
	CAN_SendMessage(BL_EN,sendbuf);
	
	vTaskDelay(5);
	
	sendSigData(picNum);
}

static void picUp(BaseType_t res,uint32_t val)
{
	int8_t qietu_num=0;
	char namebuf[16]={0};
	unsigned char sendbuf[8]={0};
	BaseType_t xResult;
	uint32_t ulValue;
	
	qietu_num=picArray.picNum;
	xResult = res;
	ulValue = val;
		
	if( xResult == pdPASS )
	{
		if((ulValue & BIT_21) != 0)
		{
			qietu_num--;
			if(qietu_num<0)
				qietu_num=picArray.picMaxN-1;
			CAN_SendMessage(0x02030001 | (qietu_num<<8),sendbuf);
			memset(namebuf,0,sizeof(namebuf));
			strncpy(namebuf,cfgname[project_number],strlen(cfgname[project_number]));
			sprintf(file_name, "%s%02d", namebuf,qietu_num);
			ReadFileData(FS_VOLUME_SD,file_name,&picArray,sizeof(CONFIG_PIC));
		}
	}
}

static void picDown(BaseType_t res,uint32_t val)
{
	int8_t qietu_num=0;
	char namebuf[16]={0};
	unsigned char sendbuf[8]={0};
	BaseType_t xResult;
	uint32_t ulValue;
	
	qietu_num=picArray.picNum;
	xResult = res;
	ulValue = val;
		
	if( xResult == pdPASS )
	{
		if((ulValue & BIT_21) != 0)
		{
			qietu_num++;
			if(qietu_num>picArray.picMaxN-1)
				qietu_num=0;
			CAN_SendMessage(0x02030001 | (qietu_num<<8),sendbuf);
			memset(namebuf,0,sizeof(namebuf));
			strncpy(namebuf,cfgname[project_number],strlen(cfgname[project_number]));
			sprintf(file_name, "%s%02d", namebuf,qietu_num);
			ReadFileData(FS_VOLUME_SD,file_name,&picArray,sizeof(CONFIG_PIC));
		}
	}
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
	xQueue1 = xQueueCreate(10, sizeof(uint8_t));
		if( xQueue1 == 0 )
		{
				/* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
		}
	
	/* ����10��uint8_t����Ϣ���� */
//		xQueue2 = xQueueCreate(10, sizeof(uint8_t *));
	xQueue2 = xQueueCreate(10, sizeof(g_tCanRxMsg.Data));
		if( xQueue2 == 0 )
		{
				/* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
		}
	xQueue3 = xQueueCreate(10, sizeof(g_tCanRxMsg.Data));
	
	/* ���������ź��� */
    xMutex = xSemaphoreCreateMutex();
	
	if(xMutex == NULL)
    {
        /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
    }
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
