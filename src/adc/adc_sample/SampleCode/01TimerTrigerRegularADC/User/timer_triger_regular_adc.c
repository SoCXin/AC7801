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
uint32_t g_averageSampleValue = 0;
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
    ctuConfig.adcRegularTriggerSource = CTU_TRIGGER_ADC_TIMER_CH0_OVERFLOW; //Timer0����ADC�����������
    ctuConfig.delay0Time = 0;  //�����ӳ�
//    ctuConfig.adcInjectTriggerSource = CTU_TRIGGER_ADC_PWM0_INIT;  // 
//    ctuConfig.delay1Time = 0;
//    ctuConfig.pwdt0In3Source = CTU_PWDT_IN3_SOURCE_UART0_RX;
//    ctuConfig.pwdt1In3Source = CTU_PWDT_IN3_SOURCE_UART0_RX; 
    CTU_Init(&ctuConfig);
}
/**
 * TIMER_Callback
 *
 * @param[in] timerIndex
 * @return void
 *
 * @brief TIMER�жϻص�����
 */
void TIMER_Callback(void *device, uint32_t wpara, uint32_t lpara)
{
    if (TIMER_CHANNEL0 == device)
    {
        g_timerCnt++;
    }
}
/**
 * TIMER0_Init
 *
 * @param[in] void
 * @return void
 *
 * @brief TIMER0��ʼ��
 */
void TIMER0_Init(void)
{
    TIMER_ConfigType timerConfig = {0};
    timerConfig.periodValue = Delay5ms;//5ms��ʱ
    timerConfig.interruptEn = ENABLE;//ʹ���ж�
    timerConfig.linkModeEn = DISABLE;//����ģʽȥ��
    timerConfig.callBack = TIMER_Callback;//�����жϻص�����
    timerConfig.timerEn = ENABLE;//ʹ�ܶ�ʱ��
    TIMER_Init(TIMER_CHANNEL0, &timerConfig);
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
    adcConfig->EOCInterruptEn = DISABLE;              //EOC�ж�ȥ��
    adcConfig->IEOCInterruptEn = DISABLE;             //IEOC�ж�ȥ��
    adcConfig->interruptEn = DISABLE;                 //ȥ���ж�
    adcConfig->regularDMAEn = ENABLE;                 //ʹ��ADC DMA
    adcConfig->regularTriggerMode = ADC_TRIGGER_EXTERNAL;//ADC����Դ���ⲿ����
    adcConfig->injectTriggerMode = ADC_TRIGGER_INTERNAL; //ADC����Դ���ڲ�����
    adcConfig->regularSequenceLength = 1;               //�����鳤����Ϊ1
    adcConfig->injectSequenceLength = 0;               //ע���鳤����Ϊ0
    adcConfig->dataAlign = ADC_DATA_ALIGN_RIGHT;       //�Ҷ���
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
    ADC_SetRegularGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 0);  //����&ת��ʱ��= (7+12)/24000000 + 5/24000000 = 1us
    ADC_DMAInit();   //ADC DMA��ʼ��
}

void sampleValueDeal(void)
{
    int i;
    uint32_t sumValue = 0;
    for (i = 0; i < DMA_TRANSFER_NUM; i++)
    {
        sumValue += g_ADCValueBuffer[i];
    }
    g_averageSampleValue = sumValue / DMA_TRANSFER_NUM;
    

}
/**
 * ADC_SampleTimerTrigerRegular
 *
 * @param[in] void
 * @return void
 *
 * @brief Timer��ʱ����������ADC_CHANNEL0���β�����
 */
void ADC_SampleTimerTrigerRegular(void)
{
    CTU_Config();
    ADC_init();
    TIMER0_Init();
    
    ///<��ʼADC������ת����ת��ֵ���ݹ�����ͨ��˳��洢��g_ADCValueBuffer�����С�
    while(1)
    {
        //nothing todo 
        mdelay(500);
        sampleValueDeal();
        printf("Timer Triger ADC Regular Sample value: %d\r\n", g_averageSampleValue);
    }
}

/**********<End>*********/
