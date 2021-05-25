/*******************************************************************************
*
* @file sdk_system.c
*
* @author AutoChips
*
* @version 0.0.1
*
* @date Aug.3.2018
*
* @brief system config interface
*
*
*
*******************************************************************************/
#include "sdk_system.h"
#include "sdk_flash.h"
#include "board_config.h"
#include "ac780x_timer.h"
#include "sdk_interruptTask.h"
#include "ac780x_gpio.h"
#include "ac780x_adc.h"
#include "ac780x_dma.h"
#include "ac780x_pwdt.h"
#include "ac780x_pwm.h"
#include "ac780x_spi.h"
#include "ac780x_i2c.h"

#include "ac780x_debugout.h"
#include "OS.h"

#define RECV_MSG_POOL_COUNT 1                       ///< receive buffer size
uint32_t ADC_Value_Buffer[10] = {0};
uint8_t GPIO_InputValue_Buffer = 0;
_Bool g_AdcdmaFinish = 0;
Module_Status g_ModuleStatus = {0};

extern uint8_t g_enterUpgrade;
/**
 * Put_Timer0_CNT
 *
 * @param[in] value:time (1us * value), flag: disable:TIMER_CNT_DISABLE, enable:TIMER_CNT_EN
 * @return  none
 * note:timer0 and timer1 is 32bit, timer2-timer7 is 16bit.
 * @brief  set timer0 load value
 */
void Put_Timer0_CNT(uint32_t value, ACTION_Type flag)
{
    uint32_t time = value;
    TIMER_ConfigType  timerConfig = {0};
    timerConfig.periodValue      = Delayus(time);/// timeout
    timerConfig.interruptEn      = flag;
    timerConfig.timerEn          = flag;
    timerConfig.callBack = TIM0_Task;
    TIMER_Init(TIMER_CHANNEL0, &timerConfig);
}
/**
 * Put_Timer1_CNT
 *
 * @param[in] value:time (1us * value), flag: disable:TIMER_CNT_DISABLE, enable:TIMER_CNT_EN
 * @return  none
 * note:timer0 and timer1 is 32bit, timer2-timer7 is 16bit.
 * @brief  set timer1 load value
 */
void Put_Timer1_CNT(uint32_t value, ACTION_Type flag)
{
    uint32_t time = value;
    TIMER_ConfigType  timerConfig = {0};
    timerConfig.periodValue      = Delayus(time);/// timeout
    timerConfig.interruptEn      = flag;
    timerConfig.timerEn          = flag;
    timerConfig.callBack = TIM1_Task;
    TIMER_Init(TIMER_CHANNEL1, &timerConfig);
}
/**
 * Put_Timer2_CNT
 *
 * @param[in] value:time (1us * value), flag: disable:TIMER_CNT_DISABLE, enable:TIMER_CNT_EN
 * @return  none
 * note:timer0 and timer1 is 32bit, timer2-timer7 is 16bit.
 * @brief  set timer1 load value
 */
void Put_Timer2_CNT(uint32_t value, ACTION_Type flag)
{
    uint32_t time = value;
    TIMER_ConfigType  timerConfig = {0};
    timerConfig.periodValue      = Delayus(time);/// timeout
    timerConfig.interruptEn      = flag;
    timerConfig.timerEn          = flag;
    timerConfig.callBack = TIM2_Task;
    TIMER_Init(TIMER_CHANNEL2, &timerConfig);
}

/**
 * SetNVICPriority
 *
 * @param[in] none
 * @return  none
 * @brief  set system NVIC Priority
 */
void SetNVICPriority(void)
{
    NVIC_SetPriority(CAN0_IRQn, 0);
    NVIC_SetPriority(TIMER_CHANNEL0_IRQn, 1);
    NVIC_SetPriority(TIMER_CHANNEL1_IRQn, 2);
    NVIC_SetPriority(TIMER_CHANNEL2_IRQn, 3);
}

/**
* System_DisableOutput
*
* @param[in] none
* @return none
*
* @brief disable output
*
*/
void System_DisableOutput(void)
{
    
}
/**
* System_UpdateAPP
*
* @param[in] none
* @return none
*
* @brief enter update app mode
*
*/
void System_UpdateAPP(void)
{
    if (g_enterUpgrade == 1)
    {
        DataRecord_WriteUpgrade(CAN_BOOTLOADER_UPGRADE);
        System_DisableOutput();
        //RESET
//        __set_FAULTMASK(1);
        NVIC_SystemReset();
    }
}

/**
 * Board_Config
 *
 * @param[in] none
 * @return  none
 * @brief  init board source
 */
void Board_Config(void)
{
    DisableInterrupts//关全局中断
    SCB->VTOR = APP_START_ADDR;//重定向中断向量表
    InitDebug();
    InitDelay();
    SetNVICPriority();
    CAN_Config();
    EnableInterrupts//开全局中断
}

