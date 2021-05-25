
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
#include "i2c.h"

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
uint8_t 	g_getKey6Sts;		/*KEY6����״̬*/
uint8_t 	g_getKey7Sts;		/*KEY7����״̬*/

uint8_t		g_scanKeyTime;		/*ɨ�谴�����ʱ��*/


/*************<prototype>**************/

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
	
	/*��ʼ�����Ʊ���.*/
	g_scanKeyTime = 0;
	g_getKey6Sts  = KEY_RELEASE;
	g_getKey7Sts  = KEY_RELEASE;
}

/**
* @prototype GPIO_ScanKeyInput(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 ɨ�谴������.
*/
void GPIO_ScanKeyInput(void)
{
	if (g_scanKeyTime >= SCAN_KEY_INTVL)/*��ʱ���ڵ�,ɨ�谴������.*/
	{
		g_scanKeyTime = 0;
		
		/*�ϴμ���KEY6״̬Ϊ�ͷ��ҵ�ǰKEY6����,���ж���ȷ�ϲ����¼���KEY6״̬.*/
		if ((g_getKey6Sts == KEY_RELEASE) && (GET_KEY6_STS() == KEY_PRESS))
		{
			mdelay(10);
			if (GET_KEY6_STS() == KEY_PRESS)
			{
				g_getKey6Sts = KEY_PRESS;
				
				I2C_Transmit1KBytes();
			}
		}
		/*�ϴμ���KEY6Ϊ�����ҵ�ǰKEY6�ͷ�,���¼���KEY6״̬,�Ա�֤ÿ�ΰ���������Ч��.*/
		else if ((g_getKey6Sts == KEY_PRESS) && (GET_KEY6_STS() == KEY_RELEASE))
		{
			g_getKey6Sts = KEY_RELEASE;
		}
		
		/*�ϴμ���KEY7״̬Ϊ�ͷ��ҵ�ǰKEY7����,���ж���ȷ�ϲ����¼���KEY7״̬.*/
		if ((g_getKey7Sts == KEY_RELEASE) && (GET_KEY7_STS() == KEY_PRESS))
		{
			mdelay(10);
			if (GET_KEY7_STS() == KEY_PRESS)
			{
				g_getKey7Sts = KEY_PRESS;
				
				I2C_Receive1KBytes();
			}
		}
		/*�ϴμ���KEY7Ϊ�����ҵ�ǰKEY7�ͷ�,���¼���KEY7״̬,�Ա�֤ÿ�ΰ���������Ч��.*/
		else if ((g_getKey7Sts == KEY_PRESS) && (GET_KEY7_STS() == KEY_RELEASE))
		{
			g_getKey7Sts = KEY_RELEASE;
		}
	}
}


/*************<end>********************/
