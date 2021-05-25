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
* @file CANBootloader.cpp
*
* @author AutoChips
*
* @version 1.1
*
* @date Oct.26.2018
*
* @brief CANBootloader interface
*
*******************************************************************************/
#include "stdint.h"
#include "stdio.h"
#include "CANBootloader.h"
#include "ControlCAN.h"
#include "mainwindow.h"
#define CMD_WIDTH   4
#define CMD_MASK    0xF
#define CAN_ID_TYPE 1   //1为扩展帧，0为标准帧

CBL_CMD_LIST	CAN_BootCmdsList[20];
extern VCI_ERR_INFO	CAN_ErrorInfo[20][2];

#ifdef USE_GCC
extern "C" VTFREQFUNCSHARED_EXPORT
#endif
int32_t WINAPI CBL_ScanDevice(uint8_t NeedInit)
{
    return VCI_ScanDevice(NeedInit);
}
#ifdef USE_GCC
extern "C" VTFREQFUNCSHARED_EXPORT
#endif
int32_t WINAPI CBL_OpenDevice(int32_t DevIndex)
{
	int ret;
	char productInfo[100]={0};
    VCI_BOARD_INFO_EX BoardInfo;
	ret = VCI_OpenDevice(VCI_USBCAN2,DevIndex,0);
    qDebug()<<"VCI_OpenDevice ret = "<<ret;
	if(ret != STATUS_OK){
		//return CAN_ErrorInfo[DevIndex][0].ErrCode;
		return CBL_ERR_OPEN_DEVICE;
	}else{
        ret = VCI_ReadBoardInfoEx(DevIndex,&BoardInfo);
        //VTI_CheckProduct(DevIndex,(int8_t*)productInfo);
        qDebug()<<"VCI_ReadBoardInfoEx ret = "<<ret;
        sprintf(productInfo,"%s",BoardInfo.ProductName);
        qDebug()<<"ProductName="<<productInfo;
        if(strstr(productInfo,"Ginkgo_USB_CAN_Adaptor")==NULL){
			return CBL_ERR_DEV_TYPE;
		}else{
			return CBL_ERR_SUCCESS;
		}
	}
}

#ifdef USE_GCC
extern "C" VTFREQFUNCSHARED_EXPORT
#endif
int32_t WINAPI CBL_Config(int32_t DevIndex,int32_t CANIndex,PVCI_INIT_CONFIG_EX pInitConfig,PCBL_CMD_LIST pCmdList)
{
	int32_t ret;
	VCI_FILTER_CONFIG CANFilter;
	if(DevIndex>=10){
		return CBL_ERR_PARAMETER;
	}
	if(CANIndex>=2){
		return CBL_ERR_PARAMETER;
	}
	if(pInitConfig == NULL){
		return CBL_ERR_PARAMETER;
	}
	CAN_BootCmdsList[DevIndex] = *pCmdList;
	pInitConfig->CAN_Mode = 0;
	pInitConfig->CAN_NART = 0;
	pInitConfig->CAN_RELAY = 0x00;
	pInitConfig->CAN_RFLM = 0;
	pInitConfig->CAN_TXFP = 1;
	pInitConfig->CAN_ABOM = 1;
	ret = VCI_InitCANEx(VCI_USBCAN2,DevIndex,CANIndex,pInitConfig);
	if(ret != STATUS_OK){
		return CBL_ERR_CONFIG;
	}
	CANFilter.Enable = 1;
	CANFilter.ExtFrame = 1;
	CANFilter.FilterIndex = 0;
	CANFilter.FilterMode = 0;
	CANFilter.ID_IDE = CAN_ID_TYPE;
	CANFilter.ID_RTR = 0;
	CANFilter.ID_Std_Ext = 0;
	CANFilter.MASK_IDE = 1;
	CANFilter.MASK_RTR = 1;
	CANFilter.MASK_Std_Ext = 0;
	ret = VCI_SetFilter(VCI_USBCAN2,DevIndex,CANIndex,&CANFilter);
	if(ret != STATUS_OK){
		return CBL_ERR_CONFIG;
	}
	ret = VCI_StartCAN(VCI_USBCAN2,DevIndex,CANIndex);
	if(ret != STATUS_OK){
		return CBL_ERR_CONFIG;
	}
	return CBL_ERR_SUCCESS;
}

