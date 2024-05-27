/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		BatteryModule.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define		__BATTERY_MODULE_C__

/********************************************************************************
* Include head files                                                            *
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
INT16U	wBattVoltHi;			// 0.01V
INT16U	wBattVoltHiBack;		// 0.01V
INT16U	wBattVoltLo;			// 0.01V
INT16U	wBattVoltLoBack;		// 0.01V

INT16U	wCellVoltHiAlm; 		// 0.001V
INT16U	wCellVoltHiAlmBack; 	// 0.001V
INT16U	wCellVoltHi;			// 0.001V
INT16U	wCellVoltHiBack;		// 0.001V
INT16U	wCellVoltHi2;			// 0.001V
INT16U	wCellVoltHi2Back;		// 0.001V

INT16U	wCellVoltLoAlm; 		// 0.001V
INT16U	wCellVoltLoAlmBack;		// 0.001V
INT16U	wCellVoltLo;			// 0.001V
INT16U	wCellVoltLoBack;		// 0.001V
INT16U	wCellVoltLo2;			// 0.001V
INT16U	wCellVoltLo2Back;		// 0.001V

INT16S	wBattChgCurrHiAlmBack;	// 0.1A
INT16S	wBattChgCurrHiAlm;		// 0.1A
INT16S	wBattChgCurrHi; 		// 0.1A
INT16S	wBattChgCurrHi2; 		// 0.1A

INT16S	wBattDcgCurrHiAlmBack;	// 0.1A
INT16S	wBattDcgCurrHiAlm;		// 0.1A
INT16S	wBattDcgCurrHi; 		// 0.1A
INT16S	wBattDcgCurrHi2; 		// 0.1A

INT8S	bCellChgTempHiAlmBack;
INT8S	bCellChgTempHiAlm;
INT8S	bCellChgTempHi;
INT8S	bCellChgTempHi2;

INT8S	bCellChgTempLoAlmBack;
INT8S	bCellChgTempLoAlm;
INT8S	bCellChgTempLo;
INT8S	bCellChgTempLo2;

INT8S	bCellDcgTempHiAlmBack;
INT8S	bCellDcgTempHiAlm;
INT8S	bCellDcgTempHi;
INT8S	bCellDcgTempHi2;

INT8S	bCellDcgTempLoAlmBack;
INT8S	bCellDcgTempLoAlm;
INT8S	bCellDcgTempLo;
INT8S	bCellDcgTempLo2;

INT16S	wCellVoltErrHiAlm;		// 1mV
INT16S	wCellVoltErrHiAlmBack;	// 1mV

INT8S	bBatteryStatus;
INT8U	bBattVoltLo2Disable;
INT8U	bCellVoltLo2Disable;

/********************************************************************************
* Output variables																*
********************************************************************************/
INT16S	wBattVoltNew;			// 0.01V
INT16S	wBattCurrNew;			// 0.1A
INT16S	wCellVoltNew[cADC_CELL_VOLT_MAX];		// 1mV
INT16S	wCellTempNew[cADC_CELL_TEMP_MAX];
INT16S	wBattVoltFiltNew;		// 0.01V
INT16S	wBattCurrFiltNew;		// 0.1A
INT16S	wCellVoltFiltNew[cADC_CELL_VOLT_MAX];	// 1mV
INT16S	wCellTempFiltNew[cADC_CELL_TEMP_MAX];

typedef struct{
	INT16S	wAvg;
	INT16S	wMax;
	INT16S	wMin;
	INT8U	bMaxNo;	// 从0开始
	INT8U	bMinNo;	// 从0开始
}BAT_AVG_MAX_MIN_STRUCT;
BAT_AVG_MAX_MIN_STRUCT strCellVoltNew;
BAT_AVG_MAX_MIN_STRUCT strCellVoltFiltNew;
BAT_AVG_MAX_MIN_STRUCT strCellTempNew;
BAT_AVG_MAX_MIN_STRUCT strCellTempFiltNew;

union BATTERY1_STATUS_TABLE
{
	struct BATTERY1_STATUS_BIT_FEILD
	{
		// BIT0-3
		INT16U	CellVoltHiAlm:1;
		INT16U	CellVoltHi:1;
		INT16U	CellVoltHi2:1;
		INT16U	BattVoltHi:1;
		
		// BIT4-7
		INT16U	CellVoltLoAlm:1;
		INT16U	CellVoltLo:1;
		INT16U	CellVoltLo2:1;
		INT16U	BattVoltLo:1;
		
		// BIT8-11
		INT16U	BattChgCurrHiAlm:1;
		INT16U	BattChgCurrHi:1;
		INT16U	BattChgCurrHi2:1;
		INT16U	CellVoltErrHiAlm:1;
		
		// BIT12-15
		INT16U	BattDcgCurrHiAlm:1;
		INT16U	BattDcgCurrHi:1;
		INT16U	BattDcgCurrHi2:1;
		INT16U	Rsvd4:1;
	} bits;
	INT16U data;
} fBatt1;

union BATTERY2_STATUS_TABLE
{
	struct BATTERY2_STATUS_BIT_FEILD
	{
		// BIT0-3
		INT16U	CellChgTempHiAlm:1;
		INT16U	CellChgTempHi:1;
		INT16U	CellChgTempHi2:1;
		INT16U	Rsvd1:1;
		
		// BIT4-7
		INT16U	CellChgTempLoAlm:1;
		INT16U	CellChgTempLo:1;
		INT16U	CellChgTempLo2:1;
		INT16U	Rsvd2:1;
		
		// BIT8-11
		INT16U	CellDcgTempHiAlm:1;
		INT16U	CellDcgTempHi:1;
		INT16U	CellDcgTempHi2:1;
		INT16U	Rsvd3:1;
		
		// BIT12-15
		INT16U	CellDcgTempLoAlm:1;
		INT16U	CellDcgTempLo:1;
		INT16U	CellDcgTempLo2:1;
		INT16U	Rsvd4:1;
	} bits;
	INT16U data;
} fBatt2;

/********************************************************************************
* Internal variables															*
********************************************************************************/


/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
void    sScanAvgMaxMin(INT16S *pBuf, INT8U bNum, BAT_AVG_MAX_MIN_STRUCT *pAvgMaxMin);


