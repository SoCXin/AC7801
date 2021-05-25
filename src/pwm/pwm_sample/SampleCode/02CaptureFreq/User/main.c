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
 /****************************************************************************
 I.AC7801X PWM模块例程（本例程基于CMSIS 1.x.x版本接口开发调试）
 例程1：
    1.PWM1_CH0,PWM1_CH1组合模式，PWM1_CH0输出频率为20K，占空比为25%的波形，PWM1_CH1与PWM1_CH0波形互补，带1us死区插入。
    2.PWM1_CH6,PWM1_CH7独立PWM模式（边沿对齐），生成频率为20K，PWM1_CH6占空比50%，PWM1_CH7占空比25%的波形。
 例程2：PWM1输出PWM,PWM2 CH0捕获频率
    PWM1_CH7输出PWM信号，频率20K，占空比CH7:25%。
    PWM0_CH0通过捕获模式捕获PWM1 CH7输出频率。

 *****************************************************************************/
/**********<Start>**********/

/**********<Incldue>**********/
#include "ac780x.h"
#include "ac780x_debugout.h"
#include "pwm_sample.h"

/**********<Macro>**********/

/**********<Union>**********/

/**********<Struct>*********/

/*********<Variable>********/

/********<Prototype>********/

 /**
*
* @param[in] none
*
* @return none
*
* @brief 
*/
int main(void)
{
    InitDebug();
    InitDelay();
    printf("\nRunning the PWM module sample code.\r\n");
    
    PWM_GpioInit();
    //产生PWM占空比，作为捕获的测试信号。
    PWM1_GenerateFrequency();
    //捕获频率
    PWM_SampleCaptureFreq();
    while (1)
    {
        mdelay(500); 
        printf("\ncapture freq value:%d.\r\n", g_captureFreq);

    
    }
}

/**********<End>*********/
