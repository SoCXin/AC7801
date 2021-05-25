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

/**********<Macro>************/
#define MAX_DUTY_CNT                    (4096)
#define ratio_q12(n)                    (int16_t)(n * MAX_DUTY_CNT / 100)
#define PWM_AH                          GPIO_PB8 //PWM1_CH2
#define PWM_AL                          GPIO_PB7 //PWM1_CH3
#define PWM_BH                          GPIO_PB10 //PWM1_CH4
#define PWM_BL                          GPIO_PB9 //PWM1_CH5
#define PWM_CH                          GPIO_PB5 //PWM1_CH0
#define PWM_CL                          GPIO_PB4 //PWM1_CH1
#define CUR_FAULT_DETECT                GPIO_PB6 //PWM1_FLT0
/**********<Union>***********/
typedef enum {      ///<Define BLDC 3 phase
    PHASEA = 0,     ///<phase A
    PHASEB,         ///<phase B
    PHASEC,         ///<phase C
    PHASE_NUM,
}MOTOR_PHASE;
/**********<Struct>*********/

/**********<Variable>*******/
uint16_t g_startupPwmRatio = ratio_q12(15);
uint16_t g_startupDelay = 0;
static PWM_ChannelType const s_motorPhaseCtrlMap [PHASE_NUM][2] =
{
    {PWM_CH_0, PWM_CH_1},
    {PWM_CH_2, PWM_CH_3},
    {PWM_CH_4, PWM_CH_5}
};
/**********<Prototype>********/
void BLDC_Stop(void);

