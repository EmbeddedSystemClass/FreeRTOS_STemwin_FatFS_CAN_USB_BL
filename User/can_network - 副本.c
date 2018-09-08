/*
*********************************************************************************************************
*	                                  
*	ģ������ : CAN������ʾ����
*	�ļ����� : can_network.c
*	��    �� : V1.3
*	˵    �� : ��ʾ���ʵ�ֶ��CAN�ڵ㣨�ڵ�ĳ�����ͬ��֮���ͨ�š�
*	�޸ļ�¼ :
*		�汾��  ����       ����      ˵��
*		v1.3    2015-01-29 Eric2013  �׷�
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"
#include "can_network.h"

uint8_t FPGA_data[3][8]={0};
uint8_t BL_data[8]={0};
uint8_t PIC_normal_abmormal=0;

/*
*********************************************************************************************************
                                            CAN1��������
��������ñ�����ã�
	CAN1��TX����ѡ������PB9����PA12��
	CAN1��RX����ѡ������PB8����PA11��
	�Ӷ�������������ϣ�
	PB9_TX      PB8_RX 
	PA12_TX     PB8_RX 
	PA12_TX     PA11_RX 
	PB9_TX      PA11_RX 

���������õ�PB9_TX   PB8_RX��Ҳ����:
	1. J12����ñ��·1-2�� CAN�շ�����RX���ӵ�CPU��PB8���š�
	2. J13����ñ��·1-2�� CAN�շ�����TX���ӵ�CPU��PB9���š�
********************************************************************************************************
*/
#define CAN1_CLK                    RCC_APB1Periph_CAN1
#define CAN1_RX_PIN                 GPIO_Pin_11
#define CAN1_TX_PIN                 GPIO_Pin_12
#define CAN1_GPIO_TX_PORT           GPIOA
#define CAN1_GPIO_TX_CLK            RCC_AHB1Periph_GPIOA
#define CAN1_GPIO_RX_PORT           GPIOA
#define CAN1_GPIO_RX_CLK            RCC_AHB1Periph_GPIOA
#define CAN1_AF_PORT                GPIO_AF_CAN1
#define CAN1_RX_SOURCE              GPIO_PinSource11
#define CAN1_TX_SOURCE              GPIO_PinSource12

/*
*********************************************************************************************************
                                            CAN2��������
********************************************************************************************************
*/
#define CAN2_CLK                 RCC_APB1Periph_CAN2
#define CAN2_RX_PIN              GPIO_Pin_12
#define CAN2_TX_PIN              GPIO_Pin_13
#define CAN2_GPIO_PORT           GPIOB
#define CAN2_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define CAN2_AF_PORT             GPIO_AF_CAN2
#define CAN2_RX_SOURCE           GPIO_PinSource12
#define CAN2_TX_SOURCE           GPIO_PinSource13


/* ����ȫ�ֱ��� */
CanTxMsg g_tCanTxMsg;	/* ���ڷ��� */
CanRxMsg g_tCanRxMsg;	/* ���ڽ��� */
uint8_t g_ucLedNo = 0;	/* ������LED����ţ�0-3 */

/* �������ļ��ڵ��õĺ������� */
//static void can1_Init(void);
//static void can1_NVIC_Config(void);
//static void can2_Init(void);
//static void can2_NVIC_Config(void);

void can_LedCtrl(void);
void can_BeepCtrl(void);

extern TimerHandle_t xTimers;
extern QueueHandle_t xQueue1;
extern QueueHandle_t xQueue2;

/*
*********************************************************************************************************
*	�� �� ��: can_demo
*	����˵��: CAN��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void can_demo(void)
{	
//	uint8_t ucKeyCode;	   	/* �������� */
	
	can1_Init();			/* ��ʼ��STM32 CAN1Ӳ�� */
	can1_NVIC_Config();		/* ����CAN1�ж� */
	
