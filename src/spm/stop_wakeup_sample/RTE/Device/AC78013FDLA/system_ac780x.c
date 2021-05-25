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

/*!
* @file system_ac780x.c
*
* @brief This file provides system clock config integration functions.
*
*/

/* ===========================================  Includes  =========================================== */
#include "ac780x.h"

/* ============================================  Define  ============================================ */

#define  XTAL            ( USE_XTAL )      /* Oscillator frequency */
#define  SYSTEM_CLOCK    ( SYSCLK_FREQ )   /* system clock frequency */

#define MAX_SYSTICK_COUNT (SysTick_LOAD_RELOAD_Msk + 1)  /* systemTick max count value */

/* for eFlash clock config */
#define SYS_EFLSH_UNLOCK_KEY1 0xac7811
#define SYS_EFLSH_UNLOCK_KEY2 0x01234567
#define Eflash_KEY_REG        0x40002000
#define EFLASH_INFO_REG       0x40002004
#define EFLASH_CTRL2_REG      0x40002014
#define EFLASH_CTRL3_REG      0x40002040

#define SYSTEM_CLOCK_SRC_HSI  0UL
#define SYSTEM_CLOCK_SRC_PLL  1UL
#define SYSTEM_CLOCK_SRC_HSE  2UL
/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */
/* systemTick us delay factor */
static uint32_t  s_facus = 0;

/* systemTick ms delay factor */
static uint32_t  s_facms = 0;

/* system clock frequency save */
uint32_t SystemCoreClock = SYSTEM_CLOCK;

/* apbclk frequency save */
uint32_t g_periphAPBClock = SYSTEM_CLOCK / 2;

/* define system pll prediv value */
static __IOM uint8_t s_pllPREDIV[] = {1, 2, 4};

/* ====================================  Functions declaration  ===================================== */

/* Externals declaration */
#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
extern uint32_t __Vectors;
#endif

/* ======================================  Functions define  ======================================== */

/*!
* @brief Init Delay count factor, 1s = TICKS * (1/f)  s;
*
* @param[in]  none
* @return none
*/
void InitDelay(void)
{
    s_facus = __SYSTEM_CLOCK / 1000000;
    s_facms = s_facus * 1000;
}

/*!
* @brief Set HSE bypass mode. if xosc enable fail then change system clock to HSI(8m).
*
* @param[in]  none
* @return none
*/
void SetHSEBYPClock(void)
{
    __IO PLL_ReferenceType pllReference = PLL_REF_INTERAL_OSC;

    /* enable xosc bypass mode */
    SPM_EnableXOSCBypassMode(ENABLE);
    CKGEN_SetSysclkDiv(SYSCLK_DIV);
    SPM_XOSCOKBypassSet(DISABLE);
    /* check if xosc enable success? */
    if (SPM_EnableXOSC(ENABLE) == SUCCESS)
    {
        CKGEN_SetPLLReference(PLL_REF_EXTERNAL_OSC);
        pllReference = PLL_REF_EXTERNAL_OSC;
    }
    else
    {
        /* xosc enable fail, use hsi(8m) as pll clock(default) */
        CKGEN_SetPLLReference(PLL_REF_INTERAL_OSC);
        pllReference = PLL_REF_INTERAL_OSC;
    }

    /* check if pll enable success? */
    if (SPM_EnablePLL(ENABLE) == SUCCESS)
    {
        CKGEN_SetPllPrevDiv(PLL_PREDIV);
        CKGEN_SetPllPostDiv(PLL_POSDIV);
        CKGEN_SetPllFeedbackDiv(PLL_FBKDIV);
        CKGEN_SetSysclkSrc(SYSCLK_SRC_PLL_OUTPUT);
    }
    else  /* pll enable fail */
    {
        if (pllReference == PLL_REF_EXTERNAL_OSC)
        {
            CKGEN_SetSysclkSrc(SYSCLK_SRC_EXTERNAL_OSC);  /* set external osc as sysclk src */
        }
        else
        {
            CKGEN_SetSysclkSrc(SYSCLK_SRC_INTERNAL_OSC);  /* set internal osc as sysclk src */
        }
    }
}

