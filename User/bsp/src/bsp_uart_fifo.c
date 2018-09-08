/*
*********************************************************************************************************
*
*	ģ������ : �����ж�+FIFO����ģ��
*	�ļ����� : bsp_uart_fifo.c
*	��    �� : V1.0
*	˵    �� : ���ô����ж�+FIFOģʽʵ�ֶ�����ڵ�ͬʱ����
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		V1.0    2013-02-01 armfly  ��ʽ����
*		V1.1    2013-06-09 armfly  FiFo�ṹ����TxCount��Ա�����������жϻ�������; ���� ��FiFo�ĺ���
*		V1.2	2014-09-29 armfly  ����RS485 MODBUS�ӿڡ����յ����ֽں�ֱ��ִ�лص�������
*		V1.3	2015-07-23 armfly  ���� UART_T �ṹ�Ķ�дָ�뼸����Ա������������ __IO ����,�����Ż���
*					�ᵼ�´��ڷ��ͺ���������
*		V1.4	2015-08-04 armfly  ���UART4����bug  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART1);
*		V1.5	2015-10-08 armfly  �����޸Ĳ����ʵĽӿں���
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "includes.h"
#include "demo_fatfs.h"
#include "can_network.h"

//int8_t picture_num=0;
uint8_t load_flag = 0;
static uint8_t pro_num=0;
extern CONFIG_PARA cfg_para;
extern char cfgname[40][16];
extern double corretPara[52];
extern struct _CONFIG_CHANNEL_ cfg_ch_tmp;
extern struct _CONFIG_DATA cfg_data_tmp;
extern uint8_t BL_data[8];
extern char flag_bl;
extern CONFIG_PIC  picArray;
extern TaskHandle_t xHandleTaskGUI;

/* ����1��GPIO --- RS323 */
#define UART1_TX_PORT      GPIOA
#define UART1_TX_PIN       GPIO_Pin_9
#define UART1_TX_CLK       RCC_AHB1Periph_GPIOA
#define UART1_TX_SOURCE    GPIO_PinSource9

#define UART1_RX_PORT      GPIOA
#define UART1_RX_PIN       GPIO_Pin_10
#define UART1_RX_CLK       RCC_AHB1Periph_GPIOA
#define UART1_RX_SOURCE    GPIO_PinSource10

/* ����2��GPIO  --- ֻ����RX�� û��TX��Ӳ��û�нӣ�����0ŷķ���ϣ� */
#define UART2_TX_PORT      GPIOA
#define UART2_TX_PIN       GPIO_Pin_2
#define UART2_TX_CLK       RCC_AHB1Periph_GPIOA
#define UART2_TX_SOURCE    GPIO_PinSource2

#define UART2_RX_PORT      GPIOA
#define UART2_RX_PIN       GPIO_Pin_3
#define UART2_RX_CLK       RCC_AHB1Periph_GPIOA
#define UART2_RX_SOURCE    GPIO_PinSource3

/* ����3��GPIO - RS485 */
#define UART3_TX_PORT      GPIOB
#define UART3_TX_PIN       GPIO_Pin_10
#define UART3_TX_CLK       RCC_AHB1Periph_GPIOB
#define UART3_TX_SOURCE    GPIO_PinSource10

#define UART3_RX_PORT      GPIOCB
#define UART3_RX_PIN       GPIO_Pin_11
#define UART3_RX_CLK       RCC_AHB1Periph_GPIOB
#define UART3_RX_SOURCE    GPIO_PinSource11

/* ����4��GPIO - ��SDIO�ӿ� */
#define UART4_TX_PORT      GPIOC
#define UART4_TX_PIN       GPIO_Pin_10
#define UART4_TX_CLK       RCC_AHB1Periph_GPIOC
#define UART4_TX_SOURCE    GPIO_PinSource10

#define UART4_RX_PORT      GPIOC
#define UART4_RX_PIN       GPIO_Pin_11
#define UART4_RX_CLK       RCC_AHB1Periph_GPIOC
#define UART4_RX_SOURCE    GPIO_PinSource11

/* ����5��GPIO - ��SDIO�ӿ� */
#define UART5_TX_PORT      GPIOC
#define UART5_TX_PIN       GPIO_Pin_12
#define UART5_TX_CLK       RCC_AHB1Periph_GPIOC
#define UART5_TX_SOURCE    GPIO_PinSource12

#define UART5_RX_PORT      GPIOD
#define UART5_RX_PIN       GPIO_Pin_2
#define UART5_RX_CLK       RCC_AHB1Periph_GPIOD
#define UART5_RX_SOURCE    GPIO_PinSource2

/* ����6��GPIO -- GPRS,WIFI */
#define UART6_TX_PORT      GPIOG
#define UART6_TX_PIN       GPIO_Pin_14
#define UART6_TX_CLK       RCC_AHB1Periph_GPIOG
#define UART6_TX_SOURCE    GPIO_PinSource14

#define UART6_RX_PORT      GPIOC
#define UART6_RX_PIN       GPIO_Pin_7
#define UART6_RX_CLK       RCC_AHB1Periph_GPIOC
#define UART6_RX_SOURCE    GPIO_PinSource7

