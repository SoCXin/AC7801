/*******************************************************************************
*
* @file sdk_interruptTask.c
*
* @author AutoChips
*
* @version 0.0.1
*
* @date Dec.12.2018
*
* @brief interrupt task interface
*
*
*
*******************************************************************************/
#include "sdk_interruptTask.h"
#include "sdk_system.h"
#include "sdk_can.h"
#include "board_config.h"
#include "ac780x_timer.h"
#include "ac780x_pwdt.h"
#include "ac780x_pwm.h"
#include "OS.h"
extern CAN_MsgInfoType g_CANRxMessage;
extern uint8_t g_canDataBuff[8];
/**
 * TIMER0_Task
 *
 * @param[in] timerIndex 
 * @return  none
 *
 * @brief timer0 deal with oil injection task
 */
void TIM0_Task(void *device, uint32_t wpara, uint32_t lpara)
{
    
}

/**
 * TIMER1_Task
 *
 * @param[in] timerIndex 
 * @return  none
 *
 * @brief timer1 deal with ignition task
 */
void TIM1_Task(void *device, uint32_t wpara, uint32_t lpara)
{
   
    
}
/**
 * TIM2_Task
 *
 * @param[in] timerIndex 
 * @return  none
 *
 * @brief timer2 deal with system task
 */
void TIM2_Task(void *device, uint32_t wpara, uint32_t lpara)
{
    SCH_Update();
}
/**
 * CAN0_IRQnCallBack
 *
 * @param[in]  device: device
 * @param[in]  wparam: parameter
 * @param[in]  lparam: parameter
 * @return     0: success
 *
 * @brief  Event interrupt callback funtion
 */
void CAN0_IRQnCallBack(void *device, uint32_t wpara, uint32_t lpara)
{
    if (wpara & CAN_CTRL1_RIF_Msk)
    {
        if (CAN_IsMsgInReceiveBuf((CAN_Type*)device))
        {
            CAN_ReadMsg((CAN_Type*)device, &g_CANRxMessage);
            CAN_BootExecutiveCommand(&g_CANRxMessage);
            //TODO 取出9个接收缓冲区
            
        }
    }
    if (wpara & CAN_CTRL1_TSIF_Msk)
    {
        
    }
}


