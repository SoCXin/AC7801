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

#include <string.h>
#include "ac780x_uart_lin.h"


// Signal data  4字节强制对齐，防止非对齐访问产生hardfault
uint8_t g_data8[8]__attribute__((aligned(4))) = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
uint8_t g_data4[4]__attribute__((aligned(4))) = {0xAA, 0x55, 0xAA, 0x55};
uint8_t g_data2[2]__attribute__((aligned(4))) = {0xFF, 0x00};
uint8_t g_data1[1]__attribute__((aligned(4))) = {0xA5};

///< Master send header and data
FRAME_ID_TBL g_frameTable1[] =
{
    {0x01, 1, 0, 1, 60, &g_data1[0]},
    {0x02, 1, 0, 2, 60, &g_data2[0]},
    {0x03, 1, 0, 4, 60, &g_data4[0]},
    {0x04, 1, 0, 8, 60, &g_data8[0]},
};

///< Master send header and receive data
FRAME_ID_TBL g_frameTable2[] =
{
    {0x01, 1, 0, 1, 60, &g_data1[0]},
    {0x02, 0, 0, 2, 60, &g_data2[0]},
    {0x03, 1, 0, 4, 60, &g_data4[0]},
    {0x04, 0, 0, 8, 60, &g_data8[0]},
};

///< Slave receive header and data
FRAME_ID_TBL g_frameTable3[] =
{
    {0x01, 0, 0, 1, 60, &g_data1[0]},
    {0x02, 0, 0, 2, 60, &g_data2[0]},
    {0x03, 0, 0, 4, 60, &g_data4[0]},
    {0x04, 0, 0, 8, 60, &g_data8[0]},
};

///< Slave receive header and response data
FRAME_ID_TBL g_frameTable4[] =
{
    {0x01, 0, 0, 1, 60, &g_data1[0]},
    {0x02, 1, 0, 2, 60, &g_data2[0]},
    {0x03, 0, 0, 4, 60, &g_data4[0]},
    {0x04, 1, 0, 8, 60, &g_data8[0]},
};

///< Master classical checksum
FRAME_ID_TBL g_frameTable5[] =
{
    {0x01, 1, 1, 1, 60, &g_data1[0]},
    {0x02, 0, 1, 2, 60, &g_data2[0]},
    {0x03, 1, 1, 4, 60, &g_data4[0]},
    {0x04, 0, 1, 8, 60, &g_data8[0]},
};

///< Slave classical checksum
FRAME_ID_TBL g_frameTable6[] =
{
    {0x01, 0, 1, 1, 60, &g_data1[0]},
    {0x02, 1, 1, 2, 60, &g_data2[0]},
    {0x03, 0, 1, 4, 60, &g_data4[0]},
    {0x04, 1, 1, 8, 60, &g_data8[0]},
};

///< Schedule table
SCHEDULE_TBL g_scheduleTable[] =
{
    {4, &g_frameTable1[0]},
    {4, &g_frameTable2[0]},
    {4, &g_frameTable3[0]},
    {4, &g_frameTable4[0]},
    {4, &g_frameTable5[0]},
    {4, &g_frameTable6[0]},
};

uint32_t g_sendTimes = 0xFFFF; ///< Send frame times

uint8_t g_mode = 0;          ///< Master or slave mode
uint8_t g_state = 0;         ///< Lin state
uint8_t g_schTblIdx = 0;     ///< Schedule table index
uint8_t g_frameIdx = 0;      ///< Frame index
uint8_t g_protectId = 0;     ///< Protect id
uint8_t g_firstFlag = 1;     ///< First time flag
uint8_t g_delay = LIN_TIME_BASE;         ///< Entry delay time

uint8_t g_direction = 0;     ///< Send direction
uint8_t g_tranByte = 0;      ///< Need transmit bytes
uint8_t g_recvByte = 0;      ///< Need receive bytes
uint8_t g_txCount = 0;       ///< Transmit data count
uint8_t g_rxCount = 0;       ///< Receive data count
uint8_t g_autoBaud = 1;      ///< Auto baud rate
uint8_t g_breakThre = 1;     ///< Break threshold
uint8_t g_buffer[10] = {0};  ///< Data buffer

