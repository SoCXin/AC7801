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

#ifndef _SYSTEM_AC780X_H
#define _SYSTEM_AC780X_H

/*!
* @file system_ac780x.h
*
* @brief This file provides system clock config integration functions interfaces.
*
*/

#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */
#include "ac780x_ckgen.h"
#include "ac780x_spm.h"
#include "device_assert.h"
#include "ac780x_debugout.h"

/* ============================================  Define  ============================================ */

/*!< global interrupt control */
#define EnableInterrupts  __ASM(" CPSIE i");
#define DisableInterrupts __ASM(" CPSID i");

/*!< global fault control */
#define EnableFatules  __ASM(" CPSIE F");
#define DisableFatules __ASM(" CPSID F");

#define IC_CLOCK_SRC_HSI         0U  /*!< High speed internal RC(HSI): The internal RC OSC to provide 8MHz clock source */
#define IC_CLOCK_SRC_HSE         1U  /*!< The external OSC to provide 4MHZ ~30MHz  crystal */
#define IC_CLOCK_SRC_XOSC        2U  /*!< The external OSC to provide 4MHZ ~30MHz  oscillator */
#define IC_CLOCK_SRC_XOSC_BYPASS 3U  /*!< The external OSC to provide 4MHZ ~30MHz  oscillator */

#define AC780X_CLOCK_SRC  IC_CLOCK_SRC_HSE /*!< define mcu clock source */

#define HSI_FREQ      8000000UL   /*!< Internal 8M RC clock */
#define XTAL_16M     16000000UL   /*!< extern crystal oscillator 16m */
#define XTAL_12M     12000000UL   /*!< extern crystal oscillator 12m */
#define XTAL_8M       8000000UL   /*!< extern crystal oscillator 8m  */
#define XTAL_4M       4000000UL   /*!< extern crystal oscillator 4m  */
#define XTAL_30M     30000000UL   /*!< extern crystal oscillator 30m */

#define USE_XTAL      XTAL_8M  /*!< Select the actual crystal frequency */

/*!< PLL available in HSE & HSI mode */
#if USE_XTAL == XTAL_16M
#define PLL_POSDIV   PLL_POSDIV_16
#define PLL_FBKDIV   96U
#define PLL_PREDIV   PLL_PREDIV_2
#elif USE_XTAL == XTAL_12M
#define PLL_POSDIV   PLL_POSDIV_12
#define PLL_FBKDIV   96U
#define PLL_PREDIV   PLL_PREDIV_2
#elif USE_XTAL == XTAL_8M
#define PLL_POSDIV   PLL_POSDIV_16
#define PLL_FBKDIV   96U
#define PLL_PREDIV   PLL_PREDIV_1
#elif USE_XTAL == XTAL_4M
#define PLL_POSDIV   PLL_POSDIV_16
#define PLL_FBKDIV   192U
#define PLL_PREDIV   PLL_PREDIV_1
#elif USE_XTAL == XTAL_30M
#define PLL_POSDIV   PLL_POSDIV_30
#define PLL_FBKDIV   48U
#define PLL_PREDIV   PLL_PREDIV_1
#else
#error
#endif

/*!< VCO = (USE_XTAL * PLL_FBKDIV) / PLL_PREDIV;
     SYSCLK_FREQ = VCO / PLL_POSDIV / SYSCLK_DIV;
     APB_BUS_FREQ = SYSCLK_FREQ / APBCLK_DIVIDER_2
*/
#define SYSCLK_DIV   SYSCLK_DIVIDER_1
#define APBCLK_DIV   APBCLK_DIVIDER_2
#define SYSCLK_FREQ  48000000UL
#define APB_BUS_FREQ 24000000UL

#define __SYSTEM_CLOCK    SYSCLK_FREQ

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

extern uint32_t SystemCoreClock;     /*!< System Clock Frequency (Core Clock)  */
extern uint32_t g_periphAPBClock;    /*!< APB Clock Frequency */

/* ====================================  Functions declaration  ===================================== */
/*!
* @brief Setup the microcontroller system. Initialize the System.
*
* @param[in] none
* @return none
*/
extern void SystemInit (void);
/*!
* @brief get system clock frequence
*
* @param[in] none
* @return none
*/
extern void SystemCoreClockUpdate(void);
/*!
* @brief Init Delay count factor, 1s = TICKS * (1/f)  s;
*
* @param[in] none
* @return none
*/
extern void udelay(uint32_t us);
/*!
* @brief delay ms
*
* @param[in] param : ms, us for delay
* @return none
*/
extern void mdelay(uint32_t ms);
/*!
* @brief Init Delay count factor, 1s = TICKS * (1/f)  s;
*
* @param[in] none
* @return none
*/
extern void InitDelay(void);
/*!
* @brief enter the Sleep mode
*
* @param[in] none
* @return none
*/
extern void SysSleep(void);
/*!
* @brief enter the Stop mode
*
* @param[in] none
* @return none
*/
extern void SysStop(void);
/*!
* @brief enter the Standby mode
*
* @param[in] none
* @return none
*/
extern void SysStandby(void);

#ifdef __cplusplus
}
#endif

#endif /* _SYSTEM_AC780X_H */

/* =============================================  EOF  ============================================== */
