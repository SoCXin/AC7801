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
 
#include "ac780x_adc.h"
#include "ac780x_gpio.h"
#include "ac780x_ctu.h"
#include "ac780x_timer.h"
#include "ac780x_timer_reg.h"
#include "ac780x_dma.h"
#include "ac780x_debugout.h"
#define SAMPLE_CYCLE    (100000)   ///< us
#define MAX_SAMPLE_CHANNEL  (3)

uint32_t s_sampleValue[MAX_SAMPLE_CHANNEL] = {0};


ADC_ConfigType tempAdcConfig;
CTU_ConfigType tempCtuConfig;
DMA_ConfigType tmpDMAConfig;
/**
 * Print_AdcVal
 *
 * @param[in]  none
 * @return 0
 *
 * @brief printf adc sample value
 */
void Print_AdcVal(void)
{
    uint8_t index;
    printf("\r\nsample data is: ");
    for (index = 0; index<MAX_SAMPLE_CHANNEL; index++)
    {
        printf(" 0x%08x ",s_sampleValue[index]);
    }
    printf("\r\n");
}
/**
 * ADC_DMACallback
 *
 * @param[in]  wparam:dmaChannelStatus 
 *             lparam:0
 * @return 0
 *
 * @brief ADC DMA interrupt callback function
 */
void ADC_DMACallback(void *device, uint32_t wpara, uint32_t lpara)
{
    /*
     wparam is CHANNELx_STATUS register 
     CHANNELx_STATUS[2] transmit error
     CHANNELx_STATUS[1] HalfFinish
     CHANNELx_STATUS[0] Finish
    */
    if ((wpara & 0x01) == 0x1)
    {
        
    }
    if ((wpara & 0x02) == 0x2)
    {
    }
    if ((wpara & 0x04) == 0x4)
    {
    }
}
/**
 * TIMER0_Callback
 *
 * @param[in] timerIndex
 * @return void
 *
 * @brief TIMER0中断回调函数
 */
void TIMER0_Callback(void *device, uint32_t wpara, uint32_t lpara)
{
    
}
/**
*
* @param[in] none
*
* @return none
*
* @brief TIMER init
*   use timer0 overflow to trigger adc sample
*/
void Timer_Init(void)
{
    TIMER_ConfigType  timerConfig0 = {0};

    /* configure TIMER channel 0, only enable timer */
    timerConfig0.periodValue      = 24 * SAMPLE_CYCLE - 1;  ///
    timerConfig0.interruptEn      = DISABLE;
    timerConfig0.linkModeEn        = DISABLE;
    timerConfig0.timerEn          = DISABLE;    ///<do not enable now
    timerConfig0.callBack = TIMER0_Callback;
    
    TIMER_Init(TIMER_CHANNEL0, &timerConfig0);
}
/**
*
* @param[in] none
*
* @return none
*
* @brief GPIO init
*   Init adc port
*/
void GPIOInit(void)
{
    GPIO_SetFunc(GPIOA, GPIO_PIN2, GPIO_FUN2); ///ADC_IN8 Analog function enable  
}
/**
*
* @param[in] none
*
* @return none
*
* @brief ADC sample
*/
void ADC_Sample(void)
{
    ADC_ConfigType* adcConfig;
    CTU_ConfigType *ctuConfig;
    
    InitDebug(); /// Initialize debug UART 2
    InitDelay();
    printf("\r\n This is adc_sample1 demo\r\n");
    
    GPIOInit();
    
    adcConfig = &tempAdcConfig;
    ctuConfig = &tempCtuConfig;
    
    adcConfig->scanModeEn = ENABLE;
    adcConfig->continousModeEn = DISABLE;
    adcConfig->regularDiscontinousModeEn = DISABLE;
    adcConfig->injectDiscontinousModeEn = DISABLE;
    adcConfig->injectAutoModeEn = DISABLE;
    adcConfig->regularDMAEn = ENABLE;   ///<Enable adc regular group DMA mode
    adcConfig->regularTriggerMode = ADC_TRIGGER_EXTERNAL;   ///<Use CTU trig adc ,we need chose external trigger
    adcConfig->regularSequenceLength = 3;
    adcConfig->powerMode = ADC_POWER_ON;    ///<adc power on
    adcConfig->clkPsc = ADC_CLK_PRESCALER_16;
    ADC_Init(ADC0, adcConfig);  ///ADC mode 5    
    
    
    ///<采样序列从0~11按顺序采样
    ADC_SetRegularGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_5, 0); ///set ADC_CH_8 为第一个采样序列
    ADC_SetRegularGroupChannel(ADC0, ADC_CH_BANDGAP, ADC_SPT_CLK_5, 1); ///set ADC_CH_BANDGAP 为第二个采样序列
    ADC_SetRegularGroupChannel(ADC0, ADC_CH_TSENSOR, ADC_SPT_CLK_5, 2); ///set ADC_CH_TSENSOR 为第三个采样序列

    ctuConfig->adcRegularTriggerSource = CTU_TRIGGER_ADC_TIMER_CH0_OVERFLOW; ///user timer0 overflow as ADC trigger
    CTU_Init(ctuConfig);
    
    Timer_Init();
    
    tmpDMAConfig.setting.channelEn = ENABLE;                
    tmpDMAConfig.setting.finishInterruptEn = ENABLE;           
    tmpDMAConfig.setting.halfFinishInterruptEn = DISABLE;        
    tmpDMAConfig.setting.errorInterruptEn = ENABLE;              
    tmpDMAConfig.setting.MEM2MEM = DISABLE;    
    tmpDMAConfig.setting.channelPriority = DMA_PRIORITY_VERY_HIGH;  
    tmpDMAConfig.setting.memSize = DMA_MEM_SIZE_32BIT;   ///<only support 32bit mode in ac7801x
    tmpDMAConfig.setting.periphSize = DMA_PERIPH_SIZE_16BIT;  
    tmpDMAConfig.setting.memIncrement = ENABLE;          
    tmpDMAConfig.setting.periphIncrement = DISABLE;         
    tmpDMAConfig.setting.circular = ENABLE;                  
    tmpDMAConfig.setting.direction = DMA_READ_FROM_PERIPH;                        
    tmpDMAConfig.setting.memByteMode = DMA_MEM_BYTE_MODE_1TIME;   ///<32bit transfer 
    tmpDMAConfig.setting.transferNum = MAX_SAMPLE_CHANNEL;                   ///<DMA通道传输长度
    tmpDMAConfig.periphSelect = DMA_PEPIRH_ADC0;
    tmpDMAConfig.memStartAddr = (uint32_t)s_sampleValue; //set start addr
    tmpDMAConfig.memEndAddr = (uint32_t)s_sampleValue + sizeof(s_sampleValue);//set end addr
    tmpDMAConfig.periphStartAddr = (uint32_t)(&(ADC0->RDR)); ///<Move ADC DR to memory
    tmpDMAConfig.callBack = ADC_DMACallback;

    DMA_Init(DMA0_CHANNEL0, &tmpDMAConfig);                    ///<ADC 使用DMA0通道
 ///<不开启DMA中断
   // NVIC_EnableIRQ(DMA0_CHANNEL0_IRQn);                              ///<使能DMA0中断请求              
     
    TIMER_SetChannel(TIMER_CHANNEL0, ENABLE);
}
int main(void)
{
    ADC_Sample();
    while (1)
    {
        Print_AdcVal();
        mdelay(500);
    }
}
