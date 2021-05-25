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
#include "adc_sample.h"

/*********<Variable>********/
uint32_t g_averageSampleValue = 0;
uint16_t g_adcInjectValue[4];
uint8_t g_AMOFlag = 0;//ģ�⿴�Ź��¼���־
/*
ע�⣺EOC��־д0���ȡADC_RDR��������ñ�־λ��
�ڽ���debugʱ������д�memory���ڻ��ADC�Ĵ�����
�ñ�־�ᱻdebug�����
*/
uint8_t g_EOCFlag = 0; //������ת��������־��
uint8_t g_IEOCFlag = 0; //ע����ת��������־��
/**
 * ADC_DMACallback
 *
 * @param[in] void
 * @return void
 *
 * @brief ADC DMA�жϻص�������
 */
void ADC_Callback(void *device, uint32_t wpara, uint32_t lpara)
{   
    if (wpara & ADC_STR_IEOC_Msk)    //ע�����жϱ�־
    {
        g_IEOCFlag = 1;
        g_adcInjectValue[0] = ADC_GetInjectData(ADC0, 0);
        g_adcInjectValue[1] = ADC_GetInjectData(ADC0, 1);
        g_adcInjectValue[2] = ADC_GetInjectData(ADC0, 2);
        g_adcInjectValue[3] = ADC_GetInjectData(ADC0, 3);
    }
    if (wpara & ADC_STR_EOC_Msk)    //�������жϱ�־
    {
        g_EOCFlag = 1;
    }
    if (wpara & ADC_STR_AMO_Msk)    //ģ�����жϱ�־
    {
        g_AMOFlag = 1;
    }
}
/**
 * CTU_Config
 *
 * @param[in] void
 * @return void
 *
 * @brief ����CTUģ�飬Timer0����ADC�����������
 */
void CTU_Config(void)
{
    CTU_ConfigType ctuConfig;
    memset(&ctuConfig, 0x00, sizeof(ctuConfig));
    ctuConfig.uart0RxFilterEn = DISABLE;  //ȥ��UART0_RX�˲�
    ctuConfig.rtcCaptureEn = DISABLE;  //ȥ��RTC����
    ctuConfig.acmpCaptureEn = DISABLE;  //ȥ��ACMP����
    ctuConfig.uart0RxCaptureEn = DISABLE;  //ȥ��UART0_RX����
    ctuConfig.uartTxModulateEn = DISABLE;  //ȥ��UART0_TX����
    ctuConfig.clkPsc = CTU_CLK_PRESCALER_1;  //��Ƶ
//    ctuConfig.adcRegularTriggerSource = CTU_TRIGGER_ADC_TIMER_CH0_OVERFLOW; //Timer0����ADC�����������
//    ctuConfig.delay0Time = 0;  //�����ӳ�
    ctuConfig.adcInjectTriggerSource = CTU_TRIGGER_ADC_PWM1_INIT;  //PWM��ʼֵ����ADCע�������
    ctuConfig.delay1Time = 0;
//    ctuConfig.pwdt0In3Source = CTU_PWDT_IN3_SOURCE_UART0_RX;
//    ctuConfig.pwdt1In3Source = CTU_PWDT_IN3_SOURCE_UART0_RX; 
    CTU_Init(&ctuConfig);
}


/**
 * ADC_init
 *
 * @param[in] void
 * @return void
 *
 * @brief ��ʼ��ADC,����ADC������
 */
void ADC_init()
{
    ADC_ConfigType tempAdcConfig;
    ADC_ConfigType* adcConfig;
    adcConfig = &tempAdcConfig;
    //����PINMUX
    GPIO_SetFunc(GPIOA, GPIO_PIN2, GPIO_FUN2);///<ADC_IN8 Analog function enable
    
    adcConfig->clkPsc = ADC_CLK_PRESCALER_1; ///<Set ADC Clk = 24M/2/(0+1)
    adcConfig->scanModeEn = ENABLE;                   //ɨ��ģʽ
    adcConfig->continousModeEn = DISABLE;             //����ģʽ
    adcConfig->regularDiscontinousModeEn = DISABLE;   //1���򿪹�������ת��ģʽ
    adcConfig->injectDiscontinousModeEn = DISABLE;    //1����ע������ת��ģʽ
    adcConfig->injectAutoModeEn = DISABLE;            //1���Զ�ע��ģʽ
    adcConfig->intervalModeEn = DISABLE;              //1��ע����Ϊ���ת��ģʽ
    adcConfig->regularDiscontinousNum = 0;            //
    adcConfig->EOCInterruptEn = ENABLE;              //EOC�ж�ʹ��
    adcConfig->IEOCInterruptEn = ENABLE;             //IEOC�ж�ʹ��
    adcConfig->interruptEn = ENABLE;                 //ʹ���ж�
    adcConfig->regularDMAEn = DISABLE;                 //ȥ��ADC DMA
    adcConfig->regularTriggerMode = ADC_TRIGGER_INTERNAL;//ADC����Դ���ڲ�����
    adcConfig->injectTriggerMode = ADC_TRIGGER_EXTERNAL; //ADC����Դ���ⲿ����
    adcConfig->regularSequenceLength = 0;               //�����鳤����Ϊ0
    adcConfig->injectSequenceLength = 4;               //ע���鳤����Ϊ4
    adcConfig->dataAlign = ADC_DATA_ALIGN_RIGHT;       //�Ҷ���
    adcConfig->callBack =  ADC_Callback;
    adcConfig->powerMode = ADC_POWER_ON;               //�ϵ�
    ADC_Init(ADC0, adcConfig);                      ///<ADC works Mode Config
    /*
        ADCת���ʼ��㹫ʽ��
        ת��ʱ��= ����ʱ��+ת��ʱ��+ͬ��ʱ��
        ת��ʱ��= (SPT+12)/ADCģ��ʱ��Ƶ��+5/APBʱ��Ƶ��
        ��ע��
        1.ͬ��ʱ��Ϊ5��APB CLK��
        2.ADCʱ��Ƶ�� = APBʱ��Ƶ�� /(��Ƶϵ��+1)
	*/
    ADC_SetInjectGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 0);  //����&ת��ʱ��= (7+12)/24000000 + 5/24000000 = 1us
    ADC_SetInjectGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 1);  //����&ת��ʱ��= (7+12)/24000000 + 5/24000000 = 1us
    ADC_SetInjectGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 2);  //����&ת��ʱ��= (7+12)/24000000 + 5/24000000 = 1us
    ADC_SetInjectGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 3);  //����&ת��ʱ��= (7+12)/24000000 + 5/24000000 = 1us
}

void sampleValueDeal(void)
{
    int i;
    uint32_t sumValue = 0;
    for (i = 0; i < 4; i++)
    {
        sumValue += g_adcInjectValue[i];
    }
    g_averageSampleValue = sumValue / 4;
    

}
/**
 * ADC_SamplePwmTrigerInject
 *
 * @param[in] void
 * @return void
 *
 * @brief Timer��ʱ����������ADC_CHANNEL0���β�����
 */
void ADC_SamplePwmTrigerInject(void)
{
    CTU_Config();
    ADC_init();
    while(1)
    {
        mdelay(500);
        sampleValueDeal();
        printf("PWM Triger ADC Inject Sample value: %d\r\n", g_averageSampleValue);
    }
}

/**********<End>*********/
