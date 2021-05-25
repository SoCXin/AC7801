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
* AutoChips Inc. (C) 2020. All rights reserved.
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

/*!
 * @file ac780x_debugout.c
 *
 * @brief This file provides debug information output integration functions.
 *
*/
/* ===========================================  Includes  =========================================== */
#include "ac780x.h"
#include "ac780x_gpio.h"
#include "ac780x_uart.h"
#include "ac780x_uart_reg.h"
/* ============================================  Define  ============================================ */
#define MAX_DEBUG_BUFF_SIZE 100             /* define uart max receive buffer size */

#define DEBUG_UART           UART2          /* define uart2 for debug output   */
#define DEBUG_UART_IRQ       UART2_IRQn     /* define uart2 interrupt vector   */
#define DEBUG_UART_CLK       CLK_UART2      /* define uart2 ckgen clock value  */
#define DEBUG_UART_SRST      SRST_UART2     /* define uart2 ckgen reset value  */
#define DEBUG_UART_SMP       16.0F
#define DEBUG_UART_BAUDRATE  115200.0F

#define DEBUG_UART_TX     GPIOB,GPIO_PIN9   /* define uart2 tx gpio */
#define DEBUG_UART_RX     GPIOB,GPIO_PIN10  /* define uart2 rx gpio */
/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* debug uart receive buf */
uint8_t g_debugBuff[MAX_DEBUG_BUFF_SIZE] = {0};

/* debug init flag */
static uint8_t s_debugInit = 0;

/* define std FILE struct */
struct __FILE
{
    int handle;
};

/* refine stdout,stdin,stderr */
FILE __stdout;
FILE __stdin;
FILE __stderr;
/* ====================================  Functions declaration  ===================================== */

/* ======================================  Functions define  ======================================== */

/*!
* @brief refine _sys_exit
*
* @param[in] x:  no use
* @return always 0
*/
int _sys_exit(int x)
{
    x = x;
    return 0;
}

/*!
* @brief refine _ttywrch
*
* @param[in] x:  no use
* @return always 0
*/
int _ttywrch(int x)
{
    x = x;
    return 0;
}

/*!
* @brief put a char to UART
*
* @param[in] f:  file pointer for the std input
* @param[in] ch: the char to put
* @return return the char of be put
*/
int fputc(int ch, FILE *f)
{
    if (s_debugInit)
    {
        UART_SendData(DEBUG_UART, ch);
    }
    return ch;
}

/*!
* @brief get a char
*
* @param[in] f: file pointer for the std input
* @return -1: not get char,  other: the char get from UART
*/
int fgetc(FILE* f)
{
    int ch = -1;
    if (s_debugInit)
    {
        if (UART_RxIsDataReady(DEBUG_UART))
        {
            ch = UART_ReceiveData(DEBUG_UART);
        }
    }

    return ch;
}

/*!
* @brief set the debug out is invalid
*
* @param[in] none
* @return none
*/
void DeinitDebug(void)
{
    s_debugInit = 0;
}

/*!
* @brief init debug out , and set the debug out is valid
*
* @param[in] none
* @return none
*/
void InitDebug(void)
{
#ifdef DEBUG_CMD_INTERRUPT
    NVIC_SetPriority(DEBUG_UART_IRQ, 3);
    NVIC_ClearPendingIRQ(DEBUG_UART_IRQ);
    NVIC_EnableIRQ(DEBUG_UART_IRQ);
#endif

    GPIO_SetFunc(DEBUG_UART_TX, GPIO_FUN3);
    GPIO_SetFunc(DEBUG_UART_RX, GPIO_FUN3);
    CKGEN_Enable(DEBUG_UART_CLK, ENABLE);
    CKGEN_SoftReset(DEBUG_UART_SRST, ENABLE);

    UART_SetDivisor(DEBUG_UART, (float)APB_BUS_FREQ / DEBUG_UART_SMP / DEBUG_UART_BAUDRATE);

    UART_SetDataBits(DEBUG_UART, UART_WORD_LEN_8BIT);
    UART_SetStopBit(DEBUG_UART, UART_STOP_1BIT);
    UART_EnableTX(DEBUG_UART, ENABLE);
    UART_EnableRX(DEBUG_UART, ENABLE);
    UART_Set2ByteFIFO(DEBUG_UART, ENABLE);

    UART_SetInterruptEn(DEBUG_UART, 9);

    s_debugInit = 1;
}

/* =============================================  EOF  ============================================== */
