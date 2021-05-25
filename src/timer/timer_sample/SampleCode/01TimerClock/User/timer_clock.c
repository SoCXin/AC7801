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
#include "string.h"
#include "ac780x_timer.h"
#include "ac780x_timer_reg.h"
#include "timer_sample.h"


/* 以下四个全局变量为具体计数 */
uint16_t g_timer0Count = 0; ///<timer0 Count 每10ms计数加1


/**
*
* @param[in] timerIndex: timer Index from 0 to 7
*
* @return none
*
* @brief TIMER0 CallBack
* TIMER0中断回调
*/
void TIM0_Task(void *device, uint32_t wpara, uint32_t lpara)
{
	g_timer0Count++;
}


/**
*
* @param[in] none
*
* @return 0:Success
*
* @brief TIMER init
*(0)timer delay frequency = SYS_CLK / (timerloadValue + 1)
*(1)timer0 for 10ms Delay
*(2)timer1,timer2,timer3 used for link mode, for 1 minute delay
* 每个timer都是32位计数器，最大loadValue为4294967295
* 在链接模式中，timer0是不能链接到任何其他timer上面的
*/
void Timer_Init(void)
{
    TIMER_ConfigType  timerConfig0;
	
	memset(&timerConfig0, 0, sizeof(timerConfig0));

    /* configure TIMER channel 0, only enable timer */
    timerConfig0.periodValue      = Delay10ms;  ///10ms timeout 
    timerConfig0.interruptEn      = ENABLE;        //中断使能
    timerConfig0.linkModeEn        = DISABLE;         //禁能链接 
    timerConfig0.timerEn          = ENABLE;        //使能TIMER0
	timerConfig0.callBack = TIM0_Task;
    TIMER_Init(TIMER_CHANNEL0, &timerConfig0);    //TIMER0配置初始化

}

/**
*
* @param[in] none
*
* @return none
*
* @brief 
* TIMER的计数配置及链接
*/
void TIMER_Clock(void)
{
	Timer_Init();
}


 
