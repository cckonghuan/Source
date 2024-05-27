/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		InterfaceJob.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __INTERFACE_JOB_H__
#define __INTERFACE_JOB_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef  __INTERFACE_JOB_C__
#define INTERFACE
#else
#define INTERFACE   extern
#endif

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
INTERFACE	void	sInterfaceJob(void);
INTERFACE	void	sInterfaceJobInit(void);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
INTERFACE	INT8U	sbGetInitializeFinished(void);
INTERFACE	INT8U	sbGetModBusDispMode(void);

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
INTERFACE	void	sSetModBusDispMode(INT8U bMode);

#endif	// __INTERFACE_JOB_H__