#define LIN_LOG 1
#if LIN_LOG
#define fprintf(fmt...) printf(fmt)
#else
#define fprintf(fmt...)
#endif

#define DATA_AND_CHECKSUM 1        ///< Data and checksum send together

void UartLIN_Handler(void *device, uint32_t wpara, uint32_t lpara);

/**
 * LIN_MakeProtId
 *
 * @param[in]  idData: Identifier
 * @return : protected Identifier
 *
 * @brief  Make protected Identifier
 */
uint8_t LIN_MakeProtId(uint8_t idData)
{
    union {
        uint8_t  byte;
        struct {
            uint8_t  d0: 1;
            uint8_t  d1: 1;
            uint8_t  d2: 1;
            uint8_t  d3: 1;
            uint8_t  d4: 1;
            uint8_t  d5: 1;
            uint8_t  p0: 1;
            uint8_t  p1: 1;
        } bit;
    } buf;

    /* Load the desired message frame ID into the LSBits of the buffer.  */
    buf.byte = idData & (uint8_t)0x3F;
    /* Set the two parity bits.  */
    buf.bit.p1 = ~(buf.bit.d1 ^ buf.bit.d3 ^ buf.bit.d4 ^ buf.bit.d5);
    buf.bit.p0 = buf.bit.d0 ^ buf.bit.d1 ^ buf.bit.d2 ^ buf.bit.d4;

    return buf.byte;
}

/**
 * LIN_MakeChecksum
 *
 * @param[in]  protectId: protect Identifier
 * @param[in]  length: data length
 * @param[in]  data: data
 * @return : check sum data
 *
 * @brief  Make lin check sum
 */
uint8_t LIN_MakeChecksum(uint8_t protectId, uint8_t length, uint8_t *data)
{
    uint8_t i = 0, checksum = 0;
    uint16_t sum = protectId;

    for (i = 0; i < length; i++)
    {
        sum += data[i];
        if (sum >= 0x100)
        {
            sum -= 0xFF;
        }
    }
    checksum = ~(uint8_t)sum;

    return checksum;
}

/**
 * LIN_IfcTx
 *
 * @param[in] : none
 * @return : none
 *
 * @brief  Indicate the transmission of a complete frame.
 */
void LIN_IfcTx(void)
{
    if (g_direction == DIR_TX)
    {
        if (g_mode == MASTER_MODE)
        {
#if DATA_AND_CHECKSUM
            if (g_state == LIN_STATE_SEND_SYNC || g_state == LIN_STATE_SEND_DATA)
#else
            if (g_state == LIN_STATE_SEND_SYNC || g_state == LIN_STATE_SEND_DATA || g_state == LIN_STATE_SEND_CHECKSUM)
#endif
            {
                // Send sync, protect id, and data
                if (g_txCount < g_tranByte)
                {
                    UART_SendData(UART_LIN, g_buffer[g_txCount]);
                    g_txCount++;
                    if (g_txCount == g_tranByte)
                    {
                        UART_SetTXEInterrupt(UART_LIN, DISABLE);
                    }
                }
                else
                {
                    UART_SetTXEInterrupt(UART_LIN, DISABLE);
                }
            }
            else
            {
                UART_SetTXEInterrupt(UART_LIN, DISABLE);
            }
        }
        else
        {
#if DATA_AND_CHECKSUM
            if (g_state == LIN_STATE_SEND_DATA)
#else
            if (g_state == LIN_STATE_SEND_DATA || g_state == LIN_STATE_SEND_CHECKSUM)
#endif
            {
                // Send data
                if (g_txCount < g_tranByte)
                {
                    UART_SendData(UART_LIN, g_buffer[g_txCount]);
                    g_txCount++;
                    if (g_txCount == g_tranByte)
                    {
                        UART_SetTXEInterrupt(UART_LIN, DISABLE);
                    }
                }
                else
                {
                    UART_SetTXEInterrupt(UART_LIN, DISABLE);
                }
            }
            else
            {
                UART_SetTXEInterrupt(UART_LIN, DISABLE);
            }
        }
    }
}

/**
 * LIN_IfcRx
 *
 * @param[in] : none
 * @return : none
 *
 * @brief  Receives a header or data from the LIN bus.
 */
