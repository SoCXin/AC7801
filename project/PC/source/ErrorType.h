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
/*******************************************************************************
*
* @file ErrorType.h
*
* @author AutoChips
*
* @version 1.1
*
* @date Oct.25.2018
*
* @brief CAN Bootloader Error type
*
*******************************************************************************/
#ifndef _ERRORTYPE_H_
#define _ERRORTYPE_H_

#include <stdint.h>
#ifndef OS_LINUX
#include <Windows.h>
#endif

//通用错误码
#define	ERR_SUCCESS					(0)		//没有错误
#define	ERR_PARAMETER_NULL			(-1)	//传入的指针为空指针
#define	ERR_INPUT_DATA_TOO_MUCH		(-2)	//参数输入个数多余规定个数
#define	ERR_INPUT_DATA_TOO_LESS		(-3)	//参数输入个数少余规定个数
#define	ERR_INPUT_DATA_ILLEGALITY	(-4)	//参数传入格式和规定的不符合
#define	ERR_USB_WRITE_DATA			(-5)	//USB写数据错误
#define	ERR_USB_READ_DATA			(-6)	//USB读数据错误
#define	ERR_READ_NO_DATA			(-7)	//请求读数据时返回没有数据
#define	ERR_OPEN_DEVICE				(-8)	//打开设备失败
#define	ERR_CLOSE_DEVICE			(-9)	//关闭设备失败
#define	ERR_EXECUTE_CMD				(-10)	//设备执行命令失败
#define ERR_SELECT_DEVICE			(-11)	//选择设备失败
#define	ERR_DEVICE_OPENED			(-12)	//设备已经打开
#define	ERR_DEVICE_NOTOPEN			(-13)	//设备没有打开
#define	ERR_BUFFER_OVERFLOW			(-14)	//缓冲区溢出
#define	ERR_DEVICE_NOTEXIST			(-15)	//此设备不存在
#define	ERR_LOAD_KERNELDLL			(-16)	//装载动态库失败
#define ERR_CMD_FAILED				(-17)	//执行命令失败错误码
#define	ERR_BUFFER_CREATE			(-18)	//内存不足

#ifdef __cplusplus
extern "C"
{
#endif
void WINAPI VTI_GetErrorString(int32_t ErrorValue,int8_t* pErrorString);
#ifdef __cplusplus
}
#endif

#endif
