/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		ProtectionModule.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define		__PROTECTION_MODULE_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/


/********************************************************************************
* Constants																		*
********************************************************************************/


/********************************************************************************
* External variables															*
********************************************************************************/


/********************************************************************************
* External routine prototypes													*
********************************************************************************/


/********************************************************************************
* Input variables																*
********************************************************************************/
INT16S	wHsTempHiAlm;
INT16S	wHsTempHiAlmBack;
INT16S	wHsTempHi;
INT16S	wHsTempHiBack;

/********************************************************************************
* Output variables																*
********************************************************************************/
INT16S	wHsTempNew;

/********************************************************************************
* Internal variables															*
********************************************************************************/
union PROTECTION_STATUS_TABLE
{
	struct PROTECTION_STATUS_BIT_FEILD
	{
		// BIT0-3
		INT16U	HsTempHiAlm:1;
		INT16U	HsTempHi:1;
		INT16U	Rsvd1:2;
		
		// BIT4-7
		INT16U	Rsvd2:4;
		
		// BIT8-11
		INT16U	Rsvd3:4;
		
		// BIT12-15
		INT16U	Rsvd4:4;
	} bits;
	INT16U data;
} fProtection;

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/


/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void	sProtectionModuleInit(void)
{
	fProtection.data = 0;
	wHsTempNew = 0;
	
	sProtectionParaUpdate();
}

void	sProtectionParaUpdate(void)
{
	wHsTempHiAlm = (INT8S)swGetEepromHsTempHiAlm();
	wHsTempHiAlmBack = wHsTempHiAlm - 10;
	wHsTempHi = (INT8S)swGetEepromHsTempHi();
	wHsTempHiBack = wHsTempHi - 20;
}

void    sHsTempHiAlmChk(INT16U wFilter1, INT16U wFilter2)
{
	static  INT16U   wHsTempHiAlmCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode))
	{
		wHsTempHiAlmCnt = 0;
		fProtection.bits.HsTempHiAlm = false;
	}
    else if(fProtection.bits.HsTempHiAlm == true)
	{
		if(sb16UUnderLevelChk(wHsTempNew,wHsTempHiAlmBack,wFilter2,&wHsTempHiAlmCnt) == true)
		{
			fProtection.bits.HsTempHiAlm = false;
		}
	}
	else
	{
		if(sb16UOverLevelChk(wHsTempNew,wHsTempHiAlm,wFilter1,&wHsTempHiAlmCnt) == true)
		{
			fProtection.bits.HsTempHiAlm = true;
		}
	}
}

void    sHsTempHiChk(INT16U wFilter1, INT16U wFilter2)
{
	static  INT16U   wHsTempHiCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode))
	{
		wHsTempHiCnt = 0;
		fProtection.bits.HsTempHi = false;
	}
    else if(fProtection.bits.HsTempHi == true)
	{
		if(sb16UUnderLevelChk(wHsTempNew,wHsTempHiBack,wFilter2,&wHsTempHiCnt) == true)
		{
			fProtection.bits.HsTempHi = false;
		}
	}
	else
	{
		if(sb16UOverLevelChk(wHsTempNew,wHsTempHi,wFilter1,&wHsTempHiCnt) == true)
		{
			fProtection.bits.HsTempHi = true;
		}
	}
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
INT16S	swGetHsTempNew(void)
{
	return wHsTempNew;
}

INT8U	sbGetHsTempHiAlm(void)
{
	return fProtection.bits.HsTempHiAlm;
}

INT8U	sbGetHsTempHi(void)
{
	return fProtection.bits.HsTempHi;
}

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
void	sSetHsTempNew(INT16S wTemp)
{
	wHsTempNew = wTemp;
}

