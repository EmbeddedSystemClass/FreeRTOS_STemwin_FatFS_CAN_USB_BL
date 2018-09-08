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




/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
static void vTaskGUI(void *pvParameters);
static void vTaskTaskUserIF(void *pvParameters);
static void vTaskLED(void *pvParameters);
static void vTaskMsgPro(void *pvParameters);
static void vTaskStart(void *pvParameters);
static void AppTaskCreate (void);
static void AppObjCreate (void);
static void App_Printf(char *format, ...);
static void vTimerCallback(xTimerHandle pxTimer);


/*
**********************************************************************************************************
											��������
**********************************************************************************************************
*/
static TaskHandle_t xHandleTaskUserIF = NULL;
static TaskHandle_t xHandleTaskLED = NULL;
static TaskHandle_t xHandleTaskMsgPro = NULL;
static TaskHandle_t xHandleTaskStart = NULL;
static SemaphoreHandle_t  xMutex = NULL;
TimerHandle_t xTimers= NULL;
QueueHandle_t xQueue1 = NULL;
QueueHandle_t xQueue2 = NULL;

extern void can_LedCtrl(void);
extern void can_demo(void);


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
	
	/* Ӳ����ʼ�� */
	bsp_Init(); 
	
	/* 1. ��ʼ��һ����ʱ���жϣ����ȸ��ڵδ�ʱ���жϣ������ſ��Ի��׼ȷ��ϵͳ��Ϣ ��������Ŀ�ģ�ʵ����
		  Ŀ�в�Ҫʹ�ã���Ϊ������ܱȽ�Ӱ��ϵͳʵʱ�ԡ�
	   2. Ϊ����ȷ��ȡFreeRTOS�ĵ�����Ϣ�����Կ��ǽ�����Ĺر��ж�ָ��__set_PRIMASK(1); ע�͵��� 
	*/
	vSetupSysInfoTest();
	
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
//	uint8_t ucKeyCode;
//	uint8_t pcWriteBuffer[500];
	

    while(1)
    {
//		ucKeyCode = bsp_GetKey();
//		
//		if (ucKeyCode != KEY_NONE)
//		{
//			switch (ucKeyCode)
//			{
//				/* K1������ ��ӡ����ִ����� */
//				case KEY_DOWN_K1:			 
//					App_Printf("=================================================\r\n");
//					App_Printf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
//					vTaskList((char *)&pcWriteBuffer);
//					App_Printf("%s\r\n", pcWriteBuffer);
//				
//					App_Printf("\r\n������       ���м���         ʹ����\r\n");
//					vTaskGetRunTimeStats((char *)&pcWriteBuffer);
//					App_Printf("%s\r\n", pcWriteBuffer);
//					printf("��ǰ��̬�ڴ�ʣ���С = %d�ֽ�\r\n", xPortGetFreeHeapSize());
//					break;
//				
//				/* �����ļ�ֵ������ */
//				default:                     
//					break;
//			}
//		}
		can_demo();
		
//		vTaskDelay(20);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: vTaskLED
*	����˵��: LED��˸
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 3 
*********************************************************************************************************
*/
static void vTaskLED(void *pvParameters)
{
//	TickType_t xLastWakeTime;
//	const TickType_t xFrequency = 200;

//	/* ��ȡ��ǰ��ϵͳʱ�� */
//    xLastWakeTime = xTaskGetTickCount();
	BaseType_t xResult;
	uint8_t ucQueueMsgValue;
	
    while(1)
    {
		/* ע�͵�LED����˸����ΪLED��SDRAM��ʹ��FMC���ߣ���������г�ͻ��
		   ʹ��IAR�汾���Ż��ȼ�NONE������£�������ʾ���ϵ�������ߡ�
		   ʹ��MDK���Ż��ȼ�Ϊ0��ʱ��û��������⡣������Ϊ�˱��ջ��ǽ�LED
		   ��˸ע�͵���
		*/
       	//bsp_LedToggle(2);
		//bsp_LedToggle(3);

		/* vTaskDelayUntil�Ǿ����ӳ٣�vTaskDelay������ӳ١�*/
//        vTaskDelayUntil(&xLastWakeTime, xFrequency);
		xResult = xQueueReceive(xQueue1,                   /* ��Ϣ���о�� */
		                        (void *)&ucQueueMsgValue,  /* �洢���յ������ݵ�����ucQueueMsgValue�� */
		                        (TickType_t)portMAX_DELAY);/* ��������ʱ�� */
		
		if(xResult == pdPASS)
		{
			/* �ɹ����գ���ͨ�����ڽ����ݴ�ӡ���� */
			/* �ȹر����е�LED���ٵ�������һ��LED */
			bsp_LedOff(1);
			bsp_LedOff(2);
			bsp_LedOff(3);
			bsp_LedOff(4);					
			bsp_LedOn(ucQueueMsgValue + 1);
		}
    }
}

/*
*********************************************************************************************************
*	�� �� ��: vTaskMsgPro
*	����˵��: U�����ļ���������
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 4  
*********************************************************************************************************
*/
extern void DemoFatFS(void);
static void vTaskMsgPro(void *pvParameters)
{
    while(1)
    {
		DemoFatFS();			
    }
	
//	BaseType_t xResult;
//	uint8_t ucQueueMsgValue;
//	
//    while(1)
//    {
//		xResult = xQueueReceive(xQueue2,                   /* ��Ϣ���о�� */
//		                        (void *)&ucQueueMsgValue,  /* �洢���յ������ݵ�����ucQueueMsgValue�� */
//		                        (TickType_t)portMAX_DELAY);/* ��������ʱ�� */
//		
//		if(xResult == pdPASS)
//		{
//			BEEP_KeyTone();
//		}
//    }
}

/*
*********************************************************************************************************
*	�� �� ��: vTaskStart
*	����˵��: ��������Ҳ����������ȼ�������Ҫʵ�ְ������ʹ�����⡣
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 5  
*********************************************************************************************************
*/
static void vTaskStart(void *pvParameters)
{
	uint8_t  ucCount = 0;
	uint8_t  ucCount1 = 0;
    
	
    while(1)
    {
		BEEP_Pro();	    /* ������������ */
		/* 1msһ�δ���ɨ�裬���败���� */
		if(g_tTP.Enable == 1)
		{
			TOUCH_Scan();
			
			/* ����ɨ�� */
			ucCount++;
			if(ucCount == 10)
			{
				ucCount = 0;
				bsp_KeyScan();
			}
		    vTaskDelay(1);			
		}
		
		/* 10msһ�δ���ɨ�裬���ݴ�����GT811 */
		if(g_GT811.Enable == 1)
		{
			bsp_KeyScan();
			ucCount1++;
			if(ucCount1 == 2)
			{
				ucCount1 = 0;
				GT811_OnePiontScan();
			}
		    vTaskDelay(10);	
		}
		
		/* 10msһ�δ���ɨ�裬���ݴ�����FT5X06 */
		if(g_tFT5X06.Enable == 1)
		{
			bsp_KeyScan();
			FT5X06_OnePiontScan();
		    vTaskDelay(10);	
		}
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
	xTaskCreate(  vTaskGUI,             /* ������  */
                  "vTaskGUI",           /* ������    */
                  1024,                 /* ����ջ��С����λword��Ҳ����4�ֽ� */
                  NULL,                 /* �������  */
                  1,                    /* �������ȼ�*/
                  NULL );               /* ������  */
	
    xTaskCreate( vTaskTaskUserIF,   	/* ������  */
                 "vTaskUserIF",     	/* ������    */
                 512,               	/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,              	/* �������  */
                 2,                 	/* �������ȼ�*/
                 &xHandleTaskUserIF );  /* ������  */
	
	
	xTaskCreate( vTaskLED,    		/* ������  */
                 "vTaskLED",  		/* ������    */
                 512,         		/* stack��С����λword��Ҳ����4�ֽ� */
                 NULL,        		/* �������  */
                 3,           		/* �������ȼ�*/
                 &xHandleTaskLED ); /* ������  */
	
	xTaskCreate( vTaskMsgPro,     		/* ������  */
                 "vTaskMsgPro",   		/* ������    */
                 2048,             		/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,           		/* �������  */
                 4,               		/* �������ȼ�*/
                 &xHandleTaskMsgPro );  /* ������  */
	
	
	xTaskCreate( vTaskStart,     		/* ������  */
                 "vTaskStart",   		/* ������    */
                 512,            		/* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,           		/* �������  */
                 5,              		/* �������ȼ�*/
                 &xHandleTaskStart );   /* ������  */
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
//	const TickType_t  xTimerPer = 50;
//	
//	xTimers = xTimerCreate("Timer",          /* ��ʱ������ */
//							xTimerPer,       /* ��ʱ������,��λʱ�ӽ��� */
//							pdTRUE,          /* ������ */
//							(void *) 0,      /* ��ʱ��ID */
//							vTimerCallback); /* ��ʱ���ص����� */

//	if(xTimers == NULL)
//	{
//		/* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
//	}
	
	/* ����10��uint8_t����Ϣ���� */
	xQueue1 = xQueueCreate(10, sizeof(uint8_t));
    if( xQueue1 == 0 )
    {
        /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
    }
	
	/* ����10��uint8_t����Ϣ���� */
	xQueue2 = xQueueCreate(10, sizeof(uint8_t));
    if( xQueue2 == 0 )
    {
        /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
    }
	
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
	configASSERT(pxTimer);
	
	can_LedCtrl();
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