/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void    sBatteryModuleInit(void)
{
	INT8U	i;
	
	fBatt1.data = 0;
	fBatt2.data = 0;
	
	wBattVoltNew = 0;
	wBattCurrNew = 0;
	wBattVoltFiltNew = 0;
	wBattCurrFiltNew = 0;
	for(i = 0; i < cADC_CELL_VOLT_MAX; i++)
	{
		wCellVoltNew[i] = 0x7FFF;
		wCellVoltFiltNew[i] = 0x7FFF;
	}
	
	for(i = 0; i < cADC_CELL_TEMP_MAX; i++)
	{
		wCellTempNew[i] = 0x7FFF;
		wCellTempFiltNew[i] = 0x7FFF;
	}
	
	bBatteryStatus = cBATTERY_IDLE;
	
	memset(&strCellVoltNew, 0, sizeof(strCellVoltNew));
	memset(&strCellVoltFiltNew, 0, sizeof(strCellVoltFiltNew));
	memset(&strCellTempNew, 0, sizeof(strCellTempNew));
	memset(&strCellTempFiltNew, 0, sizeof(strCellTempFiltNew));
	
	bBattVoltLo2Disable = 0;
	bCellVoltLo2Disable = 0;
	
	sBatteryParaUpdate();
}

void    sBatteryParaUpdate(void)
{
	wCellVoltHiAlm = swGetEepromCellVoltHiAlm();			// 3.60V
	wCellVoltHiAlmBack = swGetEepromCellVoltHiAlmBack();	// 3.50V
	wCellVoltHi = swGetEepromCellVoltHi();					// 3.65V
	wCellVoltHiBack = swGetEepromCellVoltHiBack();			// 3.50V
	wCellVoltHi2 = swGetEepromCellVoltHi2();				// 3.75V
	wCellVoltHi2Back = wCellVoltHi;							// 3.65V
	
	wCellVoltLoAlm = swGetEepromCellVoltLoAlm();			// 2.50V
	wCellVoltLoAlmBack = swGetEepromCellVoltLoAlmBack();	// 2.75V
	wCellVoltLo = swGetEepromCellVoltLo();					// 2.30V
	wCellVoltLoBack = swGetEepromCellVoltLoBack();			// 2.75V
	wCellVoltLo2 = swGetEepromCellVoltLo2();				// 0V(default)
	wCellVoltLo2Back = wCellVoltLo2 + 200;					// 1.00V
	if(wCellVoltLo2Back < 1000)
	{
		wCellVoltLo2Back = 1000;
	}
	
	wBattChgCurrHiAlmBack = (INT16S)swGetEepromBattChgCurrHiAlmBack();
	wBattChgCurrHiAlm = (INT16S)swGetEepromBattChgCurrHiAlm();
	wBattChgCurrHi = (INT16S)swGetEepromBattChgCurrHi();
	wBattChgCurrHi2 = (INT16S)swGetEepromBattChgCurrHi2();
	
	wBattDcgCurrHiAlmBack = -(INT16S)swGetEepromBattDcgCurrHiAlmBack();
	wBattDcgCurrHiAlm = -(INT16S)swGetEepromBattDcgCurrHiAlm();
	wBattDcgCurrHi = -(INT16S)swGetEepromBattDcgCurrHi();
	wBattDcgCurrHi2 = -(INT16S)swGetEepromBattDcgCurrHi2();
	
	bCellChgTempHiAlmBack = (INT8S)swGetEepromCellChgTempHiAlmBack();
	bCellChgTempHiAlm = (INT8S)swGetEepromCellChgTempHiAlm();
	bCellChgTempHi = (INT8S)swGetEepromCellChgTempHi();
	bCellChgTempHi2 = (INT8S)swGetEepromCellChgTempHi2();
	
	bCellChgTempLoAlmBack = (INT8S)swGetEepromCellChgTempLoAlmBack();
	bCellChgTempLoAlm = (INT8S)swGetEepromCellChgTempLoAlm();
	bCellChgTempLo = (INT8S)swGetEepromCellChgTempLo();
	bCellChgTempLo2 = (INT8S)swGetEepromCellChgTempLo2();
	
	bCellDcgTempHiAlmBack = (INT8S)swGetEepromCellDcgTempHiAlmBack();
	bCellDcgTempHiAlm = (INT8S)swGetEepromCellDcgTempHiAlm();
	bCellDcgTempHi = (INT8S)swGetEepromCellDcgTempHi();
	bCellDcgTempHi2 = (INT8S)swGetEepromCellDcgTempHi2();
	
	bCellDcgTempLoAlmBack = (INT8S)swGetEepromCellDcgTempLoAlmBack();
	bCellDcgTempLoAlm = (INT8S)swGetEepromCellDcgTempLoAlm();
	bCellDcgTempLo = (INT8S)swGetEepromCellDcgTempLo();
	bCellDcgTempLo2 = (INT8S)swGetEepromCellDcgTempLo2();
	
	wBattVoltHi = ((INT32U)wCellVoltHi2 * swGetBmsCellVoltNum()) / 10;
	wBattVoltHiBack = ((INT32U)wCellVoltHi2Back * swGetBmsCellVoltNum()) / 10;
	wBattVoltLo = ((INT32U)wCellVoltLo2 * swGetBmsCellVoltNum()) / 10;
	wBattVoltLoBack = ((INT32U)wCellVoltLo2Back * swGetBmsCellVoltNum()) / 10;
	
	wCellVoltErrHiAlm = 500;
	wCellVoltErrHiAlmBack = 300;
}

void    sScanCellVoltAvgMaxMin(INT8U bNum)
{
	sScanAvgMaxMin(wCellVoltNew, bNum, &strCellVoltNew);
	sScanAvgMaxMin(wCellVoltFiltNew, bNum, &strCellVoltFiltNew);
}

void    sScanCellTempAvgMaxMin(INT8U bNum)
{
	sScanAvgMaxMin(wCellTempNew, bNum, &strCellTempNew);
	sScanAvgMaxMin(wCellTempFiltNew, bNum, &strCellTempFiltNew);
}