/* ����ÿ�����ڽṹ����� */
#if UART1_FIFO_EN == 1
	static UART_T g_tUart1;
	static uint8_t g_TxBuf1[UART1_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf1[UART1_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART2_FIFO_EN == 1
	static UART_T g_tUart2;
	static uint8_t g_TxBuf2[UART2_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf2[UART2_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART3_FIFO_EN == 1
	static UART_T g_tUart3;
	static uint8_t g_TxBuf3[UART3_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf3[UART3_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART4_FIFO_EN == 1
	static UART_T g_tUart4;
	static uint8_t g_TxBuf4[UART4_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf4[UART4_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART5_FIFO_EN == 1
	static UART_T g_tUart5;
	static uint8_t g_TxBuf5[UART5_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf5[UART5_RX_BUF_SIZE];		/* ���ջ����� */
#endif

#if UART6_FIFO_EN == 1
	static UART_T g_tUart6;
	static uint8_t g_TxBuf6[UART6_TX_BUF_SIZE];		/* ���ͻ����� */
	static uint8_t g_RxBuf6[UART6_RX_BUF_SIZE];		/* ���ջ����� */
#endif

static void UartVarInit(void);

static void InitHardUart(void);
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen);
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte);
static void UartIRQ(UART_T *_pUart);
static void ConfigUartNVIC(void);

void RS485_InitTXE(void);

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitUart
*	����˵��: ��ʼ������Ӳ��������ȫ�ֱ�������ֵ.
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitUart(void)
{
	UartVarInit();		/* �����ȳ�ʼ��ȫ�ֱ���,������Ӳ�� */

	InitHardUart();		/* ���ô��ڵ�Ӳ������(�����ʵ�) */

//	RS485_InitTXE();	/* ����RS485оƬ�ķ���ʹ��Ӳ��������Ϊ������� */

	ConfigUartNVIC();	/* ���ô����ж� */
}

/*
*********************************************************************************************************
*	�� �� ��: ComToUart
*	����˵��: ��COM�˿ں�ת��ΪUARTָ��
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*	�� �� ֵ: uartָ��
*********************************************************************************************************
*/
UART_T *ComToUart(COM_PORT_E _ucPort)
{
	if (_ucPort == COM1)
	{
		#if UART1_FIFO_EN == 1
			return &g_tUart1;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM2)
	{
		#if UART2_FIFO_EN == 1
			return &g_tUart2;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM3)
	{
		#if UART3_FIFO_EN == 1
			return &g_tUart3;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM4)
	{
		#if UART4_FIFO_EN == 1
			return &g_tUart4;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM5)
	{
		#if UART5_FIFO_EN == 1
			return &g_tUart5;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM6)
	{
		#if UART6_FIFO_EN == 1
			return &g_tUart6;
		#else
			return 0;
		#endif
	}
	else
	{
		/* �����κδ��� */
		return 0;
	}
}


/*
*********************************************************************************************************
*	�� �� ��: ComToUart
*	����˵��: ��COM�˿ں�ת��Ϊ USART_TypeDef* USARTx
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*	�� �� ֵ: USART_TypeDef*,  USART1, USART2, USART3, UART4, UART5
*********************************************************************************************************
*/
USART_TypeDef *ComToUSARTx(COM_PORT_E _ucPort)
{
	if (_ucPort == COM1)
	{
		#if UART1_FIFO_EN == 1
			return USART1;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM2)
	{
		#if UART2_FIFO_EN == 1
			return USART2;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM3)
	{
		#if UART3_FIFO_EN == 1
			return USART3;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM4)
	{
		#if UART4_FIFO_EN == 1
			return USART4;
		#else
			return 0;
		#endif
	}
	else if (_ucPort == COM5)
	{
		#if UART5_FIFO_EN == 1
			return USART5;
		#else
			return 0;
		#endif
	}
	else
	{
		/* �����κδ��� */
		return 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: comSendBuf
*	����˵��: �򴮿ڷ���һ�����ݡ����ݷŵ����ͻ��������������أ����жϷ�������ں�̨��ɷ���
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*			  _ucaBuf: �����͵����ݻ�����
*			  _usLen : ���ݳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	if (pUart->SendBefor != 0)
	{
		pUart->SendBefor();		/* �����RS485ͨ�ţ���������������н�RS485����Ϊ����ģʽ */
	}

	UartSend(pUart, _ucaBuf, _usLen);
}

/*
*********************************************************************************************************
*	�� �� ��: comSendChar
*	����˵��: �򴮿ڷ���1���ֽڡ����ݷŵ����ͻ��������������أ����жϷ�������ں�̨��ɷ���
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*			  _ucByte: �����͵�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte)
{
	comSendBuf(_ucPort, &_ucByte, 1);
}

/*
*********************************************************************************************************
*	�� �� ��: comGetChar
*	����˵��: �ӽ��ջ�������ȡ1�ֽڣ��������������������ݾ��������ء�
*	��    ��: _ucPort: �˿ں�(COM1 - COM5)
*			  _pByte: ���յ������ݴ���������ַ
*	�� �� ֵ: 0 ��ʾ������, 1 ��ʾ��ȡ����Ч�ֽ�
*********************************************************************************************************
*/
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return 0;
	}

	return UartGetChar(pUart, _pByte);
}

/*
*********************************************************************************************************
*	�� �� ��: comClearTxFifo
*	����˵��: ���㴮�ڷ��ͻ�����
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void comClearTxFifo(COM_PORT_E _ucPort)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	pUart->usTxWrite = 0;
	pUart->usTxRead = 0;
	pUart->usTxCount = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: comClearRxFifo
*	����˵��: ���㴮�ڽ��ջ�����
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void comClearRxFifo(COM_PORT_E _ucPort)
{
	UART_T *pUart;

	pUart = ComToUart(_ucPort);
	if (pUart == 0)
	{
		return;
	}

	pUart->usRxWrite = 0;
	pUart->usRxRead = 0;
	pUart->usRxCount = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: comSetBaud
*	����˵��: ���ô��ڵĲ�����
*	��    ��: _ucPort: �˿ں�(COM1 - COM5)
*			  _BaudRate: �����ʣ�0-4500000�� ���4.5Mbps
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void comSetBaud(COM_PORT_E _ucPort, uint32_t _BaudRate)
{
	USART_TypeDef* USARTx;
	
	USARTx = ComToUSARTx(_ucPort);
	if (USARTx == 0)
	{
		return;
	}
	
	USART_SetBaudRate(USARTx, _BaudRate);
}

/*
*********************************************************************************************************
*	�� �� ��: USART_SetBaudRate
*	����˵��: �޸Ĳ����ʼĴ������������������á����ʹ�� USART_Init����, ����޸�Ӳ�����ز�����RX,TX����
*			  ���ݹ̼����� USART_Init���������������ò����ʵĲ��ֵ����������װΪһ������
*	��    ��: USARTx : USART1, USART2, USART3, UART4, UART5
*			  BaudRate : �����ʣ�ȡֵ 0 - 4500000
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void USART_SetBaudRate(USART_TypeDef* USARTx, uint32_t BaudRate)
{
	uint32_t tmpreg = 0x00, apbclock = 0x00;
	uint32_t integerdivider = 0x00;
	uint32_t fractionaldivider = 0x00;
	RCC_ClocksTypeDef RCC_ClocksStatus;

	/* Check the parameters */
	assert_param(IS_USART_ALL_PERIPH(USARTx));
	assert_param(IS_USART_BAUDRATE(BaudRate));  

	/*---------------------------- USART BRR Configuration -----------------------*/
	/* Configure the USART Baud Rate */
	RCC_GetClocksFreq(&RCC_ClocksStatus);

	if ((USARTx == USART1) || (USARTx == USART6))
	{
		apbclock = RCC_ClocksStatus.PCLK2_Frequency;
	}
	else
	{
		apbclock = RCC_ClocksStatus.PCLK1_Frequency;
	}

	/* Determine the integer part */
	if ((USARTx->CR1 & USART_CR1_OVER8) != 0)
	{
		/* Integer part computing in case Oversampling mode is 8 Samples */
		integerdivider = ((25 * apbclock) / (2 * (BaudRate)));    
	}
	else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
	{
		/* Integer part computing in case Oversampling mode is 16 Samples */
		integerdivider = ((25 * apbclock) / (4 * (BaudRate)));    
	}
	tmpreg = (integerdivider / 100) << 4;

	/* Determine the fractional part */
	fractionaldivider = integerdivider - (100 * (tmpreg >> 4));

	/* Implement the fractional part in the register */
	if ((USARTx->CR1 & USART_CR1_OVER8) != 0)
	{
		tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
	}
	else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
	{
		tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
	}

	/* Write to USART BRR register */
	USARTx->BRR = (uint16_t)tmpreg;
}

/* �����RS485ͨ�ţ��밴���¸�ʽ��д������ ���ǽ����� USART3��ΪRS485������ */

/*
*********************************************************************************************************
*	�� �� ��: RS485_InitTXE
*	����˵��: ����RS485����ʹ�ܿ��� TXE
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RS485_InitTXE(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_RS485_TXEN, ENABLE);	/* ��GPIOʱ�� */

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* ��Ϊ����� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ���� */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* ���������� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */
	GPIO_InitStructure.GPIO_Pin = PIN_RS485_TXEN;
	GPIO_Init(PORT_RS485_TXEN, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: RS485_SetBaud
*	����˵��: �޸�485���ڵĲ����ʡ�
*	��    ��: _baud : ������.0-4500000
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RS485_SetBaud(uint32_t _baud)
{
	comSetBaud(COM3, _baud);
}

/*
*********************************************************************************************************
*	�� �� ��: RS485_SendBefor
*	����˵��: ��������ǰ��׼������������RS485ͨ�ţ�������RS485оƬΪ����״̬��
*			  ���޸� UartVarInit()�еĺ���ָ����ڱ������������� g_tUart2.SendBefor = RS485_SendBefor
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RS485_SendBefor(void)
{
	RS485_TX_EN();	/* �л�RS485�շ�оƬΪ����ģʽ */
}

/*
*********************************************************************************************************
*	�� �� ��: RS485_SendOver
*	����˵��: ����һ�����ݽ�������ƺ�������RS485ͨ�ţ�������RS485оƬΪ����״̬��
*			  ���޸� UartVarInit()�еĺ���ָ����ڱ������������� g_tUart2.SendOver = RS485_SendOver
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RS485_SendOver(void)
{
	RS485_RX_EN();	/* �л�RS485�շ�оƬΪ����ģʽ */
}


/*
*********************************************************************************************************
*	�� �� ��: RS485_SendBuf
*	����˵��: ͨ��RS485оƬ����һ�����ݡ�ע�⣬���������ȴ�������ϡ�
*	��    ��: _ucaBuf : ���ݻ�����
*			  _usLen : ���ݳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen)
{
	comSendBuf(COM3, _ucaBuf, _usLen);
}


/*
*********************************************************************************************************
*	�� �� ��: RS485_SendStr
*	����˵��: ��485���߷���һ���ַ�����0������
*	��    ��: _pBuf �ַ�����0����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void RS485_SendStr(char *_pBuf)
{
	RS485_SendBuf((uint8_t *)_pBuf, strlen(_pBuf));
}

/*
*********************************************************************************************************
*	�� �� ��: RS485_ReciveNew
*	����˵��: ���յ��µ�����
*	��    ��: _byte ���յ���������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//extern void MODBUS_ReciveNew(uint8_t _byte);
void RS485_ReciveNew(uint8_t _byte)
{
//	MODBUS_ReciveNew(_byte);
}

/*
*********************************************************************************************************
*	�� �� ��: UartVarInit
*	����˵��: ��ʼ��������صı���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UartVarInit(void)
{
#if UART1_FIFO_EN == 1
	g_tUart1.uart = USART1;						/* STM32 �����豸 */
	g_tUart1.pTxBuf = g_TxBuf1;					/* ���ͻ�����ָ�� */
	g_tUart1.pRxBuf = g_RxBuf1;					/* ���ջ�����ָ�� */
	g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart1.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart1.usTxRead = 0;						/* ����FIFO������ */
	g_tUart1.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart1.usRxRead = 0;						/* ����FIFO������ */
	g_tUart1.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart1.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart1.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	g_tUart1.SendOver = 0;						/* ������Ϻ�Ļص����� */
	g_tUart1.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
#endif

#if UART2_FIFO_EN == 1
	g_tUart2.uart = USART2;						/* STM32 �����豸 */
	g_tUart2.pTxBuf = g_TxBuf2;					/* ���ͻ�����ָ�� */
	g_tUart2.pRxBuf = g_RxBuf2;					/* ���ջ�����ָ�� */
	g_tUart2.usTxBufSize = UART2_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart2.usRxBufSize = UART2_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart2.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart2.usTxRead = 0;						/* ����FIFO������ */
	g_tUart2.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart2.usRxRead = 0;						/* ����FIFO������ */
	g_tUart2.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart2.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart2.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	g_tUart2.SendOver = 0;						/* ������Ϻ�Ļص����� */
	g_tUart2.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
#endif

#if UART3_FIFO_EN == 1
	g_tUart3.uart = USART3;						/* STM32 �����豸 */
	g_tUart3.pTxBuf = g_TxBuf3;					/* ���ͻ�����ָ�� */
	g_tUart3.pRxBuf = g_RxBuf3;					/* ���ջ�����ָ�� */
	g_tUart3.usTxBufSize = UART3_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart3.usRxBufSize = UART3_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart3.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart3.usTxRead = 0;						/* ����FIFO������ */
	g_tUart3.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart3.usRxRead = 0;						/* ����FIFO������ */
	g_tUart3.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart3.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart3.SendBefor = RS485_SendBefor;		/* ��������ǰ�Ļص����� */
	g_tUart3.SendOver = RS485_SendOver;			/* ������Ϻ�Ļص����� */
	g_tUart3.ReciveNew = RS485_ReciveNew;		/* ���յ������ݺ�Ļص����� */
#endif

#if UART4_FIFO_EN == 1
	g_tUart4.uart = UART4;						/* STM32 �����豸 */
	g_tUart4.pTxBuf = g_TxBuf4;					/* ���ͻ�����ָ�� */
	g_tUart4.pRxBuf = g_RxBuf4;					/* ���ջ�����ָ�� */
	g_tUart4.usTxBufSize = UART4_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart4.usRxBufSize = UART4_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart4.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart4.usTxRead = 0;						/* ����FIFO������ */
	g_tUart4.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart4.usRxRead = 0;						/* ����FIFO������ */
	g_tUart4.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart4.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart4.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	g_tUart4.SendOver = 0;						/* ������Ϻ�Ļص����� */
	g_tUart4.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
#endif

#if UART5_FIFO_EN == 1
	g_tUart5.uart = UART5;						/* STM32 �����豸 */
	g_tUart5.pTxBuf = g_TxBuf5;					/* ���ͻ�����ָ�� */
	g_tUart5.pRxBuf = g_RxBuf5;					/* ���ջ�����ָ�� */
	g_tUart5.usTxBufSize = UART5_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart5.usRxBufSize = UART5_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart5.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart5.usTxRead = 0;						/* ����FIFO������ */
	g_tUart5.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart5.usRxRead = 0;						/* ����FIFO������ */
	g_tUart5.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart5.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart5.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	g_tUart5.SendOver = 0;						/* ������Ϻ�Ļص����� */
	g_tUart5.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
#endif


#if UART6_FIFO_EN == 1
	g_tUart6.uart = USART6;						/* STM32 �����豸 */
	g_tUart6.pTxBuf = g_TxBuf6;					/* ���ͻ�����ָ�� */
	g_tUart6.pRxBuf = g_RxBuf6;					/* ���ջ�����ָ�� */
	g_tUart6.usTxBufSize = UART6_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart6.usRxBufSize = UART6_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart6.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart6.usTxRead = 0;						/* ����FIFO������ */
	g_tUart6.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart6.usRxRead = 0;						/* ����FIFO������ */
	g_tUart6.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart6.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart6.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	g_tUart6.SendOver = 0;						/* ������Ϻ�Ļص����� */
	g_tUart6.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */
#endif
}

/*
*********************************************************************************************************
*	�� �� ��: InitHardUart
*	����˵��: ���ô��ڵ�Ӳ�������������ʣ�����λ��ֹͣλ����ʼλ��У��λ���ж�ʹ�ܣ��ʺ���STM32-F4������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void InitHardUart(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

#if UART1_FIFO_EN == 1		/* ����1 TX = PA9   RX = PA10 �� TX = PB6   RX = PB7*/

	/* ��1���� ����GPIO */
	#if 1	/* TX = PA9   RX = PA10 */
		/* �� GPIO ʱ�� */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

		/* �� UART ʱ�� */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

		/* �� PA9 ӳ��Ϊ USART1_TX */
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);

		/* �� PA10 ӳ��Ϊ USART1_RX */
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

		/* ���� USART Tx Ϊ���ù��� */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* ���� USART Rx Ϊ���ù��� */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	#else	/* TX = PB6   RX = PB7  */
		/* �� GPIO ʱ�� */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

		/* �� UART ʱ�� */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

		/* �� PB6 ӳ��Ϊ USART1_TX */
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);

		/* �� PB7 ӳ��Ϊ USART1_RX */
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

		/* ���� USART Tx Ϊ���ù��� */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		/* ���� USART Rx Ϊ���ù��� */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	#endif

	/* ��2���� ���ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = UART1_BAUD;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������
	*/
	USART_Cmd(USART1, ENABLE);		/* ʹ�ܴ��� */

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
#endif

#if UART2_FIFO_EN == 1		/* ����2 TX = PD5   RX = PD6 ��  TX = PA2�� RX = PA3  */
	/* ��1���� ����GPIO */
	#if 0	/* ����2 TX = PD5   RX = PD6 */
		/* �� GPIO ʱ�� */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

		/* �� UART ʱ�� */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

		/* �� PD5 ӳ��Ϊ USART2_TX */
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);

		/* �� PD6 ӳ��Ϊ USART2_RX */
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);

		/* ���� USART Tx Ϊ���ù��� */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOD, &GPIO_InitStructure);

		/* ���� USART Rx Ϊ���ù��� */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_Init(GPIOD, &GPIO_InitStructure);

	#else	/* ����2   TX = PA2�� RX = PA3 */
		/* �� GPIO ʱ�� */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

		/* �� UART ʱ�� */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

		/* �� PA2 ӳ��Ϊ USART2_TX. ��STM32-V5���У�PA2 �ܽ�������̫�� */
		//GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);

		/* �� PA3 ӳ��Ϊ USART2_RX */
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

		/* ���� USART Tx Ϊ���ù��� */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */

		//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		//GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* ���� USART Rx Ϊ���ù��� */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	#endif

	/* ��2���� ���ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = UART2_BAUD;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx;		/* ��ѡ�����ģʽ */
	USART_Init(USART2, &USART_InitStructure);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������
	*/
	USART_Cmd(USART2, ENABLE);		/* ʹ�ܴ��� */

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART2, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
#endif

