/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		SciRoute.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __SCI_ROUTE_H__
#define __SCI_ROUTE_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef  __SCI_ROUTE_C__
#define SCI_ROUTE
#else
#define SCI_ROUTE  extern
#endif

#define	cROUTE_PROT_SELFDEFINE		0
#define	cROUTE_PROT_OPENBLT			1
#define	cROUTE_PROT_VOLTRONIC		2
#define	cROUTE_PROT_PYLON			3
#define	cROUTE_PROT_Growatt			4

#define cProtocol_Branch            cROUTE_PROT_VOLTRONIC
/********************************************************************************
* Routines' implementations														*
********************************************************************************/
SCI_ROUTE	void	sSciRouting(INT8U sciid);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
SCI_ROUTE	INT8U	sbGetRouteProt(void);

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
SCI_ROUTE	void	sSetRouteProt(INT8U prot);

#endif	// __SCI_ROUTE_H__