void    sScanAvgMaxMin(INT16S *pBuf, INT8U bNum, BAT_AVG_MAX_MIN_STRUCT *pAvgMaxMin)
{
	OS_CPU_SR  cpu_sr;
	BAT_AVG_MAX_MIN_STRUCT strTemp;
	INT32S dwAvgValueAcc;
	INT8U i, bNumMax;
	
	bNumMax = bNum;
	i = 0;
	pBuf += i;
	dwAvgValueAcc = *pBuf;
	strTemp.bMaxNo = i;
	strTemp.wMax = *pBuf;
	strTemp.bMinNo = i;
	strTemp.wMin = *pBuf;
	i++;
	pBuf++;
	for(; i < bNumMax; i++,pBuf++)
	{
		dwAvgValueAcc += *pBuf;
		if(strTemp.wMax < *pBuf)
		{
			strTemp.wMax = *pBuf;
			strTemp.bMaxNo = i;
		}
		if(strTemp.wMin > *pBuf)
		{
			strTemp.wMin = *pBuf;
			strTemp.bMinNo = i;
		}
	}
	strTemp.wAvg = (INT16S)((dwAvgValueAcc + (bNumMax >> 1)) / bNumMax);
	
	OS_ENTER_CRITICAL();
	pAvgMaxMin->wAvg = strTemp.wAvg;
	pAvgMaxMin->wMax = strTemp.wMax;
	pAvgMaxMin->wMin = strTemp.wMin;
	pAvgMaxMin->bMaxNo = strTemp.bMaxNo;
	pAvgMaxMin->bMinNo = strTemp.bMinNo;
	OS_EXIT_CRITICAL();
}

void    sBattVoltHiChk(INT16U wFilter1, INT16U wFilter2)
{
	static  INT16U   wChkCnt = 0;
	INT16S	wTemp;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt1.bits.BattVoltHi = false;
	}
    else if(fBatt1.bits.BattVoltHi == true)
	{
//		if(bBatteryStatus == cBATTERY_DISCHARGING)
//		{
//			wTemp = wBattVoltHi;
//		}
//		else
		{
			wTemp = wBattVoltHiBack;
		}
		if(sb16UUnderLevelChk(wBattVoltNew,wTemp,wFilter2,&wChkCnt) == true)
		{
			fBatt1.bits.BattVoltHi = false;
		}
	}
	else
	{
		if(sb16UOverLevelChk(wBattVoltNew,wBattVoltHi,wFilter1,&wChkCnt) == true)
		{
			fBatt1.bits.BattVoltHi = true;
		}
	}
}

void    sBattVoltLoChk(INT16U wFilter1, INT16U wFilter2)
{
	static  INT16U   wChkCnt = 0;
	INT16S	wTemp;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt1.bits.BattVoltLo = false;
	}
	else if(bBattVoltLo2Disable == true)
	{
		wChkCnt = 0;
		fBatt1.bits.BattVoltLo = false;
	}
	else if(wBattVoltLo == 0)		// 使能低压充电功能(默认使能)
	{
		fBatt1.bits.BattVoltLo = false;
	}
    else if(fBatt1.bits.BattVoltLo == true)
	{
//		if(bBatteryStatus == cBATTERY_CHARGING)
//		{
//			wTemp = wBattVoltLo;
//		}
//		else
		{
			wTemp = wBattVoltLoBack;
		}
		if(sb16UOverLevelChk(wBattVoltNew,wTemp,wFilter2,&wChkCnt) == true)
		{
			fBatt1.bits.BattVoltLo = false;
		}
	}
	else
	{
		if(sb16UUnderLevelChk(wBattVoltNew,wBattVoltLo,wFilter1,&wChkCnt) == true)
		{
			fBatt1.bits.BattVoltLo = true;
		}
	}
}

void    sCellVoltHiAlmChk(INT16U wFilter1, INT16U wFilter2)
{
	static  INT16U   wChkCnt = 0;
	INT16S	wTemp;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt1.bits.CellVoltHiAlm = false;
	}
    else if(fBatt1.bits.CellVoltHiAlm == true)
	{
//		if(bBatteryStatus == cBATTERY_DISCHARGING)
//		{
//			wTemp = wCellVoltHiAlm;
//		}
//		else
		{
			wTemp = wCellVoltHiAlmBack;
		}
		if(sb16UUnderLevelChk(strCellVoltNew.wMax,wTemp,wFilter2,&wChkCnt) == true)
		{
			fBatt1.bits.CellVoltHiAlm = false;
		}
	}
	else
	{
		if(sb16UOverLevelChk(strCellVoltNew.wMax,wCellVoltHiAlm,wFilter1,&wChkCnt) == true)
		{
			fBatt1.bits.CellVoltHiAlm = true;
		}
	}
}

void    sCellVoltHiChk(INT16U wFilter1, INT16U wFilter2)
{
	static  INT16U   wChkCnt = 0;
	INT16S	wTemp;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt1.bits.CellVoltHi = false;
	}
    else if(fBatt1.bits.CellVoltHi == true)
	{
//		if(bBatteryStatus == cBATTERY_DISCHARGING)
//		{
//			wTemp = wCellVoltHi;
//		}
//		else
		{
			wTemp = wCellVoltHiBack;
		}
		if(sb16UUnderLevelChk(strCellVoltNew.wMax,wTemp,wFilter2,&wChkCnt) == true)
		{
			fBatt1.bits.CellVoltHi = false;
		}
	}
	else
	{
		if(sb16UOverLevelChk(strCellVoltNew.wMax,wCellVoltHi,wFilter1,&wChkCnt) == true)
		{
			fBatt1.bits.CellVoltHi = true;
		}
	}
}

void    sCellVoltHi2Chk(INT16U wFilter1, INT16U wFilter2)
{
	static	INT16U	 wChkCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt1.bits.CellVoltHi2 = false;
	}
    else if(fBatt1.bits.CellVoltHi2 == true)
	{
		if(sb16UUnderLevelChk(strCellVoltNew.wMax,wCellVoltHi2Back,wFilter2,&wChkCnt) == true)
		{
			fBatt1.bits.CellVoltHi2 = false;
		}
	}
	else
	{
		if(sb16UOverLevelChk(strCellVoltNew.wMax,wCellVoltHi2,wFilter1,&wChkCnt) == true)
		{
			fBatt1.bits.CellVoltHi2 = true;
		}
	}
}

