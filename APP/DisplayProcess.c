/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		DisplayProcess.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define			__DISPLAY_PROCESS_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"DisplayProcess.h"
#include		"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define         cDISPLAY_T100MS			100
#define         cDISPLAY_T200MS			200
#define         cDISPLAY_T1S	    	1000
#define         cDISPLAY_T2S			2000
#define         cDISPLAY_T3S			3000
#define         cDISPLAY_T4S			4000
#define         cDISPLAY_T5S			5000

#define         cDISPLAY_SOC_EMPTY		0
#define         cDISPLAY_SOC_10PCT		100
#define         cDISPLAY_SOC_25PCT		250
#define         cDISPLAY_SOC_50PCT		500
#define         cDISPLAY_SOC_75PCT		750
#define         cDISPLAY_SOC_FULL		1000

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
INT8U	bDisplayBmsMode;
INT8U	bDisplayBmsStatus;
INT16U	wDisplayBmsSoc;		// 0.1%

/********************************************************************************
* Output variables																*
********************************************************************************/


/********************************************************************************
* Internal variables															*
********************************************************************************/
union DISPLAY_STATUS_TABLE
{
	struct DISPLAY_STATUS_BIT_FEILD
	{
		// BIT0-3
		INT16U rsvd1:4;
		
		// BIT4-7
		INT16U rsvd2:4;
		
		// BIT8-11
		INT16U rsvd3:4;
		
		// BIT12-15
		INT16U rsvd4:4;
	} bits;
	INT16U data;
} fDisplayStatus;
INT16U	wDisplaySocCnt;
INT16U	wDisplaySocCntBias;
INT16U	wDisplaySocRedCnt;
INT16U	wDisplayKeyCnt;
INT8U	bDisplaySocTimeBase;
INT8U	bDisplayKeyTimeBase;

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/


/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void	sDisplayProcessInit(void)
{
	bDisplayBmsMode = 0;
	bDisplayBmsStatus = 0;
	wDisplayBmsSoc = cDISPLAY_SOC_EMPTY;
	
	fDisplayStatus.data = 0;
	wDisplaySocCnt = 0;
	wDisplaySocCntBias = 0;
	wDisplaySocRedCnt = 0;
	wDisplayKeyCnt = 0;
	
	bDisplaySocTimeBase = 1;
	bDisplayKeyTimeBase = 1;
}

void    sDisplaySoc(INT16U wTimeBase)
{
	wDisplaySocRedCnt += wTimeBase;
	if(wDisplaySocRedCnt >= cDISPLAY_T200MS)
	{
		wDisplaySocRedCnt = 0;
	}
	
	wDisplaySocCnt += wTimeBase;
	if(wDisplaySocCnt >= cDISPLAY_T5S)
	{
		sCanSyncDisplaySocCommand();
		
		if(wDisplayBmsSoc < cDISPLAY_SOC_25PCT)
		{
			wDisplaySocCntBias = 0;
		}
		else if(wDisplayBmsSoc < cDISPLAY_SOC_50PCT)
		{
			wDisplaySocCntBias = cDISPLAY_T1S;
		}
		else if(wDisplayBmsSoc < cDISPLAY_SOC_75PCT)
		{
			wDisplaySocCntBias = cDISPLAY_T2S;
		}
		else if(wDisplayBmsSoc < cDISPLAY_SOC_FULL)
		{
			wDisplaySocCntBias = cDISPLAY_T3S;
		}
		else
		{
			wDisplaySocCntBias = cDISPLAY_T4S;
		}
		
		bDisplaySocTimeBase = wTimeBase;
		wDisplaySocCnt = wDisplaySocCntBias;
	}
	
	if(bDisplayBmsMode == cPowerOnMode)
	{
		mDISPLAY_SOC_FULL();
		mLED_SOC_RED_ON();
	}
	else if((bDisplayBmsMode == cShutdownMode) || (bDisplayBmsMode == cUpgradeMode) \
		|| (bDisplayBmsMode == cSleepMode) || (bDisplayBmsMode == cTestMode))
	{
		mDISPLAY_SOC_EMPTY();
		mLED_SOC_RED_OFF();
	}
	else if(bDisplayBmsMode == cFaultMode)
	{
		mLED_SOC_RED_OFF();
		if(sbGetBmsFaultCode() == cBattVoltHiId)
		{
			mDISPLAY_BATT_VOLT_HI();
		}
		else if(sbGetBmsFaultCode() == cBattVoltLoId)
		{
			mDISPLAY_BATT_VOLT_LO();
		}
		else if(sbGetBmsFaultCode() == cBattChgCurrHiId)
		{
			mDISPLAY_CHG_CURR_HI();
		}
		else if(sbGetBmsFaultCode() == cBattDcgCurrHiId)
		{
			mDISPLAY_DSG_CURR_HI();
		}
		else if(sbGetBmsFaultCode() == cCellVoltHiId)
		{
			mDISPLAY_CELL_VOLT_HI();
		}
		else if(sbGetBmsFaultCode() == cCellVoltLoId)
		{
			mDISPLAY_CELL_VOLT_LO();
		}
		else if(sbGetBmsFaultCode() == cCellTempHiId)
		{
			mDISPLAY_CELL_TEMP_HI();
		}
		else if(sbGetBmsFaultCode() == cCellTempLoId)
		{
			mDISPLAY_CELL_TEMP_LO();
		}
		else if(sbGetBmsFaultCode() == cHsTempHiId)
		{
			mDISPLAY_HS_TEMP_HI();
		}
		else if(sbGetBmsFaultCode() == cHsTempLoId)
		{
			mDISPLAY_HS_TEMP_LO();
		}
		else if(sbGetBmsFaultCode() == cSampSensorId)
		{
			mDISPLAY_CURR_SENSOR();
		}
		else if(sbGetBmsFaultCode() == cOutputResLoId)
		{
			mDISPLAY_OP_RES_LO();
		}
		else if(sbGetBmsFaultCode() == cParallelFaultId)
		{
			mDISPLAY_PARALLEL_FAIL();
		}
		else
		{
			mDISPLAY_ALARM_NULL();
		}
	}
	else if(bDisplayBmsStatus == cDISPLAY_STATUS_CHG)
	{
		mLED_SOC_RED_OFF();
		if(wDisplaySocCnt < cDISPLAY_T1S)
		{
			mDISPLAY_SOC_EMPTY();
		}
		else if(wDisplaySocCnt < cDISPLAY_T2S)
		{
			mDISPLAY_SOC_25PCT();
		}
		else if(wDisplaySocCnt < cDISPLAY_T3S)
		{
			mDISPLAY_SOC_50PCT();
		}
		else if(wDisplaySocCnt < cDISPLAY_T4S)
		{
			mDISPLAY_SOC_75PCT();
		}
		else
		{
			mDISPLAY_SOC_FULL();
		}
	}
	else if(wDisplayBmsSoc < cDISPLAY_SOC_10PCT)
	{
		#if(LED_SOC_HW_VERSION==0)
			mDISPLAY_SOC_EMPTY();
			if(wDisplaySocRedCnt < cDISPLAY_T100MS)
			{
				mLED_SOC_RED_OFF();
			}
			else
			{
				mLED_SOC_RED_ON();
			}
		#else
			mLED_SOC_RED_OFF();
			mLED_SOC2_OFF();
			mLED_SOC3_OFF();
			mLED_SOC4_OFF();
			if(wDisplaySocRedCnt < cDISPLAY_T100MS)
			{
				mLED_SOC1_OFF();
			}
			else
			{
				mLED_SOC1_ON();
			}
		#endif
	}
	else
	{
		mLED_SOC_RED_OFF();
		if(wDisplayBmsSoc >= cDISPLAY_SOC_75PCT)
		{
			mDISPLAY_SOC_FULL();
		}
		else if(wDisplayBmsSoc > cDISPLAY_SOC_50PCT)
		{
			mDISPLAY_SOC_75PCT();
		}
		else if(wDisplayBmsSoc > cDISPLAY_SOC_25PCT)
		{
			mDISPLAY_SOC_50PCT();
		}
		else
		{
			mDISPLAY_SOC_25PCT();
		}
	}
}

