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
* @file CANBootloader.h
*
* @author AutoChips
*
* @version 1.1
*
* @date Oct.25.2018
*
* @brief Ginkgo USB-CAN适配器底层控制相关API函数定义.
*
*******************************************************************************/
#ifndef _CANBOOTLOADER_H_
#define _CANBOOTLOADER_H_

#include <stdint.h>
#include "ErrorType.h"
#include "ControlCAN.h"
#ifndef OS_UNIX
#include <Windows.h>
#endif

//通用错误码
#define	CBL_ERR_SUCCESS					(0)		//没有错误
#define	CBL_ERR_PARAMETER				(-1)	//参数传递错误
#define	CBL_ERR_OPEN_DEVICE				(-8)	//设备打开错误
#define	CBL_ERR_CLOSE_DEVICE			(-9)	//设备关闭错误
#define	CBL_ERR_CONFIG					(-20)	//配置设备错误
#define	CBL_ERR_DEV_TYPE				(-21)	//设备类型错误
#define	CBL_ERR_SEND					(-22)	//发送数据出错
#define	CBL_ERR_TIME_OUT				(-23)	//超时错误
#define	CBL_ERR_CMD						(-24)	//执行命令失败

typedef  struct  _CBL_CMD_LIST{
    uint8_t EraseFlash;			//擦出APP储存扇区数据
    uint8_t	WriteBlockFlash;	//以多字节形式写数据
    uint8_t	ReadBlockFlash;		//以多字节形式读数据
    uint8_t BlockWriteInfo;		//设置多字节写数据相关参数（写起始地址，数据量）
    uint8_t BlockReadInfo;		//设置多字节数据读相关参数（读起始地址，数据量）
    uint8_t	OnlineCheck;		//检测节点是否在线，同时返回固件信息
    uint8_t	CmdSuccess;			//命令执行成功
    uint8_t	CmdFaild;			//命令执行失败
    uint8_t	SetBaudRate;		//设置节点波特率
    uint8_t	ExcuteApp;			//执行应用程序
    uint8_t	EnterUpdateMode;	//进入升级模式
    uint8_t	Undefine0;			//未定义
    uint8_t	Undefine1;			//未定义
    uint8_t	Undefine2;			//未定义
    uint8_t	Undefine3;			//未定义
} CBL_CMD_LIST,*PCBL_CMD_LIST;

#ifdef __cplusplus
extern "C"
{
#endif

int32_t WINAPI CBL_ScanDevice(uint8_t NeedInit);
int32_t WINAPI CBL_OpenDevice(int32_t DevIndex);
int32_t WINAPI CBL_Config(int32_t DevIndex,int32_t CANIndex,PVCI_INIT_CONFIG_EX pInitConfig,PCBL_CMD_LIST pCmdList);
int32_t WINAPI CBL_NodeCheck(int32_t DevIndex,int32_t CANIndex,uint16_t NodeAddr,uint32_t *pVersion,uint32_t *pType,uint32_t TimeOut);
int32_t WINAPI CBL_EraseFlash(int32_t DevIndex,int32_t CANIndex,uint16_t NodeAddr,uint32_t FlashStartAddr,uint32_t FlashEndAddr,uint32_t TimeOut);
int32_t WINAPI CBL_WriteBlock(int32_t DevIndex,int32_t CANIndex,uint16_t NodeAddr,uint32_t FlashStartAddr,uint8_t *pData,uint32_t DataNum,uint32_t TimeOut);
int32_t WINAPI CBL_ExcuteApp(int32_t DevIndex,int32_t CANIndex,uint16_t NodeAddr,uint32_t FlashStartAddr);
int32_t WINAPI CBL_SetBaudRate(int32_t DevIndex,int32_t CANIndex,uint16_t NodeAddr,PVCI_INIT_CONFIG_EX pInitConfig,uint32_t BaudRate,uint32_t TimeOut);
int32_t WINAPI CBL_CloseDevice(int32_t DevIndex);
int32_t WINAPI CBL_EnterUpdateMode(int32_t DevIndex,int32_t CANIndex,uint16_t NodeAddr);

#ifdef __cplusplus
}
#endif

#endif
