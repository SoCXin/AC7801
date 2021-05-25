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
#include "pwdt_sample.h"
#include "pwm_sample.h"
/*********<Variable>********/
uint32_t g_positvePulse;///<pulse width positive value
uint32_t g_negativePulse;///<pulse width negative value
uint32_t g_freq;///<pulse width period value
float g_duty;///<pulse duty value

/**
* PWDT0_Capture
*
* @param[in] none
*
* @return none
*
* @brief PWDT Pulse Width Detection CallBack
*/
void PWDT0_Capture(void *device, uint32_t wpara, uint32_t lpara)
{
    //PWDT����ж�
    if (wpara & PWDT_INIT0_OVF_Msk)
    {
        
    
    }
    //������Ч�ж�
    if (wpara & PWDT_INIT0_RDYF_Msk)
    {
        g_positvePulse = PWDT_GetPositivePulseWidth(PWDT0); //��ȡ���������ֵ
        g_negativePulse = PWDT_GetNegativePulseWidth(PWDT0); //��ȡ���������ֵ
        if (0 != (g_positvePulse + g_negativePulse))
        {
            /*
                ����Ƶ�ʺ�ռ�ձ�
                Ƶ�� = PWDTʱ��Դ / PWDTʱ�ӷ�Ƶ / ��+���������ֵ
                ռ�ձ� = ���������ֵ / ��+���������ֵ
            */
            g_freq = PWDT_CLK / (g_positvePulse + 1 + g_negativePulse + 1);
            g_duty = (float)(((float)g_positvePulse * 100) / (float)(g_positvePulse +1 + g_negativePulse + 1)); 
//            printf("\ng_freq=%d, g_duty =%f.\r\n", g_freq, g_duty);
        }
        
    }
}


/**
* PWDT_Initalize
*
* @param[in] none
*
* @return 0:Success
*
* @brief PWDTģ���ʼ��
*/
void PWDT_Initalize(void)
{
    PWDT_ConfigType  pwdtConfig;
    
    //����PWDT GPIO
    GPIO_SetFunc(GPIOA, GPIO_PIN10, GPIO_FUN3);  ///PWDT0_IN2
    
    /* General parameters */
    pwdtConfig.clkPsc = PRESCALER;  //PWDTʱ��Դ1��Ƶ
    pwdtConfig.edgeType = PWDT_RISING_START_CAPTURE_ALL; //�����ؿ�ʼ����˫���ز���
    pwdtConfig.channel = PWDT_INPUT_CH_2;//PWDT_IN2�����ź�
    pwdtConfig.overflowInterruptEn = ENABLE; //ʹ������ж�
    pwdtConfig.readyInterruptEn = ENABLE;  //ʹ��������Ч�ж�
    pwdtConfig.interruptEn = ENABLE;       //ʹ��PWDTģ���ж�
    pwdtConfig.callBack = PWDT0_Capture;    //����PWDT�жϴ���ص�����
    pwdtConfig.cmpModeEn = DISABLE;         //�Ƚ�������ȥ��
    pwdtConfig.hallModeEn = DISABLE;        //HALLģʽȥ��
    pwdtConfig.mode = PWDT_MEASURE_MODE;    //�������ģʽ
    /* Filter parameters */
    //Filter width = 1 * 15 = 15*bclk
    pwdtConfig.filterPsc = PWDT_FILTER_PRESCALER_1;  //�˲�����Ƶ  1~12�ֱ��ʾ2/4/8.../4096
    pwdtConfig.filterValue = 15;//0~15��ʾ�˲���ͬ����
    pwdtConfig.filterEn = ENABLE;//ʹ��PWDT�˲������˵�С��pwdtFilterValue * (1 << pwdtFilterPsc)*bclk �ĸ����ź�
    /* Timer parameters */
    pwdtConfig.periodValue = 0;
    PWDT_Init(PWDT0, &pwdtConfig);//����PWDT
}

/**
 * PWDT_OneChannelPWMCapture
 *
 * @param[in] void
 * @return void
 *
 * @brief PWDT�ɼ�PWMƵ�ʺ�ռ�ձȣ�����ֵΪg_freq��Ƶ�ʣ���g_duty��ռ�ձȣ������㷽�����PWDT_Capture��
 */
void PWDT_OneChannelPWMCapture(void)
{

    //PWDTģ���ʼ��
    PWDT_Initalize();
    while(1)
    {
        mdelay(500);
        printf("\ng_freq=%d, g_duty =%f%.\r\n", g_freq, g_duty);
    }
}

/**********<End>*********/
