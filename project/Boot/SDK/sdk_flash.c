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
* @file sdk_flash.c
*
* @author AutoChips
*
* @version 0.0.1
*
* @date Aug.1.2018
*
* @brief data region manager
*
*******************************************************************************/
#include "sdk_flash.h"
#include "ac780x_eflash.h"

extern SystemDataRecord *g_systemDataRecord;
extern uint8_t g_eflashRWBuffer[DATA_SIZE];
/**
 * DataRecord_InitData
 *
* @param[in] 
* @return  none
 *
 * @brief  init datarecord data
 */
void DataRecord_InitData(void)
{
    g_systemDataRecord->canBootloaderUpgrade = CAN_BOOTLOADER_UPGRADE;
    g_systemDataRecord->test1 = 0xaa;
    g_systemDataRecord->test2 = 0xbb;
}

/**
 * DataRecord_EraseData
 *
* @param[in] 
* @return  EFLASH_STATUS
 *
 * @brief erase datarecord block
 */
int DataRecord_EraseData(void)
{
    if (SDK_DEBUG)
    {
        printf("\r\n DataRecord_EraseData\r\n");
    }
    EFLASH_StatusType ret;
    //__set_PRIMASK(1);
    EFLASH_UnlockCtrl();//
    ret = EFLASH_PageErase(DATA_START_ADDR);
    if (SDK_DEBUG)
    {
        printf("\r\n DataRecord_EraseData: %d\r\n", ret);
    }
    EFLASH_LockCtrl();
    //__set_PRIMASK(0);
    return ret;
}
/**
 * DataRecord_Read
 *
* @param[in] readAddr,dataBuffer,numbToRead
* @return  EFLASH_STATUS
 *
 * @brief  read datarecord from flash
 */
int DataRecord_Read(uint32_t readAddr, uint32_t *dataBuffer, uint32_t numbToRead)
{

    if (SDK_DEBUG)
    {
        printf("\r\n DataRecord_Read numbToReaddataBuffer[0]:0x%x\r\n", dataBuffer[0]);
    }
    EFLASH_Read(readAddr, dataBuffer, numbToRead);
    if (SDK_DEBUG)
    {
        printf("\r\n DataRecord_Read numbToRead:%d, dataBuffer[0]:0x%x\r\n", numbToRead, dataBuffer[0]);
    }
    return 0;
}
/**
 * DataRecord_Write
 *
* @param[in] readAddr, dataBuffer, numbToWrite
* @return  EFLASH_STATUS
 *
 * @brief  write dataRecord to flash
 */
int DataRecord_Write(uint32_t readAddr, uint32_t *dataBuffer, uint32_t numbToWrite)
{
    if (SDK_DEBUG)
    {
        printf("\r\n DataRecord_Write numbToWrite:%d, data[0]:0x%x\r\n", numbToWrite, dataBuffer[0]);
    }
    EFLASH_StatusType ret;
    EFLASH_UnlockCtrl();
    ret =  EFLASH_PageProgram(readAddr, dataBuffer, numbToWrite);
    EFLASH_LockCtrl();
    if (SDK_DEBUG)
    {
        printf("\r\n DataRecord_Write: %d\r\n", ret);
    }
    return ret;
}

/**
 * DataRecord_WriteUpgrade
 *
* @param[in] upgradeFlag NORMAL_ON(0):normal start, CAN_BOOTLOADER_UPGRADE(1):bootloader upgrade
* @return  0, success.-1, fail, other:flash ctrl fail
 *
 * @brief  Unlock the eflash control
 */
int DataRecord_WriteUpgrade(uint8_t upgradeFlag)
{
    int ret;
    g_systemDataRecord = (SystemDataRecord *)g_eflashRWBuffer;
    DataRecord_Read(DATA_START_ADDR, (uint32_t *)g_systemDataRecord, 512);
    ret = DataRecord_EraseData();
    if (ret != EFLASH_STATUS_SUCCESS)
    {
        return ret;
    }
    g_systemDataRecord->canBootloaderUpgrade = upgradeFlag;
    ret = DataRecord_Write(DATA_START_ADDR, (uint32_t *)g_systemDataRecord, 512);
    if (ret != EFLASH_STATUS_SUCCESS)
    {
        return ret;
    }
    DataRecord_Read(DATA_START_ADDR, (uint32_t *)g_systemDataRecord, 512);
    if (g_systemDataRecord->canBootloaderUpgrade == upgradeFlag)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}
