/* Copyright Statement:
*
* This software/firmware and related documentation ("Autochips Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to Autochips Inc. and/or its licensors.
* Without the prior written permission of Autochips inc. and/or its licensors,
* any reproduction, modification, use or disclosure of Autochips Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*
* Copyright(C) 2020, Autochips ( All rights reserved. )
*
* BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("AUTOCHIPS SOFTWARE")
* RECEIVED FROM Autochips AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
* AN "AS-IS" BASIS ONLY. Autochips EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES Autochips PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH THE Autochips SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. Autochips SHALL ALSO
* NOT BE RESPONSIBLE FOR ANY Autochips SOFTWARE RELEASES MADE TO BUYER'S
* SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
* BUYER'S SOLE AND EXCLUSIVE REMEDY AND Autochips'S ENTIRE AND CUMULATIVE
* LIABILITY WITH RESPECT TO THE Autochips SOFTWARE RELEASED HEREUNDER WILL BE,
* AT Autochips'S OPTION, TO REVISE OR REPLACE THE Autochips SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*
* Autochips FOR SUCH Autochips SOFTWARE AT ISSUE.
*
* THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
* WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
* LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
* RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
* THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*****************************************************************************/
/*!
* @file ac780x_mmdivsqrt.c
*
* @brief This file provides MMDIVSQRT integration functions.
*
*/

/* ===========================================  Includes  =========================================== */
#include "ac780x_mmdivsqrt_reg.h"


/* ============================================  Define  ============================================ */

/* ===========================================  Typedef  ============================================ */

/* ==========================================  Variables  =========================================== */

/* ====================================  Functions declaration  ===================================== */

/* ======================================  Functions define  ======================================== */
/*!
* @brief Division prototype: (x << z) / y,   calculate signed division and get result
*
* @param[in] dend: Dividend value
* @param[in] dsor: Divisor value
* @param[in] dsft: Dividend shift num, its value can be 0~31
* @param[in] rem: Division result type, 0: result is quotient   1: result is remainder
* @return result: get calculate result
*/
int32_t MMDIVSQRT_CalculateSignDivision(int32_t dend, int32_t dsor, uint8_t dsft, MMDIVSQRT_DivResultType rem)
{
    DEVICE_ASSERT(IS_MMDIVSQRT_DIV_RESULT_MODE(rem));

    MMDIVSQRT_SetDivisionUsign(0);       /* set MMDIVSQRT Division is signed type */
    MMDIVSQRT_SetDivisionRem(rem);       /* Set MMDIVSQRT Division caculate result is quotient or remainder */

    MMDIVSQRT_SetDsft(dsft);        /* set dividend shift register */
    MMDIVSQRT_SetDend(dend);    /* set dividend */
    MMDIVSQRT_SetDsor(dsor);    /* set divisor and auto start calculate */

    while(MMDIVSQRT_IsBusy());      /* wait for the result */

    return (MMDIVSQRT_GetResult());    /* get result */
}

/*!
* @brief Division prototype: (x << z)/ y, calculate signed division and get result
*
* @param[in] dend: Dividend value
* @param[in] dsor: Divisor value
* @param[in] dsft: Dividend shift num, its value can be 0~31
* @param[in] rem: Division result type, 0: result is quotient   1: result is remainder
* @return result: get calculate result
*/
uint32_t MMDIVSQRT_CalculateUnsignDivision(uint32_t dend, uint32_t dsor, uint8_t dsft, MMDIVSQRT_DivResultType rem)
{
    DEVICE_ASSERT(IS_MMDIVSQRT_DIV_RESULT_MODE(rem));

    MMDIVSQRT_SetDivisionUsign(1);        /* set MMDIVSQRT Division is unsigned type */
    MMDIVSQRT_SetDivisionRem(rem);        /* Set MMDIVSQRT Division caculate result is quotient or remainder */

    MMDIVSQRT_SetDsft(dsft);         /* set dividend shift register */
    MMDIVSQRT_SetDend(dend);    /* set dividend */
    MMDIVSQRT_SetDsor(dsor);    /* set divisor and auto start calculate */

    while(MMDIVSQRT_IsBusy());       /* wait for the result */

    return (MMDIVSQRT_GetResult());   /* get result */
}

/*!
* @brief Square root prototype: v(x^2+y^2), calculate square root and get result
*
* @param[in] rcndx: Radicand x value
* @param[in] rcndy: Radicand y value
* @return output result
*/
uint32_t MMDIVSQRT_CalculateSquareRoot(int32_t rcndx, int32_t rcndy)
{
    MMDIVSQRT_SetRcndx(rcndx);
    MMDIVSQRT_SetRcndy(rcndy);

    while(MMDIVSQRT_IsBusy());             /* wait for the result */

    return (MMDIVSQRT_GetResult());   /* get result */
}

/* =============================================  EOF  ============================================== */

