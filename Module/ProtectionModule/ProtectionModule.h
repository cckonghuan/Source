/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		ProtectionModule.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __PROTECTION_MODULE_H__
#define __PROTECTION_MODULE_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef	__PROTECTION_MODULE_C__
#define	PROTECTION
#else
#define	PROTECTION	extern
#endif

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
PROTECTION	void	sProtectionModuleInit(void);
PROTECTION	void	sProtectionParaUpdate(void);
PROTECTION	void    sHsTempHiAlmChk(INT16U wFilter1, INT16U wFilter2);
PROTECTION	void    sHsTempHiChk(INT16U wFilter1, INT16U wFilter2);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
PROTECTION	INT16S	swGetHsTempNew(void);
PROTECTION	INT8U	sbGetHsTempHiAlm(void);
PROTECTION	INT8U	sbGetHsTempHi(void);

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
PROTECTION	void	sSetHsTempNew(INT16S wTemp);

#endif	// __PROTECTION_MODULE_H__