#if UART3_FIFO_EN == 1			/* ����3 TX = PB10   RX = PB11 */

	/* ���� PB2Ϊ��������������л� RS485оƬ���շ�״̬ */
	{
		RCC_AHB1PeriphClockCmd(RCC_RS485_TXEN, ENABLE);

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		/* ��Ϊ����� */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		/* ��Ϊ����ģʽ */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;	/* ���������費ʹ�� */
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	/* IO������ٶ� */

		GPIO_InitStructure.GPIO_Pin = PIN_RS485_TXEN;
		GPIO_Init(PORT_RS485_TXEN, &GPIO_InitStructure);
	}

	/* �� GPIO ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* �� UART ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* �� PB10 ӳ��Ϊ USART3_TX */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);

	/* �� PB11 ӳ��Ϊ USART3_RX */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);

	/* ���� USART Tx Ϊ���ù��� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* ���� USART Rx Ϊ���ù��� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* ��2���� ���ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = UART3_BAUD;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������
	*/
	USART_Cmd(USART3, ENABLE);		/* ʹ�ܴ��� */

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART3, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
#endif

#if UART4_FIFO_EN == 1			/* ����4 TX = PC10   RX = PC11 */
	/* ��1���� ����GPIO */

	/* �� GPIO ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/* �� UART ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

	/* �� PC10 ӳ��Ϊ UART4_TX */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART1);

	/* �� PC11 ӳ��Ϊ UART4_RX */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART1);

	/* ���� USART Tx Ϊ���ù��� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* ���� USART Rx Ϊ���ù��� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* ��2���� ���ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = UART1_BAUD;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure);

	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������
	*/
	USART_Cmd(UART4, ENABLE);		/* ʹ�ܴ��� */

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(UART4, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
#endif

#if UART5_FIFO_EN == 1			/* ����5 TX = PC12   RX = PD2 */
	/* ��1���� ����GPIO */

	/* �� GPIO ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOD, ENABLE);

	/* �� UART ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

	/* �� PC12 ӳ��Ϊ UART5_TX */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);

	/* �� PD2 ӳ��Ϊ UART5_RX */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);

	/* ���� UART Tx Ϊ���ù��� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* ���� UART Rx Ϊ���ù��� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* ��2���� ���ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = UART5_BAUD;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);

	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������
	*/
	USART_Cmd(UART5, ENABLE);		/* ʹ�ܴ��� */

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(UART5, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
#endif

#if UART6_FIFO_EN == 1			/* PG14/USART6_TX , PC7/USART6_RX,PG8/USART6_RTS, PG15/USART6_CTS */
	/* ��1���� ����GPIO */

	/* �� GPIO ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOG, ENABLE);

	/* �� UART ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

	/* �� PG14 ӳ��Ϊ USART6_TX */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);

	/* �� PC7 ӳ��Ϊ USART6_RX */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

	/* �� PG8 ӳ��Ϊ USART6_RTS */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource8, GPIO_AF_USART6);

	/* �� PG15 ӳ��Ϊ USART6_CTS */
	GPIO_PinAFConfig(GPIOG, GPIO_PinSource15, GPIO_AF_USART6);

	/* ���� PG14/USART6_TX Ϊ���ù��� */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	/* ���� PC7/USART6_RX Ϊ���ù��� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* ���� PG8/USART6_RTS Ϊ���ù��� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	/* ���� PG15/USART6_CTS Ϊ���ù��� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	/* ��2���� ���ô���Ӳ������ */
	USART_InitStructure.USART_BaudRate = UART6_BAUD;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	//USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;	/* ѡ��Ӳ������ */
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	/* ��ҪӲ������ */
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART6, &USART_InitStructure);

	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������
	*/
	USART_Cmd(USART6, ENABLE);		/* ʹ�ܴ��� */

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
	USART_ClearFlag(USART6, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
#endif
}

