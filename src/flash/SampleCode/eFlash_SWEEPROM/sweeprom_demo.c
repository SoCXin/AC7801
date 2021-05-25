/* Copyright Statement:
 *
 * This software/firmware and related documentation "Autochips Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MEDIATEK Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited
 *
 * Copyright(C) 2020, Autochips ( All rights reserved. )
 *
 * BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 * NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
 * SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 * BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
 * LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
 *
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
 * WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
 * LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
 * RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
 * THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
 *****************************************************************************/

/*!
* @file sweeprom_demo.c
*
* @brief This file provides sweeprom demo test function.
*
*/

/* ===========================================  Includes  =========================================== */
#include <stdlib.h>
#include "ac780x_sweeprom.h"
#include "ac780x_debugout.h"

/* ============================================  Define  ============================================ */
#define SWEEPROM_SIZE           (5120UL)       /* Ssoftware eeprom size(Byte) */
#define TEST_WORDS  50
/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

uint32_t g_testA[TEST_WORDS] = {0};/* Buffer for software eeprom read and write */
uint32_t g_testReadA[TEST_WORDS] = {0};
/* ====================================  Functions declaration  ===================================== */







/* ======================================  Functions define  ======================================== */
/**
 * @brief elfash overnight test
 *
 * @param[in] none
 * @return none
 */
void SWEEPROMDemoTest()
{
    uint32_t sweepromAddr = 0;
    uint32_t i , j= 0;
    uint16_t tempData = 0;
    uint16_t tmpCircle = 0;
  
    printf("sweeprom demo test\r\n");
    EFLASH_UnlockCtrl();

    /* initialize eeprom size 1KB, locate in the 62th page in eflash */
    SWEEPROM_Init(SWEEPROM_SIZE, 32);
    if (EFLASH_STATUS_SUCCESS == SWEEPROM_Erase())
    {
        for (sweepromAddr = 0; sweepromAddr < SWEEPROM_SIZE-2; sweepromAddr++)
        {
            SWEEPROM_Read(sweepromAddr, &tempData, 1);
            if (0xFFFF != tempData)
            {
                printf("SWEEPROM erase error! sweepromAddr:0x%x\r\n", sweepromAddr);
                break;
            }
        }
    }
    else
    {
        printf("SWEEPROM_Erase Error!\r\n");
    }
        
     while(j < 1000000)  
     {
        for(i = 0; i < TEST_WORDS; i++)
        {
            g_testA[i] = i + tmpCircle;  
            //实际使用中，只需要写入最新需要更新的那一次数据
            if (EFLASH_STATUS_SUCCESS != SWEEPROM_Write(0 + i*2, (uint16_t *)&g_testA[i], 2))
            {
                printf("SWEEPROM_Write error! sweepromAddr:0x%x\n", sweepromAddr);
                while (1);
            }
        }
            
        SWEEPROM_Read(0,  (uint16_t *)g_testReadA, TEST_WORDS * 2);
            
        for(i = 0; i < TEST_WORDS; i++)
        {
            if(g_testA[i] != g_testReadA[i])
            {
                 printf("Fail, g_testA[%d] = %d , g_testReadA[%d] = %d\r\n", i, g_testA[i], i, g_testReadA[i]);
            }
        } 
          
        
        if(j % 100 == 0)
        {
            printf("==write times=%d==\r\n", j);
        }
        
        j++;
    }
    
    while(1);
}


/**
* @brief main function
*
* @param[in] none
* @return none
*/
int main(void)
{
    InitDebug();
    InitDelay();
    //sweepromDemoTest();
    
    SWEEPROMDemoTest();
    while (1);
}
