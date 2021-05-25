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
/**********<Incldue>**********/
#include <string.h>
#include "ac780x_adc.h"
#include "ac780x_adc_reg.h"
#include "ac780x_gpio.h"
#include "ac780x.h"
#include "ac780x_debugout.h"
#include "ac780x_dma.h"
#include "ac780x_ctu.h"
#include "ac780x_timer.h"
#include "ac780x_pwm.h"

#define Delay5us                      (APB_BUS_FREQ/200000-1)
#define Delay5ms                      (APB_BUS_FREQ/200-1)
#define Delay1s                       (APB_BUS_FREQ-1)
#define DMA_TRANSFER_NUM               8

/*********<Variable>********/
uint32_t g_ADCValueBuffer[DMA_TRANSFER_NUM + 1] = {0};
uint32_t g_timerCnt = 0;
uint32_t g_averageSampleValue = 0;
/**
 * ADC_DMACallback
 *
 * @param[in]  wpara:dmaChannelStatus 
 *             lpara:0
 * @return 0
 *
 * @brief ADC DMAcallback function
 */

void ADC_DMACallback(void *device, uint32_t wpara, uint32_t lpara)
{
    if ((wpara & 0x01) == 0x1)  
    {
        ///<dma finish
    }
    if ((wpara & 0x02) == 0x2)
    {
        ///<dma half finish
    }
    if ((wpara & 0x04) == 0x4)
    {
        ///<dma error
    }
}
/**
 * ADC_DMAInit
 *
 * @param[in] void
 * @return void
 *
 * @brief ADC DMA config
 */
void ADC_DMAInit(void)
{
    uint32_t tmpMemStartAddr = (uint32_t)&g_ADCValueBuffer[0];
    uint32_t tmpMemEndAddr = (uint32_t)&g_ADCValueBuffer[DMA_TRANSFER_NUM + 1]; ///<Setting memory DMA address
    DMA_ConfigType tmpDMAConfig;
    memset(&tmpDMAConfig, 0x00, sizeof(DMA_ConfigType));
    
    tmpDMAConfig.memStartAddr = tmpMemStartAddr; //DMA transfer start addr
    tmpDMAConfig.memEndAddr = tmpMemEndAddr;//DMA transfer end addr
    tmpDMAConfig.periphStartAddr = (uint32_t)(&(ADC0->RDR)); ///<Move ADC DR to memory
    tmpDMAConfig.channelEn = ENABLE;                     ///<channel enable
    tmpDMAConfig.finishInterruptEn = ENABLE;                  
    tmpDMAConfig.halfFinishInterruptEn = DISABLE;               
    tmpDMAConfig.errorInterruptEn = ENABLE;                   
    tmpDMAConfig.channelPriority = DMA_PRIORITY_VERY_HIGH;///<DMA priority
    tmpDMAConfig.circular = DISABLE;                      ///<enable circular mode
    tmpDMAConfig.direction = DMA_READ_FROM_PERIPH;          ///<dir: perorh to memory
    tmpDMAConfig.MEM2MEM = DISABLE;                     ///<
    tmpDMAConfig.memByteMode = DMA_MEM_BYTE_MODE_2TIME;  ///<
    tmpDMAConfig.memIncrement = ENABLE;                  ///<
    tmpDMAConfig.periphIncrement = DISABLE;               ///<
    tmpDMAConfig.memSize = DMA_MEM_SIZE_32BIT;           ///<0:8-bit,1:16-bit,2:32-bit
    tmpDMAConfig.periphSize = DMA_PERIPH_SIZE_16BIT;     ///<0:8-bit,1:16-bit,2:32-bit
    tmpDMAConfig.transferNum = DMA_TRANSFER_NUM;         
    tmpDMAConfig.periphSelect = DMA_PEPIRH_ADC0;  //select ADC0
    tmpDMAConfig.callBack = ADC_DMACallback; 

    DMA_Init(DMA0_CHANNEL0, &tmpDMAConfig);                     
    NVIC_EnableIRQ(DMA0_CHANNEL0_IRQn);                             
}
/**
 * CTU_Config
 *
 * @param[in] void
 * @return void
 *
 * @brief CTU config,set timer0 trig ADC regular group
 */
