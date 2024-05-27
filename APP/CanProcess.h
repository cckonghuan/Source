/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		CanProcess.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __CAN_PROCESS_H__
#define __CAN_PROCESS_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef  __CAN_PROCESS_C__
#define CAN_PROCESS
#else
#define CAN_PROCESS  extern
#endif

#define	cCAN_DEVICE_NUM				16
#define	cCAN_DEVICE_DOUBLE			(cCAN_DEVICE_NUM<<1)
typedef union
{
	INT8U 	bData[8];		// 定义字节型以满足需求
	INT16U	wData[4];		
	INT32U	dwData[2];		// 定义寄存器类型以提高读取效率,32位0x12345678换成8位:0x78,0x56,0x34,0x12
}T_CAN_DATA;

typedef struct{
	INT32U Id;
	T_CAN_DATA Data;
}T_CAN_FRAME;

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
CAN_PROCESS	void	sCanInitial(void);
CAN_PROCESS	void	sCanParsing(void);
CAN_PROCESS	void	sCanTimerPolling(INT16U wTimeBase);
CAN_PROCESS	void	sCanToShutdownParsing(void);
CAN_PROCESS	void	sCanToShutdownCmd(void);
CAN_PROCESS	void	sCanParallelAddrErr(void);
CAN_PROCESS	void	sCanWrite(INT8U canId, T_CAN_FRAME *pFrame, INT8U bLen);
CAN_PROCESS	void	sCanWriteStandard(INT8U canId, T_CAN_FRAME *pFrame, INT8U bLen);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
CAN_PROCESS	INT8U	sbGetCanAddress(void);

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
CAN_PROCESS	void	sSetCanAddress(INT8U bAddress);

#endif	// __CAN_PROCESS_H__

