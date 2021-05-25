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
#include "ac780x_spm.h"
#include "wdg_sample.h"

/**
 * WDG_5s_Timerout
 *
 * @param[in]  none
 * @return    none
 *
 * @brief  配置看门狗超时时间为5S（停止模式下该时间为2倍，即10S）
 */
void WDG_5s_StopMode(void)
{
	uint32_t tmpTimeoutValue = 159999;            // （159999+1）/32000 = 5s(时钟为32K)
    WDG_ConfigType WDGConfig;
	
	memset(&WDGConfig,0,sizeof(WDGConfig));
	
	WDGConfig.clockSource = WDG_CLOCK_LSI;                //
    WDGConfig.WDGEn = ENABLE;
    WDGConfig.interruptEn = DISABLE;           //使能中断，延迟128个总线时钟后复位，可在128总线时钟内执行中断例程
	                                                 //若需要超时立即复位，禁能中断即可
    WDGConfig.prescalerEn = DISABLE;          //0:disable 256 pre, 1:enable 256 pre
    WDGConfig.updateEn = ENABLE;        	//if update is 0,can't do any setting
    WDGConfig.windowEn = DISABLE;          //window mode watchdog
    WDGConfig.timeoutValue = tmpTimeoutValue;            //time = pre*timeout/clk source
    WDGConfig.windowValue = NULL;
    WDGConfig.callBack = NULL;              //设置中断回调
    WDG_Init(&WDGConfig);

    GPIO_SetDir(GPIOC,GPIO_PIN9,GPIO_OUT);        //LED1,用于测试看门狗功能
	GPIO_SetDir(GPIOC, GPIO_PIN6,GPIO_IN);           //使用按键K6引脚为输入
	GPIO_SetPinLevel(GPIOC,GPIO_PIN9,GPIO_LEVEL_HIGH);
	
	WDG_Feed();	
	
	while (1)
    {
		if (GPIO_GetPinLevel(GPIOC,GPIO_PIN6) == 0)//判断K6引脚电平,如果为0,说明K6按下
		{
			GPIO_SetPinLevel(GPIOC,GPIO_PIN9,GPIO_LEVEL_LOW);
			SysStop(); //停止模式，进入停止模式后，看门狗的复位时间为正常模式的两倍，即正常模式中5s复位，则停止模式中10s复位			
		}
		
        WDG_Feed();                                  //刷新看门狗				
    }
}









