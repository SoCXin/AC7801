/* Copyright Statement:
 *
 * This software/firmware and related documentation ("AutoChips Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to AutoChips Inc. and/or its licensors. Without
 * the prior written permission of AutoChips inc. and/or its licensors,  any
 * reproduction,  modification,  use or disclosure of AutoChips Software,  and
 * information contained herein,  in whole or in part,  shall be strictly
 * prohibited.
 *
 * AutoChips Inc. (C) 2018. All rights reserved.
 *
 * BY OPENING THIS FILE,  RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("AUTOCHIPS SOFTWARE")
 * RECEIVED FROM AUTOCHIPS AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. AUTOCHIPS EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES,  EXPRESS OR IMPLIED,  INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES AUTOCHIPS PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, 
 * INCORPORATED IN,  OR SUPPLIED WITH THE AUTOCHIPS SOFTWARE,  AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN AUTOCHIPS
 * SOFTWARE. AUTOCHIPS SHALL ALSO NOT BE RESPONSIBLE FOR ANY AUTOCHIPS SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AUTOCHIPS'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE AUTOCHIPS SOFTWARE
 * RELEASED HEREUNDER WILL BE,  AT AUTOCHIPS'S OPTION,  TO REVISE OR REPLACE THE
 * AUTOCHIPS SOFTWARE AT ISSUE,  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO AUTOCHIPS FOR SUCH AUTOCHIPS SOFTWARE AT ISSUE.
 */

/*************<start>******************/


/*************<include>****************/
#include "ac780x_eflash.h" 
#include "ac780x_gpio.h"
#include "ac780x_eflash_reg.h"
/*************<macro>******************/
#define TestAddressStart    0x08010000
#define TestPageNum         ((TestAddressStart - 0x08000000)/2048)
#define TestSize            0x1000  ///<4K

#define GPIO_KEY1   GPIOC,GPIO_PIN6
#define GPIO_KEY2   GPIOA,GPIO_PIN11
/*************<enum>*******************/


/*************<union>******************/


/*************<struct>*****************/


/*************<variable>***************/
uint32_t wBuff[512];

