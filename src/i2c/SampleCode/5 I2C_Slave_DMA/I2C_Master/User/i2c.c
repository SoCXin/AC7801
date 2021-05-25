
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


/*************<union>******************/


/*************<struct>*****************/


/*************<variable>***************/
uint8_t 	g_tstDataBuf[I2C_TST_BUF_LEN];
uint8_t 	g_slvDataRdyCur;

/*************<prototype>**************/
void EXTI3_8_Callback(void *device, uint32_t wpara, uint32_t lpara); 

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
	GPIO_SetDir (I2C_SLV_RDY_PORT, I2C_SLV_RDY_PIN, GPIO_IN);
	GPIO_SetPulldown(I2C_SLV_RDY_PORT, I2C_SLV_RDY_PIN, ENABLE);
	GPIO_EnableExtInterrupt(I2C_SLV_RDY_PORT, I2C_SLV_RDY_PIN, EXTI_TRIGGER_RISING_FALLING);
	GPIO_SetCallback(I2C_SLV_RDY_PIN, EXTI3_8_Callback);
	
	/*����ṹ�����.*/
	memset(&i2cConfig, 0x00, sizeof(i2cConfig));
	
	/*�������������Ǵӻ�ģʽ����Ҫ���õĲ���.*/
	i2cConfig.mode				= I2C_MASTER;/*�������ӻ�ģʽ.*/
	i2cConfig.extAddrEn			= DISABLE;/*10bit��չ��ַ����.*/
	i2cConfig.dmaRxEn			= DISABLE;/*����DMA��������.*/
	i2cConfig.dmaTxEn			= DISABLE;/*����DMA��������.*/
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
* @prototype I2C_Transmit1KBytes(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 ����1024�ֽ�����.
*/
void I2C_Transmit1KBytes(void)
{
	uint8_t tempCmdBuf[4] = {0xAC, 0x78, 0x01, 0x53};
	
	/*׼������.*/
	for (uint16_t ii = 0; ii < I2C_TST_BUF_LEN; ii++)
	{
		g_tstDataBuf[ii] = ii;
		printf("%d ", g_tstDataBuf[ii]);
	}
	printf("\r\n");
	
	/*����ָ��.*/
	while(!g_slvDataRdyCur);//�ȴ��ӻ�׼���ý���ָ��
	g_slvDataRdyCur = FALSE;
	I2C_MasterTransmitPoll(I2C0, I2C_SLV_ADDR, tempCmdBuf, I2C_CMD_BUF_LEN, DISABLE);
	
	/*��������.*/
	while(!g_slvDataRdyCur);//�ȴ��ӻ�׼���ý�������
	g_slvDataRdyCur = FALSE;
	I2C_MasterTransmitPoll(I2C0, I2C_SLV_ADDR, g_tstDataBuf, I2C_TST_BUF_LEN, ENABLE);
}

/**
* @prototype I2C_Receive1KBytes(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 ����1024�ֽ�����.
*/
void I2C_Receive1KBytes(void)
{
	uint8_t tempCmdBuf[4] = {0xAC, 0x78, 0x01, 0x52};
	
	/*��ջ���.*/
	memset(g_tstDataBuf, 0x00, I2C_TST_BUF_LEN);
	
	/*����ָ��.*/
	while(!g_slvDataRdyCur);//�ȴ��ӻ�׼���ý���ָ��
	g_slvDataRdyCur = FALSE;
	I2C_MasterTransmitPoll(I2C0, I2C_SLV_ADDR, tempCmdBuf, I2C_CMD_BUF_LEN, DISABLE);
	
	/*��������.*/
	while(!g_slvDataRdyCur);//�ȴ��ӻ�׼���ý�������
	g_slvDataRdyCur = FALSE;
	I2C_MasterReceivePoll(I2C0, I2C_SLV_ADDR, g_tstDataBuf, I2C_TST_BUF_LEN);
	
	/*��ӡ����.*/
	for (uint16_t ii = 0; ii < I2C_TST_BUF_LEN; ii++)
	{
		printf("%d ", g_tstDataBuf[ii]);
	}
	printf("\r\n");
}

/**
* @prototype EXTI3_8_Callback(void *device, uint32_t wpara, uint32_t lpara)
*
* @param[in] ...
* @return	 void
*
* @brief  	 �ⲿ�жϻص�����.
*/
void EXTI3_8_Callback(void *device, uint32_t wpara, uint32_t lpara)
{
	if ((device == I2C_SLV_RDY_PORT) && (wpara == I2C_SLV_RDY_PIN))
	{
		g_slvDataRdyCur = TRUE;
	}
}


/*************<end>********************/
