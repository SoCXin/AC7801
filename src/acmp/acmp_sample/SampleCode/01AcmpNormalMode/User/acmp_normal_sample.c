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
#include "acmp_sample.h"
/*********<Variable>********/
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
void ACMP_init()
{
    ACMP_ConfigType acmpConfig;
    //配置PINMUX
    GPIO_SetFunc(GPIOA, GPIO_PIN9, GPIO_FUN2);  ///< ACMP_IN2
    
    memset(&acmpConfig, 0x00, sizeof(acmpConfig));
    /* General parameters */
    acmpConfig.positivePin = ACMP_EXTERNAL_CH2;  //正输入通道选择
    acmpConfig.negativePin = ACMP_DAC_OUTPUT_CH7; //负输入通道选择
    acmpConfig.hysterisisVol = ACMP_HYSTERISIS_40MV; //迟滞档位选择
    acmpConfig.edgeType = ACMP_EDGE_TRIGGER_FALLING_RISING;//中断触发模式
    acmpConfig.acmpOutEn = ENABLE;//ACMP输出使能
    acmpConfig.interruptEn = ENABLE;//ACMP中断使能
    acmpConfig.callBack = ACMP_CallBack; //中断回调
    acmpConfig.acmpEn = ENABLE;//ACMP使能
    /* Dac parameters */
    acmpConfig.dacRef = ACMP_DAC_VDD;  //DAC参考源选择
    acmpConfig.dacValue = 32;//dac范围0-63 输出电压值：(dacValue/64)*dacRef
    acmpConfig.dacEn = ENABLE;//DAC输出使能
    /* Polling mode parameters */
    //NOT USE

    ACMP_Init(ACMP0, &acmpConfig);

}

/**
 * ACMP_NormalSample
 *
 * @param[in] void
 * @return void
 *
 * @brief ACMP普通模式例程
 */
void ACMP_NormalSample(void)
{
    ACMP_init();
    
    while(1)
    {
        //nothing todo 
    }
}

/**********<End>*********/
