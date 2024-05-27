/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		IdleJob.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define		__IDLE_JOB_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define		cTimerBase				Timer_100ms
#define		cT100ms					(100 / cTimerBase)
#define		cT200ms					(200 / cTimerBase)
#define		cT400ms					(400 / cTimerBase)
#define		cT500ms					(500 / cTimerBase)
#define		cT1sec					(1000 / cTimerBase)
#define		cT2sec					(2000 / cTimerBase)

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


/********************************************************************************
* Output variables																*
********************************************************************************/


/********************************************************************************
* Internal variables															*
********************************************************************************/
INT16U wLedTestNormalCnt;
INT16U wLedTestFaultCnt;
INT16U wLedTestPowerOnCnt;
INT16U wLedTestOtherCnt;

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
void	sLedTest(void);

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void	sIdleJob(void)
{
	INT16U	wEvent;
	
	sIdleJobInit();
	
	while(1)
	{
		wEvent = OSEventPend();
		
		if(wEvent & (1 << eIdleTimer))
		{
			sLedTest();
			
			// 需要测试上电到执行这段代码耗时,确保时间远小于2秒
			mIWDG_RELOAD_COUNTER();
			
		}
	}
}

void	sIdleJobInit(void)
{
	OSMaskEventPend(1 << eIdleTimer);
}

void	sLedTest(void)
{
	INT16U wCount, wFilter;
	
	if(++wLedTestFaultCnt >= cT1sec)
	{
		wLedTestFaultCnt = 0;
	}
	if(++wLedTestNormalCnt >= cT2sec)
	{
		wLedTestNormalCnt = 0;
	}
	if(++wLedTestPowerOnCnt >= cT200ms)
	{
		wLedTestPowerOnCnt = 0;
	}
	if(++wLedTestOtherCnt >= cT400ms)
	{
		wLedTestOtherCnt = 0;
	}
	
	if(bBmsMode == cFaultMode)
	{
		wCount = wLedTestFaultCnt;
		wFilter = cT500ms;
	}
	else if((bBmsMode == cStandbyMode) \
		|| (bBmsMode == cBatteryMode) \
		|| (bBmsMode == cDischargeMode) \
		|| (bBmsMode == cChargeMode))
	{
		wCount = wLedTestNormalCnt;
		wFilter = cT1sec;
	}
	else if(bBmsMode == cPowerOnMode)
	{
		wCount = wLedTestPowerOnCnt;
		wFilter = cT100ms;
	}
	else
	{
		wCount = wLedTestOtherCnt;
		wFilter = cT200ms;
	}
	
	// Driver
	if(wCount < wFilter)
	{
		mLED_TEST_ON();
	}
	else
	{
		mLED_TEST_OFF();
	}
}

void	sDisplayTestLedFault(INT8U bFlag)
{
	if(bFlag)
	{
		if(wLedTestFaultCnt >= cT500ms)
		{
			wLedTestFaultCnt++;
		}
		else if(wLedTestFaultCnt < cT500ms)
		{
			if(wLedTestFaultCnt > 0)
			{
				wLedTestFaultCnt--;
			}
		}
	}
}

void	sDisplayTestLedNormal(INT8U bFlag)
{
	if(bFlag)
	{
		if(wLedTestNormalCnt >= cT1sec)
		{
			wLedTestNormalCnt++;
		}
		else if(wLedTestNormalCnt < cT1sec)
		{
			if(wLedTestNormalCnt > 0)
			{
				wLedTestNormalCnt--;
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


