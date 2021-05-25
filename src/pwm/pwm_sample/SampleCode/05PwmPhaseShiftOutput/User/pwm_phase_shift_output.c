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
    //config PWM0 pinmux
    GPIO_SetFunc(GPIOB, GPIO_PIN12, GPIO_FUN1);//PWM0_CH2
    GPIO_SetFunc(GPIOB, GPIO_PIN11, GPIO_FUN1);//PWM0_CH3  
    
    GPIO_SetFunc(GPIOB, GPIO_PIN3, GPIO_FUN1);//PWM0_CH6
    GPIO_SetFunc(GPIOA, GPIO_PIN10, GPIO_FUN1);//PWM0_CH7

}
/**
* PWM0_SetDutyAndPhaseShift
*
* @param[in] pwmDuty: set PWM Duty(from 1 to PHASE_SHIFT_MAX_DUTY_CNT)
*            phaseShift:set phase shift value.(0-359)
*
* @return none
*
* @brief PWM0 CH6,CH7���������PWM0 CH2,CH3���������PWM0 CH2���CH6����phaseShift��
*
*/
void PWM0_SetDutyAndPhaseShift(uint16_t pwmDuty, uint16_t phaseShift)
{
    uint16_t pwmCnt = 0;
    uint16_t modulo = PWM0->MCVR;//��ȡMODֵ 
    uint32_t ch6value,ch7value,ch2value,ch3value;
    uint16_t phaseShiftValue;
    phaseShiftValue = (phaseShift * modulo)/360;   //��������ֵ
    pwmCnt = pwmDuty * (modulo + 1) / PHASE_SHIFT_MAX_DUTY_CNT;
    ch6value = 0;
    ch7value = pwmCnt + ch6value;
    ch2value = ch6value  + phaseShiftValue;
    if (ch2value > modulo)
    {
        ch2value -= modulo;
    }
    ch3value = pwmCnt + ch2value;
    if (ch3value > modulo)
    {
        ch3value -= modulo;
    }
    PWM0->CHANNELS[2].CHV = ch2value;  //CH2ͨ����ֵ
    PWM0->CHANNELS[3].CHV = ch3value;
    PWM0->CHANNELS[6].CHV = ch6value;
    PWM0->CHANNELS[7].CHV = ch7value;
    PWM_SetSoftwareTriggerSync(PWM0, ENABLE);
}
/**
* PWM0_GenerateFrequency
* 
* @param[in] none
* @return none
*
* @brief 
* PWM0_CH6(PB3),PWM0_CH7(PA10)���������PWM0_CH2(PB12),PWM0_CH3(PB11)���������
*
*/
void PWM0_GenerateFrequency(void)
{
    PWM_CombineChConfig combineChConfig[2]; //���ģʽ��ؽṹ��
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
    combineChConfig[0].pairChannel = PWM_CH_2; //PWMͨ��������PWM_CH_0/2/4/6��ӦPAIR0/1/2/3
    combineChConfig[0].ch1stValue = 0; //ͨ��2n channelֵ��nΪPWM�������
    combineChConfig[0].ch2ndValue = 0; //ͨ��2n+1 channelֵ��nΪPWM�������
    combineChConfig[0].levelMode = PWM_HIGH_TRUE; //���PWM����Ч,���ռ�ձ���Ϊ25%����ָ�ĸ���Ч��ƽռ��25%
    combineChConfig[0].deadtimeEn = DISABLE;//��������ȥ��
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

    combineChConfig[1].pairChannel = PWM_CH_6; //PWMͨ��������PWM_CH_0/2/4/6��ӦPAIR0/1/2/3
    combineChConfig[1].ch1stValue = 0; //ͨ��2n channelֵ��nΪPWM�������
    combineChConfig[1].ch2ndValue = 0; //ͨ��2n+1 channelֵ��nΪPWM�������
    combineChConfig[1].levelMode = PWM_HIGH_TRUE; //���PWM����Ч,���ռ�ձ���Ϊ25%����ָ�ĸ���Ч��ƽռ��25%
    combineChConfig[1].deadtimeEn = DISABLE;//��������ȥ��
    combineChConfig[1].complementEn = ENABLE;//����ģʽʹ��,ʹ�ܺ�PWMͨ�����λ�����DISABLE�������ͬ��
    combineChConfig[1].ch1stMatchDir = PWM_MATCH_DIR_DOWN;//��������-���¼���(countModeΪPWM_UP_DOWN_COUNT)���ģʽ��Ч������ѡ��ƥ����Ч�㷽��
    combineChConfig[1].ch2ndMatchDir = PWM_MATCH_DIR_DOWN;//��������-���¼���(countModeΪPWM_UP_DOWN_COUNT)���ģʽ��Ч������ѡ��ƥ����Ч�㷽��
    combineChConfig[1].ch1stPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//������Ը���Ч��PWM mask��PWM����͵�ƽ
    combineChConfig[1].ch2ndPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//������Ը���Ч��PWM mask��PWM����͵�ƽ
    combineChConfig[1].ch1stInterruptEn = DISABLE;//PWMͨ��ƥ���ж�ʹ��λ
    combineChConfig[1].ch2ndInterruptEn = DISABLE;//PWMͨ��ƥ���ж�ʹ��λ
    combineChConfig[1].ch1stInitLevel = PWM_LOW_LEVEL;//PWM��ʼ��ƽ���Ϊ�ͣ���������initChOutputEn���ƣ�����PWM������δ����ǰPWM�ڵ������ƽֵ��
    combineChConfig[1].ch2ndInitLevel = PWM_LOW_LEVEL;//PWM��ʼ��ƽ���Ϊ�ͣ���������initChOutputEn���ƣ�����PWM������δ����ǰPWM�ڵ������ƽֵ��
    combineChConfig[1].ch1stTriggerEn = DISABLE;//ͨ��2n�ⲿ����ʹ�ܣ�nΪPWM�������
    combineChConfig[1].ch2ndTriggerEn = DISABLE;//ͨ��2n+1�ⲿ����ʹ�ܣ�nΪPWM�������

    /*modulation mode config*/
    pwmConfig.countMode = PWM_UP_COUNT; //PWM������ģʽ (��ͬ�ļ���ģʽƵ�ʼ�ռ�ձȼ��㷽ʽ��ͬ)
    pwmConfig.independentChannelNum = 0; //����ͨ����
    pwmConfig.combineChannelNum = 2; //��϶���
    pwmConfig.independentChConfig = NULL; //����ͨ�����ñ�����ַ��ֵ
    pwmConfig.combineChConfig = combineChConfig; //���ͨ�����ñ�����ַ��ֵ
    pwmConfig.deadtimePsc = PWM_DEADTIME_DIVID_1;//���������Ƶֵ����deadtimeһ���������������ʱ�䡣
    pwmConfig.deadtime = 0;  //����ʱ�� = (DTPSC * DTVAL)/PWM������ʱ������
    pwmConfig.initChOutputEn = ENABLE; //ʹ�ܳ�ʼ��ͨ�������ʹ�ܺ����PWMģʽ��initLevel�����PWMģʽ��ch1stInitLevel��ch2ndPolarity���òŻ���Ч
    pwmConfig.initTriggerEn = DISABLE; //ͨ���ⲿ����ʹ��
    
    /*pwm config*/
    config.mode = PWM_MODE_MODULATION;//PWMģ������ΪPWMģʽ
    config.initModeStruct = &pwmConfig;//PWM���ýṹ���ַ��ֵ
    config.clkSource = PWM_CLK_SOURCE_APB; //PWMʱ��Դ����
    config.clkPsc = PWM_PRES;//PWMʱ��Դ��Ƶ
    config.initValue = 0;//��������ʼ�Ĵ���ֵ
    config.maxValue = PHASE_SHIFT_MOD_PWM - 1; //PWM���������ֵ
    config.overflowInterrupEn = DISABLE;//����������ж�ȥ��
    config.cntOverflowFreq = 0;//CNTOF�жϲ�����Ƶ���������Ƶ�ʵĹ�ϵ(0-127), 0��ʾÿ�μ������������������жϣ�1��ʾ���1�Σ�2��ʾ���2�Σ��Դ����ơ�
    config.interruptEn = DISABLE; //PWM�ж�ȥ��
    config.callBack = NULL; //PWM�жϻص�
    
    PWM_Init(PWM0, &config); //���ó�ʼ����Ч

    NVIC_SetPriority(PWM0_IRQn, 0); //����PWMģ���жϵ����ȼ�
}
/**
* PWM_PhaseShiftOutput
*
* @param[in]  None
* @return     None
*
* @brief PWM0ռ�ձ�ѭ������
          
*/
void PWM_PhaseShiftOutput(void)
{
    uint16_t pwmDuty = 0,phaseShift = 180;
    PWM0_GenerateFrequency();
    while (1)
    {
        pwmDuty ++;
        if (pwmDuty > PHASE_SHIFT_MAX_DUTY_CNT)
        {
            pwmDuty = 0;
        }
        PWM0_SetDutyAndPhaseShift(pwmDuty, phaseShift);
        mdelay(100); 
        printf("\noutput pwm duty: %d, pahse shift: %d.\r\n", pwmDuty/10, phaseShift);
    }
}

/**********<End>*********/
