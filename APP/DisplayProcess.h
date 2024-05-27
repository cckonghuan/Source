/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		DisplayProcess.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __DISPLAY_PROCESS_H__
#define __DISPLAY_PROCESS_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef  __DISPLAY_PROCESS_C__
#define DISPLAY_PROCESS
#else
#define DISPLAY_PROCESS  extern
#endif

#define	cDISPLAY_STATUS_IDLE		0
#define	cDISPLAY_STATUS_DSG			1
#define	cDISPLAY_STATUS_CHG			2

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
DISPLAY_PROCESS	void	sDisplayProcessInit(void);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
DISPLAY_PROCESS	void    sDisplaySoc(INT16U wTimeBase);
DISPLAY_PROCESS	void    sDisplayKey(INT16U wTimeBase);
DISPLAY_PROCESS	void    sDisplaySocSync(INT8U bFlag);
DISPLAY_PROCESS	void    sDisplayKeySync(INT8U bFlag);

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
DISPLAY_PROCESS	void	sSetDisplayBmsMode(INT8U bMode);
DISPLAY_PROCESS	void	sSetDisplayBmsStatus(INT8U bStatus);
DISPLAY_PROCESS	void	sSetDisplayBmsSoc(INT16U wSoc);

#endif	// __DISPLAY_PROCESS_H__
