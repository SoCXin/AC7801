/* Copyright Statement:
 *
 * This software/firmware and related documentation "Autochips Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Autochips Inc. and/or its licensors.
 * Without the prior written permission of Autochips inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of Autochips Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * Copyright(C) 2020, Autochips ( All rights reserved. )
 *
 * BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("Autochips SOFTWARE")
 * RECEIVED FROM Autochips AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
 * AN "AS-IS" BASIS ONLY. Autochips EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES Autochips PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE Autochips SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. Autochips SHALL ALSO
 * NOT BE RESPONSIBLE FOR ANY Autochips SOFTWARE RELEASES MADE TO BUYER'S
 * SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 * BUYER'S SOLE AND EXCLUSIVE REMEDY AND Autochips'S ENTIRE AND CUMULATIVE
 * LIABILITY WITH RESPECT TO THE Autochips SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT Autochips'S OPTION, TO REVISE OR REPLACE THE Autochips SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
 *
 * Autochips FOR SUCH Autochips SOFTWARE AT ISSUE.
 *
 * THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
 * WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
 * LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
 * RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
 * THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
 *****************************************************************************/

/*!
* @file ac780x_sweeprom.c
*
* @brief This file provides software eeprom integration functions.
*
*/

/* ===========================================  Includes  =========================================== */
#include "ac780x_sweeprom.h"

/* ============================================  Define  ============================================ */
#define OPERATE_EEPROM_AREA_FLAG     0x00000000UL                  /* write or read area flag */
#define COPY_DATA_COMPLETE_FLAG      0x00000000UL                  /* current page data copy complete flag*/
#define EEPROM_SPACE_FLAG_OFFSET     4UL                           /* offset address in eflash page */
#define EEPROM_COPY_FLAG_OFFSET     (EEPROM_SPACE_FLAG_OFFSET + 4) /* the offset for saveing copy complete flag */
#define EEPROM_END_ADDRESS_OFFSET   (EEPROM_COPY_FLAG_OFFSET + 4)  /* eeprom end address offset */

/*!
* @brief The assert_param macro is used for function's check.
*
* @param  expr: If x is false, it calls assert_failed function which reports 
*         the name of the source file and the source line number of the call 
*         that failed. If x is true, it returns no value.
* @retval none
*/
#ifdef ATC_DEVICE_ASSERT
    #define DEVICE_ASSERT(x) ((x) ? (void)0 : (void)printf("[ASSERT]Detect error on %s %d\n", __FILE__, __LINE__) )
#elif CUSTOM_DEVICE_ASSERT
    /* If the CUSTOM_DEVICE_ASSERT symbol is defined, then add the custom implementation */
#else
    #define DEVICE_ASSERT(x) ((void)0)
#endif
    
/* ==========================================  Variables  =========================================== */
static uint32_t s_eepromSize = 512UL;  /* eeprom default size 512 half word ,actual use one page eflash */
static uint32_t s_eepromPages = 1UL;   /* eeprom default use one page eflash */
static uint32_t s_eepromStartAddr = EFLASH_END_ADDRESS - EFLASH_PAGE_SIZE; /* eeprom start address in eflash */


/* ======================================  Functions define  ======================================== */
/*!
 * @brief software eeprom initialize,actual available space are (size - 2)
 *
 * @param[in] size: config software eeprom size(half word)
 * @param[in] pageIndex: the start page index in eflash occupied by software eeprom
 * @return none
 */
void SWEEPROM_Init(uint16_t size, uint32_t pageIndex)
{
    DEVICE_ASSERT(pageIndex * EFLASH_PAGE_SIZE + (size <<2) <= EFLASH_SIZE - EFLASH_PAGE_SIZE);
    
    s_eepromSize = size;
    s_eepromStartAddr = EFLASH_BASE_ADDRESS + pageIndex * EFLASH_PAGE_SIZE;
    s_eepromPages  = s_eepromSize / (EFLASH_PAGE_SIZE >> 2);
    if (s_eepromSize % (EFLASH_PAGE_SIZE >> 2))
    {
        s_eepromPages = s_eepromPages + 1;
        s_eepromSize = s_eepromPages * (EFLASH_PAGE_SIZE >> 2);
    }
}

