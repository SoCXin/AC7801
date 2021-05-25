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

#ifndef _AC780X_MMDIVSQRT_REG_H
#define _AC780X_MMDIVSQRT_REG_H

/*!
* @file ac780x_mmdivsqrt_reg.h
*
* @brief MMDIVSQRT access register inline function definition.
*
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ===========================================  Includes  =========================================== */
#include "ac780x_mmdivsqrt.h"

/* ============================================  Define  ============================================ */
#define MMDIVSQRT_CSR_USIGN_INVERT_Msk     (~MMDIVSQRT_CSR_USIGN_Msk)
#define MMDIVSQRT_CSR_REM_INVERT_Msk       (~MMDIVSQRT_CSR_REM_Msk)

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* ======================================  Functions define  ======================================== */
/*!
* @brief Set MMDIVSQRT dividend value
*
* @param[in] dend: Dividend value
* @return none
*/
#define MMDIVSQRT_SetDend(dend)  (MMDIVSQRT->DEND = (dend))

/*!
* @brief Set MMDIVSQRT Divisor value
*
* @param[in] dsor: Divisor value
* @return none
*/
#define MMDIVSQRT_SetDsor(dsor)  (MMDIVSQRT->DSOR = (dsor))

/*!
* @brief Set MMDIVSQRT Dividend shift num
*
* @param[in] dsft: Dividend shift num, its value can be 0~31
* @return none
*/
#define MMDIVSQRT_SetDsft(dsft)  do{  \
                                     if ((dsft) < 32)            \
                                     {                           \
                                         MMDIVSQRT->DSFT = (dsft); \
                                     }                           \
                                     else                        \
                                     {                           \
                                         MMDIVSQRT->DSFT = 0;    \
                                     }                           \
                                  }while(0)
/*!
* @brief Set MMDIVSQRT Radicand x value
*
* @param[in] rcndx: Radicand x value
* @return none
*/
#define MMDIVSQRT_SetRcndx(rcndx)  (MMDIVSQRT->RCNDX = (rcndx))

/*!
* @brief Set MMDIVSQRT Radicand y value
*
* @param[in] rcndy: Radicand y value
* @return none
*/
#define MMDIVSQRT_SetRcndy(rcndy)  (MMDIVSQRT->RCNDY = (rcndy))

/*!
* @brief Set MMDIVSQRT Division is unsigned or signed type
*
* @param[in] usign: Division type, 0: signed division   1: unsigned division
* @return none
*/
#define MMDIVSQRT_SetDivisionUsign(usign)  MODIFY_REG32(MMDIVSQRT->CSR, MMDIVSQRT_CSR_USIGN_Msk, MMDIVSQRT_CSR_USIGN_Pos, (usign))

/*!
* @brief Set MMDIVSQRT Division caculate result is quotient or remainder
*
* @param[in] rem: Division result type,
*                     -MMDIVSQRT_DIV_RESULT_QUOTIENT: result is quotient
*                     -MMDIVSQRT_DIV_RESULT_REMAINDER: result is remainder
* @return none
*/
#define MMDIVSQRT_SetDivisionRem(rem)  MODIFY_REG32(MMDIVSQRT->CSR, MMDIVSQRT_CSR_REM_Msk, MMDIVSQRT_CSR_REM_Pos, (rem))

/*!
* @brief Get MMDIVSQRT last operation type, can be division or Square root
*
* @param[in] none
* @return last operation type:
*               -0: no operation
*               -1: Square root
*               -2: division
*/
__STATIC_INLINE uint8_t MMDIVSQRT_GetLastOperation(void)
{
    uint8_t lastOperationType = 0;

    if (MMDIVSQRT->CSR & MMDIVSQRT_CSR_SQRT_Msk)
    {
      lastOperationType = 1;
    }
    else if (MMDIVSQRT->CSR & MMDIVSQRT_CSR_DIV_Msk)
    {
      lastOperationType = 2;
    }
    else
    {
      lastOperationType = 0;
    }

    return lastOperationType;
}

/*!
* @brief Get MMDIVSQRT is busy or not
*
* @param[in] none
* @return busy state:
*             -0: not busy
*             -1: busy
*/
#define MMDIVSQRT_IsBusy()  ((MMDIVSQRT->CSR & MMDIVSQRT_CSR_BUSY_Msk) >> MMDIVSQRT_CSR_BUSY_Pos)

/*!
* @brief Get MMDIVSQRT signed division result
*
* @param[in] none
* @return result: calculate result
*/
#define MMDIVSQRT_GetResult()  (MMDIVSQRT->RESULT)

/*!
* @brief Division prototype: x / y, calculate signed division and get quotient
*
* @param[in] dend: Dividend value
* @param[in] dsor: Divisor value
* @param[out] result: calurate result(quotient) pointer
* @return : none
*/
#define MMDIVSQRT_CalSignDivNoDsft(dend, dsor, result) do { \
                                               MMDIVSQRT->CSR &= MMDIVSQRT_CSR_USIGN_INVERT_Msk;   \
                                               MMDIVSQRT->CSR &= MMDIVSQRT_CSR_REM_INVERT_Msk;     \
                                               MMDIVSQRT->DSFT = 0;    \
                                               MMDIVSQRT->DEND = (dend); \
                                               MMDIVSQRT->DSOR = (dsor); \
                                               while (MMDIVSQRT_IsBusy());    \
                                               *(int32_t *)(result) = MMDIVSQRT->RESULT; \
                                               }while(0)

