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
* @brief ����CAN���ߵ�Bootloader����
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
__align(4) uint8_t g_canRxBuf[8];//8�ֽ����ݣ���ַ�����ֽڶ���
CAN_MsgInfoType g_canTxMessage;
__align(4) uint8_t g_canTxBuf[8];//8�ֽڣ���ַ��Ҫ4�ֽڶ���
volatile uint8_t g_canRxMsgFlag = 0;//���յ�CAN���ݺ�ı�־
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
    uint8_t canCmd = (g_canRxMessage.ID) & CMD_MASK;//ID��bit0~bit3λΪ������
    uint16_t canAddr = (g_canRxMessage.ID >> CMD_WIDTH);//ID��bit4~bit15λΪ�ڵ��ַ
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
 * @brief  CAN��ʼ��
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
 * @return : �ڵ��ַ��
 *
 * @brief  ��ȡ�ڵ��ַ��Ϣ
 */

uint16_t CAN_BootGetAddrData(void)
{
    return 0x10;//���صĵ�ֵַ��Ҫ����ʵ����������޸�
}

/**
 * CAN_BootErasePage
 *
 * @param[in]  StartPage ��ʼ������ַ
 * @param[in]  EndPage ����������ַ
 * @return : ��������״̬
 *
 * @brief  ����ָ�����������Flash���� ��
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
 * @param[in]  pRxMessage CAN������Ϣ
 * @return : none
 *
 * @brief ִ�������·�������
 */
void CAN_BootExecutiveCommand(CAN_MsgInfoType *pRxMessage)
{
    
    uint8_t ret;
    
    uint8_t canCmd = (pRxMessage->ID) & CMD_MASK;//ID��bit0~bit3λΪ������
    uint16_t canAddr = (pRxMessage->ID >> CMD_WIDTH);//ID��bit4~bit15λΪ�ڵ��ַ
//	uint32_t BaudRate;
    uint16_t crcData;
    EFLASH_StatusType statusRes = EFLASH_STATUS_SUCCESS;
    static uint32_t s_startAddr;
    static uint32_t s_dataSize = 0;
//  static uint32_t s_dataIndex = 0;
    //�жϽ��յ����ݵ�ַ�Ƿ�ͱ��ڵ��ַƥ�䣬����ƥ����ֱ�ӷ��أ������κ�����
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
        //CMD_List.EraseFlash������Flash�е����ݣ���ʼ��ַ�洢��Data[0]��Data[3]�У�������ַ�洢��Data[4]��Data[7]��
        //�����������Bootloader������ʵ�֣���APP�����п��Բ���ʵ��
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
        //CMD_List.SetBaudRate�����ýڵ㲨���ʣ����岨������Ϣ�洢��Data[0]��Data[3]��
        //���Ĳ����ʺ�������Ҳ��Ҫ����Ϊ��ͬ�Ĳ����ʣ�����������ͨ��
    case SET_BAUDRATE:
//�������ݲ�֧������
//			BaudRate = (pRxMessage->DATA[0]<<24)|(pRxMessage->DATA[1]<<16)|(pRxMessage->DATA[2]<<8)|(pRxMessage->DATA[3]<<0);
//			CAN_Configuration(BaudRate);
//			if(canAddr != 0x00){
//				g_canTxMessage.ID = (CAN_BootGetAddrData()<<CMD_WIDTH)|CMD_List.SetBaudRate;
//				g_canTxMessage.DLC = 0;
//				mdelay(20);
//				CAN_WriteData(&g_canTxMessage);
//			}
        break;
        //CMD_List.BlockWriteInfo������дFlash���ݵ������Ϣ������������ʼ��ַ�����ݴ�С
        //������ʼ��ַ�洢��Data[0]��Data[3]�У����ݴ�С�洢��Data[4]��Data[7]�У��ú���������Bootloader������ʵ�֣�APP������Բ���ʵ��
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
        //CMD_List.WriteBlockFlash���Ƚ����ݴ洢�ڱ��ػ������У�Ȼ��������ݵ�CRC����У����ȷ��д���ݵ�Flash��
        //ÿ��ִ�и����ݣ����ݻ������������ֽ���������pRxMessage->DLC�ֽڣ����������ﵽs_dataSize������2�ֽ�CRCУ���룩�ֽں�
        //�����ݽ���CRCУ�飬������У������������д��Flash��
        //�ú�����Bootloader�����б���ʵ�֣�APP������Բ���ʵ��
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
        //CMD_List.OnlineCheck���ڵ����߼��
        //�ڵ��յ�������󷵻ع̼��汾��Ϣ�͹̼����ͣ���������Bootloader�����APP���򶼱���ʵ��
    case ONLINE_CHECK:
        g_updateModeCnt = 1;
        if(canAddr != 0x00) 
        {
            g_canTxMessage.ID = (CAN_BootGetAddrData() << CMD_WIDTH) | g_cmdList.OnlineCheck;
            g_canTxMessage.DATA[0] = 0;//���汾�ţ����ֽ�
            g_canTxMessage.DATA[1] = 1;
            g_canTxMessage.DATA[2] = 0;//�ΰ汾�ţ����ֽ�
            g_canTxMessage.DATA[3] = 0;
            g_canTxMessage.DATA[4] = 0;
            g_canTxMessage.DATA[5] = 0;
            g_canTxMessage.DATA[6] = 0;
            g_canTxMessage.DATA[7] = 0;//0���̼���Bootloader, 1:�̼���APP
            g_canTxMessage.DLC = 8;
            int ret = CAN_TransmitMessage(g_CANx, &g_canTxMessage, g_type);
//            if(BOOT_DEBUG)printf("CAN_TransmitMessage :0x%x\r\n", ret);
        }
        break;
        //CMD_List.ExcuteApp�����Ƴ�����ת��ָ����ִַ��
        //��������Bootloader��APP�����ж�����ʵ��
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
 * @brief APP��ת
 */
void goToApp(void)
{
    printf("goToApp\r\n");
    Timer1_Config(DISABLE, DISABLE);
    uint32_t JumpAddress;
    if(((*(__IO uint32_t *)APP_START_ADDR) & 0x2FFE0000) == 0x20000000)
    {
        __ASM("CPSID I");//��ȫ���ж�
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

