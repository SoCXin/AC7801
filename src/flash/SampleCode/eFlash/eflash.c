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
/*************<macro>******************/
#define TestAddressStart    0x08010000
#define TestSize            0x1000  ///<4K
/*************<enum>*******************/


/*************<union>******************/


/*************<struct>*****************/


/*************<variable>***************/
uint32_t wBuff[512];

/*************<prototype>**************/

/**
* @prototype main(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 实现eflash的基本擦写操作
*/
int main(void)
{
    EFLASH_StatusType ret;
    uint16_t i;
    uint8_t *pdata = (uint8_t *)wBuff;
    const uint8_t *cdata = (const uint8_t *)TestAddressStart;
    InitDebug();
    for (i=0;i<TestSize;i++)
    {
        pdata[i] = (uint8_t)i;
    }
    ret = EFLASH_UnlockCtrl();  ///<unlock flash,对flash的擦写操作必须先unlock
    if (ret != EFLASH_STATUS_SUCCESS)
    {
        printf ("line%d: eflash error %d\r\n", __LINE__, ret);  
        return -1;
    }
    ret = EFLASH_PageErase(TestAddressStart);   ///<擦除一个页，地址为当前页首地址
    if (ret != EFLASH_STATUS_SUCCESS)
    {
        printf ("line%d: eflash error %d\r\n", __LINE__, ret);  
        EFLASH_LockCtrl();
        return -1;
    }
    ret = EFLASH_PageEraseVerify(TestAddressStart); ///<验证当前页是否擦除成功
    if (ret != EFLASH_STATUS_SUCCESS)
    {
        printf ("line%d: eflash error %d\r\n", __LINE__, ret);  
        EFLASH_LockCtrl();
        return -1;
    }
    ret = EFLASH_PageErase(TestAddressStart+0x800);
    if (ret != EFLASH_STATUS_SUCCESS)
    {
        printf ("line%d: eflash error %d\r\n", __LINE__, ret);  
        EFLASH_LockCtrl();
        return -1;
    }
    ret = EFLASH_PageEraseVerify(TestAddressStart+0x800);
    if (ret != EFLASH_STATUS_SUCCESS)
    {
        printf ("line%d: eflash error %d\r\n", __LINE__, ret);  
        EFLASH_LockCtrl();
        return -1;
    }
    ///写操作，只可以写擦除过的地址，一次限制512字(2K byte)，buff必须四字节对齐。
    ret = EFLASH_PageProgram(TestAddressStart, wBuff, 512); 
    if (ret != EFLASH_STATUS_SUCCESS)
    {
        printf ("line%d: eflash error %d\r\n", __LINE__, ret);  
        EFLASH_LockCtrl();
        return -1;
    }
    ret = EFLASH_PageProgram(TestAddressStart+0x800, wBuff, 512); 
    if (ret != EFLASH_STATUS_SUCCESS)
    {
        printf ("line%d: eflash error %d\r\n", __LINE__, ret);  
        EFLASH_LockCtrl();
        return -1;
    }
    EFLASH_LockCtrl();
    printf("read address 0x%08x %d bytes: \r\n", TestAddressStart, TestSize);
    for (i=0;i<TestSize;i++)
    {
        printf("%02x ", cdata[i]);
        if (i%16 == 15)
        {
            printf("\r\n");  
        }
    }
	while(1)
	{
	}
}
/*************<end>********************/

