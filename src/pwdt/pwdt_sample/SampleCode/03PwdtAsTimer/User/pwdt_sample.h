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
#ifndef PWDT_SAMPLE_H_
#define PWDT_SAMPLE_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "string.h"
#include "ac780x_adc.h"
#include "ac780x_gpio.h"
#include "ac780x.h"
#include "ac780x_debugout.h"
#include "ac780x_dma.h"
#include "ac780x_uart.h"
#include "ac780x_ctu.h"
#include "ac780x_timer.h"
#include "ac780x_pwm.h"
#include "ac780x_acmp.h"
#include "ac780x_pwdt.h"
#include "ac780x_pwdt_reg.h"
#define PRESCALER   				  (PWDT_CLK_PRESCALER_512) 
#define PWDT_DIVIDER     			  (1 << PRESCALER)
   
#define PWDT_CLK                      (APB_BUS_FREQ / PWDT_DIVIDER)

#define Delay5us                      (PWDT_CLK/200000-1)
#define Delay5ms                      (PWDT_CLK/200-1)
#define Delay500ms                    (PWDT_CLK/2-1)
#define Delay1s                       (PWDT_CLK-1)

#define LED_ON                        GPIO_SetPinLevel(GPIOC, GPIO_PIN7, GPIO_LEVEL_HIGH);
#define LED_OFF                       GPIO_SetPinLevel(GPIOC, GPIO_PIN7, GPIO_LEVEL_LOW);
void PwdtAsTimer(void);
void LED_Init(void); 
#ifdef __cplusplus
}
#endif

#endif
