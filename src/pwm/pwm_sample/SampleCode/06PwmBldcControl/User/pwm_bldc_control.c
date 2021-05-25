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
#include "ac780x_gpio.h"
#include "ac780x.h"
#include "ac780x_timer.h"
#include "ac780x_pwm.h"
#include "pwm_sample.h"

/**********<Macro>************/
#define MAX_DUTY_CNT                    (4096)
#define ratio_q12(n)                    (int16_t)(n * MAX_DUTY_CNT / 100)
#define PWM_AH                          GPIO_PB8 //PWM1_CH2
#define PWM_AL                          GPIO_PB7 //PWM1_CH3
#define PWM_BH                          GPIO_PB10 //PWM1_CH4
#define PWM_BL                          GPIO_PB9 //PWM1_CH5
#define PWM_CH                          GPIO_PB5 //PWM1_CH0
#define PWM_CL                          GPIO_PB4 //PWM1_CH1
#define CUR_FAULT_DETECT                GPIO_PB6 //PWM1_FLT0
/**********<Union>***********/
typedef enum {      ///<Define BLDC 3 phase
    PHASEA = 0,     ///<phase A
    PHASEB,         ///<phase B
    PHASEC,         ///<phase C
    PHASE_NUM,
}MOTOR_PHASE;
/**********<Struct>*********/

/**********<Variable>*******/
uint16_t g_startupPwmRatio = ratio_q12(15);
uint16_t g_startupDelay = 0;
static PWM_ChannelType const s_motorPhaseCtrlMap [PHASE_NUM][2] =
{
    {PWM_CH_0, PWM_CH_1},
    {PWM_CH_2, PWM_CH_3},
    {PWM_CH_4, PWM_CH_5}
};
/**********<Prototype>********/
void BLDC_Stop(void);

