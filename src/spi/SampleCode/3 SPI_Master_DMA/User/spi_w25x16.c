
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
#define W25X16_DTRD_BUF_LEN		(1030)//读取BUF长度
#define W25X16_DTWR_BUF_LEN		(260) //写入BUF长度

#define W25X16_TEST_DAT_LEN		(1024)//测试数据BUF长度
#define W25X16_WRRD_DAT_ADDR	(0x00)



/*************<enum>*******************/


/*************<union>******************/


/*************<struct>*****************/


/*************<variable>***************/
uint8_t	__align(4)	g_flashWrDataBuf[W25X16_DTWR_BUF_LEN];
uint8_t	__align(4)	g_flashRdDataBuf[W25X16_DTRD_BUF_LEN];
uint8_t 	g_flshTstDataBuf[W25X16_TEST_DAT_LEN];
uint8_t		g_flashWrRdRdy;
uint16_t	g_flashWrRdTime;//读取写入计时
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
* @brief  	 初始化SPI.
*/
void SPI_ParaInit(void)
{
	SPI_ConfigType spiConfig;
	
	/*初始化SPI引脚,功能复用选择.*/
	GPIO_SetFunc(GPIOB, GPIO_PIN15, GPIO_FUN3);//SCK
	GPIO_SetFunc(GPIOB, GPIO_PIN14, GPIO_FUN3);//MOSI
	GPIO_SetFunc(GPIOC, GPIO_PIN0, GPIO_FUN3);//MISO
	GPIO_SetFunc(GPIOC, GPIO_PIN1, GPIO_FUN3);//CS
	
	/*清零配置结构体变量.*/
	memset(&spiConfig, 0x00, sizeof(spiConfig));
	
	/*初始化SPI参数,波特率 = 2Mbps = (F_BCLK / (SCK_LOW+1 + SCK_HIGH+1)).*/
	spiConfig.csSetup = 4;/*片选建立时间  = (CS_SETUP + 1) * CLK_PERIOD.*/
	spiConfig.csHold  = 4;/*片选保持时间  = (CS_HOLD + 1) * CLK_PERIOD.*/
	spiConfig.sckHigh = 5;/*SCK高电平时间 = (SCK_HIGH + 1) * CLK_PERIOD.*/
	spiConfig.sckLow  = 5;/*SCK低电平时间 = (SCK_LOW + 1) * CLK_PERIOD.*/
	spiConfig.csIdle  = 4;/*两条数据间最短时间间隔 = (CS_IDLE + 1) * CLK_PERIOD.*/
	spiConfig.mode   		= SPI_MASTER;//设置为主机模式
	spiConfig.cpol	 		= SPI_CPOL_HIGH;//设置SCK空闲时极性,空闲时SCK为低
	spiConfig.cpha	 		= SPI_CPHA_2EDGE;//设置数据采样相位,第2个边沿采样数据
	spiConfig.frmSize 		= SPI_FRAME_SIZE_8BITS;
	spiConfig.rxMsbFirstEn	= ENABLE;//选择从最高位开始接收
	spiConfig.txMsbFirstEn	= ENABLE;//选择从最高位开始发送
	spiConfig.csOutputEn	= ENABLE;//CS有SPI硬件控制
	spiConfig.continuousCSEn= ENABLE;//片选连续模式
	spiConfig.dmaRxEn		= ENABLE;//禁止使用DMA接收数据
	spiConfig.dmaTxEn		= ENABLE;//禁止使用DMA发送数据
	spiConfig.modeFaultEn	= DISABLE;//模式故障禁止
	spiConfig.wakeUpEn		= DISABLE;//主机模式不支持唤醒功能
	spiConfig.spiEn 		= ENABLE;
	spiConfig.callBack		= NULL;
	spiConfig.interruptEn		= DISABLE;//禁能VIC中断,可使能
	spiConfig.txUFInterruptEn 	= DISABLE;//禁能TXUF中断,可使能
	spiConfig.rxOFInterruptEn 	= DISABLE;//禁能RXOF中断,可使能
	spiConfig.modeFaultInterruptEn = DISABLE;//模式故障中断
	SPI_Init(SPI1, &spiConfig);
	
	/*初始化控制变量.*/
	g_flashWrRdRdy	= 0;
	g_flashWrRdTime = 0;
}

