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
/**********<Incldue>***********/
#include "pwm_sample.h"

/**
*
* @param[in] none
*
* @return none
*
* @brief PWM GPIO Init
*
*/
void PWM_GpioInit(void)
{

    //����GPIOΪPWM
    //config PWM1 pinmux
    GPIO_SetFunc(GPIOC, GPIO_PIN9, GPIO_FUN1);//PWM1_CH6
}
/**
* PWM0_CompTask
*
* @param[in] none
*
* @return none
*
* @brief PWM Output Compare CallBack
*/
void PWM0_CompTask(void *device, uint32_t wpara, uint32_t lpara)
{
     //PWM����������ж�
    if (wpara & PWM_INIT_CNTOF_Msk) 
    {
        //user add 
    
    }
    //PWMͨ���ж�
    //ͨ���жϻص����ص�lpara��ӦPWM_STR_CHnSF_Mskλ״̬�ж��Ƿ���ͨ���ж�
    if (lpara & PWM_STR_CHSF_Msk)
    {
        //ͨ��0�ж�
        if (lpara & PWM_STR_CH0SF_Msk)
        {
            
        }
        //ͨ��1�ж�
        if (lpara & PWM_STR_CH1SF_Msk)
        {
            
        }
    }
}

/**
* PWM_SampleOutputCompare
*
* @param[in]  None
* @return     None
*
* @brief PWM0 CH0��Ϊ����Ƚ�ģʽ��
          
*/
void PWM_SampleOutputCompare(void)
{

    PWM_OutputCompareChConfigType outputChConfig[1];//����Ƚ�ͨ�����ýṹ��
    PWM_OutputCompareConfigType outputCompconfig;//����Ƚ����ýṹ��
    PWM_ConfigType config; //PWMģ��ṹ��

    //�ṹ����������
    memset(&outputChConfig, 0, sizeof(outputChConfig));
    memset(&outputCompconfig, 0, sizeof(outputCompconfig));
    memset(&config, 0, sizeof(config));

    outputChConfig[0].channel = PWM_CH_6; //����Ƚ�ͨ��

    outputChConfig[0].comparedValue = (MOD_OUTPUT_COMPARE_PWM>>1); //����Ƚ�ģʽƥ��㣬ƥ����赽�е㡣
    outputChConfig[0].mode = PWM_TOGGLE_OUTPUT;//����ʱ��ֵ��CH0V�Ĵ���ֵƥ��ʱ����תPWM0 CH0���������Ҳ����Ϊ��ֵPWM_CLEAR_OUTPUT��PWM_SET_OUTPUT��
    outputChConfig[0].interruptEn = ENABLE;//�ж�ʹ��
    outputChConfig[0].initLevel = PWM_HIGH_LEVEL;//��ʼ�����ƽ
    outputChConfig[0].triggerEn = DISABLE;//ͨ������ȥ��
    //����Ƚ�����
    outputCompconfig.channelNum = 1;
    outputCompconfig.channelConfig = outputChConfig;
    //PWMģ������
    config.mode = PWM_MODE_OUTPUT_COMPARE;//PWMģ������Ϊ����Ƚ�ģʽ
    config.initModeStruct = &outputCompconfig;//����Ƚ����ñ�����ֵ
    config.clkSource = PWM_CLK_SOURCE_APB; //PWMʱ��Դ����

    config.clkPsc = PWM_OUTPUT_COMPARE_PRESCALER;//PWMʱ��Դ��Ƶ
    config.initValue = 0;//��������ʼ�Ĵ���ֵ
    /* 
        ��ֵMODֵ����Ӱ��Ƚ��¼�������Ƶ�ʡ�����Ƚ�ģʽƵ�� = PWMģ��Ƶ�� / ��MODֵ + 1�� 
    */
    config.maxValue = MOD_OUTPUT_COMPARE_PWM; //PWM���������ֵ
    config.overflowInterrupEn = DISABLE;//����������ж�ʹ��λ
    config.cntOverflowFreq = 0;//CNTOF�жϲ�����Ƶ���������Ƶ�ʵĹ�ϵ(0-127), 0��ʾÿ�μ������������������жϣ�1��ʾ���1�Σ�2��ʾ���2�Σ��Դ����ơ�
    
    config.interruptEn = ENABLE; //PWM�ж�ʹ��
    config.callBack = PWM0_CompTask; //PWM�жϻص�

    PWM_Init(PWM1, &config); //���ó�ʼ����Ч

    NVIC_SetPriority(PWM1_IRQn, 0); //����PWMģ���жϵ����ȼ�
}

/**********<End>*********/
