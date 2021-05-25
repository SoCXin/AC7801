
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
#include "spi_w25x16.h"


/*************<macro>******************/
#define W25X16_DTRD_BUF_LEN		(1030)//��ȡBUF����
#define W25X16_DTWR_BUF_LEN		(260) //д��BUF����

#define W25X16_TEST_DAT_LEN		(1024)//��������BUF����
#define W25X16_WRRD_DAT_ADDR	(0x00)



/*************<enum>*******************/


/*************<union>******************/


/*************<struct>*****************/


/*************<variable>***************/
uint8_t	__align(4)	g_flashWrDataBuf[W25X16_DTWR_BUF_LEN];
uint8_t	__align(4)	g_flashRdDataBuf[W25X16_DTRD_BUF_LEN];
uint8_t 	g_flshTstDataBuf[W25X16_TEST_DAT_LEN];
uint8_t		g_flashWrRdRdy;
uint16_t	g_flashWrRdTime;//��ȡд���ʱ
uint8_t 	g_DMA_startTransmit;
uint8_t 	g_DMA_startReceive;


/*************<prototype>**************/
void DMA_TX_Callback(void *device, uint32_t wpara, uint32_t lpara);
void DMA_RX_Callback(void *device, uint32_t wpara, uint32_t lpara);
	
static uint8_t 	W25X16_IsBusy(void);
static void 	W25X16_EnableWr(void);

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
	GPIO_SetFunc(GPIOB, GPIO_PIN15, GPIO_FUN3);//SCK
	GPIO_SetFunc(GPIOB, GPIO_PIN14, GPIO_FUN3);//MOSI
	GPIO_SetFunc(GPIOC, GPIO_PIN0, GPIO_FUN3);//MISO
	GPIO_SetFunc(GPIOC, GPIO_PIN1, GPIO_FUN3);//CS
	
	/*�������ýṹ�����.*/
	memset(&spiConfig, 0x00, sizeof(spiConfig));
	
	/*��ʼ��SPI����,������ = 2Mbps = (F_BCLK / (SCK_LOW+1 + SCK_HIGH+1)).*/
	spiConfig.csSetup = 4;/*Ƭѡ����ʱ��  = (CS_SETUP + 1) * CLK_PERIOD.*/
	spiConfig.csHold  = 4;/*Ƭѡ����ʱ��  = (CS_HOLD + 1) * CLK_PERIOD.*/
	spiConfig.sckHigh = 5;/*SCK�ߵ�ƽʱ�� = (SCK_HIGH + 1) * CLK_PERIOD.*/
	spiConfig.sckLow  = 5;/*SCK�͵�ƽʱ�� = (SCK_LOW + 1) * CLK_PERIOD.*/
	spiConfig.csIdle  = 4;/*�������ݼ����ʱ���� = (CS_IDLE + 1) * CLK_PERIOD.*/
	spiConfig.mode   		= SPI_MASTER;//����Ϊ����ģʽ
	spiConfig.cpol	 		= SPI_CPOL_HIGH;//����SCK����ʱ����,����ʱSCKΪ��
	spiConfig.cpha	 		= SPI_CPHA_2EDGE;//�������ݲ�����λ,��2�����ز�������
	spiConfig.frmSize 		= SPI_FRAME_SIZE_8BITS;
	spiConfig.rxMsbFirstEn	= ENABLE;//ѡ������λ��ʼ����
	spiConfig.txMsbFirstEn	= ENABLE;//ѡ������λ��ʼ����
	spiConfig.csOutputEn	= ENABLE;//CS��SPIӲ������
	spiConfig.continuousCSEn= ENABLE;//Ƭѡ����ģʽ
	spiConfig.dmaRxEn		= ENABLE;//��ֹʹ��DMA��������
	spiConfig.dmaTxEn		= ENABLE;//��ֹʹ��DMA��������
	spiConfig.modeFaultEn	= DISABLE;//ģʽ���Ͻ�ֹ
	spiConfig.wakeUpEn		= DISABLE;//����ģʽ��֧�ֻ��ѹ���
	spiConfig.spiEn 		= ENABLE;
	spiConfig.callBack		= NULL;
	spiConfig.interruptEn		= DISABLE;//����VIC�ж�,��ʹ��
	spiConfig.txUFInterruptEn 	= DISABLE;//����TXUF�ж�,��ʹ��
	spiConfig.rxOFInterruptEn 	= DISABLE;//����RXOF�ж�,��ʹ��
	spiConfig.modeFaultInterruptEn = DISABLE;//ģʽ�����ж�
	SPI_Init(SPI1, &spiConfig);
	
	/*��ʼ�����Ʊ���.*/
	g_flashWrRdRdy	= 0;
	g_flashWrRdTime = 0;
}

