
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

/*************<macro>******************/
/*���尴������,����Ϊ�͵�ƽΪ�ͷ�״̬,����Ϊ�ߵ�ƽΪ����״̬.*/
#define KEY_PRESS      		(GPIO_LEVEL_LOW)
#define KEY_RELEASE    		(GPIO_LEVEL_HIGH)

#define GET_KEY6_STS()		(GPIO_GetPinLevel(KEY6_PORT, KEY6_PIN))	
#define GET_KEY7_STS()		(GPIO_GetPinLevel(KEY7_PORT, KEY7_PIN))	


/*************<enum>*******************/


/*************<union>******************/


/*************<struct>*****************/


/*************<variable>***************/
uint8_t		g_scanKeyTime;		/*ɨ�谴�����ʱ��*/
uint16_t	g_blinkLedTime;		/*LED��˸Ƶ�ʿ���ʱ��*/
uint16_t	g_blinkLedTgtTime;	/*LEDĿ����˸Ƶ��*/


/*************<prototype>**************/
void EXTI_Key6_Callback(void *device, uint32_t wpara, uint32_t lpara);
void EXTI_Key7_Callback(void *device, uint32_t wpara, uint32_t lpara);


/**
* @prototype GPIO_LedInit(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 ��ʼ��LED����.
*/
void GPIO_LedInit(void)
{
	/*��ʼ�����Ź���,��������ϵ��Ĭ��ΪGPIO,��ʡ�Ե���ʼ������.
	  �в��������ϵ�Ĭ��Ϊ��GPIO,�����ѡ���书��ΪGPIO������ΪGPIOʹ��.*/
	GPIO_SetFunc(LED2_PORT, LED2_PIN, GPIO_FUN0);/*���ܸ���ѡ��.*/
	GPIO_SetFunc(LED3_PORT, LED3_PIN, GPIO_FUN0);
	
	/*����LED����ΪGPIO���.*/
	GPIO_SetDir(LED2_PORT, LED2_PIN, GPIO_OUT);
	GPIO_SetDir(LED3_PORT, LED3_PIN, GPIO_OUT);
	
	/*�ϵ�Ĭ��LED����.*/
	LED2_ON;
	LED3_ON;
	
	/*��ʼ�����Ʊ���.*/
	g_blinkLedTime 	  = 0;
	g_blinkLedTgtTime = BLINK_LED_DFTT;
}

/**
* @prototype GPIO_KeyInit(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 ��ʼ����������.
*/
void GPIO_KeyInit(void)
{
	/*��ʼ�����Ź���,��������ϵ��Ĭ��ΪGPIO,��ʡ�Ե���ʼ������.
	  �в��������ϵ�Ĭ��Ϊ��GPIO,�����ѡ���书��ΪGPIO������ΪGPIOʹ��.*/
	GPIO_SetFunc(KEY6_PORT, KEY6_PIN, GPIO_FUN0);/*���ܸ���ѡ��.*/
	GPIO_SetFunc(KEY7_PORT, KEY7_PIN, GPIO_FUN0);
	
	/*���ð�������Ϊ����,��ⰴ������.*/
	GPIO_SetDir(KEY6_PORT, KEY6_PIN, GPIO_IN);
	GPIO_SetDir(KEY7_PORT, KEY7_PIN, GPIO_IN);
	
	/*ʹ�ܰ������ŵ�����.*/
	GPIO_SetPullup(KEY6_PORT, KEY6_PIN, ENABLE);
	GPIO_SetPullup(KEY7_PORT, KEY7_PIN, ENABLE);
	
	/*ʹ��KEY6,KEY7���ⲿ�ж�,����Ϊ�½��ز����ж�,�����°���ʱ�����ж�.*/
	GPIO_EnableExtInterrupt(KEY6_PORT, KEY6_PIN, EXTI_TRIGGER_FALLING);
	GPIO_EnableExtInterrupt(KEY7_PORT, KEY7_PIN, EXTI_TRIGGER_FALLING);
	
	/*�����ⲿ�ж�,�жϻص�����.*/
	GPIO_SetCallback(KEY6_PIN, EXTI_Key6_Callback);
	GPIO_SetCallback(KEY7_PIN, EXTI_Key7_Callback);
	
	/*��ʼ�����Ʊ���.*/
	g_scanKeyTime = 0;
}

/**
* @prototype GPIO_ToggleLedPrd(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 ��������˸LED.
*/
void GPIO_ToggleLedPrd(void)
{
	/*�����Եؼ��LED��˸,LED2��LED3ͬʱ��˸.*/
	if (g_blinkLedTime >= g_blinkLedTgtTime)
	{
		g_blinkLedTime = 0;
		
		/*��תLED3��LED3.*/
		LED2_TOGGLE;
		LED3_TOGGLE;
	}
}

/**
* @prototype EXTI_Key6_Callback(void *device, uint32_t wpara, uint32_t lpara)
*
* @param[in] ...
* @return	 void
*
* @brief  	 KEY6�ⲿ�жϻص�����.
*/
void EXTI_Key6_Callback(void *device, uint32_t wpara, uint32_t lpara)
{
	if (GET_KEY6_STS() == KEY_PRESS)
	{
		mdelay(10);
		if (GET_KEY6_STS() == KEY_PRESS)
		{
			if (g_blinkLedTgtTime < BLINK_LED_MAXT)
			{
				g_blinkLedTgtTime += 20;
				
				printf("LED Blinking IntervalTime: %d ms\r\n", g_blinkLedTgtTime);
			}
		}
	}
}

/**
* @prototype EXTI_Key7_Callback(void *device, uint32_t wpara, uint32_t lpara)
*
* @param[in] ...
* @return	 void
*
* @brief  	 KEY7�ⲿ�жϻص�����.
*/
void EXTI_Key7_Callback(void *device, uint32_t wpara, uint32_t lpara)
{
	if (GET_KEY7_STS() == KEY_PRESS)
	{
		mdelay(10);
		if (GET_KEY7_STS() == KEY_PRESS)
		{
			if (g_blinkLedTgtTime > BLINK_LED_MINT)
			{
				g_blinkLedTgtTime -= 20;
				
				printf("LED Blinking IntervalTime: %d ms\r\n", g_blinkLedTgtTime);
			}
		}
	}
}

/*************<end>********************/