void    sCellVoltLoAlmChk(INT16U wFilter1, INT16U wFilter2)
{
	static  INT16U   wChkCnt = 0;
	INT16S	wTemp;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt1.bits.CellVoltLoAlm = false;
	}
    else if(fBatt1.bits.CellVoltLoAlm == true)
	{
//		if(bBatteryStatus == cBATTERY_CHARGING)
//		{
//			wTemp = wCellVoltLoAlm;
//		}
//		else
		{
			wTemp = wCellVoltLoAlmBack;
		}
		if(sb16UOverLevelChk(strCellVoltNew.wMin,wTemp,wFilter2,&wChkCnt) == true)
		{
			fBatt1.bits.CellVoltLoAlm = false;
		}
	}
	else
	{
		if(sb16UUnderLevelChk(strCellVoltNew.wMin,wCellVoltLoAlm,wFilter1,&wChkCnt) == true)
		{
			fBatt1.bits.CellVoltLoAlm = true;
		}
	}
}

void    sCellVoltLoChk(INT16U wFilter1, INT16U wFilter2)
{
	static	INT16U	 wChkCnt = 0;
	INT16S	wTemp;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt1.bits.CellVoltLo = false;
	}
    else if(fBatt1.bits.CellVoltLo == true)
	{
//		if(bBatteryStatus == cBATTERY_CHARGING)
//		{
//			wTemp = wCellVoltLo;
//		}
//		else
		{
			wTemp = wCellVoltLoBack;
		}
		if(sb16UOverLevelChk(strCellVoltNew.wMin,wTemp,wFilter2,&wChkCnt) == true)
		{
			fBatt1.bits.CellVoltLo = false;
		}
	}
	else
	{
		if(sb16UUnderLevelChk(strCellVoltNew.wMin,wCellVoltLo,wFilter1,&wChkCnt) == true)
		{
			fBatt1.bits.CellVoltLo = true;
		}
	}
}

void    sCellVoltLo2Chk(INT16U wFilter1, INT16U wFilter2)
{
	static	INT16U	 wChkCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt1.bits.CellVoltLo2 = false;
	}
	else if(bCellVoltLo2Disable == true)
	{
		wChkCnt = 0;
		fBatt1.bits.CellVoltLo2 = false;
	}
	else if(wCellVoltLo2 == 0)		// 使能低压充电功能(默认使能)
	{
		wChkCnt = 0;
		fBatt1.bits.CellVoltLo2 = false;
	}
    else if(fBatt1.bits.CellVoltLo2 == true)
	{
		if(sb16UOverLevelChk(strCellVoltNew.wMin,wCellVoltLo2Back,wFilter2,&wChkCnt) == true)
		{
			fBatt1.bits.CellVoltLo2 = false;
		}
	}
	else
	{
		if(sb16UUnderLevelChk(strCellVoltNew.wMin,wCellVoltLo2,wFilter1,&wChkCnt) == true)
		{
			fBatt1.bits.CellVoltLo2 = true;
		}
	}
}

void    sBattChgCurrHiAlmChk(INT16U wFilter1, INT16U wFilter2)
{
	static  INT16U   wChkCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt1.bits.BattChgCurrHiAlm = false;
	}
    else if(fBatt1.bits.BattChgCurrHiAlm == true)
	{
		if(sb16UUnderLevelChk(wBattCurrNew,wBattChgCurrHiAlmBack,wFilter2,&wChkCnt) == true)
		{
			fBatt1.bits.BattChgCurrHiAlm = false;
		}
	}
	else
	{
		if(sb16UOverLevelChk(wBattCurrNew,wBattChgCurrHiAlm,wFilter1,&wChkCnt) == true)
		{
			fBatt1.bits.BattChgCurrHiAlm = true;
		}
	}
}

void    sBattChgCurrHiChk(INT16U wFilter1, INT16U wFilter2)
{
	static	INT16U	 wChkCnt = 0;
	static	INT16U	 wBackChkCnt = 0;
	
 	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		wBackChkCnt = 0;
		fBatt1.bits.BattChgCurrHi = false;
	}
	else if(bBatteryStatus == cBATTERY_DISCHARGING)
	{
		wChkCnt = 0;
		fBatt1.bits.BattChgCurrHi = false;
	}
	else if(fBatt1.bits.BattChgCurrHi == true)
	{
		wChkCnt = 0;
		if(sb16UUnderLevelChk(wBattCurrNew,wBattChgCurrHiAlmBack,wFilter2,&wBackChkCnt) == true)
		{
			fBatt1.bits.BattChgCurrHi = false;
		}
	}
	else
	{
		wBackChkCnt = 0;
		if(sb16UOverLevelChk(wBattCurrNew,wBattChgCurrHi,wFilter1,&wChkCnt) == true)
		{
			fBatt1.bits.BattChgCurrHi = true;
		}
	}
}

void    sBattChgCurrHi2Chk(INT16U wFilter1, INT16U wFilter2)
{
	static  INT16U   wChkCnt = 0;
	static  INT16U   wBackChkCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		wBackChkCnt = 0;
		fBatt1.bits.BattChgCurrHi2 = false;
	}
    else if(fBatt1.bits.BattChgCurrHi2 == true)
	{
		wChkCnt = 0;
		if(sb16UUnderLevelChk(wBattCurrNew,wBattChgCurrHiAlmBack,wFilter2,&wBackChkCnt) == true)
		{
			fBatt1.bits.BattChgCurrHi2 = false;
		}
	}
	else
	{
		wBackChkCnt = 0;
		if(sb16UOverLevelChk(wBattCurrNew,wBattChgCurrHi2,wFilter1,&wChkCnt) == true)
		{
			fBatt1.bits.BattChgCurrHi2 = true;
		}
	}
}

void    sBattDcgCurrHiAlmChk(INT16U wFilter1, INT16U wFilter2)
{
	static  INT16U   wChkCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt1.bits.BattDcgCurrHiAlm = false;
	}
    else if(fBatt1.bits.BattDcgCurrHiAlm == true)
	{
		if(sb16UOverLevelChk(wBattCurrNew,wBattDcgCurrHiAlmBack,wFilter2,&wChkCnt) == true)
		{
			fBatt1.bits.BattDcgCurrHiAlm = false;
		}
	}
	else
	{
		if(sb16UUnderLevelChk(wBattCurrNew,wBattDcgCurrHiAlm,wFilter1,&wChkCnt) == true)
		{
			fBatt1.bits.BattDcgCurrHiAlm = true;
		}
	}
}

