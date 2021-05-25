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
#ifndef _AC78XX_UART_LIN_H_
#define _AC78XX_UART_LIN_H_

#include "ac780x.h"
#include "ac780x_uart_reg.h"
#include "ac780x_gpio.h"
#include "ac780x_uart.h"
#include "ac780x_timer.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    LIN_STATE_IDLE,                  ///< Idle state
    LIN_STATE_SEND_BREAK,            ///< Send break state
    LIN_STATE_RECEIVE_BREAK,         ///< Receive break state
    LIN_STATE_SEND_SYNC,             ///< Send sync state
    LIN_STATE_RECEIVE_SYNC,          ///< Receive sync state
    LIN_STATE_SEND_IDENTIFIER,       ///< Send identifier state
    LIN_STATE_RECEIVE_IDENTIFIER,    ///< Receive identifier state
    LIN_STATE_SEND_DATA,             ///< Send data state
    LIN_STATE_RECEIVE_DATA,          ///< Receive data state
    LIN_STATE_SEND_CHECKSUM,         ///< Send checksum state
    LIN_STATE_BUSSLEEP               ///< Bus sleep state
} lin_ifcState;

typedef struct {
    uint8_t  id;               ///< LIN message frame ID for this frame
    uint8_t  dir;              ///< Name of the node that publishes the data
    uint8_t  cst;              ///< Type of checksum to be used (enhanced or classic)
    uint8_t  dtl;              ///< Number of bytes in the data field
    uint16_t  delayTime;       ///< Actual slot time in ms for the frame
    uint8_t  *data;            ///< Address of the structure that lists the signals
} FRAME_ID_TBL;

typedef struct {
    uint8_t  frameNum;         ///< Number of frame slots in the schedule table
    FRAME_ID_TBL  *schedule;   ///< Address of the schedule table
} SCHEDULE_TBL;

#define UART_LIN        UART1
#define UART_LIN_IRQn   UART1_IRQn

///GPIO pin mux define
#define SWLIN_RX        GPIOC, GPIO_PIN3
#define RXPinFunc       GPIO_FUN1

#define SWLIN_TX        GPIOC, GPIO_PIN2
#define TXPinFunc       GPIO_FUN1

#define SWLIN_SLP       GPIOC, GPIO_PIN5

// Checksum type
#define    ENHANCED_CHECKSUM    0    ///< Enhanced checksum type
#define    CLASSIC_CHECKSUM     1    ///< Classic checksum type

// Direction
#define    DIR_RX               0    ///< receive direction
#define    DIR_TX               1    ///< transmit direction

// Node mode
#define    MASTER_MODE          0    ///< master mode
#define    SLAVE_MODE           1    ///< slave mode


#define LIN_TIME_BASE           5        ///< base time 5 ms
#define LIN_TIMER_CHANNEL       TIMER_CHANNEL1   ///< Use timer 1

void LIN_InitLin(uint8_t mode, uint8_t schTblIdx, uint16_t baudrate);

#ifdef __cplusplus
}
#endif

#endif