/*!
* @brief Set XOSC Clock. if xosc enable fail, change system clock to HSI(8m).
*
* @param[in]  none
* @return none
*/
void SetXOSCClock(void)
{
    CKGEN_SetSysclkDiv(SYSCLK_DIV);
    SPM_XOSCOKBypassSet(DISABLE);
    /* check if xosc enable success? */
    if (SPM_EnableXOSC(ENABLE) == SUCCESS)
    {
        /* xosc enable success, use external osc as system clock */
        CKGEN_SetSysclkSrc(SYSCLK_SRC_EXTERNAL_OSC);
    }
    else
    {
        /* xosc enable fail, use hsi(8m) as system clock(default) */
        CKGEN_SetSysclkSrc(SYSCLK_SRC_INTERNAL_OSC);
    }
}

/*!
* @brief Use HSE mode, set the system clock.
*        pll reference clock default use xosc,if xosc enable fail,change to hsi(8m).
*        system clock source default use pll output. if pll enable fail, then
*        stop change system clock source to osc(HSI or HSE).
*
* @param[in]  none
* @return none
*/
void SetHSEClock(void)
{
    __IO PLL_ReferenceType pllReference = PLL_REF_INTERAL_OSC;

    SPM_XOSCOKBypassSet(DISABLE);
    /* check if xosc enable success? */
    if (SPM_EnableXOSC(ENABLE) == SUCCESS)
    {
        CKGEN_SetPLLReference(PLL_REF_EXTERNAL_OSC);
        pllReference = PLL_REF_EXTERNAL_OSC;
    }
    else  /* if xosc enable fail,change pll reference clock to HSI 8M */
    {
        CKGEN_SetPLLReference(PLL_REF_INTERAL_OSC);
        pllReference = PLL_REF_INTERAL_OSC;
    }

    /* set system clock divider */
    CKGEN_SetSysclkDiv(SYSCLK_DIV);
    /* check if pll enable success? */
    if (SPM_EnablePLL(ENABLE) == SUCCESS)
    {
        CKGEN_SetPllPrevDiv(PLL_PREDIV);
        CKGEN_SetPllPostDiv(PLL_POSDIV);
        CKGEN_SetPllFeedbackDiv(PLL_FBKDIV);
        CKGEN_SetSysclkSrc(SYSCLK_SRC_PLL_OUTPUT);
    }
    else  /* pll enable fail */
    {
        if (pllReference == PLL_REF_EXTERNAL_OSC)
        {
            CKGEN_SetSysclkSrc(SYSCLK_SRC_EXTERNAL_OSC);  /* set external osc as sysclk src */
        }
        else
        {
            CKGEN_SetSysclkSrc(SYSCLK_SRC_INTERNAL_OSC);  /* set internal osc as sysclk src */
        }
    }
}

/*!
* @brief Use HSI Mode.Default set pll output as system clock;if pll enable fail
*        ,change system clock to HSI(8m).
*
* @param[in]  none
* @return none
*/
void SetHSIClock(void)
{
    CKGEN_SetPLLReference(PLL_REF_INTERAL_OSC);
    CKGEN_SetPllPrevDiv(PLL_PREDIV);
    CKGEN_SetPllPostDiv(PLL_POSDIV);
    CKGEN_SetPllFeedbackDiv(PLL_FBKDIV);
    CKGEN_SetSysclkDiv(SYSCLK_DIV);
    /* check if pll enable success? */
    if (SPM_EnablePLL(ENABLE) == SUCCESS)
    {
        /* pll enable success,use pll output as system clock src */
        CKGEN_SetSysclkSrc(SYSCLK_SRC_PLL_OUTPUT);
    }
    else
    {
        /* pll enable fail,use hsi(8m) as system clock src */
        CKGEN_SetSysclkSrc(SYSCLK_SRC_INTERNAL_OSC);
    }
}

/*!
* @brief set system clock
*
* @param[in] none
* @return none
*/
void SetSysClock(void)
{
#if AC780X_CLOCK_SRC == IC_CLOCK_SRC_XOSC
    SetXOSCClock();
#elif AC780X_CLOCK_SRC == IC_CLOCK_SRC_HSI
    SetHSIClock();
#elif AC780X_CLOCK_SRC == IC_CLOCK_SRC_HSE
    SetHSEClock();
#elif AC780X_CLOCK_SRC == IC_CLOCK_SRC_XOSC_BYPASS
    SetHSEBYPClock();
#else
    SetHSIClock();
#endif
}

