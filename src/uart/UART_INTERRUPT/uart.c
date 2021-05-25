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
 
#include "ac780x_uart_reg.h"
#include "ac780x_gpio.h"

#define UARTx   UART2
#define UARTx_IRQn  UART2_IRQn

#define UART_RX_BUFF_LENGTH     16

uint8_t g_uartRxDataBuff[UART_RX_BUFF_LENGTH];
uint16_t g_uartRxDataIndex;

uint8_t *g_uartTxDataPoint;
uint16_t g_uartTxDataLength;
uint16_t g_uartTxDataIndex;

int TransmitData(uint8_t *pdata, uint16_t length);
/**
* UartEventCallback
*
* @param[in] device: UART_Type pointer
* @param[in] wpara: UART lsr0 register
* @param[in] lpara: UART lsr1 register
* @return    none
*
* @brief   uart receive handle
*/
static void UartEventCallback(void *device, uint32_t wpara, uint32_t lpara)
{
    uint8_t data = 0;
    UART_Type *uart_Device = (UART_Type *)device;
    /*rx interrupt*/
    if ((uart_Device->IER & UART_IER_ERXNE_Msk) && (wpara & UART_LSR0_DR_Msk))
    {
        data = uart_Device->RBR;  
        g_uartRxDataBuff[g_uartRxDataIndex++] = data;
        
        if (g_uartRxDataIndex >= UART_RX_BUFF_LENGTH)
        {
            g_uartRxDataIndex = 0;
            TransmitData(g_uartRxDataBuff, UART_RX_BUFF_LENGTH);
        }
    }
    /*tx interrupt*/
    if ((uart_Device->IER & UART_IER_ETXE_Msk) && (wpara & UART_LSR0_THRE_Msk)) 
    {
        uart_Device->RBR = g_uartTxDataPoint[g_uartTxDataIndex++];

        if (g_uartTxDataIndex >= g_uartTxDataLength)
        {
            UART_SetTXEInterrupt(device, DISABLE);  ///<发送最后一个字节时关闭发送空中断
        }
    }
}
/**
* uart_Initialize
* 
* @param[in] none
* @return none
*
* @brief uart 初始化
*/
void uart_Initialize(void)
{
    UART_ConfigType  uartConfig = {0};
    
    //set pin mux
    GPIO_SetFunc(GPIOB, GPIO_PIN9, GPIO_FUN3);
    GPIO_SetFunc(GPIOB, GPIO_PIN10, GPIO_FUN3);
    
    
    uartConfig.baudrate = 115200;
    uartConfig.dataBits = UART_WORD_LEN_8BIT;
    uartConfig.stopBits = UART_STOP_1BIT;
    uartConfig.parity = UART_PARI_NO;
    uartConfig.fifoByteEn = ENABLE; ///<must enable fifoByte when use DMA
    uartConfig.dmaEn = UART_DMA_TXRX_EN;
    uartConfig.callBack = UartEventCallback;   ///<uart2 interrupt callback
    UART_Init(UARTx, &uartConfig);
    
    UART_SetRXNEInterrupt(UARTx, ENABLE);
    ///Enable UARTx interrupt
    NVIC_SetPriority(UARTx_IRQn, 3);
    NVIC_ClearPendingIRQ(UARTx_IRQn);
    NVIC_EnableIRQ(UARTx_IRQn);
    
}
/**
* TransmitData
* 
* @param[in] pdata :发送数据指针
* @param[in] length :发送数据长度
* @return none
*
* @brief uart 发送函数，通过中断发送 
*/
int TransmitData(uint8_t *pdata, uint16_t length) 
{
    if (g_uartTxDataIndex < g_uartTxDataLength)  
    {
        return -1;      ///<有数据正在发送
    }
    g_uartTxDataPoint = pdata;
    g_uartTxDataLength = length;
    g_uartTxDataIndex = 0;
    UART_SetTXEInterrupt(UARTx, ENABLE);
    return 0;
}

uint8_t printData[] = "This is a Uart interrupt test\r\n";
int main(void)
{
    uart_Initialize();
    TransmitData(printData, sizeof(printData));
    while(1)
    {
        ;
    }
}

