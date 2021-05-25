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
/*******************************************************************************
*
* @file main.c
*
* @author AutoChips
*
* @version 0.0.1
*
* @date Aug.16.2018
*
* @brief BOOT implement
*
*******************************************************************************/
#include "ac780x.h"
#include "sdk_flash.h"
#include "sdk_system.h"
#include "ac780x_eflash.h"
#include "can_bootloader.h"
#include "ac780x_debugout.h"
#define BCM_BOOT_VERSION "BOOT_V1.0_2018_10_08 10:00"
#define MCU_R_WAS			35//


extern CBL_CMD_LIST g_cmdList;
extern CAN_MsgInfoType g_canRxMessage;
extern volatile uint8_t g_canRxMsgFlag;
extern uint16_t g_canAddr;
extern uint16_t g_updateModeCnt;
SystemDataRecord *g_systemDataRecord;
uint8_t g_eflashRWBuffer[DATA_SIZE] = {0x00};
void LED_Init(void)
{
    /* init gpio as output mode */
    GPIO_SetFunc(LED2, GPIO_FUN0);
    GPIO_SetDir(LED2, GPIO_OUT);
}
/**
 * ReadUpgradeFlag
 *
* @param[in] 
* @return  none
 *
 * @brief  read upgrade flag from flash
 */
void ReadUpgradeFlag(void)
{
    g_systemDataRecord = (SystemDataRecord *)g_eflashRWBuffer;
    DataRecord_Read(DATA_START_ADDR, (uint32_t *)g_systemDataRecord, 512);
//        g_systemDataRecord->canBootloaderUpgrade = CAN_BOOTLOADER_UPGRADE;
    if (g_systemDataRecord->canBootloaderUpgrade == CAN_BOOTLOADER_UPGRADE)
    {
        printf("\r\n enter CAN bootloader upgrade mode\r\n");
        //goToApp();
        Timer1_Config(ENABLE,ENABLE);
        g_updateModeCnt = 1;
        while (1)
        {
            if (g_canRxMsgFlag)
            {
                g_canRxMsgFlag = 0;
                CAN_BootExecutiveCommand(&g_canRxMessage);
            }
            if (g_updateModeCnt > 60)//30s
            {
                printf("\r\n update timeout ,exit updateMode!if you want to go on,please manual reset MCU\r\n");
                goToApp();
            }
        }
    }
    else
    {
        goToApp();
    }
}

int main(void) 
{
    EnableInterrupts//开全局中断
    //CAN_Type *CANx = CAN1;
    //TRANSMIT_TYPE type = TRANSMIT_SECONDARY;
    InitDebug();
    InitDelay();
    LED_Init();
    printf("\r\nBoot Version:%s\r\n", BCM_BOOT_VERSION);
    System_GetCompileTime();
    if (BOOT_DEBUG)printf("welcome to AC7801 \r\n");
    CAN_Config();
    g_cmdList.OnlineCheck = ONLINE_CHECK;
    g_cmdList.EraseFlash = ERASE_FLASH;
    g_cmdList.SetBaudRate = SET_BAUDRATE;
    g_cmdList.BlockWriteInfo = BLOCK_WRITE_INFO;
    g_cmdList.WriteBlockFlash = WRITE_BLOCK_FLASH;
    g_cmdList.BlockReadInfo = BLOCK_READ_INFO;
    g_cmdList.ReadBlockFlash = READ_BLOCK_FLASH;
    g_cmdList.ExcuteApp = EXCUTE_APP;
    g_cmdList.CmdFaild = CMD_FAILD;
    g_cmdList.EnterUpdateMode = ENTER_UPDATE_MODE;
    g_cmdList.CmdSuccess = CMD_SUCCESS;
    g_canRxMsgFlag =1;
    g_canAddr= CAN_BootGetAddrData();
    //goToApp();
    ReadUpgradeFlag();
}


