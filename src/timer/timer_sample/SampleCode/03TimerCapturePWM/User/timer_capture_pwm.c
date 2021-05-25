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
#include "string.h"
#include "ac780x_timer.h"
#include "ac780x_timer_reg.h"
#include "ac780x_gpio.h" 
#include "ac780x_pwm.h"  
#include "timer_sample.h"  

#define PRESCALER   				        (PWDT_CLK_PRESCALER_8)
#define APB_CLK                             (APB_BUS_FREQ)
#define FREQ                                (1000) ///freq = 1KHz
#define PWM_PRES                            (0)
#define MOD_PWM                             (APB_CLK / FREQ / (PWM_PRES + 1))

uint32_t PWM_Down_moment_One = 0;
uint32_t PWM_Down_moment_Two = 0;
uint32_t PWM_Up_moment = 0;
uint32_t PWM_Period = 0;
uint32_t PWM_Duty = 0;

/**
*
* @param[in] none
*
* @return none
*
* @brief TIM0_Task1
* TIMER0�жϻص�
*/
void TIM0_Task1(void *device, uint32_t wpara, uint32_t lpara)
{
  //�жϴ���
}

/**
*
* @param[in] none
*
* @return none
*
* @brief Timer0_Init
*��ʼ��TIMER0��ÿ100s����һ���ж�
*/
void Timer0_Init(void)
{
	TIMER_ConfigType  timerConfig0;
	
	memset(&timerConfig0, 0, sizeof(timerConfig0));
	
	/* configure TIMER channel 0, only enable timer */
    timerConfig0.periodValue      = DelayS(100);  ///100s timeout
    timerConfig0.interruptEn      = ENABLE;
    timerConfig0.linkModeEn        = DISABLE;
    timerConfig0.timerEn          = ENABLE;
	timerConfig0.callBack         = TIM0_Task1;
	
    TIMER_Init(TIMER_CHANNEL0, &timerConfig0);		
}

/**
*
* @param[in] none
*
* @return none
*
* @brief 
*��������ʱ�����½����жϣ��������㣻�����ɿ������������жϣ�����������g_pressingTime��ֵ����ͨ������۲�ñ���
*/
void PWM_Capture_ByTimer(void *device, uint32_t wpara, uint32_t lpara)
{
	static uint8_t PMW_Edg_Count = 0;
	
	if(PMW_Edg_Count  == 0)
	{
		if (GPIO_GetPinLevel(GPIOA,GPIO_PIN3) == 0)//PWM�½���
		{
			PWM_Down_moment_One = TIMER_GetCurrentValue(TIMER_CHANNEL0);
			PMW_Edg_Count = 1;
		}		
	}	
	else if(PMW_Edg_Count  == 1)
	{
		if (GPIO_GetPinLevel(GPIOA,GPIO_PIN3) == 1)//PWM������
		{
			PWM_Up_moment = TIMER_GetCurrentValue(TIMER_CHANNEL0);
			PMW_Edg_Count = 2;
		}		
	}	
	else if(PMW_Edg_Count  == 2)
	{
		if(GPIO_GetPinLevel(GPIOA,GPIO_PIN3) == 0)//PWM�½���
		{
			PWM_Down_moment_Two = TIMER_GetCurrentValue(TIMER_CHANNEL0);	
			PMW_Edg_Count = 0;
		}		
	}
	
	if((PWM_Up_moment < PWM_Down_moment_One)&&(PWM_Down_moment_Two < PWM_Up_moment)) //TimerΪ���¼���
	{
		PWM_Period = (PWM_Down_moment_One - PWM_Down_moment_Two)/24;   //24������Ϊ1us
		PWM_Duty = (100 * (PWM_Up_moment - PWM_Down_moment_Two))/(PWM_Down_moment_One - PWM_Down_moment_Two);	
	}
		
}

