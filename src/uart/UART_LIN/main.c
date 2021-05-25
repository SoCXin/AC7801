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
#include <stdio.h>
#include <string.h>

#include "ac780x_debugout.h"
#include "ac780x_uart_lin.h"

#define GPIO_KEY6   GPIOC,GPIO_PIN6
#define GPIO_KEY7   GPIOA,GPIO_PIN11

extern uint8_t g_data1[];
extern uint8_t g_data2[];
extern uint8_t g_data4[];
extern uint8_t g_data8[];

/**
 * main
 *
 * @param[in] : none
 * @return : none
 *
 * @brief  uart lin demo main function
 */
int main(void)
{
    uint8_t key6 = 1, key7 = 1;
    uint8_t preKey6 = 1, preKey7 = 1;
    uint8_t mode = MASTER_MODE;
    uint8_t preData1 = g_data1[0];
    uint8_t data1 = 0;
    uint16_t preData2 = *(uint16_t *)&g_data2[0];
    uint16_t data2 = 0;

    InitDelay();
    InitDebug();

    if (mode == MASTER_MODE)
    {
        printf("Uart LIN Master Demo Test\r\n");
        LIN_InitLin(MASTER_MODE, 0, 19200);    // for master
    }
    else
    {
        printf("Uart LIN Slave Demo Test\r\n");
        LIN_InitLin(SLAVE_MODE, 3, 19200);    // for salve
    }

    // Configure key 6
    GPIO_SetFunc(GPIO_KEY6, GPIO_FUN0);
    GPIO_SetDir(GPIO_KEY6, GPIO_IN);

    // Configure key 7
    GPIO_SetFunc(GPIO_KEY7, GPIO_FUN0);
    GPIO_SetDir(GPIO_KEY7, GPIO_IN);

    while (1)
    {
        key6 = GPIO_GetPinLevel(GPIO_KEY6);
        if (key6 == 0 && key6 != preKey6)  // Press key 6
        {
            preKey6 = key6;
            printf("key6\r\n");
            if (mode == MASTER_MODE)
            {
                g_data1[0] += 1;
            }
            else
            {
                g_data2[0] += 1;
            }
        }
        else if (key6 == 1)
        {
            preKey6 = key6;
        }
        key7 = GPIO_GetPinLevel(GPIO_KEY7);
        if (key7 == 0 && key7 != preKey7)  // Press key 7
        {
            preKey7 = key7;
            printf("key7\r\n");
            if (mode == MASTER_MODE)
            {
                g_data4[0] += 1;
            }
            else
            {
                g_data8[0] += 1;
            }
        }
        else if (key7 == 1)
        {
            preKey7 = key7;
        }
        
        if (mode == MASTER_MODE)
        {
            data2 = *(uint16_t *)&g_data2[0];
            if (data2 != preData2)  // Signal change
            {
                printf("Slave key6\r\n");
                preData2 = data2;
            }
        }
        else
        {
            data1 = g_data1[0];
            if (data1 != preData1)  // Signal change
            {
                printf("Master key6\r\n");
                preData1 = data1;
            }
        }

        mdelay(10);
    }
}
