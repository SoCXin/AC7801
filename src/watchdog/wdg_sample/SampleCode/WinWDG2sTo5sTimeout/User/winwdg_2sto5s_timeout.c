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
#include "ac780x.h"
#include "ac780x_wdg.h"
#include "ac780x_wdg_reg.h"
#include "ac780x_gpio.h"
#include "wdg_sample.h"

/**
 * WINWDG_2sTo5s_Timerout
 *
 * @param[in]  none
 * @return    none
 *
 * @brief  配置窗口看门狗窗口时间为2~5S，只能在这个时间段喂狗，否则引起复位
 */
void WINWDG_2sTo5s_Timerout(void)
{
	uint32_t tmpTimeoutValue = WDG_CLK(5);            // （119999999 + 1）/24000000 = 5s(时钟为24M)（最晚喂狗时间为5S）
    WDG_ConfigType WDGConfig;
	
	memset(&WDGConfig,0,sizeof(WDGConfig));
	
	WDGConfig.clockSource = WDG_CLOCK_APB;                //0 is 24MHz clk source
    WDGConfig.WDGEn = ENABLE;
    WDGConfig.interruptEn = DISABLE;           //禁能中断，未在窗口时间内喂狗立即复位	                                                 
    WDGConfig.prescalerEn = DISABLE;          //0:disable 256 pre, 1:enable 256 pre
    WDGConfig.updateEn = ENABLE;        //if update is 0,can't do any setting
    WDGConfig.windowEn = ENABLE;           //使能看门狗窗口模式
    WDGConfig.timeoutValue = tmpTimeoutValue;            //time = pre*timeout/clk source
    WDGConfig.windowValue = WDG_CLK(2);                //// （48000000 + 1）/24000000 = 2s(最早喂狗时间为2S)
    
    WDG_Init(&WDGConfig);
	
	GPIO_SetDir(GPIOC,GPIO_PIN9,GPIO_OUT);        //LED1,用于测试看门狗功能
	GPIO_SetPinLevel(GPIOC,GPIO_PIN9,GPIO_LEVEL_HIGH);
	
	//配置好窗口看门狗后，不能立即喂狗，否则会立即重启（重启后因为喂狗又会一直重启）。进入不到下面的While循环

	while (1)
    {
		mdelay(4000);      //延时4S再喂狗，不会重启，若为验证看门狗有效性，可改时间小于2S或大于5S
        WDG_Feed();                                  //feed dog		
		GPIO_SetPinLevel(GPIOC,GPIO_PIN9,GPIO_LEVEL_LOW);
    }
}

