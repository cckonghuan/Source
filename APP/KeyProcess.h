/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		KeyProcess.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __KEY_PROCESS_H__
#define __KEY_PROCESS_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef	__KEY_PROCESS_C__
#define KEY_PROCESS
#else
#define KEY_PROCESS  extern
#endif

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
KEY_PROCESS	void	sKeyProcessInit(void);
KEY_PROCESS	void	sKeyPWRScanProc(INT16U wOffCnt, INT16U wForceCnt);
KEY_PROCESS	void	sKeyLCDScanProc(INT16U wOffCnt, INT16U wDbgCnt);
KEY_PROCESS	void	sKeyForceOnTimeout(INT16U wTimeOut);
KEY_PROCESS	void	sKeyTurnOffProc(void);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
KEY_PROCESS	INT8U	sbGetKeyStatusForcedOn(void);
KEY_PROCESS	INT8U	sbGetKeyStatusDebugMode(void);
KEY_PROCESS	INT8U	sbGetKeyStatusConfigMode(void);
KEY_PROCESS	INT8U	sbGetKeyStatusPressTrig(void);
KEY_PROCESS	INT8U	sbGetKeyStatusLCDMode(void);
KEY_PROCESS	INT8U	sbGetKeyAddress(void);
KEY_PROCESS	INT8U	sbGetSciAddress(void);
KEY_PROCESS void	sSetSciAddress(INT8U bAddr);
KEY_PROCESS	INT16U	swGetKeyCode(void);
KEY_PROCESS	INT8U	sbGetKeyPressTurnOff(void);
KEY_PROCESS	INT8U	sbGetKeyPressForceOn(void);
KEY_PROCESS	INT8U	sbGetKeyPressDebug(void);
KEY_PROCESS	INT8U	sbGetKeyPressConfig(void);
KEY_PROCESS	INT8U	sbGetKeyPressAdjust(void);
KEY_PROCESS	INT8U	sbGetKeyPressEnter(void);

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
KEY_PROCESS	void	sSetKeyStatusForcedOn(INT8U bStatus);
KEY_PROCESS	void	sSetKeyStatusDebugMode(INT8U bStatus);
KEY_PROCESS	void	sSetKeyStatusConfigMode(INT8U bStatus);
KEY_PROCESS	void	sSetKeyStatusPressTrig(INT8U bStatus);
KEY_PROCESS	void	sSetKeyStatusLCDMode(INT8U bStatus);
KEY_PROCESS	void	sClrKeyPressTurnOff(void);
KEY_PROCESS	void	sClrKeyPressForceOn(void);
KEY_PROCESS	void	sClrKeyPressDebug(void);
KEY_PROCESS	void	sClrKeyPressConfig(void);
KEY_PROCESS	void	sClrKeyPressAdjust(void);
KEY_PROCESS	void	sClrKeyPressEnter(void);

#endif	// __KEY_PROCESS_H__