/*
*********************************************************************************************************
*	�� �� ��: ConfigUartNVIC
*	����˵��: ���ô���Ӳ���ж�.
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void ConfigUartNVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */
	/*	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  --- �� bsp.c �� bsp_Init() �������ж����ȼ��� */

#if UART1_FIFO_EN == 1
	/* ʹ�ܴ���1�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#if UART2_FIFO_EN == 1
	/* ʹ�ܴ���2�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#if UART3_FIFO_EN == 1
	/* ʹ�ܴ���3�ж�t */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#if UART4_FIFO_EN == 1
	/* ʹ�ܴ���4�ж�t */
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#if UART5_FIFO_EN == 1
	/* ʹ�ܴ���5�ж�t */
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#if UART6_FIFO_EN == 1
	/* ʹ�ܴ���6�ж�t */
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
}

/*
*********************************************************************************************************
*	�� �� ��: UartSend
*	����˵��: ��д���ݵ�UART���ͻ�����,�����������жϡ��жϴ�����������Ϻ��Զ��رշ����ж�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UartSend(UART_T *_pUart, uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint16_t i;

	for (i = 0; i < _usLen; i++)
	{
		/* ������ͻ������Ѿ����ˣ���ȴ��������� */
	#if 0
		/*
			�ڵ���GPRS����ʱ������Ĵ������������while ��ѭ��
			ԭ�� ���͵�1���ֽ�ʱ _pUart->usTxWrite = 1��_pUart->usTxRead = 0;
			������while(1) �޷��˳�
		*/
		while (1)
		{
			uint16_t usRead;

			DISABLE_INT();
			usRead = _pUart->usTxRead;
			ENABLE_INT();

			if (++usRead >= _pUart->usTxBufSize)
			{
				usRead = 0;
			}

			if (usRead != _pUart->usTxWrite)
			{
				break;
			}
		}
	#else
		/* �� _pUart->usTxBufSize == 1 ʱ, ����ĺ���������(������) */
		while (1)
		{
			__IO uint16_t usCount;

			DISABLE_INT();
			usCount = _pUart->usTxCount;
			ENABLE_INT();

			if (usCount < _pUart->usTxBufSize)
			{
				break;
			}
		}
	#endif

		/* �����������뷢�ͻ����� */
		_pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];

		DISABLE_INT();
		if (++_pUart->usTxWrite >= _pUart->usTxBufSize)
		{
			_pUart->usTxWrite = 0;
		}
		_pUart->usTxCount++;
		ENABLE_INT();
	}

	USART_ITConfig(_pUart->uart, USART_IT_TXE, ENABLE);
}

