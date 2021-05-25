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
#include "pwdt_sample.h"
/**********<Macro>************/

/**********<Variable>*********/
uint32_t g_positvePulse;///<pulse width positive value
uint32_t g_negativePulse;///<pulse width negative value
uint32_t g_freq;///<pulse width period value
float g_duty;///<pulse duty value
uint32_t g_hallStatus;///<hall status value
/**
* PWDT_Hall
*
* @param[in] none
*
* @return none
*
* @brief PWDT Pulse Width Detection CallBack
*/
void PWDT_Hall(void *device, uint32_t wpara, uint32_t lpara)
{
    //PWDT����ж�
    if (wpara & PWDT_INIT0_OVF_Msk)
    {
        
    
    }
    //������Ч�ж�
    if (wpara & PWDT_INIT0_RDYF_Msk)
    {
        g_hallStatus = PWDT_GetHallStatus(PWDT) & 0x07;
        g_positvePulse = PWDT_GetPositivePulseWidth(PWDT); //��ȡ���������ֵ
        g_negativePulse = PWDT_GetNegativePulseWidth(PWDT); //��ȡ���������ֵ
        if (0 != (g_positvePulse + g_negativePulse))
        {
            /*
                ����Ƶ�ʺ�ռ�ձ�
                Ƶ�� = PWDTʱ��Դ / PWDTʱ�ӷ�Ƶ / ��+���������ֵ
                ռ�ձ� = ���������ֵ / ��+���������ֵ
            */
            g_freq = PWDT_CLK / (g_positvePulse + g_negativePulse);
            g_duty = ((g_positvePulse * 100)/ (g_positvePulse + g_negativePulse)); 
        }
        
    }
}
/**
* PWDT_GpioInit
*
* @param[in] none
*
* @return 0:Success
*
* @brief PWDT GPIO Init
*
*/
void PWDT_GpioInit(void)
{
    //����PWDT GPIO
    GPIO_SetFunc(GPIOC, GPIO_PIN8, GPIO_FUN3);  ///< PWDT1 channel 0
    GPIO_SetFunc(GPIOC, GPIO_PIN7, GPIO_FUN3);  ///< PWDT1 channel 1
    GPIO_SetFunc(GPIOC, GPIO_PIN6, GPIO_FUN3);  ///< PWDT1 channel 2
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
    ///����PWDT
    PWDT_ConfigType  pwdtConfig;
    memset(&pwdtConfig, 0, sizeof(pwdtConfig));
     /* General parameters */
    pwdtConfig.clkPsc = PRESCALER;  //PWDTʱ��Դ1��Ƶ
    pwdtConfig.edgeType = PWDT_RISING_START_CAPTURE_ALL; //�����ؿ�ʼ����˫���ز���
//    pwdtConfig.channel = PWDT_INPUT_CH_2;//HALL��ѯģʽ������ͨ����
    pwdtConfig.overflowInterruptEn = ENABLE; //ʹ������ж�
    pwdtConfig.readyInterruptEn = ENABLE;  //ʹ��������Ч�ж�
    pwdtConfig.interruptEn = ENABLE;       //ʹ��PWDTģ���ж�
    pwdtConfig.callBack = PWDT_Hall;    //����PWDT�жϴ���ص�����
    pwdtConfig.cmpModeEn = DISABLE;         //�Ƚ�������ȥ��
    pwdtConfig.hallModeEn = ENABLE;        //HALLģʽʹ��
    pwdtConfig.mode = PWDT_MEASURE_MODE;    //�������ģʽ
    /* Filter parameters */
    //Filter width = 4 * 15 = 60*bclk
    pwdtConfig.filterPsc = PWDT_FILTER_PRESCALER_4;  //�˲�����Ƶ  1~12�ֱ��ʾ2/4/8.../4096
    pwdtConfig.filterValue = 15;//0~15��ʾ�˲���ͬ����
    pwdtConfig.filterEn = ENABLE;//ʹ��PWDT�˲������˵�С��pwdtFilterValue * (1 << pwdtFilterPsc)*bclk �ĸ����ź�
    /* Timer parameters */
    pwdtConfig.periodValue = 0;
    PWDT_Init(PWDT, &pwdtConfig);//����PWDT
}


/**
 * PWDT_ThreeChannelInputHallstatus
 *
 * @param[in] void
 * @return void
 *
 * @brief 
    1.ͨ��PWDT_GetHallStatus��ֱ�ӻ�ȡHALL״̬���.
    2.PWDT�ɼ�HALL�ź�Ƶ�ʣ�����ֵΪg_freq��Ƶ�ʣ���g_duty��ռ�ձȣ������㷽�����PWDT_Hall
 */
void PWDT_ThreeChannelInputHallstatus(void)
{
    //PWDTģ���ʼ��
    PWDT_GpioInit();
    PWDT_Initalize();
    while(1)
    {
        mdelay(500);
        //TODO
    }

}

/**********<End>*********/
