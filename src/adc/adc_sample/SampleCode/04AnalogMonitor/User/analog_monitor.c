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
uint8_t g_dmaFinish = 0;//DMA�������
uint8_t g_halfDmaFinish = 0; //DMA��������
uint8_t g_dmaTransError = 0; //DMA�������
uint32_t g_ADCValueBuffer[DMA_TRANSFER_NUM + 1] = {0};
uint32_t g_timerCnt = 0;
uint16_t g_regularAverageSampleValue = 0; //���������ƽ��ֵ
uint16_t g_injectAverageSampleValue = 0; //ע�������ƽ��ֵ
uint16_t g_adcInjectValue[4];
uint8_t g_AMOErrorFlag = 0;//ģ�⿴�Ź��쳣�¼���־
uint8_t g_AMORecoveryFlag = 0;//ģ�⿴�Ź��ָ��¼���־
uint8_t g_AMOFlag = 0;
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
    if (wpara & ADC_STR_AMO_Msk)    //���ͨ����ѹ���ڸ߷�ֵ��С�ڵͷ�ֵ��AMO��־λ��λ����ƽ����ģʽ��Ч����
    {
        g_AMOFlag = 1;
    }
    if (wpara & ADC_STR_AAMO_Msk)   //���ͨ����ѹ�����������쳣����AAMO��־λ��λ�����ش���ģʽ��Ч����
    {
        g_AMOFlag = 2;
    }
    if (wpara & ADC_STR_NAMO_Msk)    //���ͨ����ѹ���쳣������������NAMO��־λ��λ�����ش���ģʽ��Ч����
    {
        g_AMOFlag = 3;
    }
    
}
/**
 * ADC_DMACallback
 *
 * @param[in]  wpara:dmaChannelStatus 
 *             lpara:0
 * @return 0
 *
 * @brief ADC DMA�жϻص�������
 */

void ADC_DMACallback(void *device, uint32_t wpara, uint32_t lpara)
{
    /*
     wparamΪDMAͨ��״̬��״̬����ɲο�CHANNELx_STATUS�Ĵ�����
     CHANNELx_STATUS[2] �������
     CHANNELx_STATUS[1] �봫����ɣ�������õ�transferNum������봫���ж���ʹ�ܣ�transferNum��Ϊ6����DATA_TRANS_NUMΪ3ʱ�����жϣ�����ص���
     CHANNELx_STATUS[0] �������
    */
    if ((wpara & 0x01) == 0x1)
    {
        g_dmaFinish = 1;
    }
    if ((wpara & 0x02) == 0x2)
    {
        g_halfDmaFinish = 1;
    }
    if ((wpara & 0x04) == 0x4)
    {
        g_dmaTransError = 1;
    }
}
/**
 * ADC_DMAInit
 *
 * @param[in] void
 * @return void
 *
 * @brief ADC DMA��ʼ��������DMA��ز�����
 */
