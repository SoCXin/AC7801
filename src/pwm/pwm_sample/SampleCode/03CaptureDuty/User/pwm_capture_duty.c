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

/**********<Macro>************/
#define   CAPTURE_FREQ          0
#define   CAPTURE_DUTY          1
/**********<Variable>*********/
uint32_t g_captureValue0;
uint32_t g_captureValue1;
uint32_t g_countValue = 0;
uint32_t g_freqCountValue = 1;
uint32_t g_captureMask = 0;///<capture mask flag
uint32_t g_captrueDuty;
uint32_t g_captureFreq = 0;
uint8_t g_captureMode = 0; //0 freq 1 duty
/**********<Prototype>********/
void PWM0_Capture_Duty(void);

/**********<Prototype>*********/
/**
* PWM0_Singal_Capture
* @param[in] none
*
* @return none
*
* @brief PWM �������жϴ�����
*
*/
/**
* PWM0_Singal_Capture
*
* @param[in] none
*
* @return none
*
* @brief PWM0 CallBack
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
        
        //user add
        //ͨ��0�ж�
        if (lpara & PWM_STR_CH0SF_Msk)
        {
            
        }
        //ͨ��1�ж�
        if (lpara & PWM_STR_CH1SF_Msk)
        {
            //��ȡCH0,CH1 countֵ
            g_captureValue0 = PWM0->CHANNELS[0].CHV;
            g_captureValue1 = PWM0->CHANNELS[1].CHV;
            if (g_captureValue0)
            {
                //���CH1��¼ֵС��CH0��˵��CH1�����������ǰ��������ֵ
                if (g_captureValue1 < g_captureValue0)
                {
                    g_captureValue1 = g_captureValue1 + 0xffff;
                }
                //����CH1��CH0��ֵ��
                g_countValue = g_captureValue1 - g_captureValue0;
            }
        }
        if (g_countValue)
        {
            if (g_captureMode == CAPTURE_DUTY)
            {
                if( 0 != g_captureFreq)
                {
                    /*
                        ����ռ�ձȵļ��㹫ʽ��
                        PWM���� = 1 / (PWMģ��ʱ�� /PWMģ���Ƶ/PWMģ��MODֵ/�ź�Ƶ��)
                        ռ�ձ� = �ߵ�ƽʱ�� / PWM����
                    */

                    g_captrueDuty = (g_countValue * 100)/ g_freqCountValue;
                }
            }
            else if (g_captureMode == CAPTURE_FREQ)
            {
                /*
                 Ƶ�� = PWMʱ��Դ / PWMģ���Ƶֵ / ���񵽵�Count�����
                 */
                g_captureFreq = APB_CLK / (PWM_CAPTURE_PRESCALER + 1) / g_countValue;
                g_freqCountValue = g_countValue;
                if( 0 != g_captureFreq)
                {
                    g_captureMode = CAPTURE_DUTY;
                    PWM0_Capture_Duty();
                }
            }
        }

    }
}
/**
* PWM0_Capture_Duty
*
* @param[in]  None
* @return     None
*
* @brief  ˫ͨ�����ز���ռ�ձ�ԭ��
          ��CH2n��Ϊ�����أ�CH2n+1��Ϊ�½��أ��������Ͻ���ʱ��CH2n��¼��ǰ��Countֵ��
          �������½���ʱ��CH2n+1��¼��ǰ��Countֵ������CH2n+1�����½���ʱ����ͨ���жϡ�
          Ȼ���������ͨ��������ߵ�ƽ��Countֵ�������֪Ƶ�ʣ��������ռ�ձȡ�
*/
void PWM0_Capture_Duty(void)
{
    PWM_DeInit(PWM0);
    PWM_inputChConfigType inputChConfig[1];//����ͨ������
    PWM_inputCaptureConfigType inputCapconfig;//���벶��ṹ��
    PWM_ConfigType config; //PWMģ��ṹ��

    //�ṹ����������
    memset(&inputChConfig, 0, sizeof(inputChConfig));
    memset(&inputCapconfig, 0, sizeof(inputCapconfig));
    memset(&config, 0, sizeof(config));
    //����ͨ���ṹ�����ã�����ж������ͨ�������Ľṹ�����飬�ں������������ü��ɡ�

    /*
        ��ʼ��PWMΪ˫���ز���ģʽ����ʹ��һ��ͨ�����в����ź�����Ϊż�����ͨ����
        ����Ƶ�ʼ�ⷽʽ��Ϊ�������½��أ�����ռ�ձȼ�ⷽʽ��Ϊ�������������⡣
    */
    inputChConfig[0].channel = PWM_CH_0; //�źŲ���ͨ��
    inputChConfig[0].mode = PWM_INPUTCAP_DUAL_EDGE; //˫���ز���ģʽ
    inputChConfig[0].measureType = PWM_POSITIVE_PLUSE_WIDTH_MEASURE;// �ߵ�ƽ���
    inputChConfig[0].onceMode = PWM_INPUTCAP_CONTINUOUS;//��������
    inputChConfig[0].filterEn = DISABLE;//ʹ�������˲���CH0-CH3���иù���
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
* PWM0_Capture_Freq
*
* @param[in]  None
* @return     None
*
* @brief  Ҳ����ͨ��˫ͨ�����ز���PWMƵ�ʡ���ע�⣬����·ͨ��������һ�飬��PWM2 CH0,CH1��
          ˫ͨ�����ز���Ƶ��ԭ��
          �����˫ͨ������ģʽ����Ϊ�½��أ��������γ����½���ʱ��CH2n��¼��ǰ��Countֵ��
          ������ż�����½���ʱ��CH2n+1��¼��ǰ��Countֵ������CH2n+1�����½���ʱ����ͨ���жϡ�
          Ȼ���������ͨ��ͳ�Ƶ�Countֵ����Ƶ��
          
*/
void PWM0_Capture_Freq(void)
{
    PWM_DeInit(PWM0);//ȥ��ʼ��PWM0  
    PWM_inputChConfigType inputChConfig[2];//����ͨ������
    PWM_inputCaptureConfigType inputCapconfig;//���벶��ṹ��
    PWM_ConfigType config; //PWMģ��ṹ��

    //�ṹ����������
    memset(&inputChConfig, 0, sizeof(inputChConfig));
    memset(&inputCapconfig, 0, sizeof(inputCapconfig));
    memset(&config, 0, sizeof(config));
    //����ͨ���ṹ�����ã�����ж������ͨ�������Ľṹ�����飬�ں������������ü��ɡ�
    /*
        ��ʼ��PWMΪ˫���ز���ģʽ����ʹ��һ��ͨ�����в����ź�����Ϊż�����ͨ����
        ����Ƶ�ʼ�ⷽʽ��Ϊ�������½��أ�����ռ�ձȼ�ⷽʽ��Ϊ�������������⡣
    */
    //ͨ��0
    inputChConfig[0].channel = PWM_CH_0; //�źŲ���ͨ��
    inputChConfig[0].mode = PWM_INPUTCAP_DUAL_EDGE; //˫���ز���ģʽ
    inputChConfig[0].measureType = PWM_FALLING_EDGE_PERIOD_MEASURE;// �½��ؼ��
    inputChConfig[0].onceMode = PWM_INPUTCAP_CONTINUOUS;//��������
    inputChConfig[0].filterEn = DISABLE;//ʹ�������˲���CH0-CH3���иù���
    inputChConfig[0].filterValue = 0; //filterValueֵ��Χ0-31�������źŽ����ӳ�filterValue*4������ʱ��
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
* PWM_SampleCaptureDuty
*
* @param[in]  None
* @return     None
*
* @brief  ͨ��PWM˫���ز���Ƶ�ʺ�ռ�ձȡ��Ȼ�ȡƵ�ʣ��ٻ�ȡռ�ձȡ������ͬʱ��ȡƵ�ʺ�ռ�ձȣ��Ƽ�ʹ��PWDTģ��
          
*/
void PWM_SampleCaptureDuty(void)
{
    PWM0_Capture_Freq();
}

/**********<End>*********/
