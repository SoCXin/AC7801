
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
uint8_t		g_eepromWrRdTime;//读取写入计时
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
* @brief  	 初始化I2C.
*/
void I2C_ParaInit(void)
{
	I2C_ConfigType i2cConfig;
	
	/*初始化引脚功能为I2C.*/
	GPIO_SetFunc(I2C0_SCL_PORT, I2C0_SCL_PIN, GPIO_FUN3);
	GPIO_SetFunc(I2C0_SDA_PORT, I2C0_SDA_PIN, GPIO_FUN3);
	
	/*清零结构体变量.*/
	memset(&i2cConfig, 0x00, sizeof(i2cConfig));
	
	/*无论是主机还是从机模式都需要配置的参数.*/
	i2cConfig.mode				= I2C_MASTER;/*设置主从机模式.*/
	i2cConfig.extAddrEn			= DISABLE;/*10bit扩展地址禁能.*/
	i2cConfig.dmaRxEn			= ENABLE;/*设置DMA接收数据.*/
	i2cConfig.dmaTxEn			= ENABLE;/*设置DMA发送数据.*/
	i2cConfig.interruptEn		= ENABLE;/*I2C中断,BND/SAMF/ARBLOST.*/
	i2cConfig.nackInterruptEn	= DISABLE;/*NACK中断.*/
	i2cConfig.ssInterruptEn		= DISABLE;/*总线start或stop中断.*/
	i2cConfig.i2cEn				= ENABLE;/*使能模块.*/
	i2cConfig.callBack			= NULL;/*中断回调函数.*/
	
	/*主机模式需要配置的参数,配置成从机模式可忽略.*/
	i2cConfig.masterConfig.sampleCnt = 9;/*设置波特率为100Kbps,bandrate=(24M/(10*12*2))=100Kbps.*/
	i2cConfig.masterConfig.stepCnt	 = 11;
	i2cConfig.masterConfig.ARBEn 	 = ENABLE;/*设置主机仲裁功能.*/
	i2cConfig.masterConfig.SYNCEn	 = ENABLE;/*设置主机SCL同步功能.*/
	
	/*从机模式需要配置的参数,配置成主机模式可忽略.*/
	i2cConfig.slaveConfig.slaveAddr		 = AT24C02_DEV_ADDR;/*从机地址.*/
	i2cConfig.slaveConfig.slaveRangeAddr = 0;/*从机范围地址.*/
	i2cConfig.slaveConfig.glitchFilterCnt= 0;/*毛刺过滤.*/
	i2cConfig.slaveConfig.stretchEn		 = DISABLE;/*从机SCL延伸功能.*/
	i2cConfig.slaveConfig.rangeAddrEn	 = DISABLE;/*禁能范围地址.*/
	i2cConfig.slaveConfig.monitorEn		 = DISABLE;/*禁能从机监测功能.*/
	i2cConfig.slaveConfig.generalCallEn	 = DISABLE;/*从机SCL广播地址.*/
	i2cConfig.slaveConfig.wakeupEn		 = DISABLE;/*唤醒功能,仅从机时有效.*/
	i2cConfig.slaveConfig.rxOFInterruptEn	= DISABLE;/*接收缓存溢出中断.*/
	i2cConfig.slaveConfig.txUFInterruptEn	= DISABLE;/*发送缓存溢出中断.*/
	
	I2C_Init(I2C0, &i2cConfig);
}

/**
* @prototype AT24C0X_WriteDataTest(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 写数据到EEPROM测试.
*/
void AT24C0X_WriteDataTest(void)
{
	/*准备数据.*/
	for (uint16_t ii = 0; ii < AT24C02_TST_BUF_LEN; ii++)
	{
		g_eepromTstDataBuf[ii] = ii;
		printf("%d ", g_eepromTstDataBuf[ii]);
	}
	printf("\r\n");
	
	/*准备计时.*/
	g_eepromWrRdRdy  = 1;
	g_eepromWrRdTime = 0;
	
	/*写入数据.*/
	AT24C0X_WriteBytes(AT24C02_WR_TST_ADDR, g_eepromTstDataBuf, AT24C02_TST_BUF_LEN);
	
	/*停止计时.*/
	g_eepromWrRdRdy  = 0;
	
	/*打印时间.*/
	printf("Write AT24C02 256 bytes time is: %d ms", g_eepromWrRdTime);
	printf("\r\n");
}