void ADC_DMAInit(void)
{
    uint32_t tmpMemStartAddr = (uint32_t)&g_ADCValueBuffer[0];
    uint32_t tmpMemEndAddr = (uint32_t)&g_ADCValueBuffer[DMA_TRANSFER_NUM + 1]; ///<Setting memory DMA address
    DMA_ConfigType tmpDMAConfig;
    memset(&tmpDMAConfig, 0x00, sizeof(DMA_ConfigType));
    
    tmpDMAConfig.memStartAddr = tmpMemStartAddr; //����DMA��ʼ��ַ
    tmpDMAConfig.memEndAddr = tmpMemEndAddr;//����DMA������ַ
    tmpDMAConfig.periphStartAddr = (uint32_t)(&(ADC0->RDR)); ///<Move ADC DR to memory
    tmpDMAConfig.channelEn = ENABLE;                     ///<ʹ��DMAxͨ��
    tmpDMAConfig.finishInterruptEn = ENABLE;                   ///<ʹ��DMA��������ж�
    tmpDMAConfig.halfFinishInterruptEn = DISABLE;               ///<ȥ��DMA�봫������ж�
    tmpDMAConfig.errorInterruptEn = ENABLE;                    ///<ʹ��DMA��������ж�
    tmpDMAConfig.channelPriority = DMA_PRIORITY_VERY_HIGH;///<����DMAͨ�����ȼ���0~3 �����ȼ��ɵ͵���
    tmpDMAConfig.circular = ENABLE;                      ///<ʹ��ѭ��ģʽ�����ֻ�빤��һ�Σ���Ϊ0���ɡ�
    tmpDMAConfig.direction = DMA_READ_FROM_PERIPH;          ///<0: �������ȡ��1���Ӵ洢����ȡ
    tmpDMAConfig.MEM2MEM = DISABLE;                     ///<0���ڷǴ洢����洢��֮�䴫�䣬1���ڴ洢����洢��֮�䴫��
    tmpDMAConfig.memByteMode = DMA_MEM_BYTE_MODE_1TIME;  ///<MEM�ַָ������0:32-bit,1:16-bit[15:0]; 2:16-bit[23:16][7:0];3:8-bit������ɲο�AC781XоƬ�ֲ�  ��20-2 �ɱ�����ݿ��&���ݶ��� 
    tmpDMAConfig.memIncrement = ENABLE;                  ///<1:MEM��ַ����
    tmpDMAConfig.periphIncrement = DISABLE;               ///<0�������ַ�̶�
    tmpDMAConfig.memSize = DMA_MEM_SIZE_32BIT;           ///<0:8-bit,1:16-bit,2:32-bit
    tmpDMAConfig.periphSize = DMA_PERIPH_SIZE_16BIT;     ///<0:8-bit,1:16-bit,2:32-bit
    tmpDMAConfig.transferNum = DMA_TRANSFER_NUM;         ///<DMAͨ�����䳤��
    tmpDMAConfig.periphSelect = DMA_PEPIRH_ADC0;  //����ѡ��
    tmpDMAConfig.callBack = ADC_DMACallback; ///<����DMA�жϻص�

    DMA_Init(DMA0_CHANNEL0, &tmpDMAConfig);                     ///<ADC ʹ��DMA1ͨ����ÿ��ģ���Ӧ��DMAͨ�����ɲο� AC781XоƬ�ֲ� ��20-1 DMA�����б�
    NVIC_EnableIRQ(DMA0_CHANNEL0_IRQn);                              ///<ʹ��DMA1�ж�����
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
    ADC_AMOConfigType amoConfig;
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
    adcConfig->interruptEn = ENABLE;                 //�ж�ʹ��
    adcConfig->regularDMAEn = ENABLE;                 //ʹ��ADC DMA
    adcConfig->regularTriggerMode = ADC_TRIGGER_INTERNAL;//ADC����Դ���ڲ�����
    adcConfig->injectTriggerMode = ADC_TRIGGER_INTERNAL; //ADC����Դ���ڲ�����
    adcConfig->regularSequenceLength = 4;               //�����鳤����Ϊ1
    adcConfig->injectSequenceLength = 4;               //ע���鳤����Ϊ4
    adcConfig->dataAlign = ADC_DATA_ALIGN_RIGHT;       //�Ҷ���
    adcConfig->callBack =  ADC_Callback;               //�ص�
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
    ADC_SetRegularGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 0);  //����&ת��ʱ��= (7+12)/24000000 + 5/24000000 = 1us
    ADC_SetRegularGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 1);  //����&ת��ʱ��= (7+12)/24000000 + 5/24000000 = 1us
    ADC_SetRegularGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 2);  //����&ת��ʱ��= (7+12)/24000000 + 5/24000000 = 1us
    ADC_SetRegularGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 3);  //����&ת��ʱ��= (7+12)/24000000 + 5/24000000 = 1us
    //ע����ͨ������
    ADC_SetInjectGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 0);  //����&ת��ʱ��= (7+12)/24000000 + 5/24000000 = 1us
    ADC_SetInjectGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 1);  //����&ת��ʱ��= (7+12)/24000000 + 5/24000000 = 1us
    ADC_SetInjectGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 2);  //����&ת��ʱ��= (7+12)/24000000 + 5/24000000 = 1us
    ADC_SetInjectGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 3);  //����&ת��ʱ��= (7+12)/24000000 + 5/24000000 = 1us
    
    //������DMA��ʼ��
    ADC_DMAInit();   //ADC DMA��ʼ��
    //ģ�������� 
    amoConfig.AMOTriggerMode = ADC_AMO_TRIGGER_EDGE;//���ش���ģʽ������쳣�ͻָ�
    amoConfig.AMOInterruptEn = ENABLE; //AMO�ж�ʹ��
    amoConfig.AMORegularEn = ENABLE; //������ͨ�����ʹ��
    amoConfig.AMOInjectEn = DISABLE; //ע����ͨ�����ȥ��
    amoConfig.AMOSingleModeEn = ENABLE; //��ͨ��ʹ��
    amoConfig.AMOSingleChannel = ADC_CH_8;//���ͨ��8
    amoConfig.AMOHighThreshold = AMOHighValue; //��ص�ѹ�߷�ֵ�����ڸ�ֵ����λAMO��־����ƽ����ģʽ��������λAAMO��־�����ش���ģʽ����
    amoConfig.AMOLowThreshold = AMOLowValue; //��ص�ѹ�ͷ�ֵ�����ڸ�ֵ����λAMO��־����ƽ����ģʽ��������λNAMO��־�����ش���ģʽ����
    amoConfig.AMOHighOffset = AMOHighOffsetValue;//�߷�ֵ��ѹ�ָ�ƫ��ֵ���ָ��߽�ֵ= �߷�ֵ-��ƫ��ֵ������ѹ���ڸ߻ָ��߽�ֵ������λNAMO��־��
    amoConfig.AMOLowOffset  = AMOLowOffsetValue;//�ͷ�ֵ��ѹ�ָ�ƫ��ֵ���ָ��߽�ֵ= �ͷ�ֵ+��ƫ��ֵ������ѹ���ڵͻָ��߽�ֵ������λNAMO��־��
    ADC_SetAnalogMonitor(ADC0, &amoConfig); //���üĴ���
    
}