/*
*********************************************************************************************************
*	�� �� ��: UartGetChar
*	����˵��: �Ӵ��ڽ��ջ�������ȡ1�ֽ����� ��������������ã�
*	��    ��: _pUart : �����豸
*			  _pByte : ��Ŷ�ȡ���ݵ�ָ��
*	�� �� ֵ: 0 ��ʾ������  1��ʾ��ȡ������
*********************************************************************************************************
*/
static uint8_t UartGetChar(UART_T *_pUart, uint8_t *_pByte)
{
	uint16_t usCount;

	/* usRxWrite �������жϺ����б���д���������ȡ�ñ���ʱ����������ٽ������� */
	DISABLE_INT();
	usCount = _pUart->usRxCount;
	ENABLE_INT();

	/* �������д������ͬ���򷵻�0 */
	//if (_pUart->usRxRead == usRxWrite)
	if (usCount == 0)	/* �Ѿ�û������ */
	{
		return 0;
	}
	else
	{
		*_pByte = _pUart->pRxBuf[_pUart->usRxRead];		/* �Ӵ��ڽ���FIFOȡ1������ */

		/* ��дFIFO������ */
		DISABLE_INT();
		if (++_pUart->usRxRead >= _pUart->usRxBufSize)
		{
			_pUart->usRxRead = 0;
		}
		_pUart->usRxCount--;
		ENABLE_INT();
		return 1;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: UartIRQ
*	����˵��: ���жϷ��������ã�ͨ�ô����жϴ�����
*	��    ��: _pUart : �����豸
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UartIRQ(UART_T *_pUart)
{
	/* ��������ж�  */
	if (USART_GetITStatus(_pUart->uart, USART_IT_RXNE) != RESET)
	{
		/* �Ӵ��ڽ������ݼĴ�����ȡ���ݴ�ŵ�����FIFO */
		uint8_t ch;
		
		ch = USART_ReceiveData(_pUart->uart);
		_pUart->pRxBuf[_pUart->usRxWrite] = ch;
		if (++_pUart->usRxWrite >= _pUart->usRxBufSize)
		{
			_pUart->usRxWrite = 0;
		}
		if (_pUart->usRxCount < _pUart->usRxBufSize)
		{
			_pUart->usRxCount++;
		}

		/* �ص�����,֪ͨӦ�ó����յ�������,һ���Ƿ���1����Ϣ��������һ����� */
		//if (_pUart->usRxWrite == _pUart->usRxRead)
		//if (_pUart->usRxCount == 1)
		{
			if (_pUart->ReciveNew)
			{
				_pUart->ReciveNew(ch);
			}
		}
	}

	/* �����ͻ��������ж� */
	if (USART_GetITStatus(_pUart->uart, USART_IT_TXE) != RESET)
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_pUart->usTxCount == 0)
		{
			/* ���ͻ�������������ȡ��ʱ�� ��ֹ���ͻ��������ж� ��ע�⣺��ʱ���1�����ݻ�δ����������ϣ�*/
			USART_ITConfig(_pUart->uart, USART_IT_TXE, DISABLE);

			/* ʹ�����ݷ�������ж� */
			USART_ITConfig(_pUart->uart, USART_IT_TC, ENABLE);
		}
		else
		{
			/* �ӷ���FIFOȡ1���ֽ�д�봮�ڷ������ݼĴ��� */
			USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			if (++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}

	}
	/* ����bitλȫ��������ϵ��ж� */
	else if (USART_GetITStatus(_pUart->uart, USART_IT_TC) != RESET)
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_pUart->usTxCount == 0)
		{
			/* �������FIFO������ȫ��������ϣ���ֹ���ݷ�������ж� */
			USART_ITConfig(_pUart->uart, USART_IT_TC, DISABLE);

			/* �ص�����, һ����������RS485ͨ�ţ���RS485оƬ����Ϊ����ģʽ��������ռ���� */
			if (_pUart->SendOver)
			{
				_pUart->SendOver();
			}
		}
		else
		{
			/* ��������£��������˷�֧ */

			/* �������FIFO�����ݻ�δ��ϣ���ӷ���FIFOȡ1������д�뷢�����ݼĴ��� */
			USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			if (++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: USART1_IRQHandler  USART2_IRQHandler USART3_IRQHandler UART4_IRQHandler UART5_IRQHandler
*	����˵��: USART�жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if UART1_FIFO_EN == 1
void USART1_IRQHandler(void)
{
	UartIRQ(&g_tUart1);
}
#endif

#if UART2_FIFO_EN == 1
void USART2_IRQHandler(void)
{
	UartIRQ(&g_tUart2);
}
#endif

#if UART3_FIFO_EN == 1
void USART3_IRQHandler(void)
{
	UartIRQ(&g_tUart3);
}
#endif

#if UART4_FIFO_EN == 1
void UART4_IRQHandler(void)
{
	UartIRQ(&g_tUart4);
}
#endif

#if UART5_FIFO_EN == 1
void UART5_IRQHandler(void)
{
	UartIRQ(&g_tUart5);
}
#endif

#if UART6_FIFO_EN == 1
void USART6_IRQHandler(void)
{
	UartIRQ(&g_tUart6);
}
#endif

/*
*********************************************************************************************************
*	�� �� ��: fputc
*	����˵��: �ض���putc��������������ʹ��printf�����Ӵ���1��ӡ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int fputc(int ch, FILE *f)
{
#if 0	/* ����Ҫprintf���ַ�ͨ�������ж�FIFO���ͳ�ȥ��printf�������������� */
	comSendChar(COM1, ch);

	return ch;
#else	/* ����������ʽ����ÿ���ַ�,�ȴ����ݷ������ */
	/* дһ���ֽڵ�USART1 */
	USART_SendData(USART1, (uint8_t) ch);

	/* �ȴ����ͽ��� */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}

	return ch;
#endif
}

/*
*********************************************************************************************************
*	�� �� ��: fgetc
*	����˵��: �ض���getc��������������ʹ��getchar�����Ӵ���1��������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int fgetc(FILE *f)
{

#if 1	/* �Ӵ��ڽ���FIFO��ȡ1������, ֻ��ȡ�����ݲŷ��� */
	uint8_t ucData;

	while(comGetChar(COM1, &ucData) == 0);

	return ucData;
#else
	/* �ȴ�����1�������� */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

	return (int)USART_ReceiveData(USART1);
#endif
}

