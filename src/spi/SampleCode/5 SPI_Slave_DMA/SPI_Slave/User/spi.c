
/* Copyright Statement:
 *
 * This software/firmware and related documentation ("AutoChips Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to AutoChips Inc. and/or its licensors. Without
 * the prior written permission of AutoChips inc. and/or its licensors,  any
 * reproduction,  modification,  use or disclosure of AutoChips Software,  and
 * information contained herein,  in whole or in part,  shall be strictly
 * prohibited.
 *
 * AutoChips Inc. (C) 2018. All rights reserved.
 *
 * BY OPENING THIS FILE,  RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("AUTOCHIPS SOFTWARE")
 * RECEIVED FROM AUTOCHIPS AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. AUTOCHIPS EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES,  EXPRESS OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES AUTOCHIPS PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, 
 * INCORPORATED IN,  OR SUPPLIED WITH THE AUTOCHIPS SOFTWARE,  AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN AUTOCHIPS
 * SOFTWARE. AUTOCHIPS SHALL ALSO NOT BE RESPONSIBLE FOR ANY AUTOCHIPS SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AUTOCHIPS'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE AUTOCHIPS SOFTWARE
 * RELEASED HEREUNDER WILL BE,  AT AUTOCHIPS'S OPTION,  TO REVISE OR REPLACE THE
 * AUTOCHIPS SOFTWARE AT ISSUE,  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO AUTOCHIPS FOR SUCH AUTOCHIPS SOFTWARE AT ISSUE.
 */

/*************<start>******************/


/*************<include>****************/
#include "string.h"
#include "gpio.h"
#include "spi.h"
#include "ac780x_spi_reg.h"

/*************<macro>******************/
#define SPI_CMD_BUF_LEN		(4)
#define SPI_TST_BUF_LEN		(1024)


/*************<enum>*******************/
typedef enum
{
	Receive	 = 0,
	Transmit = 1,
}SPI_TxRxType;

/*************<union>******************/


/*************<struct>*****************/


/*************<variable>***************/
uint8_t __align(4)	g_tstDataBuf[SPI_TST_BUF_LEN];
uint8_t __align(4)	g_cmdDataBuf[SPI_CMD_BUF_LEN];
uint8_t		g_cmdReceive[]	= {0xAC, 0x78, 0x01, 0x52};
uint8_t 	g_cmdTransmit[] = {0xAC, 0x78, 0x01, 0x53};
uint8_t 	g_DMA_startTransmit;
uint8_t 	g_DMA_startReceive;

/*************<prototype>**************/
void DMA_TX_Callback(void *device, uint32_t wpara, uint32_t lpara);
void DMA_RX_Callback(void *device, uint32_t wpara, uint32_t lpara);
void SPI_SetSlvRdy(SPI_TxRxType CommDir);

/**
* @prototype SPI_ParaInit(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 ��ʼ��SPI.
*/
void SPI_ParaInit(void)
{
	SPI_ConfigType spiConfig;
	
	/*��ʼ��SPI����,���ܸ���ѡ��.*/
	GPIO_SetFunc(GPIOA, GPIO_PIN7, GPIO_FUN3);//MOSI
	GPIO_SetFunc(GPIOB, GPIO_PIN4, GPIO_FUN3);//MISO
	GPIO_SetFunc(GPIOB, GPIO_PIN5, GPIO_FUN3);//SCK
	GPIO_SetFunc(GPIOA, GPIO_PIN8, GPIO_FUN3);//CS
	
	GPIO_SetFunc(SPI_SLV_RDY_PORT, SPI_SLV_RDY_PIN, GPIO_FUN0);
	GPIO_SetDir (SPI_SLV_RDY_PORT, SPI_SLV_RDY_PIN, GPIO_OUT);
	
	/*�������ýṹ�����.*/
	memset(&spiConfig, 0x00, sizeof(spiConfig));
	
	/*��ʼ��SPI����,������ = 1Mbps = (F_BCLK / (SCK_LOW+1 + SCK_HIGH+1)).*/
	spiConfig.csSetup = 4;/*Ƭѡ����ʱ��  = (CS_SETUP + 1) * CLK_PERIOD.*/
	spiConfig.csHold  = 4;/*Ƭѡ����ʱ��  = (CS_HOLD + 1) * CLK_PERIOD.*/
	spiConfig.sckHigh = 11;/*SCK�ߵ�ƽʱ�� = (SCK_HIGH + 1) * CLK_PERIOD.*/
	spiConfig.sckLow  = 11;/*SCK�͵�ƽʱ�� = (SCK_LOW + 1) * CLK_PERIOD.*/
	spiConfig.csIdle  = 4;/*�������ݼ����ʱ���� = (CS_IDLE + 1) * CLK_PERIOD.*/
	spiConfig.mode   		= SPI_SLAVE;//����Ϊ�ӻ�ģʽ
	spiConfig.cpha	 		= SPI_CPHA_2EDGE;//�������ݲ�����λ,��2�����ز�������
	spiConfig.cpol	 		= SPI_CPOL_HIGH;//����SCK����ʱ����,����ʱSCKΪ��
	spiConfig.frmSize 		= SPI_FRAME_SIZE_8BITS;
	spiConfig.rxMsbFirstEn	= ENABLE;//ѡ������λ��ʼ����
	spiConfig.txMsbFirstEn	= ENABLE;//ѡ������λ��ʼ����
	spiConfig.csOutputEn	= ENABLE;//CS��SPIӲ������
	spiConfig.continuousCSEn= ENABLE;//Ƭѡ����ģʽ
	spiConfig.dmaRxEn		= ENABLE;//ʹ��DMA��������
	spiConfig.dmaTxEn		= ENABLE;//ʹ��DMA��������
	spiConfig.modeFaultEn	= DISABLE;//ģʽ���Ͻ�ֹ
	spiConfig.wakeUpEn		= DISABLE;//����ģʽ��֧�ֻ��ѹ���
	spiConfig.spiEn 		= ENABLE;
	spiConfig.callBack		= NULL;
	spiConfig.interruptEn		= DISABLE;//��ֹNVIC�ж�
	spiConfig.txUFInterruptEn 	= DISABLE;//��ֹTXUF�ж�
	spiConfig.rxOFInterruptEn 	= DISABLE;//��ֹRXOF�ж�
	spiConfig.modeFaultInterruptEn = DISABLE;//ģʽ�����ж�
	SPI_Init(SPI0, &spiConfig);
	
	/*׼������ָ��.*/
	SPI_ReceiveDMA(SPI0, DMA0_CHANNEL0, g_cmdDataBuf, SPI_CMD_BUF_LEN, DMA_RX_Callback);
	SPI_SetSlvRdy(Receive);//�ӻ�����
}

