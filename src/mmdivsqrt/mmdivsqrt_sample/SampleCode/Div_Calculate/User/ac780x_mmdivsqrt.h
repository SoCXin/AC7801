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

#ifndef _AC780X_MMDIVSQRT_H
#define _AC780X_MMDIVSQRT_H

/*!
* @file ac78xx_mmdivsqrt.h
*
* @brief MMDIVSQRT access register inline function definition.
*
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* ===========================================  Includes  =========================================== */
#include "ac780x.h"

/* ============================================  Define  ============================================ */
/*!< MMDIVSQRT Division caculate result is quotient or remainder */
typedef enum
{
    MMDIVSQRT_DIV_RESULT_QUOTIENT = 0U,   /*!< Division result is quotient */
    MMDIVSQRT_DIV_RESULT_REMAINDER,       /*!< Division result is remainder */
}MMDIVSQRT_DivResultType;
/*!< Check function parameter: Is MMDIVSQRT Div result mode or not */
#define IS_MMDIVSQRT_DIV_RESULT_MODE(resultMode) (((resultMode) == MMDIVSQRT_DIV_RESULT_QUOTIENT) || \
                                               ((resultMode) == MMDIVSQRT_DIV_RESULT_REMAINDER))

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */
/*!
* @brief Division prototype: (x << z ) / y,   calculate signed division and get result
*
* @param[in] dend: Dividend value
* @param[in] dsor: Divisor value
* @param[in] dsft: Dividend shift num, its value can be 0~31
* @param[in] rem: Division result type, 0: result is quotient   1: result is remainder
* @return result: get calculate result
*/
int32_t MMDIVSQRT_CalculateSignDivision(int32_t dend, int32_t dsor, uint8_t dsft, MMDIVSQRT_DivResultType rem);

/*!
* @brief Division prototype: (x << z)/ y, calculate signed division and get result
*
* @param[in] dend: Dividend value
* @param[in] dsor: Divisor value
* @param[in] dsft: Dividend shift num, its value can be 0~31
* @param[in] rem: Division result type, 0: result is quotient   1: result is remainder
* @return result: get calculate result
*/
uint32_t MMDIVSQRT_CalculateUnsignDivision(uint32_t dend, uint32_t dsor, uint8_t dsft, MMDIVSQRT_DivResultType rem);

/*!
* @brief Square root prototype: ¡Ì(x^2+y^2), calculate square root and get result
*
* @param[in] rcndx: Radicand x value
* @param[in] rcndy: Radicand y value
* @return output result
*/
uint32_t MMDIVSQRT_CalculateSquareRoot(int32_t rcndx, int32_t rcndy);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AC780X_MMDIVSQRT_H */

/* =============================================  EOF  ============================================== */