void sendSigData(char picNum)
{
//	#define deal_num(x)		(2 - x / 16.7647)/4.096*1024
	#define deal_sig_num(x,K,B)		(2 - (x-B) / 16.7647/K)/4.096*1024
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
						datatmp2 = deal_sig_num(voltage,corretPara[(i-24)*2+24],corretPara[(i-24)*2+1+24]);
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
						datatmp2 = deal_sig_num(voltage,corretPara[(i-24)*2+24],corretPara[(i-24)*2+1+24]);
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
//				vTaskDelay(1);
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
//				vTaskDelay(1);
				if( *((unsigned int *)(sendbuf + 4)) == 0)
					break;
			}
		}
	 }
}

void sendToBl(void)
{	
	int BL_ID  = 0x01010000;
	int BL_EN  = 0x01030000;
	unsigned char sendbuf[8]={0};
	
	sendbuf[0] = (cfg_para.voltage/10)>>8;
	sendbuf[1] = (cfg_para.voltage/10) & 0xff;
	sendbuf[2] = (cfg_para.current/100)>>8;
	sendbuf[3] = (cfg_para.current/100) & 0xff;
	sendbuf[4] = (cfg_para.current/100 +150) >> 8;
	sendbuf[5] = (cfg_para.current/100 +150) & 0xff;
	
	CAN_SendMessage(BL_ID,sendbuf);
//	vTaskDelay(5);
	
	memset(sendbuf,0,8);
	sendbuf[0] = 0x01;
	CAN_SendMessage(BL_EN,sendbuf);
//	vTaskDelay(5);
}

