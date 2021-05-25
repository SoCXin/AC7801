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
* @brief provide bcm system config interface
*
*
*
*******************************************************************************/
#include "sdk_system.h"
#include "ac780x_timer.h"
#define Delay500ms  (APB_BUS_FREQ/2-1)
#define Delay1s  (APB_BUS_FREQ-1)
CompileTime g_compileTime = {0};//编译时间
uint16_t g_updateModeCnt = 0;//升级计时
/**
 * GetTime
 *
* @param[in] curTime, n
* @return  time
 *
 * @brief  Get time
 */
uint32_t GetTime(uint8_t *curTime, uint32_t n)
{
    uint32_t i;
    uint32_t time = 0;
    for(i = 0; i < n; i++)
    {
        if ((curTime[i] > '9') || (curTime[i] < '0'))
            time = time * 10;
        else
            time = (curTime[i] - '0') + time * 10;
    }
    return time;
}
/**
 * System_GetCompileTime
 *
* @param[in] none
* @return  none
 *
 * @brief  Get compile time
 */
void System_GetCompileTime(void)
{
    uint32_t i, j;
    unsigned char DATE[] = __DATE__;
    unsigned char TIME[] = __TIME__;
    char *month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                       "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
                      };
    for(i = 0; i < 12; i++)
    {
        for(j = 1; j < 4; j++)
        {
            if (*(month[i] + j) != *(DATE + j))
            {
                break;
            }
        }
        if (j == 3)
        {
            break;
        }
    }
    g_compileTime.year = GetTime(DATE + 7, 4);
    g_compileTime.month = i + 1;
    printf("BOOT Build time:%s\r\n", __DATE__);
    g_compileTime.day = GetTime(DATE + 4, 2);
    g_compileTime.hour = GetTime(TIME, 2);
    g_compileTime.min = GetTime(TIME + 3, 2);
    g_compileTime.sec = GetTime(TIME + 5, 2);
}
/**
 * TIM1_Task
 *
* @param[in] timerIndex 
* @return  none
 *
 * @brief timer1 task
 */
void TIM1_Task(void *device, uint32_t wpara, uint32_t lpara)
{
    if (SDK_DEBUG)
    {
        printf("\r\nboot TIM1_Task \r\n");
    }
    LED2_TOGGLE;
    if ((g_updateModeCnt > 0) && (g_updateModeCnt < 300))
        g_updateModeCnt ++;
    
}
/**
 * Timer1_Config
 *
* @param[in] timerEn, interruptEn
* @return  none
 *
 * @brief  Timer1 Config
 */
void Timer1_Config(ACTION_Type timerEn, ACTION_Type interruptEn)
{
    if (SDK_DEBUG)
    {
        printf("\r\nTimer1_Config timerEn:%d,interruptEn:%d\r\n", timerEn, interruptEn);
    }
    TIMER_ConfigType  timerConfig1 = {0};
    /* configure TIMER channel 1, only enable timer */
    timerConfig1.periodValue      = Delay500ms;//DelayS(1);  ///500ms timeout
    timerConfig1.interruptEn      = interruptEn;
    timerConfig1.timerEn          = timerEn;
    timerConfig1.callBack = TIM1_Task;
    TIMER_Init(TIMER_CHANNEL1, &timerConfig1);
}
