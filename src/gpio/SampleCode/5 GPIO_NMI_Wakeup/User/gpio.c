
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
#include "gpio.h"
#include "timer.h"

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
uint16_t 	g_blinkLedTtlTime;	/*LED闪烁总时间计时*/

/*************<prototype>**************/
void SPM_Callback(void *device, uint32_t wpara, uint32_t lpara);

/**
* @prototype GPIO_InitHw(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 初始化LED引脚.
*/
void GPIO_LedInit(void)
{
	/*初始化引脚功能,如果引脚上电后默认为GPIO,可省略掉初始化步骤.
	  有部分引脚上电默认为非GPIO,则必须选择其功能为GPIO才能作为GPIO使用.*/
	GPIO_SetFunc(LED2_PORT, LED2_PIN, GPIO_FUN0);/*功能复用选择.*/
	GPIO_SetFunc(LED3_PORT, LED3_PIN, GPIO_FUN0);
	
	/*设置LED引脚为GPIO输出.*/
	GPIO_SetDir(LED2_PORT, LED2_PIN, GPIO_OUT);
	GPIO_SetDir(LED3_PORT, LED3_PIN, GPIO_OUT);
	
	/*上电默认LED点亮.*/
	LED2_ON;
	LED3_ON;
	
	/*初始化控制变量.*/
	g_blinkLedTime 	  = 0;
	g_blinkLedTgtTime = BLINK_LED_DFTT;
}

/**
* @prototype GPIO_InitHw(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 初始化按键引脚.
*/
void GPIO_KeyInit(void)
{
	/*初始化引脚功能,如果引脚上电后默认为GPIO,可省略掉初始化步骤.
	  有部分引脚上电默认为非GPIO,则必须选择其功能为GPIO才能作为GPIO使用.*/
	GPIO_SetFunc(KEY6_PORT, KEY6_PIN, GPIO_FUN0);/*功能复用选择.*/
	GPIO_SetFunc(KEY7_PORT, KEY7_PIN, GPIO_FUN0);
	
	/*设置按键引脚为输入,检测按键动作.*/
	GPIO_SetDir(KEY6_PORT, KEY6_PIN, GPIO_IN);
	GPIO_SetDir(KEY7_PORT, KEY7_PIN, GPIO_IN);
	
	/*使能按键引脚的上拉.*/
	GPIO_SetPullup(KEY6_PORT, KEY6_PIN, ENABLE);
	GPIO_SetPullup(KEY7_PORT, KEY7_PIN, ENABLE);
	
	/*初始化控制变量.*/
	g_scanKeyTime = 0;
	g_getKey6Sts  = KEY_RELEASE;
	g_getKey7Sts  = KEY_RELEASE;
}

/**
* @prototype GPIO_NMIInit(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 初始化NMI引脚.
*/
void GPIO_NMIInit(void)
{
	/*选择NMI引脚功能为NMI,而非GPIO.*/
	GPIO_SetFunc(NMI_PORT, NMI_PIN, GPIO_FUN1);
	/*使能NMI唤醒.*/
	SPM_EnableModuleWakeup(SPM_MODULE_NMI, ENABLE);
	/*设置中断回调函数*/
	SPM_SetCallback(SPM_Callback);
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
		
		LED2_TOGGLE;
		LED3_TOGGLE;
	}
}

/**
* @prototype GPIO_MCUGotoSleep(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 查询进入休眠.
*/
void GPIO_MCUGotoSleep(void)
{
	/*当LED闪烁总时间超过一定时间后进入休眠.*/
	if (g_blinkLedTtlTime >= BLINK_LED_TTLT)
	{
		g_blinkLedTtlTime = 0;
		
		/*休眠前关闭LED2/LED3.*/
		LED2_OFF;
		LED3_OFF;
		
		/*进入STOP模式.*/
		SysStop();
		
		/*唤醒后重新初始化定时器,因为定时器在STOP模式下被关闭.*/
		TIMER_PrdInit();
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
	if (g_scanKeyTime >= SCAN_KEY_INTVL)
	{
		g_scanKeyTime = 0;
		
		/*上次记忆KEY6状态为释放且当前KEY6按下,进行二次确认并重新记忆KEY6状态.*/
		if ((g_getKey6Sts == KEY_RELEASE) && (GET_KEY6_STS() == KEY_PRESS))
		{
			mdelay(10);
			if (GET_KEY6_STS() == KEY_PRESS)
			{
				/*按键动作确认,清除LED闪烁总时间.*/
				g_blinkLedTtlTime = 0;
				
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
				/*按键动作确认,清除LED闪烁总时间.*/
				g_blinkLedTtlTime = 0;
				
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

/**
* @prototype SPM_Callback(void *device, uint32_t wpara, uint32_t lpara)
*
* @param[in] ...
* @return	 void
*
* @brief  	 SPM NMI中断回调函数.
*/
void SPM_Callback(void *device, uint32_t wpara, uint32_t lpara)
{
	if (wpara & (1 << SPM_MODULE_NMI))//被NMI唤醒
	{
	}
}

/*************<end>********************/