/**
* BLDC_GPIO_Init
*
* @param[in] none
*
* @return none
*
* @brief 初始化BLDC gpio
*
*/
void BLDC_GPIO_Init(void)
{
    GPIO_SetFunc(GPIOB, GPIO_PIN5, GPIO_FUN1);//PWM1_CH0 
    GPIO_SetFunc(GPIOB, GPIO_PIN4, GPIO_FUN1);//PWM1_CH1
    GPIO_SetFunc(GPIOB, GPIO_PIN8,GPIO_FUN1);//PWM1_CH2 
    GPIO_SetFunc(GPIOB, GPIO_PIN7, GPIO_FUN1);//PWM1_CH3
    GPIO_SetFunc(GPIOB, GPIO_PIN10, GPIO_FUN1);//PWM1_CH4 
    GPIO_SetFunc(GPIOB, GPIO_PIN9, GPIO_FUN1);//PWM1_CH5
    //trap
    GPIO_SetFunc(GPIOB, GPIO_PIN6, GPIO_FUN2);//PWM1_FLT0
} 
/**
* BLDC_ChangeMotorPhaseUseTrigMode
*
* @param[in] None
*
* @return none
*
* @brief Change BLDC PWM Phase using Trig Mode
*
*/
void BLDC_ChangeMotorPhaseUseTrigMode(void)
{
    PWM_SetSync(PWM1, ENABLE);
    PWM1->MODESEL |= PWM_MODESEL_PAIR0SYNCEN_Msk | PWM_MODESEL_PAIR1SYNCEN_Msk | PWM_MODESEL_PAIR2SYNCEN_Msk; ////Enable PWM Sync
    PWM1->SYNCONF |= PWM_SYNCONF_SWVSWSYNC_Msk |  PWM_SYNCONF_SYNCMODE_Msk | PWM_SYNCONF_SWOC_Msk;
    PWM1->SYNC |= PWM_SYNC_OMSYNCP_Msk;
    PWM1->SYNCONF |= PWM_SYNCONF_OMVSWSYNC_Msk |  PWM_SYNCONF_SYNCMODE_Msk ;
    PWM1->SYNCONF |= PWM_SYNCONF_CNTVSWSYNC_Msk | PWM_SYNCONF_PWMSVSWSYNC_Msk;  ////SW Trig MCVR and CHnV
}
/**
* BLDC_ClearAllPWMOutput
*
* @param[in] None
*
* @return none
*
* @brief Set ALL BLDC PWM Output to Low through Software Control
*
*/
void BLDC_ClearAllPWMOutput(void)
{
    PWM_SoftControlChannelLevel(PWM1, PWM_CH_0, PWM_LOW_LEVEL, ENABLE);
    PWM_SoftControlChannelLevel(PWM1, PWM_CH_1, PWM_LOW_LEVEL, ENABLE);
    PWM_SoftControlChannelLevel(PWM1, PWM_CH_2, PWM_LOW_LEVEL, ENABLE);
    PWM_SoftControlChannelLevel(PWM1, PWM_CH_3, PWM_LOW_LEVEL, ENABLE);
    PWM_SoftControlChannelLevel(PWM1, PWM_CH_4, PWM_LOW_LEVEL, ENABLE);
    PWM_SoftControlChannelLevel(PWM1, PWM_CH_5, PWM_LOW_LEVEL, ENABLE);
}
/**
* BLDC_PwmUpdate
*
* @param[in] positivePhase: positive phase, refer to enum BLDC_PHASE
* @param[in] negativePhase: negative phase, refer to enum BLDC_PHASE
*
* @return none
*
* @brief Set BLDC PWM Output phase
*
*/
void BLDC_PwmUpdate(uint8_t positivePhase, uint8_t negativePhase)
{
    uint8_t i = 0;
    BLDC_Stop();
    BLDC_ClearAllPWMOutput();
    //使用PWM_ON方式控制，及上半桥PWM控制，下半桥一直导通
    for (i = 0; i < PHASE_NUM; i++)
    {
        if (i == positivePhase)
        {
            PWM_SetChannelSoftControl(PWM1, s_motorPhaseCtrlMap[positivePhase][0], DISABLE);
            PWM_SetChannelOutputMask(PWM1, s_motorPhaseCtrlMap[positivePhase][0], DISABLE);
            PWM_SetChannelOutputMask(PWM1, s_motorPhaseCtrlMap[positivePhase][1], ENABLE);
        }
        if (i == negativePhase)
        {
            PWM_SetChannelOutputMask(PWM1, s_motorPhaseCtrlMap[negativePhase][0], ENABLE);
             //上半桥关闭，下半桥全通
            PWM_SoftControlChannelLevel(PWM1, s_motorPhaseCtrlMap[negativePhase][0], PWM_LOW_LEVEL, ENABLE);
            PWM_SoftControlChannelLevel(PWM1, s_motorPhaseCtrlMap[negativePhase][1], PWM_HIGH_LEVEL, ENABLE);
            PWM_SetChannelOutputMask(PWM1, s_motorPhaseCtrlMap[negativePhase][1], DISABLE); 
        }
    }
    PWM_SetSoftwareTriggerSync(PWM1, ENABLE);
    
}
/**
* BLDC_Stop
*
* @param[in] none
*
* @return none
*
* @brief Motor Stop Code
*
*/
void BLDC_Stop(void)
{
    //Mask ALL BLDC PWM Output
    PWM_SetChannelOutputMask(PWM1, PWM_CH_0, ENABLE);
    PWM_SetChannelOutputMask(PWM1, PWM_CH_1, ENABLE);
    PWM_SetChannelOutputMask(PWM1, PWM_CH_2, ENABLE);
    PWM_SetChannelOutputMask(PWM1, PWM_CH_3, ENABLE);
    PWM_SetChannelOutputMask(PWM1, PWM_CH_4, ENABLE);
    PWM_SetChannelOutputMask(PWM1, PWM_CH_5, ENABLE);
}
/**
* BLDC_SetMotorSpeed
*
* @param[in] motorSpeed: set Motor Speed(from 0 to 4096)
*
* @return none
*
* @brief Set Motor Speed
*
*/
void BLDC_SetMotorSpeed(uint16_t motorSpeed)
{
    uint16_t pwmCnt = 0;
    uint16_t modulo = PWM1->MCVR;
    pwmCnt = motorSpeed * (modulo + 1) / MAX_DUTY_CNT;
    PWM1->CHANNELS[0].CHV = (modulo + 1 - pwmCnt) >> 1;
    PWM1->CHANNELS[1].CHV = (modulo + 1 + pwmCnt) >> 1;
    PWM1->CHANNELS[2].CHV = (modulo + 1 - pwmCnt) >> 1;
    PWM1->CHANNELS[3].CHV = (modulo + 1 + pwmCnt) >> 1;
    PWM1->CHANNELS[4].CHV = (modulo + 1 - pwmCnt) >> 1;
    PWM1->CHANNELS[5].CHV = (modulo + 1 + pwmCnt) >> 1;
    PWM_SetSoftwareTriggerSync(PWM1, ENABLE);
}
/**
* BLDC_PWM_FaultCallback
*
* @param[in]   none
* @return none
*
* @brief PWM模块中断处理函数
*
*/
void BLDC_PWM_FaultCallback(void *device, uint32_t wpara, uint32_t lpara)
{
    if (PWM_GetFaultChannelFlag(PWM1, PWM_FAULT_CH_1))
    {

        PWM_ClearFaultChannelFlag(PWM1, PWM_FAULT_CH_1);
    }
}
/**
* BLDC_PWM_Output
*
* @param[in] enable: 1,enable PWM Output; 0: Disable PWM Output
*
* @return none
*
* @brief 控制PWM输出，这时PWM模块仍然在正常工作，只是PWM未输出到引脚上。
*
*/
void BLDC_PWM_Output(uint8_t enable)
{
    if (enable)
    {
        PWM_SetChannelOutputMask(PWM1, PWM_CH_0, DISABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_1, DISABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_2, DISABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_3, DISABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_4, DISABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_5, DISABLE);   
    }
    else
    {
        PWM_SetChannelOutputMask(PWM1, PWM_CH_0, ENABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_1, ENABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_2, ENABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_3, ENABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_4, ENABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_5, ENABLE);       
    }
    PWM_SetSoftwareTriggerSync(PWM1, ENABLE);
}
/**
* BLDC_PWM_Config_COMP_Mode
*
* @param[in] none
*
* @return none
*
* @brief 使能、去能PWM1互补模式
*
*/
void BLDC_PWM_Config_COMP_Mode(uint8_t enable)
{
    if (enable)
    {
        PWM1->MODESEL |= PWM_MODESEL_PAIR0COMPEN_Msk | PWM_MODESEL_PAIR1COMPEN_Msk | PWM_MODESEL_PAIR2COMPEN_Msk ;        
    }
    else
    {
        PWM1->MODESEL &= ~PWM_MODESEL_PAIR0COMPEN_Msk;
        PWM1->MODESEL &= ~PWM_MODESEL_PAIR1COMPEN_Msk;
        PWM1->MODESEL &= ~PWM_MODESEL_PAIR2COMPEN_Msk;         
    }
}
/**
* BLDC_PWM_Init
* 
* @param[in] none
* @return none
*
* @brief 配置PWM
*
*/
void BLDC_PWM_Init(void)
{
    PWM_CombineChConfig combineChConfig[4]; //组合模式相关结构体
    PWM_ModulationConfigType pwmConfig; //PWM模式相关结构体
    PWM_ConfigType config; //PWM模块结构体
    PWM_FaultConfig faultConfig;
    PWM_SyncConfigType syncConfig;
    //结构体数据清零
    memset(&combineChConfig, 0, sizeof(combineChConfig));
    memset(&pwmConfig, 0, sizeof(PWM_ModulationConfigType));
    memset(&faultConfig, 0, sizeof(PWM_FaultConfig));
    memset(&syncConfig, 0, sizeof(PWM_SyncConfigType));
    memset(&config, 0, sizeof(PWM_ConfigType));
    CKGEN_Enable(CLK_PWM1, ENABLE);
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
    combineChConfig[0].ch1stValue = 1; //通道2n channel值，n为PWM对数编号
    combineChConfig[0].ch2ndValue = 1; //通道2n+1 channel值，n为PWM对数编号
    combineChConfig[0].levelMode = PWM_HIGH_TRUE; //输出PWM高有效,如果占空比设为25%，是指的高有效电平占比25%
    combineChConfig[0].deadtimeEn = ENABLE;//死区插入使能，组合模式才支持死区插入
    combineChConfig[0].complementEn = ENABLE;//互补模式使能,使能后，PWM通道波形互补，DISABLE波形输出同向
    combineChConfig[0].ch1stMatchDir = PWM_MATCH_DIR_UP;//仅在向上-向下计数(countMode为PWM_UP_DOWN_COUNT)组合模式有效，用于选择匹配生效点方向
    combineChConfig[0].ch2ndMatchDir = PWM_MATCH_DIR_DOWN;//仅在向上-向下计数(countMode为PWM_UP_DOWN_COUNT)组合模式有效，用于选择匹配生效点方向
    combineChConfig[0].ch1stPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//输出极性高有效，PWM mask后PWM输出低电平
    combineChConfig[0].ch2ndPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//输出极性高有效，PWM mask后PWM输出低电平
    combineChConfig[0].ch1stInterruptEn = DISABLE;//PWM通道匹配中断使能位
    combineChConfig[0].ch2ndInterruptEn = DISABLE;//PWM通道匹配中断使能位
    combineChConfig[0].ch1stInitLevel = PWM_LOW_LEVEL;//PWM初始电平输出为低，该配置受initChOutputEn控制，决定PWM计数器未工作前PWM口的输出电平值。
    combineChConfig[0].ch2ndInitLevel = PWM_LOW_LEVEL;//PWM初始电平输出为低，该配置受initChOutputEn控制，决定PWM计数器未工作前PWM口的输出电平值。
    combineChConfig[0].ch1stTriggerEn = DISABLE;//通道2n外部触发使能，n为PWM对数编号
    combineChConfig[0].ch2ndTriggerEn = DISABLE;//通道2n+1外部触发使能，n为PWM对数编号

    /*channel 2/3*/
    combineChConfig[1].pairChannel = PWM_CH_2; //PWM通道对数，PWM_CH_0/2/4/6对应PAIR0/1/2/3
    combineChConfig[1].ch1stValue = 1; //通道2n channel值，n为PWM对数编号
    combineChConfig[1].ch2ndValue = 1; //通道2n+1 channel值，n为PWM对数编号
    combineChConfig[1].levelMode = PWM_HIGH_TRUE; //输出PWM高有效,如果占空比设为25%，是指的高有效电平占比25%
    combineChConfig[1].deadtimeEn = ENABLE;//死区插入使能，组合模式才支持死区插入
    combineChConfig[1].complementEn = ENABLE;//互补模式使能,使能后，PWM通道波形互补，DISABLE波形输出同向
    combineChConfig[1].ch1stMatchDir = PWM_MATCH_DIR_UP;//仅在向上-向下计数(countMode为PWM_UP_DOWN_COUNT)组合模式有效，用于选择匹配生效点方向
    combineChConfig[1].ch2ndMatchDir = PWM_MATCH_DIR_DOWN;//仅在向上-向下计数(countMode为PWM_UP_DOWN_COUNT)组合模式有效，用于选择匹配生效点方向
    combineChConfig[1].ch1stPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//输出极性高有效，PWM mask后PWM输出低电平
    combineChConfig[1].ch2ndPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//输出极性高有效，PWM mask后PWM输出低电平
    combineChConfig[1].ch1stInterruptEn = DISABLE;//PWM通道匹配中断使能位
    combineChConfig[1].ch2ndInterruptEn = DISABLE;//PWM通道匹配中断使能位
    combineChConfig[1].ch1stInitLevel = PWM_LOW_LEVEL;//PWM初始电平输出为低，该配置受initChOutputEn控制，决定PWM计数器未工作前PWM口的输出电平值。
    combineChConfig[1].ch2ndInitLevel = PWM_LOW_LEVEL;//PWM初始电平输出为低，该配置受initChOutputEn控制，决定PWM计数器未工作前PWM口的输出电平值。
    combineChConfig[1].ch1stTriggerEn = DISABLE;//通道2n外部触发使能，n为PWM对数编号
    combineChConfig[1].ch2ndTriggerEn = DISABLE;//通道2n+1外部触发使能，n为PWM对数编号
    /*channel 4/5*/
    combineChConfig[2].pairChannel = PWM_CH_4; //PWM通道对数，PWM_CH_0/2/4/6对应PAIR0/1/2/3
    combineChConfig[2].ch1stValue = 1; //通道2n channel值，n为PWM对数编号
    combineChConfig[2].ch2ndValue = 1; //通道2n+1 channel值，n为PWM对数编号
    combineChConfig[2].levelMode = PWM_HIGH_TRUE; //输出PWM高有效,如果占空比设为25%，是指的高有效电平占比25%
    combineChConfig[2].deadtimeEn = ENABLE;//死区插入使能，组合模式才支持死区插入
    combineChConfig[2].complementEn = ENABLE;//互补模式使能,使能后，PWM通道波形互补，DISABLE波形输出同向
    combineChConfig[2].ch1stMatchDir = PWM_MATCH_DIR_UP;//仅在向上-向下计数(countMode为PWM_UP_DOWN_COUNT)组合模式有效，用于选择匹配生效点方向
    combineChConfig[2].ch2ndMatchDir = PWM_MATCH_DIR_DOWN;//仅在向上-向下计数(countMode为PWM_UP_DOWN_COUNT)组合模式有效，用于选择匹配生效点方向
    combineChConfig[2].ch1stPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//输出极性高有效，PWM mask后PWM输出低电平
    combineChConfig[2].ch2ndPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//输出极性高有效，PWM mask后PWM输出低电平
    combineChConfig[2].ch1stInterruptEn = DISABLE;//PWM通道匹配中断使能位
    combineChConfig[2].ch2ndInterruptEn = DISABLE;//PWM通道匹配中断使能位
    combineChConfig[2].ch1stInitLevel = PWM_LOW_LEVEL;//PWM初始电平输出为低，该配置受initChOutputEn控制，决定PWM计数器未工作前PWM口的输出电平值。
    combineChConfig[2].ch2ndInitLevel = PWM_LOW_LEVEL;//PWM初始电平输出为低，该配置受initChOutputEn控制，决定PWM计数器未工作前PWM口的输出电平值。
    combineChConfig[2].ch1stTriggerEn = DISABLE;//通道2n外部触发使能，n为PWM对数编号
    combineChConfig[2].ch2ndTriggerEn = DISABLE;//通道2n+1外部触发使能，n为PWM对数编号

    /*modulation mode config*/
    pwmConfig.countMode = PWM_UP_COUNT; //PWM计数器模式 (不同的计数模式频率及占空比计算方式不同)
    pwmConfig.independentChannelNum = 0; //独立通道数
    pwmConfig.combineChannelNum = 3; //组合对数
    pwmConfig.independentChConfig = NULL; //独立通道配置变量地址赋值
    pwmConfig.combineChConfig = combineChConfig; //组合通道配置变量地址赋值
    pwmConfig.deadtimePsc = PWM_DEADTIME_DIVID_1;//死区插入分频值，与deadtime一起决定插入死区的时间。
    pwmConfig.deadtime = 24;  //死区时间 = (DTPSC * DTVAL)/PWM计数器时钟周期 = 1*24/24000000 = 1us
    pwmConfig.initChOutputEn = ENABLE; //使能初始化通道输出，使能后独立PWM模式的initLevel和组合PWM模式的ch1stInitLevel和ch2ndPolarity配置才会生效
    pwmConfig.initTriggerEn = DISABLE; //通道外部触发使能

    /*pwm config*/
    config.mode = PWM_MODE_MODULATION;//PWM模块配置为PWM模式
    config.initModeStruct = &pwmConfig;//PWM配置结构体地址赋值
    config.clkSource = PWM_CLK_SOURCE_NONE; //PWM时钟源配置
    config.clkPsc = PWM_PRES;//PWM时钟源分频
    config.initValue = 0;//计数器初始寄存器值
    config.maxValue = BLDC_PWM_PERIOD - 1; //PWM计数器最大值
    config.overflowInterrupEn = ENABLE;//计数器溢出中断使能
    config.cntOverflowFreq = 0;//CNTOF中断产生的频率与计数器频率的关系(0-127), 0表示每次计数器溢出都产生溢出中断，1表示间隔1次，2表示间隔2次，以此内推。
    config.interruptEn = ENABLE; //PWM中断使能
    config.callBack = BLDC_PWM_FaultCallback; //PWM中断回调
    PWM_Init(PWM1, &config); //配置初始化生效

    /*pwm 同步功能配置*/
    syncConfig.syncTriggerMode = PWM_SYNC_TRIGGER_SOFTWARE;  //软件触发同步
    syncConfig.hardwareSync0En = DISABLE;//硬件同步触发源0去能            
    syncConfig.hardwareSync1En = DISABLE;//硬件同步触发源1去能                   
    syncConfig.hardwareSync2En = DISABLE;//硬件同步触发源2去能                     
    syncConfig.autoClearHWTriggerEn = DISABLE; //
    syncConfig.counterInitSyncEn = ENABLE;//使能CNTIN同步 
    syncConfig.outCtrlSyncEn = ENABLE;//使能CHOSWCR同步
    syncConfig.inverterSyncEn = DISABLE;//去能INVCR同步               
    syncConfig.outmaskSyncEn = ENABLE;//使能OMCR同步               
    syncConfig.polaritySyncEn = DISABLE;//去能CHOPOLCR同步                
    syncConfig.chValueSyncEn[0] = ENABLE; //使能双通道CHV同步
    syncConfig.chValueSyncEn[1] = ENABLE; //使能双通道CHV同步
    syncConfig.chValueSyncEn[2] = ENABLE; //使能双通道CHV同步
    syncConfig.chValueSyncEn[3] = ENABLE; //使能双通道CHV同步
    syncConfig.maxLoadingPointEn = DISABLE;//去能最大加载点
    syncConfig.minLoadingPointEn = DISABLE;//去能最小加载点           
    syncConfig.syncEn = ENABLE;//使能同步                       
    PWM_InitSyncControl(PWM1, &syncConfig);//配置同步相关寄存器
    
    /* PWM故障配置 */
    faultConfig.mode = PWM_FAULT_CTRL_MANUAL_ALL; //手动清除故障
    faultConfig.channelConfig[PWM_FAULT_CH_1].faultInputEn = ENABLE;//使能PWM_FLT1通道
    faultConfig.channelConfig[PWM_FAULT_CH_1].faultFilterEn = ENABLE; //去能PWM FLT1通道滤波
    faultConfig.channelConfig[PWM_FAULT_CH_1].faultPolarity = PWM_INPUT_POLARITY_ACTIVE_LOW; //输入故障低有效，默认为高 
    faultConfig.filterValue = 0;//故障滤波器值
    faultConfig.faultCtrlOutputEn[0] = ENABLE;//PWM通道CH0,CH1故障使能
    faultConfig.faultCtrlOutputEn[1] = ENABLE;//PWM通道CH2,CH3故障使能
    faultConfig.faultCtrlOutputEn[2] = ENABLE;//PWM通道CH4,CH5故障使能
    faultConfig.faultCtrlOutputEn[3] = ENABLE;//PWM通道CH6,CH7故障使能
    faultConfig.interruptEn = ENABLE;//故障中断使能
    PWM_InitFaultControl(PWM1, &faultConfig); //配置PWM故障相关寄存器
    
    NVIC_SetPriority(PWM1_IRQn, 0);// 设置PWM1中断优先级
    ///PWM Start
    PWM_SetClockSource(PWM1, PWM_CLK_SOURCE_APB); //启动PWM计数器
}

