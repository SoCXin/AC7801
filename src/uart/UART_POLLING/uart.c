/* Copyright Statement:
*
* This software/firmware and related documentation ("Autochips Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to Autochips Inc. and/or its licensors.
* Without the prior written permission of Autochips inc. and/or its licensors,
* any reproduction, modification, use or disclosure of Autochips Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*
* Copyright(C) 2020, Autochips ( All rights reserved. )
*
* BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("AUTOCHIPS SOFTWARE")
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

#include "ac780x_gpio.h"
#include "ac780x_uart.h"
#include "ac780x_uart_reg.h" 

#define UARTx   UART2

/**
* uart_Initialize
* 
* @param[in] none
* @return none
*
* @brief uart 
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
    uartConfig.fifoByteEn = DISABLE; ///<must enable fifoByte when use DMA
    uartConfig.dmaEn = UART_DMA_TXRX_NONE;
    UART_Init(UARTx, &uartConfig);    
}

int main(void)
{
    uint8_t data;
    uart_Initialize();
    
    while (1)
    {
        while(!UART_RxIsDataReady(UARTx));    /* when rx data is ready */
        UARTx->LSR0 |= UART_LSR0_DR_Msk;      /* Clear data ready flag */
        data = UART_ReceiveData(UARTx);
        while (!UART_TxIsEmpty(UARTx));
        UART_SendData(UARTx, data);
    }
}

