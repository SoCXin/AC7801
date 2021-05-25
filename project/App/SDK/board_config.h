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

#ifndef BOARD_CONFIG_H_
#define BOARD_CONFIG_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "ac780x.h"
extern uint32_t ADC_Value_Buffer[10];
#define HIGH                        1
#define LOW                         0

#define PD1_GPIO                   1
    
#define CLK                         APB_BUS_FREQ

#define Delay500ms                  (CLK/2 - 1)
#define Delay1ms                    (CLK/1000 - 1)
#define Delayus(n)                  ((CLK/1000000) * n - 1)  //n us
#define DelayHalfus(n)              ((CLK/2000000) * n - 1)  //n 0.5us




//gpio output
#define OUT_PB8                     GPIO_PB8
#define OUT_PB0                     GPIO_PB0
#define OUT_PC14                    GPIO_PC14
#define OUT_PC8                     GPIO_PC8
//#define OUT_PD1                     GPIO_PD1
#define OUT_PC6                     GPIO_PC6
#define OUT_PC2                     GPIO_PC2
#define OUT_PC1                     GPIO_PC1
#define OUT_PB4                     GPIO_PB4
#define OUT_PB7                     GPIO_PB7
#define OUT_PC4                     GPIO_PC4
#define OUT_PC5                     GPIO_PC5

//gpio input
#define IN_PA0                      GPIO_PA0  //GPIO_InputValue_Buffer.bit6
#define IN_PA1                      GPIO_PA1  //GPIO_InputValue_Buffer.bit2
#define IN_PA2                      GPIO_PA2  //GPIO_InputValue_Buffer.bit5
//#define IN_PD1                      GPIO_PD1  //GPIO_InputValue_Buffer.bit1
//pwdt
#define PWDT_IN0                    GPIO_PC11  

//#define PWDT_IN0                    GPIO_PB8  //GPIO_InputValue_Buffer[3]  //for demo board test
//#define PWM0_CH1                    GPIO_PD2  //GPIO_InputValue_Buffer[4]    
#define PWDT_IN1                    GPIO_PB3  //for test
//pwm
#define PWM2_CH1                    GPIO_PC0
#define PWM2_CH0                    GPIO_PB15

#define DEMO_BOARD                  1

#define LED2                        GPIOC, GPIO_PIN9
/*LED2¿ØÖÆ*/	
#define LED2_ON				do{GPIO_SetPinLevel(LED2, GPIO_LEVEL_HIGH);}while(0)
#define LED2_OFF			do{GPIO_SetPinLevel(LED2, GPIO_LEVEL_LOW);}while(0)
#define LED2_TOGGLE			do{if(GPIO_GetPinLevel(LED2)){LED2_OFF;}else{LED2_ON;}}while(0)

//CAN
#define CAN0_TX						GPIOB, GPIO_PIN0
#define CAN0_RX						GPIOB, GPIO_PIN1
#define CAN0_STB					GPIOC, GPIO_PIN8

#define SPI2_MOSI                   GPIO_PB14
#define SPI2_MISO                   GPIO_PB13
#define SPI2_SCK                    GPIO_PB12
#define SPI2_NSS                    GPIO_PB11

#define I2C1_SDA                    GPIO_PC13
#define I2C1_SCL                    GPIO_PC12
#ifdef __cplusplus
}
#endif
#endif /* BOARD_CONFIG_H_ */
