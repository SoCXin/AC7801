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
 /****************************************************************************
 I.AC7801X PWMģ�����̣������̻���CMSIS 1.x.x�汾�ӿڿ������ԣ�
 ����1��
    1.PWM1_CH0,PWM1_CH1���ģʽ��PWM1_CH0���Ƶ��Ϊ20K��ռ�ձ�Ϊ25%�Ĳ��Σ�PWM1_CH1��PWM1_CH0���λ�������1us�������롣
    2.PWM1_CH6,PWM1_CH7����PWMģʽ�����ض��룩������Ƶ��Ϊ20K��PWM1_CH6ռ�ձ�50%��PWM1_CH7ռ�ձ�25%�Ĳ��Ρ�
 ����2��PWM1���PWM,PWM2 CH0����Ƶ��
    PWM1_CH7���PWM�źţ�Ƶ��20K��ռ�ձ�CH7:25%��
    PWM0_CH0ͨ������ģʽ����PWM1 CH7���Ƶ�ʡ�

 *****************************************************************************/
/**********<Start>**********/

/**********<Incldue>**********/
#include "ac780x.h"
#include "ac780x_debugout.h"
#include "pwm_sample.h"

/**********<Macro>**********/

/**********<Union>**********/

/**********<Struct>*********/

/*********<Variable>********/

/********<Prototype>********/

 /**
*
* @param[in] none
*
* @return none
*
* @brief 
*/
int main(void)
{
    InitDebug();
    InitDelay();
    printf("\nRunning the PWM module sample code.\r\n");
    
    PWM_GpioInit();
    //����PWMռ�ձȣ���Ϊ����Ĳ����źš�
    PWM1_GenerateFrequency();
    //����Ƶ��
    PWM_SampleCaptureFreq();
    while (1)
    {
        mdelay(500); 
        printf("\ncapture freq value:%d.\r\n", g_captureFreq);

    
    }
}

/**********<End>*********/
