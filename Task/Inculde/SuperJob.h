/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		SuperJob.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __SUPER_JOB_H__
#define __SUPER_JOB_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef	__SUPER_JOB_C__
#define	SUPER
#else
#define	SUPER  extern
#endif

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
SUPER	void	sSuperJob(void);
SUPER	void	sSuperJobInit(void);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
SUPER	INT16U	swGetBmsFaultFlag(void);
SUPER	INT16U	swGetBmsNoticeFlag(void);
SUPER	INT16U	swGetBmsAlarmFlag(void);
SUPER	INT8U	sbGetBmsFaultCode(void);
SUPER	INT8U	sbGetFaultRestartFlag(void);
SUPER	INT8U	sbGetFaultPowerOnFlag(void);
SUPER	INT8U	sbGetStandbySteadyFlag(void);
SUPER	INT8U	sbGetChargeSteadyFlag(void);
SUPER	INT8U	sbGetDischargeSteadyFlag(void);

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
SUPER	void	sSetFaultRestartFlag(INT8U bFlag);
SUPER	void	sSetFaultPowerOnFlag(INT8U bFlag);
SUPER	void	sSetStandbySteadyFlag(INT8U bFlag);
SUPER	void	sSetChargeSteadyFlag(INT8U bFlag);
SUPER	void	sSetDischargeSteadyFlag(INT8U bFlag);

#endif	// __SUPER_JOB_H__

