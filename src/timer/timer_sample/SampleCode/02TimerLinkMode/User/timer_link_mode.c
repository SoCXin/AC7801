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

#define MAX_MIN_SECOND_COUNT        99
#define MAX_SECOND_MINUTE_COUNT     59


/* 以下四个全局变量为具体计数 */
uint16_t g_timer3Count = 0; ///<timer3 Count 每1分钟计数加1


/**
*
* @param[in] timerIndex: timer Index from 0 to 7
*
* @return none
*
* @brief TIMER4 CallBack
* TIMER3中断回调
*/
void TIM3_Task(void *device, uint32_t wpara, uint32_t lpara)
{
    g_timer3Count++;
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
    TIMER_ConfigType  timerConfig1;
    TIMER_ConfigType  timerConfig2;
    TIMER_ConfigType  timerConfig3;

	memset(&timerConfig1, 0, sizeof(timerConfig1));
	memset(&timerConfig2, 0, sizeof(timerConfig2));
	memset(&timerConfig3, 0, sizeof(timerConfig3));
	
    /*Config TIMER1.TIMER2.TIMER3 Chain Mode*/

    /* configure TIMER channel 1, only enable timer */
    timerConfig1.periodValue      = Delay1ms;    ////1ms TimerOut
    timerConfig1.interruptEn      = DISABLE;         //禁能中断
    timerConfig1.linkModeEn        = DISABLE;          //禁能链接
    timerConfig1.timerEn          = ENABLE;
	timerConfig1.callBack = NULL;

    /* configure TIMER channel 2 in chain mode, Only Enable timer */
    timerConfig2.periodValue      = 999;    ////1s TimerOut
    timerConfig2.interruptEn      = DISABLE;
    timerConfig2.linkModeEn        = ENABLE;     //使能链接
    timerConfig2.timerEn          = ENABLE;
	timerConfig2.callBack = NULL;

    /* configure TIMER channel 3 in chain mode, Enable Interrupt and timer */
    timerConfig3.periodValue      = 59;    ////60s TimerOut
    timerConfig3.interruptEn      = ENABLE;    //使能中断
    timerConfig3.linkModeEn        = ENABLE;     //使能链接
    timerConfig3.timerEn          = ENABLE;
	timerConfig3.callBack         = TIM3_Task;

    TIMER_Init(TIMER_CHANNEL1, &timerConfig1);
    TIMER_Init(TIMER_CHANNEL2, &timerConfig2);
    TIMER_Init(TIMER_CHANNEL3, &timerConfig3);       

}

/**
*
* @param[in] none
*
* @return none
*
* @brief 
* TIMER的链接模式计时
*/
void Timer_Link_Mode(void)
{
	Timer_Init();
}


 
