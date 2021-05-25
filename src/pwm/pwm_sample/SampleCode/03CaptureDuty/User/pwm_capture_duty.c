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
 /**********<Incldue>***********/
#include "pwm_sample.h"

/**********<Macro>************/
#define   CAPTURE_FREQ          0
#define   CAPTURE_DUTY          1
/**********<Variable>*********/
uint32_t g_captureValue0;
uint32_t g_captureValue1;
uint32_t g_countValue = 0;
uint32_t g_freqCountValue = 1;
uint32_t g_captureMask = 0;///<capture mask flag
uint32_t g_captrueDuty;
uint32_t g_captureFreq = 0;
uint8_t g_captureMode = 0; //0 freq 1 duty
/**********<Prototype>********/
void PWM0_Capture_Duty(void);

/**********<Prototype>*********/
/**
* PWM0_Singal_Capture
* @param[in] none
*
* @return none
*
* @brief PWM 捕获功能中断处理函数
*
*/
/**
* PWM0_Singal_Capture
*
* @param[in] none
*
* @return none
*
* @brief PWM0 CallBack
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
        
        //user add
        //通道0中断
        if (lpara & PWM_STR_CH0SF_Msk)
        {
            
        }
        //通道1中断
        if (lpara & PWM_STR_CH1SF_Msk)
        {
            //获取CH0,CH1 count值
            g_captureValue0 = PWM0->CHANNELS[0].CHV;
            g_captureValue1 = PWM0->CHANNELS[1].CHV;
            if (g_captureValue0)
            {
                //如果CH1记录值小于CH0，说明CH1有溢出，计算前需加上溢出值
                if (g_captureValue1 < g_captureValue0)
                {
                    g_captureValue1 = g_captureValue1 + 0xffff;
                }
                //计算CH1与CH0差值。
                g_countValue = g_captureValue1 - g_captureValue0;
            }
        }
        if (g_countValue)
        {
            if (g_captureMode == CAPTURE_DUTY)
            {
                if( 0 != g_captureFreq)
                {
                    /*
                        捕获占空比的计算公式：
                        PWM周期 = 1 / (PWM模块时钟 /PWM模块分频/PWM模块MOD值/信号频率)
                        占空比 = 高电平时间 / PWM周期
                    */

                    g_captrueDuty = (g_countValue * 100)/ g_freqCountValue;
                }
            }
            else if (g_captureMode == CAPTURE_FREQ)
            {
                /*
                 频率 = PWM时钟源 / PWM模块分频值 / 捕获到的Count间隔。
                 */
                g_captureFreq = APB_CLK / (PWM_CAPTURE_PRESCALER + 1) / g_countValue;
                g_freqCountValue = g_countValue;
                if( 0 != g_captureFreq)
                {
                    g_captureMode = CAPTURE_DUTY;
                    PWM0_Capture_Duty();
                }
            }
        }

    }
}
/**
* PWM0_Capture_Duty
*
* @param[in]  None
* @return     None
*
* @brief  双通道边沿捕获占空比原理：
          把CH2n设为上升沿，CH2n+1设为下降沿，当出现上降沿时，CH2n记录当前的Count值，
          当出现下降沿时，CH2n+1记录当前的Count值，并在CH2n+1捕获到下降沿时产生通道中断。
          然后基于两个通道计算出高电平的Count值。如果已知频率，即可算出占空比。
*/
void PWM0_Capture_Duty(void)
{
    PWM_DeInit(PWM0);
    PWM_inputChConfigType inputChConfig[1];//输入通道配置
    PWM_inputCaptureConfigType inputCapconfig;//输入捕获结构体
    PWM_ConfigType config; //PWM模块结构体

    //结构体数据清零
    memset(&inputChConfig, 0, sizeof(inputChConfig));
    memset(&inputCapconfig, 0, sizeof(inputCapconfig));
    memset(&config, 0, sizeof(config));
    //输入通道结构体配置，如果有多个输入通道，更改结构体数组，在后面添加相关配置即可。

    /*
        初始化PWM为双边沿捕获模式，会使用一组通道进行捕获。信号输入为偶数编号通道。
        捕获频率检测方式设为上升或下降沿，捕获占空比检测方式设为高脉宽或低脉宽检测。
    */
    inputChConfig[0].channel = PWM_CH_0; //信号捕获通道
    inputChConfig[0].mode = PWM_INPUTCAP_DUAL_EDGE; //双边沿捕获模式
    inputChConfig[0].measureType = PWM_POSITIVE_PLUSE_WIDTH_MEASURE;// 高电平检测
    inputChConfig[0].onceMode = PWM_INPUTCAP_CONTINUOUS;//连续捕获
    inputChConfig[0].filterEn = DISABLE;//使能输入滤波，CH0-CH3才有该功能
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
* PWM0_Capture_Freq
*
* @param[in]  None
* @return     None
*
* @brief  也可以通过双通道边沿捕获PWM频率。（注意，这两路通道必须是一组，如PWM2 CH0,CH1）
          双通道边沿捕获频率原理：
          如果把双通道捕获模式都设为下降沿，当奇数次出现下降沿时，CH2n记录当前的Count值，
          当出现偶数次下降沿时，CH2n+1记录当前的Count值，并在CH2n+1捕获到下降沿时产生通道中断。
          然后基于两个通道统计的Count值计算频率
          
*/
void PWM0_Capture_Freq(void)
{
    PWM_DeInit(PWM0);//去初始化PWM0  
    PWM_inputChConfigType inputChConfig[2];//输入通道配置
    PWM_inputCaptureConfigType inputCapconfig;//输入捕获结构体
    PWM_ConfigType config; //PWM模块结构体

    //结构体数据清零
    memset(&inputChConfig, 0, sizeof(inputChConfig));
    memset(&inputCapconfig, 0, sizeof(inputCapconfig));
    memset(&config, 0, sizeof(config));
    //输入通道结构体配置，如果有多个输入通道，更改结构体数组，在后面添加相关配置即可。
    /*
        初始化PWM为双边沿捕获模式，会使用一组通道进行捕获。信号输入为偶数编号通道。
        捕获频率检测方式设为上升或下降沿，捕获占空比检测方式设为高脉宽或低脉宽检测。
    */
    //通道0
    inputChConfig[0].channel = PWM_CH_0; //信号捕获通道
    inputChConfig[0].mode = PWM_INPUTCAP_DUAL_EDGE; //双边沿捕获模式
    inputChConfig[0].measureType = PWM_FALLING_EDGE_PERIOD_MEASURE;// 下降沿检测
    inputChConfig[0].onceMode = PWM_INPUTCAP_CONTINUOUS;//连续捕获
    inputChConfig[0].filterEn = DISABLE;//使能输入滤波，CH0-CH3才有该功能
    inputChConfig[0].filterValue = 0; //filterValue值范围0-31，输入信号将被延迟filterValue*4个总线时钟
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
* PWM_SampleCaptureDuty
*
* @param[in]  None
* @return     None
*
* @brief  通过PWM双边沿捕获频率和占空比。先获取频率，再获取占空比。如果想同时获取频率和占空比，推荐使用PWDT模块
          
*/
void PWM_SampleCaptureDuty(void)
{
    PWM0_Capture_Freq();
}

/**********<End>*********/
