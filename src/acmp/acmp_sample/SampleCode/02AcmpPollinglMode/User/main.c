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

 I.AC7801x ACMP模块例程（本例程基于CMSIS 1.x.x版本接口开发调试）
 例程2：ACMP轮询模式
 ACMP_IN3(PA8)作为负端输入，ACMP_IN0(PA11),ACMP_IN1(PA10),ACMP_IN2(PA9)作为正端输入。
 通过轮询方式比较ACMP 正负输入。
 
 *****************************************************************************/
/**********<Start>**********/

/**********<Incldue>**********/
#include "acmp_sample.h"
#include "pwdt_sample.h"
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
* @brief 在电机Demo板上通过BLDC开环方式转动电机，通过ACMP轮询模式检测电机的反电动势，通过PWDT捕获ACMP输出的信号。
*/
int main(void)
{
    InitDebug();
    InitDelay();
    mdelay(5000);
    printf("\nRunning the acmp polling mode sample code.\r\n");
    //PWDT模块配置
    PWDT_ThreeChannelInputHallstatus();
    //ACMP模块配置
    ACMP_PollingLogicSample();
    //PWM模块配置，六步开环驱动DEMO小电机
    PWM_SampleBldcControl();
   
}

/**********<End>*********/