/*!
 * @brief write data to eflash
 *
 * @param[in] writeFlashStartAddr: write eflash start address
 * @param[in] eepromAddr: software eeprom address
 * @param[in] dataBuffer: data pointer
 * @param[in] writeNum: write half word number
 * @return statusRes: software eeprom write status
 */
EFLASH_StatusType WriteDataToEflash(uint32_t writeFlashStartAddr, uint16_t eepromAddr, const uint16_t *dataBuffer, uint16_t writeNum)
{
    EFLASH_StatusType statusRes = EFLASH_STATUS_SUCCESS;
    uint32_t newCombineData = 0;
    uint32_t tempWriteAddr = 0;
    uint32_t i = 0;

    for (i = 0; i < writeNum; i++)
    {
        tempWriteAddr = writeFlashStartAddr + i * 4;
        newCombineData = ((eepromAddr + i) << 16) + dataBuffer[i];
        statusRes = EFLASH_PageProgram(tempWriteAddr, &newCombineData, 1);
        if (EFLASH_STATUS_SUCCESS != statusRes)
        {
            break;
        }
    }

    return statusRes;
}

/*!
 * @brief look for the first data which is not 0xFFFFFFFF form end to start address
 *
 * @param[in] endAddr: end address
 * @param[in] validDataAddr: valid data start address
 * @return 0: no valid data, 1: found valid data
 */
static uint8_t GetValidDataStartAddr(uint32_t endAddr, uint32_t *validDataAddr)
{
    uint32_t i = 0;
    uint8_t ret = 0;

    for (i = 0; i < ((s_eepromPages * EFLASH_PAGE_SIZE) >> 2) - 2; i++)
    {
        if (0xffffffff !=  *((uint32_t *)(endAddr - i * 4)))
        {
            ret = 1;
            break;
        }
    }

    *validDataAddr = endAddr - i * 4;

    return ret;
}

/*!
 * @brief Check data whether valid
 *
 * @param[in] newAreaCheckStartAddr: new software eeprom space check start address
 * @param[in] oldAreaCheckStartAddr: old software eeprom spacecheck start address, high 24 bits are address, low 8 bits are data
 * @param[in] checkLength: the valid data length of backup software eeprom
 * @return isDataValid: the chacked data whether valid
 */
static BOOL_Type IsDataValid(uint32_t newAreaCheckStartAddr, uint32_t *oldAreaCheckStartAddr, uint32_t checkLength)
{
    BOOL_Type isDataValid = TRUE;
    uint32_t i = 0;

    for (i = 0; i < checkLength; i++)
    {
        if (((*(uint32_t *)(newAreaCheckStartAddr - i * 4)) >> 16) == ((*(uint32_t *)(*oldAreaCheckStartAddr)) >> 16))
        {
            isDataValid = FALSE;
        }
    }

    return isDataValid;
}

/*!
 * @brief get data from software eeprom
 *
 * @param[in] eepromEndAddr: software eeprom end address
 * @param[in] eepromDataLength: software eeprom data length
 * @param[in] readAddr: read software eeprom address
 * @param[in] data: read data from software eeprom
 * @return none
 */
static void GetDataFromEEPROM(uint32_t eepromEndAddr, uint32_t eepromDataLength, uint16_t readAddr, uint16_t *data)
{
    uint32_t i = 0;

    *data = 0xFFFF;
    for (i = 0; i < eepromDataLength; i++)
    {
        if ((*(uint32_t *)((eepromEndAddr - i * 4)) >> 16) == readAddr)
        {
           *data = *(uint16_t *)((eepromEndAddr - i * 4));
           break;
        }
    }
}

/*!
 * @brief copy valid data to backup area
 *
 * @param[in] newAreaStartAddr: new software eeprom start address
 * @param[in] oldAreaStartAddr: old software eeprom start address
 * @return statusRes: copy data status
 */