#ifdef USE_GCC
extern "C" VTFREQFUNCSHARED_EXPORT
#endif
int32_t WINAPI CBL_NodeCheck(int32_t DevIndex,int32_t CANIndex,uint16_t NodeAddr,uint32_t *pVersion,uint32_t *pType,uint32_t TimeOut)
{
    VCI_CAN_OBJ CAN_SendData;
    VCI_CAN_OBJ CAN_ReadData;
	int32_t ret;
	uint32_t Time = 0;
	if(DevIndex >= 10){
		return CBL_ERR_PARAMETER;
	}
	if(CANIndex >= 2){
		return CBL_ERR_PARAMETER;
	}
	if(NodeAddr == 0){
		return CBL_ERR_PARAMETER;
	}
	ret = VCI_ClearBuffer(VCI_USBCAN2,DevIndex,CANIndex);
	if(ret == STATUS_ERR){
		return CBL_ERR_SEND;
	}
    CAN_SendData.DataLen = 0;
	CAN_SendData.ID = (NodeAddr<<CMD_WIDTH)|(CAN_BootCmdsList[DevIndex].OnlineCheck&CMD_MASK);
    CAN_SendData.ExternFlag = CAN_ID_TYPE;
    CAN_SendData.RemoteFlag = 0;
    CAN_SendData.SendType = 0;
    ret = VCI_Transmit(VCI_USBCAN2,DevIndex,CANIndex,&CAN_SendData,1);
	if(ret == STATUS_ERR){
		return CBL_ERR_SEND;
	}
	while(1){
#ifndef OS_UNIX
        Sleep(10);
#else
        sleep(10);
#endif
		Time += 10;
		ret = VCI_Receive(VCI_USBCAN2,DevIndex,CANIndex,&CAN_ReadData,1);
		if((ret==1)&&(NodeAddr==(CAN_ReadData.ID>>CMD_WIDTH))&&((CAN_ReadData.ID&CMD_MASK)==CAN_BootCmdsList[DevIndex].OnlineCheck)){
			*pVersion = (CAN_ReadData.Data[0]<<24)|(CAN_ReadData.Data[1]<<16)|(CAN_ReadData.Data[2]<<8)|(CAN_ReadData.Data[3]<<0);
			*pType = (CAN_ReadData.Data[4]<<24)|(CAN_ReadData.Data[5]<<16)|(CAN_ReadData.Data[6]<<8)|(CAN_ReadData.Data[7]<<0);
			break;
		}
		if((ret==1)&&((CAN_ReadData.ID&CMD_MASK)!=CAN_BootCmdsList[DevIndex].OnlineCheck)){
			return CBL_ERR_CMD;
		}
		if(Time > TimeOut){
			return CBL_ERR_TIME_OUT;
		}
	}
	ret = VCI_ClearBuffer(VCI_USBCAN2,DevIndex,CANIndex);
	if(ret == STATUS_ERR){
		return CBL_ERR_SEND;
	}
	return CBL_ERR_SUCCESS;
}

