
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


/*************<macro>******************/
#define SPI_CMD_BUF_LEN		(4)
#define SPI_TST_BUF_LEN		(1024)

/*************<enum>*******************/


/*************<union>******************/


/*************<struct>*****************/


/*************<variable>***************/
uint8_t 	g_tstDataBuf[SPI_TST_BUF_LEN];
uint8_t 	g_slvDataRdyCur;

/*************<prototype>**************/
void EXTI3_8_Callback(void *device, uint32_t wpara, uint32_t lpara);


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
	GPIO_SetFunc(GPIOA, GPIO_PIN7, GPIO_FUN3);//MOSI
	GPIO_SetFunc(GPIOB, GPIO_PIN4, GPIO_FUN3);//MISO
	GPIO_SetFunc(GPIOB, GPIO_PIN5, GPIO_FUN3);//SCK
	GPIO_SetFunc(GPIOA, GPIO_PIN8, GPIO_FUN3);//CS
	
	GPIO_SetFunc(SPI_SLV_RDY_PORT, SPI_SLV_RDY_PIN, GPIO_FUN0);
	GPIO_SetDir (SPI_SLV_RDY_PORT, SPI_SLV_RDY_PIN, GPIO_IN);
	GPIO_SetPulldown(SPI_SLV_RDY_PORT, SPI_SLV_RDY_PIN, ENABLE);
	GPIO_EnableExtInterrupt(SPI_SLV_RDY_PORT, SPI_SLV_RDY_PIN, EXTI_TRIGGER_RISING_FALLING);
	GPIO_SetCallback(SPI_SLV_RDY_PIN, EXTI3_8_Callback);
	
	/*清零配置结构体变量.*/
	memset(&spiConfig, 0x00, sizeof(spiConfig));
	
	/*初始化SPI参数,波特率 = 0.6Mbps = (F_BCLK / (SCK_LOW+1 + SCK_HIGH+1)).*/
	spiConfig.csSetup = 4;/*片选建立时间  = (CS_SETUP + 1) * CLK_PERIOD.*/
	spiConfig.csHold  = 0;/*片选保持时间  = (CS_HOLD + 1) * CLK_PERIOD.*/
	spiConfig.sckHigh = 11;/*SCK高电平时间 = (SCK_HIGH + 1) * CLK_PERIOD.*/
	spiConfig.sckLow  = 11;/*SCK低电平时间 = (SCK_LOW + 1) * CLK_PERIOD.*/
	spiConfig.csIdle  = 4;/*两条数据间最短时间间隔 = (CS_IDLE + 1) * CLK_PERIOD.*/
	spiConfig.mode   		= SPI_MASTER;//设置为主机模式
	spiConfig.cpha	 		= SPI_CPHA_2EDGE;//设置数据采样相位,第2个边沿采样数据
	spiConfig.cpol	 		= SPI_CPOL_HIGH;//设置SCK空闲时极性,空闲时SCK为低
	spiConfig.frmSize 		= SPI_FRAME_SIZE_8BITS;
	spiConfig.rxMsbFirstEn	= ENABLE;//选择从最高位开始接收
	spiConfig.txMsbFirstEn	= ENABLE;//选择从最高位开始发送
	spiConfig.csOutputEn	= ENABLE;//CS有SPI硬件控制
	spiConfig.continuousCSEn= ENABLE;//片选连续模式
	spiConfig.dmaRxEn		= DISABLE;//禁止使用DMA接收数据
	spiConfig.dmaTxEn		= DISABLE;//禁止使用DMA发送数据
	spiConfig.modeFaultEn	= DISABLE;//模式故障禁止
	spiConfig.wakeUpEn		= DISABLE;//主机模式不支持唤醒功能
	spiConfig.spiEn 		= ENABLE;
	spiConfig.callBack		= NULL;
	spiConfig.interruptEn		= DISABLE;//使能NVIC中断
	spiConfig.txUFInterruptEn 	= DISABLE;//打开TXUF中断,可禁止
	spiConfig.rxOFInterruptEn 	= DISABLE;//打开RXOF中断,可禁止
	spiConfig.modeFaultInterruptEn = DISABLE;//模式故障中断
	SPI_Init(SPI0, &spiConfig);
}

/**
* @prototype SPI_Transmit1KBytes(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 发送1024字节数据.
*/
void SPI_Transmit1KBytes(void)
{
	uint8_t tempCmdBuf[4] = {0xAC, 0x78, 0x01, 0x53};
	
	/*准备数据.*/
	for (uint16_t ii = 0; ii < SPI_TST_BUF_LEN; ii++)
	{
		g_tstDataBuf[ii] = ii;
		printf("%d ", g_tstDataBuf[ii]);
	}
	printf("\r\n");
	
	/*发送指令.*/
	while(!g_slvDataRdyCur);//等待从机准备好接收指令
	g_slvDataRdyCur = FALSE;
	SPI_TransmitPoll(SPI0, tempCmdBuf, SPI_CMD_BUF_LEN);
	/*发送数据.*/
	while(!g_slvDataRdyCur);//等待从机准备好接收数据
	g_slvDataRdyCur = FALSE;
	SPI_TransmitPoll(SPI0, g_tstDataBuf, SPI_TST_BUF_LEN);
}

/**
* @prototype SPI_Receive1KBytes(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 接收1024字节数据.
*/
void SPI_Receive1KBytes(void)
{
	uint8_t tempCmdBuf[4] = {0xAC, 0x78, 0x01, 0x52};
	
	/*清空缓冲.*/
	memset(g_tstDataBuf, 0x00, SPI_TST_BUF_LEN);
	
	/*发送指令.*/
	while(!g_slvDataRdyCur);//等待从机准备好接收指令
	g_slvDataRdyCur = FALSE;
	SPI_TransmitPoll(SPI0, tempCmdBuf, SPI_CMD_BUF_LEN);
	
	/*接收数据.*/
	while(!g_slvDataRdyCur);//等待从机准备好接收数据
	g_slvDataRdyCur = FALSE;
	SPI_ReceivePoll(SPI0, g_tstDataBuf, SPI_TST_BUF_LEN);
	
	/*打印数据.*/
	for (uint16_t ii = 0; ii < SPI_TST_BUF_LEN; ii++)
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
* @brief  	 外部中断回调函数.
*/
void EXTI3_8_Callback(void *device, uint32_t wpara, uint32_t lpara)
{
	if ((device == SPI_SLV_RDY_PORT) && (wpara == SPI_SLV_RDY_PIN))
	{
		g_slvDataRdyCur = TRUE;
	}
}

/*************<end>********************/