static EFLASH_StatusType CopyDataToNewArea(uint32_t newAreaStartAddr, uint32_t oldAreaStartAddr)
{
    EFLASH_StatusType statusRes = EFLASH_STATUS_SUCCESS;
    uint32_t oldAreaCopyStartAddr = 0;
    uint32_t newAreaCopyStartAddr = 0;
    uint32_t newAreaDataLen = 0;
    uint32_t oldAreaDataLen = 0;
    uint32_t checkStartAddr = 0;
    uint32_t i = 0;

    checkStartAddr = newAreaStartAddr + s_eepromPages * EFLASH_PAGE_SIZE - EEPROM_END_ADDRESS_OFFSET;
    if(GetValidDataStartAddr(checkStartAddr, &newAreaCopyStartAddr))
    {
        newAreaDataLen = ((newAreaCopyStartAddr - newAreaStartAddr) >> 2) + 1;
    }
    checkStartAddr = oldAreaStartAddr + s_eepromPages * EFLASH_PAGE_SIZE - EEPROM_END_ADDRESS_OFFSET;
    if(GetValidDataStartAddr(checkStartAddr, &oldAreaCopyStartAddr))
    {
         /* The last used data not 0xFFFFFFFF, src copy address no need decrease 4*/
        oldAreaDataLen = ((oldAreaCopyStartAddr - oldAreaStartAddr) >> 2) + 1;
    }
    for (i = 0; i < oldAreaDataLen; i++)
    {
        /* check data effective */
        if (IsDataValid(newAreaCopyStartAddr, &oldAreaCopyStartAddr, newAreaDataLen))
        {
            /* caculate write eeprom backup start address */
            /* copy data to backup area*/
            statusRes = EFLASH_PageProgram(newAreaCopyStartAddr + 4, (uint32_t *)oldAreaCopyStartAddr, 1);
            if (EFLASH_STATUS_SUCCESS!= statusRes)
            {
                break;
            }
            newAreaDataLen++;
            newAreaCopyStartAddr = newAreaCopyStartAddr + 4;
        }
        oldAreaCopyStartAddr = oldAreaCopyStartAddr - 4;
    }

    return statusRes;
}
/*!
 * @brief update software eeprom operate address
 *
 * @param[in] newStartAddr: new eeprom area start address
 * @param[in] oldStartAddr: old eeprom area start address
 * @return statusRes: software eeprom write status
 */
static EFLASH_StatusType UpdateOperateAddr(uint32_t *newStartAddr, uint32_t *oldStartAddr)
{
    EFLASH_StatusType statusRes = EFLASH_STATUS_SUCCESS;
    uint32_t eepromEndAddr = 0;
    uint32_t  flag = COPY_DATA_COMPLETE_FLAG;

    eepromEndAddr = s_eepromStartAddr + s_eepromPages * EFLASH_PAGE_SIZE;
    if (OPERATE_EEPROM_AREA_FLAG == *(uint32_t *)(eepromEndAddr - EEPROM_SPACE_FLAG_OFFSET))
    {
        /* operate address in eeprom backup area */
        *newStartAddr = s_eepromStartAddr - s_eepromPages * EFLASH_PAGE_SIZE;
        *oldStartAddr = s_eepromStartAddr;
    }
    else
    {
        /* operate address in eeprom area */
        *newStartAddr = s_eepromStartAddr;
        *oldStartAddr = s_eepromStartAddr - s_eepromPages * EFLASH_PAGE_SIZE;
        eepromEndAddr = s_eepromStartAddr;
    }
    /* check data copy is successed last time, if not success, will copy data again to avoid power down suddenly*/
    if ((~flag) == *(uint32_t *) (eepromEndAddr - EEPROM_COPY_FLAG_OFFSET))
    {
        /* copy data again */
        statusRes = CopyDataToNewArea(*newStartAddr, *oldStartAddr);
        if (EFLASH_STATUS_SUCCESS == statusRes)
        {
            /* write copy complete flag in the eighth last position */
            statusRes = EFLASH_PageProgram(eepromEndAddr - EEPROM_COPY_FLAG_OFFSET, &flag, 1);
        }
    }

   return statusRes;
}

/*!
 * @brief software eeprom Write expand
 *
 * @param[in] writeAddr: software eeprom address
 * @param[in] dataBuffer: data buffer pointer
 * @param[in] writeNum: write half word number
 * @return statusRes: software eeprom write status
 */