void LIN_IfcRx(void)
{
    uint8_t data = 0, id = 0, index = 0, checksum = 0, frameNum = 0;
    FRAME_ID_TBL  *frameTable = 0, *frameInfo = 0;

    data = UART_ReceiveData(UART_LIN);
    //fprintf("%02x ", data);
    if (g_direction == DIR_TX)
    {
        if (data != g_buffer[g_rxCount])
        {
            // Send error
            fprintf("bit err\r\n");
        }
        g_rxCount++;
        // transmit done
        if (g_rxCount == g_tranByte)
        {
            frameInfo = &g_scheduleTable[g_schTblIdx].schedule[g_frameIdx];
            if (g_state == LIN_STATE_SEND_SYNC)
            {
                // Start transmit or receive data
                if (frameInfo->dir == DIR_TX)
                {
                    g_state = LIN_STATE_SEND_DATA;
                    g_direction = DIR_TX;
#if DATA_AND_CHECKSUM
                    g_tranByte = frameInfo->dtl + 1;
                    memcpy((void *)g_buffer, (const void *)frameInfo->data, (unsigned int)g_tranByte - 1);  
                    checksum = LIN_MakeChecksum((frameInfo->cst == ENHANCED_CHECKSUM) ? g_protectId : 0,
                                                  frameInfo->dtl, frameInfo->data);
                    g_buffer[g_tranByte - 1] = checksum;
#else
                    g_tranByte = frameInfo->dtl;
                    memcpy(g_buffer, frameInfo->data, g_tranByte);
#endif
                    g_txCount = 0;
                    g_rxCount = 0;

                    UART_SetTXEInterrupt(UART_LIN, ENABLE);
                }
                else
                {
                    g_state = LIN_STATE_RECEIVE_DATA;
                    g_direction = DIR_RX;
                    g_recvByte = frameInfo->dtl + 1;
                    g_rxCount = 0;
                }
            }
#if DATA_AND_CHECKSUM
            else if (g_state == LIN_STATE_SEND_DATA)
            {
                // Transmit frame done
                g_state = LIN_STATE_IDLE;
            }
#else
            else if (g_state == LIN_STATE_SEND_DATA)
            {
                g_state = LIN_STATE_SEND_CHECKSUM;
                g_direction = DIR_TX;
                g_tranByte = 1;
                checksum = LIN_MakeChecksum((frameInfo->cst == ENHANCED_CHECKSUM) ? g_protectId : 0,
                                              frameInfo->dtl, frameInfo->data);
                g_buffer[0] = checksum;
                g_txCount = 0;
                g_rxCount = 0;

                UART_SetTXEInterrupt(UART_LIN, 1);
            }
            else if (g_state == LIN_STATE_SEND_CHECKSUM)
            {
                // Transmit frame done
                g_state = LIN_STATE_IDLE;
            }
            else
            {
            }
#endif
        }
    }
    else
    {
        if (g_mode == MASTER_MODE)
        {
            if (g_state == LIN_STATE_RECEIVE_DATA)
            {
                if (g_rxCount < g_recvByte)
                {
                    g_buffer[g_rxCount] = data;
                    g_rxCount++;
                }
                // Receive frame done
                if (g_rxCount == g_recvByte)
                {
                    frameInfo = &g_scheduleTable[g_schTblIdx].schedule[g_frameIdx];
                    checksum = LIN_MakeChecksum((frameInfo->cst == ENHANCED_CHECKSUM) ? g_protectId : 0,
                                                g_recvByte - 1, g_buffer);
                    if (checksum == g_buffer[g_rxCount - 1])
                    {
                        // Checksum is successful.
                        // Save data
                        memcpy((void *)frameInfo->data, (const void *)g_buffer, g_rxCount - 1);
                    }
                    else
                    {
                        // Checksum is failed.
                        fprintf("Cs err\r\n");
                    }
                    g_state = LIN_STATE_IDLE;
                }
            }
        }
        else
        {
            if (g_state == LIN_STATE_RECEIVE_SYNC)
            {
                // Check sync field
                if (data == 0x55)
                {
                    g_state = LIN_STATE_RECEIVE_IDENTIFIER;
                }
                else
                {
                    g_state = LIN_STATE_IDLE;
                    fprintf("Sync err\r\n");
                }
            }
            else if (g_state == LIN_STATE_RECEIVE_IDENTIFIER)
            {
                // Check protect id
                id = data & 0x3F;
                if (data == LIN_MakeProtId(id))
                {
                    frameTable = g_scheduleTable[g_schTblIdx].schedule;
                    frameNum = g_scheduleTable[g_schTblIdx].frameNum;
                    for (index = 0; index < frameNum; index++)
                    {
                        frameInfo = &frameTable[index];
                        // Find needed id
                        if (id == frameInfo->id)
                        {
                            g_frameIdx = index;
                            g_protectId = data;
                            //fprintf("%d\r\n", id);
                            // Start transmit or receive data
                            if (frameInfo->dir == DIR_TX)
                            {
                                g_state = LIN_STATE_SEND_DATA;
                                g_direction = DIR_TX;
#if DATA_AND_CHECKSUM
                                g_tranByte = frameInfo->dtl + 1;
                                memcpy((void *)g_buffer, (const void *)frameInfo->data, g_tranByte - 1);
                                checksum = LIN_MakeChecksum((frameInfo->cst == ENHANCED_CHECKSUM) ? g_protectId : 0,
                                                              frameInfo->dtl, frameInfo->data);
                                g_buffer[g_tranByte - 1] = checksum;
#else
                                g_tranByte = frameInfo->dtl;
                                memcpy(g_buffer, frameInfo->data, g_tranByte);
#endif
                                g_txCount = 0;
                                g_rxCount = 0;

                                UART_SetTXEInterrupt(UART_LIN, ENABLE);
                            }
                            else
                            {
                                g_state = LIN_STATE_RECEIVE_DATA;
                                g_direction = DIR_RX;
                                g_recvByte = frameInfo->dtl + 1;
                                g_rxCount = 0;
                            }

                            break;
                        }
                    }
                    // Not needed id
                    if (index == frameNum)
                    {
                        g_state = LIN_STATE_IDLE;
                        fprintf("Err Id:0x%x\r\n", data);
                    }
                }
                else
                {
                    g_state = LIN_STATE_IDLE;
                    fprintf("Pid:0x%x err\r\n", data);
                }
            }
            else if (g_state == LIN_STATE_RECEIVE_DATA)
            {
                if (g_rxCount < g_recvByte)
                {
                    g_buffer[g_rxCount] = data;
                    g_rxCount++;
                }
                // Receive frame done
                if (g_rxCount == g_recvByte)
                {
                    frameInfo = &g_scheduleTable[g_schTblIdx].schedule[g_frameIdx];
                    checksum = LIN_MakeChecksum((frameInfo->cst == ENHANCED_CHECKSUM) ? g_protectId : 0,
                                                g_recvByte - 1, g_buffer);
                    if (checksum == g_buffer[g_rxCount - 1])
                    {
                        // Checksum is successful.
                        // Save data
                        memcpy((void *)frameInfo->data, (const void *)g_buffer, g_rxCount - 1);
                    }
                    else
                    {
                        // Checksum is failed.
                        fprintf("Cs err\r\n");
                    }
                    g_state = LIN_STATE_IDLE;
                }
            }
            else
            {
            }
        }
    }
}