/**
* @prototype W25X16_ReadDataTest(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 ����W25X16��ȡ����.
*/
void W25X16_ReadDataTest(void)
{
	/*׼����ʱ.*/
	g_flashWrRdRdy	= 1;
	g_flashWrRdTime = 0;
	
	/*��ȡ����.*/
	memset(g_flshTstDataBuf, 0x00, W25X16_TEST_DAT_LEN);
	W25X16_ReadBytes(W25X16_WRRD_DAT_ADDR, g_flshTstDataBuf, W25X16_TEST_DAT_LEN);
	
	/*ֹͣ��ʱ.*/
	g_flashWrRdRdy	= 0;
	/*��ӡ���ݺ�ʱ��.*/
	for (uint16_t ii = 0; ii < W25X16_TEST_DAT_LEN; ii++)
	{
		printf("%d ", g_flshTstDataBuf[ii]);
	}
	printf("\r\n");
	printf("Read W25X16 1K bytes time is: %d ms", g_flashWrRdTime);
	printf("\r\n");
}
	
/**
* @prototype W25X16_WriteDataTest(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 ����W25X16д����.
*/
void W25X16_WriteDataTest(void)
{
	/*��������.*/
	W25X16_ErsSector(W25X16_WRRD_DAT_ADDR, 1);//����1������4KB
	
	/*׼������.*/
	for (uint16_t ii = 0; ii < W25X16_TEST_DAT_LEN; ii++)
	{
		g_flshTstDataBuf[ii] = ii;
		printf("%d ", g_flshTstDataBuf[ii]);
	}
	printf("\r\n");
	
	/*׼����ʱ.*/
	g_flashWrRdRdy	= 1;
	g_flashWrRdTime = 0;
	
	/*д������.*/
	W25X16_WriteBytes(W25X16_WRRD_DAT_ADDR, g_flshTstDataBuf, W25X16_TEST_DAT_LEN);
	
	/*ֹͣ��ʱ.*/
	g_flashWrRdRdy	= 0;
	/*��ӡʱ��.*/
	printf("Write W25X16 1K bytes time is: %d ms", g_flashWrRdTime);
	printf("\r\n");
}

/**
* @prototype W25X16_ReadBytes(uint32_t startAddr, uint8_t *pDataBuf, uint16_t dataLen)
*
* @param[in] startAddr: ��ʼ��ַ
* @param[in] pDataBuf:	���ݻ���
* @param[in] dataLen:	���ݳ���
* @return	 void
*
* @brief  	 ��W25X16��ȡָ�������ֽ�����.
*/
void W25X16_ReadBytes(uint32_t startAddr, uint8_t *pDataBuf, uint16_t dataLen)
{
	while(W25X16_IsBusy());
	
	g_flashWrDataBuf[0] = W25X16_CMD_RD_DAT;
	g_flashWrDataBuf[1] = (startAddr >> 16);
	g_flashWrDataBuf[2] = (startAddr >>  8);
	g_flashWrDataBuf[3] = (startAddr >>  0);
	
	g_DMA_startReceive  = TRUE;//׼����������
	SPI_TransmitReceiveDMA(SPI1, SPI1_TX_DMA_CHN, SPI1_RX_DMA_CHN,\
							g_flashRdDataBuf, g_flashWrDataBuf, (dataLen + 4), DMA_RX_Callback);//�ɶ�ȡ���ⳤ������
	while(g_DMA_startReceive == TRUE);//�ȴ���ȡ���
	SPI_MasterReleaseCS(SPI1);
	
	memcpy(pDataBuf, &g_flashRdDataBuf[4], dataLen);
}