EFLASH_StatusType SWEEPROM_WriteExt(uint16_t writeAddr, const uint16_t *dataBuffer, uint16_t writeNum)
{
    EFLASH_StatusType statusRes = EFLASH_STATUS_SUCCESS;
    uint32_t leftSize = 0;
    uint32_t newWriteStartAddr = 0;
    uint32_t oldWriteStartAddr = 0;
    uint32_t validDataStartAddr = 0;
    uint32_t flag = 0;
    uint8_t i = 0;

    statusRes = UpdateOperateAddr(&newWriteStartAddr, &oldWriteStartAddr);
    if (EFLASH_STATUS_SUCCESS != statusRes)
    {
        return statusRes;
    }
    /* because the last eflash address save the operate eeprom address flag, so
       form the last fifth eflash address check start page has writable enough space
    */
    if(GetValidDataStartAddr(newWriteStartAddr + s_eepromPages * EFLASH_PAGE_SIZE - EEPROM_END_ADDRESS_OFFSET, &validDataStartAddr))
    {
       leftSize = (newWriteStartAddr + s_eepromPages * EFLASH_PAGE_SIZE - EEPROM_END_ADDRESS_OFFSET - validDataStartAddr) >> 2; 
    }
    else
    {
        leftSize = (s_eepromPages * EFLASH_PAGE_SIZE - EEPROM_COPY_FLAG_OFFSET) >> 2;
    }

    /* check space is enough, every half word combine with write address will use 4 eflash address */
    if (leftSize >= writeNum)
    {
        /* has enough space */
        statusRes = WriteDataToEflash(validDataStartAddr + 4, writeAddr, dataBuffer, writeNum);
    }
    else
    {
        for (i = 0; i < s_eepromPages; i++)
        {
            /* erase backup eflash area, size equal the configraton of eeprom
               eflash backup position in front of the eeprom start position
            */
            statusRes = EFLASH_PageEraseVerify(oldWriteStartAddr + EFLASH_PAGE_SIZE * i);
            if (EFLASH_STATUS_SUCCESS != statusRes)
            {
                statusRes = EFLASH_PageErase(oldWriteStartAddr + EFLASH_PAGE_SIZE * i);
            }
            if (EFLASH_STATUS_SUCCESS != statusRes)
            {
                return statusRes;
            }
        }
        /* new data write in the back of the old data */
        statusRes = WriteDataToEflash(oldWriteStartAddr, writeAddr, dataBuffer, writeNum);
        /* write 0x00000000 to the last eflash 4 address to flag
           current operate is in eeprom backup area
        */
        if (EFLASH_STATUS_SUCCESS != statusRes)
        {
            return statusRes;
        }
        flag = OPERATE_EEPROM_AREA_FLAG;
        statusRes = EFLASH_PageProgram(newWriteStartAddr + s_eepromPages * EFLASH_PAGE_SIZE - EEPROM_SPACE_FLAG_OFFSET, &flag, 1);
        /* write current data to backup eflash area */
        if (EFLASH_STATUS_SUCCESS == statusRes)
        {
            /* copy effective data form eeprom to backup area */
            statusRes = CopyDataToNewArea(oldWriteStartAddr, newWriteStartAddr);
            if (EFLASH_STATUS_SUCCESS == statusRes)
            {
                /* write copy complete flag in the eighth last position */
                flag = COPY_DATA_COMPLETE_FLAG;
                statusRes = EFLASH_PageProgram(newWriteStartAddr + s_eepromPages * EFLASH_PAGE_SIZE - EEPROM_COPY_FLAG_OFFSET, &flag, 1);
            }
        }
    }

    return statusRes;
}

/*!
 * @brief software eeprom Write
 *
 * @param[in] writeAddr: software eeprom address
 * @param[in] dataBuffer: data buffer pointer
 * @param[in] writeNum: write half word number
 * @return statusRes: software eeprom write status
 */
