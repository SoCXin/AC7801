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
#include "pwdt_sample.h"
/*********<Variable>********/
uint8_t g_ledFlag = 0;
void LED_Init(void)
{
    //LED2 
    GPIO_SetFunc(GPIOC, GPIO_PIN7, GPIO_FUN0);  ///PC7 GPIO
    GPIO_SetDir(GPIOC, GPIO_PIN7, GPIO_OUT); //PC7 OUTPUT
}

/**
* PWDT_Timer
*
* @param[in] none
*
* @return none
*
* @brief PWDT Pulse Width Detection CallBack
*/
void PWDT_Timer(void *device, uint32_t wpara, uint32_t lpara)
{
    //PWDT溢出中断
    if (wpara & PWDT_INIT0_OVF_Msk)
    {
        if (g_ledFlag)
        {
            LED_ON;
            g_ledFlag = 0;
        }
        else
        {
            LED_OFF;
            g_ledFlag = 1;
        }
    }
    //脉宽有效中断
    if (wpara & PWDT_INIT0_RDYF_Msk)
    {
        
    }
}


/**
* PWDT_Initalize
*
* @param[in] none
*
* @return 0:Success
*
* @brief PWDT模块初始化
*/
void PWDT_Initalize(void)
{
    PWDT_ConfigType  pwdtConfig;
    memset(&pwdtConfig, 0x00, sizeof(pwdtConfig));
    /* General parameters */
    pwdtConfig.clkPsc = PRESCALER;  //PWDT时钟源分频
    pwdtConfig.overflowInterruptEn = ENABLE; //使能溢出中断
    pwdtConfig.readyInterruptEn = DISABLE;  //脉宽有效中断去能
    pwdtConfig.interruptEn = ENABLE;       //使能PWDT模块中断
    pwdtConfig.callBack = PWDT_Timer;    //设置PWDT中断处理回调函数
    pwdtConfig.cmpModeEn = DISABLE;         //比较器输入去能
    pwdtConfig.hallModeEn = DISABLE;        //HALL模式去能
    pwdtConfig.mode = PWDT_TIMER_MODE;    //定时器模式

    /* Timer parameters */
    pwdtConfig.periodValue = Delay1s;
    PWDT_Init(PWDT0, &pwdtConfig);//设置PWDT
}

/**
 * PwdtAsTimer
 *
 * @param[in] void
 * @return void
 *
 * @brief PWDT作为普通定时器功能
 */
void PwdtAsTimer(void)
{
    //PWDT模块初始化
    PWDT_Initalize();
    while(1)
    {
        mdelay(500);
    }
}

/**********<End>*********/
