
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
#ifndef __I2C_SFRWR_H__
#define __I2C_SFRWR_H__

#ifdef __cplusplus
extern "C" {
#endif

/*************<include>****************/
#include "ac780x_gpio.h"
	
/*************<macro>******************/
#define I2C_SCL_PORT		(GPIOB)
#define I2C_SDA_PORT		(GPIOB)
#define I2C_SCL_PIN			(GPIO_PIN7)
#define I2C_SDA_PIN			(GPIO_PIN8)

	
/*SDA输入输出配置.*/	
#define SDA_IN()			GPIO_SetDir(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_IN)
#define SDA_OUT()			GPIO_SetDir(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_OUT)

/*SDA输出高低电平.*/
#define SDA_LOW				GPIO_ResetPinBit(I2C_SDA_PORT, I2C_SDA_PIN)
#define SDA_HIGH			GPIO_SetPinBit  (I2C_SDA_PORT, I2C_SDA_PIN)

/*读取SDA输入电平.*/	
#define SDA_READ()			GPIO_GetPinLevel(I2C_SDA_PORT, I2C_SDA_PIN)

/*SCL输出高低电平.*/
#define SCL_LOW				GPIO_ResetPinBit(I2C_SCL_PORT, I2C_SCL_PIN)
#define SCL_HIGH			GPIO_SetPinBit  (I2C_SCL_PORT, I2C_SCL_PIN)


/*************<enum>*******************/
typedef enum
{
    I2C_WRITE = 0x00U,      /*!< I2C write transfer */
    I2C_READ = 0x01U        /*!< I2C read transfer */
}I2C_WRType;


/*************<union>******************/


/*************<struct>*****************/


/*************<extern>*****************/	


/*************<prototype>**************/
void I2C_GPIO_Init(void);

void I2C_Start(void);
void I2C_Stop(void);
void I2C_SendACK(void);
void I2C_SendNACK(void);
uint8_t I2C_WaitACK(void);

uint8_t I2C_ReceiveOneByte(void);
void I2C_TransmitOneByte(uint8_t dataVal);


#ifdef __cplusplus
}
#endif

#endif /* __I2C_SFRWR_H__ */

/*************<end>********************/