/**
* SetEflashClock
*
* @param[in] none
* @return none
*
* @brief Set eflash clock.
*
**/
void SetEflashClock(void)
{
    uint32_t eflashCKDIV = 0;
    uint32_t ctrl2 = 0x0;
    int32_t timeout = 500;

    do
    {
        WRITE_MEM32(Eflash_KEY_REG, SYS_EFLSH_UNLOCK_KEY1);
        WRITE_MEM32(Eflash_KEY_REG, SYS_EFLSH_UNLOCK_KEY2);
        timeout--;
    } while ((READ_BIT32(EFLASH_INFO_REG, (uint32_t)1 << 31) != 0) && (0 < timeout));

    eflashCKDIV = (SYSCLK_FREQ / 1000000 + 1) & 0x7F;
    ctrl2 = 0x2000 | eflashCKDIV;
    WRITE_MEM32(EFLASH_CTRL2_REG, ctrl2);

    WRITE_MEM32(EFLASH_CTRL3_REG, 0x20000);
    SET_BIT32((*(uint32_t*)EFLASH_CTRL2_REG), (uint32_t)1 << 8);
    SET_BIT32((*(uint32_t*)EFLASH_INFO_REG), (uint32_t)1 << 31);
}

/*!
* @brief delay until the syctick count tick to 0
*
* @param[in] tick : systick count value
* @return none
*/
static void SysTickDelay(uint32_t tick)
{
    __IO uint32_t tickFlag = 0;

    SysTick->LOAD = tick - 1;
    SysTick->VAL = 0x00;
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    do
    {
        tickFlag = SysTick->CTRL;

    } while(!(tickFlag & SysTick_CTRL_COUNTFLAG_Msk));

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0X00;
}

/*!
* @brief Repeatlly delay the ticks for the given times
*
* @param[in] param : times, the times of delay the tick
* @param[in] param : tick, each tick to delay
* @return none
*/
static void SysTickRepeatDelay(uint32_t times, uint32_t tick)
{
    uint32_t i = 0;

    for (i = 0; i < times; i++)
    {
        SysTickDelay(tick);
    }
}

/*!
* @brief delay us
*
* @param[in] param : us, us for delay
* @return none
*/
void udelay(uint32_t us)
{
    uint32_t tick = us * s_facus;

    SysTickRepeatDelay(tick / MAX_SYSTICK_COUNT, MAX_SYSTICK_COUNT);

    SysTickDelay(tick % MAX_SYSTICK_COUNT);
}

/*!
* @brief delay ms
*
* @param[in] param : ms, us for delay
* @return none
*/
void mdelay(uint32_t ms)
{
    uint32_t tick = ms * s_facms;

    SysTickRepeatDelay(tick / MAX_SYSTICK_COUNT, MAX_SYSTICK_COUNT);

    SysTickDelay(tick % MAX_SYSTICK_COUNT);
}