/*************<prototype>**************/
void GPIO_KeyInit(void)
{
    GPIO_SetDir(GPIO_KEY1, GPIO_IN);
    GPIO_SetDir(GPIO_KEY2, GPIO_IN);
}
/**
* @prototype main(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 ʵ��eflash��option byte����
*/
int main(void)
{
    static uint8_t key1_down = 0;
    static uint8_t key2_down = 0;
    static uint8_t wprotectStatus = 0;  ///<д����״̬
    static uint8_t rprotectStatus = 0;  ///<������״̬
    EFLASH_StatusType ret;
    uint16_t i;
    uint8_t *pdata = (uint8_t *)wBuff;
    InitDebug();
    InitDelay();
    
    GPIO_KeyInit();
    
    if (1 == EFLASH_GetReadProtectReg())
    {
        printf ("eflash is in read protect\r\n");
        rprotectStatus = 1;
    }
    else
    {
        printf ("eflash is not in read protect\r\n");
        rprotectStatus = 0;
    }
    printf ("eflash write protect status is %08x %08x\r\n", EFLASH_GetWriteProReg1(), EFLASH_GetWriteProReg2()); 
    if ((EFLASH_GetWriteProReg2() & 0x0001) == 0x0001)  ///<��Ӧ��32ҳ����0x08010000��ʼ��ַҳ
    {
        wprotectStatus = 1;
    }
    for (i=0;i<TestSize;i++)
    {
        pdata[i] = (uint8_t)i;
    }
    
    ret = EFLASH_UnlockCtrl();  ///<unlock flash,��flash�Ĳ�д����������unlock
    if (ret != EFLASH_STATUS_SUCCESS)
    {
        printf ("line%d: eflash error %d\r\n", __LINE__, ret);  
    }
    ret = EFLASH_PageErase(TestAddressStart);   ///<����һ��ҳ����ַΪ��ǰҳ�׵�ַ
    if (ret != EFLASH_STATUS_SUCCESS)
    {
        printf ("line%d: eflash error %d\r\n", __LINE__, ret);  
    }
    ret = EFLASH_PageEraseVerify(TestAddressStart); ///<��֤��ǰҳ�Ƿ�����ɹ�
    if (ret != EFLASH_STATUS_SUCCESS)
    {
        printf ("line%d: eflash error %d\r\n", __LINE__, ret);  
    }
    ///д������ֻ����д�������ĵ�ַ��һ������512��(2K byte)��buff�������ֽڶ��롣
    ret = EFLASH_PageProgram(TestAddressStart, wBuff, 512); 
    if (ret != EFLASH_STATUS_SUCCESS)
    {
        printf ("line%d: eflash error %d\r\n", __LINE__, ret);  
    }
    EFLASH_LockCtrl();
    
    
    
	while(1)
	{
        if (GPIO_LEVEL_LOW == GPIO_GetPinLevel(GPIO_KEY1))
        {
            if (key1_down == 0) ///<��ֹ��μ��
            {
                mdelay(10); ///<�ӳ�����
                if (GPIO_LEVEL_LOW == GPIO_GetPinLevel(GPIO_KEY1))
                {
                    key1_down = 1;
                    ret = EFLASH_UnlockCtrl();  ///<unlock flash,��flash�Ĳ�д����������unlock
                    if (ret != EFLASH_STATUS_SUCCESS)
                    {
                        printf ("line%d: eflash error %d\r\n", __LINE__, ret);  
                        return -1;
                    }
                    if (rprotectStatus == 1)    ///<���ڶ��������������
                    {
                        ///�����������eflash����ᱻ��գ�������ִ������������ܷɡ���Ҫע��
                        ret = EFLASH_DisableReadOut();      
                        if (ret != EFLASH_STATUS_SUCCESS)
                        {
                            printf ("line%d: eflash error %d\r\n", __LINE__, ret);  
                            return -1;
                        }
                        rprotectStatus = 0;
                        printf("disable read protect\r\n");
                    }
                    else
                    {
                        ///���ö���������Ҫ���¶ϵ��ϵ������Ч
                        ret = EFLASH_EnableReadOut();
                        if (ret != EFLASH_STATUS_SUCCESS)
                        {
                            printf ("line%d: eflash error %d\r\n", __LINE__, ret);  
                            return -1;
                        }
                        rprotectStatus = 1;
                        printf("enable read protect\r\n");
                    }
                    EFLASH_LockCtrl();
                }
            }
        }
        else
        {
            key1_down = 0;
        }
        
        if (GPIO_LEVEL_LOW == GPIO_GetPinLevel(GPIO_KEY2))
        {
            if (key2_down == 0) ///<��ֹ��μ��
            {
                mdelay(10);
                if (GPIO_LEVEL_LOW == GPIO_GetPinLevel(GPIO_KEY2))
                {
                    key2_down = 1;
                    ret = EFLASH_UnlockCtrl();  ///<unlock flash,��flash�Ĳ�д����������unlock
                    if (ret != EFLASH_STATUS_SUCCESS)
                    {
                        printf ("line%d: eflash error %d\r\n", __LINE__, ret);  
                        return -1;
                    }
                    if (wprotectStatus == 1)    ///<����д�����������������Ҫ������Ч
                    {
                        ret = EFLASH_SetWriteProtect(TestPageNum, 1, DISABLE);   ///<�����ҳд����
                        if (ret != EFLASH_STATUS_SUCCESS)
                        {
                            printf ("line%d: eflash error %d\r\n", __LINE__, ret);  
                            return -1;
                        }
                        wprotectStatus = 0;
                        printf("disable write protect\r\n");
                    }
                    else
                    {
                        ret = EFLASH_SetWriteProtect(TestPageNum, 1, ENABLE);   ///<���ø�ҳд����
                        if (ret != EFLASH_STATUS_SUCCESS)
                        {
                            printf ("line%d: eflash error %d\r\n", __LINE__, ret);  
                            return -1;
                        }
                        wprotectStatus = 1;
                        printf("enable write protect\r\n");
                    }
                    EFLASH_LockCtrl();
                }
            }
        }
        else
        {
            key2_down = 0;
        }
	}
}
/*************<end>********************/
