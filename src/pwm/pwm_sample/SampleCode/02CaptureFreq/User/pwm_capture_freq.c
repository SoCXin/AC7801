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
#include "pwm_sample.h"

/**********<Variable>*********/
uint32_t g_captureValueNew;///<capture pulse latest value
uint32_t g_captureValueOld; ///<capture pulse previous value
uint32_t g_captureMask = 0;///<capture mask flag
uint32_t g_captureFreq;
/**********<Prototype>*********/
/**
* PWM0_Singal_Capture
* @param[in] none
*
* @return none
*
* @brief 频率捕获原理：统计捕获通道两次CHV间隔的COUNT值，根据PWM时钟源，即可计算出频率。
*
*/
void PWM0_Singal_Capture(void *device, uint32_t wpara, uint32_t lpara)
{
     //PWM计数器溢出中断
    if (wpara & PWM_INIT_CNTOF_Msk) 
    {
        //user add 
    
    }
    //PWM通道中断
    //通过判断回调返回的lpara对应PWM_STR_CHnSF_Msk位状态判断是否发生通道中断
    if (lpara & PWM_STR_CHSF_Msk)
    {
        //通道0中断
        if (lpara & PWM_STR_CH0SF_Msk)
        {
            //通过g_captureMask来区别前后两次捕获
            if (0 == g_captureMask)
            {
                //获取第一次通道中断产生时CH0的CHV。
                g_captureValueOld = PWM0->CHANNELS[0].CHV;
                g_captureMask = 1;
            }
            else
            {
                //获取第二次通道中断产生时CH0的CHV。
                g_captureValueNew = PWM0->CHANNELS[0].CHV;
                //如果第二次的Count值小于第一次的Count值，说明通道有产生溢出，需把溢出值加上后进行计算。
                if (g_captureValueNew < g_captureValueOld)
                {
                    g_captureValueNew = g_captureValueNew + 0xffff;
                }
                /*
                 获取到两次下降沿（与init配置有关）中断间隔Count值后，即可通过如下公式计算出频率
                 频率 = PWM时钟源 / PWM模块分频值 / 捕获到的Count间隔。
                 */
                g_captureFreq = APB_CLK /(PWM_CAPTURE_PRESCALER + 1) / (g_captureValueNew - g_captureValueOld);
                g_captureMask = 0;
            }
        }
        //通道1中断
        if (lpara & PWM_STR_CH1SF_Msk)
        {
            
        }
    }
}
/**
* PWM0_Capture_Freq
*
* @param[in]  None
* @return     None
*
* @brief  通过一个PWM通道就可以捕获PWM频率。
          捕获频率原理：如果捕获模式设为上升沿，则当出现上升沿时，触发中断，在中断中去读取Channel值。
          注意：PWM模块不能同时捕获频率或占空比。若需要同时捕获频率和占空比，可用PWDT实现。 
          捕获占空比需使用双边沿模式（及需要两组通道，具体见PWM_SAMPLE_CAPTURE_DUTY例程）。
*/
void PWM0_Capture_Freq(void)
{
    PWM_inputChConfigType inputChConfig[1];//输入通道配置
    PWM_inputCaptureConfigType inputCapconfig;//输入捕获结构体
    PWM_ConfigType config; //PWM模块结构体

    //结构体数据清零
    memset(&inputChConfig, 0, sizeof(inputChConfig));
    memset(&inputCapconfig, 0, sizeof(inputCapconfig));
    memset(&config, 0, sizeof(config));
    //输入通道结构体配置，如果有多个输入通道，更改结构体数组，在后面添加相关配置即可。
    /*
    初始化PWM为捕获模式，捕获频率用单通道即可。任意一个通道都可以进行捕获。
    捕获模式可以设为上升沿或下降沿，不能设为双边沿。
    */
    inputChConfig[0].channel = PWM_CH_0; //信号捕获通道
    inputChConfig[0].mode = PWM_INPUTCAP_SINGLE_EDGE; //单边沿捕获模式
    inputChConfig[0].detectType = PWM_FALLING_EDGE_DETECT;//下降沿检测
    inputChConfig[0].onceMode = PWM_INPUTCAP_CONTINUOUS;//连续捕获
    inputChConfig[0].filterEn = ENABLE;//使能输入滤波，CH0-CH3才有该功能
    inputChConfig[0].filterValue = 10; //filterValue值范围0-31，输入信号将被延迟filterValue*4个总线时钟
    inputChConfig[0].eventPsc = PWM_EVENT_PSC_1; //输入事件与通道中断的对应关系。PWM_EVENT_PSC_1表示1次输入事件产生一次通道中断，PWM_EVENT_PSC_4表示4次输入事件产生一次通道中断。
    inputChConfig[0].interruptEn = ENABLE; //输入捕获中断使能
    
    //输入捕获配置
    inputCapconfig.channelNum = 1;
    inputCapconfig.channelConfig = inputChConfig;//输入通道配置变量地址赋值。
    
    //PWM模块配置
    config.mode = PWM_MODE_INPUT_CAPTURE;//PWM模块配置为输入捕获模式
    config.initModeStruct = &inputCapconfig;//输入捕获配置变量赋值
    config.clkSource = PWM_CLK_SOURCE_APB; //PWM时钟源配置
    /*
    设置捕获时钟频率：
    可捕获的最小频率（前提是时钟源为PWM_CLK_SOURCE_APB）：
    min_clk = SYS_CLK/(PWM_CAPTURE_PRESCALER+1)/65536 = 24000000/2/65536 = 183HZ
    */
    config.clkPsc = PWM_CAPTURE_PRESCALER;//PWM时钟源分频
    config.initValue = 0;//计数器初始寄存器值
    config.maxValue = 65535; //PWM计数器最大值
    config.overflowInterrupEn = DISABLE;//计数器溢出中断使能位
    config.cntOverflowFreq = 0;//CNTOF中断产生的频率与计数器频率的关系(0-127), 0表示每次计数器溢出都产生溢出中断，1表示间隔1次，2表示间隔2次，以此内推。
    
    config.interruptEn = ENABLE; //PWM中断使能
    config.callBack = PWM0_Singal_Capture; //PWM中断回调

    PWM_Init(PWM0, &config); //配置初始化生效

    NVIC_SetPriority(PWM0_IRQn, 0); //设置PWM模块中断的优先级
}
/**
* PWM_SampleCaptureFreq
*
* @param[in]  None
* @return     None
*
* @brief  PWM0_CH0通过捕获模式捕获PWM1 CH0输出频率
          
*/
void PWM_SampleCaptureFreq(void)
{
    PWM0_Capture_Freq();
}

/**********<End>*********/
