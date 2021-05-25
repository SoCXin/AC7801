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
#include "ac780x_uart.h"
#include "ac780x_gpio.h"

/*
* 本例程通过使用DMA以及UART空闲中断，实现UART DMA收发，并在UART空闲中断中处理接收的数据
*/

#define UARTx   UART2
#define UARTx_IRQn  UART2_IRQn

#define BaudRate    115200

#define UART_TRANSMIT_DATA_POOL_COUNT 64
#define UART_RECV_DATA_POOL_COUNT     64
///这里DMA mem设置为32bit模式，所以DMA传输的buffer必须4字节对齐
uint32_t dmaRxBuf[UART_RECV_DATA_POOL_COUNT>>2];
uint32_t dmaTxBuf[UART_TRANSMIT_DATA_POOL_COUNT>>2];

uint8_t *rxBuf = (uint8_t *)dmaRxBuf;
uint8_t *txBuf = (uint8_t *)dmaTxBuf;


/**
* UARTTX_DMA_EventCallback
*
* @param[in] device: DMA_Type pointer
* @param[in] wpara: DMA Channel Status
* @param[in] lpara: reserve for user's code
* @return    none
*
* @brief   uart transmit event callback
*/
void UartTxDMAEventCallback(void *device, uint32_t wpara, uint32_t lpara)
{
    if (wpara & DMA_CHANNEL_STATUS_FINISH_Msk)    //!<DMA finish
    {
        
    }
    else if (wpara & DMA_CHANNEL_STATUS_HALF_FINISH_Msk)  //!<DMA half finish
    {
        
    }
    else if (wpara & DMA_CHANNEL_STATUS_TRANS_ERROR_Msk)  //!<DMA error
    {
        
    }
    else
    {
        //!<never be here
    }
}


/**
* UARTRX_DMA_EventCallback
*
* @param[in] device: DMA_Type pointer
* @param[in] wpara: DMA Channel Status
* @param[in] lpara: reserve for user's code
* @return    none
*
* @brief   uart dma receive handle
*/
static void UartRxDMAEventCallback(void *device, uint32_t wpara, uint32_t lpara)
{
    /* Check if DMA Channel Receive Complete */
    if (wpara & DMA_CHANNEL_STATUS_FINISH_Msk)    //!<DMA finish
    {
        
    }
    else if (wpara & DMA_CHANNEL_STATUS_HALF_FINISH_Msk)  //!<DMA half finish
    {
        
    }
    else if (wpara & DMA_CHANNEL_STATUS_TRANS_ERROR_Msk)  //!<DMA error
    {
        
    }
    else
    {
        //!<never be here
    }
}
/**
* UARTRX_DMA_EventCallback
*
* @param[in] device: UART_Type pointer
* @param[in] wpara: UART lsr0 register
* @param[in] lpara: UART lsr1 register
* @return    none
*
* @brief   uart dma receive handle
*/
static void UartRxIdleCallBack(void *device, uint32_t wpara, uint32_t lpara)
{
    /* Check if IDLE interrupt happened */
    if (lpara & UART_LSR1_IDLE_Msk)
    {
        uint16_t recLen;
        uint16_t index;
        ///接收不足4字节会在DMA fifo中缓存
        recLen = DMA0_CHANNEL1->DATA_TRANS_NUM + DMA0_CHANNEL1->FIFO_LEFT_NUM;
        ///将DMA fifo数据flush出来，因为mem设置为32bit，所以fifo也是4字节对齐
        ///(即使fifo里只有1个字节也会刷出4字节，需要保证缓存冗余不会溢出)
        DMA_ChannelFlush(DMA0_CHANNEL1);
        for (index=0;index<recLen;index++)
        {
            txBuf[index] = rxBuf[index];
        }
        UART_TransmitDMA(UARTx, DMA0_CHANNEL0, txBuf, recLen, UartTxDMAEventCallback); 

        UART_ReceiveDMA(UARTx, DMA0_CHANNEL1, rxBuf, UART_RECV_DATA_POOL_COUNT, UartRxDMAEventCallback); 
    }
}
/**
* Basic_uart
*
* @param[in] none
* @return    none
*
* @brief  Basic_uart function
*/
void Basic_uart(void)
{
    uint8_t i = 0;
    UART_ConfigType  uartConfig;
    memset((void *)&uartConfig, 0, sizeof(UART_ConfigType));

    //set pin mux
    GPIO_SetFunc(GPIOB, GPIO_PIN9, GPIO_FUN3);
    GPIO_SetFunc(GPIOB, GPIO_PIN10, GPIO_FUN3);
    
    uartConfig.baudrate = BaudRate;
    uartConfig.dataBits = UART_WORD_LEN_8BIT;
    uartConfig.stopBits = UART_STOP_1BIT;
    uartConfig.parity = UART_PARI_NO;
    uartConfig.fifoByteEn = ENABLE; ///<must enable fifoByte when use DMA
    uartConfig.dmaEn = UART_DMA_TXRX_EN;
    uartConfig.callBack = UartRxIdleCallBack;   ///<uart2 interrupt callback
    UART_Init(UARTx, &uartConfig);
    UARTx->IDLE |= UART_IDLE_IDLEIE_Msk | UART_IDLE_ILEN_Msk;    ///<enable uart idle interrupt
    
    ///Enable UARTx interrupt
    NVIC_SetPriority(UARTx_IRQn, 3);
    NVIC_ClearPendingIRQ(UARTx_IRQn);
    NVIC_EnableIRQ(UARTx_IRQn);
    /* Initializing data in the txBuf */
    for (i = 0; i < 10; i++)
    {
        txBuf[i] = i + 1;
    }
    UART_TransmitDMA(UARTx, DMA0_CHANNEL0, txBuf, 10, UartTxDMAEventCallback); 
    
    UART_ReceiveDMA(UARTx, DMA0_CHANNEL1, rxBuf, UART_RECV_DATA_POOL_COUNT, UartRxDMAEventCallback); 


    while(1);
}