/**
 * LIN_IfcAux
 *
 * @param[in] : none
 * @return : none
 *
 * @brief  Receives a break field handle function
 */
void LIN_IfcAux(void)
{
    if (g_mode == SLAVE_MODE)
    {
        if (g_state == LIN_STATE_IDLE)
        {
            g_state = g_autoBaud ? LIN_STATE_RECEIVE_IDENTIFIER : LIN_STATE_RECEIVE_SYNC;
            g_direction = DIR_RX;
            g_txCount = 0;
            g_rxCount = 0;
        }
        else
        {
            fprintf("Frame err\r\n");
            g_state = g_autoBaud ? LIN_STATE_RECEIVE_IDENTIFIER : LIN_STATE_RECEIVE_SYNC;
            g_direction = DIR_RX;
            g_txCount = 0;
            g_rxCount = 0;
        }
    }
    else
    {
#if 1  // Send break, then send sync field after receive break field
        if (g_state == LIN_STATE_SEND_BREAK)
        {
            g_state = LIN_STATE_SEND_SYNC;
            UART_SetTXEInterrupt(UART_LIN, ENABLE);
        }
#endif
    }
}

/**
 * LIN_TxHeader
 *
 * @param[in] protectId: the protect idendifier of the header
 * @return : none
 *
 * @brief  Start to send a break field
 */