/**
* @prototype AT24C0X_ReadDataTest(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 从EEPROM读取数据测试.
*/
void AT24C0X_ReadDataTest(void)
{
	/*准备计时.*/
	g_eepromWrRdRdy  = 1;
	g_eepromWrRdTime = 0;
	
	/*读取数据.*/
	memset(g_eepromTstDataBuf, 0x00, AT24C02_TST_BUF_LEN);
	AT24C0X_ReadBytes(AT24C02_WR_TST_ADDR, g_eepromTstDataBuf, AT24C02_TST_BUF_LEN);
	
	/*停止计时.*/
	g_eepromWrRdRdy  = 0;
	
	/*打印数据及时间.*/
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
* @param[in] startAddr:读数据起始地址
* @param[in] pDataBuf: 读数据缓存区
* @param[in] dataLen:  读数据长度
* @return	 void
*
* @brief  	 从EEPROM读取数据.
*/
void AT24C0X_ReadBytes (uint8_t startAddr, uint8_t *pDataBuf, uint16_t dataLen)
{
	uint8_t __align(4) tempRdBuf[AT24C02_MAX_SIZE + 1];//DMA发送需要4字节对齐
	
	tempRdBuf[0] = startAddr;
	
	g_DMA_startTransmit = TRUE;//准备发送数据
	I2C_MasterTransmitDMA(I2C0, AT24C02_DEV_ADDR, DMA0_CHANNEL0, tempRdBuf, 1, DMA_TX_Callback);//发送读取地址
	while(g_DMA_startTransmit == TRUE);//等待发送完毕
	I2C_WaitOneByteFinished(I2C0);//等待最后一个字节发送完毕
	
	g_DMA_startReceive  = TRUE;//准备接收数据
	I2C_MasterReceiveDMA (I2C0, AT24C02_DEV_ADDR, DMA0_CHANNEL1, tempRdBuf, dataLen, DMA_RX_Callback);//读取数据
	while(g_DMA_startReceive == TRUE);//等待读取完毕
	I2C_Stop(I2C0);
	
	memcpy(pDataBuf, tempRdBuf, dataLen);//将数据放在指定的缓存区
}

/**
* @prototype AT24C0X_WriteBytes(uint8_t startAddr, uint8_t *pDataBuf, uint16_t dataLen)
*
* @param[in] startAddr:写数据起始地址
* @param[in] pDataBuf: 写数据缓存区
* @param[in] dataLen:  写数据长度
* @return	 void
*
* @brief  	 向EEPROM写入数据.
*/
void AT24C0X_WriteBytes(uint8_t startAddr, uint8_t *pDataBuf, uint16_t dataLen)
{
	uint8_t ii,restLen,wrTimes;
	uint8_t __align(4) tempWrBuf[AT24C02_PAGE_SIZ + 1];//DMA发送需要4字节对齐
	
	if ((startAddr + (dataLen - 1)) > AT24C02_ADDR_MAX){return;}//地址超出范围退出
	
	restLen = (dataLen % 8);
	wrTimes = (dataLen / 8);
	
	for (ii = 0; ii < wrTimes; ii++)
	{
		tempWrBuf[0] = startAddr;
		memcpy(&tempWrBuf[1], &pDataBuf[ii * AT24C02_PAGE_SIZ], AT24C02_PAGE_SIZ);//单次写页8字节
		
		g_DMA_startTransmit = TRUE;//准备发送数据
		I2C_MasterTransmitDMA(I2C0, AT24C02_DEV_ADDR, DMA0_CHANNEL0, tempWrBuf, (AT24C02_PAGE_SIZ + 1), DMA_TX_Callback);
		startAddr   += AT24C02_PAGE_SIZ;
		while(g_DMA_startTransmit == TRUE);//等待发送完毕
		I2C_WaitOneByteFinished(I2C0);//等待最后一个字节发送完毕
		I2C_Stop(I2C0);
		
		mdelay(5);//需要等待5ms发送下一条写入命令
	}
	
	if (restLen > 0)//非满页,需要单字节方式写入
	{
		for (ii = 0; ii < restLen; ii++)
		{
			tempWrBuf[0] = startAddr++;
			memcpy(&tempWrBuf[1], &pDataBuf[wrTimes * AT24C02_PAGE_SIZ + ii], 1);
			
			g_DMA_startTransmit = TRUE;//准备发送数据
			I2C_MasterTransmitDMA(I2C0, AT24C02_DEV_ADDR, DMA0_CHANNEL0, tempWrBuf, 1, DMA_TX_Callback);//单次写1个byte
			while(g_DMA_startTransmit == TRUE);//等待发送完毕
			I2C_WaitOneByteFinished(I2C0);//等待最后一个字节发送完毕
			I2C_Stop(I2C0);
			
			mdelay(5);//需要等待5ms发送下一条写入命令
		}
	}
}


/**
* @prototype DMA_TX_Callback(void *device, uint32_t wpara, uint32_t lpara)
*
* @param[in] ...
* @return	 void
*
* @brief  	 DMA TX中断回调函数.
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
* @brief  	 DMA RX中断回调函数.
*/
void DMA_RX_Callback(void *device, uint32_t wpara, uint32_t lpara)
{
	if (wpara & DMA_CHANNEL_STATUS_FINISH_Msk)
	{
		g_DMA_startReceive = FALSE;
	}
}

/*************<end>********************/
