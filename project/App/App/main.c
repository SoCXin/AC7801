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
* @date Oct.31.2018
*
* @brief main implement
*
*******************************************************************************/
#include "ac780x.h"
#include "sdk_system.h"
#include "ac780x_debugout.h"
#include "ac780x_gpio.h"
#include "sdk_flash.h"
/* define LED gpio */
#include "tasks.h"
#include "board_config.h"

SystemDataRecord *g_systemDataRecord;
uint8_t g_eflashRWBuffer[DATA_SIZE] = {0x00};
void LED_Init(void)
{
    /* init gpio as output mode */
    GPIO_SetFunc(LED2, GPIO_FUN0);
    GPIO_SetDir(LED2, GPIO_OUT);
}

int main(void) 
{
    Board_Config();
    LED_Init();
    Put_Timer2_CNT(1000, ENABLE);
    while(1)
    {  
		task();
    }
}

void HardFault_Handler(void)
{
    uint32_t sp = __get_MSP();

//    InitDebug();
    printf("Hardfault...\r\n");
    printf("sp=0x%08x\r\n", sp);
    printf("sp[0]=0x%08x\r\n", *((uint32_t *)(sp + 0x00))); 
    printf("sp[1]=0x%08x\r\n", *((uint32_t *)(sp + 0x04)));
    printf("sp[2]=0x%08x\r\n", *((uint32_t *)(sp + 0x08)));
    printf("sp[3]=0x%08x\r\n", *((uint32_t *)(sp + 0x0c)));
    printf("sp[4]=0x%08x\r\n", *((uint32_t *)(sp + 0x10)));
    printf("sp[5]=0x%08x\r\n", *((uint32_t *)(sp + 0x14)));
    printf("sp[6]=0x%08x\r\n", *((uint32_t *)(sp + 0x18)));
    printf("sp[7]=0x%08x\r\n", *((uint32_t *)(sp + 0x1c)));
    while(1);
}
