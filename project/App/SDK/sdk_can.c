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
* @file sdk_can.c
*
* @author AutoChips
*
* @version 0.0.1
*
* @date Mar.11.2019
*
* @brief 
*       1. CAN总线升级功能实现
*
*******************************************************************************/
#include "board_config.h"
#include "sdk_can.h"
#include "sdk_interruptTask.h"
CAN_Type *g_CANx = CAN0;
CAN_TransmitBufferType g_type = CAN_TRANSMIT_SECONDARY;

CAN_MsgInfoType g_CANRxMessage;
CAN_MsgInfoType g_CANTxMessage;
uint32_t g_canTxDataBuff[2]; //数据地址必须4字节对齐，否则会导致hardfalut
uint32_t g_canRxDataBuff[2]; //数据地址必须4字节对齐，否则会导致hardfalut
volatile uint8_t g_canRxMsgFlag=0;//接收到CAN数据后的标志

CBL_CMD_LIST g_cmdList = {0};
uint8_t g_enterUpgrade = 0;
/**
 * CAN_SetCmdList
 *
* @param[in]  void
 * @return : void
 *
 * @brief  set cmd list
*/
void CAN_SetCmdList(void)
{
    g_cmdList.OnlineCheck = ONLINE_CHECK;
    g_cmdList.EraseFlash = ERASE_FLASH;
    g_cmdList.SetBaudRate = SET_BAUDRATE;
    g_cmdList.BlockWriteInfo = BLOCK_WRITE_INFO;
    g_cmdList.WriteBlockFlash = WRITE_BLOCK_FLASH;
    g_cmdList.BlockReadInfo = BLOCK_READ_INFO;
    g_cmdList.ReadBlockFlash = READ_BLOCK_FLASH;
    g_cmdList.ExcuteApp = EXCUTE_APP;
    g_cmdList.CmdFaild = CMD_FAILD;
    g_cmdList.EnterUpdateMode = ENTER_UPDATE_MODE;
    g_cmdList.CmdSuccess = CMD_SUCCESS;
}
/**
 * CAN_ReadMsg
 *
* @param[in]  CANx: CAN type pointer , CAN_MsgInfoType:rx message 
 * @return : error (0: read success, 1: no message)
 *
 * @brief  Read CANx message
 */
int32_t CAN_ReadMsg(CAN_Type *CANx, CAN_MsgInfoType* RxMessage)
{
    if (CAN_ReceiveMessage(CANx, RxMessage))
    {
        return 1;
    }
    return 0;
}

/**
 * CAN_WriteMsg
 *
 * @param[in]  CANx: CAN type pointer  ,CAN_MsgInfoType:TxMessage
 * @return : error (0: send success, 1:  fail or timeout)
 *
 * @brief  send CANx message
 */
int32_t CAN_WriteMsg(CAN_Type *CANx, CAN_MsgInfoType* TxMessage)
{
    if (CAN_TransmitMessage(g_CANx, TxMessage, g_type))
    {
        return 1;
    }
    return 0;
}



void CAN_Initialize(void)
{
    CAN_ConfigType canConfig = {0};
	CAN_BitrateConfigType canBandrateConfig = {0}; 
	/*
	tSeg1 = (S_SEG_1 + 2); tSeg2 = (S_SEG_2 + 1).
	BandRate 	= (48M / (S_PRESC + 1) / ((S_SEG_1 + 2) + (S_SEG_2 + 1)))
	SamplePoint = (tSeg1 / (tSeg1 + tSeg2)).
	 */
	canBandrateConfig.PRESC 	= 5;
	canBandrateConfig.SEG_1 	= 11;
	canBandrateConfig.SEG_2 	= 2;
	canBandrateConfig.SJW 	    = 2;
	
	canConfig.interruptEn = ENABLE;
    canConfig.interruptMask = CAN_IRQ_ALL_ENABLE_MSK;  
	canConfig.canMode = CAN_MODE_NORMAL;
    canConfig.clockSrc = CAN_CLKSRC_AHB;
    canConfig.callback = CAN0_IRQnCallBack;
    canConfig.normalBitrate = &canBandrateConfig;
	CAN_Init(CAN0, &canConfig);
    
    g_CANRxMessage.DATA = (uint8_t *)g_canRxDataBuff;
    g_CANTxMessage.DATA = (uint8_t *)g_canTxDataBuff;
}
/**
 * CAN_Config
 *
 * @param[in]  none
 * @return     none
 *
 * @brief  CAN初始化
 */
void CAN_Config(void)
{
    GPIO_SetFunc(CAN0_TX, GPIO_FUN1);
    GPIO_SetFunc(CAN0_RX, GPIO_FUN1);
    GPIO_SetFunc(CAN0_STB, GPIO_FUN0);
    GPIO_SetDir(CAN0_STB, GPIO_OUT);
    GPIO_SetPinLevel(CAN0_STB, GPIO_LEVEL_LOW);
    CAN_Initialize();
    CAN_SetCmdList();
}
/**
 * CAN_BootGetAddrData
 *
 * @param[in] none
 * @return : 节点地址。
 *
 * @brief  获取节点地址信息
 */

uint16_t CAN_BootGetAddrData(void)
{
    return 0x10;//返回的地址值需要根据实际情况进行修改
}
/**
 * CAN_BootExecutiveCommand
 *
 * @param[in]  pRxMessage CAN总线消息
 * @return : none
 *
 * @brief 执行主机下发的命令
 */
void CAN_BootExecutiveCommand(CAN_MsgInfoType *pRxMessage)
{
    uint8_t canCmd = (pRxMessage->ID) & CMD_MASK;//ID的bit0~bit3位为命令码
    uint16_t canAddr = (pRxMessage->ID >> CMD_WIDTH);//ID的bit4~bit15位为节点地址
    g_CANTxMessage.DLC = 0;
    g_CANTxMessage.ID = 0;
    g_CANTxMessage.IDE = 1;
    g_CANTxMessage.RTR = 0;
    switch (canCmd)
    {
        //ONLINE_CHECK，节点在线检测
        //节点收到该命令后返回固件版本信息和固件类型，该命令在Bootloader程序和APP程序都必须实现
        case ONLINE_CHECK:
            if(canAddr != 0x00) 
            {
                g_CANTxMessage.ID = (CAN_BootGetAddrData() << CMD_WIDTH) | g_cmdList.OnlineCheck;
                g_CANTxMessage.DATA[0] = 0;//主版本号，两字节
                g_CANTxMessage.DATA[1] = 1;
                g_CANTxMessage.DATA[2] = 0;//次版本号，两字节
                g_CANTxMessage.DATA[3] = 0;
                g_CANTxMessage.DATA[4] = 0;
                g_CANTxMessage.DATA[5] = 0;
                g_CANTxMessage.DATA[6] = 0;
                g_CANTxMessage.DATA[7] = 1;//0：固件是Bootloader, 1:固件是APP
                g_CANTxMessage.DLC = 8;
                int ret = CAN_TransmitMessage(g_CANx, &g_CANTxMessage, g_type);
//                if(BOOT_DEBUG)printf("CAN_MessageSend :0x%x\r\n", ret);
            }
            break;
        //ENTER_UPDATE_MODE 进入升级模式
        case ENTER_UPDATE_MODE:
            g_enterUpgrade = 1;
        default:
            break;
    }
}