void CTU_Config(void)
{
    CTU_ConfigType ctuConfig;
    memset(&ctuConfig, 0x00, sizeof(ctuConfig));
    ctuConfig.uart0RxFilterEn = DISABLE; 
    ctuConfig.rtcCaptureEn = DISABLE;  
    ctuConfig.acmpCaptureEn = DISABLE;  
    ctuConfig.uart0RxCaptureEn = DISABLE;  
    ctuConfig.uartTxModulateEn = DISABLE; 
    ctuConfig.clkPsc = CTU_CLK_PRESCALER_1;  
    ctuConfig.adcRegularTriggerSource = CTU_TRIGGER_ADC_TIMER_CH0_OVERFLOW; 
    ctuConfig.delay0Time = 0;  
    CTU_Init(&ctuConfig);
}
/**
 * TIMER_Callback
 *
 * @param[in] timerIndex
 * @return void
 *
 * @brief 
 */
void TIMER_Callback(void *device, uint32_t wpara, uint32_t lpara)
{
    if (TIMER_CHANNEL0 == device)
    {
        g_timerCnt++;
    }
}
/**
 * TIMER0_Init
 *
 * @param[in] void
 * @return void
 *
 * @brief TIMER0
 */
void TIMER0_Init(void)
{
    TIMER_ConfigType timerConfig = {0};
    timerConfig.periodValue = Delay5ms;//5ms
    timerConfig.interruptEn = ENABLE;
    timerConfig.linkModeEn = DISABLE;
    timerConfig.callBack = TIMER_Callback;
    timerConfig.timerEn = ENABLE;
    TIMER_Init(TIMER_CHANNEL0, &timerConfig);
}
/**
 * ADC_init
 *
 * @param[in] void
 * @return void
 *
 * @brief ADC config, set mode3
 */
void ADC_init()
{
    ADC_ConfigType tempAdcConfig;
    ADC_ConfigType* adcConfig;
    adcConfig = &tempAdcConfig;

    GPIO_SetFunc(GPIOA, GPIO_PIN2, GPIO_FUN2);///<ADC_IN8 Analog function enable
    adcConfig->clkPsc = ADC_CLK_PRESCALER_1; ///<Set ADC Clk = 24M/2/(0+1)
    adcConfig->scanModeEn = ENABLE;                  
    adcConfig->continousModeEn = DISABLE;             
    adcConfig->regularDiscontinousModeEn = DISABLE; 
    adcConfig->injectDiscontinousModeEn = DISABLE;    
    adcConfig->injectAutoModeEn = DISABLE;          
    adcConfig->intervalModeEn = DISABLE;              
    adcConfig->regularDiscontinousNum = 0;            
    adcConfig->EOCInterruptEn = DISABLE;              
    adcConfig->IEOCInterruptEn = DISABLE;             
    adcConfig->interruptEn = DISABLE;                 
    adcConfig->regularDMAEn = ENABLE;                 
    adcConfig->regularTriggerMode = ADC_TRIGGER_EXTERNAL;   ///<choose external trigger
    adcConfig->injectTriggerMode = ADC_TRIGGER_INTERNAL; 
    adcConfig->regularSequenceLength = 1;               
    adcConfig->injectSequenceLength = 0;              
    adcConfig->dataAlign = ADC_DATA_ALIGN_RIGHT;       
    adcConfig->powerMode = ADC_POWER_ON;               
    ADC_Init(ADC0, adcConfig);                      ///<ADC works Mode Config
    
    ADC_SetRegularGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 0);  //(7+12)/24000000 + 5/24000000 = 1us
    ADC_DMAInit();   //ADC DMA
}

int main(void)
{
    uint8_t i;
    InitDebug();
    InitDelay();
    CTU_Config();
    ADC_init();
    TIMER0_Init();
    
    while (1)
    {
        mdelay(500);
        printf("value: ");
        for (i=0;i<DMA_TRANSFER_NUM;i++)
        {
            printf("%08x ", g_ADCValueBuffer[i]);
        }
        printf("\r\n");
    }
    
}
