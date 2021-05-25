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
* @file can_bootloader.c
*
* @author AutoChips
*
* @version 0.0.1
*
* @date Aug.22.2018
*
* @brief 基于CAN总线的Bootloader程序
*
*******************************************************************************/
#include "can_bootloader.h"
#include "ac780x_eflash.h"
#include "ac780x_can.h"
#include "ac780x_can_reg.h"
#include "crc16.h"
#include "sdk_flash.h"
#include "sdk_system.h"
#include "string.h"
#define APP_START_ADDR    0x8006000
#define PAGE_SIZE     (0x800)    /* 2 Kbytes */
#define RECV_MSG_POOL_COUNT 1                       ///< receive buffer size
typedef void (*function)(void);
function JumpToApplication;
extern int rand(void);
extern uint16_t g_updateModeCnt;

CAN_Type *g_CANx = CAN0;
CAN_TransmitBufferType g_type = CAN_TRANSMIT_SECONDARY;
uint32_t g_recvCount[1] = {0};                 ///< receive frame count
uint32_t g_sendCount[1] = {0};                 ///< send frame count
CBL_CMD_LIST g_cmdList = {0};
CAN_MsgInfoType g_canRxMessage = {0};
__align(4) uint8_t g_canRxBuf[8];//8字节数据，地址需四字节对齐
CAN_MsgInfoType g_canTxMessage;
__align(4) uint8_t g_canTxBuf[8];//8字节，地址需要4字节对齐
volatile uint8_t g_canRxMsgFlag = 0;//接收到CAN数据后的标志
uint32_t g_recvLen = 0;
uint16_t g_canAddr = 0;
uint8_t g_recvData[1028] = {0};


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
    g_canRxMsgFlag = 1;
    uint8_t canCmd = (g_canRxMessage.ID) & CMD_MASK;//ID的bit0~bit3位为命令码
    uint16_t canAddr = (g_canRxMessage.ID >> CMD_WIDTH);//ID的bit4~bit15位为节点地址
    if((canCmd == g_cmdList.WriteBlockFlash) && (canAddr == g_canAddr))
    {
        for(int i = 0; i< g_canRxMessage.DLC; i++)
        {
            g_recvData[g_recvLen++] = g_canRxMessage.DATA[i];
        }
    }
    return 0;
}

///**
// * ClearCanMsgPool
// *
// * @param[in]  CANx: CAN type pointer
// * @return : none
// *
// * @brief  Clear can MSG Pool
// */
void ClearCanMsg(void)
{
    memset(g_recvData, 0, sizeof(g_recvData));
    g_recvLen = 0;
}
/**
 * CanEventCallback
 *
 * @param[in]  event: event
 * @param[in]  wparam: parameter
 * @param[in]  lparam: parameter
 * @return     0: success
 *
 * @brief  Event interrupt callback funtion
 */
