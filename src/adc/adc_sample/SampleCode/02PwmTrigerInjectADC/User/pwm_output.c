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
/**********<Incldue>**********/
#include "ac780x_gpio.h"
#include "ac780x.h"
#include "ac780x_timer.h"
#include "ac780x_pwm.h"
#include "pwm_sample.h"
/**********<Prototype>*********/

/**********<Variable>*********/
uint32_t g_positvePulse;///<pulse width positive value
uint32_t g_negativePulse;///<pulse width negative value
uint32_t g_freq;///<pulse width period value
float g_duty;///<pulse duty value
uint32_t g_hallStatus;///<hall status value
uint32_t g_overFlowCount;//PWM1��������ж�
/**
* PWM1_CallBack
*
* @param[in] device: device pointer
* @param[in] wpara: callback parameter
* @param[in] lpara: callback parameter
* @return none
* @brief PWM CallBack
*/
void PWM1_CallBack(void *device, uint32_t wpara, uint32_t lpara)
{
    //PWM����������ж�
    if (wpara & PWM_INIT_CNTOF_Msk) 
    {
        g_overFlowCount ++;
        //user add 
    
    }
    //PWMͨ���ж�
    //ͨ���жϻص����ص�lpara��ӦPWM_STR_CHnSF_Mskλ״̬�ж��Ƿ���ͨ���ж�
    if (lpara & PWM_STR_CHSF_Msk)
    {
        
        //user add
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
    GPIO_SetFunc(GPIOB, GPIO_PIN5, GPIO_FUN1);//PWM1_CH0 
    GPIO_SetFunc(GPIOB, GPIO_PIN4, GPIO_FUN1);//PWM1_CH1
}
/**
* PWM1_GenerateFrequency
* 
* @param[in] none
* @return none
*
* @brief 
* 1.PWM1_CH0,PWM1_CH1���ģʽ��PWM1_CH0���Ƶ��Ϊ20K��ռ�ձ�Ϊ25%�Ĳ��Σ�PWM1_CH1��PWM1_CH0���λ�������1us�������롣
* 2.PWM����������жϣ������PWM�����м���
*
*/
void PWM1_GenerateFrequency(void)
{
    PWM_CombineChConfig combineChConfig[1]; //���ģʽ��ؽṹ��
    PWM_ModulationConfigType pwmConfig; //PWMģʽ��ؽṹ��
    PWM_ConfigType config; //PWMģ��ṹ��
    //�ṹ����������
    memset(&combineChConfig, 0, sizeof(combineChConfig));
    memset(&pwmConfig, 0, sizeof(pwmConfig));
    memset(&config, 0, sizeof(config));
    
    /*ͨ��0/1������ģʽPWM���*/
    /*
    ���ϼ������ģʽ��
    ����=(MCVR-CNTIN+1)*PWM������ʱ������
    ռ�ձ�=|CH(n+1)V-CH(n)V|*PWM������ʱ������
    ����-���¼������ģʽ��
    ����=2*(MCVR-CNTIN)*PWM������ʱ������
    ռ�ձ�=2*(|CH(n+1)V-CH(n)V|)*PWM������ʱ������
    */
    combineChConfig[0].pairChannel = PWM_CH_0; //PWMͨ��������PWM_CH_0/2/4/6��ӦPAIR0/1/2/3
    combineChConfig[0].ch1stValue = MOD_PWM >> 2; //ͨ��2n channelֵ��nΪPWM�������
    combineChConfig[0].ch2ndValue = MOD_PWM >> 1; //ͨ��2n+1 channelֵ��nΪPWM�������
    combineChConfig[0].levelMode = PWM_HIGH_TRUE; //���PWM����Ч,���ռ�ձ���Ϊ25%����ָ�ĸ���Ч��ƽռ��25%
    combineChConfig[0].deadtimeEn = ENABLE;//��������ʹ�ܣ����ģʽ��֧����������
    combineChConfig[0].complementEn = ENABLE;//����ģʽʹ��,ʹ�ܺ�PWMͨ�����λ�����DISABLE�������ͬ��
    combineChConfig[0].ch1stMatchDir = PWM_MATCH_DIR_DOWN;//��������-���¼���(countModeΪPWM_UP_DOWN_COUNT)���ģʽ��Ч������ѡ��ƥ����Ч�㷽��
    combineChConfig[0].ch2ndMatchDir = PWM_MATCH_DIR_DOWN;//��������-���¼���(countModeΪPWM_UP_DOWN_COUNT)���ģʽ��Ч������ѡ��ƥ����Ч�㷽��
    combineChConfig[0].ch1stPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//������Ը���Ч��PWM mask��PWM����͵�ƽ
    combineChConfig[0].ch2ndPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//������Ը���Ч��PWM mask��PWM����͵�ƽ
    combineChConfig[0].ch1stInterruptEn = DISABLE;//PWMͨ��ƥ���ж�ʹ��λ
    combineChConfig[0].ch2ndInterruptEn = DISABLE;//PWMͨ��ƥ���ж�ʹ��λ
    combineChConfig[0].ch1stInitLevel = PWM_LOW_LEVEL;//PWM��ʼ��ƽ���Ϊ�ͣ���������initChOutputEn���ƣ�����PWM������δ����ǰPWM�ڵ������ƽֵ��
    combineChConfig[0].ch2ndInitLevel = PWM_LOW_LEVEL;//PWM��ʼ��ƽ���Ϊ�ͣ���������initChOutputEn���ƣ�����PWM������δ����ǰPWM�ڵ������ƽֵ��
    combineChConfig[0].ch1stTriggerEn = DISABLE;//ͨ��2n�ⲿ����ʹ�ܣ�nΪPWM�������
    combineChConfig[0].ch2ndTriggerEn = DISABLE;//ͨ��2n+1�ⲿ����ʹ�ܣ�nΪPWM�������

    /*modulation mode config*/
    pwmConfig.countMode = PWM_UP_COUNT; //PWM������ģʽ (��ͬ�ļ���ģʽƵ�ʼ�ռ�ձȼ��㷽ʽ��ͬ)
    pwmConfig.independentChannelNum = 0; //����ͨ����
    pwmConfig.combineChannelNum = 1; //��϶���
    pwmConfig.independentChConfig = NULL; //����ͨ�����ñ�����ַ��ֵ
    pwmConfig.combineChConfig = combineChConfig; //���ͨ�����ñ�����ַ��ֵ
    pwmConfig.deadtimePsc = PWM_DEADTIME_DIVID_1;//���������Ƶֵ����deadtimeһ���������������ʱ�䡣
    pwmConfig.deadtime = 24;  //����ʱ�� = (DTPSC * DTVAL)/PWM������ʱ������ = 1*24/24000000 = 1us
    pwmConfig.initChOutputEn = ENABLE; //ʹ�ܳ�ʼ��ͨ�������ʹ�ܺ����PWMģʽ��initLevel�����PWMģʽ��ch1stInitLevel��ch2ndPolarity���òŻ���Ч
    pwmConfig.initTriggerEn = ENABLE; //ͨ���ⲿ����ʹ��
    
    /*pwm config*/
    config.mode = PWM_MODE_MODULATION;//PWMģ������ΪPWMģʽ
    config.initModeStruct = &pwmConfig;//PWM���ýṹ���ַ��ֵ
    config.clkSource = PWM_CLK_SOURCE_APB; //PWMʱ��Դ����
    config.clkPsc = PWM_PRES;//PWMʱ��Դ��Ƶ
    config.initValue = 0;//��������ʼ�Ĵ���ֵ
    config.maxValue = MOD_PWM - 1; //PWM���������ֵ
    config.overflowInterrupEn = ENABLE;//����������ж�ʹ��
    config.cntOverflowFreq = 0;//CNTOF�жϲ�����Ƶ���������Ƶ�ʵĹ�ϵ(0-127), 0��ʾÿ�μ������������������жϣ�1��ʾ���1�Σ�2��ʾ���2�Σ��Դ����ơ�
    config.interruptEn = ENABLE; //PWM�ж�ʹ��
    config.callBack = PWM1_CallBack; //PWM�жϻص�
    
    PWM_Init(PWM1, &config); //���ó�ʼ����Ч

    NVIC_SetPriority(PWM1_IRQn, 0); //����PWMģ���жϵ����ȼ�
}
/**********<End>*********/
