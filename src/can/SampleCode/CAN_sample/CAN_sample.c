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
#include <string.h>
#include "ac780x_timer.h"
#include "ac780x_can.h"
#include "ac780x_gpio.h"
#include "ac780x_can_reg.h"
/*************<macro>******************/

#define LED1_PIN					GPIOC, GPIO_PIN9//定义LED1引脚
#define LED2_PIN					GPIOC, GPIO_PIN7//定义LED2引脚


#define LED2_ON						do{GPIO_SetPinLevel(LED2_PIN, GPIO_LEVEL_HIGH);}while(0)
#define LED2_OFF					do{GPIO_SetPinLevel(LED2_PIN, GPIO_LEVEL_LOW);}while(0)
#define LED2_TOGGLE					do{if(GPIO_GetPinLevel(LED2_PIN)){LED2_OFF;}else{LED2_ON;}}while(0)
	
#define LED1_ON						do{GPIO_SetPinLevel(LED1_PIN, GPIO_LEVEL_HIGH);}while(0)
#define LED1_OFF					do{GPIO_SetPinLevel(LED1_PIN, GPIO_LEVEL_LOW);}while(0)
#define LED1_TOGGLE					do{if(GPIO_GetPinLevel(LED1_PIN)){LED1_OFF;}else{LED1_ON;}}while(0)


#define CAN_RECV_DATA_ID1			(0x220)
#define CAN_RECV_DATA_ID2			(0x230)
#define CAN_SEND_DATA_ID1			(0x430)

#define CAN_RECV_DATA_ID11			(0x18DFF930)
#define CAN_RECV_DATA_ID12			(0x18BFF930)
#define CAN_SEND_DATA_ID11			(0x18DEE050)

#define CAN0_TX						GPIOB, GPIO_PIN0
#define CAN0_RX						GPIOB, GPIO_PIN1
#define CAN0_STB					GPIOC, GPIO_PIN8

#define CAN0_TRASCVER_NML			do{GPIO_SetPinLevel(CAN0_STB, GPIO_LEVEL_LOW);}while(0)
#define CAN0_TRASCVER_SLP			do{GPIO_SetPinLevel(CAN0_STB, GPIO_LEVEL_HIGH);udelay(20);}while(0)

#define Cycle1ms    (APB_BUS_FREQ / 1000 - 1)

/*************<enum>*******************/
typedef enum
{
	GPIO_INPUT	= 0,
	GPIO_OUTPUT	= 1
}GPIO_Dir_TypeDef;

typedef enum
{
	GPIO_FUNC_0	= 0,
	GPIO_FUNC_1	= 1,
	GPIO_FUNC_2	= 2,
	GPIO_FUNC_3	= 3
}GPIO_Func_TypeDef;


typedef enum
{
	CAN_IDE_STD = 0,
	CAN_IDE_EXT
}CAN_IDE_TypeDef;

typedef enum
{
	CAN_RTR_STD = 0,
	CAN_RTR_RMT
}CAN_RTR_TypeDef;

typedef enum
{
	CAN_SEND_MSG_FILED = 0,
	CAN_SEND_MSG_SUCCS
}CAN_SendSts_TypeDef;

typedef enum
{
	FILTER_IDE_STD_ONLY 	= 0,
	FILTER_IDE_EXT_ONLY 	= 1,
	FILTER_IDE_STD_EXT_BOTH	= 2
}CAN_FilterMask_TypeDef;

typedef enum
{
	CAN_FILTER_CODE_MODE	= 0,
	CAN_FILTER_MASK_MODE	= 1,
}CAN_FilterMode_TypeDef;

/*************<union>******************/


/*************<struct>*****************/


/*************<variable>***************/
CAN_MsgInfoType 			g_sendCANMsgInfo;
CAN_MsgInfoType 			g_recvCANMsgInfo;
uint32_t sendDataBuff[64/4];
uint32_t recvDataBuff[64/4];    ///<must be 4byte align

uint8_t 				g_recvedCANDataRdy = FALSE;
uint8_t 				g_sendCANDataFlag = TRUE;
uint8_t					g_sendCANDataTimeCnt = 0;
uint32_t 				g_sendCANDataTtlTimeCnt = 0;
uint32_t				g_recvStopTimeCnt = 0;

uint8_t 				g_blinkLED2Flag = FALSE;
uint8_t					g_blinkLED1Flag = FALSE;
uint8_t					g_blinkLED2TimeCnt = 0;
uint8_t					g_blinkLED1TimeCnt = 0;


