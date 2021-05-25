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

#define WDG_Timeout(n)   (n * APB_BUS_FREQ)

/**
 * WDGCallback
 *
 * @param[in]  wparam: reserved
 * @param[in]  lparam: reserved
 * @return 0: success, other: error
 *
 * @brief Watchdog IRQ callback function
 * �жϷ������128������ʱ�ӣ�ʱ�伫�̣���Ȼ��MCU�ᱻ��λ
 */
void WDGCallback(void *device, uint32_t wpara, uint32_t lpara)
{
    
	return;
}

/**
 * WDG_5s_Timerout
 *
 * @param[in]  none
 * @return    none
 *
 * @brief  ���ÿ��Ź���ʱʱ��Ϊ5S
 */
void WDG_5s_Timerout(void)
{
	uint32_t tmpTimeoutValue = WDG_CLK(5);            // ��119999999+1��/24000000 = 5s(ʱ��Ϊ48M)
    WDG_ConfigType WDGConfig;
	
	memset(&WDGConfig,0,sizeof(WDGConfig));
	
	WDGConfig.clockSource = WDG_CLOCK_APB;                //0 is 24MHz clk source
    WDGConfig.WDGEn = ENABLE;
    WDGConfig.interruptEn = ENABLE;           //ʹ���жϣ��ӳ�128������ʱ�Ӻ�λ������128����ʱ����ִ���ж�����
	                                                 //����Ҫ��ʱ������λ�������жϼ���
    WDGConfig.prescalerEn = DISABLE;          //0:disable 256 pre, 1:enable 256 pre
    WDGConfig.updateEn = ENABLE;       		 //if update is 0,can't do any setting
    WDGConfig.windowEn = DISABLE;          //window mode watchdog
    WDGConfig.timeoutValue = tmpTimeoutValue;            //time = pre*timeout/clk source
    WDGConfig.windowValue = NULL;
    WDGConfig.callBack = WDGCallback;              //�����жϻص�
    WDG_Init(&WDGConfig);

    GPIO_SetDir(GPIOC,GPIO_PIN9,GPIO_OUT);        //LED1,���ڲ��Կ��Ź�����
	GPIO_SetPinLevel(GPIOC,GPIO_PIN9,GPIO_LEVEL_HIGH);
	
	WDG_Feed();	
	while (1)
    {
		mdelay(4000);      //��ʱ4S��ι����������������Ϊ��֤���Ź���Ч�ԣ��ɰ�ʱ���Ϊ6S
        WDG_Feed();                                  //ˢ�¿��Ź�		
		GPIO_SetPinLevel(GPIOC,GPIO_PIN9,GPIO_LEVEL_LOW);
    }
}