/*!
* @brief Division prototype: (x << z) / y,   calculate signed division and get quotient
*
* @param[in] dend: Dividend value
* @param[in] dsor: Divisor value
* @param[in] dsft: Dividend shift num, its value can be 0~31
* @param[out] result: calurate result(quotient) pointer
* @return : none
*/
#define MMDIVSQRT_CalSignDivWithDsft( dend, dsor, dsft, result) do {  \
                                                      MMDIVSQRT->CSR &= MMDIVSQRT_CSR_USIGN_INVERT_Msk;   \
                                                      MMDIVSQRT->CSR &= MMDIVSQRT_CSR_REM_INVERT_Msk;    \
                                                      MMDIVSQRT->DSFT = (dsft);           \
                                                      MMDIVSQRT->DEND = (dend);          \
                                                      MMDIVSQRT->DSOR = (dsor);          \
                                                      while (MMDIVSQRT_IsBusy());      \
                                                      *(int32_t *)(result) = MMDIVSQRT->RESULT; \
                                                      }while(0)

/*!
* @brief Division prototype: (x << z) / y,   calculate signed division and get quotient
*
* @param[in] dend: Dividend value
* @param[in] dsor: Divisor value
* @param[in] dsft: Dividend shift num, its value can be 0~31
* @param[out] result: calurate result(quotient) pointer
* @return : none
*/
#define MMDIVSQRT_CalSignDivWithDsft_Remainder( dend, dsor, dsft, result) do {  \
                                                      MMDIVSQRT->CSR &= MMDIVSQRT_CSR_USIGN_INVERT_Msk;   \
                                                      MMDIVSQRT->CSR |= MMDIVSQRT_CSR_REM_Msk;    \
                                                      MMDIVSQRT->DSFT = (dsft);           \
                                                      MMDIVSQRT->DEND = (dend);          \
                                                      MMDIVSQRT->DSOR = (dsor);          \
                                                      while (MMDIVSQRT_IsBusy());      \
                                                      *(int32_t *)(result) = MMDIVSQRT->RESULT; \
                                                      }while(0)													  
/*!
* @brief Division prototype: x / y, calculate signed division and get quotient
*
* @param[in] dend: Dividend value
* @param[in] dsor: Divisor value
* @param[out] result: calurate result(quotient) pointer
* @return : none
*/
#define MMDIVSQRT_CalUnsignDivNoDsft(dend, dsor, result) do {  \
                                           MMDIVSQRT->CSR |= MMDIVSQRT_CSR_USIGN_Msk;      \
                                           MMDIVSQRT->CSR &= MMDIVSQRT_CSR_REM_INVERT_Msk; \
                                           MMDIVSQRT->DSFT = 0;      \
                                           MMDIVSQRT->DEND = (dend);   \
                                           MMDIVSQRT->DSOR = (dsor);   \
                                           while (MMDIVSQRT_IsBusy());  \
                                           *(uint32_t *)(result) = MMDIVSQRT->RESULT; \
                                           }while(0)

/*!
* @brief Division prototype: (x << z) / y, calculate signed division and get quotient
*
* @param[in] dend: Dividend value (uint32_t)
* @param[in] dsor: Divisor value (uint32_t)
* @param[in] dsft: Dividend shift num, its value can be 0~31 (uint8_t)
* @param[out] result: calculate result(quotient) pointer (uint32_t*)
* @return : none
*/
#define MMDIVSQRT_CalUnsignDivWithDsft(dend, dsor, dsft, result) do{ \
                                           MMDIVSQRT->CSR |= MMDIVSQRT_CSR_USIGN_Msk;       \
                                           MMDIVSQRT->CSR &= MMDIVSQRT_CSR_REM_INVERT_Msk;  \
                                           MMDIVSQRT->DSFT = (dsft);                          \
                                           MMDIVSQRT->DEND = (dend);                          \
                                           MMDIVSQRT->DSOR = (dsor);                          \
                                           while (MMDIVSQRT_IsBusy());                      \
                                           *(uint32_t *)(result) = MMDIVSQRT->RESULT;         \
                                          }while(0)
/*!
* @brief Division prototype: (x << z) / y, calculate signed division and get quotient
*
* @param[in] dend: Dividend value (uint32_t)
* @param[in] dsor: Divisor value (uint32_t)
* @param[in] dsft: Dividend shift num, its value can be 0~31 (uint8_t)
* @param[out] result: calculate result(quotient) pointer (uint32_t*)
* @return : none
*/
#define MMDIVSQRT_CalUnsignDivWithDsft_Remainder(dend, dsor, dsft, result) do{ \
                                           MMDIVSQRT->CSR |= MMDIVSQRT_CSR_USIGN_Msk;       \
                                           MMDIVSQRT->CSR |= MMDIVSQRT_CSR_REM_Msk;  \
                                           MMDIVSQRT->DSFT = (dsft);                          \
                                           MMDIVSQRT->DEND = (dend);                          \
                                           MMDIVSQRT->DSOR = (dsor);                          \
                                           while (MMDIVSQRT_IsBusy());                      \
                                           *(uint32_t *)(result) = MMDIVSQRT->RESULT;         \
                                          }while(0)
/*!
* @brief Square root prototype: v(x^2+y^2), calculate square root and get result
*
* @param[in] rcndx: Radicand x value
* @param[in] rcndy: Radicand y value
* @param[out] result: calurate result pointer
* @return : none
*/
#define MMDIVSQRT_CalSquareRoot(rcndx, rcndy, result) do{  \
                                                       MMDIVSQRT->RCNDX = (rcndx); \
                                                       MMDIVSQRT->RCNDY = (rcndy); \
                                                       while (MMDIVSQRT_IsBusy()); \
                                                       *(uint32_t *)(result) = MMDIVSQRT->RESULT;  \
                                                       }while(0)


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AC780X_MMDIVSQRT_REG_H */


/* =============================================  EOF  ============================================== */
