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
    //config PWM0 pinmux
    GPIO_SetFunc(GPIOB, GPIO_PIN12, GPIO_FUN1);//PWM0_CH2
    GPIO_SetFunc(GPIOB, GPIO_PIN11, GPIO_FUN1);//PWM0_CH3  
    
    GPIO_SetFunc(GPIOB, GPIO_PIN3, GPIO_FUN1);//PWM0_CH6
    GPIO_SetFunc(GPIOA, GPIO_PIN10, GPIO_FUN1);//PWM0_CH7

}
/**
* PWM0_SetDutyAndPhaseShift
*
* @param[in] pwmDuty: set PWM Duty(from 1 to PHASE_SHIFT_MAX_DUTY_CNT)
*            phaseShift:set phase shift value.(0-359)
*
* @return none
*
* @brief PWM0 CH6,CH7互补输出，PWM0 CH2,CH3互补输出。PWM0 CH2相对CH6移相phaseShift度
*
*/
void PWM0_SetDutyAndPhaseShift(uint16_t pwmDuty, uint16_t phaseShift)
{
    uint16_t pwmCnt = 0;
    uint16_t modulo = PWM0->MCVR;//读取MOD值 
    uint32_t ch6value,ch7value,ch2value,ch3value;
    uint16_t phaseShiftValue;
    phaseShiftValue = (phaseShift * modulo)/360;   //计算相移值
    pwmCnt = pwmDuty * (modulo + 1) / PHASE_SHIFT_MAX_DUTY_CNT;
    ch6value = 0;
    ch7value = pwmCnt + ch6value;
    ch2value = ch6value  + phaseShiftValue;
    if (ch2value > modulo)
    {
        ch2value -= modulo;
    }
    ch3value = pwmCnt + ch2value;
    if (ch3value > modulo)
    {
        ch3value -= modulo;
    }
    PWM0->CHANNELS[2].CHV = ch2value;  //CH2通道赋值
    PWM0->CHANNELS[3].CHV = ch3value;
    PWM0->CHANNELS[6].CHV = ch6value;
    PWM0->CHANNELS[7].CHV = ch7value;
    PWM_SetSoftwareTriggerSync(PWM0, ENABLE);
}
/**
* PWM0_GenerateFrequency
* 
* @param[in] none
* @return none
*
* @brief 
* PWM0_CH6(PB3),PWM0_CH7(PA10)互补输出，PWM0_CH2(PB12),PWM0_CH3(PB11)互补输出。
*
*/
void PWM0_GenerateFrequency(void)
{
    PWM_CombineChConfig combineChConfig[2]; //组合模式相关结构体
    PWM_ModulationConfigType pwmConfig; //PWM模式相关结构体
    PWM_ConfigType config; //PWM模块结构体
    //结构体数据清零
    memset(&combineChConfig, 0, sizeof(combineChConfig));
    memset(&pwmConfig, 0, sizeof(pwmConfig));
    memset(&config, 0, sizeof(config));
    
    /*通道0/1配成组合模式PWM输出*/
    /*
    向上计数组合模式：
    周期=(MCVR-CNTIN+1)*PWM计数器时钟周期
    占空比=|CH(n+1)V-CH(n)V|*PWM计数器时钟周期
    向上-向下计数组合模式：
    周期=2*(MCVR-CNTIN)*PWM计数器时钟周期
    占空比=2*(|CH(n+1)V-CH(n)V|)*PWM计数器时钟周期
    */
    combineChConfig[0].pairChannel = PWM_CH_2; //PWM通道对数，PWM_CH_0/2/4/6对应PAIR0/1/2/3
    combineChConfig[0].ch1stValue = 0; //通道2n channel值，n为PWM对数编号
    combineChConfig[0].ch2ndValue = 0; //通道2n+1 channel值，n为PWM对数编号
    combineChConfig[0].levelMode = PWM_HIGH_TRUE; //输出PWM高有效,如果占空比设为25%，是指的高有效电平占比25%
    combineChConfig[0].deadtimeEn = DISABLE;//死区插入去能
    combineChConfig[0].complementEn = ENABLE;//互补模式使能,使能后，PWM通道波形互补，DISABLE波形输出同向
    combineChConfig[0].ch1stMatchDir = PWM_MATCH_DIR_DOWN;//仅在向上-向下计数(countMode为PWM_UP_DOWN_COUNT)组合模式有效，用于选择匹配生效点方向
    combineChConfig[0].ch2ndMatchDir = PWM_MATCH_DIR_DOWN;//仅在向上-向下计数(countMode为PWM_UP_DOWN_COUNT)组合模式有效，用于选择匹配生效点方向
    combineChConfig[0].ch1stPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//输出极性高有效，PWM mask后PWM输出低电平
    combineChConfig[0].ch2ndPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//输出极性高有效，PWM mask后PWM输出低电平
    combineChConfig[0].ch1stInterruptEn = DISABLE;//PWM通道匹配中断使能位
    combineChConfig[0].ch2ndInterruptEn = DISABLE;//PWM通道匹配中断使能位
    combineChConfig[0].ch1stInitLevel = PWM_LOW_LEVEL;//PWM初始电平输出为低，该配置受initChOutputEn控制，决定PWM计数器未工作前PWM口的输出电平值。
    combineChConfig[0].ch2ndInitLevel = PWM_LOW_LEVEL;//PWM初始电平输出为低，该配置受initChOutputEn控制，决定PWM计数器未工作前PWM口的输出电平值。
    combineChConfig[0].ch1stTriggerEn = DISABLE;//通道2n外部触发使能，n为PWM对数编号
    combineChConfig[0].ch2ndTriggerEn = DISABLE;//通道2n+1外部触发使能，n为PWM对数编号

    combineChConfig[1].pairChannel = PWM_CH_6; //PWM通道对数，PWM_CH_0/2/4/6对应PAIR0/1/2/3
    combineChConfig[1].ch1stValue = 0; //通道2n channel值，n为PWM对数编号
    combineChConfig[1].ch2ndValue = 0; //通道2n+1 channel值，n为PWM对数编号
    combineChConfig[1].levelMode = PWM_HIGH_TRUE; //输出PWM高有效,如果占空比设为25%，是指的高有效电平占比25%
    combineChConfig[1].deadtimeEn = DISABLE;//死区插入去能
    combineChConfig[1].complementEn = ENABLE;//互补模式使能,使能后，PWM通道波形互补，DISABLE波形输出同向
    combineChConfig[1].ch1stMatchDir = PWM_MATCH_DIR_DOWN;//仅在向上-向下计数(countMode为PWM_UP_DOWN_COUNT)组合模式有效，用于选择匹配生效点方向
    combineChConfig[1].ch2ndMatchDir = PWM_MATCH_DIR_DOWN;//仅在向上-向下计数(countMode为PWM_UP_DOWN_COUNT)组合模式有效，用于选择匹配生效点方向
    combineChConfig[1].ch1stPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//输出极性高有效，PWM mask后PWM输出低电平
    combineChConfig[1].ch2ndPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//输出极性高有效，PWM mask后PWM输出低电平
    combineChConfig[1].ch1stInterruptEn = DISABLE;//PWM通道匹配中断使能位
    combineChConfig[1].ch2ndInterruptEn = DISABLE;//PWM通道匹配中断使能位
    combineChConfig[1].ch1stInitLevel = PWM_LOW_LEVEL;//PWM初始电平输出为低，该配置受initChOutputEn控制，决定PWM计数器未工作前PWM口的输出电平值。
    combineChConfig[1].ch2ndInitLevel = PWM_LOW_LEVEL;//PWM初始电平输出为低，该配置受initChOutputEn控制，决定PWM计数器未工作前PWM口的输出电平值。
    combineChConfig[1].ch1stTriggerEn = DISABLE;//通道2n外部触发使能，n为PWM对数编号
    combineChConfig[1].ch2ndTriggerEn = DISABLE;//通道2n+1外部触发使能，n为PWM对数编号

    /*modulation mode config*/
    pwmConfig.countMode = PWM_UP_COUNT; //PWM计数器模式 (不同的计数模式频率及占空比计算方式不同)
    pwmConfig.independentChannelNum = 0; //独立通道数
    pwmConfig.combineChannelNum = 2; //组合对数
    pwmConfig.independentChConfig = NULL; //独立通道配置变量地址赋值
    pwmConfig.combineChConfig = combineChConfig; //组合通道配置变量地址赋值
    pwmConfig.deadtimePsc = PWM_DEADTIME_DIVID_1;//死区插入分频值，与deadtime一起决定插入死区的时间。
    pwmConfig.deadtime = 0;  //死区时间 = (DTPSC * DTVAL)/PWM计数器时钟周期
    pwmConfig.initChOutputEn = ENABLE; //使能初始化通道输出，使能后独立PWM模式的initLevel和组合PWM模式的ch1stInitLevel和ch2ndPolarity配置才会生效
    pwmConfig.initTriggerEn = DISABLE; //通道外部触发使能
    
    /*pwm config*/
    config.mode = PWM_MODE_MODULATION;//PWM模块配置为PWM模式
    config.initModeStruct = &pwmConfig;//PWM配置结构体地址赋值
    config.clkSource = PWM_CLK_SOURCE_APB; //PWM时钟源配置
    config.clkPsc = PWM_PRES;//PWM时钟源分频
    config.initValue = 0;//计数器初始寄存器值
    config.maxValue = PHASE_SHIFT_MOD_PWM - 1; //PWM计数器最大值
    config.overflowInterrupEn = DISABLE;//计数器溢出中断去能
    config.cntOverflowFreq = 0;//CNTOF中断产生的频率与计数器频率的关系(0-127), 0表示每次计数器溢出都产生溢出中断，1表示间隔1次，2表示间隔2次，以此内推。
    config.interruptEn = DISABLE; //PWM中断去能
    config.callBack = NULL; //PWM中断回调
    
    PWM_Init(PWM0, &config); //配置初始化生效

    NVIC_SetPriority(PWM0_IRQn, 0); //设置PWM模块中断的优先级
}
/**
* PWM_PhaseShiftOutput
*
* @param[in]  None
* @return     None
*
* @brief PWM0占空比循环递增
          
*/
void PWM_PhaseShiftOutput(void)
{
    uint16_t pwmDuty = 0,phaseShift = 180;
    PWM0_GenerateFrequency();
    while (1)
    {
        pwmDuty ++;
        if (pwmDuty > PHASE_SHIFT_MAX_DUTY_CNT)
        {
            pwmDuty = 0;
        }
        PWM0_SetDutyAndPhaseShift(pwmDuty, phaseShift);
        mdelay(100); 
        printf("\noutput pwm duty: %d, pahse shift: %d.\r\n", pwmDuty/10, phaseShift);
    }
}

/**********<End>*********/