CAN_FilterControlType canFilterControl[]=
{
    {0, ENABLE, 0x00000400, 0x00000000},    ///<只接收0x400 ID
    {1, ENABLE, 0x00000500, 0x000000FF},    ///<接收0x500~0x5FF 的ID
};
/*************<prototype>**************/
void TIM2_IRQnCallBack(void *device, uint32_t wpara, uint32_t lpara);//TIM2的回调函数
void CAN0_IRQnCallBack(void *device, uint32_t wpara, uint32_t lpara);
void CAN_CheckDataDealPrd(void);
void CAN_CheckLEDBlinkPrd(void);
void CAN_InitHardwr(void);
void TIMER_InitHardwr(void);
void GPIO_InitHardwr(void);
/**
* @prototype CAN_Basic_Sample(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 CAN basic sample.
*			 CAN基本应用展示.实现数据收发.
*/
int main(void)
{
	GPIO_InitHardwr();
	CAN_InitHardwr();
	TIMER_InitHardwr();
	
	while(1)
	{
		CAN_CheckDataDealPrd();
		CAN_CheckLEDBlinkPrd();
	}
}

/**
* @prototype CAN_CheckDataDealPrd(void)
*
* @param[in] void 
* @return	 void
*
* @brief  	 Check CAN data recv or send should be do.
*			 查询是否需要处理CAN数据.
*/
void CAN_CheckDataDealPrd(void)
{
    int32_t ret=0;
	if ((g_sendCANDataFlag) && (g_sendCANDataTimeCnt > 99))//100ms的周期发送数据
	{
		g_sendCANDataTimeCnt = 0;
		g_blinkLED1Flag   	 = TRUE;
		g_sendCANMsgInfo.ID = CAN_SEND_DATA_ID1;
        g_sendCANMsgInfo.IDE = 1;
        g_sendCANMsgInfo.DLC = 8;
        g_sendCANMsgInfo.DATA[0] = 0;
        g_sendCANMsgInfo.DATA[1] = 1;
        g_sendCANMsgInfo.DATA[2] = 2;
        g_sendCANMsgInfo.DATA[3] = 3;
        g_sendCANMsgInfo.DATA[4] = 4;
        g_sendCANMsgInfo.DATA[5] = 5;
        g_sendCANMsgInfo.DATA[6] = 6;
        g_sendCANMsgInfo.DATA[7] = 7;
		ret = CAN_TransmitMessage(CAN0, &g_sendCANMsgInfo, CAN_TRANSMIT_SECONDARY);//发送数据
		if (ret == 0)
        {
            //success
        }
        else
        {
            //error
        }
		if (g_sendCANDataTtlTimeCnt > 1999)//总发送时间2s,时间到达后,停止发送数据
		{
			g_sendCANDataFlag = FALSE;
			g_sendCANDataTtlTimeCnt = 0;
			
			g_blinkLED1Flag = FALSE;
			LED1_OFF; 
		}
	}
	else if ((!g_sendCANDataFlag) && (g_sendCANDataTtlTimeCnt > 1999))//停止发送2s后,又重新开始2s的发送周期
	{
		g_sendCANDataFlag = TRUE;
		
		g_sendCANDataTimeCnt = 0;
		g_sendCANDataTtlTimeCnt = 0;
	}
	
	if (g_recvedCANDataRdy)//数据接收后,处理
	{
		//复制数据到处理缓冲区,获取长度,ID信息
		
		g_recvedCANDataRdy = FALSE;
		
		//处理CAN数据
		
		if (g_blinkLED2Flag == FALSE)//只有在当前没有闪烁灯的情况下,启动闪烁
		{
			g_blinkLED2Flag = TRUE;
			g_blinkLED2TimeCnt = 0;
		}
		
		g_recvStopTimeCnt = 0;
	}
	else
	{
		if ((g_blinkLED2Flag) && (g_recvStopTimeCnt > 299))//300ms内没有接收到任何数据,停止闪烁
		{
			g_blinkLED2Flag = FALSE;
			
			LED2_OFF;
		}
	}
}

/**
* @prototype CAN_CheckLEDBlinkPrd(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 Check the led weather to be blinking.
*			 检查LED是否需要闪烁.
*/
void CAN_CheckLEDBlinkPrd(void)
{
	if ((g_blinkLED2Flag) && (g_blinkLED2TimeCnt > 49))//闪烁LED2
	{
		g_blinkLED2TimeCnt = 0;
		
		LED2_TOGGLE;
	}
	
	if ((g_blinkLED1Flag) && (g_blinkLED1TimeCnt > 49))//闪烁LED1
	{
		g_blinkLED1TimeCnt = 0;
		
		LED1_TOGGLE;
	}
}

