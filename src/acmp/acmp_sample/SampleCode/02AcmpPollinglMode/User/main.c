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
 * AutoChips Inc. (C) 2016. All rights reserved.
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
 /****************************************************************************

 I.AC7801x ACMPģ�����̣������̻���CMSIS 1.x.x�汾�ӿڿ������ԣ�
 ����2��ACMP��ѯģʽ
 ACMP_IN3(PA8)��Ϊ�������룬ACMP_IN0(PA11),ACMP_IN1(PA10),ACMP_IN2(PA9)��Ϊ�������롣
 ͨ����ѯ��ʽ�Ƚ�ACMP �������롣
 
 *****************************************************************************/
/**********<Start>**********/

/**********<Incldue>**********/
#include "acmp_sample.h"
#include "pwdt_sample.h"
#include "pwm_sample.h"
/**********<Macro>**********/

/**********<Union>**********/

/**********<Struct>*********/

/*********<Variable>********/

/********<Prototype>********/

 /**
*
* @param[in] none
*
* @return none
*
* @brief �ڵ��Demo����ͨ��BLDC������ʽת�������ͨ��ACMP��ѯģʽ������ķ��綯�ƣ�ͨ��PWDT����ACMP������źš�
*/
int main(void)
{
    InitDebug();
    InitDelay();
    mdelay(5000);
    printf("\nRunning the acmp polling mode sample code.\r\n");
    //PWDTģ������
    PWDT_ThreeChannelInputHallstatus();
    //ACMPģ������
    ACMP_PollingLogicSample();
    //PWMģ�����ã�������������DEMOС���
    PWM_SampleBldcControl();
   
}

/**********<End>*********/
