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
uint8_t g_dmaFinish = 0;//DMA传输完成
uint8_t g_halfDmaFinish = 0; //DMA传输半完成
uint8_t g_dmaTransError = 0; //DMA传输错误
uint32_t g_ADCValueBuffer[DMA_TRANSFER_NUM + 1] = {0};
uint32_t g_timerCnt = 0;
uint16_t g_regularAverageSampleValue = 0; //规则组采样平均值
uint16_t g_injectAverageSampleValue = 0; //注入组采样平均值
uint16_t g_adcInjectValue[4];
uint8_t g_AMOErrorFlag = 0;//模拟看门狗异常事件标志
uint8_t g_AMORecoveryFlag = 0;//模拟看门狗恢复事件标志
uint8_t g_AMOFlag = 0;
/*
注意：EOC标志写0或读取ADC_RDR都会清除该标志位。
在进行debug时，如果有打开memory窗口或打开ADC寄存器。
该标志会被debug清除。
*/
uint8_t g_EOCFlag = 0; //规则组转换结束标志。
uint8_t g_IEOCFlag = 0; //注入组转换结束标志。
/**
 * ADC_DMACallback
 *
 * @param[in] void
 * @return void
 *
 * @brief ADC DMA中断回调函数。
 */
void ADC_Callback(void *device, uint32_t wpara, uint32_t lpara)
{   
    if (wpara & ADC_STR_IEOC_Msk)    //注入组中断标志
    {
        g_IEOCFlag = 1;
        g_adcInjectValue[0] = ADC_GetInjectData(ADC0, 0);
        g_adcInjectValue[1] = ADC_GetInjectData(ADC0, 1);
        g_adcInjectValue[2] = ADC_GetInjectData(ADC0, 2);
        g_adcInjectValue[3] = ADC_GetInjectData(ADC0, 3);
    }
    if (wpara & ADC_STR_EOC_Msk)    //规则组中断标志
    {
        g_EOCFlag = 1;
    }
    if (wpara & ADC_STR_AMO_Msk)    //监控通道电压大于高阀值或小于低阀值，AMO标志位置位（电平触发模式有效）。
    {
        g_AMOFlag = 1;
    }
    if (wpara & ADC_STR_AAMO_Msk)   //监控通道电压从正常区域到异常区域，AAMO标志位置位（边沿触发模式有效）。
    {
        g_AMOFlag = 2;
    }
    if (wpara & ADC_STR_NAMO_Msk)    //监控通道电压从异常区域到正常区域，NAMO标志位置位（边沿触发模式有效）。
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
 * @brief ADC DMA中断回调函数。
 */

void ADC_DMACallback(void *device, uint32_t wpara, uint32_t lpara)
{
    /*
     wparam为DMA通道状态，状态含义可参考CHANNELx_STATUS寄存器，
     CHANNELx_STATUS[2] 传输错误
     CHANNELx_STATUS[1] 半传输完成（相对设置的transferNum，如果半传输中断有使能，transferNum设为6，则DATA_TRANS_NUM为3时产生中断，进入回调）
     CHANNELx_STATUS[0] 传输完成
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
 * @brief ADC DMA初始化，配置DMA相关参数。
 */
void ADC_DMAInit(void)
{
    uint32_t tmpMemStartAddr = (uint32_t)&g_ADCValueBuffer[0];
    uint32_t tmpMemEndAddr = (uint32_t)&g_ADCValueBuffer[DMA_TRANSFER_NUM + 1]; ///<Setting memory DMA address
    DMA_ConfigType tmpDMAConfig;
    memset(&tmpDMAConfig, 0x00, sizeof(DMA_ConfigType));
    
    tmpDMAConfig.memStartAddr = tmpMemStartAddr; //设置DMA开始地址
    tmpDMAConfig.memEndAddr = tmpMemEndAddr;//设置DMA结束地址
    tmpDMAConfig.periphStartAddr = (uint32_t)(&(ADC0->RDR)); ///<Move ADC DR to memory
    tmpDMAConfig.channelEn = ENABLE;                     ///<使能DMAx通道
    tmpDMAConfig.finishInterruptEn = ENABLE;                   ///<使能DMA传输完成中断
    tmpDMAConfig.halfFinishInterruptEn = DISABLE;               ///<去能DMA半传输完成中断
    tmpDMAConfig.errorInterruptEn = ENABLE;                    ///<使能DMA传输错误中断
    tmpDMAConfig.channelPriority = DMA_PRIORITY_VERY_HIGH;///<设置DMA通道优先级，0~3 ：优先级由低到高
    tmpDMAConfig.circular = ENABLE;                      ///<使能循环模式，如果只想工作一次，设为0即可。
    tmpDMAConfig.direction = DMA_READ_FROM_PERIPH;          ///<0: 从外设读取，1：从存储器读取
    tmpDMAConfig.MEM2MEM = DISABLE;                     ///<0：在非存储器与存储器之间传输，1：在存储器与存储器之间传输
    tmpDMAConfig.memByteMode = DMA_MEM_BYTE_MODE_1TIME;  ///<MEM字分割传输数，0:32-bit,1:16-bit[15:0]; 2:16-bit[23:16][7:0];3:8-bit。详情可参考AC781X芯片手册  表20-2 可编程数据宽度&数据对齐 
    tmpDMAConfig.memIncrement = ENABLE;                  ///<1:MEM地址增加
    tmpDMAConfig.periphIncrement = DISABLE;               ///<0：外设地址固定
    tmpDMAConfig.memSize = DMA_MEM_SIZE_32BIT;           ///<0:8-bit,1:16-bit,2:32-bit
    tmpDMAConfig.periphSize = DMA_PERIPH_SIZE_16BIT;     ///<0:8-bit,1:16-bit,2:32-bit
    tmpDMAConfig.transferNum = DMA_TRANSFER_NUM;         ///<DMA通道传输长度
    tmpDMAConfig.periphSelect = DMA_PEPIRH_ADC0;  //外设选择
    tmpDMAConfig.callBack = ADC_DMACallback; ///<设置DMA中断回调

    DMA_Init(DMA0_CHANNEL0, &tmpDMAConfig);                     ///<ADC 使用DMA1通道，每个模块对应的DMA通道，可参考 AC781X芯片手册 表20-1 DMA请求列表
    NVIC_EnableIRQ(DMA0_CHANNEL0_IRQn);                              ///<使能DMA1中断请求
}


/**
 * ADC_init
 *
 * @param[in] void
 * @return void
 *
 * @brief 初始化ADC,配置ADC参数。
 */
void ADC_init()
{
    ADC_ConfigType tempAdcConfig;
    ADC_AMOConfigType amoConfig;
    ADC_ConfigType* adcConfig;
    adcConfig = &tempAdcConfig;
    //配置PINMUX
    GPIO_SetFunc(GPIOA, GPIO_PIN2, GPIO_FUN2);///<ADC_IN8 Analog function enable
    adcConfig->clkPsc = ADC_CLK_PRESCALER_1; ///<Set ADC Clk = 24M/2/(0+1)
    adcConfig->scanModeEn = ENABLE;                   //扫描模式
    adcConfig->continousModeEn = DISABLE;             //连续模式
    adcConfig->regularDiscontinousModeEn = DISABLE;   //1：打开规则组间断转换模式
    adcConfig->injectDiscontinousModeEn = DISABLE;    //1：打开注入组间断转换模式
    adcConfig->injectAutoModeEn = DISABLE;            //1：自动注入模式
    adcConfig->intervalModeEn = DISABLE;              //1：注入组为间隔转换模式
    adcConfig->regularDiscontinousNum = 0;            //
    adcConfig->EOCInterruptEn = ENABLE;              //EOC中断使能
    adcConfig->IEOCInterruptEn = ENABLE;             //IEOC中断使能
    adcConfig->interruptEn = ENABLE;                 //中断使能
    adcConfig->regularDMAEn = ENABLE;                 //使能ADC DMA
    adcConfig->regularTriggerMode = ADC_TRIGGER_INTERNAL;//ADC触发源，内部触发
    adcConfig->injectTriggerMode = ADC_TRIGGER_INTERNAL; //ADC触发源，内部触发
    adcConfig->regularSequenceLength = 4;               //规则组长度设为1
    adcConfig->injectSequenceLength = 4;               //注入组长度设为4
    adcConfig->dataAlign = ADC_DATA_ALIGN_RIGHT;       //右对齐
    adcConfig->callBack =  ADC_Callback;               //回调
    adcConfig->powerMode = ADC_POWER_ON;               //上电
    ADC_Init(ADC0, adcConfig);                      ///<ADC works Mode Config
    /*
        ADC转换率计算公式：
        转换时间= 采样时间+转换时间+同步时间
        转换时间= (SPT+12)/ADC模块时钟频率+5/APB时钟频率
        备注：
        1.同步时间为5个APB CLK。
        2.ADC时钟频率 = APB时钟频率 /(分频系数+1)
	*/
    //规则组通道设置
    ADC_SetRegularGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 0);  //采样&转换时间= (7+12)/24000000 + 5/24000000 = 1us
    ADC_SetRegularGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 1);  //采样&转换时间= (7+12)/24000000 + 5/24000000 = 1us
    ADC_SetRegularGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 2);  //采样&转换时间= (7+12)/24000000 + 5/24000000 = 1us
    ADC_SetRegularGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 3);  //采样&转换时间= (7+12)/24000000 + 5/24000000 = 1us
    //注入组通道设置
    ADC_SetInjectGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 0);  //采样&转换时间= (7+12)/24000000 + 5/24000000 = 1us
    ADC_SetInjectGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 1);  //采样&转换时间= (7+12)/24000000 + 5/24000000 = 1us
    ADC_SetInjectGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 2);  //采样&转换时间= (7+12)/24000000 + 5/24000000 = 1us
    ADC_SetInjectGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 3);  //采样&转换时间= (7+12)/24000000 + 5/24000000 = 1us
    
    //规则组DMA初始化
    ADC_DMAInit();   //ADC DMA初始化
    //模拟监控配置 
    amoConfig.AMOTriggerMode = ADC_AMO_TRIGGER_EDGE;//边沿触发模式，监控异常和恢复
    amoConfig.AMOInterruptEn = ENABLE; //AMO中断使能
    amoConfig.AMORegularEn = ENABLE; //规则组通道监控使能
    amoConfig.AMOInjectEn = DISABLE; //注入组通道监控去能
    amoConfig.AMOSingleModeEn = ENABLE; //单通道使能
    amoConfig.AMOSingleChannel = ADC_CH_8;//监控通道8
    amoConfig.AMOHighThreshold = AMOHighValue; //监控电压高阀值，大于该值会置位AMO标志（电平触发模式），或置位AAMO标志（边沿触发模式）。
    amoConfig.AMOLowThreshold = AMOLowValue; //监控电压低阀值，低于该值会置位AMO标志（电平触发模式），或置位NAMO标志（边沿触发模式）。
    amoConfig.AMOHighOffset = AMOHighOffsetValue;//高阀值电压恢复偏移值；恢复边界值= 高阀值-高偏移值，当电压低于高恢复边界值，会置位NAMO标志。
    amoConfig.AMOLowOffset  = AMOLowOffsetValue;//低阀值电压恢复偏移值；恢复边界值= 低阀值+低偏移值，当电压高于低恢复边界值，会置位NAMO标志。
    ADC_SetAnalogMonitor(ADC0, &amoConfig); //配置寄存器
    
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
 * @brief Timer定时触发规则组ADC_CHANNEL0单次采样。
 */
void ADC_SampleSoftwareTrigerADC(void)
{
    ADC_init();
    
    while(1)
    {
        //每次转换数据清零
        memset(g_ADCValueBuffer, 0x00, sizeof(g_ADCValueBuffer));
        memset(g_adcInjectValue, 0x00, sizeof(g_adcInjectValue));

        ADC_SoftwareStartRegularConvert(ADC0);  ///软件触发规则组采样
        ADC_SoftwareStartInjectConvert(ADC0);  ///软件触发规则组采样

        udelay(8);//需要采样8个通道，延时8us以保证数据采样完成
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
