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
#include "ac780x.h"
#include "ac780x_gpio.h"
#include "ac780x_mmdivsqrt.h"
#include "ac780x_mmdivsqrt_reg.h"
#include "math.h"

int32_t sign_quotientResult = 0;      //�з�����
int32_t sign_remainderResult = 0;     //�з�������
uint32_t unsign_quotientResult = 0;    //�޷�����
uint32_t unsign_remainderResult = 0;   //�޷�������

int32_t sign_dend = 9;  //������
int32_t sing_dsor = -4;  //����
uint8_t dsft1 = 1;    //������������λ��
uint32_t unsign_dend = 600;
uint32_t unsign_dsor = 500;
uint8_t dsft2 = 3;

int32_t sign_dend_complex = 88777;  //������
int32_t sing_dsor_complex = -7;  //����
uint8_t dsft1_complex = 10;    //������������λ��
uint32_t unsign_dend_complex = 8777;
uint32_t unsign_dsor_complex = 8;
uint8_t dsft2_complex = 10;

uint32_t test_result;

/**
*
* @param[in] none
*
* @return none
*
* @brief 
* Ӳ���������㣨ע����������ֵ���ܴ���32λ������������Ϊ�������ļĴ���ֻ��32λ��
*/
void div_calculate(void)
{	
	GPIO_SetDir(GPIOA,GPIO_PIN3,GPIO_OUT);	
	
	/* �з��ų��� */
	MMDIVSQRT_CalSignDivWithDsft(sign_dend, sing_dsor, dsft1, &sign_quotientResult); //ȡ��
		
	MMDIVSQRT_CalSignDivWithDsft_Remainder(sign_dend, sing_dsor, dsft1, &sign_remainderResult); //ȡ�࣬��������ȡ���ڱ�����

	/* �޷��ų��� */
	MMDIVSQRT_CalUnsignDivWithDsft(unsign_dend, unsign_dsor, dsft2, &unsign_quotientResult);//ȡ��
	
	MMDIVSQRT_CalUnsignDivWithDsft_Remainder(unsign_dend, unsign_dsor, dsft2, &unsign_remainderResult);//ȡ�࣬��������ȡ���ڱ�����
	
	test_result = (sign_dend<<dsft1)%sing_dsor;
	while(1)
	{
		//��IO�����䣬�ɼ�����ʱ��
		GPIOA->ODR |= 0x08;
		MMDIVSQRT_CalSignDivWithDsft(sign_dend, sing_dsor, dsft1, &sign_quotientResult); 	//1.7us	
		//MMDIVSQRT_CalSignDivWithDsft_Remainder(sign_dend, sing_dsor, dsft1, &sign_remainderResult);	//1.7us	
		//MMDIVSQRT_CalUnsignDivWithDsft(unsign_dend, unsign_dsor, dsft2, &unsign_quotientResult);	//1.7us	
		//MMDIVSQRT_CalUnsignDivWithDsft_Remainder(unsign_dend, unsign_dsor, dsft2, &unsign_remainderResult);	//1.8us	
		
		//sign_quotientResult = (sign_dend<<dsft1)/sing_dsor; //2.1us
		//sign_remainderResult = (sign_dend<<dsft1)%sing_dsor;  //2.1us
		//unsign_quotientResult = (unsign_dend<<dsft2)/unsign_dsor; //1.5us
		//unsign_remainderResult = (unsign_dend<<dsft2)%unsign_dsor; //1.5us
		
		/*Э��������Ӳ����������ʱ����Թ̶��������������ʱ��ȡ�������ݸ��Ӷ�(��������С��������С����λ����С����Ӱ��)*/
		//MMDIVSQRT_CalSignDivWithDsft(sign_dend_complex, sing_dsor_complex, dsft1_complex, &sign_quotientResult); //1.96us
		//MMDIVSQRT_CalSignDivWithDsft_Remainder(sign_dend_complex, sing_dsor_complex, dsft1_complex, &sign_remainderResult);	//1.96us
		//MMDIVSQRT_CalUnsignDivWithDsft(unsign_dend_complex, unsign_dsor_complex, dsft2_complex, &unsign_quotientResult);	//1.85us
		//MMDIVSQRT_CalUnsignDivWithDsft_Remainder(unsign_dend_complex, unsign_dsor_complex, dsft2_complex, &unsign_remainderResult);	//1.82us
		
		//sign_quotientResult = (sign_dend_complex<<dsft1_complex)/sing_dsor_complex; //5.7us
		//sign_remainderResult = (sign_dend_complex<<dsft1_complex)%sing_dsor_complex;  //5.7us
		//unsign_quotientResult = (unsign_dend_complex<<dsft2_complex)/unsign_dsor_complex; //5.4us
		//unsign_remainderResult = (unsign_dend_complex<<dsft2_complex)%unsign_dsor_complex;  //5.4us 
				
		GPIOA->ODR &= 0xF7;
	}
}
