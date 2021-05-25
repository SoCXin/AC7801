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

/**
*
* @param[in] none
*
* @return none
*
* @brief PWM GPIO Init
*
*/
void PWM_GpioInit(void)
{

    //配置GPIO为PWM
    //config PWM1 pinmux
    GPIO_SetFunc(GPIOC, GPIO_PIN9, GPIO_FUN1);//PWM1_CH6
}
/**
* PWM0_CompTask
*
* @param[in] none
*
* @return none
*
* @brief PWM Output Compare CallBack
*/
void PWM0_CompTask(void *device, uint32_t wpara, uint32_t lpara)
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
            
        }
        //通道1中断
        if (lpara & PWM_STR_CH1SF_Msk)
        {
            
        }
    }
}

/**
* PWM_SampleOutputCompare
*
* @param[in]  None
* @return     None
*
* @brief PWM0 CH0设为输出比较模式。
          
*/
void PWM_SampleOutputCompare(void)
{

    PWM_OutputCompareChConfigType outputChConfig[1];//输出比较通道配置结构体
    PWM_OutputCompareConfigType outputCompconfig;//输出比较配置结构体
    PWM_ConfigType config; //PWM模块结构体

    //结构体数据清零
    memset(&outputChConfig, 0, sizeof(outputChConfig));
    memset(&outputCompconfig, 0, sizeof(outputCompconfig));
    memset(&config, 0, sizeof(config));

    outputChConfig[0].channel = PWM_CH_6; //输出比较通道

    outputChConfig[0].comparedValue = (MOD_OUTPUT_COMPARE_PWM>>1); //输出比较模式匹配点，匹配点设到中点。
    outputChConfig[0].mode = PWM_TOGGLE_OUTPUT;//当计时器值与CH0V寄存器值匹配时，翻转PWM0 CH0的输出。（也可配为设值PWM_CLEAR_OUTPUT，PWM_SET_OUTPUT）
    outputChConfig[0].interruptEn = ENABLE;//中断使能
    outputChConfig[0].initLevel = PWM_HIGH_LEVEL;//初始输出电平
    outputChConfig[0].triggerEn = DISABLE;//通道触发去能
    //输出比较配置
    outputCompconfig.channelNum = 1;
    outputCompconfig.channelConfig = outputChConfig;
    //PWM模块配置
    config.mode = PWM_MODE_OUTPUT_COMPARE;//PWM模块配置为输出比较模式
    config.initModeStruct = &outputCompconfig;//输出比较配置变量赋值
    config.clkSource = PWM_CLK_SOURCE_APB; //PWM时钟源配置

    config.clkPsc = PWM_OUTPUT_COMPARE_PRESCALER;//PWM时钟源分频
    config.initValue = 0;//计数器初始寄存器值
    /* 
        设值MOD值，会影响比较事件发生的频率。输出比较模式频率 = PWM模块频率 / （MOD值 + 1） 
    */
    config.maxValue = MOD_OUTPUT_COMPARE_PWM; //PWM计数器最大值
    config.overflowInterrupEn = DISABLE;//计数器溢出中断使能位
    config.cntOverflowFreq = 0;//CNTOF中断产生的频率与计数器频率的关系(0-127), 0表示每次计数器溢出都产生溢出中断，1表示间隔1次，2表示间隔2次，以此内推。
    
    config.interruptEn = ENABLE; //PWM中断使能
    config.callBack = PWM0_CompTask; //PWM中断回调

    PWM_Init(PWM1, &config); //配置初始化生效

    NVIC_SetPriority(PWM1_IRQn, 0); //设置PWM模块中断的优先级
}

/**********<End>*********/