void    sBattDcgCurrHiChk(INT16U wFilter1, INT16U wFilter2)
{
	static	INT16U	 wChkCnt = 0;
	static	INT16U	 wBackChkCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		wBackChkCnt = 0;
		fBatt1.bits.BattDcgCurrHi = false;
	}
	else if(bBatteryStatus == cBATTERY_CHARGING)
	{
		wChkCnt = 0;
		fBatt1.bits.BattDcgCurrHi = false;
	}
    else if(fBatt1.bits.BattDcgCurrHi == true)
	{
		wChkCnt = 0;
		if(sb16UOverLevelChk(wBattCurrNew,wBattDcgCurrHiAlmBack,wFilter2,&wBackChkCnt) == true)
		{
			fBatt1.bits.BattDcgCurrHi = false;
		}
	}
	else
	{
		wBackChkCnt = 0;
		if(sb16UUnderLevelChk(wBattCurrNew,wBattDcgCurrHi,wFilter1,&wChkCnt) == true)
		{
			fBatt1.bits.BattDcgCurrHi = true;
		}
	}
}

void    sBattDcgCurrHi2Chk(INT16U wFilter1, INT16U wFilter2)
{
	static	INT16U	 wChkCnt = 0;
	static	INT16U	 wBackChkCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		wBackChkCnt = 0;
		fBatt1.bits.BattDcgCurrHi2 = false;
	}
    else if(fBatt1.bits.BattDcgCurrHi2 == true)
	{
		wChkCnt = 0;
		if(sb16UOverLevelChk(wBattCurrNew,wBattDcgCurrHiAlmBack,wFilter2,&wBackChkCnt) == true)
		{
			fBatt1.bits.BattDcgCurrHi2 = false;
		}
	}
	else
	{
		wBackChkCnt = 0;
		if(sb16UUnderLevelChk(wBattCurrNew,wBattDcgCurrHi2,wFilter1,&wChkCnt) == true)
		{
			fBatt1.bits.BattDcgCurrHi2 = true;
		}
	}
}

void    sCellChgTempHiAlmChk(INT16U wFilter1, INT16U wFilter2)
{
	static  INT16U   wChkCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt2.bits.CellChgTempHiAlm = false;
	}
	else if(bBatteryStatus == cBATTERY_DISCHARGING)
	{
		wChkCnt = 0;
		fBatt2.bits.CellChgTempHiAlm = false;
	}
    else if(fBatt2.bits.CellChgTempHiAlm == true)
	{
		if(sb16UUnderLevelChk(strCellTempNew.wMax,bCellChgTempHiAlmBack,wFilter2,&wChkCnt) == true)
		{
			fBatt2.bits.CellChgTempHiAlm = false;
		}
	}
	else
	{
		if(sb16UOverLevelChk(strCellTempNew.wMax,bCellChgTempHiAlm,wFilter1,&wChkCnt) == true)
		{
			if(bBatteryStatus == cBATTERY_CHARGING)
			{
				fBatt2.bits.CellChgTempHiAlm = true;
			}
		}
	}
}

void    sCellChgTempHiChk(INT16U wFilter1, INT16U wFilter2)
{
	static	INT16U	 wChkCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt2.bits.CellChgTempHi = false;
	}
	else if(bBatteryStatus == cBATTERY_DISCHARGING)
	{
		wChkCnt = 0;
		fBatt2.bits.CellChgTempHi = false;
	}
    else if(fBatt2.bits.CellChgTempHi == true)
	{
		if(sb16UUnderLevelChk(strCellTempNew.wMax,bCellChgTempHiAlmBack,wFilter2,&wChkCnt) == true)
		{
			fBatt2.bits.CellChgTempHi = false;
		}
	}
	else
	{
		if(sb16UOverLevelChk(strCellTempNew.wMax,bCellChgTempHi,wFilter1,&wChkCnt) == true)
		{
			if(bBatteryStatus == cBATTERY_CHARGING)
			{
				fBatt2.bits.CellChgTempHi = true;
			}
		}
	}
}

void    sCellChgTempHi2Chk(INT16U wFilter1, INT16U wFilter2)
{
	static	INT16U	 wChkCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt2.bits.CellChgTempHi2 = false;
	}
	else if(bBatteryStatus == cBATTERY_DISCHARGING)
	{
		wChkCnt = 0;
		fBatt2.bits.CellChgTempHi2 = false;
	}
    else if(fBatt2.bits.CellChgTempHi2 == true)
	{
		if(sb16UUnderLevelChk(strCellTempNew.wMax,bCellChgTempHiAlmBack,wFilter2,&wChkCnt) == true)
		{
			fBatt2.bits.CellChgTempHi2 = false;
		}
	}
	else
	{
		if(sb16UOverLevelChk(strCellTempNew.wMax,bCellChgTempHi2,wFilter1,&wChkCnt) == true)
		{
			if(bBatteryStatus == cBATTERY_CHARGING)
			{
				fBatt2.bits.CellChgTempHi2 = true;
			}
		}
	}
}

void    sCellChgTempLoAlmChk(INT16U wFilter1, INT16U wFilter2)
{
	static  INT16U   wChkCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt2.bits.CellChgTempLoAlm = false;
	}
	else if(bBatteryStatus == cBATTERY_DISCHARGING)
	{
		wChkCnt = 0;
		fBatt2.bits.CellChgTempLoAlm = false;
	}
    else if(fBatt2.bits.CellChgTempLoAlm == true)
	{
		if(sb16UOverLevelChk(strCellTempNew.wMin,bCellChgTempLoAlmBack,wFilter2,&wChkCnt) == true)
		{
			fBatt2.bits.CellChgTempLoAlm = false;
		}
	}
	else
	{
		if(sb16UUnderLevelChk(strCellTempNew.wMin,bCellChgTempLoAlm,wFilter1,&wChkCnt) == true)
		{
			if(bBatteryStatus == cBATTERY_CHARGING)
			{
				fBatt2.bits.CellChgTempLoAlm = true;
			}
		}
	}
}

