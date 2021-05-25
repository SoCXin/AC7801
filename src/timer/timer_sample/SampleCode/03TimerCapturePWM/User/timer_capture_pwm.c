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
* TIMER0中断回调
*/
void TIM0_Task1(void *device, uint32_t wpara, uint32_t lpara)
{
  //中断处理
}

/**
*
* @param[in] none
*
* @return none
*
* @brief Timer0_Init
*初始化TIMER0，每100s产生一次中断
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
*按键按下时产生下降沿中断，计数清零；按键松开产生上升沿中断，给按键计数g_pressingTime赋值，可通过仿真观察该变量
*/
void PWM_Capture_ByTimer(void *device, uint32_t wpara, uint32_t lpara)
{
	static uint8_t PMW_Edg_Count = 0;
	
	if(PMW_Edg_Count  == 0)
	{
		if (GPIO_GetPinLevel(GPIOA,GPIO_PIN3) == 0)//PWM下降沿
		{
			PWM_Down_moment_One = TIMER_GetCurrentValue(TIMER_CHANNEL0);
			PMW_Edg_Count = 1;
		}		
	}	
	else if(PMW_Edg_Count  == 1)
	{
		if (GPIO_GetPinLevel(GPIOA,GPIO_PIN3) == 1)//PWM上升沿
		{
			PWM_Up_moment = TIMER_GetCurrentValue(TIMER_CHANNEL0);
			PMW_Edg_Count = 2;
		}		
	}	
	else if(PMW_Edg_Count  == 2)
	{
		if(GPIO_GetPinLevel(GPIOA,GPIO_PIN3) == 0)//PWM下降沿
		{
			PWM_Down_moment_Two = TIMER_GetCurrentValue(TIMER_CHANNEL0);	
			PMW_Edg_Count = 0;
		}		
	}
	
	if((PWM_Up_moment < PWM_Down_moment_One)&&(PWM_Down_moment_Two < PWM_Up_moment)) //Timer为向下技术
	{
		PWM_Period = (PWM_Down_moment_One - PWM_Down_moment_Two)/24;   //24个计数为1us
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
*初始化PWM捕获口
*/
void Capture_Gpio_Init(void)
{
	GPIO_SetDir(GPIOA,GPIO_PIN3,GPIO_IN);   //使用按键1引脚为输入
	
	GPIO_EnableExtInterrupt(GPIOA,GPIO_PIN3, EXTI_TRIGGER_RISING_FALLING);//使能外部中断,上升沿下降沿都中断
	
	GPIO_SetCallback(GPIO_PIN3, PWM_Capture_ByTimer);//设置中断回调函数
}

/**
*
* @param[in] none
*
* @return none
*
* @brief 
*PMW输出频率为1K，占空比为25%
*/
void PWM_Generate(void)
{
    GPIO_SetFunc(GPIOB, GPIO_PIN3, GPIO_FUN2);//PWM1_CH7
	
	PWM_IndependentChConfig independentChConfig[1];//独立模式相关结构体
	PWM_ModulationConfigType pwmConfig; //PWM模式相关结构体
    PWM_ConfigType config; //PWM模块结构体
	
	memset(&independentChConfig, 0, sizeof(independentChConfig));
	memset(&pwmConfig, 0, sizeof(pwmConfig));
    memset(&config, 0, sizeof(config));
	
	independentChConfig[0].channel = PWM_CH_7; //通道7
    independentChConfig[0].chValue = MOD_PWM >> 2;//通道7 channel值，输出占空比 = chValue / maxValue = 25%
    independentChConfig[0].levelMode = PWM_HIGH_TRUE; //输出PWM高有效
    independentChConfig[0].polarity = PWM_OUTPUT_POLARITY_ACTIVE_HIGH; //输出极性高有效，PWM mask后PWM输出低电平
    independentChConfig[0].interruptEn = DISABLE;//PWM通道匹配中断使能位
    independentChConfig[0].initLevel = PWM_LOW_LEVEL;//PWM初始电平输出为低，该配置受initChOutputEn控制，决定PWM计数器未工作前PWM口的输出电平值。
    independentChConfig[0].triggerEn = DISABLE;//通道外部触发使能
	
	/*modulation mode config*/
    pwmConfig.countMode = PWM_UP_COUNT; //PWM计数器模式 (不同的计数模式频率及占空比计算方式不同)
    pwmConfig.independentChannelNum = 1; //独立通道数
    pwmConfig.combineChannelNum = 0; //组合对数
    pwmConfig.independentChConfig = independentChConfig; //独立通道配置变量地址赋值
    pwmConfig.initChOutputEn = ENABLE; //使能初始化通道输出
    pwmConfig.initTriggerEn = DISABLE; //通道外部触发使能
	
	/*pwm config*/
    config.mode = PWM_MODE_MODULATION;//PWM模块配置为PWM模式
    config.initModeStruct = &pwmConfig;//PWM配置结构体地址赋值
    config.clkSource = PWM_CLK_SOURCE_APB; //PWM时钟源配置
    config.clkPsc = PWM_PRES;//PWM时钟源分频
    config.initValue = 0;//计数器初始寄存器值
    config.maxValue = MOD_PWM - 1; //PWM计数器最大值
    config.overflowInterrupEn = DISABLE;//计数器溢出中断禁能
    config.cntOverflowFreq = 0;//CNTOF中断产生的频率与计数器频率的关系(0-127), 0表示每次计数器溢出都产生溢出中断，1表示间隔1次，2表示间隔2次，以此内推。
    config.interruptEn = DISABLE; //PWM中断使能
    
    PWM_Init(PWM1, &config); //配置初始化生效

}

/**
*
* @param[in] none
*
* @return none
*
* @brief 
* 由TIMER0计算按键按下时间
*/
void TIMER_PWM_Capture(void)
{
	Capture_Gpio_Init();
	Timer0_Init();
	PWM_Generate();
}