/**
* @prototype SPI_DealDataPrd(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 SPI�ӻ���������.
*/
void SPI_DealDataPrd(void)
{
	if (!g_DMA_startReceive)
	{
		if (memcmp(g_cmdDataBuf, g_cmdTransmit, SPI_CMD_BUF_LEN) == 0)//�������ʹӻ�����
		{
			/*��������.*/
			SPI_ReceiveDMA(SPI0, DMA0_CHANNEL0, g_tstDataBuf, SPI_TST_BUF_LEN, DMA_RX_Callback);
			SPI_SetSlvRdy(Receive);//�ӻ�����
			while(g_DMA_startReceive);//�ȴ��������
		}
		else if (memcmp(g_cmdDataBuf, g_cmdReceive,  SPI_CMD_BUF_LEN) == 0)//�������մӻ�����
		{
			/*��������.*/
			SPI_TransmitDMA(SPI0, DMA0_CHANNEL1, g_tstDataBuf, SPI_TST_BUF_LEN, DMA_TX_Callback);
			SPI_SetSlvRdy(Transmit);//�ӻ�����
			while(g_DMA_startTransmit);//�ȴ��������
			udelay(30);
		}
		
		/*׼������ָ��.*/
		SPI_ReceiveDMA(SPI0, DMA0_CHANNEL0, g_cmdDataBuf, SPI_CMD_BUF_LEN, DMA_RX_Callback);
		SPI_SetSlvRdy(Receive);//�ӻ�����
	}
}

/**
* @prototype SPI_SetSlvRdy(SPI_TxRxType CommDir)
*
* @param[in] ...
* @return	 void
*
* @brief  	 ����GPIO��ʾ���ݾ���.
*/
void SPI_SetSlvRdy(SPI_TxRxType CommDir)
{
	if (CommDir == Receive)
	{
		g_DMA_startReceive = TRUE;
	}
	else
	{
		g_DMA_startTransmit = TRUE;
	}
	
	/*��תIO.*/
	if (GPIO_GetPinLevel(SPI_SLV_RDY_PORT, SPI_SLV_RDY_PIN) == GPIO_LEVEL_HIGH)
	{
		GPIO_SetPinLevel(SPI_SLV_RDY_PORT, SPI_SLV_RDY_PIN, GPIO_LEVEL_LOW);
	}
	else
	{
		GPIO_SetPinLevel(SPI_SLV_RDY_PORT, SPI_SLV_RDY_PIN, GPIO_LEVEL_HIGH);
	}
}

/**
* @prototype DMA_TX_Callback(void *device, uint32_t wpara, uint32_t lpara)
*
* @param[in] ...
* @return	 void
*
* @brief  	 DMA TX�жϻص�����.
*/
void DMA_TX_Callback(void *device, uint32_t wpara, uint32_t lpara)
{
	if (wpara & DMA_CHANNEL_STATUS_FINISH_Msk)
	{
		g_DMA_startTransmit = FALSE;
	}
}

/**
* @prototype DMA_TX_Callback(void *device, uint32_t wpara, uint32_t lpara)
*
* @param[in] ...
* @return	 void
*
* @brief  	 DMA RX�жϻص�����.
*/
void DMA_RX_Callback(void *device, uint32_t wpara, uint32_t lpara)
{
	if (wpara & DMA_CHANNEL_STATUS_FINISH_Msk)
	{
		g_DMA_startReceive = FALSE;
	}
}

/*************<end>********************/