#ifdef USE_GCC
extern "C" VTFREQFUNCSHARED_EXPORT
#endif
int32_t WINAPI CBL_EraseFlash(int32_t DevIndex,int32_t CANIndex,uint16_t NodeAddr,uint32_t FlashStartAddr,uint32_t FlashEndAddr,uint32_t TimeOut)
{
    VCI_CAN_OBJ CAN_SendData;
    VCI_CAN_OBJ CAN_ReadData;
	int32_t ret;
	uint32_t Time = 0;
	if(FlashStartAddr>FlashEndAddr){
		return CBL_ERR_PARAMETER;
	}
	if(DevIndex >= 10){
		return CBL_ERR_PARAMETER;
	}
	if(DevIndex >= 2){
		return CBL_ERR_PARAMETER;
	}
	ret = VCI_ClearBuffer(VCI_USBCAN2,DevIndex,CANIndex);
	if(ret == STATUS_ERR){
		return CBL_ERR_SEND;
	}
    CAN_SendData.Data[0] = FlashStartAddr>>24;
    CAN_SendData.Data[1] = FlashStartAddr>>16;
    CAN_SendData.Data[2] = FlashStartAddr>>8;
    CAN_SendData.Data[3] = FlashStartAddr>>0;
    CAN_SendData.Data[4] = FlashEndAddr>>24;
    CAN_SendData.Data[5] = FlashEndAddr>>16;
    CAN_SendData.Data[6] = FlashEndAddr>>8;
    CAN_SendData.Data[7] = FlashEndAddr>>0;
    CAN_SendData.DataLen = 8;
	CAN_SendData.ID = (NodeAddr<<CMD_WIDTH)|(CAN_BootCmdsList[DevIndex].EraseFlash&CMD_MASK);
    CAN_SendData.ExternFlag = CAN_ID_TYPE;
    CAN_SendData.RemoteFlag = 0;
    CAN_SendData.SendType = 0;
    ret = VCI_Transmit(VCI_USBCAN2,DevIndex,CANIndex,&CAN_SendData,1);
	if(ret == STATUS_ERR){
		return CBL_ERR_SEND;
	}
	while(NodeAddr!=0){
#ifndef OS_UNIX
		Sleep(10);
#else
        sleep(10);
#endif
		Time += 10;
		ret = VCI_Receive(VCI_USBCAN2,DevIndex,CANIndex,&CAN_ReadData,1);
		if((ret==1)&&(NodeAddr==(CAN_ReadData.ID>>CMD_WIDTH))&&((CAN_ReadData.ID&CMD_MASK)==CAN_BootCmdsList[DevIndex].EraseFlash)){
			break;
		}
		if((ret==1)&&((CAN_ReadData.ID&CMD_MASK)!=CAN_BootCmdsList[DevIndex].EraseFlash)){
			return CBL_ERR_CMD;
		}
		if(Time > TimeOut){
			return CBL_ERR_TIME_OUT;
		}
	}
	ret = VCI_ClearBuffer(VCI_USBCAN2,DevIndex,CANIndex);
	if(ret == STATUS_ERR){
		return CBL_ERR_SEND;
	}
	return CBL_ERR_SUCCESS;
}