/**
* @prototype CAN_InitHardwr(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 Initalize CAN module.
*			 初始化CAN模块.
*/
void CAN_InitHardwr(void)
{
	CAN_ConfigType canConfig;
	CAN_BitrateConfigType canBandrateConfig; 
	memset((void *)&canConfig, 0, (uint32_t)sizeof(CAN_ConfigType));   
	memset((void *)&canBandrateConfig, 0, sizeof(CAN_BitrateConfigType));
	GPIO_SetFunc(CAN0_TX, GPIO_FUN1);//设置CAN0引脚复用功能
	GPIO_SetFunc(CAN0_RX, GPIO_FUN1);
    GPIO_SetFunc(CAN0_STB, GPIO_FUN0);
	GPIO_SetDir(CAN0_STB, GPIO_OUT);//设置CAN0收发器控制为IO控制 
	//CAN0_TRASCVER_NML;//使能CAN收发器
    GPIO_SetPinLevel(CAN0_STB, GPIO_LEVEL_LOW);
	
	/*
	设置波特率为500K,采样点为81.25%.
	tSeg1 = (S_SEG_1 + 2); tSeg2 = (S_SEG_2 + 1).
	BandRate 	= (48M / (S_PRESC + 1) / ((S_SEG_1 + 2) + (S_SEG_2 + 1)))
	SamplePoint = (tSeg1 / (tSeg1 + tSeg2)).
	在已经知道波特率与采样点的情况下,经过计算后:tSeg1 = 13,tSeg2 = 3.
	 */
	canBandrateConfig.PRESC 	= 5;//分频后时钟为8M.
	canBandrateConfig.SEG_1 	= 11;
	canBandrateConfig.SEG_2 	= 2;
	canBandrateConfig.SJW 	    = 2;//满足S_SJW <= tSeg2即可.
	

	canConfig.interruptEn = ENABLE;//使能中断
    canConfig.interruptMask = CAN_IRQ_ALL_ENABLE_MSK;   //打开所有中断
	canConfig.canMode = CAN_MODE_NORMAL;
    canConfig.clockSrc = CAN_CLKSRC_AHB;
    //canConfig.tpss = ENABLE;
    //canConfig.tsss = ENABLE;
    //canConfig.filterNum = 2;
	//canConfig.filterList = canFilterControl;//赋值过滤器设置list
    canConfig.callback = CAN0_IRQnCallBack;
    canConfig.normalBitrate = &canBandrateConfig;
	
    g_sendCANMsgInfo.DATA = (uint8_t *)sendDataBuff;
    g_recvCANMsgInfo.DATA = (uint8_t *)recvDataBuff;
    
	CAN_Init(CAN0, &canConfig);
}
/**
* @prototype GPIO_InitHardwr(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 Initalize GPIO module.
*			 初始化GPIO模块.
*/
void GPIO_InitHardwr(void)
{
	GPIO_SetDir(LED2_PIN, GPIO_OUT);//设置LED引脚为输出
	GPIO_SetDir(LED1_PIN, GPIO_OUT);
	
	
	LED2_OFF;
	LED1_OFF;
}


/**
* @prototype TIMR_InitHardwr(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 Initalize TIM2 module.
*			 初始化TIM2模块.
*/
void TIMER_InitHardwr(void)
{
	TIMER_ConfigType timerConfig;
	memset((void *)&timerConfig, 0, (uint32_t)sizeof(TIMER_ConfigType));   
	
	timerConfig.periodValue = Cycle1ms;//定时器时钟只能是外设总线时钟
	timerConfig.linkModeEn = DISABLE;//不使用多定时器进行链接
	timerConfig.interruptEn = ENABLE;//使能中断
	timerConfig.timerEn = ENABLE;//使能定时器
	timerConfig.callBack = TIM2_IRQnCallBack; 
    
	TIMER_Init(TIMER_CHANNEL2,&timerConfig);
}

/**
* @prototype TIM2_IRQnCallBack(uint8_t lparam)
*
* @param[in] ...
* @return	 ...
*
* @brief  	 TIM2 module interrupt handler.
*			 TIM2中断处理函数.
*/
void TIM2_IRQnCallBack(void *device, uint32_t wpara, uint32_t lpara)
{
    if ((g_blinkLED2Flag) && (g_blinkLED2TimeCnt < 0xFF))
    {
        g_blinkLED2TimeCnt++;
    }
    
    if ((g_blinkLED1Flag) && (g_blinkLED1TimeCnt < 0xFF))
    {
        g_blinkLED1TimeCnt++;
    }
    
    if ((g_sendCANDataFlag) && (g_sendCANDataTimeCnt < 0xFF))
    {
        g_sendCANDataTimeCnt++;
    }
    
    if ((g_blinkLED2Flag) && (g_recvStopTimeCnt < 0xFFFF))
    {
        g_recvStopTimeCnt++;
    }
    
    g_sendCANDataTtlTimeCnt++;
}

/**
* @prototype void CAN0_IRQnCallBack(void *device, uint32_t wpara, uint32_t lpara)
*
* @param[in] ...
* @return	 ...
*
* @brief  	 CAN0 module interrupt handler.
*			 CAN0中断处理函数.
*/
void CAN0_IRQnCallBack(void *device, uint32_t wpara, uint32_t lpara)
{
    if (wpara & CAN_CTRL1_EIF_Msk)  ///<error happened
    {
        if (((CAN_Type*)device)->CTRL0 & CAN_CTRL0_BUSOFF_Msk)
        {
            ///<Busoff happened  
        }
    }
	if (wpara & CAN_CTRL1_RIF_Msk)
	{
		if (CAN_IsMsgInReceiveBuf((CAN_Type*)device))
        {
            CAN_ReceiveMessage((CAN_Type*)device, &g_recvCANMsgInfo);
			
			g_recvedCANDataRdy = TRUE;
        }
	}
}
/*************<end>********************/

