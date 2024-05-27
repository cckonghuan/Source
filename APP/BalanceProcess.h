/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		BalanceProcess.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __BALANCE_PROCESS_H__
#define __BALANCE_PROCESS_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef  __BALANCE_PROCESS_C__
#define BALANCE
#else
#define BALANCE extern
#endif

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
BALANCE	void	sBalanceInit(void);
BALANCE	void	sBalanceParaUpdate(void);
BALANCE	void	sBalanceStatusChk(INT8U bFilter);
BALANCE	void	sBalancePauseAutoRst(INT16U wFilter);
BALANCE	void	sBalanceTestStatusAutoRst(INT16U wFilter);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
BALANCE	INT16U	swGetBalanceCellStatus(void);
BALANCE	INT8U	sbGetBalanceUnbalance(void);
BALANCE	INT8U	sbGetBalancePassivePause(void);
BALANCE	INT8U	sbGetBreakWireFault(void);
BALANCE	INT16U	swGetBalanceTestStatus(void);

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
BALANCE	void	sSetBalanceCellNum(INT8U bNum);
BALANCE	void	sSetBalanceTempNew(INT16S wTemp);
BALANCE	void	sRstBalanceCellStatus(void);
BALANCE	void	sSetBalanceCellStatus(INT16U wStatus);
BALANCE	void	sClrBalanceCellStatus(INT16U wStatus);
BALANCE	void	sSetBalancePassivePause(INT8U bStatus);
BALANCE	void	sSetBreakWireFault(INT8U bStatus);
BALANCE	void	sSetBalanceTestStatus(INT16U bStatus);

#endif	// __BALANCE_PROCESS_H__

