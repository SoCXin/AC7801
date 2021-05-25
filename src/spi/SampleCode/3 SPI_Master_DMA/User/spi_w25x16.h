
/* Copyright Statement:
 *
 * This software/firmware and related documentation ("AutoChips Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to AutoChips Inc. and/or its licensors. Without
 * the prior written permission of AutoChips inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of AutoChips Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * AutoChips Inc. (C) 2018. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("AUTOCHIPS SOFTWARE")
 * RECEIVED FROM AUTOCHIPS AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. AUTOCHIPS EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES AUTOCHIPS PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE AUTOCHIPS SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN AUTOCHIPS
 * SOFTWARE. AUTOCHIPS SHALL ALSO NOT BE RESPONSIBLE FOR ANY AUTOCHIPS SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AUTOCHIPS'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE AUTOCHIPS SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT AUTOCHIPS'S OPTION, TO REVISE OR REPLACE THE
 * AUTOCHIPS SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO AUTOCHIPS FOR SUCH AUTOCHIPS SOFTWARE AT ISSUE.
 */

/*************<start>******************/
#ifndef __SPI_W25X16_H__
#define __SPI_W25X16_H__

#ifdef __cplusplus
extern "C" {
#endif

/*************<include>****************/
#include "ac780x_spi.h"

/*************<macro>******************/
#define W25X16_BUSY_MSK			(0x01)
#define W25X16_WEL_MSK			(0x02)
	
#define W25X16_CMD_WR_EN		(0x06)//写使能指令
#define W25X16_CMD_WR_DIS		(0x07)//写禁能指令
#define W25X16_CMD_RD_STS		(0x05)//读状态寄存器
#define W25X16_CMD_WR_STS		(0x01)//写状态寄存器
#define W25X16_CMD_RD_DAT		(0x03)//读取数据
#define W25X16_CMD_FR_DAT		(0x0B)//快速读取
#define W25X16_CMD_WR_DAT		(0x02)//页编程
#define W25X16_CMD_ER_SEC		(0x20)//擦除sector
#define W25X16_CMD_ER_BLK		(0xD8)//擦除block
#define W25X16_CMD_ER_CHP		(0xC7)//整片擦除

#define W25X16_SEC_SIZE			(4096)//4KB
#define W25X16_ADDR_MAX			(0x200000)

#define SPI1_TX_DMA_CHN			(DMA0_CHANNEL0)//SPI发送DMA通道
#define SPI1_RX_DMA_CHN			(DMA0_CHANNEL1)//SPI接收DMA通道


/*************<enum>*******************/


/*************<union>******************/


/*************<struct>*****************/


/*************<extern>*****************/	
extern uint8_t		g_flashWrRdRdy;
extern uint16_t		g_flashWrRdTime;//读取写入计时

/*************<prototype>**************/
void SPI_ParaInit(void);

void W25X16_ReadDataTest(void);
void W25X16_WriteDataTest(void);

void W25X16_ErsSector (uint32_t startAddr, uint8_t secErNum);
void W25X16_ReadBytes (uint32_t startAddr, uint8_t *pDataBuf, uint16_t dataLen);
void W25X16_WriteBytes(uint32_t startAddr, uint8_t *pDataBuf, uint16_t dataLen);


#ifdef __cplusplus
}
#endif

#endif /* __SPI_W25X16_H__ */

/*************<end>********************/