void sendToFPGA(char picNum)
{
//	#define deal_num(x) (2 - ((x / 100.0) - 31) /10.0) * 1024 / 4.096
	#define deal_num(x,K,B) (2 - (((x / 100.0) - 31) - B) / 10.0/K ) * 1024 / 4.096
	int ZU_ID1 = 0x02000000;
	int ZU_ID2 = 0x02000001;
	int ZU_ID3 = 0x02000002;
	int ZU_ID4 = 0x02000003;
	int BL_ID  = 0x01010000;
	int BL_EN  = 0x01030000;
	short sg[4] ;
	unsigned char sendbuf[8]={0};
	
	sg[0] = deal_num(cfg_para.cfg_ch_zu.ch_zu1_h,corretPara[0],corretPara[1]);
	sg[1] = deal_num(cfg_para.cfg_ch_zu.ch_zu1_l,corretPara[2],corretPara[3]);
	sg[2] = deal_num(cfg_para.cfg_ch_zu.ch_zu2_h,corretPara[4],corretPara[5]);
	sg[3] = deal_num(cfg_para.cfg_ch_zu.ch_zu2_l,corretPara[6],corretPara[7]);
	sendbuf[0] = sg[0]>>8;
	sendbuf[1] = sg[0] & 0xff;
	sendbuf[2] = sg[1]>>8;
	sendbuf[3] = sg[1] & 0xff;
	sendbuf[4] = sg[2]>>8;
	sendbuf[5] = sg[2] & 0xff;
	sendbuf[6] = sg[3]>>8;
	sendbuf[7] = sg[3] & 0xff;
	CAN_SendMessage(ZU_ID1 | picNum<<8,sendbuf);
	
	sg[0] = deal_num(cfg_para.cfg_ch_zu.ch_zu3_h,corretPara[8],corretPara[9]);
	sg[1] = deal_num(cfg_para.cfg_ch_zu.ch_zu3_l,corretPara[10],corretPara[11]);
	sg[2] = deal_num(cfg_para.cfg_ch_zu.ch_zu4_h,corretPara[12],corretPara[13]);
	sg[3] = deal_num(cfg_para.cfg_ch_zu.ch_zu4_l,corretPara[14],corretPara[15]);
	sendbuf[0] = sg[0]>>8;
	sendbuf[1] = sg[0] & 0xff;
	sendbuf[2] = sg[1]>>8;
	sendbuf[3] = sg[1] & 0xff;
	sendbuf[4] = sg[2]>>8;
	sendbuf[5] = sg[2] & 0xff;
	sendbuf[6] = sg[3]>>8;
	sendbuf[7] = sg[3] & 0xff;
	CAN_SendMessage(ZU_ID2 | picNum<<8,sendbuf);
	
  sg[0] = deal_num(cfg_para.cfg_ch_zu.ch_zu5_h,corretPara[16],corretPara[17]);
	sg[1] = deal_num(cfg_para.cfg_ch_zu.ch_zu5_l,corretPara[18],corretPara[19]);
	sg[2] = deal_num(cfg_para.cfg_ch_zu.ch_zu6_h,corretPara[20],corretPara[21]);
	sg[3] = deal_num(cfg_para.cfg_ch_zu.ch_zu6_l,corretPara[22],corretPara[23]);
	sendbuf[0] = sg[0]>>8;
	sendbuf[1] = sg[0] & 0xff;
	sendbuf[2] = sg[1]>>8;
	sendbuf[3] = sg[1] & 0xff;
	sendbuf[4] = sg[2]>>8;
	sendbuf[5] = sg[2] & 0xff;
	sendbuf[6] = sg[3]>>8;
	sendbuf[7] = sg[3] & 0xff;
	CAN_SendMessage(ZU_ID3 | picNum<<8,sendbuf);	
//	vTaskDelay(1);
	
	memset(sendbuf,0,8);
	sendbuf[0] = cfg_para.totalTime>>24;
	sendbuf[1] = (cfg_para.totalTime>>16) & 0xff;
	sendbuf[2] = (cfg_para.totalTime>>8) & 0xff;
	sendbuf[3] = cfg_para.totalTime & 0xff;
	CAN_SendMessage(ZU_ID4 | picNum<<8,sendbuf);
//	vTaskDelay(2);
	
//	memset(sendbuf,0,8);
//	sendbuf[0] = (cfg_para.voltage/10)>>8;
//	sendbuf[1] = (cfg_para.voltage/10) & 0xff;
//	sendbuf[2] = (cfg_para.current/100)>>8;
//	sendbuf[3] = (cfg_para.current/100) & 0xff;
//	sendbuf[4] = (cfg_para.current/100 +150) >> 8;
//	sendbuf[5] = (cfg_para.current/100 +150) & 0xff;
//	
//	CAN_SendMessage(BL_ID,sendbuf);
//	vTaskDelay(5);
//	
//	memset(sendbuf,0,8);
//	sendbuf[0] = 0x01;
//	CAN_SendMessage(BL_EN,sendbuf);	
//	vTaskDelay(5);
	sendToBl();
	
	sendSigData(picNum);
}