#ifdef USE_GCC
extern "C" VTFREQFUNCSHARED_EXPORT
#endif
int32_t WINAPI CBL_WriteBlock(int32_t DevIndex,int32_t CANIndex,uint16_t NodeAddr,uint32_t FlashStartAddr,uint8_t *pData,uint32_t DataNum,uint32_t TimeOut)
{
    VCI_CAN_OBJ CAN_SendData[129];
    VCI_CAN_OBJ CAN_ReadData;
	int32_t ret;
	uint32_t Time = 0;
	if(DevIndex>=10){
		return CBL_ERR_PARAMETER;
	}
	if(DevIndex>=2){
		return CBL_ERR_PARAMETER;
	}
	if(DataNum > 1026){
		return CBL_ERR_PARAMETER;
	}
	ret = VCI_ClearBuffer(VCI_USBCAN2,DevIndex,CANIndex);
	if(ret == STATUS_ERR){
		return CBL_ERR_SEND;
	}
	//发送写数据相关信息
    CAN_SendData[0].Data[0] = FlashStartAddr>>24;
    CAN_SendData[0].Data[1] = FlashStartAddr>>16;
    CAN_SendData[0].Data[2] = FlashStartAddr>>8;
    CAN_SendData[0].Data[3] = FlashStartAddr>>0;
    CAN_SendData[0].Data[4] = DataNum>>24;
    CAN_SendData[0].Data[5] = DataNum>>16;
    CAN_SendData[0].Data[6] = DataNum>>8;
    CAN_SendData[0].Data[7] = DataNum>>0;
    CAN_SendData[0].DataLen = 8;
	CAN_SendData[0].ID = (NodeAddr<<CMD_WIDTH)|(CAN_BootCmdsList[DevIndex].BlockWriteInfo&CMD_MASK);
    CAN_SendData[0].ExternFlag = CAN_ID_TYPE;
    CAN_SendData[0].RemoteFlag = 0;
    CAN_SendData[0].SendType = 0;
    ret = VCI_Transmit(VCI_USBCAN2,DevIndex,CANIndex,&CAN_SendData[0],1);
	if(ret == STATUS_ERR){
		return CBL_ERR_SEND;
	}
	while(NodeAddr!=0){
#ifndef OS_UNIX
        Sleep(10);
#else
        sleep(10);
#endif
		Time += 10;
		CAN_ReadData.ID = 0;
		ret = VCI_Receive(VCI_USBCAN2,DevIndex,CANIndex,&CAN_ReadData,1);
		if((ret==1)&&(NodeAddr==(CAN_ReadData.ID>>CMD_WIDTH))&&((CAN_ReadData.ID&CMD_MASK)==CAN_BootCmdsList[DevIndex].BlockWriteInfo)){
			break;
		}
		if((ret==1)&&((CAN_ReadData.ID&CMD_MASK)!=CAN_BootCmdsList[DevIndex].BlockWriteInfo)){
			return CBL_ERR_CMD;
		}
		if(Time > TimeOut){
			return CBL_ERR_TIME_OUT;
		}
	}
	VCI_ClearBuffer(VCI_USBCAN2,DevIndex,CANIndex);
	//发送数据
	int i=0;
	for(i=0;i<(DataNum/8);i++){
		int j=0;
		for(j=0;j<8;j++){
			CAN_SendData[i].Data[j] = *pData++;
		}
		CAN_SendData[i].DataLen = 8;
		CAN_SendData[i].ExternFlag = CAN_ID_TYPE;
		CAN_SendData[i].ID = (NodeAddr<<CMD_WIDTH)|CAN_BootCmdsList[DevIndex].WriteBlockFlash;
		CAN_SendData[i].RemoteFlag = 0;
		CAN_SendData[i].SendType = 0;
	}
	if(DataNum%8){
		for(int j=0;j<(DataNum%8);j++){
			CAN_SendData[i].Data[j] = *pData++;
		}
		CAN_SendData[i].DataLen = DataNum%8;
		CAN_SendData[i].ExternFlag = CAN_ID_TYPE;
		CAN_SendData[i].ID = (NodeAddr<<CMD_WIDTH)|CAN_BootCmdsList[DevIndex].WriteBlockFlash;
		CAN_SendData[i].RemoteFlag = 0;
		CAN_SendData[i].SendType = 0;
		i++;
	}
	ret = VCI_Transmit(VCI_USBCAN2,DevIndex,CANIndex,CAN_SendData,i);
	if(ret == STATUS_ERR){
		return CBL_ERR_SEND;
	}
	while(NodeAddr!=0){
#ifndef OS_UNIX
        Sleep(10);
#else
        sleep(10);
#endif
		Time += 10;
		CAN_ReadData.ID = 0;
		ret = VCI_Receive(VCI_USBCAN2,DevIndex,CANIndex,&CAN_ReadData,1);
		if((ret==1)&&(NodeAddr==(CAN_ReadData.ID>>CMD_WIDTH))&&((CAN_ReadData.ID&CMD_MASK)==CAN_BootCmdsList[DevIndex].WriteBlockFlash)){
			break;
		}
		if((ret==1)&&((CAN_ReadData.ID&CMD_MASK)!=CAN_BootCmdsList[DevIndex].WriteBlockFlash)){
			return CBL_ERR_CMD;
		}
		if(Time > TimeOut){
			return CBL_ERR_TIME_OUT;
		}
	}
	ret = VCI_ClearBuffer(VCI_USBCAN2,DevIndex,CANIndex);
	if(ret == STATUS_ERR){
		return CBL_ERR_SEND;
	}
	return CBL_ERR_SUCCESS;
}

