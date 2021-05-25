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
#include "acmp_sample.h"
/**********<Macro>************/

/**********<Variable>*********/
uint8_t g_acmpIntStatus = 0;

/**
* ACMP_CallBack
*
* @param[in] none
*
* @return none
*
* @brief callback routine of ACMP polling mode.
*
*/
void ACMP_CallBack(void *device, uint32_t wpara, uint32_t lpara)
{
    if (wpara & ACMP_SR_POLLING_F_Msk)
    {

    }
    if (wpara & ACMP_SR_WPF_Msk)
    {
        
    }
    if (wpara & ACMP_SR_F_Msk)
    {
         g_acmpIntStatus = wpara;
    }
}
/**
 * ACMP_init
 *
 * @param[in] void
 * @return void
 *
 * @brief 初始化ACMP,配置ACMP参数。
 */
void ACMP_init(void)
{
    ACMP_ConfigType acmpConfig;
    //配置PINMUX
    GPIO_SetFunc(GPIOA, GPIO_PIN11, GPIO_FUN2);  ///< ACMP_IN0
    GPIO_SetFunc(GPIOA, GPIO_PIN10, GPIO_FUN2);  ///< ACMP_IN1
    GPIO_SetFunc(GPIOA, GPIO_PIN9, GPIO_FUN2);  ///< ACMP_IN2
    GPIO_SetFunc(GPIOA, GPIO_PIN8, GPIO_FUN2);  ///< ACMP_IN3
    
    memset(&acmpConfig, 0x00, sizeof(acmpConfig));
    /* General parameters */
    acmpConfig.negativePin = ACMP_EXTERNAL_CH3; //负输入通道选择
    acmpConfig.hysterisisVol = ACMP_HYSTERISIS_20MV; //迟滞档位选择
    acmpConfig.edgeType = ACMP_EDGE_TRIGGER_FALLING_RISING;//中断触发模式
    acmpConfig.acmpOutEn = DISABLE;//ACMP输出使能
    acmpConfig.interruptEn = ENABLE;//ACMP中断去能
    acmpConfig.callBack = ACMP_CallBack; //中断回调
    acmpConfig.acmpEn = ENABLE;//ACMP使能
    /* Polling mode parameters */
    acmpConfig.pollingModeType = ACMP_POSITIVE_POLLING;//轮询比较器正输入端
    acmpConfig.pollingSequence = 7;//轮询通道序列设置，禁用/使能相应的通道
    acmpConfig.pollingDiv = ACMP_CLK_DIVIDE_256;//轮询分频值
    acmpConfig.hallAOutputCh = ACMP_EXTERNAL_CH0;//HALL A（输出对应PWDT_IN0）输入
    acmpConfig.hallBOutputCh = ACMP_EXTERNAL_CH1;//HALL B（输出对应PWDT_IN1）输入
    acmpConfig.hallCOutputCh = ACMP_EXTERNAL_CH2;//HALL C（输出对应PWDT_IN2）输入
    acmpConfig.hallOutEn = ENABLE;//HALL输出使能
    
    ACMP_Init(ACMP0, &acmpConfig);
}

/**
 * ACMP_PollingLogicSample
 *
 * @param[in] void
 * @return void
 *
 * @brief 轮询模式例程
 */
void ACMP_PollingLogicSample(void)
{

    //ACMP初始化为轮询模式
    ACMP_init();


}

/**********<End>*********/
