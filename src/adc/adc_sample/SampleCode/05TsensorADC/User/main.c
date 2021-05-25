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
 /****************************************************************************

 I.AC780X ADC模块例程（本例程基于CMSIS 1.0.x版本接口开发调试）
	1.通过软件触发规则组采样，获取Tsensor传感器AD值，并计算出芯片温度。
    
 *****************************************************************************/
 /**********<Incldue>**********/
#include "ac780x.h"
#include "ac780x_debugout.h"
#include "ac780x_adc_reg.h"
#include "ac780x_gpio.h"

/**
 * ADC_init
 *
 * @param[in] void
 * @return void
 *
 * @brief 初始化ADC,配置ADC参数。
 */
void ADC_init()
{
    ADC_ConfigType tempAdcConfig;
    ADC_ConfigType* adcConfig;
    adcConfig = &tempAdcConfig;
    adcConfig->clkPsc = ADC_CLK_PRESCALER_1; ///<Set ADC Clk = 24M/2/(0+1)
    adcConfig->scanModeEn = ENABLE;                   //扫描模式
    adcConfig->continousModeEn = DISABLE;             //连续模式
    adcConfig->regularDiscontinousModeEn = DISABLE;   //1：打开规则组间断转换模式
    adcConfig->injectDiscontinousModeEn = DISABLE;    //1：打开注入组间断转换模式
    adcConfig->injectAutoModeEn = DISABLE;            //1：自动注入模式
    adcConfig->intervalModeEn = DISABLE;              //1：注入组为间隔转换模式
    adcConfig->regularDiscontinousNum = 0;            //
    adcConfig->EOCInterruptEn = DISABLE;              //EOC中断使能
    adcConfig->IEOCInterruptEn = DISABLE;             //IEOC中断使能
    adcConfig->interruptEn = DISABLE;                 //中断使能
    adcConfig->regularDMAEn = DISABLE;                 //ADC DMA
    adcConfig->regularTriggerMode = ADC_TRIGGER_INTERNAL;//ADC触发源，内部触发
    adcConfig->injectTriggerMode = ADC_TRIGGER_INTERNAL; //ADC触发源，内部触发
    adcConfig->regularSequenceLength = 1;               //规则组长度设为1
    adcConfig->injectSequenceLength = 0;               //注入组长度设为0
    adcConfig->dataAlign = ADC_DATA_ALIGN_RIGHT;       //右对齐
    adcConfig->callBack =  NULL;               //回调
    adcConfig->powerMode = ADC_POWER_ON;               //上电
    ADC_Init(ADC0, adcConfig);                      ///<ADC works Mode Config
    /*
        ADC转换率计算公式：
        转换时间= 采样时间+转换时间+同步时间
        转换时间= (SPT+12)/ADC模块时钟频率+5/APB时钟频率
        备注：
        1.同步时间为5个APB CLK。
        2.ADC时钟频率 = APB时钟频率 /(分频系数+1)
	*/
    //规则组通道设置
   // ADC_SetRegularGroupChannel(ADC0, ADC_CH_TSENSOR, ADC_SPT_CLK_7, 0);  //采样&转换时间= (7+12)/24000000 + 5/24000000 = 1us

}
/**
 * main
 *
 * @param[in]  none
 * @return 0: success, other: error value
 *
 * @brief  
 */
int main(void)
{
        /* Cali para */

    uint32_t TsensorCali = 0;
    uint8_t VbandgapRT = 0;
    uint8_t VtsensorRT = 0;



    float temperatureVal = 0.0;
    float bgVol = 0.0;
    float vt25Vol = 0.0;
    uint32_t TsensorVol;
    uint32_t TsensorVal;
    uint32_t bandGapAVG;  
    InitDelay();  
    InitDebug(); 
    ADC_init();
    printf("\r\nAC7801x: ADC_Sample\r\n");  




    /* Get Tsensor calibration  value*/

    TsensorCali = *((uint32_t*)0x40002058);
    VbandgapRT = (uint8_t)(TsensorCali & 0x1F);
    VtsensorRT = (uint8_t)((TsensorCali >> 8) & 0xFF);

    /* BG calibration */
    if (VbandgapRT >> 4)
    {
        bgVol = 1200 - (VbandgapRT & 0xF);
    }
    else
    {
        bgVol = 1200 + (VbandgapRT & 0xF);
    }

    /* Vtsensor calibration */
    if (VtsensorRT >> 7)
    {
        vt25Vol = 720 - (VtsensorRT & 0x7F) * 0.5;
    }
    else
    {
        vt25Vol = 720 + (VtsensorRT & 0x7F) * 0.5;
    }

    
    ADC_SetRegularGroupChannel(ADC0, ADC_CH_BANDGAP, ADC_SPT_CLK_215, 0);  //采样&转换时间= (7+12)/24000000 + 5/24000000 = 1us

    ADC_SoftwareStartRegularConvert(ADC0);  
    while (!ADC_GetEOCFlag(ADC0));
    bandGapAVG = ADC_GetRegularData(ADC0);
   // referVol = 4095.0 * bgVol / bandGapAVG;
    
    ADC_SetRegularGroupChannel(ADC0, ADC_CH_TSENSOR, ADC_SPT_CLK_215, 0);
    while (1)
    {
        ADC_SoftwareStartRegularConvert(ADC0);  
        while (!ADC_GetEOCFlag(ADC0));
        
        TsensorVal = ADC_GetRegularData(ADC0);
        //Get Tsensor value and calculate Tsensor voltage(TsensorVol)
        TsensorVol = TsensorVal * bgVol/bandGapAVG;
        temperatureVal = 25 + (vt25Vol - TsensorVol) / 1.629;
        printf("[ADC]Tsensor temperature:%f\r\n", temperatureVal);
        mdelay(1000);
    }

}