/**
* BLDC_GPIO_Init
*
* @param[in] none
*
* @return none
*
* @brief ��ʼ��BLDC gpio
*
*/
void BLDC_GPIO_Init(void)
{
    GPIO_SetFunc(GPIOB, GPIO_PIN5, GPIO_FUN1);//PWM1_CH0 
    GPIO_SetFunc(GPIOB, GPIO_PIN4, GPIO_FUN1);//PWM1_CH1
    GPIO_SetFunc(GPIOB, GPIO_PIN8,GPIO_FUN1);//PWM1_CH2 
    GPIO_SetFunc(GPIOB, GPIO_PIN7, GPIO_FUN1);//PWM1_CH3
    GPIO_SetFunc(GPIOB, GPIO_PIN10, GPIO_FUN1);//PWM1_CH4 
    GPIO_SetFunc(GPIOB, GPIO_PIN9, GPIO_FUN1);//PWM1_CH5
    //trap
    GPIO_SetFunc(GPIOB, GPIO_PIN6, GPIO_FUN2);//PWM1_FLT0
} 
/**
* BLDC_ChangeMotorPhaseUseTrigMode
*
* @param[in] None
*
* @return none
*
* @brief Change BLDC PWM Phase using Trig Mode
*
*/
void BLDC_ChangeMotorPhaseUseTrigMode(void)
{
    PWM_SetSync(PWM1, ENABLE);
    PWM1->MODESEL |= PWM_MODESEL_PAIR0SYNCEN_Msk | PWM_MODESEL_PAIR1SYNCEN_Msk | PWM_MODESEL_PAIR2SYNCEN_Msk; ////Enable PWM Sync
    PWM1->SYNCONF |= PWM_SYNCONF_SWVSWSYNC_Msk |  PWM_SYNCONF_SYNCMODE_Msk | PWM_SYNCONF_SWOC_Msk;
    PWM1->SYNC |= PWM_SYNC_OMSYNCP_Msk;
    PWM1->SYNCONF |= PWM_SYNCONF_OMVSWSYNC_Msk |  PWM_SYNCONF_SYNCMODE_Msk ;
    PWM1->SYNCONF |= PWM_SYNCONF_CNTVSWSYNC_Msk | PWM_SYNCONF_PWMSVSWSYNC_Msk;  ////SW Trig MCVR and CHnV
}
/**
* BLDC_ClearAllPWMOutput
*
* @param[in] None
*
* @return none
*
* @brief Set ALL BLDC PWM Output to Low through Software Control
*
*/
void BLDC_ClearAllPWMOutput(void)
{
    PWM_SoftControlChannelLevel(PWM1, PWM_CH_0, PWM_LOW_LEVEL, ENABLE);
    PWM_SoftControlChannelLevel(PWM1, PWM_CH_1, PWM_LOW_LEVEL, ENABLE);
    PWM_SoftControlChannelLevel(PWM1, PWM_CH_2, PWM_LOW_LEVEL, ENABLE);
    PWM_SoftControlChannelLevel(PWM1, PWM_CH_3, PWM_LOW_LEVEL, ENABLE);
    PWM_SoftControlChannelLevel(PWM1, PWM_CH_4, PWM_LOW_LEVEL, ENABLE);
    PWM_SoftControlChannelLevel(PWM1, PWM_CH_5, PWM_LOW_LEVEL, ENABLE);
}
/**
* BLDC_PwmUpdate
*
* @param[in] positivePhase: positive phase, refer to enum BLDC_PHASE
* @param[in] negativePhase: negative phase, refer to enum BLDC_PHASE
*
* @return none
*
* @brief Set BLDC PWM Output phase
*
*/
void BLDC_PwmUpdate(uint8_t positivePhase, uint8_t negativePhase)
{
    uint8_t i = 0;
    BLDC_Stop();
    BLDC_ClearAllPWMOutput();
    //ʹ��PWM_ON��ʽ���ƣ����ϰ���PWM���ƣ��°���һֱ��ͨ
    for (i = 0; i < PHASE_NUM; i++)
    {
        if (i == positivePhase)
        {
            PWM_SetChannelSoftControl(PWM1, s_motorPhaseCtrlMap[positivePhase][0], DISABLE);
            PWM_SetChannelOutputMask(PWM1, s_motorPhaseCtrlMap[positivePhase][0], DISABLE);
            PWM_SetChannelOutputMask(PWM1, s_motorPhaseCtrlMap[positivePhase][1], ENABLE);
        }
        if (i == negativePhase)
        {
            PWM_SetChannelOutputMask(PWM1, s_motorPhaseCtrlMap[negativePhase][0], ENABLE);
             //�ϰ��Źرգ��°���ȫͨ
            PWM_SoftControlChannelLevel(PWM1, s_motorPhaseCtrlMap[negativePhase][0], PWM_LOW_LEVEL, ENABLE);
            PWM_SoftControlChannelLevel(PWM1, s_motorPhaseCtrlMap[negativePhase][1], PWM_HIGH_LEVEL, ENABLE);
            PWM_SetChannelOutputMask(PWM1, s_motorPhaseCtrlMap[negativePhase][1], DISABLE); 
        }
    }
    PWM_SetSoftwareTriggerSync(PWM1, ENABLE);
    
}
/**
* BLDC_Stop
*
* @param[in] none
*
* @return none
*
* @brief Motor Stop Code
*
*/
void BLDC_Stop(void)
{
    //Mask ALL BLDC PWM Output
    PWM_SetChannelOutputMask(PWM1, PWM_CH_0, ENABLE);
    PWM_SetChannelOutputMask(PWM1, PWM_CH_1, ENABLE);
    PWM_SetChannelOutputMask(PWM1, PWM_CH_2, ENABLE);
    PWM_SetChannelOutputMask(PWM1, PWM_CH_3, ENABLE);
    PWM_SetChannelOutputMask(PWM1, PWM_CH_4, ENABLE);
    PWM_SetChannelOutputMask(PWM1, PWM_CH_5, ENABLE);
}
/**
* BLDC_SetMotorSpeed
*
* @param[in] motorSpeed: set Motor Speed(from 0 to 4096)
*
* @return none
*
* @brief Set Motor Speed
*
*/
void BLDC_SetMotorSpeed(uint16_t motorSpeed)
{
    uint16_t pwmCnt = 0;
    uint16_t modulo = PWM1->MCVR;
    pwmCnt = motorSpeed * (modulo + 1) / MAX_DUTY_CNT;
    PWM1->CHANNELS[0].CHV = (modulo + 1 - pwmCnt) >> 1;
    PWM1->CHANNELS[1].CHV = (modulo + 1 + pwmCnt) >> 1;
    PWM1->CHANNELS[2].CHV = (modulo + 1 - pwmCnt) >> 1;
    PWM1->CHANNELS[3].CHV = (modulo + 1 + pwmCnt) >> 1;
    PWM1->CHANNELS[4].CHV = (modulo + 1 - pwmCnt) >> 1;
    PWM1->CHANNELS[5].CHV = (modulo + 1 + pwmCnt) >> 1;
    PWM_SetSoftwareTriggerSync(PWM1, ENABLE);
}
/**
* BLDC_PWM_FaultCallback
*
* @param[in]   none
* @return none
*
* @brief PWMģ���жϴ�����
*
*/
void BLDC_PWM_FaultCallback(void *device, uint32_t wpara, uint32_t lpara)
{
    if (PWM_GetFaultChannelFlag(PWM1, PWM_FAULT_CH_1))
    {

        PWM_ClearFaultChannelFlag(PWM1, PWM_FAULT_CH_1);
    }
}
/**
* BLDC_PWM_Output
*
* @param[in] enable: 1,enable PWM Output; 0: Disable PWM Output
*
* @return none
*
* @brief ����PWM�������ʱPWMģ����Ȼ������������ֻ��PWMδ����������ϡ�
*
*/
void BLDC_PWM_Output(uint8_t enable)
{
    if (enable)
    {
        PWM_SetChannelOutputMask(PWM1, PWM_CH_0, DISABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_1, DISABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_2, DISABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_3, DISABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_4, DISABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_5, DISABLE);   
    }
    else
    {
        PWM_SetChannelOutputMask(PWM1, PWM_CH_0, ENABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_1, ENABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_2, ENABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_3, ENABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_4, ENABLE);
        PWM_SetChannelOutputMask(PWM1, PWM_CH_5, ENABLE);       
    }
    PWM_SetSoftwareTriggerSync(PWM1, ENABLE);
}
/**
* BLDC_PWM_Config_COMP_Mode
*
* @param[in] none
*
* @return none
*
* @brief ʹ�ܡ�ȥ��PWM1����ģʽ
*
*/
void BLDC_PWM_Config_COMP_Mode(uint8_t enable)
{
    if (enable)
    {
        PWM1->MODESEL |= PWM_MODESEL_PAIR0COMPEN_Msk | PWM_MODESEL_PAIR1COMPEN_Msk | PWM_MODESEL_PAIR2COMPEN_Msk ;        
    }
    else
    {
        PWM1->MODESEL &= ~PWM_MODESEL_PAIR0COMPEN_Msk;
        PWM1->MODESEL &= ~PWM_MODESEL_PAIR1COMPEN_Msk;
        PWM1->MODESEL &= ~PWM_MODESEL_PAIR2COMPEN_Msk;         
    }
}
/**
* BLDC_PWM_Init
* 
* @param[in] none
* @return none
*
* @brief ����PWM
*
*/
void BLDC_PWM_Init(void)
{
    PWM_CombineChConfig combineChConfig[4]; //���ģʽ��ؽṹ��
    PWM_ModulationConfigType pwmConfig; //PWMģʽ��ؽṹ��
    PWM_ConfigType config; //PWMģ��ṹ��
    PWM_FaultConfig faultConfig;
    PWM_SyncConfigType syncConfig;
    //�ṹ����������
    memset(&combineChConfig, 0, sizeof(combineChConfig));
    memset(&pwmConfig, 0, sizeof(PWM_ModulationConfigType));
    memset(&faultConfig, 0, sizeof(PWM_FaultConfig));
    memset(&syncConfig, 0, sizeof(PWM_SyncConfigType));
    memset(&config, 0, sizeof(PWM_ConfigType));
    CKGEN_Enable(CLK_PWM1, ENABLE);
    /*ͨ��0/1������ģʽPWM���*/
    /*
    ���ϼ������ģʽ��
    ����=(MCVR-CNTIN+1)*PWM������ʱ������
    ռ�ձ�=|CH(n+1)V-CH(n)V|*PWM������ʱ������
    ����-���¼������ģʽ��
    ����=2*(MCVR-CNTIN)*PWM������ʱ������
    ռ�ձ�=2*(|CH(n+1)V-CH(n)V|)*PWM������ʱ������
    */
    combineChConfig[0].pairChannel = PWM_CH_0; //PWMͨ��������PWM_CH_0/2/4/6��ӦPAIR0/1/2/3
    combineChConfig[0].ch1stValue = 1; //ͨ��2n channelֵ��nΪPWM�������
    combineChConfig[0].ch2ndValue = 1; //ͨ��2n+1 channelֵ��nΪPWM�������
    combineChConfig[0].levelMode = PWM_HIGH_TRUE; //���PWM����Ч,���ռ�ձ���Ϊ25%����ָ�ĸ���Ч��ƽռ��25%
    combineChConfig[0].deadtimeEn = ENABLE;//��������ʹ�ܣ����ģʽ��֧����������
    combineChConfig[0].complementEn = ENABLE;//����ģʽʹ��,ʹ�ܺ�PWMͨ�����λ�����DISABLE�������ͬ��
    combineChConfig[0].ch1stMatchDir = PWM_MATCH_DIR_UP;//��������-���¼���(countModeΪPWM_UP_DOWN_COUNT)���ģʽ��Ч������ѡ��ƥ����Ч�㷽��
    combineChConfig[0].ch2ndMatchDir = PWM_MATCH_DIR_DOWN;//��������-���¼���(countModeΪPWM_UP_DOWN_COUNT)���ģʽ��Ч������ѡ��ƥ����Ч�㷽��
    combineChConfig[0].ch1stPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//������Ը���Ч��PWM mask��PWM����͵�ƽ
    combineChConfig[0].ch2ndPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//������Ը���Ч��PWM mask��PWM����͵�ƽ
    combineChConfig[0].ch1stInterruptEn = DISABLE;//PWMͨ��ƥ���ж�ʹ��λ
    combineChConfig[0].ch2ndInterruptEn = DISABLE;//PWMͨ��ƥ���ж�ʹ��λ
    combineChConfig[0].ch1stInitLevel = PWM_LOW_LEVEL;//PWM��ʼ��ƽ���Ϊ�ͣ���������initChOutputEn���ƣ�����PWM������δ����ǰPWM�ڵ������ƽֵ��
    combineChConfig[0].ch2ndInitLevel = PWM_LOW_LEVEL;//PWM��ʼ��ƽ���Ϊ�ͣ���������initChOutputEn���ƣ�����PWM������δ����ǰPWM�ڵ������ƽֵ��
    combineChConfig[0].ch1stTriggerEn = DISABLE;//ͨ��2n�ⲿ����ʹ�ܣ�nΪPWM�������
    combineChConfig[0].ch2ndTriggerEn = DISABLE;//ͨ��2n+1�ⲿ����ʹ�ܣ�nΪPWM�������

    /*channel 2/3*/
    combineChConfig[1].pairChannel = PWM_CH_2; //PWMͨ��������PWM_CH_0/2/4/6��ӦPAIR0/1/2/3
    combineChConfig[1].ch1stValue = 1; //ͨ��2n channelֵ��nΪPWM�������
    combineChConfig[1].ch2ndValue = 1; //ͨ��2n+1 channelֵ��nΪPWM�������
    combineChConfig[1].levelMode = PWM_HIGH_TRUE; //���PWM����Ч,���ռ�ձ���Ϊ25%����ָ�ĸ���Ч��ƽռ��25%
    combineChConfig[1].deadtimeEn = ENABLE;//��������ʹ�ܣ����ģʽ��֧����������
    combineChConfig[1].complementEn = ENABLE;//����ģʽʹ��,ʹ�ܺ�PWMͨ�����λ�����DISABLE�������ͬ��
    combineChConfig[1].ch1stMatchDir = PWM_MATCH_DIR_UP;//��������-���¼���(countModeΪPWM_UP_DOWN_COUNT)���ģʽ��Ч������ѡ��ƥ����Ч�㷽��
    combineChConfig[1].ch2ndMatchDir = PWM_MATCH_DIR_DOWN;//��������-���¼���(countModeΪPWM_UP_DOWN_COUNT)���ģʽ��Ч������ѡ��ƥ����Ч�㷽��
    combineChConfig[1].ch1stPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//������Ը���Ч��PWM mask��PWM����͵�ƽ
    combineChConfig[1].ch2ndPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//������Ը���Ч��PWM mask��PWM����͵�ƽ
    combineChConfig[1].ch1stInterruptEn = DISABLE;//PWMͨ��ƥ���ж�ʹ��λ
    combineChConfig[1].ch2ndInterruptEn = DISABLE;//PWMͨ��ƥ���ж�ʹ��λ
    combineChConfig[1].ch1stInitLevel = PWM_LOW_LEVEL;//PWM��ʼ��ƽ���Ϊ�ͣ���������initChOutputEn���ƣ�����PWM������δ����ǰPWM�ڵ������ƽֵ��
    combineChConfig[1].ch2ndInitLevel = PWM_LOW_LEVEL;//PWM��ʼ��ƽ���Ϊ�ͣ���������initChOutputEn���ƣ�����PWM������δ����ǰPWM�ڵ������ƽֵ��
    combineChConfig[1].ch1stTriggerEn = DISABLE;//ͨ��2n�ⲿ����ʹ�ܣ�nΪPWM�������
    combineChConfig[1].ch2ndTriggerEn = DISABLE;//ͨ��2n+1�ⲿ����ʹ�ܣ�nΪPWM�������
    /*channel 4/5*/
    combineChConfig[2].pairChannel = PWM_CH_4; //PWMͨ��������PWM_CH_0/2/4/6��ӦPAIR0/1/2/3
    combineChConfig[2].ch1stValue = 1; //ͨ��2n channelֵ��nΪPWM�������
    combineChConfig[2].ch2ndValue = 1; //ͨ��2n+1 channelֵ��nΪPWM�������
    combineChConfig[2].levelMode = PWM_HIGH_TRUE; //���PWM����Ч,���ռ�ձ���Ϊ25%����ָ�ĸ���Ч��ƽռ��25%
    combineChConfig[2].deadtimeEn = ENABLE;//��������ʹ�ܣ����ģʽ��֧����������
    combineChConfig[2].complementEn = ENABLE;//����ģʽʹ��,ʹ�ܺ�PWMͨ�����λ�����DISABLE�������ͬ��
    combineChConfig[2].ch1stMatchDir = PWM_MATCH_DIR_UP;//��������-���¼���(countModeΪPWM_UP_DOWN_COUNT)���ģʽ��Ч������ѡ��ƥ����Ч�㷽��
    combineChConfig[2].ch2ndMatchDir = PWM_MATCH_DIR_DOWN;//��������-���¼���(countModeΪPWM_UP_DOWN_COUNT)���ģʽ��Ч������ѡ��ƥ����Ч�㷽��
    combineChConfig[2].ch1stPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//������Ը���Ч��PWM mask��PWM����͵�ƽ
    combineChConfig[2].ch2ndPolarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH;//������Ը���Ч��PWM mask��PWM����͵�ƽ
    combineChConfig[2].ch1stInterruptEn = DISABLE;//PWMͨ��ƥ���ж�ʹ��λ
    combineChConfig[2].ch2ndInterruptEn = DISABLE;//PWMͨ��ƥ���ж�ʹ��λ
    combineChConfig[2].ch1stInitLevel = PWM_LOW_LEVEL;//PWM��ʼ��ƽ���Ϊ�ͣ���������initChOutputEn���ƣ�����PWM������δ����ǰPWM�ڵ������ƽֵ��
    combineChConfig[2].ch2ndInitLevel = PWM_LOW_LEVEL;//PWM��ʼ��ƽ���Ϊ�ͣ���������initChOutputEn���ƣ�����PWM������δ����ǰPWM�ڵ������ƽֵ��
    combineChConfig[2].ch1stTriggerEn = DISABLE;//ͨ��2n�ⲿ����ʹ�ܣ�nΪPWM�������
    combineChConfig[2].ch2ndTriggerEn = DISABLE;//ͨ��2n+1�ⲿ����ʹ�ܣ�nΪPWM�������

    /*modulation mode config*/
    pwmConfig.countMode = PWM_UP_COUNT; //PWM������ģʽ (��ͬ�ļ���ģʽƵ�ʼ�ռ�ձȼ��㷽ʽ��ͬ)
    pwmConfig.independentChannelNum = 0; //����ͨ����
    pwmConfig.combineChannelNum = 3; //��϶���
    pwmConfig.independentChConfig = NULL; //����ͨ�����ñ�����ַ��ֵ
    pwmConfig.combineChConfig = combineChConfig; //���ͨ�����ñ�����ַ��ֵ
    pwmConfig.deadtimePsc = PWM_DEADTIME_DIVID_1;//���������Ƶֵ����deadtimeһ���������������ʱ�䡣
    pwmConfig.deadtime = 24;  //����ʱ�� = (DTPSC * DTVAL)/PWM������ʱ������ = 1*24/24000000 = 1us
    pwmConfig.initChOutputEn = ENABLE; //ʹ�ܳ�ʼ��ͨ�������ʹ�ܺ����PWMģʽ��initLevel�����PWMģʽ��ch1stInitLevel��ch2ndPolarity���òŻ���Ч
    pwmConfig.initTriggerEn = DISABLE; //ͨ���ⲿ����ʹ��

    /*pwm config*/
    config.mode = PWM_MODE_MODULATION;//PWMģ������ΪPWMģʽ
    config.initModeStruct = &pwmConfig;//PWM���ýṹ���ַ��ֵ
    config.clkSource = PWM_CLK_SOURCE_NONE; //PWMʱ��Դ����
    config.clkPsc = PWM_PRES;//PWMʱ��Դ��Ƶ
    config.initValue = 0;//��������ʼ�Ĵ���ֵ
    config.maxValue = BLDC_PWM_PERIOD - 1; //PWM���������ֵ
    config.overflowInterrupEn = ENABLE;//����������ж�ʹ��
    config.cntOverflowFreq = 0;//CNTOF�жϲ�����Ƶ���������Ƶ�ʵĹ�ϵ(0-127), 0��ʾÿ�μ������������������жϣ�1��ʾ���1�Σ�2��ʾ���2�Σ��Դ����ơ�
    config.interruptEn = ENABLE; //PWM�ж�ʹ��
    config.callBack = BLDC_PWM_FaultCallback; //PWM�жϻص�
    PWM_Init(PWM1, &config); //���ó�ʼ����Ч

    /*pwm ͬ����������*/
    syncConfig.syncTriggerMode = PWM_SYNC_TRIGGER_SOFTWARE;  //�������ͬ��
    syncConfig.hardwareSync0En = DISABLE;//Ӳ��ͬ������Դ0ȥ��            
    syncConfig.hardwareSync1En = DISABLE;//Ӳ��ͬ������Դ1ȥ��                   
    syncConfig.hardwareSync2En = DISABLE;//Ӳ��ͬ������Դ2ȥ��                     
    syncConfig.autoClearHWTriggerEn = DISABLE; //
    syncConfig.counterInitSyncEn = ENABLE;//ʹ��CNTINͬ�� 
    syncConfig.outCtrlSyncEn = ENABLE;//ʹ��CHOSWCRͬ��
    syncConfig.inverterSyncEn = DISABLE;//ȥ��INVCRͬ��               
    syncConfig.outmaskSyncEn = ENABLE;//ʹ��OMCRͬ��               
    syncConfig.polaritySyncEn = DISABLE;//ȥ��CHOPOLCRͬ��                
    syncConfig.chValueSyncEn[0] = ENABLE; //ʹ��˫ͨ��CHVͬ��
    syncConfig.chValueSyncEn[1] = ENABLE; //ʹ��˫ͨ��CHVͬ��
    syncConfig.chValueSyncEn[2] = ENABLE; //ʹ��˫ͨ��CHVͬ��
    syncConfig.chValueSyncEn[3] = ENABLE; //ʹ��˫ͨ��CHVͬ��
    syncConfig.maxLoadingPointEn = DISABLE;//ȥ�������ص�
    syncConfig.minLoadingPointEn = DISABLE;//ȥ����С���ص�           
    syncConfig.syncEn = ENABLE;//ʹ��ͬ��                       
    PWM_InitSyncControl(PWM1, &syncConfig);//����ͬ����ؼĴ���
    
    /* PWM�������� */
    faultConfig.mode = PWM_FAULT_CTRL_MANUAL_ALL; //�ֶ��������
    faultConfig.channelConfig[PWM_FAULT_CH_1].faultInputEn = ENABLE;//ʹ��PWM_FLT1ͨ��
    faultConfig.channelConfig[PWM_FAULT_CH_1].faultFilterEn = ENABLE; //ȥ��PWM FLT1ͨ���˲�
    faultConfig.channelConfig[PWM_FAULT_CH_1].faultPolarity = PWM_INPUT_POLARITY_ACTIVE_LOW; //������ϵ���Ч��Ĭ��Ϊ�� 
    faultConfig.filterValue = 0;//�����˲���ֵ
    faultConfig.faultCtrlOutputEn[0] = ENABLE;//PWMͨ��CH0,CH1����ʹ��
    faultConfig.faultCtrlOutputEn[1] = ENABLE;//PWMͨ��CH2,CH3����ʹ��
    faultConfig.faultCtrlOutputEn[2] = ENABLE;//PWMͨ��CH4,CH5����ʹ��
    faultConfig.faultCtrlOutputEn[3] = ENABLE;//PWMͨ��CH6,CH7����ʹ��
    faultConfig.interruptEn = ENABLE;//�����ж�ʹ��
    PWM_InitFaultControl(PWM1, &faultConfig); //����PWM������ؼĴ���
    
    NVIC_SetPriority(PWM1_IRQn, 0);// ����PWM1�ж����ȼ�
    ///PWM Start
    PWM_SetClockSource(PWM1, PWM_CLK_SOURCE_APB); //����PWM������
}

