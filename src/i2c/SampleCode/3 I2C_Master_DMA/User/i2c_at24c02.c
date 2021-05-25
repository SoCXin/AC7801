
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
#include "i2c_at24c02.h"

/*************<macro>******************/
#define AT24C02_MAX_SIZE		(256)

#define AT24C02_WR_TST_ADDR		(0x00)
#define AT24C02_TST_BUF_LEN		(256)


/*************<enum>*******************/


/*************<union>******************/


/*************<struct>*****************/


/*************<variable>***************/
uint8_t 	g_eepromTstDataBuf[AT24C02_TST_BUF_LEN];
uint8_t		g_eepromWrRdRdy;
uint8_t		g_eepromWrRdTime;//��ȡд���ʱ
uint8_t 	g_DMA_startTransmit;
uint8_t 	g_DMA_startReceive;


/*************<prototype>**************/
void DMA_TX_Callback(void *device, uint32_t wpara, uint32_t lpara);
void DMA_RX_Callback(void *device, uint32_t wpara, uint32_t lpara);

/**
* @prototype I2C_ParaInit(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 ��ʼ��I2C.
*/
void I2C_ParaInit(void)
{
	I2C_ConfigType i2cConfig;
	
	/*��ʼ�����Ź���ΪI2C.*/
	GPIO_SetFunc(I2C0_SCL_PORT, I2C0_SCL_PIN, GPIO_FUN3);
	GPIO_SetFunc(I2C0_SDA_PORT, I2C0_SDA_PIN, GPIO_FUN3);
	
	/*����ṹ�����.*/
	memset(&i2cConfig, 0x00, sizeof(i2cConfig));
	
	/*�������������Ǵӻ�ģʽ����Ҫ���õĲ���.*/
	i2cConfig.mode				= I2C_MASTER;/*�������ӻ�ģʽ.*/
	i2cConfig.extAddrEn			= DISABLE;/*10bit��չ��ַ����.*/
	i2cConfig.dmaRxEn			= ENABLE;/*����DMA��������.*/
	i2cConfig.dmaTxEn			= ENABLE;/*����DMA��������.*/
	i2cConfig.interruptEn		= ENABLE;/*I2C�ж�,BND/SAMF/ARBLOST.*/
	i2cConfig.nackInterruptEn	= DISABLE;/*NACK�ж�.*/
	i2cConfig.ssInterruptEn		= DISABLE;/*����start��stop�ж�.*/
	i2cConfig.i2cEn				= ENABLE;/*ʹ��ģ��.*/
	i2cConfig.callBack			= NULL;/*�жϻص�����.*/
	
	/*����ģʽ��Ҫ���õĲ���,���óɴӻ�ģʽ�ɺ���.*/
	i2cConfig.masterConfig.sampleCnt = 9;/*���ò�����Ϊ100Kbps,bandrate=(24M/(10*12*2))=100Kbps.*/
	i2cConfig.masterConfig.stepCnt	 = 11;
	i2cConfig.masterConfig.ARBEn 	 = ENABLE;/*���������ٲù���.*/
	i2cConfig.masterConfig.SYNCEn	 = ENABLE;/*��������SCLͬ������.*/
	
	/*�ӻ�ģʽ��Ҫ���õĲ���,���ó�����ģʽ�ɺ���.*/
	i2cConfig.slaveConfig.slaveAddr		 = AT24C02_DEV_ADDR;/*�ӻ���ַ.*/
	i2cConfig.slaveConfig.slaveRangeAddr = 0;/*�ӻ���Χ��ַ.*/
	i2cConfig.slaveConfig.glitchFilterCnt= 0;/*ë�̹���.*/
	i2cConfig.slaveConfig.stretchEn		 = DISABLE;/*�ӻ�SCL���칦��.*/
	i2cConfig.slaveConfig.rangeAddrEn	 = DISABLE;/*���ܷ�Χ��ַ.*/
	i2cConfig.slaveConfig.monitorEn		 = DISABLE;/*���ܴӻ���⹦��.*/
	i2cConfig.slaveConfig.generalCallEn	 = DISABLE;/*�ӻ�SCL�㲥��ַ.*/
	i2cConfig.slaveConfig.wakeupEn		 = DISABLE;/*���ѹ���,���ӻ�ʱ��Ч.*/
	i2cConfig.slaveConfig.rxOFInterruptEn	= DISABLE;/*���ջ�������ж�.*/
	i2cConfig.slaveConfig.txUFInterruptEn	= DISABLE;/*���ͻ�������ж�.*/
	
	I2C_Init(I2C0, &i2cConfig);
}

/**
* @prototype AT24C0X_WriteDataTest(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 д���ݵ�EEPROM����.
*/
void AT24C0X_WriteDataTest(void)
{
	/*׼������.*/
	for (uint16_t ii = 0; ii < AT24C02_TST_BUF_LEN; ii++)
	{
		g_eepromTstDataBuf[ii] = ii;
		printf("%d ", g_eepromTstDataBuf[ii]);
	}
	printf("\r\n");
	
	/*׼����ʱ.*/
	g_eepromWrRdRdy  = 1;
	g_eepromWrRdTime = 0;
	
	/*д������.*/
	AT24C0X_WriteBytes(AT24C02_WR_TST_ADDR, g_eepromTstDataBuf, AT24C02_TST_BUF_LEN);
	
	/*ֹͣ��ʱ.*/
	g_eepromWrRdRdy  = 0;
	
	/*��ӡʱ��.*/
	printf("Write AT24C02 256 bytes time is: %d ms", g_eepromWrRdTime);
	printf("\r\n");
}