#ifdef USE_GCC
extern "C" VTFREQFUNCSHARED_EXPORT
#endif
int32_t WINAPI CBL_ExcuteApp(int32_t DevIndex,int32_t CANIndex,uint16_t NodeAddr,uint32_t FlashStartAddr)
{
    VCI_CAN_OBJ CAN_SendData;
    VCI_CAN_OBJ CAN_ReadData;
	int32_t ret;
	if(DevIndex>=10){
		return CBL_ERR_PARAMETER;
	}
	if(DevIndex>=2){
		return CBL_ERR_PARAMETER;
	}
    CAN_SendData.Data[0] = FlashStartAddr>>24;
    CAN_SendData.Data[1] = FlashStartAddr>>16;
    CAN_SendData.Data[2] = FlashStartAddr>>8;
    CAN_SendData.Data[3] = FlashStartAddr>>0;
    CAN_SendData.DataLen = 4;
	CAN_SendData.ID = (NodeAddr<<CMD_WIDTH)|(CAN_BootCmdsList[DevIndex].ExcuteApp&CMD_MASK);
    CAN_SendData.ExternFlag = CAN_ID_TYPE;
    CAN_SendData.RemoteFlag = 0;
    CAN_SendData.SendType = 0;
    ret = VCI_Transmit(VCI_USBCAN2,DevIndex,CANIndex,&CAN_SendData,1);
	if(ret == STATUS_ERR){
		return CBL_ERR_SEND;
	}
	return CBL_ERR_SUCCESS;
}

#ifdef USE_GCC
extern "C" VTFREQFUNCSHARED_EXPORT
#endif
int32_t WINAPI CBL_EnterUpdateMode(int32_t DevIndex,int32_t CANIndex,uint16_t NodeAddr)
{
    VCI_CAN_OBJ CAN_SendData;
    VCI_CAN_OBJ CAN_ReadData;
    int32_t ret;
    if(DevIndex>=10){
        return CBL_ERR_PARAMETER;
    }
    if(DevIndex>=2){
        return CBL_ERR_PARAMETER;
    }
    CAN_SendData.DataLen = 0;
    CAN_SendData.ID = (NodeAddr<<CMD_WIDTH)|(CAN_BootCmdsList[DevIndex].EnterUpdateMode&CMD_MASK);
    qDebug()<<"CAN_SendData.ID = "<<CAN_SendData.ID;
    CAN_SendData.ExternFlag = CAN_ID_TYPE;
    CAN_SendData.RemoteFlag = 0;
    CAN_SendData.SendType = 0;
    ret = VCI_Transmit(VCI_USBCAN2,DevIndex,CANIndex,&CAN_SendData,1);
    if(ret == STATUS_ERR){
        return CBL_ERR_SEND;
    }
    return CBL_ERR_SUCCESS;
}

