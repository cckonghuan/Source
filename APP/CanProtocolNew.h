/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		CanProtocolNew.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __CAN_PROTOCOL_NEW_H__
#define __CAN_PROTOCOL_NEW_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef  __CAN_PROTOCOL_NEW_C__
#define CAN_PROTOCOL_NEW
#else
#define CAN_PROTOCOL_NEW  extern
#endif

/********************************************************************************
* Include head files															*
********************************************************************************/


/********************************************************************************
* Macros 																		*
********************************************************************************/
#define		cCAN_PARALLEL_NEW      		0
#define		cCAN_PARALLEL_SLAVE       	1
#define		cCAN_PARALLEL_HOST       	2

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
CAN_PROTOCOL_NEW	void	sCanProtocolNewInitial(void);
CAN_PROTOCOL_NEW	void	sCanErrCntInit(void);
CAN_PROTOCOL_NEW	void	sCanProtocolNewParsing(T_CAN_FRAME *pData);
CAN_PROTOCOL_NEW	void	sCanParallelLogic(INT16U wTimeBase);
CAN_PROTOCOL_NEW	void	sCanParallelTimerPolling(INT16U wTimeBase, INT16U wFilter);
CAN_PROTOCOL_NEW	void	sCanEmsDataTimerPolling(INT16U wTimeBase, INT16U wFilter);
CAN_PROTOCOL_NEW	void	sCanBmsDataTimerPolling(INT16U wTimeBase, INT16U wFilter);
CAN_PROTOCOL_NEW	void	sCanUserDataTimerPolling(INT16U wTimeBase, INT16U wFilter);
CAN_PROTOCOL_NEW	void	sCanSyncDisplayKeyCommand(void);
CAN_PROTOCOL_NEW	void	sCanSyncDisplaySocCommand(void);
CAN_PROTOCOL_NEW	void	sCanSyncUserParaCommand(void);
CAN_PROTOCOL_NEW	void	sCanNewShutdownEvent(void);
CAN_PROTOCOL_NEW	void	sCanAutoAdsParsing(T_CAN_FRAME *pData);
CAN_PROTOCOL_NEW	void	sCanAutoAdsCheck(INT16U wTimeBase);
CAN_PROTOCOL_NEW	void	sCanAutoAdsAssign(void);
CAN_PROTOCOL_NEW	void	sCanParallelErrSync(void);
CAN_PROTOCOL_NEW	void 	sCanParaErrCntClr(INT16U wTimeBase);


/********************************************************************************
* Output interface Routines														*
********************************************************************************/
CAN_PROTOCOL_NEW	INT32U	sdwGetCanParallelFlag(void);
CAN_PROTOCOL_NEW	INT8U	sbGetCanParallelNum(void);
CAN_PROTOCOL_NEW	INT8U	sbGetCanParallelHostId(void);
CAN_PROTOCOL_NEW	INT8U	sbGetCanDeviceRole(void);
CAN_PROTOCOL_NEW	INT8U	sbIsCanDeviceHost(void);
CAN_PROTOCOL_NEW    void    sInitial_CAN_New_Addr(void);

/********************************************************************************
* Input interface Routines														*
********************************************************************************/


#endif // __CAN_PROTOCOL_NEW_H__