/**
* @prototype AT24C0X_ReadDataTest(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 ��EEPROM��ȡ���ݲ���.
*/
void AT24C0X_ReadDataTest(void)
{
	/*׼����ʱ.*/
	g_eepromWrRdRdy  = 1;
	g_eepromWrRdTime = 0;
	
	/*��ȡ����.*/
	memset(g_eepromTstDataBuf, 0x00, AT24C02_TST_BUF_LEN);
	AT24C0X_ReadBytes(AT24C02_WR_TST_ADDR, g_eepromTstDataBuf, AT24C02_TST_BUF_LEN);
	
	/*ֹͣ��ʱ.*/
	g_eepromWrRdRdy  = 0;
	
	/*��ӡ���ݼ�ʱ��.*/
	for (uint16_t ii = 0; ii < AT24C02_TST_BUF_LEN; ii++)
	{
		printf("%d ", g_eepromTstDataBuf[ii]);
	}
	printf("\r\n");
	printf("Read AT24C02 256 bytes time is: %d ms", g_eepromWrRdTime);
	printf("\r\n");
}

/**
* @prototype AT24C0X_ReadBytes(uint8_t startAddr, uint8_t *pDataBuf, uint16_t dataLen)
*
* @param[in] startAddr:��������ʼ��ַ
* @param[in] pDataBuf: �����ݻ�����
* @param[in] dataLen:  �����ݳ���
* @return	 void
*
* @brief  	 ��EEPROM��ȡ����.
*/
void AT24C0X_ReadBytes (uint8_t startAddr, uint8_t *pDataBuf, uint16_t dataLen)
{
	uint8_t __align(4) tempRdBuf[AT24C02_MAX_SIZE + 1];//DMA������Ҫ4�ֽڶ���
	
	tempRdBuf[0] = startAddr;
	
	g_DMA_startTransmit = TRUE;//׼����������
	I2C_MasterTransmitDMA(I2C0, AT24C02_DEV_ADDR, DMA0_CHANNEL0, tempRdBuf, 1, DMA_TX_Callback);//���Ͷ�ȡ��ַ
	while(g_DMA_startTransmit == TRUE);//�ȴ��������
	I2C_WaitOneByteFinished(I2C0);//�ȴ����һ���ֽڷ������
	
	g_DMA_startReceive  = TRUE;//׼����������
	I2C_MasterReceiveDMA (I2C0, AT24C02_DEV_ADDR, DMA0_CHANNEL1, tempRdBuf, dataLen, DMA_RX_Callback);//��ȡ����
	while(g_DMA_startReceive == TRUE);//�ȴ���ȡ���
	I2C_Stop(I2C0);
	
	memcpy(pDataBuf, tempRdBuf, dataLen);//�����ݷ���ָ���Ļ�����
}

/**
* @prototype AT24C0X_WriteBytes(uint8_t startAddr, uint8_t *pDataBuf, uint16_t dataLen)
*
* @param[in] startAddr:д������ʼ��ַ
* @param[in] pDataBuf: д���ݻ�����
* @param[in] dataLen:  д���ݳ���
* @return	 void
*
* @brief  	 ��EEPROMд������.
*/
void AT24C0X_WriteBytes(uint8_t startAddr, uint8_t *pDataBuf, uint16_t dataLen)
{
	uint8_t ii,restLen,wrTimes;
	uint8_t __align(4) tempWrBuf[AT24C02_PAGE_SIZ + 1];//DMA������Ҫ4�ֽڶ���
	
	if ((startAddr + (dataLen - 1)) > AT24C02_ADDR_MAX){return;}//��ַ������Χ�˳�
	
	restLen = (dataLen % 8);
	wrTimes = (dataLen / 8);
	
	for (ii = 0; ii < wrTimes; ii++)
	{
		tempWrBuf[0] = startAddr;
		memcpy(&tempWrBuf[1], &pDataBuf[ii * AT24C02_PAGE_SIZ], AT24C02_PAGE_SIZ);//����дҳ8�ֽ�
		
		g_DMA_startTransmit = TRUE;//׼����������
		I2C_MasterTransmitDMA(I2C0, AT24C02_DEV_ADDR, DMA0_CHANNEL0, tempWrBuf, (AT24C02_PAGE_SIZ + 1), DMA_TX_Callback);
		startAddr   += AT24C02_PAGE_SIZ;
		while(g_DMA_startTransmit == TRUE);//�ȴ��������
		I2C_WaitOneByteFinished(I2C0);//�ȴ����һ���ֽڷ������
		I2C_Stop(I2C0);
		
		mdelay(5);//��Ҫ�ȴ�5ms������һ��д������
	}
	
	if (restLen > 0)//����ҳ,��Ҫ���ֽڷ�ʽд��
	{
		for (ii = 0; ii < restLen; ii++)
		{
			tempWrBuf[0] = startAddr++;
			memcpy(&tempWrBuf[1], &pDataBuf[wrTimes * AT24C02_PAGE_SIZ + ii], 1);
			
			g_DMA_startTransmit = TRUE;//׼����������
			I2C_MasterTransmitDMA(I2C0, AT24C02_DEV_ADDR, DMA0_CHANNEL0, tempWrBuf, 1, DMA_TX_Callback);//����д1��byte
			while(g_DMA_startTransmit == TRUE);//�ȴ��������
			I2C_WaitOneByteFinished(I2C0);//�ȴ����һ���ֽڷ������
			I2C_Stop(I2C0);
			
			mdelay(5);//��Ҫ�ȴ�5ms������һ��д������
		}
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