/**
* @prototype W25X16_ReadDataTest(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 测试W25X16读取功能.
*/
void W25X16_ReadDataTest(void)
{
	/*准备计时.*/
	g_flashWrRdRdy	= 1;
	g_flashWrRdTime = 0;
	
	/*读取数据.*/
	memset(g_flshTstDataBuf, 0x00, W25X16_TEST_DAT_LEN);
	W25X16_ReadBytes(W25X16_WRRD_DAT_ADDR, g_flshTstDataBuf, W25X16_TEST_DAT_LEN);
	
	/*停止计时.*/
	g_flashWrRdRdy	= 0;
	/*打印数据和时间.*/
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
* @brief  	 测试W25X16写功能.
*/
void W25X16_WriteDataTest(void)
{
	/*擦除数据.*/
	W25X16_ErsSector(W25X16_WRRD_DAT_ADDR, 1);//擦除1个扇区4KB
	
	/*准备数据.*/
	for (uint16_t ii = 0; ii < W25X16_TEST_DAT_LEN; ii++)
	{
		g_flshTstDataBuf[ii] = ii;
		printf("%d ", g_flshTstDataBuf[ii]);
	}
	printf("\r\n");
	
	/*准备计时.*/
	g_flashWrRdRdy	= 1;
	g_flashWrRdTime = 0;
	
	/*写入数据.*/
	W25X16_WriteBytes(W25X16_WRRD_DAT_ADDR, g_flshTstDataBuf, W25X16_TEST_DAT_LEN);
	
	/*停止计时.*/
	g_flashWrRdRdy	= 0;
	/*打印时间.*/
	printf("Write W25X16 1K bytes time is: %d ms", g_flashWrRdTime);
	printf("\r\n");
}

/**
* @prototype W25X16_ReadBytes(uint32_t startAddr, uint8_t *pDataBuf, uint16_t dataLen)
*
* @param[in] startAddr: 起始地址
* @param[in] pDataBuf:	数据缓存
* @param[in] dataLen:	数据长度
* @return	 void
*
* @brief  	 从W25X16读取指定长度字节数据.
*/
void W25X16_ReadBytes(uint32_t startAddr, uint8_t *pDataBuf, uint16_t dataLen)
{
	while(W25X16_IsBusy());
	
	g_flashWrDataBuf[0] = W25X16_CMD_RD_DAT;
	g_flashWrDataBuf[1] = (startAddr >> 16);
	g_flashWrDataBuf[2] = (startAddr >>  8);
	g_flashWrDataBuf[3] = (startAddr >>  0);
	
	g_DMA_startReceive  = TRUE;//准备接收数据
	SPI_TransmitReceiveDMA(SPI1, SPI1_TX_DMA_CHN, SPI1_RX_DMA_CHN,\
							g_flashRdDataBuf, g_flashWrDataBuf, (dataLen + 4), DMA_RX_Callback);//可读取任意长度数据
	while(g_DMA_startReceive == TRUE);//等待读取完毕
	SPI_MasterReleaseCS(SPI1);
	
	memcpy(pDataBuf, &g_flashRdDataBuf[4], dataLen);
}

/**
* @prototype W25X16_WriteBytes(uint32_t startAddr, uint8_t *pDataBuf, uint16_t dataLen)
*
* @param[in] startAddr: 起始地址
* @param[in] pDataBuf:	数据缓存
* @param[in] dataLen:	数据长度
* @return	 void
*
* @brief  	 向W25X16写入指定长度字节数据.
*/
void W25X16_WriteBytes(uint32_t startAddr, uint8_t *pDataBuf, uint16_t dataLen)
{
	uint8_t ii,restLen, wrTimes;
	
	if ((startAddr + (dataLen - 1)) > W25X16_ADDR_MAX){return;}
	
	restLen = (dataLen % 256);//少于256字节长度
	wrTimes = (dataLen / 256);//每次最多写入256字节
	
	/*满足256字节的数据写入.*/
	for (ii = 0; ii < wrTimes; ii++)
	{
		/*使能写.*/
		W25X16_EnableWr();
		
		/*写入数据.*/
		g_flashWrDataBuf[0] = W25X16_CMD_WR_DAT;
		g_flashWrDataBuf[1] = (startAddr >> 16);
		g_flashWrDataBuf[2] = (startAddr >> 8);
		g_flashWrDataBuf[3] = (startAddr >> 0);
		memcpy(&g_flashWrDataBuf[4], &pDataBuf[ii * 256], 256);//单次写入最多256字节
		
		g_DMA_startTransmit = TRUE;//准备发送数据
		SPI_TransmitDMA(SPI1, SPI1_TX_DMA_CHN, g_flashWrDataBuf, (256 + 4), DMA_TX_Callback);
		while(g_DMA_startTransmit == TRUE);//等待发送完毕
		SPI_MasterReleaseCS(SPI1);
		udelay(50);//等待写入数据完成,PageProgram时间1.6ms
		
		startAddr += 256;
	}
	
	/*不足256字节的数据写入.*/
	if (restLen > 0)
	{
		/*使能写.*/
		W25X16_EnableWr();
		
		/*写入数据.*/
		g_flashWrDataBuf[0] = W25X16_CMD_WR_DAT;
		g_flashWrDataBuf[1] = (startAddr >> 16);
		g_flashWrDataBuf[2] = (startAddr >> 8);
		g_flashWrDataBuf[3] = (startAddr >> 0);
		memcpy(&g_flashWrDataBuf[4], &pDataBuf[wrTimes * 256], restLen);
		
		g_DMA_startTransmit = TRUE;//准备发送数据
		SPI_TransmitDMA(SPI1, SPI1_TX_DMA_CHN, g_flashWrDataBuf, (restLen + 4), DMA_TX_Callback);
		while(g_DMA_startTransmit == TRUE);//等待发送完毕
		SPI_MasterReleaseCS(SPI1);
	}
}

/**
* @prototype W25X16_ErsSector(uint32_t startAddr, uint8_t secErNum)
*
* @param[in] startAddr:擦除
* @param[in] secErNum:需要擦除的sec数
* @return	 void
*
* @brief  	 擦除W25X16.
*/
void W25X16_ErsSector(uint32_t startAddr, uint8_t secErNum)
{
	uint8_t  ii;
	
	if ((startAddr + secErNum * W25X16_SEC_SIZE) > W25X16_ADDR_MAX){return;}
	
	for (ii = 0; ii < secErNum; ii++)
	{
		/*使能写,在擦除、写入后自动关闭写.*/
		W25X16_EnableWr();
		
		/*装载擦除指令,并发送指令.*/
		g_flashWrDataBuf[0] = W25X16_CMD_ER_SEC;
		g_flashWrDataBuf[1] = (startAddr >> 16);
		g_flashWrDataBuf[2] = (startAddr >>  8);
		g_flashWrDataBuf[3] = (startAddr >>  0);
		
		g_DMA_startTransmit = TRUE;//准备发送数据
		SPI_TransmitDMA(SPI1, SPI1_TX_DMA_CHN, g_flashWrDataBuf, 4, DMA_TX_Callback);
		while(g_DMA_startTransmit == TRUE);//等待发送完毕
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
* @brief  	 使能写W25X16.
*/
static void W25X16_EnableWr(void)
{
	/*先判断W25X16是否忙.空闲时才能进行操作.*/
	while(W25X16_IsBusy());
	
	g_flashWrDataBuf[0] = W25X16_CMD_WR_EN;
	
	g_DMA_startTransmit = TRUE;//准备发送数据
	SPI_TransmitDMA(SPI1, SPI1_TX_DMA_CHN, g_flashWrDataBuf, 1, DMA_TX_Callback);
	while(g_DMA_startTransmit == TRUE);//等待发送完毕
	SPI_MasterReleaseCS(SPI1);
}

/**
* @prototype W25X16_IsBusy(void)
*
* @param[in] void
* @return	 0->Not busy. 1->Busy.
*
* @brief  	 判断W25X16是否忙.
*/
static uint8_t W25X16_IsBusy(void)
{
	g_flashWrDataBuf[0] = W25X16_CMD_RD_STS;
	
	g_DMA_startReceive  = TRUE;//准备接收数据
	SPI_TransmitReceiveDMA(SPI1, SPI1_TX_DMA_CHN, SPI1_RX_DMA_CHN,\
							g_flashRdDataBuf, g_flashWrDataBuf, 2, DMA_RX_Callback);
	while(g_DMA_startReceive == TRUE);//等待读取完毕
	SPI_MasterReleaseCS(SPI1);
	
	return (g_flashRdDataBuf[1] & W25X16_BUSY_MSK);
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
