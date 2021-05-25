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
* AutoChips Inc. (C) 2016. All rights reserved.
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

#ifndef SDK_FLASH_H_
#define SDK_FLASH_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "ac780x.h"
#define SDK_DEBUG			1
#define SDK_DEBUG_ERR		1
#define SDK_DEBUG_WARNING	1
#define SDK_DEBUG_CHECK		1
#define DATA_START_ADDR    		0x08005800// //BOOT后面2K用于存放数据
#define DATA_SIZE     			(0x800)    /* 2 Kbytes */

typedef struct {
	uint8_t canBootloaderUpgrade;   //CAN升级标志位
	uint8_t test1;
	uint8_t test2;

	
} SystemDataRecord;//这个结构体需与APP同步

typedef enum{
	TURN_LAMP_OK = 0,
	LACK_ONE_LAMP,
	LACK_TWO_LAMP,
	SHORT_OUT_LAMP,//两个转向灯并联，有一个短路，另一个也短路
}TurnLampError;

typedef enum{
	NORMAL_ON = 0,
	CAN_BOOTLOADER_UPGRADE,
}UpgradeFlag;

int DataRecord_Read(uint32_t readAddr, uint32_t *dataBuffer, uint32_t numbToRead);
void DataRecord_InitData(void) ;
void DataRecord_Test(void);
int DataRecord_WriteUpgrade(uint8_t upgradeFlag);
#ifdef __cplusplus
}
#endif
#endif /* SDK_FLASH_H_ */

