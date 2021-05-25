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

int16_t g_encoderCircleCnt = 0;  //编码器旋转圈数计数
uint16_t g_encoderCnt = 0; //编码器位置
uint8_t g_encoderDir = 0;  //编码器方向
/**
* PWM0_QuadTask
* @brief PWM quad decode call back function.
*
* @param[in] device: device pointer
* @param[in] wpara: callback parameter
* @param[in] lpara: callback parameter
* @return none
*/
void PWM0_QuadTask(void *device, uint32_t wpara, uint32_t lpara)
{
     //PWM计数器溢出中断
    if (wpara & PWM_INIT_CNTOF_Msk) 
    {
        //user add 
        g_encoderDir = (PWM0->QDI & PWM_QDI_QUADIR_Msk)>>2; //读取正交解码计数方向
        if (g_encoderDir != 0)
        {
            g_encoderCircleCnt++;
        }
        else
        {
            g_encoderCircleCnt--;
        }
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
*
* @param[in] none
*
* @return none
*
* @brief PWM GPIO Init
*
*/
void PWM_QuadDecodeGpioInit(void)
{
    //配置GPIO为PWM
    GPIO_SetFunc(GPIOA, GPIO_PIN1, GPIO_FUN1);//PWM0_CH0,A相
    GPIO_SetFunc(GPIOA, GPIO_PIN0, GPIO_FUN1);//PWM0_CH1,B相

}
/**
* PWM_QuadDecodeInit
*
* @param[in]  None
* @return     None
*
* @brief PWM0_CH0:A相输入，PWM0_CH1:B相输入
*/
void PWM_QuadDecodeInit(void)
{
    PWM_QuadPhaseConfigType quadPhaseAConfig,quadPhaseBConfig;
    PWM_QuadDecodeConfigType quadDecodeConfig;

    PWM_ConfigType config; //PWM模块结构体

    //结构体数据清零
    memset(&quadPhaseAConfig, 0, sizeof(quadPhaseAConfig));
    memset(&quadPhaseBConfig, 0, sizeof(quadPhaseBConfig));
    memset(&quadDecodeConfig, 0, sizeof(quadDecodeConfig));
    memset(&config, 0, sizeof(config));

    quadPhaseAConfig.polarity = PWM_QUAD_PHASE_NORMAL; //输入极性为正常极性。
    quadPhaseAConfig.filterEn = DISABLE;//滤波器去能
    quadPhaseAConfig.filterVal = 0;//滤波值
    quadPhaseBConfig.polarity = PWM_QUAD_PHASE_NORMAL; //输入极性为正常极性。
    quadPhaseBConfig.filterEn = DISABLE;//滤波器去能
    quadPhaseBConfig.filterVal = 0;//滤波值
    quadDecodeConfig.mode = PWM_QUAD_PHASE_ENCODE; //A相和B相编码模式
    quadDecodeConfig.phaseAConfig = quadPhaseAConfig; //A相配置赋值
    quadDecodeConfig.phaseBConfig = quadPhaseBConfig; //B相配置赋值
    quadDecodeConfig.quadEn = ENABLE; //正交解码使能
    //PWM模块配置
    config.mode = PWM_MODE_QUADRATURE_DECODER;//PWM模块配置为正交解码模式
    config.initModeStruct = &quadDecodeConfig;//正交解码配置赋值
    config.clkSource = PWM_CLK_SOURCE_APB; //PWM时钟源配置
    config.clkPsc = 0;//PWM时钟源分频
    config.initValue = 0;//计数器初始寄存器值
    config.maxValue = ENCODER_LINES - 1; //PWM计数器最大值
    config.overflowInterrupEn = ENABLE;//计数器溢出中断使能
    config.cntOverflowFreq = 0;//CNTOF中断产生的频率与计数器频率的关系(0-127), 0表示每次计数器溢出都产生溢出中断，1表示间隔1次，2表示间隔2次，以此内推。
    config.interruptEn = ENABLE; //PWM中断使能
    config.callBack = PWM0_QuadTask; //PWM中断回调
    PWM_Init(PWM0, &config); //配置初始化生效
    NVIC_SetPriority(PWM0_IRQn, 0); //设置PWM模块中断的优先级
}

void getEncoderCnt(void)
{
    g_encoderCnt = PWM0->CNT & 0xFFFF;   //获取编码器脉冲计数
}

/**
* PWM_QuadDecodeMode
*
* @param[in]  None
* @return     None
*
* @brief A相和B相编码模式
          
*/
void PWM_QuadDecodeMode(void)
{
    PWM_QuadDecodeGpioInit();
    PWM_QuadDecodeInit();
    while (1)
    {
        getEncoderCnt();
        mdelay(200); 
    }
}


/**********<End>*********/