void LIN_TxHeader(uint8_t protectId)
{
    if (g_mode == MASTER_MODE)
    {
        UART_SendLINBreak(UART_LIN);
#if 1  // Send break, then send sync field after receive break field
        g_state = LIN_STATE_SEND_BREAK;
        g_tranByte = 2;
        g_buffer[0] = 0x55;
        g_buffer[1] = protectId;
        g_direction = DIR_TX;
        g_txCount = 0;
        g_rxCount = 0;
#else  // Send break, then send sync field immediately
        g_state = LIN_STATE_SEND_SYNC;
        g_tranByte = 2;
        g_buffer[0] = 0x55;
        g_buffer[1] = protectId;
        g_direction = DIR_TX;
        g_txCount = 0;
        g_rxCount = 0;

        UART_SetTXEInterrupt(UART_LIN, 1);
#endif
    }
}

/**
 * LIN_SchTick
 *
 * @param[in] : none
 * @return : entry index
 *
 * @brief  Sends the next message frame on the current schedule table, This function is called
 * only from the ISR for the scheduler timer.
 */
uint8_t LIN_SchTick(void)
{
    FRAME_ID_TBL *frameInfo = 0;

    // Decrease the delay time.
    if (g_delay >= LIN_TIME_BASE)
    {
        g_delay -= LIN_TIME_BASE;
    }
    // If the delay time delay has expired, send the next frame.
    if (g_delay == 0)
    {
        if(g_firstFlag)
        {
            g_firstFlag = 0;
        }
        else if (g_frameIdx < (g_scheduleTable[g_schTblIdx].frameNum - 1))    // Send the next frame.
        {
            g_frameIdx++;
        }
        else    // The current schedule is end.
        {
            g_frameIdx = 0;
        }
        frameInfo = &g_scheduleTable[g_schTblIdx].schedule[g_frameIdx];
        g_delay = frameInfo->delayTime;
        g_protectId = LIN_MakeProtId(frameInfo->id);
        LIN_TxHeader(g_protectId);
    }

    return g_frameIdx;
}

/**
 * LIN_ScheduleTimerHandle
 *
 * @param[in] : none
 * @return : none
 *
 * @brief  Timer handle function for master schedule table tick time
 */
void LIN_ScheduleTimerHandle(void *device, uint32_t wpara, uint32_t lpara)
{
    LIN_SchTick();
}

/**
 * LIN_InitScheduleTimer
 *
 * @param[in]  timeout: time
 * @return : none
 *
 * @brief  Init timer for master mode
 */
void LIN_InitScheduleTimer(uint32_t timeout)
{
    TIMER_ConfigType config;
    memset((void *)&config, 0, sizeof(TIMER_ConfigType));

    config.periodValue = (APB_BUS_FREQ / 1000) * timeout;
    config.timerEn = ENABLE;
    config.interruptEn = ENABLE;
    config.callBack = LIN_ScheduleTimerHandle;

    TIMER_Init(LIN_TIMER_CHANNEL, &config);
}

/**
 * LIN_RunSchTbl
 *
 * @param[in] : none
 * @return : none
 *
 * @brief  Run schedule table for master mode
 */
void LIN_RunSchTbl(void)
{
    uint32_t index = 0, time = 0;
    FRAME_ID_TBL *frameTable = g_scheduleTable[g_schTblIdx].schedule;

    for (time = 0; time < g_sendTimes; time++)
    {
        fprintf("test times:%d\r\n", time);
        for (index = 0; index < g_scheduleTable[g_schTblIdx].frameNum; index++)
        {
            g_frameIdx = index;
            g_protectId = LIN_MakeProtId(frameTable[index].id);
            LIN_TxHeader(g_protectId);
            //while (g_state != LIN_STATE_IDLE);    // Wait transmit done
            // Frame delay time
            mdelay(frameTable[index].delayTime);
        }
        //while (1);    // Send once
    }
}

