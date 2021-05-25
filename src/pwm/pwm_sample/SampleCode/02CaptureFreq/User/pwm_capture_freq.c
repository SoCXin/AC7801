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
#include "pwm_sample.h"

/**********<Variable>*********/
uint32_t g_captureValueNew;///<capture pulse latest value
uint32_t g_captureValueOld; ///<capture pulse previous value
uint32_t g_captureMask = 0;///<capture mask flag
uint32_t g_captureFreq;
/**********<Prototype>*********/
/**
* PWM0_Singal_Capture
* @param[in] none
*
* @return none
*
* @brief Ƶ�ʲ���ԭ��ͳ�Ʋ���ͨ������CHV�����COUNTֵ������PWMʱ��Դ�����ɼ����Ƶ�ʡ�
*
*/
void PWM0_Singal_Capture(void *device, uint32_t wpara, uint32_t lpara)
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
            //ͨ��g_captureMask������ǰ�����β���
            if (0 == g_captureMask)
            {
                //��ȡ��һ��ͨ���жϲ���ʱCH0��CHV��
                g_captureValueOld = PWM0->CHANNELS[0].CHV;
                g_captureMask = 1;
            }
            else
            {
                //��ȡ�ڶ���ͨ���жϲ���ʱCH0��CHV��
                g_captureValueNew = PWM0->CHANNELS[0].CHV;
                //����ڶ��ε�CountֵС�ڵ�һ�ε�Countֵ��˵��ͨ���в��������������ֵ���Ϻ���м��㡣
                if (g_captureValueNew < g_captureValueOld)
                {
                    g_captureValueNew = g_captureValueNew + 0xffff;
                }
                /*
                 ��ȡ�������½��أ���init�����йأ��жϼ��Countֵ�󣬼���ͨ�����¹�ʽ�����Ƶ��
                 Ƶ�� = PWMʱ��Դ / PWMģ���Ƶֵ / ���񵽵�Count�����
                 */
                g_captureFreq = APB_CLK /(PWM_CAPTURE_PRESCALER + 1) / (g_captureValueNew - g_captureValueOld);
                g_captureMask = 0;
            }
        }
        //ͨ��1�ж�
        if (lpara & PWM_STR_CH1SF_Msk)
        {
            
        }
    }
}
/**
* PWM0_Capture_Freq
*
* @param[in]  None
* @return     None
*
* @brief  ͨ��һ��PWMͨ���Ϳ��Բ���PWMƵ�ʡ�
          ����Ƶ��ԭ���������ģʽ��Ϊ�����أ��򵱳���������ʱ�������жϣ����ж���ȥ��ȡChannelֵ��
          ע�⣺PWMģ�鲻��ͬʱ����Ƶ�ʻ�ռ�ձȡ�����Ҫͬʱ����Ƶ�ʺ�ռ�ձȣ�����PWDTʵ�֡� 
          ����ռ�ձ���ʹ��˫����ģʽ������Ҫ����ͨ���������PWM_SAMPLE_CAPTURE_DUTY���̣���
*/
void PWM0_Capture_Freq(void)
{
    PWM_inputChConfigType inputChConfig[1];//����ͨ������
    PWM_inputCaptureConfigType inputCapconfig;//���벶��ṹ��
    PWM_ConfigType config; //PWMģ��ṹ��

    //�ṹ����������
    memset(&inputChConfig, 0, sizeof(inputChConfig));
    memset(&inputCapconfig, 0, sizeof(inputCapconfig));
    memset(&config, 0, sizeof(config));
    //����ͨ���ṹ�����ã�����ж������ͨ�������Ľṹ�����飬�ں������������ü��ɡ�
    /*
    ��ʼ��PWMΪ����ģʽ������Ƶ���õ�ͨ�����ɡ�����һ��ͨ�������Խ��в���
    ����ģʽ������Ϊ�����ػ��½��أ�������Ϊ˫���ء�
    */
    inputChConfig[0].channel = PWM_CH_0; //�źŲ���ͨ��
    inputChConfig[0].mode = PWM_INPUTCAP_SINGLE_EDGE; //�����ز���ģʽ
    inputChConfig[0].detectType = PWM_FALLING_EDGE_DETECT;//�½��ؼ��
    inputChConfig[0].onceMode = PWM_INPUTCAP_CONTINUOUS;//��������
    inputChConfig[0].filterEn = ENABLE;//ʹ�������˲���CH0-CH3���иù���
    inputChConfig[0].filterValue = 10; //filterValueֵ��Χ0-31�������źŽ����ӳ�filterValue*4������ʱ��
    inputChConfig[0].eventPsc = PWM_EVENT_PSC_1; //�����¼���ͨ���жϵĶ�Ӧ��ϵ��PWM_EVENT_PSC_1��ʾ1�������¼�����һ��ͨ���жϣ�PWM_EVENT_PSC_4��ʾ4�������¼�����һ��ͨ���жϡ�
    inputChConfig[0].interruptEn = ENABLE; //���벶���ж�ʹ��
    
    //���벶������
    inputCapconfig.channelNum = 1;
    inputCapconfig.channelConfig = inputChConfig;//����ͨ�����ñ�����ַ��ֵ��
    
    //PWMģ������
    config.mode = PWM_MODE_INPUT_CAPTURE;//PWMģ������Ϊ���벶��ģʽ
    config.initModeStruct = &inputCapconfig;//���벶�����ñ�����ֵ
    config.clkSource = PWM_CLK_SOURCE_APB; //PWMʱ��Դ����
    /*
    ���ò���ʱ��Ƶ�ʣ�
    �ɲ������СƵ�ʣ�ǰ����ʱ��ԴΪPWM_CLK_SOURCE_APB����
    min_clk = SYS_CLK/(PWM_CAPTURE_PRESCALER+1)/65536 = 24000000/2/65536 = 183HZ
    */
    config.clkPsc = PWM_CAPTURE_PRESCALER;//PWMʱ��Դ��Ƶ
    config.initValue = 0;//��������ʼ�Ĵ���ֵ
    config.maxValue = 65535; //PWM���������ֵ
    config.overflowInterrupEn = DISABLE;//����������ж�ʹ��λ
    config.cntOverflowFreq = 0;//CNTOF�жϲ�����Ƶ���������Ƶ�ʵĹ�ϵ(0-127), 0��ʾÿ�μ������������������жϣ�1��ʾ���1�Σ�2��ʾ���2�Σ��Դ����ơ�
    
    config.interruptEn = ENABLE; //PWM�ж�ʹ��
    config.callBack = PWM0_Singal_Capture; //PWM�жϻص�

    PWM_Init(PWM0, &config); //���ó�ʼ����Ч

    NVIC_SetPriority(PWM0_IRQn, 0); //����PWMģ���жϵ����ȼ�
}
/**
* PWM_SampleCaptureFreq
*
* @param[in]  None
* @return     None
*
* @brief  PWM0_CH0ͨ������ģʽ����PWM1 CH0���Ƶ��
          
*/
void PWM_SampleCaptureFreq(void)
{
    PWM0_Capture_Freq();
}

/**********<End>*********/