//	can2_Init();			/* ��ʼ��STM32 CAN2Ӳ�� */
//	can2_NVIC_Config();		/* ����CAN2�ж� */
	
	g_ucLedNo = 0;
	/* �ȹر����е�LED���ٵ�������һ��LED */
	bsp_LedOff(1);
	bsp_LedOff(2);
	bsp_LedOff(3);
	bsp_LedOff(4);
	bsp_LedOn(g_ucLedNo + 1);

	while(1)
	{
		/* �������¼� */
//		ucKeyCode = bsp_GetKey();
//		if (ucKeyCode > 0)
//		{
//			/* �м����� */
//			switch (ucKeyCode)
//			{
//				case KEY_DOWN_K1:			/* K1������ */
//					can_LedCtrl();		
//					break;

//				case KEY_DOWN_K2:			/* K2������ */
//					xTimerStart(xTimers, 50);
//					break;

//				case KEY_DOWN_K3:			/* K3������ */
//					xTimerStop( xTimers, portMAX_DELAY);
//					break;
//				
//				case JOY_DOWN_OK:			/* K3������ */
//					can_BeepCtrl();
//					break;

//				default:
//					/* �����ļ�ֵ������ */
//					break;
//			}
//		}
		
		vTaskDelay(5);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: can_LedCtrl
*	����˵��: ����һ�����ݣ�����LED
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void can_LedCtrl(void)
{
	/* ÿ��1�ΰ�����������һ��LED������CAN���緢��һ������ */
	if (++g_ucLedNo > 3)
	{
		g_ucLedNo = 0;
	}

	
	/* ��䷢�Ͳ��� */
	g_tCanTxMsg.StdId = 0x321;
	g_tCanTxMsg.ExtId = 0x01;
	g_tCanTxMsg.RTR = CAN_RTR_DATA;
	g_tCanTxMsg.IDE = CAN_ID_STD;
	
	/* ��CAN���緢��1���ֽ�����, ָLED����� */
	g_tCanTxMsg.DLC = 1;          /* ÿ������֧��0-8���ֽڣ���������Ϊ����1���ֽ� */
    g_tCanTxMsg.Data[0] = g_ucLedNo;
	
    CAN_Transmit(CAN1, &g_tCanTxMsg);	
}

/*
*********************************************************************************************************
*	�� �� ��: can_BeepCtrl
*	����˵��: ����һ�����ݣ����Ʒ�����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void can_BeepCtrl(void)
{		
	/* ��䷢�Ͳ����� Ҳ������ÿ�η��͵�ʱ���� */
	g_tCanTxMsg.StdId = 0x321;
	g_tCanTxMsg.ExtId = 0x01;
	g_tCanTxMsg.RTR = CAN_RTR_DATA;
	g_tCanTxMsg.IDE = CAN_ID_STD;
	
	g_tCanTxMsg.DLC = 1;
						
	g_tCanTxMsg.Data[1] = 0x01;		/* 01��ʾ����1�� */
    CAN_Transmit(CAN2, &g_tCanTxMsg);	
}

void CAN_SendMessage(unsigned int ID,unsigned char *array)
{
	g_tCanTxMsg.ExtId=ID;
	g_tCanTxMsg.RTR = CAN_RTR_DATA;
	g_tCanTxMsg.IDE = CAN_ID_EXT;	
	g_tCanTxMsg.DLC = 8;
	g_tCanTxMsg.Data[0]=array[0];
	g_tCanTxMsg.Data[1]=array[1];
	g_tCanTxMsg.Data[2]=array[2];
	g_tCanTxMsg.Data[3]=array[3];
	g_tCanTxMsg.Data[4]=array[4];
	g_tCanTxMsg.Data[5]=array[5];
	g_tCanTxMsg.Data[6]=array[6];
	g_tCanTxMsg.Data[7]=array[7];
  CAN_Transmit(CAN1, &g_tCanTxMsg);	
}

