/*
*********************************************************************************************************
*	                                  
*	ģ������ : CAN������ʾ����
*	�ļ����� : can_network.h
*	��    �� : V1.3
*	˵    �� : ͷ�ļ�
*	�޸ļ�¼ :
*		�汾��  ����       ����      ˵��
*		v1.3    2015-01-29 Eric2013  �׷�
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/


#ifndef _CAN_NETWORK_H
#define _CAN_NETWORK_H

/* ���ⲿ���õĺ������� */
void can1_Init(void);
void can1_NVIC_Config(void);
void can2_Init(void);
void can2_NVIC_Config(void);
void can_demo(void);
void CAN_SendMessage(unsigned int ID,unsigned char *array);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
