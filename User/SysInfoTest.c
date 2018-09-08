/*
*********************************************************************************************************
*
*	ģ������ : ��ʱ��ʱ��
*	�ļ����� : SysInfoTest.c
*	��    �� : V1.0
*	˵    �� : Ϊ�˻�ȡFreeRTOS��������Ϣ����Ҫ����һ����ʱ���������ʱ����ʱ���׼����Ҫ����
*              ϵͳʱ�ӽ��ġ������õ���������Ϣ��׼ȷ��
*              ���ļ��ṩ�ĺ��������ڲ���Ŀ�ģ��в��ɽ�������ʵ����Ŀ��ԭ�������㣺
*               1. FreeRTOS��ϵͳ�ں�û�ж��ܵļ���ʱ�������������
*               2. ��ʱ���ж���50us����һ�Σ��Ƚ�Ӱ��ϵͳ���ܡ�
*              --------------------------------------------------------------------------------------
*              ���ļ�ʹ�õ���32λ����������50usһ�εļ���ֵ�����֧�ּ���ʱ�䣺
*              2^32 * 50us / 3600s = 59.6���ӡ�ʹ���в��Ե��������м���������ռ���ʳ�����59.6���ӽ���׼ȷ��
*
*	�޸ļ�¼ :
*		�汾��    ����        ����     ˵��
*		V1.0    2015-08-19  Eric2013   �׷�
*
*	Copyright (C), 2015-2020, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "includes.h"
#include "MainTask.h"
#include "SysInfoTest.h"

uint8_t data[1024];
FRESULT result;
FIL file;
FIL MyFiles;
FIL FileSave;   //ר������ͼƬ�ı���
FILINFO finfo;
DIR DirInf;
DIR DirInf_usb;
UINT bw;
FATFS fs;
FATFS fs_nand;
FATFS fs_usb;

/* ��ʱ��Ƶ�ʣ�50usһ���ж� */
#define  timerINTERRUPT_FREQUENCY	20000

/* �ж����ȼ� */
#define  timerHIGHEST_PRIORITY		2

/* ��ϵͳ���� */
volatile uint32_t ulHighFrequencyTimerTicks = 0UL;
struct roundbutton_struct ROUND_BUTTON;
/*
*********************************************************************************************************
*	�� �� ��: vSetupTimerTest
*	����˵��: ������ʱ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void vSetupSysInfoTest(void)
{
	bsp_SetTIMforInt(TIM6, timerINTERRUPT_FREQUENCY, timerHIGHEST_PRIORITY, 0);
}

/*
*********************************************************************************************************
*	�� �� ��: TIM6_DAC_IRQHandler
*	����˵��: TIM6�жϷ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void TIM6_DAC_IRQHandler( void )
{
	if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
	{
		ulHighFrequencyTimerTicks++;
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: EXTI0_Config
*	����˵��: �����ⲿ�жϡ�K1��K2��K3 �ж�.
*				K1�� PC13 ���½��ش���
*				K2�� PA0  : �����ش���
*				K3�� PG8  : �½��ش���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void EXTI_Config(void)
{
	/* ����PB7 */
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		EXTI_InitTypeDef   EXTI_InitStructure;
		NVIC_InitTypeDef   NVIC_InitStructure;		
		
		/* ʹ�� GPIO ʱ�� */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		/* Enable SYSCFG clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
		
		/* Configure PI8 pin as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		 /* Connect EXTI Line8 to PI8 pin */
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource7);

		/* Configure EXTI8 line */
		EXTI_InitStructure.EXTI_Line = EXTI_Line7;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  	/*�½��ش��� */
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);

		/* Enable and set EXTI8 Interrupt to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0f;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: EXTI9_5_IRQHandler
*	����˵��: �ⲿ�жϷ������
*	��    �Σ���
*	�� �� ֵ: �� 40013C00
*********************************************************************************************************
*/
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line7) != RESET)
	{
		EXIT_CLOSE;
		ROUND_BUTTON.Valid_flag = 0x01;
		ROUND_BUTTON.status =  (GPIOB->IDR & (1 << 8))?LEFT:RIGHT;
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
	
}
//void EXTI9_5_IRQHandler(void)
//{
//	if (EXTI_GetITStatus(EXTI_Line7) != RESET)
//	{
////		((union int_2_bit *)40013C00)->temp_bit.exit_en = 0 ;
//		*((unsigned int *)(0013C00))=0x00;
//		EXTI_ClearITPendingBit(EXTI_Line7);
//	}
//}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
