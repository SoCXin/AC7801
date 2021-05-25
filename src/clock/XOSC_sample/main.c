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
* AutoChips Inc. (C) 2020. All rights reserved.
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

#include "ac780x.h" 
#include "ac780x_spm.h"
#include "ac780x_gpio.h"

/* define key gpio */
#define GPIO_KEY1   GPIOC,GPIO_PIN6
#define GPIO_KEY2   GPIOA,GPIO_PIN11

/* define LED gpio */
#define LED1_PIN					GPIOC, GPIO_PIN9
#define LED2_PIN					GPIOC, GPIO_PIN7


#define LED2_ON						do{GPIO_SetPinLevel(LED2_PIN, GPIO_LEVEL_HIGH);}while(0)
#define LED2_OFF					do{GPIO_SetPinLevel(LED2_PIN, GPIO_LEVEL_LOW);}while(0)
#define LED2_TOGGLE					do{if(GPIO_GetPinLevel(LED2_PIN)){LED2_OFF;}else{LED2_ON;}}while(0)
	
#define LED1_ON						do{GPIO_SetPinLevel(LED1_PIN, GPIO_LEVEL_HIGH);}while(0)
#define LED1_OFF					do{GPIO_SetPinLevel(LED1_PIN, GPIO_LEVEL_LOW);}while(0)
#define LED1_TOGGLE					do{if(GPIO_GetPinLevel(LED1_PIN)){LED1_OFF;}else{LED1_ON;}}while(0)
/**
* LED_Init
*
* @param[in]  none
* @return none
*
* @brief  init led control gpio
*
*/
void LED_Init(void)
{
    /* init gpio as output mode */
    GPIO_SetDir(LED1_PIN, GPIO_OUT);
    GPIO_SetDir(LED2_PIN, GPIO_OUT);
}

/**
* KEY_Init
*
* @param[in]  none
* @return none
*
* @brief  init key control gpio
*
*/
void KEY_Init(void)
{
    /* init gpio as input mode */
    GPIO_SetDir(GPIO_KEY1, GPIO_IN);
    GPIO_SetDir(GPIO_KEY2, GPIO_IN);
}

int main(void)
{
    InitDelay();
    InitDebug();
    LED_Init();
    KEY_Init();
    //MODIFY_REG32(CKGEN->RESET_CTRL, CKGEN_RESET_CTRL_XOSC_LOSS_RST_EN_Msk, CKGEN_RESET_CTRL_XOSC_LOSS_RST_EN_Pos, ENABLE);
    MODIFY_REG32(CKGEN->CTRL, CKGEN_CTRL_XOSC_MON_EN_Msk, CKGEN_CTRL_XOSC_MON_EN_Pos, ENABLE);
    while(1)
    {
        LED1_TOGGLE;
        mdelay(500);
    }
}



