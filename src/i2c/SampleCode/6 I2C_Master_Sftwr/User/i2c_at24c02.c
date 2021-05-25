
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
#include "i2c_sftwr.h"
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


/*************<prototype>**************/
 

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
	uint16_t ii;
	
	/*���Ͷ�ȡ���ݵ�ַ.*/
	I2C_Start();
	I2C_TransmitOneByte((AT24C02_DEV_ADDR << 1) | I2C_WRITE);
	I2C_WaitACK();
	I2C_TransmitOneByte(startAddr);
	I2C_WaitACK();
	/*��ʼ��ȡ����.*/
	I2C_Start();
	I2C_TransmitOneByte((AT24C02_DEV_ADDR << 1) | I2C_READ);
	I2C_WaitACK();
	for (ii = 0; ii < dataLen; ii++)
	{
		pDataBuf[ii] = I2C_ReceiveOneByte();
		I2C_SendACK();
	}
	I2C_Stop();
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
	uint8_t ii,jj,restLen,wrTimes;
	uint8_t tempWrBuf[AT24C02_PAGE_SIZ + 1];
	
	if ((startAddr + (dataLen - 1)) > AT24C02_ADDR_MAX){return;}//��ַ������Χ�˳�
	
	restLen = (dataLen % 8);
	wrTimes = (dataLen / 8);
	
	for (ii = 0; ii < wrTimes; ii++)
	{
		tempWrBuf[0] = startAddr;
		startAddr   += AT24C02_PAGE_SIZ;
		memcpy(&tempWrBuf[1], &pDataBuf[ii * AT24C02_PAGE_SIZ], AT24C02_PAGE_SIZ);//����дҳ8�ֽ�
		
		/*��ʼд������.*/
		I2C_Start();
		I2C_TransmitOneByte((AT24C02_DEV_ADDR << 1) | I2C_WRITE);
		I2C_WaitACK();
		for (jj = 0; jj < (AT24C02_PAGE_SIZ + 1); jj++)
		{
			I2C_TransmitOneByte(tempWrBuf[jj]);
			I2C_WaitACK();
		}
		I2C_Stop();
		/*д��ҳ���ݽ���.*/
		
		mdelay(5);//��Ҫ�ȴ�5ms������һ��д������
	}
	
	if (restLen > 0)//����ҳ,��Ҫ���ֽڷ�ʽд��
	{
		for (ii = 0; ii < restLen; ii++)
		{
			tempWrBuf[0] = startAddr++;
			memcpy(&tempWrBuf[1], &pDataBuf[wrTimes * AT24C02_PAGE_SIZ + ii], 1);
			
			/*��ʼд������.*/
			I2C_Start();
			I2C_TransmitOneByte((AT24C02_DEV_ADDR << 1) | I2C_WRITE);
			I2C_WaitACK();
			for (jj = 0; jj < 2; jj++)//1����ַ+һ������
			{
				I2C_TransmitOneByte(tempWrBuf[jj]);
				I2C_WaitACK();
			}
			I2C_Stop();
			/*д�뵥�ֽڽ���.*/
			
			mdelay(5);//��Ҫ�ȴ�5ms������һ��д������
		}
	}
}


/*************<end>********************/
