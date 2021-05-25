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

 I.AC780X ADCģ�����̣������̻���CMSIS 1.0.x�汾�ӿڿ������ԣ�
	1.ͨ����������������������ȡTsensor������ADֵ���������оƬ�¶ȡ�
    
 *****************************************************************************/
 /**********<Incldue>**********/
#include "ac780x.h"
#include "ac780x_debugout.h"
#include "ac780x_adc_reg.h"
#include "ac780x_gpio.h"

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
    adcConfig->clkPsc = ADC_CLK_PRESCALER_1; ///<Set ADC Clk = 24M/2/(0+1)
    adcConfig->scanModeEn = ENABLE;                   //ɨ��ģʽ
    adcConfig->continousModeEn = DISABLE;             //����ģʽ
    adcConfig->regularDiscontinousModeEn = DISABLE;   //1���򿪹�������ת��ģʽ
    adcConfig->injectDiscontinousModeEn = DISABLE;    //1����ע������ת��ģʽ
    adcConfig->injectAutoModeEn = DISABLE;            //1���Զ�ע��ģʽ
    adcConfig->intervalModeEn = DISABLE;              //1��ע����Ϊ���ת��ģʽ
    adcConfig->regularDiscontinousNum = 0;            //
    adcConfig->EOCInterruptEn = DISABLE;              //EOC�ж�ʹ��
    adcConfig->IEOCInterruptEn = DISABLE;             //IEOC�ж�ʹ��
    adcConfig->interruptEn = DISABLE;                 //�ж�ʹ��
    adcConfig->regularDMAEn = DISABLE;                 //ADC DMA
    adcConfig->regularTriggerMode = ADC_TRIGGER_INTERNAL;//ADC����Դ���ڲ�����
    adcConfig->injectTriggerMode = ADC_TRIGGER_INTERNAL; //ADC����Դ���ڲ�����
    adcConfig->regularSequenceLength = 1;               //�����鳤����Ϊ1
    adcConfig->injectSequenceLength = 0;               //ע���鳤����Ϊ0
    adcConfig->dataAlign = ADC_DATA_ALIGN_RIGHT;       //�Ҷ���
    adcConfig->callBack =  NULL;               //�ص�
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
    //������ͨ������
   // ADC_SetRegularGroupChannel(ADC0, ADC_CH_TSENSOR, ADC_SPT_CLK_7, 0);  //����&ת��ʱ��= (7+12)/24000000 + 5/24000000 = 1us

}
/**
 * main
 *
 * @param[in]  none
 * @return 0: success, other: error value
 *
 * @brief  
 */
int main(void)
{
        /* Cali para */

    uint32_t TsensorCali = 0;
    uint8_t VbandgapRT = 0;
    uint8_t VtsensorRT = 0;



    float temperatureVal = 0.0;
    float bgVol = 0.0;
    float vt25Vol = 0.0;
    uint32_t TsensorVol;
    uint32_t TsensorVal;
    uint32_t bandGapAVG;  
    InitDelay();  
    InitDebug(); 
    ADC_init();
    printf("\r\nAC7801x: ADC_Sample\r\n");  




    /* Get Tsensor calibration  value*/

    TsensorCali = *((uint32_t*)0x40002058);
    VbandgapRT = (uint8_t)(TsensorCali & 0x1F);
    VtsensorRT = (uint8_t)((TsensorCali >> 8) & 0xFF);

    /* BG calibration */
    if (VbandgapRT >> 4)
    {
        bgVol = 1200 - (VbandgapRT & 0xF);
    }
    else
    {
        bgVol = 1200 + (VbandgapRT & 0xF);
    }

    /* Vtsensor calibration */
    if (VtsensorRT >> 7)
    {
        vt25Vol = 720 - (VtsensorRT & 0x7F) * 0.5;
    }
    else
    {
        vt25Vol = 720 + (VtsensorRT & 0x7F) * 0.5;
    }

    
    ADC_SetRegularGroupChannel(ADC0, ADC_CH_BANDGAP, ADC_SPT_CLK_215, 0);  //����&ת��ʱ��= (7+12)/24000000 + 5/24000000 = 1us

    ADC_SoftwareStartRegularConvert(ADC0);  
    while (!ADC_GetEOCFlag(ADC0));
    bandGapAVG = ADC_GetRegularData(ADC0);
   // referVol = 4095.0 * bgVol / bandGapAVG;
    
    ADC_SetRegularGroupChannel(ADC0, ADC_CH_TSENSOR, ADC_SPT_CLK_215, 0);
    while (1)
    {
        ADC_SoftwareStartRegularConvert(ADC0);  
        while (!ADC_GetEOCFlag(ADC0));
        
        TsensorVal = ADC_GetRegularData(ADC0);
        //Get Tsensor value and calculate Tsensor voltage(TsensorVol)
        TsensorVol = TsensorVal * bgVol/bandGapAVG;
        temperatureVal = 25 + (vt25Vol - TsensorVol) / 1.629;
        printf("[ADC]Tsensor temperature:%f\r\n", temperatureVal);
        mdelay(1000);
    }

}