/**
 * LIN_InitLin
 *
 * @param[in] mode: master or slave mode
 * @param[in] schTblIdx: schedule table index
 * @param[in] baudrate: baud rate
 * @return : none
 *
 * @brief  Init uart LIN module
 */
void LIN_InitLin(uint8_t mode, uint8_t schTblIdx, uint16_t baudrate)
{
    UART_ConfigType uartConfig;
    uint8_t linCtrl = 0;
    memset((void *)&uartConfig, 0, sizeof(UART_ConfigType));
    
    fprintf("mode:%d schtable:%d baudrate:%d\r\n", mode, schTblIdx, baudrate);
    ///Init GPIO
    GPIO_SetFunc(SWLIN_RX, RXPinFunc);
    GPIO_SetFunc(SWLIN_TX, TXPinFunc); 
    GPIO_SetDir(SWLIN_SLP, GPIO_OUT);
    GPIO_SetPinLevel(SWLIN_SLP, GPIO_LEVEL_HIGH); 
    
    uartConfig.baudrate = baudrate;
    uartConfig.dataBits = UART_WORD_LEN_8BIT;
    uartConfig.stopBits = UART_STOP_1BIT;
    uartConfig.parity = UART_PARI_NO;
    uartConfig.dmaEn = UART_DMA_TXRX_NONE; 
    uartConfig.fifoByteEn = ENABLE;
    uartConfig.sampleCnt = UART_SMP_CNT0;   ///<must set UART_SMP_CNT0 when use LIN function
    uartConfig.callBack = UartLIN_Handler;
    
    UART_Init(UART_LIN, &uartConfig);   ///<Init UART
    
    // Set master or slave mode
    g_mode = mode;
    g_state = LIN_STATE_IDLE;
    g_schTblIdx = schTblIdx;
    
    
    // Init uart LIN
    if (g_mode == MASTER_MODE)
    {
        linCtrl = UART_LINCR_LINEN_Msk | UART_LINCR_LBRKIE_Msk | UART_LINCR_LBRKDL_Msk;
    }
    else
    {
        linCtrl = UART_LINCR_LINEN_Msk | UART_LINCR_LBRKIE_Msk | (g_breakThre ? UART_LINCR_LBRKDL_Msk : 0) | (g_autoBaud ? UART_LINCR_LABAUDEN_Msk : 0);
    }
    UART_SetLIN(UART_LIN, linCtrl); 
       
    // Enable interrupt
    UART_SetRXNEInterrupt(UART_LIN, ENABLE);
    UART_SetTXEInterrupt(UART_LIN, DISABLE);    ///< Default DISABLE
    NVIC_ClearPendingIRQ(UART_LIN_IRQn);
    NVIC_EnableIRQ(UART_LIN_IRQn);

    // Start to run schedule table in master mode
    if (g_mode == MASTER_MODE)
    {
        //LIN_RunSchTbl();  // Not use timer
        LIN_InitScheduleTimer(LIN_TIME_BASE);
    }
}

/**
 * Uart_SendData
 *
 * @param[in] length: data length
 * @param[in] data: data buffer
 * @return : none
 *
 * @brief  Send data using uart
 */
void Uart_SendData(uint8_t length, uint8_t *data)
{
    int index = 0;

    for (index = 0; index < length; index++)
    {
        do
        {
        }
        while (!UART_TxIsEmpty(UART_LIN));

        UART_SendData(UART_LIN, data[index]);
    }
}

/**
 * UartLIN_Handler
 *
 * @param[in] : none
 * @return : none
 *
 * @brief  Uart lin interrupt handle function
 */
void UartLIN_Handler(void *device, uint32_t wpara, uint32_t lpara)
{
    // Receive break field
    if (0 != (lpara & UART_LSR1_FBRK_Msk))
    {
        LIN_IfcAux();
        UART_LIN->LSR1 |= UART_LSR1_FBRK_Msk;   ///<write 1 to clear break status
    }

    // Receive data
    if (0 != (wpara & UART_LSR0_DR_Msk))
    {
        LIN_IfcRx();
    }

    // Transmit data done
    if (0 != (wpara & UART_LSR0_THRE_Msk))
    {
        LIN_IfcTx();
    }
}