void    sCellChgTempLoChk(INT16U wFilter1, INT16U wFilter2)
{
	static	INT16U	 wChkCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt2.bits.CellChgTempLo = false;
	}
	else if(bBatteryStatus == cBATTERY_DISCHARGING)
	{
		wChkCnt = 0;
		fBatt2.bits.CellChgTempLo = false;
	}
    else if(fBatt2.bits.CellChgTempLo == true)
	{
		if(sb16UOverLevelChk(strCellTempNew.wMin,bCellChgTempLoAlmBack,wFilter2,&wChkCnt) == true)
		{
			fBatt2.bits.CellChgTempLo = false;
		}
	}
	else
	{
		if(sb16UUnderLevelChk(strCellTempNew.wMin,bCellChgTempLo,wFilter1,&wChkCnt) == true)
		{
			if(bBatteryStatus == cBATTERY_CHARGING)
			{
				fBatt2.bits.CellChgTempLo = true;
			}
		}
	}
}

void    sCellChgTempLo2Chk(INT16U wFilter1, INT16U wFilter2)
{
	static	INT16U	 wChkCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt2.bits.CellChgTempLo2 = false;
	}
	else if(bBatteryStatus == cBATTERY_DISCHARGING)
	{
		wChkCnt = 0;
		fBatt2.bits.CellChgTempLo2 = false;
	}
    else if(fBatt2.bits.CellChgTempLo2 == true)
	{
		if(sb16UOverLevelChk(strCellTempNew.wMin,bCellChgTempLoAlmBack,wFilter2,&wChkCnt) == true)
		{
			fBatt2.bits.CellChgTempLo2 = false;
		}
	}
	else
	{
		if(sb16UUnderLevelChk(strCellTempNew.wMin,bCellChgTempLo2,wFilter1,&wChkCnt) == true)
		{
			if(bBatteryStatus == cBATTERY_CHARGING)
			{
				fBatt2.bits.CellChgTempLo2 = true;
			}
		}
	}
}

void    sCellDcgTempHiAlmChk(INT16U wFilter1, INT16U wFilter2)
{
	static  INT16U   wChkCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt2.bits.CellDcgTempHiAlm = false;
	}
    else if(fBatt2.bits.CellDcgTempHiAlm == true)
	{
		if(sb16UUnderLevelChk(strCellTempNew.wMax,bCellDcgTempHiAlmBack,wFilter2,&wChkCnt) == true)
		{
			fBatt2.bits.CellDcgTempHiAlm = false;
		}
	}
	else
	{
		if(sb16UOverLevelChk(strCellTempNew.wMax,bCellDcgTempHiAlm,wFilter1,&wChkCnt) == true)
		{
			fBatt2.bits.CellDcgTempHiAlm = true;
		}
	}
}

void    sCellDcgTempHiChk(INT16U wFilter1, INT16U wFilter2)
{
	static	INT16U	 wChkCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt2.bits.CellDcgTempHi = false;
	}
    else if(fBatt2.bits.CellDcgTempHi == true)
	{
		if(sb16UUnderLevelChk(strCellTempNew.wMax,bCellDcgTempHiAlmBack,wFilter2,&wChkCnt) == true)
		{
			fBatt2.bits.CellDcgTempHi = false;
		}
	}
	else
	{
		if(sb16UOverLevelChk(strCellTempNew.wMax,bCellDcgTempHi,wFilter1,&wChkCnt) == true)
		{
			fBatt2.bits.CellDcgTempHi = true;
		}
	}
}

void    sCellDcgTempHi2Chk(INT16U wFilter1, INT16U wFilter2)
{
	static	INT16U	 wChkCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt2.bits.CellDcgTempHi2 = false;
	}
    else if(fBatt2.bits.CellDcgTempHi2 == true)
	{
		if(sb16UUnderLevelChk(strCellTempNew.wMax,bCellDcgTempHiAlmBack,wFilter2,&wChkCnt) == true)
		{
			fBatt2.bits.CellDcgTempHi2 = false;
		}
	}
	else
	{
		if(sb16UOverLevelChk(strCellTempNew.wMax,bCellDcgTempHi2,wFilter1,&wChkCnt) == true)
		{
			fBatt2.bits.CellDcgTempHi2 = true;
		}
	}
}

void    sCellDcgTempLoAlmChk(INT16U wFilter1, INT16U wFilter2)
{
	static  INT16U   wChkCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt2.bits.CellDcgTempLoAlm = false;
	}
    else if(fBatt2.bits.CellDcgTempLoAlm == true)
	{
		if(sb16UOverLevelChk(strCellTempNew.wMin,bCellDcgTempLoAlmBack,wFilter2,&wChkCnt) == true)
		{
			fBatt2.bits.CellDcgTempLoAlm = false;
		}
	}
	else
	{
		if(sb16UUnderLevelChk(strCellTempNew.wMin,bCellDcgTempLoAlm,wFilter1,&wChkCnt) == true)
		{
			fBatt2.bits.CellDcgTempLoAlm = true;
		}
	}
}

void    sCellDcgTempLoChk(INT16U wFilter1, INT16U wFilter2)
{
	static	INT16U	 wChkCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt2.bits.CellDcgTempLo = false;
	}
    else if(fBatt2.bits.CellDcgTempLo == true)
	{
		if(sb16UOverLevelChk(strCellTempNew.wMin,bCellDcgTempLoAlmBack,wFilter2,&wChkCnt) == true)
		{
			fBatt2.bits.CellDcgTempLo = false;
		}
	}
	else
	{
		if(sb16UUnderLevelChk(strCellTempNew.wMin,bCellDcgTempLo,wFilter1,&wChkCnt) == true)
		{
			fBatt2.bits.CellDcgTempLo = true;
		}
	}
}

void    sCellDcgTempLo2Chk(INT16U wFilter1, INT16U wFilter2)
{
	static	INT16U	 wChkCnt = 0;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt2.bits.CellDcgTempLo2 = false;
	}
    else if(fBatt2.bits.CellDcgTempLo2 == true)
	{
		if(sb16UOverLevelChk(strCellTempNew.wMin,bCellDcgTempLoAlmBack,wFilter2,&wChkCnt) == true)
		{
			fBatt2.bits.CellDcgTempLo2 = false;
		}
	}
	else
	{
		if(sb16UUnderLevelChk(strCellTempNew.wMin,bCellDcgTempLo2,wFilter1,&wChkCnt) == true)
		{
			fBatt2.bits.CellDcgTempLo2 = true;
		}
	}
}