/*
*********************************************************************************************************
*	�� �� ��: can1_Init
*	����˵��: ����CANӲ��,���ò�����Ϊ1Mbps
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
 void can1_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	/* CAN GPIOs ����*/
	/* ʹ��GPIOʱ�� */
	RCC_AHB1PeriphClockCmd(CAN1_GPIO_TX_CLK|CAN1_GPIO_RX_CLK, ENABLE);

	/* ����ӳ��ΪCAN����  */
	GPIO_PinAFConfig(CAN1_GPIO_RX_PORT, CAN1_RX_SOURCE, CAN1_AF_PORT);
	GPIO_PinAFConfig(CAN1_GPIO_TX_PORT, CAN1_TX_SOURCE, CAN1_AF_PORT); 

	/* ���� CAN RX �� TX ���� */
	GPIO_InitStructure.GPIO_Pin = CAN1_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(CAN1_GPIO_RX_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = CAN1_TX_PIN;
	GPIO_Init(CAN1_GPIO_TX_PORT, &GPIO_InitStructure);

	/* CAN����*/  
	/* ʹ��CANʱ�� */
	RCC_APB1PeriphClockCmd(CAN1_CLK, ENABLE);
	/* ��λCAN�Ĵ��� */
	CAN_DeInit(CAN1);
	
	/*
		TTCM = time triggered communication mode
		ABOM = automatic bus-off management 
		AWUM = automatic wake-up mode
		NART = no automatic retransmission
		RFLM = receive FIFO locked mode 
		TXFP = transmit FIFO priority		
	*/
	CAN_InitStructure.CAN_TTCM = DISABLE;			/* ��ֹʱ�䴥��ģʽ��������ʱ���), T  */
	CAN_InitStructure.CAN_ABOM = DISABLE;			/* ��ֹ�Զ����߹رչ��� */
	CAN_InitStructure.CAN_AWUM = DISABLE;			/* ��ֹ�Զ�����ģʽ */
	CAN_InitStructure.CAN_NART = DISABLE;			/* ��ֹ�ٲö�ʧ��������Զ��ش����� */
	CAN_InitStructure.CAN_RFLM = DISABLE;			/* ��ֹ����FIFO����ģʽ */
	CAN_InitStructure.CAN_TXFP = DISABLE;			/* ��ֹ����FIFO���ȼ� */
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;	/* ����CANΪ��������ģʽ */
	
	/* 
		CAN ������ = RCC_APB1Periph_CAN1 / Prescaler / (SJW + BS1 + BS2);
		
		SJW = synchronisation_jump_width 
		BS = bit_segment
		
		�����У�����CAN������Ϊ1 Mbps		
		CAN ������ = 420000000 / 2 / (1 + 12 + 8) / = 1 Mbps		
	*/
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
	CAN_InitStructure.CAN_Prescaler = 4;
	CAN_Init(CAN1, &CAN_InitStructure);
	
	/* ����CAN ɸѡ��0 */
	CAN_FilterInitStructure.CAN_FilterNumber = 0;		/*  ɸѡ����ţ�0-13����14���˲��� */
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;		/* ɸѡ��ģʽ������ID����ģʽ */
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;	/* 32λɸѡ�� */
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;					/* �����ID�ĸ�16bit */
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;					/* �����ID�ĵ�16bit */
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;				/* ID����ֵ��16bit */
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;				/* ID����ֵ��16bit */
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;		/* ɸѡ����FIFO 0 */
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;				/* ʹ��ɸѡ�� */
	CAN_FilterInit(&CAN_FilterInitStructure);

	/* ��䷢�Ͳ����� Ҳ������ÿ�η��͵�ʱ���� */
//	g_tCanTxMsg.StdId = 0x321;
//	g_tCanTxMsg.ExtId = 0x01;
	g_tCanTxMsg.RTR = CAN_RTR_DATA;
//	g_tCanTxMsg.IDE = CAN_ID_STD;
	g_tCanTxMsg.IDE = CAN_ID_EXT;
	g_tCanTxMsg.DLC = 8;	
}    

/*
*********************************************************************************************************
*	�� �� ��: can2_Init
*	����˵��: ����CANӲ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void can2_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

	/* CAN GPIOs ����*/
	/* ʹ��GPIOʱ�� */
	RCC_AHB1PeriphClockCmd(CAN2_GPIO_CLK, ENABLE);

	/* ����ӳ��ΪCAN����  */
	GPIO_PinAFConfig(CAN2_GPIO_PORT, CAN2_RX_SOURCE, CAN2_AF_PORT);
	GPIO_PinAFConfig(CAN2_GPIO_PORT, CAN2_TX_SOURCE, CAN2_AF_PORT); 

	/* ���� CAN RX �� TX ���� */
	GPIO_InitStructure.GPIO_Pin = CAN2_RX_PIN|CAN2_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(CAN2_GPIO_PORT, &GPIO_InitStructure);

	/* CAN����*/  
	/* ʹ��CANʱ��, ʹ��CAN2�����CAN1��ʱ�� */
	RCC_APB1PeriphClockCmd(CAN2_CLK, ENABLE);

	/* ��λCAN�Ĵ��� */
	CAN_DeInit(CAN2);
	
	/*
		TTCM = time triggered communication mode
		ABOM = automatic bus-off management 
		AWUM = automatic wake-up mode
		NART = no automatic retransmission
		RFLM = receive FIFO locked mode 
		TXFP = transmit FIFO priority		
	*/
	CAN_InitStructure.CAN_TTCM = DISABLE;			/* ��ֹʱ�䴥��ģʽ��������ʱ���), T  */
	CAN_InitStructure.CAN_ABOM = DISABLE;			/* ��ֹ�Զ����߹رչ��� */
	CAN_InitStructure.CAN_AWUM = DISABLE;			/* ��ֹ�Զ�����ģʽ */
	CAN_InitStructure.CAN_NART = DISABLE;			/* ��ֹ�ٲö�ʧ��������Զ��ش����� */
	CAN_InitStructure.CAN_RFLM = DISABLE;			/* ��ֹ����FIFO����ģʽ */
	CAN_InitStructure.CAN_TXFP = DISABLE;			/* ��ֹ����FIFO���ȼ� */
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;	/* ����CANΪ��������ģʽ */
	
	/* 
		CAN ������ = RCC_APB1Periph_CAN1 / Prescaler / (SJW + BS1 + BS2);
		
		SJW = synchronisation_jump_width 
		BS = bit_segment
		
		�����У�����CAN������Ϊ1 Mbps		
		CAN ������ = 420000000 / 2 / (1 + 12 + 8) / = 1 Mbps		
	*/
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
	CAN_InitStructure.CAN_Prescaler = 2;
	CAN_Init(CAN2, &CAN_InitStructure);
	
	/* ����CANɸѡ�����14 ��CAN2ɸѡ����� 14--27����CAN1����0--13*/
	CAN_FilterInitStructure.CAN_FilterNumber = 14;						/* ɸѡ����ţ�14-27����14���˲��� */
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;		/* ɸѡ��ģʽ������ID����ģʽ */
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;	/* 32λɸѡ�� */
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;					/* �����ID�ĸ�16bit */
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;					/* �����ID�ĵ�16bit */
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;				/* ID����ֵ��16bit */
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;				/* ID����ֵ��16bit */
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO1;		/*  ɸѡ����FIFO 1 */
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;				/* ʹ��ɸѡ�� */
	CAN_FilterInit(&CAN_FilterInitStructure);

	/* ��䷢�Ͳ����� Ҳ������ÿ�η��͵�ʱ���� */
