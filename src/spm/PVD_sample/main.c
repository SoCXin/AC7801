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
#include "ac780x_debugout.h"
#include "ac780x_gpio.h"
#include "ac780x_spm.h"

#define LED1_PIN					GPIOC, GPIO_PIN9
#define LED2_PIN					GPIOC, GPIO_PIN7


#define LED2_ON						do{GPIO_SetPinLevel(LED2_PIN, GPIO_LEVEL_HIGH);}while(0)
#define LED2_OFF					do{GPIO_SetPinLevel(LED2_PIN, GPIO_LEVEL_LOW);}while(0)
#define LED2_TOGGLE					do{if(GPIO_GetPinLevel(LED2_PIN)){LED2_OFF;}else{LED2_ON;}}while(0)
	
#define LED1_ON						do{GPIO_SetPinLevel(LED1_PIN, GPIO_LEVEL_HIGH);}while(0)
#define LED1_OFF					do{GPIO_SetPinLevel(LED1_PIN, GPIO_LEVEL_LOW);}while(0)
#define LED1_TOGGLE					do{if(GPIO_GetPinLevel(LED1_PIN)){LED1_OFF;}else{LED1_ON;}}while(0)


void PVD_HandlerCallback(void *device, uint32_t wpara, uint32_t lpara);

int main(void)
{
    InitDelay();
    InitDebug();
    
    MODIFY_REG32(SPM->PWR_MGR_CFG1, SPM_PWR_MGR_CFG1_PORLPVD_Msk, SPM_PWR_MGR_CFG1_PORLPVD_Pos, 3); /* set LVD 2.60V  PVD 2.95V */  
    SPM_EnablePVD(ENABLE);  ///<enable PVD
    SPM_EnableLVD(ENABLE);  ///<enable LVD
    MODIFY_REG32(SPM->PWR_MGR_CFG0, SPM_PWR_MGR_CFG0_PWR_EN_Msk, SPM_PWR_MGR_CFG0_PWR_EN_Pos, ENABLE); /* enable spm power */  
    SPM_EnableModuleWakeup(SPM_MODULE_PVD, ENABLE); 

    GPIO_SetDir(LED1_PIN, GPIO_OUT);
    GPIO_SetDir(LED2_PIN, GPIO_OUT);
    LED2_ON; 
    LED1_OFF;
    PVD_SetCallback(PVD_HandlerCallback);   ///<set PVD callback
    NVIC_ClearPendingIRQ(PVD_IRQn);
    NVIC_EnableIRQ(PVD_IRQn);       ///<enable PVD interrupt
    
    while(1)
    {
        
    }
}

void PVD_HandlerCallback(void *device, uint32_t wpara, uint32_t lpara)
{
   // printf("a PVD interrupt happened\r\n");
    LED2_OFF; 
    LED1_ON;
}

