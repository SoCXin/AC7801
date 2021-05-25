; /* Copyright Statement:
; *
; * This software/firmware and related documentation ("AutoChips Software") are
; * protected under relevant copyright laws. The information contained herein is
; * confidential and proprietary to AutoChips Inc. and/or its licensors. Without
; * the prior written permission of AutoChips inc. and/or its licensors, any
; * reproduction, modification, use or disclosure of AutoChips Software, and
; * information contained herein, in whole or in part, shall be strictly
; * prohibited.
; *
; * AutoChips Inc. (C) 2020. All rights reserved.
; *
; * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
; * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("AUTOCHIPS SOFTWARE")
; * RECEIVED FROM AUTOCHIPS AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
; * ON AN "AS-IS" BASIS ONLY. AUTOCHIPS EXPRESSLY DISCLAIMS ANY AND ALL
; * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
; * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
; * NONINFRINGEMENT. NEITHER DOES AUTOCHIPS PROVIDE ANY WARRANTY WHATSOEVER WITH
; * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
; * INCORPORATED IN, OR SUPPLIED WITH THE AUTOCHIPS SOFTWARE, AND RECEIVER AGREES
; * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
; * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
; * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN AUTOCHIPS
; * SOFTWARE. AUTOCHIPS SHALL ALSO NOT BE RESPONSIBLE FOR ANY AUTOCHIPS SOFTWARE
; * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
; * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AUTOCHIPS'S
; * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE AUTOCHIPS SOFTWARE
; * RELEASED HEREUNDER WILL BE, AT AUTOCHIPS'S OPTION, TO REVISE OR REPLACE THE
; * AUTOCHIPS SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
; * CHARGE PAID BY RECEIVER TO AUTOCHIPS FOR SUCH AUTOCHIPS SOFTWARE AT ISSUE.
; */