//	g_tCanTxMsg.StdId = 0x321;
//	g_tCanTxMsg.ExtId = 0x01;
	g_tCanTxMsg.RTR = CAN_RTR_DATA;
	g_tCanTxMsg.IDE = CAN_ID_EXT;
	g_tCanTxMsg.DLC = 8;
} 

/*
*********************************************************************************************************
*	�� �� ��: can1_NVIC_Config
*	����˵��: ����CAN�ж�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/  
void can1_NVIC_Config(void)
{
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* CAN FIFO0 ��Ϣ�����ж�ʹ�� */ 
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);	
}

/*
*********************************************************************************************************
*	�� �� ��: can2_NVIC_Config
*	����˵��: ����CAN�ж�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/  
void can2_NVIC_Config(void)
{
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* CAN FIFO1 ��Ϣ�����ж�ʹ�� */ 
	CAN_ITConfig(CAN2, CAN_IT_FMP1, ENABLE);	
}

/*
*********************************************************************************************************
*	�� �� ��: CAN1_RX0_IRQHandler
*	����˵��: CAN�жϷ������.
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/ 
void CAN1_RX0_IRQHandler(void)
{
	u8 i;
	
	CAN_Receive(CAN1, CAN_FIFO0, &g_tCanRxMsg);
	
	switch (g_tCanRxMsg.ExtId)
	{
		
		case 0x000203ff:
			PIC_normal_abmormal=g_tCanRxMsg.Data[0];
			break;
		
		case 0x00020101:
			for(i=0;i<8;i++)
			FPGA_data[0][i]=g_tCanRxMsg.Data[i];
			break;
		case 0x00020102:	
			for(i=0;i<8;i++)
			FPGA_data[1][i]=g_tCanRxMsg.Data[i];
			break;
		case 0x00020103:
			for(i=0;i<8;i++)
			FPGA_data[2][i]=g_tCanRxMsg.Data[i];
			break;
		
		case 0x00010000:	
			for(i=0;i<8;i++)
			BL_data[i]=g_tCanRxMsg.Data[i];
			break;
		
		case 0x00020200:
//			FPGA_init_number=((uint16_t)(g_tCanRxMsg.Data[0])<<8)|g_tCanRxMsg.Data[1];
//			if(FPGA_init_number!=0x35f)
//			repeat_send=1;	
			break;
		default:
			break;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: CAN2_RX1_IRQHandler
*	����˵��: CAN�жϷ������.
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/ 
void CAN2_RX1_IRQHandler(void)
{	
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	
	CAN_Receive(CAN2, CAN_FIFO1, &g_tCanRxMsg);
	
	if ((g_tCanRxMsg.StdId == 0x321) && (g_tCanRxMsg.IDE == CAN_ID_STD) && (g_tCanRxMsg.DLC == 1))
	{
		xQueueSendFromISR(xQueue2,
						  (void *)&(g_tCanRxMsg.Data[0]),
						   &xHigherPriorityTaskWoken);
					  
		/* ���xHigherPriorityTaskWoken = pdTRUE����ô�˳��жϺ��е���ǰ������ȼ�����ִ�� */
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}	
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