/*!
* @brief get system clock frequence
*
* @param[in] none
* @return none
*/
void SystemCoreClockUpdate(void)
{
    __IO uint8_t clkSrc = 0, postDiv = 0, feedbackDiv = 0, sysclkDiv = 0;
    __IO uint8_t pllRef = 0, preDiv = 0, apbClkDiv = 0;
    __IO uint32_t vcoClk = 0;

    clkSrc = CKGEN->CTRL & CKGEN_CTRL_SYSCLK_SEL_Msk;
    apbClkDiv = ((CKGEN->CTRL & CKGEN_CTRL_APBCLK_DIV_Msk) >> CKGEN_CTRL_APBCLK_DIV_Pos) + 1;
    switch (clkSrc)
    {
    case SYSTEM_CLOCK_SRC_HSI:
        SystemCoreClock = HSI_FREQ;  /* HSI RC */

        break;
    case SYSTEM_CLOCK_SRC_PLL:
        /* PLL Output */
        pllRef = (CKGEN->CTRL & CKGEN_CTRL_PLL_REF_SEL_Msk) >> CKGEN_CTRL_PLL_REF_SEL_Pos;
        sysclkDiv = ((CKGEN->CTRL & CKGEN_CTRL_SYSCLK_DIV_Msk) >> CKGEN_CTRL_SYSCLK_DIV_Pos) + 1;
        preDiv = s_pllPREDIV[(CKGEN->SYSPLL1_CFG0 & CKGEN_SYSPLL1_CFG0_SYSPLL1_PREDIV_Msk) >> CKGEN_SYSPLL1_CFG0_SYSPLL1_PREDIV_Pos];
        postDiv = ((CKGEN->SYSPLL1_CFG0 & CKGEN_SYSPLL1_CFG0_SYSPLL1_POSDIV_Msk) >> CKGEN_SYSPLL1_CFG0_SYSPLL1_POSDIV_Pos) * 2;
        feedbackDiv = (CKGEN->SYSPLL1_CFG0 & CKGEN_SYSPLL1_CFG0_SYSPLL1_FBKDIV_Msk) >> CKGEN_SYSPLL1_CFG0_SYSPLL1_FBKDIV_Pos;
        if (pllRef)
        {
            /* ext xosc clock */
            SystemCoreClock = USE_XTAL;
        }
        else
        {
            /* internal RC */
            SystemCoreClock = HSI_FREQ;
        }

        vcoClk = (SystemCoreClock * feedbackDiv) / preDiv;   /* pll vco clock */
        SystemCoreClock = vcoClk / (postDiv * sysclkDiv);    /* system clock freq */
        break;
    case SYSTEM_CLOCK_SRC_HSE:
        SystemCoreClock = USE_XTAL;  /* HSE XTAL */
        break;

    default:
        break;
    }

    g_periphAPBClock = SystemCoreClock / apbClkDiv;    /* apb clock freq */
}

/*!
* @brief Setup the microcontroller system. Initialize the System.
*
* @param[in] none
* @return none
*/
void SystemInit (void)
{
#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    SCB->VTOR = (uint32_t) &__Vectors;
#endif

    SystemCoreClock = SYSTEM_CLOCK;

    SetEflashClock();
    SetSysClock();
    CKGEN_SetAPBClockDivider(APBCLK_DIV);
    //SPM_EnableLVD(DISABLE);  //disable LVD if need
}

/*!
* @brief  change the current menu and display
*
* @param[in]
* @return return the CPUID
*/
uint32_t GetCPUID(void)
{
    return SCB->CPUID;
}

/*!
* @brief enter the Standby mode
*
* @param[in] none
* @return none
*/
void SysStandby(void)
{
    SPM_SetLowPowerMode(LOW_POWER_MODE_STANDBY);
    SPM_EnableModuleWakeup(SPM_MODULE_NMI, ENABLE);
    /* Set the SLEEPDEEP bit to enable deep sleep mode (STOP) */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

#if defined ( __ICCARM__ )
    asm("WFI");
#elif defined ( __GNUC__)
    asm("WFI");
#elif defined (__CC_ARM)
    // If using KEIL's uVision, use the CMSIS intrinsic
    __wfi();
#endif

}

/*!
* @brief enter the Stop mode
*
* @param[in] none
* @return none
*/
void SysStop(void)
{
    SPM_SetLowPowerMode(LOW_POWER_MODE_STOP);
   // SPM_EnableModuleWakeup(SPM_MODULE_NMI, ENABLE);
    /* Set the SLEEPDEEP bit to enable deep sleep mode (STOP) */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
#if defined ( __ICCARM__ )
    asm("WFI");
#elif defined ( __GNUC__)
    asm("WFI");
#elif defined (__CC_ARM)
    // If using KEIL's uVision, use the CMSIS intrinsic
    __wfi();
#endif
    // Add Debug Interface to enable printf after wakeup
}

/*!
* @brief enter the Sleep mode
*
* @param[in] none
* @return none
*/
void SysSleep(void)
{
    /* Clear the SLEEPDEEP bit to make sure we go into WAIT (sleep) mode instead
    * of deep sleep.
    */

    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
#if defined ( __ICCARM__ )
    asm("WFI");
#elif defined ( __GNUC__)
    asm("WFI");
#elif defined (__CC_ARM)
    // If using KEIL's uVision, use the CMSIS intrinsic
    __wfi();
#endif
}

/* =============================================  EOF  ============================================== */
