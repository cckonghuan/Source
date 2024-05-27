/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		SoxProcess.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __SOX_PROCESS_H__
#define __SOX_PROCESS_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef  __SOX_PROCESS_C__
#define SOX_PROCESS
#else
#define SOX_PROCESS extern
#endif

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
SOX_PROCESS	void	sSoxInit(void);
SOX_PROCESS	void	sSoxParaUpdate(void);
SOX_PROCESS	void	sSoxOcvChk(INT16U wFilter1, INT16U wFilter2);
SOX_PROCESS	void	sSoxAhCalc(INT16S wCurr);
SOX_PROCESS	void	sSocEstimate(INT16S wTemp);
SOX_PROCESS	void	sSocFilter(INT16U wFilter);
SOX_PROCESS	void	sSohEstimate(void);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
SOX_PROCESS	INT16U	swGetSocNew(void);
SOX_PROCESS	INT16U	swGetSohNew(void);
SOX_PROCESS	INT16U	swGetSoxAhReal(void);
SOX_PROCESS	INT16U	swGetSoxAhMax(void);
SOX_PROCESS	INT16U	swGetSoxAhComp(void);

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
SOX_PROCESS	void	sSetSoxAhReal(INT16U wAH);
SOX_PROCESS	void	sSetSoxAhMax(INT16U wAH);
SOX_PROCESS	void	sSetSoxCompTemp(INT16S wTemp);

#endif	// __SOX_PROCESS_H__