void sampleValueDeal(void)
{
    int i;
    uint32_t sumValue = 0;
    sumValue = 0;
    for (i = 0; i < 4; i++)
    {
        sumValue += g_adcInjectValue[i];
    }
    g_injectAverageSampleValue = sumValue / 4;
    sumValue = 0;
    for (i = 0; i < DMA_TRANSFER_NUM; i++)
    {
        sumValue += g_ADCValueBuffer[i];
    }
    g_regularAverageSampleValue = sumValue / DMA_TRANSFER_NUM;
}
/**
 * ADC_SampleSoftwareTrigerADC
 *
 * @param[in] void
 * @return void
 *
 * @brief Timer��ʱ����������ADC_CHANNEL0���β�����
 */
void ADC_SampleSoftwareTrigerADC(void)
{
    ADC_init();
    
    while(1)
    {
        //ÿ��ת����������
        memset(g_ADCValueBuffer, 0x00, sizeof(g_ADCValueBuffer));
        memset(g_adcInjectValue, 0x00, sizeof(g_adcInjectValue));

        ADC_SoftwareStartRegularConvert(ADC0);  ///����������������
        ADC_SoftwareStartInjectConvert(ADC0);  ///����������������

        udelay(8);//��Ҫ����8��ͨ������ʱ8us�Ա�֤���ݲ������
        sampleValueDeal();
        printf("ADC regular group Sample value: %d\r\n", g_regularAverageSampleValue);
        switch(g_AMOFlag)
        {
            case 1:
                printf("analog monitor level trigger mode. g_AMOFlag: %d.\r\n", g_AMOFlag);
            break;
            case 2:
                printf("analog monitor edge trigger mode. g_AMOFlag: %d.\r\n", g_AMOFlag);
            break;
            case 3:
                printf("analog monitor edge trigger mode. g_AMOFlag: %d.\r\n", g_AMOFlag);
            break;
            default:
                printf("analog monitor status: %d\r\n", g_AMOFlag);
            break;
        }
        
        mdelay(2000);
    }
}

/**********<End>*********/