void    sCellVoltErrHiAlmChk(INT16U wFilter1, INT16U wFilter2)
{
	static  INT16U   wChkCnt = 0;
	INT16S	wTemp;
	
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		wChkCnt = 0;
		fBatt1.bits.CellVoltErrHiAlm = false;
	}
	else
	{
		wTemp = strCellVoltNew.wMax - strCellVoltNew.wMin;
		if(fBatt1.bits.CellVoltErrHiAlm == true)
		{
			if(sb16UUnderLevelChk(wTemp,wCellVoltErrHiAlmBack,wFilter2,&wChkCnt) == true)
			{
				fBatt1.bits.CellVoltErrHiAlm = false;
			}
		}
		else
		{
			if(sb16UOverLevelChk(wTemp,wCellVoltErrHiAlm,wFilter1,&wChkCnt) == true)
			{
				fBatt1.bits.CellVoltErrHiAlm = true;
			}
		}
	}
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
INT16S	swGetBattVoltNew(void)
{
	return(wBattVoltNew);
}

INT16S	swGetBattCurrNew(void)
{
	return(wBattCurrNew);
}

INT16S	swGetCellVoltNew(INT8U bNo)
{
	return(wCellVoltNew[bNo]);
}

INT16S	swGetCellTempNew(INT8U bNo)
{
	return(wCellTempNew[bNo]);
}

INT16S	swGetBattVoltFiltNew(void)
{
	return(wBattVoltFiltNew);
}

INT16S	swGetBattCurrFiltNew(void)
{
	return(wBattCurrFiltNew);
}

INT16S	swGetCellVoltFiltNew(INT8U bNo)
{
	return(wCellVoltFiltNew[bNo]);
}

INT16S	swGetCellTempFiltNew(INT8U bNo)
{
	return(wCellTempFiltNew[bNo]);
}

INT16S	swGetAvgCellVoltNew(void)
{
	return strCellVoltNew.wAvg;
}

INT16S	swGetMaxCellVoltNew(void)
{
	return strCellVoltNew.wMax;
}

INT8U	sbGetMaxCellVoltNo(void)
{
	return strCellVoltNew.bMaxNo;
}

INT16S	swGetMinCellVoltNew(void)
{
	return strCellVoltNew.wMin;
}

INT8U	sbGetMinCellVoltNo(void)
{
	return strCellVoltNew.bMinNo;
}

INT16S	swGetAvgCellTempNew(void)
{
	return strCellTempNew.wAvg;
}

INT16S	swGetMaxCellTempNew(void)
{
	return strCellTempNew.wMax;
}

INT8U	sbGetMaxCellTempNo(void)
{
	return strCellTempNew.bMaxNo;
}

INT16S	swGetMinCellTempNew(void)
{
	return strCellTempNew.wMin;
}

INT8U	sbGetMinCellTempNo(void)
{
	return strCellTempNew.bMinNo;
}

INT16S	swGetAvgCellVoltFiltNew(void)
{
	return strCellVoltFiltNew.wAvg;
}

INT16S	swGetMaxCellVoltFiltNew(void)
{
	return strCellVoltFiltNew.wMax;
}

INT8U	sbGetMaxCellVoltFiltNo(void)
{
	return strCellVoltNew.bMaxNo;
}

INT16S	swGetMinCellVoltFiltNew(void)
{
	return strCellVoltNew.wMin;
}

INT8U	sbGetMinCellVoltFiltNo(void)
{
	return strCellVoltNew.bMinNo;
}

INT16S	swGetAvgCellTempFiltNew(void)
{
	return strCellTempNew.wAvg;
}

INT16S	swGetMaxCellTempFiltNew(void)
{
	return strCellTempNew.wMax;
}

INT8U	sbGetMaxCellTempFiltNo(void)
{
	return strCellTempNew.bMaxNo;
}

INT16S	swGetMinCellTempFiltNew(void)
{
	return strCellTempNew.wMin;
}

INT8U	sbGetMinCellTempFiltNo(void)
{
	return strCellTempNew.bMinNo;
}

INT8U	sbGetBattVoltHi(void)
{
	return fBatt1.bits.BattVoltHi;
}

INT8U	sbGetBattVoltLo(void)
{
	return fBatt1.bits.BattVoltLo;
}

INT8U	sbGetCellVoltHiAlm(void)
{
	return fBatt1.bits.CellVoltHiAlm;
}

INT8U	sbGetCellVoltHi(void)
{
	return fBatt1.bits.CellVoltHi;
}

INT8U	sbGetCellVoltHi2(void)
{
	return fBatt1.bits.CellVoltHi2;
}

INT8U	sbGetCellVoltLoAlm(void)
{
	return fBatt1.bits.CellVoltLoAlm;
}

INT8U	sbGetCellVoltLo(void)
{
	return fBatt1.bits.CellVoltLo;
}

INT8U	sbGetCellVoltLo2(void)
{
	return fBatt1.bits.CellVoltLo2;
}

INT8U	sbGetBattChgCurrHiAlm(void)
{
	return fBatt1.bits.BattChgCurrHiAlm;
}

INT8U	sbGetBattChgCurrHi(void)
{
	return fBatt1.bits.BattChgCurrHi;
}

INT8U	sbGetBattChgCurrHi2(void)
{
	return fBatt1.bits.BattChgCurrHi2;
}

INT8U	sbGetBattDcgCurrHiAlm(void)
{
	return fBatt1.bits.BattDcgCurrHiAlm;
}

INT8U	sbGetBattDcgCurrHi(void)
{
	return fBatt1.bits.BattDcgCurrHi;
}

INT8U	sbGetBattDcgCurrHi2(void)
{
	return fBatt1.bits.BattDcgCurrHi2;
}

INT8U	sbGetCellChgTempHiAlm(void)
{
	return fBatt2.bits.CellChgTempHiAlm;
}

INT8U	sbGetCellChgTempHi(void)
{
	return fBatt2.bits.CellChgTempHi;
}

INT8U	sbGetCellChgTempHi2(void)
{
	return fBatt2.bits.CellChgTempHi2;
}

INT8U	sbGetCellChgTempLoAlm(void)
{
	return fBatt2.bits.CellChgTempLoAlm;
}

INT8U	sbGetCellChgTempLo(void)
{
	return fBatt2.bits.CellChgTempLo;
}