void    sDisplayKey(INT16U wTimeBase)
{
	wDisplayKeyCnt += wTimeBase;
	if(wDisplayKeyCnt >= cDISPLAY_T2S)
	{
		sCanSyncDisplayKeyCommand();
		
		bDisplayKeyTimeBase = wTimeBase;
		wDisplayKeyCnt = 0;
	}
	
	if(bDisplayBmsMode == cPowerOnMode)
	{
		mLED_STATUS_OFF();
		mLED_ALARM_ON();
	}
	else if(bDisplayBmsMode == cStandbyMode)
	{
		mLED_STATUS_OFF();
		mLED_ALARM_OFF();
	}
	else if((bDisplayBmsMode == cBatteryMode) || (bDisplayBmsMode == cDischargeMode))
	{
		mLED_STATUS_ON();
		mLED_ALARM_OFF();
	}
	else if((bDisplayBmsMode == cChargeMode) || (bDisplayBmsMode == cSleepMode))
	{
		mLED_ALARM_OFF();
		if(wDisplayKeyCnt < cDISPLAY_T1S)
		{
			mLED_STATUS_OFF();
		}
		else
		{
			mLED_STATUS_ON();
		}
	}
	else if(bDisplayBmsMode == cFaultMode)
	{
		mLED_STATUS_OFF();
		mLED_ALARM_ON();
	}
	else
	{
		mLED_STATUS_OFF();
		mLED_ALARM_OFF();
	}
}

void    sDisplaySocSync(INT8U bFlag)
{
	INT16U wTemp;
	
	if(bFlag)
	{
		wTemp = (cDISPLAY_T5S + wDisplaySocCntBias) >> 1;
		if(wDisplaySocCnt > wTemp)
		{
			wDisplaySocCnt += bDisplaySocTimeBase;
		}
		else if(wDisplaySocCnt < wTemp)
		{
			if(wDisplaySocCnt > bDisplaySocTimeBase)
			{
				wDisplaySocCnt -= bDisplaySocTimeBase;
			}
			else
			{
				wDisplaySocCnt = 0;
			}
		}
	}
}

void    sDisplayKeySync(INT8U bFlag)
{
	if(bFlag)
	{
		if(wDisplayKeyCnt > cDISPLAY_T1S)
		{
			wDisplayKeyCnt += bDisplayKeyTimeBase;
		}
		else if(wDisplayKeyCnt < cDISPLAY_T1S)
		{
			if(wDisplayKeyCnt > bDisplayKeyTimeBase)
			{
				wDisplayKeyCnt -= bDisplayKeyTimeBase;
			}
			else
			{
				wDisplayKeyCnt = 0;
			}
		}
	}
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/


/********************************************************************************
* Input interface Routines														*
********************************************************************************/
void	sSetDisplayBmsMode(INT8U bMode)
{
	bDisplayBmsMode = bMode;
}

void	sSetDisplayBmsStatus(INT8U bStatus)
{
	bDisplayBmsStatus = bStatus;
}

void	sSetDisplayBmsSoc(INT16U wSoc)
{
	wDisplayBmsSoc = wSoc;
}

