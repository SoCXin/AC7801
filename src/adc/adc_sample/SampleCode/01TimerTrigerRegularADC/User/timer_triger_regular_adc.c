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
uint32_t g_averageSampleValue = 0;
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
 * CTU_Config
 *
 * @param[in] void
 * @return void
 *
 * @brief 配置CTU模块，Timer0触发ADC规则组采样。
 */
void CTU_Config(void)
{
    CTU_ConfigType ctuConfig;
    memset(&ctuConfig, 0x00, sizeof(ctuConfig));
    ctuConfig.uart0RxFilterEn = DISABLE;  //去能UART0_RX滤波
    ctuConfig.rtcCaptureEn = DISABLE;  //去能RTC捕获
    ctuConfig.acmpCaptureEn = DISABLE;  //去能ACMP捕获
    ctuConfig.uart0RxCaptureEn = DISABLE;  //去能UART0_RX捕获
    ctuConfig.uartTxModulateEn = DISABLE;  //去能UART0_TX调制
    ctuConfig.clkPsc = CTU_CLK_PRESCALER_1;  //分频
    ctuConfig.adcRegularTriggerSource = CTU_TRIGGER_ADC_TIMER_CH0_OVERFLOW; //Timer0触发ADC规则组采样。
    ctuConfig.delay0Time = 0;  //触发延迟
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
 * @brief TIMER中断回调函数
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
 * @brief TIMER0初始化
 */
void TIMER0_Init(void)
{
    TIMER_ConfigType timerConfig = {0};
    timerConfig.periodValue = Delay5ms;//5ms定时
    timerConfig.interruptEn = ENABLE;//使能中断
    timerConfig.linkModeEn = DISABLE;//级联模式去能
    timerConfig.callBack = TIMER_Callback;//设置中断回调函数
    timerConfig.timerEn = ENABLE;//使能定时器
    TIMER_Init(TIMER_CHANNEL0, &timerConfig);
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
    adcConfig->EOCInterruptEn = DISABLE;              //EOC中断去能
    adcConfig->IEOCInterruptEn = DISABLE;             //IEOC中断去能
    adcConfig->interruptEn = DISABLE;                 //去能中断
    adcConfig->regularDMAEn = ENABLE;                 //使能ADC DMA
    adcConfig->regularTriggerMode = ADC_TRIGGER_EXTERNAL;//ADC触发源，外部触发
    adcConfig->injectTriggerMode = ADC_TRIGGER_INTERNAL; //ADC触发源，内部触发
    adcConfig->regularSequenceLength = 1;               //规则组长度设为1
    adcConfig->injectSequenceLength = 0;               //注入组长度设为0
    adcConfig->dataAlign = ADC_DATA_ALIGN_RIGHT;       //右对齐
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
    ADC_SetRegularGroupChannel(ADC0, ADC_CH_8, ADC_SPT_CLK_7, 0);  //采样&转换时间= (7+12)/24000000 + 5/24000000 = 1us
    ADC_DMAInit();   //ADC DMA初始化
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
 * @brief Timer定时触发规则组ADC_CHANNEL0单次采样。
 */
void ADC_SampleTimerTrigerRegular(void)
{
    CTU_Config();
    ADC_init();
    TIMER0_Init();
    
    ///<开始ADC规则组转换，转换值根据规则组通道顺序存储在g_ADCValueBuffer数组中。
    while(1)
    {
        //nothing todo 
        mdelay(500);
        sampleValueDeal();
        printf("Timer Triger ADC Regular Sample value: %d\r\n", g_averageSampleValue);
    }
}

/**********<End>*********/
