
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
 * AutoChips Inc. (C) 2018. All rights reserved.
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

/*************<start>******************/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/*************<include>****************/
#include "ac780x_gpio.h"
	
/*************<macro>******************/
#define LED2_PORT			(GPIOC)
#define LED2_PIN			(GPIO_PIN9)

#define LED3_PORT			(GPIOC)
#define LED3_PIN			(GPIO_PIN7)

/*LED2动作定义.*/	
#define LED2_ON				do{GPIO_SetPinLevel(LED2_PORT, LED2_PIN, GPIO_LEVEL_HIGH);}while(0)
#define LED2_OFF			do{GPIO_SetPinLevel(LED2_PORT, LED2_PIN, GPIO_LEVEL_LOW);}while(0)
#define LED2_TOGGLE			do{if(GPIO_GetPinLevel(LED2_PORT, LED2_PIN)){LED2_OFF;}else{LED2_ON;}}while(0)

/*LED3动作定义.*/
#define LED3_ON				do{GPIO_SetPinLevel(LED3_PORT, LED3_PIN, GPIO_LEVEL_HIGH);}while(0)
#define LED3_OFF			do{GPIO_SetPinLevel(LED3_PORT, LED3_PIN, GPIO_LEVEL_LOW);}while(0)
#define LED3_TOGGLE			do{if(GPIO_GetPinLevel(LED3_PORT, LED3_PIN)){LED3_OFF;}else{LED3_ON;}}while(0)

#define KEY6_PORT			(GPIOC)
#define KEY6_PIN			(GPIO_PIN6)

#define KEY7_PORT			(GPIOA)
#define KEY7_PIN			(GPIO_PIN11)

#define SCAN_KEY_INTVL		(10)	/*扫描按键时间间隔*/
#define BLINK_LED_MINT		(50)	/*闪烁LED灯最小时间间隔*/
#define BLINK_LED_MAXT		(500)	/*闪烁LED灯最大时间间隔*/
#define BLINK_LED_DFTT		(100)	/*闪烁LED默认时间间隔*/


/*************<enum>*******************/


/*************<union>******************/


/*************<struct>*****************/


/*************<extern>*****************/	
extern uint8_t		g_scanKeyTime;		/*扫描按键间隔时间*/
extern uint16_t		g_blinkLedTime;		/*LED闪烁频率控制时间*/
extern uint16_t		g_blinkLedTgtTime;	/*LED目标闪烁频率*/


/*************<prototype>**************/
void GPIO_LedInit(void);
void GPIO_KeyInit(void);

void GPIO_ToggleLedPrd(void);
void GPIO_ScanKeyInput(void);


#ifdef __cplusplus
}
#endif

#endif /* __GPIO_H__ */

/*************<end>********************/
