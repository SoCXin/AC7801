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
#include "ac780x_rtc.h"
#include "ac780x.h"
#include "ac780x_gpio.h" 
#include "rtc_sample.h"

/**
 * RTCCallBackFunc1
 *
 * @param[in]  wparam: reserved
 * @param[in]  lparam: reserved
 * @return 0: success, other: error value
 *
 * @brief  中断回调函数
 * 
 */
void RTCCallBackFunc1(void *device, uint32_t wpara, uint32_t lpara)
{
    return;
}

/**
 * RTC_configuration
 *
 * @param[in]  none
 * @return   none
 *
 * @brief  配置RTC计时5S,时钟源为LSI
 */
void RTC_configuration(void)
{
	uint32_t tmpMod = 99;
    uint32_t tmpPrescalerValue = 1599;             //（1599+1）*（99+1）/32000 = 5s
    RTC_ConfigType RTCConfig;
	
	memset(&RTCConfig,0,sizeof(RTCConfig));

    RTCConfig.clockSource = RTC_CLOCK_LSI;                         //时钟源选择：0:Bus clk (demo板上总线时钟为24M)；
	                                                   //1: LSI 32KHz; 2: External OSC; 3:Internal 8M;
    RTCConfig.periodValue = tmpMod;
	RTCConfig.psrInterruptEn = DISABLE;
    RTCConfig.rtcInterruptEn = ENABLE;
    RTCConfig.psrValue = tmpPrescalerValue;      //time = (pre+1)*(mod+1)/clk
    RTCConfig.rtcOutEn = ENABLE;
    RTCConfig.callBack = RTCCallBackFunc1;
    RTC_Init(&RTCConfig);                        //配置RTC

}


/**
 * RTC_Wakeup
 *
 * @param[in]  none
 * @return   none
 *
 * @brief  配置RTC计时5S,时钟源为LSI，正常模式下LED2打开，进入STOP模式LED1关闭，退出STOP模式后LED2打开
 */
void RTC_Wakeup(void)
{        
	GPIO_SetDir(GPIOC,GPIO_PIN9, GPIO_OUT);//设置LED2引脚为输出
	GPIO_SetDir(GPIOC,GPIO_PIN6,GPIO_IN);//使用按键K6引脚为输入
	GPIO_SetPinLevel(GPIOC,GPIO_PIN9,GPIO_LEVEL_HIGH);  //LED2打开
	mdelay(5000); //delay 5S以防止误操作导致的一上电就进入Stop模式
	while(1)
	{
		if (GPIO_GetPinLevel(GPIOC,GPIO_PIN6) == 0)//判断K6引脚电平,如果为0,说明K6按下
		{
			RTC_configuration();
			GPIO_SetPinLevel(GPIOC,GPIO_PIN9,GPIO_LEVEL_LOW);  //进入STOP模式，LED2关闭
			SysStop();
					
			GPIO_SetPinLevel(GPIOC,GPIO_PIN9,GPIO_LEVEL_HIGH);  //退出STOP模式，LED2打开	
			RTC_DeInit();//关闭RTC
		}
		
	}
	
}