;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp               ; Top of Stack
                DCD     Reset_Handler              ; Reset Handler
                DCD     NMI_Handler                ; NMI Handler
                DCD     HardFault_Handler          ; Hard Fault Handler
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     SVC_Handler                ; SVCall Handler
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     PendSV_Handler             ; PendSV Handler
                DCD     SysTick_Handler            ; SysTick Handler

                ; External Interrupts
                DCD     PWDT0_IRQHandler           ;  0: PWDT0 interrupt
                DCD     PWDT1_IRQHandler           ;  1: PWDT1 interrupt
                DCD     PWM0_IRQHandler            ;  2: PWM0 interrupt
                DCD     PWM1_IRQHandler            ;  3: PWM1 interrupt
                DCD     ACMP0_IRQHandler           ;  4: ACMP0 interrupt
                DCD     UART0_IRQHandler           ;  5: UART0 interrupt
                DCD     UART1_IRQHandler           ;  6: UART1 interrupt
                DCD     UART2_IRQHandler           ;  7: UART2 interrupt
                DCD     WDG_IRQHandler             ;  8: WDG interrupt
                DCD     SPI0_IRQHandler            ;  9: SPI0 interrupt
                DCD     SPI1_IRQHandler            ; 10: SPI1 interrupt
                DCD     I2C0_IRQHandler            ; 11: I2C0 Interrupt
                DCD     I2C1_IRQHandler            ; 12: I2C1 Interrupt
                DCD     DMA0_Channel0_IRQHandler   ; 13: DMA0 channel 0 interrupt
                DCD     DMA0_Channel1_IRQHandler   ; 14: DMA0 channel 1 interrupt
                DCD     DMA0_Channel2_IRQHandler   ; 15: DMA0 channel 2 interrupt
                DCD     DMA0_Channel3_IRQHandler   ; 16: DMA0 channel 3 interrupt
                DCD     TIMER_Channel0_IRQHandler  ; 17: TIMER channel 0 interrupt
                DCD     TIMER_Channel1_IRQHandler  ; 18: TIMER channel 1 interrupt
                DCD     TIMER_Channel2_IRQHandler  ; 19: TIMER channel 2 interrupt
                DCD     TIMER_Channel3_IRQHandler  ; 20: TIMER channel 3 interrupt
                DCD     RTC_IRQHandler             ; 21: RTC Interrupt
                DCD     PVD_IRQHandler             ; 22: PVD Interrupt
                DCD     SPM_IRQHandler             ; 23: SPM interrupt
                DCD     CAN0_Handler               ; 24: CAN0 interrupt
                DCD     ADC0_IRQHandler            ; 25: ADC0 interrupt
                DCD     ECC_SRAM_IRQHandler        ; 26: ECC SRAM interrupt
                DCD     EXTI0_IRQHandler           ; 27: GPIOx PIN0 external interrupt
                DCD     EXTI1_IRQHandler           ; 28: GPIOx PIN1 external interrupt
                DCD     EXTI2_IRQHandler           ; 29: GPIOx PIN2 external interrupt
                DCD     EXTI3_8_IRQHandler         ; 30: GPIOx PIN3~8 external interrupt
                DCD     EXTI9_15_IRQHandler        ; 31: GPIOx PIN9~15 external interrupt
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main

                LDR     R0, =SystemInit
                BLX     R0

                LDR     R0, =0x20080014
                LDR     R1, =0xFFFF
                STRH    R1, [R0]

                LDR     R0, =0x20080044
                LDR     R1, =0xFFFF
                STRH    R1, [R0]

                LDR     R0, =0x20080074
                LDR     R1, =0x3FF
                STRH    R1, [R0]

                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  PWDT0_IRQHandler           [WEAK]
                EXPORT  PWDT1_IRQHandler           [WEAK]
                EXPORT  PWM0_IRQHandler            [WEAK]
                EXPORT  PWM1_IRQHandler            [WEAK]
                EXPORT  ACMP0_IRQHandler           [WEAK]
                EXPORT  UART0_IRQHandler           [WEAK]
                EXPORT  UART1_IRQHandler           [WEAK]
                EXPORT  UART2_IRQHandler           [WEAK]
                EXPORT  WDG_IRQHandler             [WEAK]
                EXPORT  SPI0_IRQHandler            [WEAK]
                EXPORT  SPI1_IRQHandler            [WEAK]
                EXPORT  I2C0_IRQHandler            [WEAK]
                EXPORT  I2C1_IRQHandler            [WEAK]
                EXPORT  DMA0_Channel0_IRQHandler   [WEAK]
                EXPORT  DMA0_Channel1_IRQHandler   [WEAK]
                EXPORT  DMA0_Channel2_IRQHandler   [WEAK]
                EXPORT  DMA0_Channel3_IRQHandler   [WEAK]
                EXPORT  TIMER_Channel0_IRQHandler  [WEAK]
                EXPORT  TIMER_Channel1_IRQHandler  [WEAK]
                EXPORT  TIMER_Channel2_IRQHandler  [WEAK]
                EXPORT  TIMER_Channel3_IRQHandler  [WEAK]
                EXPORT  RTC_IRQHandler             [WEAK]
                EXPORT  PVD_IRQHandler             [WEAK]
                EXPORT  SPM_IRQHandler             [WEAK]
                EXPORT  CAN0_Handler               [WEAK]
                EXPORT  ADC0_IRQHandler            [WEAK]
                EXPORT  ECC_SRAM_IRQHandler        [WEAK]
                EXPORT  EXTI0_IRQHandler           [WEAK]
                EXPORT  EXTI1_IRQHandler           [WEAK]
                EXPORT  EXTI2_IRQHandler           [WEAK]
                EXPORT  EXTI3_8_IRQHandler         [WEAK]
                EXPORT  EXTI9_15_IRQHandler        [WEAK]

PWDT0_IRQHandler
PWDT1_IRQHandler
PWM0_IRQHandler
PWM1_IRQHandler
ACMP0_IRQHandler
UART0_IRQHandler
UART1_IRQHandler
UART2_IRQHandler
WDG_IRQHandler
SPI0_IRQHandler
SPI1_IRQHandler
I2C0_IRQHandler
I2C1_IRQHandler
DMA0_Channel0_IRQHandler
DMA0_Channel1_IRQHandler
DMA0_Channel2_IRQHandler
DMA0_Channel3_IRQHandler
TIMER_Channel0_IRQHandler
TIMER_Channel1_IRQHandler
TIMER_Channel2_IRQHandler
TIMER_Channel3_IRQHandler
RTC_IRQHandler
PVD_IRQHandler
SPM_IRQHandler
CAN0_Handler
ADC0_IRQHandler
ECC_SRAM_IRQHandler
EXTI0_IRQHandler
EXTI1_IRQHandler
EXTI2_IRQHandler
EXTI3_8_IRQHandler
EXTI9_15_IRQHandler
                B       .

                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF


                END