/**
*
* @param[in] none
*
* @return none
*
* @brief 
*��ʼ��PWM�����
*/
void Capture_Gpio_Init(void)
{
	GPIO_SetDir(GPIOA,GPIO_PIN3,GPIO_IN);   //ʹ�ð���1����Ϊ����
	
	GPIO_EnableExtInterrupt(GPIOA,GPIO_PIN3, EXTI_TRIGGER_RISING_FALLING);//ʹ���ⲿ�ж�,�������½��ض��ж�
	
	GPIO_SetCallback(GPIO_PIN3, PWM_Capture_ByTimer);//�����жϻص�����
}

/**
*
* @param[in] none
*
* @return none
*
* @brief 
*PMW���Ƶ��Ϊ1K��ռ�ձ�Ϊ25%
*/
void PWM_Generate(void)
{
    GPIO_SetFunc(GPIOB, GPIO_PIN3, GPIO_FUN2);//PWM1_CH7
	
	PWM_IndependentChConfig independentChConfig[1];//����ģʽ��ؽṹ��
	PWM_ModulationConfigType pwmConfig; //PWMģʽ��ؽṹ��
    PWM_ConfigType config; //PWMģ��ṹ��
	
	memset(&independentChConfig, 0, sizeof(independentChConfig));
	memset(&pwmConfig, 0, sizeof(pwmConfig));
    memset(&config, 0, sizeof(config));
	
	independentChConfig[0].channel = PWM_CH_7; //ͨ��7
    independentChConfig[0].chValue = MOD_PWM >> 2;//ͨ��7 channelֵ�����ռ�ձ� = chValue / maxValue = 25%
    independentChConfig[0].levelMode = PWM_HIGH_TRUE; //���PWM����Ч
    independentChConfig[0].polarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH; //������Ը���Ч��PWM mask��PWM����͵�ƽ
    independentChConfig[0].interruptEn = DISABLE;//PWMͨ��ƥ���ж�ʹ��λ
    independentChConfig[0].initLevel = PWM_LOW_LEVEL;//PWM��ʼ��ƽ���Ϊ�ͣ���������initChOutputEn���ƣ�����PWM������δ����ǰPWM�ڵ������ƽֵ��
    independentChConfig[0].triggerEn = DISABLE;//ͨ���ⲿ����ʹ��
	
	/*modulation mode config*/
    pwmConfig.countMode = PWM_UP_COUNT; //PWM������ģʽ (��ͬ�ļ���ģʽƵ�ʼ�ռ�ձȼ��㷽ʽ��ͬ)
    pwmConfig.independentChannelNum = 1; //����ͨ����
    pwmConfig.combineChannelNum = 0; //��϶���
    pwmConfig.independentChConfig = independentChConfig; //����ͨ�����ñ�����ַ��ֵ
    pwmConfig.initChOutputEn = ENABLE; //ʹ�ܳ�ʼ��ͨ�����
    pwmConfig.initTriggerEn = DISABLE; //ͨ���ⲿ����ʹ��
	
	/*pwm config*/
    config.mode = PWM_MODE_MODULATION;//PWMģ������ΪPWMģʽ
    config.initModeStruct = &pwmConfig;//PWM���ýṹ���ַ��ֵ
    config.clkSource = PWM_CLK_SOURCE_APB; //PWMʱ��Դ����
    config.clkPsc = PWM_PRES;//PWMʱ��Դ��Ƶ
    config.initValue = 0;//��������ʼ�Ĵ���ֵ
    config.maxValue = MOD_PWM - 1; //PWM���������ֵ
    config.overflowInterrupEn = DISABLE;//����������жϽ���
    config.cntOverflowFreq = 0;//CNTOF�жϲ�����Ƶ���������Ƶ�ʵĹ�ϵ(0-127), 0��ʾÿ�μ������������������жϣ�1��ʾ���1�Σ�2��ʾ���2�Σ��Դ����ơ�
    config.interruptEn = DISABLE; //PWM�ж�ʹ��
    
    PWM_Init(PWM1, &config); //���ó�ʼ����Ч

}

/**
*
* @param[in] none
*
* @return none
*
* @brief 
* ��TIMER0���㰴������ʱ��
*/
void TIMER_PWM_Capture(void)
{
	Capture_Gpio_Init();
	Timer0_Init();
	PWM_Generate();
}