static void comSendMessage(uint8_t cmd,uint8_t *buf)
{
	uint8_t sendBuf[24]={0};
	uint16_t sum=0;
	uint8_t len=0,i;
	
	len = strlen(buf);
	
	sendBuf[0] = 0xC9;
	sendBuf[1] = 0xDB;
	
	sendBuf[2] = len+1;
	sendBuf[3] = cmd;	
	memcpy(sendBuf+4,buf,len);
	
	sum = sendBuf[2] + sendBuf[3];
	for(i=0;i<len;i++)
	 sum += buf[i];
	sendBuf[len+4] = (sum & 0xff00)>>8;
	sendBuf[len+5] = sum & 0xff;
	
	sendBuf[len+6] = 0xE8;
	sendBuf[len+7] = 0xB2;
	
	comSendBuf(COM1, sendBuf, len+8);
}

static void uart_jiexi(uint8_t *buf,char len)
{
	uint8_t i=0,cmd=0,piclen=1;
	uint16_t sum=0,sumR=0;
	char FileNamebuf[16]={0},ack[2]={0},canbuf[8]={0};

	for(i=2;i<3+buf[2];i++)
		sum+=buf[i];
	
	sumR = buf[len-4]<<8 | buf[len-3];
	if((uint16_t)(sum&0x0000FFFF)==sumR)    //��У���
	{
		sum=0;
		switch (buf[3])
		{
			case 1:
				cmd = 0x01;
				for(i=0;cfgname[i][0]!='\0';i++)
				{
					memset(FileNamebuf,0,sizeof(FileNamebuf));
					strcpy(FileNamebuf,cfgname[i]);
					sprintf(FileNamebuf, "%d:%s",i,cfgname[i]);
					comSendMessage(cmd,FileNamebuf);
				}
				break;
			case 2:
				pro_num = buf[4];
				xTaskNotify(xHandleTaskGUI,BIT_23 | pro_num,eSetValueWithOverwrite);
				break;
			case 3:
				for(i=0;i<piclen;i++)
				{
					memset(&cfg_para,0,sizeof(cfg_para));
					sprintf(FileNamebuf, "%s%02d",cfgname[pro_num], i);
					ReadFileData(FS_VOLUME_SD,FileNamebuf,&cfg_para,sizeof(cfg_para));
					sendToFPGA(i);
					piclen = cfg_para.picMaxN;
				}
				cmd = 0x02;
				ack[0] = 0x01;
				comSendMessage(cmd,ack);
				break;
			case 4:
//				sendToBl();
//				temp1=((unsigned int)BL_data[0]<<8)|((unsigned int)BL_data[1]);    //BL 	U
				if(buf[4]==1)
					flag_bl = 1;
				else if(buf[4]==2)
				{
					flag_bl = 0;
					CAN_SendMessage(0x02030000,canbuf);//disable play	
					CAN_SendMessage(0x01030000,canbuf);//disable backlight out
					memset(BL_data,0,8);
				}
				break;
			case 5:
				cmd = 0x04;
				ack[0] = cfg_para.picMaxN;
				comSendMessage(cmd,ack);
				break;
			case 6:
				memset(canbuf,0,8);
				CAN_SendMessage(0x02030001 | (buf[4]<<8),canbuf);
				sprintf(FileNamebuf, "%s%02d", cfgname[pro_num],buf[4]);
				ReadFileData(FS_VOLUME_SD,FileNamebuf,&picArray,sizeof(CONFIG_PIC));
				break;
			default:
				break;
		}
	}
}

void DemoCOM(void)
{
	uint8_t aRxBuffer[24]={0},ack[2]={0};
	uint8_t temp=0,cnt=0,cmd=0;
	uint8_t  read_flag=0,len=0;
	char flagRd = 1;
	char comStatus = UART_IDLE;
	uint16_t temp1=0,temp2=0;
	
	while (1)
	{	
		if(comGetChar(COM1, &temp))
		{
			if(temp==0xc9)
			{
				aRxBuffer[0] = temp;
				flagRd=1;
				comStatus = UART_RECV_BEGIN;
			}
			while(flagRd)
			{
				if(comGetChar(COM1, &temp))
				{
					switch (comStatus)
					{
						case UART_RECV_BEGIN:
							if(temp==0xDB)
								aRxBuffer[1] = temp;
							comStatus = UART_RECV_LENGTH;
							break;
						case UART_RECV_LENGTH:
							aRxBuffer[2] = temp;
							comStatus = UART_RECV_DATA;
							break;
						case UART_RECV_DATA:
							aRxBuffer[3+cnt] = temp;
							cnt++;
							if(cnt>=aRxBuffer[2]+2)
								comStatus = UART_RECV_END;
							break;
						case UART_RECV_END:
							if(temp==0xE8)
								aRxBuffer[3+cnt] = temp;
							comStatus = UART_RECV_RETURN;
							break;
						case UART_RECV_RETURN:
							if(temp==0xB2)
								aRxBuffer[4+cnt] = temp;
							comStatus = UART_IDLE;
							len = aRxBuffer[2]+7;
							flagRd = 0;
							cnt = 0;
							break;
					}
				}
			}
//			comSendBuf(COM1,aRxBuffer,len);
			temp1=((unsigned int)BL_data[0]<<8)|((unsigned int)BL_data[1]);    //BL 	U
			uart_jiexi(aRxBuffer,len);
		}
		temp2=((unsigned int)BL_data[0]<<8)|((unsigned int)BL_data[1]);    //BL 	U
		if(temp1!=temp2)
		{
			temp1 = temp2;
			cmd = 0x03;
			ack[0] = 0x01;
			comSendMessage(cmd,ack);
		}
		
		vTaskDelay(10);
	}
}
/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