/**
* BLDC_OpenKick
*
* @param[in] dir, motor running direction
*
* @return none
*
* @brief 开环方式让电机运行
*
*/
void BLDC_OpenKick(uint8_t dir)
{
    uint8_t flowState = 0;
    g_startupPwmRatio = ratio_q12(30);
    g_startupDelay = 10;
    do
    {
        BLDC_SetMotorSpeed(g_startupPwmRatio); 
        if (dir) //reverse
        {
            switch (flowState)
            {
            case 0:// A+,C-
                BLDC_PwmUpdate(PHASEA, PHASEC);
                flowState = 1;
            break;
            case 1: //A+,B-
                BLDC_PwmUpdate(PHASEA, PHASEB);
                flowState = 2;
            break;
            case 2:  //C+,B-
                BLDC_PwmUpdate(PHASEC, PHASEB);
                flowState = 3;
            break;
            case 3://C+,A-
                BLDC_PwmUpdate(PHASEC, PHASEA);
                flowState = 4;
            break;
            case 4: //B+,A-
                BLDC_PwmUpdate(PHASEB, PHASEA);
                flowState = 5;
            break;
            case 5://B+,C-
                BLDC_PwmUpdate(PHASEB, PHASEC);
                flowState = 0;
            break;
            default:
            break;
            }
        }
        else //forward
        {
            switch (flowState)
            {
            case 0:// A+,B-
                BLDC_PwmUpdate(PHASEA, PHASEB);
                flowState = 1;
            break;
            case 1: //A+,C-
                BLDC_PwmUpdate(PHASEA, PHASEC);
                flowState = 2;
            break;
            case 2:  //B+,C-
                BLDC_PwmUpdate(PHASEB, PHASEC);
                flowState = 3;
            break;
            case 3://B+,A-
                BLDC_PwmUpdate(PHASEB, PHASEA);
                flowState = 4;
            break;
            case 4: //C+,A-
                BLDC_PwmUpdate(PHASEC, PHASEA);
                flowState = 5;
            break;
            case 5://C+,B-
                BLDC_PwmUpdate(PHASEC, PHASEB);
                flowState = 0;
            break;
            default:
            break;
            }            
        }
    mdelay(g_startupDelay); 
    } while (1);
}  

/**
* PWM_SampleBldcControl
*
* @param[in]  None
* @return     None
*
* @brief 在电机demo板上开环运行电机
          
*/
void PWM_SampleBldcControl(void)
{
    //初始化PWM GPIO
    BLDC_GPIO_Init();
    //初始化PWM模块
    BLDC_PWM_Init();
    //使用六步方波开环运行电机
    BLDC_OpenKick(0);
}

/**********<End>*********/