/**
* BLDC_OpenKick
*
* @param[in] dir, motor running direction
*
* @return none
*
* @brief ������ʽ�õ������
*
*/
void BLDC_OpenKick(uint8_t dir)
{
    uint8_t flowState = 0;
    g_startupPwmRatio = ratio_q12(30);
    g_startupDelay = 10;
    do
    {
        BLDC_SetMotorSpeed(g_startupPwmRatio); 
        if (dir) //reverse
        {
            switch (flowState)
            {
            case 0:// A+,C-
                BLDC_PwmUpdate(PHASEA, PHASEC);
                flowState = 1;
            break;
            case 1: //A+,B-
                BLDC_PwmUpdate(PHASEA, PHASEB);
                flowState = 2;
            break;
            case 2:  //C+,B-
                BLDC_PwmUpdate(PHASEC, PHASEB);
                flowState = 3;
            break;
            case 3://C+,A-
                BLDC_PwmUpdate(PHASEC, PHASEA);
                flowState = 4;
            break;
            case 4: //B+,A-
                BLDC_PwmUpdate(PHASEB, PHASEA);
                flowState = 5;
            break;
            case 5://B+,C-
                BLDC_PwmUpdate(PHASEB, PHASEC);
                flowState = 0;
            break;
            default:
            break;
            }
        }
        else //forward
        {
            switch (flowState)
            {
            case 0:// A+,B-
                BLDC_PwmUpdate(PHASEA, PHASEB);
                flowState = 1;
            break;
            case 1: //A+,C-
                BLDC_PwmUpdate(PHASEA, PHASEC);
                flowState = 2;
            break;
            case 2:  //B+,C-
                BLDC_PwmUpdate(PHASEB, PHASEC);
                flowState = 3;
            break;
            case 3://B+,A-
                BLDC_PwmUpdate(PHASEB, PHASEA);
                flowState = 4;
            break;
            case 4: //C+,A-
                BLDC_PwmUpdate(PHASEC, PHASEA);
                flowState = 5;
            break;
            case 5://C+,B-
                BLDC_PwmUpdate(PHASEC, PHASEB);
                flowState = 0;
            break;
            default:
            break;
            }            
        }
    mdelay(g_startupDelay); 
    } while (1);
}  

/**
* PWM_SampleBldcControl
*
* @param[in]  None
* @return     None
*
* @brief �ڵ��demo���Ͽ������е��
          
*/
void PWM_SampleBldcControl(void)
{
    //��ʼ��PWM GPIO
    BLDC_GPIO_Init();
    //��ʼ��PWMģ��
    BLDC_PWM_Init();
    //ʹ�����������������е��
    BLDC_OpenKick(0);
}

/**********<End>*********/
