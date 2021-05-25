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

#ifndef CAN_BOOTLOADER_H_
#define CAN_BOOTLOADER_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "ac780x_can.h"
#include "can_bootloader.h"
#define BOOT_DEBUG			1
typedef  struct  _CBL_CMD_LIST{
	uint8_t EraseFlash;			//����APP������������
	uint8_t	WriteBlockFlash;	//�Զ��ֽ���ʽд����
	uint8_t	ReadBlockFlash;		//�Զ��ֽ���ʽ������
	uint8_t BlockWriteInfo;		//���ö��ֽ�д������ز�����д��ʼ��ַ����������
	uint8_t BlockReadInfo;		//���ö��ֽ����ݶ���ز���������ʼ��ַ����������
	uint8_t	OnlineCheck;		//���ڵ��Ƿ�����
	uint8_t	CmdSuccess;			//����ִ�гɹ�
	uint8_t	CmdFaild;			//����ִ��ʧ��
	uint8_t	SetBaudRate;		//���ýڵ㲨����
	uint8_t	ExcuteApp;			//ִ��Ӧ�ó���
    uint8_t	EnterUpdateMode;	//��������ģʽ
	uint8_t	Undefine0;			//δ����
	uint8_t	Undefine1;			//δ����
	uint8_t	Undefine2;			//δ����
	uint8_t	Undefine3;			//δ����
} CBL_CMD_LIST,*PCBL_CMD_LIST; 

#define CMD_FAILD               0x0
#define ONLINE_CHECK            0x1
#define ERASE_FLASH             0x3
#define SET_BAUDRATE            0x4
#define BLOCK_WRITE_INFO        0x5
#define WRITE_BLOCK_FLASH       0x6
#define BLOCK_READ_INFO         0x7
#define READ_BLOCK_FLASH        0x8
#define EXCUTE_APP              0x9
#define ENTER_UPDATE_MODE       0xA
#define CMD_SUCCESS             0xB

#define CMD_WIDTH   4
#define CMD_MASK    0xF
#define ADDR_MASK   0x1FFFFFF



#define DEMO_BOARD                  1

//CAN
#define CAN0_TX						GPIOB, GPIO_PIN0
#define CAN0_RX						GPIOB, GPIO_PIN1
#define CAN0_STB					GPIOC, GPIO_PIN8


void CAN_Config(void);
void CAN_BootExecutiveCommand(CAN_MsgInfoType *pRxMessage);
void goToApp(void);
uint16_t CAN_BootGetAddrData(void);
#ifdef __cplusplus
}
#endif
#endif /* CAN_BOOTLOADER_H_ */
