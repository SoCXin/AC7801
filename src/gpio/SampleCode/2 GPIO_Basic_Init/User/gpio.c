
/* Copyright Statement:
 *
 * This software/firmware and related documentation ("AutoChips Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to AutoChips Inc. and/or its licensors. Without
 * the prior written permission of AutoChips inc. and/or its licensors,  any
 * reproduction,  modification,  use or disclosure of AutoChips Software,  and
 * information contained herein,  in whole or in part,  shall be strictly
 * prohibited.
 *
 * AutoChips Inc. (C) 2018. All rights reserved.
 *
 * BY OPENING THIS FILE,  RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("AUTOCHIPS SOFTWARE")
 * RECEIVED FROM AUTOCHIPS AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. AUTOCHIPS EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES,  EXPRESS OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES AUTOCHIPS PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, 
 * INCORPORATED IN,  OR SUPPLIED WITH THE AUTOCHIPS SOFTWARE,  AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN AUTOCHIPS
 * SOFTWARE. AUTOCHIPS SHALL ALSO NOT BE RESPONSIBLE FOR ANY AUTOCHIPS SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AUTOCHIPS'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE AUTOCHIPS SOFTWARE
 * RELEASED HEREUNDER WILL BE,  AT AUTOCHIPS'S OPTION,  TO REVISE OR REPLACE THE
 * AUTOCHIPS SOFTWARE AT ISSUE,  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO AUTOCHIPS FOR SUCH AUTOCHIPS SOFTWARE AT ISSUE.
 */

/*************<start>******************/


/*************<include>****************/
#include "string.h"
#include "gpio.h"

/*************<macro>******************/
/*定义按键动作,引脚为低电平为释放状态,引脚为高电平为按下状态.*/
#define KEY_PRESS      		(GPIO_LEVEL_LOW)
#define KEY_RELEASE    		(GPIO_LEVEL_HIGH)

#define GET_KEY6_STS()		(GPIO_GetPinLevel(KEY6_PORT, KEY6_PIN))	
#define GET_KEY7_STS()		(GPIO_GetPinLevel(KEY7_PORT, KEY7_PIN))	


/*************<enum>*******************/


/*************<union>******************/


/*************<struct>*****************/


/*************<variable>***************/
uint8_t 	g_getKey6Sts;		/*KEY6按键状态*/
uint8_t 	g_getKey7Sts;		/*KEY7按键状态*/

uint8_t		g_scanKeyTime;		/*扫描按键间隔时间*/
uint16_t	g_blinkLedTime;		/*LED闪烁频率控制时间*/
uint16_t	g_blinkLedTgtTime;	/*LED目标闪烁频率*/


/*************<prototype>**************/

/**
* @prototype GPIO_LedInit(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 初始化LED引脚.
*/
void GPIO_LedInit(void)
{
	GPIO_ConfigType gpioConfig;
	
	/*清零变量.*/
	memset(&gpioConfig, 0x00, sizeof(gpioConfig));
	
	/*初始化引脚功能,如果引脚上电后默认为GPIO,可省略掉初始化步骤.
	  有部分引脚上电默认为非GPIO,则必须选择其功能为GPIO才能作为GPIO使用.*/
	gpioConfig.GPIO_Pin 			= LED2_PIN;
	gpioConfig.GPIO_Mode.GPIO_Fun	= GPIO_FUN0;
	gpioConfig.GPIO_Mode.GPIO_Dir	= GPIO_OUT;
	gpioConfig.GPIO_Mode.GPIO_PuPd	= GPIO_FLOATING;
	gpioConfig.GPIO_Driving			= GPIO_DRIVING_4MA;
	GPIO_Init(LED2_PORT, &gpioConfig);
	gpioConfig.GPIO_Pin 			= LED3_PIN;
	GPIO_Init(LED3_PORT, &gpioConfig);
	
	/*上电默认LED点亮.*/
	LED2_ON;
	LED3_ON;
	
	/*初始化控制变量.*/
	g_blinkLedTime 	  = 0;
	g_blinkLedTgtTime = BLINK_LED_DFTT;
}

