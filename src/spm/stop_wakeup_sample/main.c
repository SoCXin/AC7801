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
* AutoChips Inc. (C) 2020. All rights reserved.
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
#include "ac780x.h"
#include "ac780x_gpio.h"
#include "ac780x_debugout.h"
#include "ac780x_uart.h"

/* define key gpio */
#define GPIO_KEY1   GPIOC,GPIO_PIN6
#define GPIO_KEY2   GPIOA,GPIO_PIN11

/* define LED gpio */
#define LED1_PIN					GPIOC, GPIO_PIN9
#define LED2_PIN					GPIOC, GPIO_PIN7


#define LED2_ON						do{GPIO_SetPinLevel(LED2_PIN, GPIO_LEVEL_HIGH);}while(0)
#define LED2_OFF					do{GPIO_SetPinLevel(LED2_PIN, GPIO_LEVEL_LOW);}while(0)
#define LED2_TOGGLE					do{if(GPIO_GetPinLevel(LED2_PIN)){LED2_OFF;}else{LED2_ON;}}while(0)
	
#define LED1_ON						do{GPIO_SetPinLevel(LED1_PIN, GPIO_LEVEL_HIGH);}while(0)
#define LED1_OFF					do{GPIO_SetPinLevel(LED1_PIN, GPIO_LEVEL_LOW);}while(0)
#define LED1_TOGGLE					do{if(GPIO_GetPinLevel(LED1_PIN)){LED1_OFF;}else{LED1_ON;}}while(0)


void GPIO_Handler(void *device, uint32_t wpara, uint32_t lpara);

/*!
* @brief Setup the microcontroller system. DeInitialize the System.
*
* @param[in] none
* @return none
*/
void SystemDeInit (void)
{
    CKGEN_SetSysclkSrc(SYSCLK_SRC_INTERNAL_OSC);    ///<set internal 8M RC as sysclk first
    SPM_EnablePLL(DISABLE);     ///<Disable PLL next
    SPM_EnableXOSC(DISABLE);    ///<Disable XOSC final
    CKGEN_SetPLLReference(PLL_REF_INTERAL_OSC);
    //SPM_EnableLVD(DISABLE);  //disable LVD if need
}

/**
* LED_Init
*
* @param[in]  none
* @return none
*
* @brief  init led control gpio
*
*/
void LED_Init(void)
{
    /* init gpio as output mode */
    GPIO_SetDir(LED1_PIN, GPIO_OUT);
    GPIO_SetDir(LED2_PIN, GPIO_OUT);
}

/**
* KEY_Init
*
* @param[in]  none
* @return none
*
* @brief  init key control gpio,KEY1通过普通输入判断，KEY2使用中断
*
*/
void KEY_Init(void)
{
    /* init gpio as input mode */
    GPIO_SetDir(GPIO_KEY1, GPIO_IN);
    GPIO_SetDir(GPIO_KEY2, GPIO_IN);
    
   // GPIO_EnableExtInterrupt(GPIO_KEY1, EXTI_TRIGGER_FALLING);
    GPIO_EnableExtInterrupt(GPIO_KEY2, EXTI_TRIGGER_FALLING);
    
   // GPIO_SetCallback(GPIO_PIN6, GPIO_Handler);
    GPIO_SetCallback(GPIO_PIN11, GPIO_Handler);    
}

/**
* main
*
* @param[in]  none
* @return none
*
* @brief  main function
*
*/
int main(void)
{
    
    static uint8_t key1_down = 0;
    uint32_t wakeupStatus = 0;
   // static uint8_t key2_down = 0;
    InitDelay();
    InitDebug();
    KEY_Init();    /* init key gpio */
    LED_Init();    /* init led gpio */
    printf("\r\nAC7801: stop_wakeup demo\r\n");
    GPIO_SetFunc(GPIOC, GPIO_PIN3, GPIO_FUN3);
    SPM_EnableModuleWakeup(SPM_MODULE_GPIO, ENABLE);
    while(1)
    {
        if (GPIO_LEVEL_LOW == GPIO_GetPinLevel(GPIO_KEY1))
        {
            if (key1_down == 0) ///<
            {
                mdelay(10); ///<消抖
                if (GPIO_LEVEL_LOW == GPIO_GetPinLevel(GPIO_KEY1))
                {
                    key1_down = 1;
                    LED1_TOGGLE;
                    
                    DisableInterrupts   ///<休眠前关闭中断，防止休眠流程被打断。
                    /* 休眠前关闭使用的模块，保证系统正常进入休眠
                    * Note: 如果有使能CAN唤醒之类功能，关闭CAN模块本身不影响CAN唤醒，只要引脚配置为CAN功能，并启用
                    *       了CAN唤醒即可。
                    */
                    UART_DeInit(UART2);     ///<关闭通信模块，防止干扰休眠。
                    SysTick->CTRL = 0;    ///<关闭系统时钟。
                    /* 休眠前手动关闭外部晶振和PLL */
                    SystemDeInit();
                    SysStop();      ///<go to sleep
                    SystemInit();
                    InitDebug();
                    InitDelay();
                    wakeupStatus = READ_MEM32(0x4000801C);
                    printf("system wakeup wakeupStatus=0x%08x \r\n", wakeupStatus);
                    EnableInterrupts    ///<唤醒后先进行必要的处理，再打开中断。
                }
            }
        }
        else
        {
            key1_down = 0;
        }
    }
}
/**
* GPIO_Handler
*
* @param[in]  device: GPIOx
* @param[in]  wpara: GPIO_PINx
* @param[in]  lpara: 0
* @return none
*
* @brief  init led control gpio
*
*/
void GPIO_Handler(void *device, uint32_t wpara, uint32_t lpara)
{
    if (wpara == GPIO_PIN11)
    {
        printf("this is a gpio interrupt\r\n");  
    }
}