/**
* @prototype W25X16_WriteBytes(uint32_t startAddr, uint8_t *pDataBuf, uint16_t dataLen)
*
* @param[in] startAddr: ��ʼ��ַ
* @param[in] pDataBuf:	���ݻ���
* @param[in] dataLen:	���ݳ���
* @return	 void
*
* @brief  	 ��W25X16д��ָ�������ֽ�����.
*/
void W25X16_WriteBytes(uint32_t startAddr, uint8_t *pDataBuf, uint16_t dataLen)
{
	uint8_t ii,restLen, wrTimes;
	
	if ((startAddr + (dataLen - 1)) > W25X16_ADDR_MAX){return;}
	
	restLen = (dataLen % 256);//����256�ֽڳ���
	wrTimes = (dataLen / 256);//ÿ�����д��256�ֽ�
	
	/*����256�ֽڵ�����д��.*/
	for (ii = 0; ii < wrTimes; ii++)
	{
		/*ʹ��д.*/
		W25X16_EnableWr();
		
		/*д������.*/
		g_flashWrDataBuf[0] = W25X16_CMD_WR_DAT;
		g_flashWrDataBuf[1] = (startAddr >> 16);
		g_flashWrDataBuf[2] = (startAddr >> 8);
		g_flashWrDataBuf[3] = (startAddr >> 0);
		memcpy(&g_flashWrDataBuf[4], &pDataBuf[ii * 256], 256);//����д�����256�ֽ�
		
		g_DMA_startTransmit = TRUE;//׼����������
		SPI_TransmitDMA(SPI1, SPI1_TX_DMA_CHN, g_flashWrDataBuf, (256 + 4), DMA_TX_Callback);
		while(g_DMA_startTransmit == TRUE);//�ȴ��������
		SPI_MasterReleaseCS(SPI1);
		udelay(50);//�ȴ�д���������,PageProgramʱ��1.6ms
		
		startAddr += 256;
	}
	
	/*����256�ֽڵ�����д��.*/
	if (restLen > 0)
	{
		/*ʹ��д.*/
		W25X16_EnableWr();
		
		/*д������.*/
		g_flashWrDataBuf[0] = W25X16_CMD_WR_DAT;
		g_flashWrDataBuf[1] = (startAddr >> 16);
		g_flashWrDataBuf[2] = (startAddr >> 8);
		g_flashWrDataBuf[3] = (startAddr >> 0);
		memcpy(&g_flashWrDataBuf[4], &pDataBuf[wrTimes * 256], restLen);
		
		g_DMA_startTransmit = TRUE;//׼����������
		SPI_TransmitDMA(SPI1, SPI1_TX_DMA_CHN, g_flashWrDataBuf, (restLen + 4), DMA_TX_Callback);
		while(g_DMA_startTransmit == TRUE);//�ȴ��������
		SPI_MasterReleaseCS(SPI1);
	}
}

/**
* @prototype W25X16_ErsSector(uint32_t startAddr, uint8_t secErNum)
*
* @param[in] startAddr:����
* @param[in] secErNum:��Ҫ������sec��
* @return	 void
*
* @brief  	 ����W25X16.
*/
void W25X16_ErsSector(uint32_t startAddr, uint8_t secErNum)
{
	uint8_t  ii;
	
	if ((startAddr + secErNum * W25X16_SEC_SIZE) > W25X16_ADDR_MAX){return;}
	
	for (ii = 0; ii < secErNum; ii++)
	{
		/*ʹ��д,�ڲ�����д����Զ��ر�д.*/
		W25X16_EnableWr();
		
		/*װ�ز���ָ��,������ָ��.*/
		g_flashWrDataBuf[0] = W25X16_CMD_ER_SEC;
		g_flashWrDataBuf[1] = (startAddr >> 16);
		g_flashWrDataBuf[2] = (startAddr >>  8);
		g_flashWrDataBuf[3] = (startAddr >>  0);
		
		g_DMA_startTransmit = TRUE;//׼����������
		SPI_TransmitDMA(SPI1, SPI1_TX_DMA_CHN, g_flashWrDataBuf, 4, DMA_TX_Callback);
		while(g_DMA_startTransmit == TRUE);//�ȴ��������
		SPI_MasterReleaseCS(SPI1);
		
		startAddr += W25X16_SEC_SIZE;
	}
}

/**
* @prototype W25X16_EnableWr(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 ʹ��дW25X16.
*/
static void W25X16_EnableWr(void)
{
	/*���ж�W25X16�Ƿ�æ.����ʱ���ܽ��в���.*/
	while(W25X16_IsBusy());
	
	g_flashWrDataBuf[0] = W25X16_CMD_WR_EN;
	
	g_DMA_startTransmit = TRUE;//׼����������
	SPI_TransmitDMA(SPI1, SPI1_TX_DMA_CHN, g_flashWrDataBuf, 1, DMA_TX_Callback);
	while(g_DMA_startTransmit == TRUE);//�ȴ��������
	SPI_MasterReleaseCS(SPI1);
}

/**
* @prototype W25X16_IsBusy(void)
*
* @param[in] void
* @return	 0->Not busy. 1->Busy.
*
* @brief  	 �ж�W25X16�Ƿ�æ.
*/
static uint8_t W25X16_IsBusy(void)
{
	g_flashWrDataBuf[0] = W25X16_CMD_RD_STS;
	
	g_DMA_startReceive  = TRUE;//׼����������
	SPI_TransmitReceiveDMA(SPI1, SPI1_TX_DMA_CHN, SPI1_RX_DMA_CHN,\
							g_flashRdDataBuf, g_flashWrDataBuf, 2, DMA_RX_Callback);
	while(g_DMA_startReceive == TRUE);//�ȴ���ȡ���
	SPI_MasterReleaseCS(SPI1);
	
	return (g_flashRdDataBuf[1] & W25X16_BUSY_MSK);
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
