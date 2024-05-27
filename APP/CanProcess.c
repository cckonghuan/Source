/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		CanProcess.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define		__CAN_PROCESS_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define		cCAN_POLLING_100MS		100
#define		cCAN_POLLING_200MS		200
#define		cCAN_POLLING_500MS		500
#define		cCAN_POLLING_1S			1000

/********************************************************************************
* Constants																		*
********************************************************************************/


/********************************************************************************
* External variables															*
********************************************************************************/


/********************************************************************************
* External routine prototypes													*
********************************************************************************/


/********************************************************************************
* Input variables																*
********************************************************************************/
INT8U	bCanAddress;

INT8U	bCanDataRecv;
/********************************************************************************
* Output variables																*
********************************************************************************/


/********************************************************************************
* Internal variables															*
********************************************************************************/
union CAN_STATUS_TABLE
{
	struct CAN_STATUS_BIT_FEILD
	{
		// BIT0-3
		INT16U rsvd1:4;
		
		// BIT4-7
		INT16U rsvd2:4;
		
		// BIT8-11
		INT16U rsvd3:4;
		
		// BIT12-15
		INT16U rsvd4:4;
	} bits;
	INT16U data;
} fCanStatus;

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/


/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void	sCanInitial(void)
{
	bCanDataRecv = 0;
	sCanProtocolNewInitial();
	sCanErrCntInit();
}

void	sCanParsing(void)
{
	T_CanFrame RxFrame;
	T_CAN_FRAME RxFrameNew;
	
	while(CAN_ReadFrame(cBMS_CAN, &RxFrame) == QUEUE_OK)
	{
		bCanDataRecv = 1;
		RxFrameNew.Id = RxFrame.IxR >> 3;
		RxFrameNew.Data.dwData[0] = RxFrame.Data.u32[0];
		RxFrameNew.Data.dwData[1] = RxFrame.Data.u32[1];	
		
		sCanAutoAdsParsing(&RxFrameNew);	// 自动地址分配
		
		// 刚开机以及地址为0时,不进行CAN通讯
		if(sbGetCanAddress() != 0)
		{
			sCanProtocolNewParsing(&RxFrameNew);	// 新CAN协议
		}
	}
}

// 地址为0时,单机运行模式,待机状态下固定延时3秒后根据状态标志进入相应模式
// 地址不为0时,并机运行模式,必须等待并机角色变更为主机或者从机后,才允许进入相应模式
// 如果并机运行后,并机角色变更为新接入设备,则需要退出到待机模式
// 并机逻辑只在所有主机或者从机都支持最新CAN总线协议才执行,如果有一台不支持最新协议
// 单机运行模式,不外发指令也不解析指令,按单机运行
// 并机运行模式,支持并机指令轮询(新通讯协议)以及旧通信协议轮询,
// 并机模式且支持并机运行(所有模块支持并机协议),则系统状态切换支持并机角色
// 并机模式且不支持并机运行(有一个模块不支持并机协议),则系统状态切换支持并机角色
void	sCanTimerPolling(INT16U wTimeBase)
{
	// 刚开机以及地址为0时,不进行CAN通讯
	if(bBmsMode == cPowerOnMode)
	{
		return;
	}

	sCanParaErrCntClr(wTimeBase);
	sCanAutoAdsCheck(wTimeBase);	// 自动地址分配
	sCanParallelTimerPolling(wTimeBase, cCAN_POLLING_200MS);
	
	if(sbGetCanAddress() != 0)
	{
		sCanParallelLogic(wTimeBase);
		
//		sCanParallelTimerPolling(wTimeBase, cCAN_POLLING_200MS);
		
		// 并机地址错误
		if((swGetBmsFaultFlag() & cParallelFault) == 0)
		{
			sCanEmsDataTimerPolling(wTimeBase, cCAN_POLLING_200MS);
			sCanBmsDataTimerPolling(wTimeBase, cCAN_POLLING_200MS);
			sCanUserDataTimerPolling(wTimeBase, cCAN_POLLING_200MS);
		}
	}
}

void	sCanToShutdownParsing(void)
{
	if(sbGetBmsMode() != cShutdownMode)
	{
		sSetBmsParallelShutdown(true);
		OSEventSend(cPrioSuper, eSuperToShutdown);
	}
}

void	sCanToShutdownCmd(void)
{
	if(sbGetCanAddress() != 0)
	{
		sCanNewShutdownEvent();
	}
}

void	sCanParallelAddrErr(void)
{
	if(sbGetBmsFaultCode() == 0)
	{
		sSetBmsFaultCode(cParallelFaultId);
	}
	sSetBmsFaultFlag(cParallelFault);
	OSEventSend(cPrioSuper,eSuperToFault);
}

void	sCanWrite(INT8U canId, T_CAN_FRAME *pFrame, INT8U bLen)
{
	T_CanFrame TxFrame;
	
	TxFrame.IxR = (pFrame->Id << 3) + (CAN_Id_Extended | CAN_RTR_Data);
	TxFrame.DTxR = bLen;	// 0-8之间
	TxFrame.Data.u32[0] = pFrame->Data.dwData[0];
	TxFrame.Data.u32[1] = pFrame->Data.dwData[1];
	CAN_SendFrame(canId, &TxFrame);
}

void	sCanWriteStandard(INT8U canId, T_CAN_FRAME *pFrame, INT8U bLen)
{
	T_CanFrame TxFrame;
	
	TxFrame.IxR = (pFrame->Id << 21) + (CAN_Id_Standard | CAN_RTR_Data);
	TxFrame.DTxR = bLen;	// 0-8之间
	TxFrame.Data.u32[0] = pFrame->Data.dwData[0];
	TxFrame.Data.u32[1] = pFrame->Data.dwData[1];
	CAN_SendFrame(canId, &TxFrame);
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
INT8U	sbGetCanAddress(void)
{
	return bCanAddress;
}

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
void	sSetCanAddress(INT8U bAddress)
{
	bCanAddress = bAddress;
}