#ifdef USE_GCC
extern "C" VTFREQFUNCSHARED_EXPORT
#endif
int32_t WINAPI CBL_SetBaudRate(int32_t DevIndex,int32_t CANIndex,uint16_t NodeAddr,PVCI_INIT_CONFIG_EX pInitConfig,uint32_t BaudRate,uint32_t TimeOut)
{
    VCI_CAN_OBJ CAN_SendData;
    VCI_CAN_OBJ CAN_ReadData;
	VCI_FILTER_CONFIG CANFilter;
	int32_t ret,Time=0;
	if(DevIndex>=10){
		return CBL_ERR_PARAMETER;
	}
	if(DevIndex>=2){
		return CBL_ERR_PARAMETER;
	}
	ret = VCI_ClearBuffer(VCI_USBCAN2,DevIndex,CANIndex);
	if(ret == STATUS_ERR){
		return CBL_ERR_CMD;
	}
    CAN_SendData.Data[0] = BaudRate>>24;
    CAN_SendData.Data[1] = BaudRate>>16;
    CAN_SendData.Data[2] = BaudRate>>8;
    CAN_SendData.Data[3] = BaudRate>>0;
    CAN_SendData.DataLen = 4;
	CAN_SendData.ID = (NodeAddr<<CMD_WIDTH)|(CAN_BootCmdsList[DevIndex].SetBaudRate&CMD_MASK);
    CAN_SendData.ExternFlag = CAN_ID_TYPE;
    CAN_SendData.RemoteFlag = 0;
    CAN_SendData.SendType = 0;
    ret = VCI_Transmit(VCI_USBCAN2,DevIndex,CANIndex,&CAN_SendData,1);
	if(ret == STATUS_ERR){
		return CBL_ERR_SEND;
	}
	ret = VCI_ResetCAN(VCI_USBCAN2,DevIndex,CANIndex);
	if(ret == STATUS_ERR){
		return CBL_ERR_SEND;
	}
	pInitConfig->CAN_Mode = 0;
	pInitConfig->CAN_NART = 0;
	pInitConfig->CAN_RELAY = 0x00;
	pInitConfig->CAN_RFLM = 0;
	pInitConfig->CAN_TXFP = 1;
	pInitConfig->CAN_ABOM = 1;
	ret = VCI_InitCANEx(VCI_USBCAN2,DevIndex,CANIndex,pInitConfig);
	if(ret != STATUS_OK){
		return CBL_ERR_CONFIG;
	}
	CANFilter.Enable = 1;
	CANFilter.ExtFrame = 1;
	CANFilter.FilterIndex = 0;
	CANFilter.FilterMode = 0;
	CANFilter.ID_IDE = CAN_ID_TYPE;
	CANFilter.ID_RTR = 0;
	CANFilter.ID_Std_Ext = 0;
	CANFilter.MASK_IDE = 1;
	CANFilter.MASK_RTR = 1;
	CANFilter.MASK_Std_Ext = 0;
	ret = VCI_SetFilter(VCI_USBCAN2,DevIndex,CANIndex,&CANFilter);
	if(ret != STATUS_OK){
		return CBL_ERR_CONFIG;
	}
	ret = VCI_StartCAN(VCI_USBCAN2,DevIndex,CANIndex);
	if(ret == STATUS_ERR){
		return CBL_ERR_SEND;
	}
	while(NodeAddr!=0){
#ifndef OS_UNIX
        Sleep(10);
#else
        sleep(10);
#endif
		Time += 10;
		ret = VCI_Receive(VCI_USBCAN2,DevIndex,CANIndex,&CAN_ReadData,1);
		if((ret==1)&&(NodeAddr==(CAN_ReadData.ID>>CMD_WIDTH))&&((CAN_ReadData.ID&CMD_MASK)==CAN_BootCmdsList[DevIndex].SetBaudRate)){
			break;
		}
		if((ret==1)&&((CAN_ReadData.ID&CMD_MASK)!=CAN_BootCmdsList[DevIndex].SetBaudRate)){
			return CBL_ERR_CMD;
		}
		if(Time > TimeOut){
			return CBL_ERR_TIME_OUT;
		}
	}
	return CBL_ERR_SUCCESS;
}



#ifdef USE_GCC
extern "C" VTFREQFUNCSHARED_EXPORT
#endif
int32_t WINAPI CBL_CloseDevice(int32_t DevIndex)
{
	int ret;
	VCI_ResetCAN(VCI_USBCAN2,DevIndex,0);
	VCI_ResetCAN(VCI_USBCAN2,DevIndex,1);
	ret =  VCI_CloseDevice(VCI_USBCAN2,DevIndex);
	if(ret != STATUS_OK){
		return CBL_ERR_CLOSE_DEVICE;
	}else{
		return CBL_ERR_SUCCESS;
	}
}
