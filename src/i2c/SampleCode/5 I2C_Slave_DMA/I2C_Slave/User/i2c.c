
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
#include "i2c.h"

/*************<macro>******************/
#define I2C_CMD_BUF_LEN		(4)
#define I2C_TST_BUF_LEN		(1024)


/*************<enum>*******************/
typedef enum
{
	Receive	 = 0,
	Transmit = 1,
}I2C_TxRxType;

/*************<union>******************/


/*************<struct>*****************/


/*************<variable>***************/
uint8_t __align(4)	g_tstDataBuf[I2C_TST_BUF_LEN];
uint8_t __align(4)	g_cmdDataBuf[I2C_CMD_BUF_LEN];
uint8_t		g_cmdReceive[]	= {0xAC, 0x78, 0x01, 0x52};
uint8_t 	g_cmdTransmit[] = {0xAC, 0x78, 0x01, 0x53};
uint8_t 	g_DMA_startTransmit;
uint8_t 	g_DMA_startReceive;


/*************<prototype>**************/
void DMA_TX_Callback(void *device, uint32_t wpara, uint32_t lpara);
void DMA_RX_Callback(void *device, uint32_t wpara, uint32_t lpara); 
void I2C_SetSlvRdy(I2C_TxRxType CommDir);

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
	
	GPIO_SetFunc(I2C_SLV_RDY_PORT, I2C_SLV_RDY_PIN, GPIO_FUN0);
	GPIO_SetDir (I2C_SLV_RDY_PORT, I2C_SLV_RDY_PIN, GPIO_OUT);
	
	/*����ṹ�����.*/
	memset(&i2cConfig, 0x00, sizeof(i2cConfig));
	
	/*�������������Ǵӻ�ģʽ����Ҫ���õĲ���.*/
	i2cConfig.mode				= I2C_SLAVE;/*�������ӻ�ģʽ.*/
	i2cConfig.extAddrEn			= DISABLE;/*10bit��չ��ַ����.*/
	i2cConfig.dmaRxEn			= ENABLE;/*����DMA��������.*/
	i2cConfig.dmaTxEn			= ENABLE;/*����DMA��������.*/
	i2cConfig.interruptEn		= DISABLE;/*I2C�ж�,BND/SAMF/ARBLOST.*/
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
	i2cConfig.slaveConfig.slaveAddr		 = I2C_SLV_ADDR;/*�ӻ���ַ.*/
	i2cConfig.slaveConfig.slaveRangeAddr = 0;/*�ӻ���Χ��ַ.*/
	i2cConfig.slaveConfig.glitchFilterCnt= 0;/*ë�̹���.*/
	i2cConfig.slaveConfig.stretchEn		 = ENABLE;/*�ӻ�SCL���칦��.*/
	i2cConfig.slaveConfig.rangeAddrEn	 = DISABLE;/*���ܷ�Χ��ַ.*/
	i2cConfig.slaveConfig.monitorEn		 = DISABLE;/*���ܴӻ���⹦��.*/
	i2cConfig.slaveConfig.generalCallEn	 = DISABLE;/*�ӻ�SCL�㲥��ַ.*/
	i2cConfig.slaveConfig.wakeupEn		 = DISABLE;/*���ѹ���,���ӻ�ʱ��Ч.*/
	i2cConfig.slaveConfig.rxOFInterruptEn	= DISABLE;/*���ջ�������ж�.*/
	i2cConfig.slaveConfig.txUFInterruptEn	= DISABLE;/*���ͻ�������ж�.*/
	
	I2C_Init(I2C0, &i2cConfig);
	
	/*׼������ָ��.*/
	I2C_SlaveReceiveDMA(I2C0, DMA0_CHANNEL0, g_cmdDataBuf, I2C_CMD_BUF_LEN, DMA_RX_Callback);
	I2C_SetSlvRdy(Receive);//�ӻ�����
}


/**
* @prototype I2C_DealDataPrd(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 I2C�ӻ���������.
*/
void I2C_DealDataPrd(void)
{
	if (!g_DMA_startReceive)//��ѯָ������Ƿ����
	{
		if (memcmp(g_cmdDataBuf, g_cmdTransmit, I2C_CMD_BUF_LEN) == 0)//�������ʹӻ�����
		{
			/*��������.*/
			I2C_SlaveReceiveDMA(I2C0, DMA0_CHANNEL0, g_tstDataBuf, I2C_TST_BUF_LEN, DMA_RX_Callback);
			I2C_SetSlvRdy(Receive);//�ӻ�����
			while(g_DMA_startReceive);//�ȴ��������
		}
		else if (memcmp(g_cmdDataBuf, g_cmdReceive,  I2C_CMD_BUF_LEN) == 0)//�������մӻ�����
		{
			/*��������.*/
			I2C_SlaveTransmitDMA(I2C0, DMA0_CHANNEL1, g_tstDataBuf, I2C_TST_BUF_LEN, DMA_TX_Callback);
			I2C_SetSlvRdy(Transmit);//�ӻ�����
			while(g_DMA_startTransmit);//�ȴ��������
			I2C_WaitOneByteFinished(I2C0);//�ȴ����һ���ֽڴ�I2C����ȥ
		}
		
		/*׼������ָ��.*/
		I2C_SlaveReceiveDMA(I2C0, DMA0_CHANNEL0, g_cmdDataBuf, I2C_CMD_BUF_LEN, DMA_RX_Callback);
		I2C_SetSlvRdy(Receive);//�ӻ�����
	}
}

/**
* @prototype I2C_SetSlvRdy(I2C_TxRxType CommDir)
*
* @param[in] ...
* @return	 void
*
* @brief  	 ����GPIO��ʾ���ݾ���.
*/
void I2C_SetSlvRdy(I2C_TxRxType CommDir)
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
	if (GPIO_GetPinLevel(I2C_SLV_RDY_PORT, I2C_SLV_RDY_PIN) == GPIO_LEVEL_HIGH)
	{
		GPIO_SetPinLevel(I2C_SLV_RDY_PORT, I2C_SLV_RDY_PIN, GPIO_LEVEL_LOW);
	}
	else
	{
		GPIO_SetPinLevel(I2C_SLV_RDY_PORT, I2C_SLV_RDY_PIN, GPIO_LEVEL_HIGH);
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