INT8U	sbGetCellChgTempLo2(void)
{
	return fBatt2.bits.CellChgTempLo2;
}

INT8U	sbGetCellDcgTempHiAlm(void)
{
	return fBatt2.bits.CellDcgTempHiAlm;
}

INT8U	sbGetCellDcgTempHi(void)
{
	return fBatt2.bits.CellDcgTempHi;
}

INT8U	sbGetCellDcgTempHi2(void)
{
	return fBatt2.bits.CellDcgTempHi2;
}

INT8U	sbGetCellDcgTempLoAlm(void)
{
	return fBatt2.bits.CellDcgTempLoAlm;
}

INT8U	sbGetCellDcgTempLo(void)
{
	return fBatt2.bits.CellDcgTempLo;
}

INT8U	sbGetCellDcgTempLo2(void)
{
	return fBatt2.bits.CellDcgTempLo2;
}

INT8U	sbGetCellVoltErrHiAlm(void)
{
	return fBatt1.bits.CellVoltErrHiAlm;
}

INT8U	sbGetCellTempHiAlm(void)
{
	if(fBatt2.bits.CellChgTempHiAlm || fBatt2.bits.CellDcgTempHiAlm)
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT8U	sbGetCellTempLoAlm(void)
{
	if(fBatt2.bits.CellChgTempLoAlm || fBatt2.bits.CellDcgTempLoAlm)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
void    sSetBattVoltNew(INT16S wVolt)
{
	wBattVoltNew = wVolt;
}

void    sSetBattCurrNew(INT16S wCurr)
{
	wBattCurrNew = wCurr;
}

void    sSetCellVoltNew(INT8U bNo, INT16S wVolt)
{
	wCellVoltNew[bNo] = wVolt;
}

void    sSetCellTempNew(INT8U bNo, INT16S wTemp)
{
	wCellTempNew[bNo] = wTemp;
}

void    sSetBattVoltFiltNew(INT16S wVolt)
{
	wBattVoltFiltNew = wVolt;
}

void    sSetBattCurrFiltNew(INT16S wCurr)
{
	wBattCurrFiltNew = wCurr;
}

void    sSetCellVoltFiltNew(INT8U bNo, INT16S wVolt)
{
	wCellVoltFiltNew[bNo] = wVolt;
}

void    sSetCellTempFiltNew(INT8U bNo, INT16S wTemp)
{
	wCellTempFiltNew[bNo] = wTemp;
}

void    sSetBatteryStatus(INT8U bStatus)
{
	bBatteryStatus = bStatus;
}

void    sSetBattVoltLo2Disable(INT8U bStatus)
{
	bBattVoltLo2Disable = bStatus;
}

void    sSetCellVoltLo2Disable(INT8U bStatus)
{
	bCellVoltLo2Disable = bStatus;
}

void    sSetCellVoltHi(INT8U bStatus)
{
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		fBatt1.bits.CellVoltHi = false;
	}
	else
	{
		fBatt1.bits.CellVoltHi = bStatus;
	}
}

void    sSetCellVoltHi2(INT8U bStatus)
{
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		fBatt1.bits.CellVoltHi2 = false;
	}
	else
	{
		fBatt1.bits.CellVoltHi2 = bStatus;
	}
}

void    sSetCellVoltLo(INT8U bStatus)
{
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		fBatt1.bits.CellVoltLo = false;
	}
	else
	{
		fBatt1.bits.CellVoltLo = bStatus;
	}
}

void    sSetCellVoltLo2(INT8U bStatus)
{
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		fBatt1.bits.CellVoltLo2 = false;
	}
	else
	{
		fBatt1.bits.CellVoltLo2 = bStatus;
	}
}

void    sSetBattChgCurrHi(INT8U bStatus)
{
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		fBatt1.bits.BattChgCurrHi = false;
	}
	else
	{
		fBatt1.bits.BattChgCurrHi = bStatus;
	}
}

void    sSetBattChgCurrHi2(INT8U bStatus)
{
	fBatt1.bits.BattChgCurrHi2 = bStatus;
}

void    sSetBattDcgCurrHi(INT8U bStatus)
{
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		fBatt1.bits.BattDcgCurrHi = false;
	}
	else
	{
		fBatt1.bits.BattDcgCurrHi = bStatus;
	}
}

void    sSetBattDcgCurrHi2(INT8U bStatus)
{
	fBatt1.bits.BattDcgCurrHi2 = bStatus;
}

void    sSetCellChgTempHi(INT8U bStatus)
{
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		fBatt2.bits.CellChgTempHi = false;
	}
	else
	{
		fBatt2.bits.CellChgTempHi = bStatus;
	}
}

void    sSetCellChgTempHi2(INT8U bStatus)
{
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		fBatt2.bits.CellChgTempHi2 = false;
	}
	else
	{
		fBatt2.bits.CellChgTempHi2 = bStatus;
	}
}

void    sSetCellChgTempLo(INT8U bStatus)
{
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		fBatt2.bits.CellChgTempLo = false;
	}
	else
	{
		fBatt2.bits.CellChgTempLo = bStatus;
	}
}

void    sSetCellChgTempLo2(INT8U bStatus)
{
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		fBatt2.bits.CellChgTempLo2 = false;
	}
	else
	{
		fBatt2.bits.CellChgTempLo2 = bStatus;
	}
}

void    sSetCellDcgTempHi(INT8U bStatus)
{
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		fBatt2.bits.CellDcgTempHi = false;
	}
	else
	{
		fBatt2.bits.CellDcgTempHi = bStatus;
	}
}

void    sSetCellDcgTempHi2(INT8U bStatus)
{
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		fBatt2.bits.CellDcgTempHi2 = false;
	}
	else
	{
		fBatt2.bits.CellDcgTempHi2 = bStatus;
	}
}

void    sSetCellDcgTempLo(INT8U bStatus)
{
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		fBatt2.bits.CellDcgTempLo = false;
	}
	else
	{
		fBatt2.bits.CellDcgTempLo = bStatus;
	}
}

void    sSetCellDcgTempLo2(INT8U bStatus)
{
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cTestMode) \
		|| (bBmsMode == cUpgradeMode))
	{
		fBatt2.bits.CellDcgTempLo2 = false;
	}
	else
	{
		fBatt2.bits.CellDcgTempLo2 = bStatus;
	}
}