/**
* @prototype GPIO_KeyInit(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 初始化按键引脚.
*/
void GPIO_KeyInit(void)
{
	GPIO_ConfigType gpioConfig;
	
	/*清零变量.*/
	memset(&gpioConfig, 0x00, sizeof(gpioConfig));
	
	/*初始化引脚功能,如果引脚上电后默认为GPIO,可省略掉初始化步骤.
	  有部分引脚上电默认为非GPIO,则必须选择其功能为GPIO才能作为GPIO使用.*/
	gpioConfig.GPIO_Pin 			= KEY6_PIN;
	gpioConfig.GPIO_Mode.GPIO_Fun	= GPIO_FUN0;
	gpioConfig.GPIO_Mode.GPIO_Dir	= GPIO_IN;
	gpioConfig.GPIO_Mode.GPIO_PuPd	= GPIO_PU;
	gpioConfig.GPIO_Driving			= GPIO_DRIVING_4MA;
	GPIO_Init(KEY6_PORT, &gpioConfig);
	gpioConfig.GPIO_Pin 			= KEY7_PIN;
	GPIO_Init(KEY7_PORT, &gpioConfig);
	
	/*初始化控制变量.*/
	g_scanKeyTime = 0;
	g_getKey6Sts  = KEY_RELEASE;
	g_getKey7Sts  = KEY_RELEASE;
}

/**
* @prototype GPIO_ToggleLedPrd(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 周期性闪烁LED.
*/
void GPIO_ToggleLedPrd(void)
{
	/*周期性地检查LED闪烁,LED2和LED3同时闪烁.*/
	if (g_blinkLedTime >= g_blinkLedTgtTime)
	{
		g_blinkLedTime = 0;
		
		/*翻转LED3和LED3.*/
		LED2_TOGGLE;
		LED3_TOGGLE;
	}
}

/**
* @prototype GPIO_ScanKeyInput(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 扫描按键输入.
*/
void GPIO_ScanKeyInput(void)
{
	if (g_scanKeyTime >= SCAN_KEY_INTVL)/*定时周期到,扫描按键动作.*/
	{
		g_scanKeyTime = 0;
		
		/*上次记忆KEY6状态为释放且当前KEY6按下,进行二次确认并重新记忆KEY6状态.*/
		if ((g_getKey6Sts == KEY_RELEASE) && (GET_KEY6_STS() == KEY_PRESS))
		{
			mdelay(10);
			if (GET_KEY6_STS() == KEY_PRESS)
			{
				if (g_blinkLedTgtTime < BLINK_LED_MAXT)
				{
					g_blinkLedTgtTime += 20;
					g_getKey6Sts = KEY_PRESS;
					
					printf("LED Blinking IntervalTime: %d ms\r\n", g_blinkLedTgtTime);
				}
			}
		}
		/*上次记忆KEY6为按下且当前KEY6释放,更新记忆KEY6状态,以保证每次按键动作有效性.*/
		else if ((g_getKey6Sts == KEY_PRESS) && (GET_KEY6_STS() == KEY_RELEASE))
		{
			g_getKey6Sts = KEY_RELEASE;
		}
		
		/*上次记忆KEY7状态为释放且当前KEY7按下,进行二次确认并重新记忆KEY7状态.*/
		if ((g_getKey7Sts == KEY_RELEASE) && (GET_KEY7_STS() == KEY_PRESS))
		{
			mdelay(10);
			if (GET_KEY7_STS() == KEY_PRESS)
			{
				if (g_blinkLedTgtTime > BLINK_LED_MINT)
				{
					g_blinkLedTgtTime -= 20;
					g_getKey7Sts = KEY_PRESS;
					
					printf("LED Blinking IntervalTime: %d ms\r\n", g_blinkLedTgtTime);
				}
			}
		}
		/*上次记忆KEY7为按下且当前KEY7释放,更新记忆KEY7状态,以保证每次按键动作有效性.*/
		else if ((g_getKey7Sts == KEY_PRESS) && (GET_KEY7_STS() == KEY_RELEASE))
		{
			g_getKey7Sts = KEY_RELEASE;
		}
	}
}


/*************<end>********************/
