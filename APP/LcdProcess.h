/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		LcdProcess.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __LCD_PROCESS_H__
#define __LCD_PROCESS_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include "OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef __LCD_PROCESS_C__
#define LCD_PROCESS
#else
#define LCD_PROCESS	extern
#endif

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
LCD_PROCESS	void	sLcdInitial(void);
LCD_PROCESS	void	sLcdTask(INT16U wFilter);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/


/********************************************************************************
* Input interface Routines														*
********************************************************************************/
LCD_PROCESS	void	sRstLCDLoopCnt(void);


#endif // __LCD_PROCESS_H__