void CAN0_IRQnCallBack(void *device, uint32_t wpara, uint32_t lpara)
{
    if (wpara & CAN_CTRL1_RIF_Msk)
    {
        while (CAN_IsMsgInReceiveBuf((CAN_Type*)device))
        {
            CAN_ReadMsg((CAN_Type*)device, &g_canRxMessage);
        }
    }
    if (wpara & CAN_CTRL1_TSIF_Msk)
    {
        
    }
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
    
    g_canRxMessage.DATA = (uint8_t *)g_canRxBuf;
    g_canTxMessage.DATA = (uint8_t *)g_canTxBuf;
    
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
 * CAN_BootErasePage
 *
 * @param[in]  StartPage 起始扇区地址
 * @param[in]  EndPage 结束扇区地址
 * @return : 扇区擦出状态
 *
 * @brief  擦出指定扇区区间的Flash数据 。
 */

EFLASH_StatusType CAN_BootErasePage(uint32_t StartPageAddr, uint32_t EndPageAddr)
{
    uint32_t i;
    EFLASH_StatusType FLASHStatus = EFLASH_STATUS_SUCCESS;
    EFLASH_UnlockCtrl();
    if(BOOT_DEBUG)printf("StartPageAddr :0x%x, EndPageAddr:0x%x\r\n", StartPageAddr, EndPageAddr);
    for (i = StartPageAddr; i <= EndPageAddr; i += PAGE_SIZE) 
    {
        FLASHStatus = EFLASH_PageErase(i);
        if(BOOT_DEBUG)printf("FLASHStatus :0x%x, i:0x%x\r\n", FLASHStatus, i);
        if (FLASHStatus != EFLASH_STATUS_SUCCESS) 
        {
            EFLASH_LockCtrl();
            return	FLASHStatus;
        }
    }
    EFLASH_LockCtrl();
    return FLASHStatus;
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
    
    uint8_t ret;
    
    uint8_t canCmd = (pRxMessage->ID) & CMD_MASK;//ID的bit0~bit3位为命令码
    uint16_t canAddr = (pRxMessage->ID >> CMD_WIDTH);//ID的bit4~bit15位为节点地址
//	uint32_t BaudRate;
    uint16_t crcData;
    EFLASH_StatusType statusRes = EFLASH_STATUS_SUCCESS;
    static uint32_t s_startAddr;
    static uint32_t s_dataSize = 0;
//  static uint32_t s_dataIndex = 0;
    //判断接收的数据地址是否和本节点地址匹配，若不匹配则直接返回，不做任何事情
    if((canAddr != CAN_BootGetAddrData()) && (canAddr!=0))
    {
        if(BOOT_DEBUG)printf("canAddr not equal CAN_BOOT_ADDR:0x%x,\r\n", canAddr);
        return;
    }
    
    g_canTxMessage.DLC = 0;
    g_canTxMessage.ID = 0;
    g_canTxMessage.IDE = 1;
    g_canTxMessage.RTR = 0;
    if(BOOT_DEBUG)printf("canCmd :0x%x, canAddr:0x%x\r\n", canCmd, canAddr);
    switch (canCmd)
    {
        //CMD_List.EraseFlash，擦除Flash中的数据，起始地址存储在Data[0]到Data[3]中，结束地址存储在Data[4]到Data[7]中
        //该命令必须在Bootloader程序中实现，在APP程序中可以不用实现
    case ERASE_FLASH:
        if(BOOT_DEBUG)printf("canCmd :0x%x, canAddr:0x%x\r\n", canCmd, canAddr);
        __set_PRIMASK(1);
        EFLASH_UnlockCtrl();
        statusRes = CAN_BootErasePage((pRxMessage->DATA[0]<<24)|(pRxMessage->DATA[1]<<16)|(pRxMessage->DATA[2]<<8)|(pRxMessage->DATA[3]<<0),
                                      (pRxMessage->DATA[4]<<24)|(pRxMessage->DATA[5]<<16)|(pRxMessage->DATA[6]<<8)|(pRxMessage->DATA[7]<<0));
        EFLASH_LockCtrl();
        if(BOOT_DEBUG)printf("statusRes :0x%x\r\n", statusRes);
        __set_PRIMASK(0);
        if(canAddr != 0x00) 
        {
            if(statusRes==EFLASH_STATUS_SUCCESS) 
            {
                g_canTxMessage.ID = (CAN_BootGetAddrData()<<CMD_WIDTH)|g_cmdList.EraseFlash;
            } else {
                g_canTxMessage.ID = (CAN_BootGetAddrData()<<CMD_WIDTH)|g_cmdList.CmdFaild;
            }
            g_canTxMessage.DLC = 0;
            CAN_TransmitMessage(g_CANx, &g_canTxMessage, g_type);
        }
        break;
        //CMD_List.SetBaudRate，设置节点波特率，具体波特率信息存储在Data[0]到Data[3]中
        //更改波特率后，适配器也需要更改为相同的波特率，否则不能正常通信
    case SET_BAUDRATE:
//波特率暂不支持设置
//			BaudRate = (pRxMessage->DATA[0]<<24)|(pRxMessage->DATA[1]<<16)|(pRxMessage->DATA[2]<<8)|(pRxMessage->DATA[3]<<0);
//			CAN_Configuration(BaudRate);
//			if(canAddr != 0x00){
//				g_canTxMessage.ID = (CAN_BootGetAddrData()<<CMD_WIDTH)|CMD_List.SetBaudRate;
//				g_canTxMessage.DLC = 0;
//				mdelay(20);
//				CAN_WriteData(&g_canTxMessage);
//			}
        break;
        //CMD_List.BlockWriteInfo，设置写Flash数据的相关信息，比如数据起始地址，数据大小
        //数据起始地址存储在Data[0]到Data[3]中，数据大小存储在Data[4]到Data[7]中，该函数必须在Bootloader程序中实现，APP程序可以不用实现
    case BLOCK_WRITE_INFO:
        s_startAddr = (pRxMessage->DATA[0]<<24) | (pRxMessage->DATA[1]<<16) | (pRxMessage->DATA[2]<<8) | (pRxMessage->DATA[3]<<0);
        s_dataSize = (pRxMessage->DATA[4]<<24) | (pRxMessage->DATA[5]<<16) | (pRxMessage->DATA[6]<<8) | (pRxMessage->DATA[7]<<0);
        //s_dataIndex = 0;
        if(BOOT_DEBUG)printf("s_startAddr:0x%x, s_dataSize:%d\r\n", s_startAddr, s_dataSize);
        if(canAddr != 0x00) 
        {
            g_canTxMessage.ID = (CAN_BootGetAddrData() << CMD_WIDTH) | g_cmdList.BlockWriteInfo;
            g_canTxMessage.DLC = 0;
            ClearCanMsg();
            CAN_TransmitMessage(g_CANx, &g_canTxMessage, g_type);
        }
        break;
        //CMD_List.WriteBlockFlash，先将数据存储在本地缓冲区中，然后计算数据的CRC，若校验正确则写数据到Flash中
        //每次执行该数据，数据缓冲区的数据字节数会增加pRxMessage->DLC字节，当数据量达到s_dataSize（包含2字节CRC校验码）字节后
        //对数据进行CRC校验，若数据校验无误，则将数据写入Flash中
        //该函数在Bootloader程序中必须实现，APP程序可以不用实现
    case WRITE_BLOCK_FLASH:
        g_updateModeCnt = 1;
        if(BOOT_DEBUG)printf("g_recvLen:%d\r\n", g_recvLen);
        if((g_recvLen >= s_dataSize) || (g_recvLen >= 1026))
        {
            crcData = crc16_ccitt(g_recvData, s_dataSize-2);
            if(crcData == ((g_recvData[s_dataSize-2] << 8) | (g_recvData[s_dataSize-1])))
            {
                __set_PRIMASK(1);
                EFLASH_UnlockCtrl();
                ret = EFLASH_PageProgram(s_startAddr, (uint32_t *)g_recvData, (g_recvLen-2) / 4);
                EFLASH_LockCtrl();
                __set_PRIMASK(0);
                if(canAddr != 0x00)
                {
                    if(ret == EFLASH_STATUS_SUCCESS)
                    {
                        g_canTxMessage.ID = (CAN_BootGetAddrData() << CMD_WIDTH) | g_cmdList.WriteBlockFlash;
                    }
                    else
                    {
                        g_canTxMessage.ID = (CAN_BootGetAddrData() << CMD_WIDTH) | g_cmdList.CmdFaild;
                        if(BOOT_DEBUG)printf("EFLASH_PageProgram error :%d\r\n", ret);
                    }
                    g_canTxMessage.DLC = 0;
                    CAN_TransmitMessage(g_CANx, &g_canTxMessage, g_type);
                }
            } else {
                g_canTxMessage.ID = (CAN_BootGetAddrData() << CMD_WIDTH) | g_cmdList.CmdFaild;
                g_canTxMessage.DLC = 0;
                CAN_TransmitMessage(g_CANx, &g_canTxMessage, g_type);
                if(BOOT_DEBUG)printf("crc error s_dataSize:%d\r\n", s_dataSize);
            }
            ClearCanMsg();
        }
        break;
        //CMD_List.OnlineCheck，节点在线检测
        //节点收到该命令后返回固件版本信息和固件类型，该命令在Bootloader程序和APP程序都必须实现
    case ONLINE_CHECK:
        g_updateModeCnt = 1;
        if(canAddr != 0x00) 
        {
            g_canTxMessage.ID = (CAN_BootGetAddrData() << CMD_WIDTH) | g_cmdList.OnlineCheck;
            g_canTxMessage.DATA[0] = 0;//主版本号，两字节
            g_canTxMessage.DATA[1] = 1;
            g_canTxMessage.DATA[2] = 0;//次版本号，两字节
            g_canTxMessage.DATA[3] = 0;
            g_canTxMessage.DATA[4] = 0;
            g_canTxMessage.DATA[5] = 0;
            g_canTxMessage.DATA[6] = 0;
            g_canTxMessage.DATA[7] = 0;//0：固件是Bootloader, 1:固件是APP
            g_canTxMessage.DLC = 8;
            int ret = CAN_TransmitMessage(g_CANx, &g_canTxMessage, g_type);
//            if(BOOT_DEBUG)printf("CAN_TransmitMessage :0x%x\r\n", ret);
        }
        break;
        //CMD_List.ExcuteApp，控制程序跳转到指定地址执行
        //该命令在Bootloader和APP程序中都必须实现
    case EXCUTE_APP:
        DataRecord_WriteUpgrade(NORMAL_ON);
        goToApp();
        break;
    default:
        break;
    }
}
/**
 * goToApp
 *
 * @param[in]  none
 * @return : none
 *
 * @brief APP跳转
 */
void goToApp(void)
{
    printf("goToApp\r\n");
    Timer1_Config(DISABLE, DISABLE);
    uint32_t JumpAddress;
    if(((*(__IO uint32_t *)APP_START_ADDR) & 0x2FFE0000) == 0x20000000)
    {
        __ASM("CPSID I");//关全局中断
        JumpAddress = *(__IO uint32_t *)(APP_START_ADDR + 4); // Jump to user application
        JumpToApplication = (function)JumpAddress;   // Initialize user application's Stack Pointer
        __set_MSP(*(__IO uint32_t*)APP_START_ADDR);
        JumpToApplication();
    }
    else
    {
        printf("CANN'T JUMP\r\n");
    }
}

