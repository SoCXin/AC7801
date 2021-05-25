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
#include "ac780x_gpio.h"
#include "ac780x.h"
#include "ac780x_timer.h"
#include "ac780x_pwm.h"
#include "pwm_sample.h"
/**********<Prototype>*********/

/**********<Variable>*********/
uint32_t g_positvePulse;///<pulse width positive value
uint32_t g_negativePulse;///<pulse width negative value
uint32_t g_freq;///<pulse width period value
float g_duty;///<pulse duty value
uint32_t g_hallStatus;///<hall status value
uint32_t g_overFlowCount;//PWM1计数溢出中断
uint8_t g_falutCnt = 0;
/**
* PWM1_CallBack
*
* @param[in] device: device pointer
* @param[in] wpara: callback parameter
* @param[in] lpara: callback parameter
* @return none
* @brief PWM CallBack
*/
void PWM1_CallBack(void *device, uint32_t wpara, uint32_t lpara)
{
    //PWM计数器溢出中断
    if (wpara & PWM_INIT_CNTOF_Msk) 
    {
        g_overFlowCount ++;
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
            
        }
        
    }
    
    if (PWM_GetFaultChannelFlag(PWM1, PWM_FAULT_CH_0))
    {
        //若配为手动清除故障，清标志后，PWM会继续输出。
        g_falutCnt ++;
        printf("\npwm falut triger. falut count: %d\r\n", g_falutCnt);
        PWM_ClearFaultChannelFlag(PWM1, PWM_FAULT_CH_0); //配为手动清除故障标志时，需软件清标志，否则会一直触发中断
        //
        
    }
}
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
    GPIO_SetFunc(GPIOB, GPIO_PIN5, GPIO_FUN1);//PWM1_CH0 
    GPIO_SetFunc(GPIOB, GPIO_PIN4, GPIO_FUN1);//PWM1_CH1
    GPIO_SetFunc(GPIOC, GPIO_PIN9, GPIO_FUN1);//PWM1_CH6
    GPIO_SetFunc(GPIOB, GPIO_PIN3, GPIO_FUN2);//PWM1_CH7



}
/**
* PWM1_GenerateFrequency
* 
* @param[in] none
* @return none
*
* @brief 
* 1.PWM1_CH0,PWM1_CH1组合模式，PWM1_CH0输出频率为20K，占空比为25%的波形，PWM1_CH1与PWM1_CH0波形互补，带1us死区插入。
* 2.PWM1_CH6,PWM1_CH7独立PWM模式（边沿对齐），生成频率为20K，PWM1_CH6占空比50%，PWM1_CH7占空比25%的波形。
* 3.PWM计数器溢出中断，对输出PWM波进行计数
*
*/
void PWM1_GenerateFrequency(void)
{
    PWM_CombineChConfig combineChConfig[1]; //组合模式相关结构体
    PWM_IndependentChConfig independentChConfig[2];//独立模式相关结构体
    PWM_ModulationConfigType pwmConfig; //PWM模式相关结构体
    PWM_FaultConfig faultConfig;
    PWM_ConfigType config; //PWM模块结构体
    //结构体数据清零
    memset(&combineChConfig, 0, sizeof(combineChConfig));
    memset(&independentChConfig, 0, sizeof(independentChConfig));
    memset(&pwmConfig, 0, sizeof(pwmConfig));
    memset(&faultConfig, 0, sizeof(faultConfig));
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
    combineChConfig[0].pairChannel = PWM_CH_0; //PWM通道对数，PWM_CH_0/2/4/6对应PAIR0/1/2/3
    combineChConfig[0].ch1stValue = MOD_PWM >> 2; //通道2n channel值，n为PWM对数编号
    combineChConfig[0].ch2ndValue = MOD_PWM >> 1; //通道2n+1 channel值，n为PWM对数编号
    combineChConfig[0].levelMode = PWM_HIGH_TRUE; //输出PWM高有效,如果占空比设为25%，是指的高有效电平占比25%
    combineChConfig[0].deadtimeEn = ENABLE;//死区插入使能，组合模式才支持死区插入
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
    /*
    边沿对齐PWM模式：
    周期=(MCVR-CNTIN+1)*PWM计数器时钟周期
    占空比=(CHnV-CNTIN+1)*PWM计数器时钟周期
    中心对齐PWM模式：
    周期=2*(MCVR-CNTIN)*PWM计数器时钟周期
    占空比=2*(CH(n)V-CNTIN)*PWM计数器时钟周期
    */
    /*channel 6*/
    independentChConfig[0].channel = PWM_CH_6; //通道6
    independentChConfig[0].chValue = MOD_PWM >> 1;//通道6 channel值，输出占空比 = chValue / maxValue = 50%
    independentChConfig[0].levelMode = PWM_HIGH_TRUE; //输出PWM高有效
    independentChConfig[0].polarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH; //输出极性高有效，PWM mask后PWM输出低电平
    independentChConfig[0].interruptEn = DISABLE;//PWM通道匹配中断使能位
    independentChConfig[0].initLevel = PWM_LOW_LEVEL;//PWM初始电平输出为低，该配置受initChOutputEn控制，决定PWM计数器未工作前PWM口的输出电平值。
    independentChConfig[0].triggerEn = DISABLE;//通道外部触发使能
    /*channel 7*/
    independentChConfig[1].channel = PWM_CH_7; //通道7
    independentChConfig[1].chValue = MOD_PWM >> 2;//通道7 channel值，输出占空比 = chValue / maxValue = 25%
    independentChConfig[1].levelMode = PWM_HIGH_TRUE; //输出PWM高有效
    independentChConfig[1].polarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH; //输出极性高有效，PWM mask后PWM输出低电平
    independentChConfig[1].interruptEn = DISABLE;//PWM通道匹配中断使能位
    independentChConfig[1].initLevel = PWM_LOW_LEVEL;//PWM初始电平输出为低，该配置受initChOutputEn控制，决定PWM计数器未工作前PWM口的输出电平值。
    independentChConfig[1].triggerEn = DISABLE;//通道外部触发使能

    /*modulation mode config*/
    pwmConfig.countMode = PWM_UP_COUNT; //PWM计数器模式 (不同的计数模式频率及占空比计算方式不同)
    pwmConfig.independentChannelNum = 2; //独立通道数
    pwmConfig.combineChannelNum = 1; //组合对数
    pwmConfig.independentChConfig = independentChConfig; //独立通道配置变量地址赋值
    pwmConfig.combineChConfig = combineChConfig; //组合通道配置变量地址赋值
    pwmConfig.deadtimePsc = PWM_DEADTIME_DIVID_1;//死区插入分频值，与deadtime一起决定插入死区的时间。
    pwmConfig.deadtime = 24;  //死区时间 = (DTPSC * DTVAL)/PWM计数器时钟周期 = 1*24/24000000 = 1us
    pwmConfig.initChOutputEn = ENABLE; //使能初始化通道输出，使能后独立PWM模式的initLevel和组合PWM模式的ch1stInitLevel和ch2ndPolarity配置才会生效
    pwmConfig.initTriggerEn = DISABLE; //通道外部触发使能
    
    /*pwm config*/
    config.mode = PWM_MODE_MODULATION;//PWM模块配置为PWM模式
    config.initModeStruct = &pwmConfig;//PWM配置结构体地址赋值
    config.clkSource = PWM_CLK_SOURCE_APB; //PWM时钟源配置
    config.clkPsc = PWM_PRES;//PWM时钟源分频
    config.initValue = 0;//计数器初始寄存器值
    config.maxValue = MOD_PWM - 1; //PWM计数器最大值
    config.overflowInterrupEn = ENABLE;//计数器溢出中断使能
    config.cntOverflowFreq = 0;//CNTOF中断产生的频率与计数器频率的关系(0-127), 0表示每次计数器溢出都产生溢出中断，1表示间隔1次，2表示间隔2次，以此内推。
    config.interruptEn = ENABLE; //PWM中断使能
    config.callBack = PWM1_CallBack; //PWM中断回调
    
    PWM_Init(PWM1, &config); //配置初始化生效
    
    /* PWM故障配置 */
    faultConfig.mode = PWM_FAULT_CTRL_MANUAL_ALL; //手动清除故障
    faultConfig.channelConfig[PWM_FAULT_CH_0].faultInputEn = ENABLE;//使能PWM_FLT0通道
    faultConfig.channelConfig[PWM_FAULT_CH_0].faultFilterEn = DISABLE; //去能PWM FLT0通道滤波
    faultConfig.channelConfig[PWM_FAULT_CH_0].faultPolarity = PWM_INPUT_POLARITY_ACTIVE_HIGH; //输入故障高有效，默认为低 
    faultConfig.filterValue = 0;//故障滤波器值
    faultConfig.faultCtrlOutputEn[0] = ENABLE;//PWM通道CH0,CH1故障使能
    faultConfig.faultCtrlOutputEn[1] = ENABLE;//PWM通道CH2,CH3故障使能
    faultConfig.faultCtrlOutputEn[2] = ENABLE;//PWM通道CH4,CH5故障使能
    faultConfig.faultCtrlOutputEn[3] = ENABLE;//PWM通道CH6,CH7故障使能
    faultConfig.interruptEn = ENABLE;//故障中断使能
    PWM_InitFaultControl(PWM1, &faultConfig); //配置PWM故障相关寄存器

    NVIC_SetPriority(PWM1_IRQn, 0); //设置PWM模块中断的优先级
}
/**********<End>*********/
