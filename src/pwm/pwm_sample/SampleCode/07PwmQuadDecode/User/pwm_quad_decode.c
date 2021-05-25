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

int16_t g_encoderCircleCnt = 0;  //��������תȦ������
uint16_t g_encoderCnt = 0; //������λ��
uint8_t g_encoderDir = 0;  //����������
/**
* PWM0_QuadTask
* @brief PWM quad decode call back function.
*
* @param[in] device: device pointer
* @param[in] wpara: callback parameter
* @param[in] lpara: callback parameter
* @return none
*/
void PWM0_QuadTask(void *device, uint32_t wpara, uint32_t lpara)
{
     //PWM����������ж�
    if (wpara & PWM_INIT_CNTOF_Msk) 
    {
        //user add 
        g_encoderDir = (PWM0->QDI & PWM_QDI_QUADIR_Msk)>>2; //��ȡ���������������
        if (g_encoderDir != 0)
        {
            g_encoderCircleCnt++;
        }
        else
        {
            g_encoderCircleCnt--;
        }
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
*
* @param[in] none
*
* @return none
*
* @brief PWM GPIO Init
*
*/
void PWM_QuadDecodeGpioInit(void)
{
    //����GPIOΪPWM
    GPIO_SetFunc(GPIOA, GPIO_PIN1, GPIO_FUN1);//PWM0_CH0,A��
    GPIO_SetFunc(GPIOA, GPIO_PIN0, GPIO_FUN1);//PWM0_CH1,B��

}
/**
* PWM_QuadDecodeInit
*
* @param[in]  None
* @return     None
*
* @brief PWM0_CH0:A�����룬PWM0_CH1:B������
*/
void PWM_QuadDecodeInit(void)
{
    PWM_QuadPhaseConfigType quadPhaseAConfig,quadPhaseBConfig;
    PWM_QuadDecodeConfigType quadDecodeConfig;

    PWM_ConfigType config; //PWMģ��ṹ��

    //�ṹ����������
    memset(&quadPhaseAConfig, 0, sizeof(quadPhaseAConfig));
    memset(&quadPhaseBConfig, 0, sizeof(quadPhaseBConfig));
    memset(&quadDecodeConfig, 0, sizeof(quadDecodeConfig));
    memset(&config, 0, sizeof(config));

    quadPhaseAConfig.polarity = PWM_QUAD_PHASE_NORMAL; //���뼫��Ϊ�������ԡ�
    quadPhaseAConfig.filterEn = DISABLE;//�˲���ȥ��
    quadPhaseAConfig.filterVal = 0;//�˲�ֵ
    quadPhaseBConfig.polarity = PWM_QUAD_PHASE_NORMAL; //���뼫��Ϊ�������ԡ�
    quadPhaseBConfig.filterEn = DISABLE;//�˲���ȥ��
    quadPhaseBConfig.filterVal = 0;//�˲�ֵ
    quadDecodeConfig.mode = PWM_QUAD_PHASE_ENCODE; //A���B�����ģʽ
    quadDecodeConfig.phaseAConfig = quadPhaseAConfig; //A�����ø�ֵ
    quadDecodeConfig.phaseBConfig = quadPhaseBConfig; //B�����ø�ֵ
    quadDecodeConfig.quadEn = ENABLE; //��������ʹ��
    //PWMģ������
    config.mode = PWM_MODE_QUADRATURE_DECODER;//PWMģ������Ϊ��������ģʽ
    config.initModeStruct = &quadDecodeConfig;//�����������ø�ֵ
    config.clkSource = PWM_CLK_SOURCE_APB; //PWMʱ��Դ����
    config.clkPsc = 0;//PWMʱ��Դ��Ƶ
    config.initValue = 0;//��������ʼ�Ĵ���ֵ
    config.maxValue = ENCODER_LINES - 1; //PWM���������ֵ
    config.overflowInterrupEn = ENABLE;//����������ж�ʹ��
    config.cntOverflowFreq = 0;//CNTOF�жϲ�����Ƶ���������Ƶ�ʵĹ�ϵ(0-127), 0��ʾÿ�μ������������������жϣ�1��ʾ���1�Σ�2��ʾ���2�Σ��Դ����ơ�
    config.interruptEn = ENABLE; //PWM�ж�ʹ��
    config.callBack = PWM0_QuadTask; //PWM�жϻص�
    PWM_Init(PWM0, &config); //���ó�ʼ����Ч
    NVIC_SetPriority(PWM0_IRQn, 0); //����PWMģ���жϵ����ȼ�
}

void getEncoderCnt(void)
{
    g_encoderCnt = PWM0->CNT & 0xFFFF;   //��ȡ�������������
}

/**
* PWM_QuadDecodeMode
*
* @param[in]  None
* @return     None
*
* @brief A���B�����ģʽ
          
*/
void PWM_QuadDecodeMode(void)
{
    PWM_QuadDecodeGpioInit();
    PWM_QuadDecodeInit();
    while (1)
    {
        getEncoderCnt();
        mdelay(200); 
    }
}


/**********<End>*********/
