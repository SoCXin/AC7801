/* Copyright Statement:
 *
 * This software/firmware and related documentation "Autochips Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Autochips Inc. and/or its licensors.
 * Without the prior written permission of Autochips inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of Autochips Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * Copyright(C) 2020, Autochips ( All rights reserved. )
 *
 * BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("Autochips SOFTWARE")
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
#ifndef _AC780X_SWEEPROM_H
#define _AC780X_SWEEPROM_H
/*!
* @file ac780x_sweeprom.h
*
* @brief This file provides software eeprom integration functions interface.
*
*/
#ifdef __cplusplus
extern "C" {
#endif

/* ===========================================  Includes  =========================================== */
#include "ac780x.h"
#include "ac780x_eflash.h"

/* ============================================  Define  ============================================ */

/* ====================================  Functions declaration  ===================================== */
/*!
 * @brief software eeprom initialize,actual available space are (Size - 2)
 *
 * @param[in] size: config software eeprom size(half word)
 * @param[in] pageIndex: the start page index in eflash occupied by software eeprom
 * @return none
 */
void SWEEPROM_Init(uint16_t size, uint32_t pageIndex);

/*!
 * @brief software eeprom Write
 *
 * @param[in] writeAddr: software eeprom address
 * @param[in] dataBuffer: data buffer pointer
 * @param[in] writeNum: write half word number
 * @return statusRes: software eeprom write status
 */
EFLASH_StatusType SWEEPROM_Write(uint16_t writeAddr, const uint16_t *dataBuffer, uint16_t writeNum);

/*!
 * @brief software eeprom read
 *
 * @param[in] readAddr: Read start address of software software eeprom
 * @param[in] dataBuffer: Point to the buffer ready to read the data from software eeorom
 * @param[in] readNum: read half word number
 * @return software eeprom read status
 */
EFLASH_StatusType SWEEPROM_Read(uint16_t readAddr, uint16_t *dataBuffer, uint16_t readNum);

/*!
 * @brief software eeprom erase(include backup area)
 *
 * @param[in] none
 * @return statusRes: software eeprom erase status
 */
EFLASH_StatusType SWEEPROM_Erase(void);

#ifdef __cplusplus
}
#endif

#endif /* _AC780X_SWEEPROM_H */

/* =============================================  EOF  ============================================== */
