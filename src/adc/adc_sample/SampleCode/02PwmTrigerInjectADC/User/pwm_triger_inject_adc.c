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
#include "adc_sample.h"

/*********<Variable>********/
uint32_t g_averageSampleValue = 0;
uint16_t g_adcInjectValue[4];
uint8_t g_AMOFlag = 0;//模拟看门狗事件标志
/*
注意：EOC标志写0或读取ADC_RDR都会清除该标志位。
在进行debug时，如果有打开memory窗口或打开ADC寄存器。
该标志会被debug清除。
*/
uint8_t g_EOCFlag = 0; //规则组转换结束标志。
uint8_t g_IEOCFlag = 0; //注入组转换结束标志。
/**
 * ADC_DMACallback
 *
 * @param[in] void
 * @return void
 *
 * @brief ADC DMA中断回调函数。
 */
void ADC_Callback(void *device, uint32_t wpara, uint32_t lpara)
{   
    if (wpara & ADC_STR_IEOC_Msk)    //注入组中断标志
    {
        g_IEOCFlag = 1;
        g_adcInjectValue[0] = ADC_GetInjectData(ADC0, 0);
        g_adcInjectValue[1] = ADC_GetInjectData(ADC0, 1);
        g_adcInjectValue[2] = ADC_GetInjectData(ADC0, 2);
        g_adcInjectValue[3] = ADC_GetInjectData(ADC0, 3);
    }
    if (wpara & ADC_STR_EOC_Msk)    //规则组中断标志
    {
        g_EOCFlag = 1;
    }
    if (wpara & ADC_STR_AMO_Msk)    //模拟监控中断标志
    {
        g_AMOFlag = 1;
    }
}
/**
 * CTU_Config
 *
 * @param[in] void
 * @return void
 *
 * @brief 配置CTU模块，Timer0触发ADC规则组采样。
 */
void CTU_Config(void)
{
    CTU_ConfigType ctuConfig;
    memset(&ctuConfig, 0x00, sizeof(ctuConfig));
    ctuConfig.uart0RxFilterEn = DISABLE;  //去能UART0_RX滤波
    ctuConfig.rtcCaptureEn = DISABLE;  //去能RTC捕获
    ctuConfig.acmpCaptureEn = DISABLE;  //去能ACMP捕获
    ctuConfig.uart0RxCaptureEn = DISABLE;  //去能UART0_RX捕获
    ctuConfig.uartTxModulateEn = DISABLE;  //去能UART0_TX调制
    ctuConfig.clkPsc = CTU_CLK_PRESCALER_1;  //分频
//    ctuConfig.adcRegularTriggerSource = CTU_TRIGGER_ADC_TIMER_CH0_OVERFLOW; //Timer0触发ADC规则组采样。
//    ctuConfig.delay0Time = 0;  //触发延迟
    ctuConfig.adcInjectTriggerSource = CTU_TRIGGER_ADC_PWM1_INIT;  //PWM初始值触发ADC注入组采样
    ctuConfig.delay1Time = 0;
//    ctuConfig.pwdt0In3Source = CTU_PWDT_IN3_SOURCE_UART0_RX;
//    ctuConfig.pwdt1In3Source = CTU_PWDT_IN3_SOURCE_UART0_RX; 
    CTU_Init(&ctuConfig);
}


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
    //配置PINMUX
    GPIO_SetFunc(GPIOA, GPIO_PIN2, GPIO_FUN2);///<ADC_IN8 Analog function enable
    
    adcConfig->clkPsc = ADC_CLK_PRESCALER_1; ///<Set ADC Clk = 24M/2/(0+1)
    adcConfig->scanModeEn = ENABLE;                   //扫描模式
    adcConfig->continousModeEn = DISABLE;             //连续模式
    adcConfig->regularDiscontinousModeEn = DISABLE;   //1：打开规则组间断转换模式
    adcConfig->injectDiscontinousModeEn = DISABLE;    //1：打开注入组间断转换模式
    adcConfig->injectAutoModeEn = DISABLE;            //1：自动注入模式
    adcConfig->intervalModeEn = DISABLE;              //1：注入组为间隔转换模式
    adcConfig->regularDiscontinousNum = 0;            //
    adcConfig->EOCInterruptEn = ENABLE;              //EOC中断使能
    adcConfig->IEOCInterruptEn = ENABLE;             //IEOC中断使能
    adcConfig->interruptEn = ENABLE;                 //使能中断
    adcConfig->regularDMAEn = DISABLE;                 //去能ADC DMA
    adcConfig->regularTriggerMode = ADC_TRIGGER_INTERNAL;//ADC触发源，内部触发
    adcConfig->injectTriggerMode = ADC_TRIGGER_EXTERNAL; //ADC触发源，外部触发
    adcConfig->regularSequenceLength = 0;               //规则组长度设为0
    adcConfig->injectSequenceLength = 4;               //注入组长度设为4
    adcConfig->dataAlign = ADC_DATA_ALIGN_RIGHT;       //右对齐
    adcConfig->callBack =  ADC_Callback;
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
    ADC_SetInjectGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 0);  //采样&转换时间= (7+12)/24000000 + 5/24000000 = 1us
    ADC_SetInjectGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 1);  //采样&转换时间= (7+12)/24000000 + 5/24000000 = 1us
    ADC_SetInjectGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 2);  //采样&转换时间= (7+12)/24000000 + 5/24000000 = 1us
    ADC_SetInjectGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 3);  //采样&转换时间= (7+12)/24000000 + 5/24000000 = 1us
}

void sampleValueDeal(void)
{
    int i;
    uint32_t sumValue = 0;
    for (i = 0; i < 4; i++)
    {
        sumValue += g_adcInjectValue[i];
    }
    g_averageSampleValue = sumValue / 4;
    

}
/**
 * ADC_SamplePwmTrigerInject
 *
 * @param[in] void
 * @return void
 *
 * @brief Timer定时触发规则组ADC_CHANNEL0单次采样。
 */
void ADC_SamplePwmTrigerInject(void)
{
    CTU_Config();
    ADC_init();
    while(1)
    {
        mdelay(500);
        sampleValueDeal();
        printf("PWM Triger ADC Inject Sample value: %d\r\n", g_averageSampleValue);
    }
}

/**********<End>*********/