EFLASH_StatusType SWEEPROM_Write(uint16_t writeAddr, const uint16_t *dataBuffer, uint16_t writeNum)
{
    EFLASH_StatusType statusRes = EFLASH_STATUS_SUCCESS;

    /* check write address is in eeprom size scope */
    DEVICE_ASSERT(s_eepromSize - 2 > writeAddr);
    DEVICE_ASSERT(dataBuffer != NULL);
    DEVICE_ASSERT(0 != writeNum);
    /* when debug suggest redefine function DEVICE_ASSERT, else it's easy happen error */
    DEVICE_ASSERT((s_eepromSize - 2) >= (writeAddr + writeNum));

    statusRes = EFLASH_UnlockCtrl();
    if (EFLASH_STATUS_SUCCESS != statusRes)
    {
        return statusRes;
    }
    statusRes = SWEEPROM_WriteExt(writeAddr, dataBuffer, writeNum);
    EFLASH_LockCtrl();

    return statusRes;
}

/*!
 * @brief software eeprom read
 *
 * @param[in] readAddr: Read start address of software software eeprom
 * @param[in] dataBuffer: Point to the buffer ready to read the data from software eeorom
 * @param[in] readNum: read half word number
 * @return software eeprom read status
 */
EFLASH_StatusType SWEEPROM_Read(uint16_t readAddr, uint16_t *dataBuffer, uint16_t readNum)
{
    EFLASH_StatusType statusRes = EFLASH_STATUS_SUCCESS;
    uint32_t newStartAddr = 0;
    uint32_t eepromDataLen = 0;
    uint32_t validDataStartAddr = 0;
    uint32_t i = 0;

    DEVICE_ASSERT(s_eepromSize > readAddr);
    DEVICE_ASSERT((s_eepromSize - 2) >= (readAddr + readNum));
    DEVICE_ASSERT(dataBuffer != NULL);
    /* check write address is eeprom area or backup area, eeprom last half word(eflash last
       4 address) are used save state information, if 0x00000000 use back area
    */
    if (OPERATE_EEPROM_AREA_FLAG == *(uint32_t *) (s_eepromStartAddr + s_eepromPages * EFLASH_PAGE_SIZE - EEPROM_SPACE_FLAG_OFFSET))
    {
        /* operate address in eeprom back area */
        newStartAddr = s_eepromStartAddr - s_eepromPages * EFLASH_PAGE_SIZE;
    }
    else
    {
        /* operate address in eeprom area */
        newStartAddr = s_eepromStartAddr;
    }
    /* check the atart address of valid data */
    if (GetValidDataStartAddr(newStartAddr + s_eepromPages * EFLASH_PAGE_SIZE - EEPROM_END_ADDRESS_OFFSET, &validDataStartAddr))
    {
        eepromDataLen = ((validDataStartAddr - newStartAddr) >> 2) + 1; 
    }

    for (i = 0; i < readNum; i++)
    {
        /* check data effective */
        GetDataFromEEPROM(validDataStartAddr, eepromDataLen, readAddr + i, &dataBuffer[i]);
    }

    return statusRes;
}

/*!
 * @brief software eeprom erase(include backup area)
 *
 * @param[in] none
 * @return statusRes: software eeprom erase status
 */
EFLASH_StatusType SWEEPROM_Erase()
{
    EFLASH_StatusType statusRes = EFLASH_STATUS_SUCCESS;
    uint8_t i = 0;

    statusRes = EFLASH_UnlockCtrl();
    if (EFLASH_STATUS_SUCCESS != statusRes)
    {
        return statusRes;
    }
    for (i = 0; i < s_eepromPages * 2; i++)
    {
        /* erase eeprom space and backup space*/
        statusRes = EFLASH_PageEraseVerify(s_eepromStartAddr + (s_eepromPages - 1 - i) * EFLASH_PAGE_SIZE);
        if (EFLASH_STATUS_SUCCESS != statusRes)
        {
            statusRes = EFLASH_PageErase(s_eepromStartAddr + (s_eepromPages - 1 - i) * EFLASH_PAGE_SIZE);
            if (EFLASH_STATUS_SUCCESS != statusRes)
            {
                break;
            }
        }
    }
    EFLASH_LockCtrl();

    return statusRes;
}
/* =============================================  EOF  ============================================== */
