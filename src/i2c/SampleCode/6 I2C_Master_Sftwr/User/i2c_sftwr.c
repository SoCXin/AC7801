
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
#include "i2c_sftwr.h"


/*************<macro>******************/


/*************<enum>*******************/


/*************<union>******************/


/*************<struct>*****************/


/*************<variable>***************/


/*************<prototype>**************/

/**
* @prototype I2C_GPIO_Init(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 初始化LED引脚.
*/
void I2C_GPIO_Init(void)
{
	GPIO_SetFunc(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_FUN0);
	GPIO_SetFunc(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_FUN0);

	GPIO_SetDir(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_OUT);
	GPIO_SetDir(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_OUT);
	
	/*默认SDA和SCL都为高电平.*/
	SCL_HIGH;
	SDA_HIGH;
}

/**
* @prototype I2C_Start(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 SCL为高电平时SDA由高变低.
*/
void I2C_Start(void)
{
	SDA_HIGH;
	udelay(2);
	SCL_HIGH;
	udelay(2);
	SDA_LOW;
	udelay(2);
}

/**
* @prototype I2C_Stop(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 SCL为高电平时SDA由低变高.
*/
void I2C_Stop(void)
{
	SDA_LOW;
	udelay(2);
	SCL_HIGH;
	udelay(2);
	SDA_HIGH;
	udelay(2);
}

/**
* @prototype I2C_SendACK(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 发送ACK应答.
*/
void I2C_SendACK(void)
{
	SCL_LOW;
	udelay(2);
	SDA_LOW;
	udelay(2);
	SCL_HIGH;
	udelay(2);
}

/**
* @prototype I2C_SendNACK(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 发送NACK应答.
*/
void I2C_SendNACK(void)
{
	SCL_LOW;
	udelay(2);
	SDA_HIGH;
	udelay(2);
	SCL_HIGH;
	udelay(2);
}

/**
* @prototype I2C_WaitACK(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 等待ACK应答.
*/
uint8_t I2C_WaitACK(void)
{
	uint8_t ack;
	
	SDA_IN();
	SCL_HIGH;
	udelay(2);
	ack = SDA_READ();
	SDA_OUT();
	SCL_LOW;
	udelay(2);
	SDA_HIGH;
	udelay(2);
	
	return ack;
}

/**
* @prototype I2C_ReceiveOneByte(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 读取一个字节.
*/
uint8_t I2C_ReceiveOneByte(void)
{
	uint8_t ii;
	uint8_t recvByte = 0;
	
	SCL_LOW;
	udelay(2);
	SDA_HIGH;
	udelay(2);
	
	SDA_IN();
	for (ii = 0; ii < 8; ii++)
	{
		SCL_HIGH;
		udelay(2);
		recvByte = ((recvByte << 1) | SDA_READ());
		SCL_LOW;
		udelay(2);
	}
	SDA_OUT();
	udelay(2);
	
	return recvByte;
}
	
/**
* @prototype I2C_TransmitOneByte(uint8_t dataVal)
*
* @param[in] dataVal:
* @return	 void
*
* @brief  	 发送一个字节.
*/
void I2C_TransmitOneByte(uint8_t dataVal)
{
	uint8_t ii;
	
	for (ii = 0; ii < 8; ii++)
	{
		SCL_LOW;
		udelay(2);
		if (dataVal & 0x80)
		{
			SDA_HIGH;
			udelay(2);
		}
		else
		{
			SDA_LOW;
			udelay(2);
		}
		dataVal = (dataVal << 1);
		SCL_HIGH;
		udelay(2);
	}
	
	SCL_LOW;
	udelay(2);
	SDA_HIGH;
	udelay(2);
}
	
/*************<end>********************/
