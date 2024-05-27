/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		GeneralJob.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define		__GENERAL_JOB_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define         cTimerBase                      Timer_100ms

#define         cT100msec                       (100 / cTimerBase)
#define         cT200msec                       (200 / cTimerBase)
#define         cT500msec                       (500 / cTimerBase)
#define         cT1sec                          (1000 / cTimerBase)
#define         cT2sec                          (2000 / cTimerBase)
#define         cT3sec                          (3000 / cTimerBase)
#define         cT4sec                          (4000 / cTimerBase)
#define         cT5sec                          (5000 / cTimerBase)
#define         cT10sec                         (10000 / cTimerBase)
#define         cT15sec                         (15000 / cTimerBase)
#define         cT30sec                         (30000 / cTimerBase)
#define         cT1min                          (60000 / cTimerBase)
#define         cT2min                          (120000 / cTimerBase)
#define         cT2min5                         (150000 / cTimerBase)
#define         cT3min                          (180000 / cTimerBase)
#define         cT5min                          (300000 / cTimerBase)
#define         cT10min                         (600000 / cTimerBase)

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
union GENERAL_STATUS_TABLE
{
	struct GENERAL_STATUS_BIT_FEILD
	{
		// BIT0-3
		INT16U bRsvd1:4;
		
		// BIT4-7
		INT16U bRsvd2:4;
		
		// BIT8-11
		INT16U bRsvd3:4;
		
		// BIT12-15
		INT16U bRsvd4:4;
	} bits;
	INT16U data;
} fGeneral;

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
void	sGeneralJobInit(void);

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void	sGeneralJob(void)
{
	INT16U	wEvent;
	INT8U	i;
	
	while(sbGetInitializeFinished() == false)
	{
		OSMaskEventPend(1 << eGeneralTimer);
	}
	
	sGeneralJobInit();
	
	while(1)
	{
		wEvent = OSEventPend();
		
		if(wEvent & (1 << eAdcAvgCalc))
		{
			sAdcAvgCalc();
		}
		
		if(wEvent & (1 << eGeneralTimer))
		{
			//mLED_TEST_ON();	// AFE任务15ms,其他任务<1ms
			
			// ADC
			sAdcAvgFilter(cT1sec);
			
			// Battery Module
			if(sbGetBmsCurrentStatus() == cBMS_CURR_DSG)
			{
				sSetBatteryStatus(cBATTERY_DISCHARGING);
			}
			else if(sbGetBmsCurrentStatus() == cBMS_CURR_CHG)
			{
				sSetBatteryStatus(cBATTERY_CHARGING);
			}
			else
			{
				sSetBatteryStatus(cBATTERY_IDLE);
			}
			sSetBattVoltLo2Disable(sbGetKeyStatusForcedOn());
			sSetCellVoltLo2Disable(sbGetKeyStatusForcedOn());
			sSetBattVoltNew(swGetBattVoltReal());
			sSetBattCurrNew(swGetBattCurrReal());
			sSetBattVoltFiltNew(swGetBattVoltRealFilt());
			sSetBattCurrFiltNew(swGetBattCurrRealFilt());
			for(i = 0; i < swGetBmsCellVoltNum(); i++)
			{
				sSetCellVoltNew(i, swGetCellVoltReal(i));
				sSetCellVoltFiltNew(i, swGetCellVoltRealFilt(i));
			}
			for(i = 0; i < swGetBmsCellTempNum(); i++)
			{
				sSetCellTempNew(i, swGetCellTempReal(i));
				sSetCellTempFiltNew(i, swGetCellTempRealFilt(i));
			}
			sScanCellVoltAvgMaxMin(swGetBmsCellVoltNum());
			sScanCellTempAvgMaxMin(swGetBmsCellTempNum());
			sBattVoltHiChk(cT5sec, cT15sec);
			sBattVoltLoChk(cT5sec, cT15sec);
			
			#if (UPDATE_ALARM_STATUS_AFE == 0)
			if(swGetBmsCurrHigh2() <= cBMS_CURR_120A)
			{
				sBattChgCurrHiAlmChk(cT15sec, cT5sec);
				sBattChgCurrHiChk(cT5min, cT15sec);
				sBattChgCurrHi2Chk(cT30sec, cT15sec);
				sBattDcgCurrHiAlmChk(cT15sec, cT5sec);
				sBattDcgCurrHiChk(cT5min, cT15sec);
				sBattDcgCurrHi2Chk(cT30sec, cT15sec);
			}
			else
			{
				sBattChgCurrHiAlmChk(cT10sec, cT5sec);
				sBattChgCurrHiChk(cT2min, cT15sec);
				sBattChgCurrHi2Chk(cT15sec, cT15sec);
				sBattDcgCurrHiAlmChk(cT10sec, cT5sec);
				sBattDcgCurrHiChk(cT2min, cT15sec);
				sBattDcgCurrHi2Chk(cT15sec, cT15sec);
			}
			// 待机时检测时间缩短,和待机状态延时时间匹配
			if(bBmsMode == cStandbyMode)
			{
				sCellVoltErrHiAlmChk(cT1sec, cT5sec);
				sCellVoltHiAlmChk(cT1sec, cT5sec);
				sCellVoltHiChk(cT1sec, cT5sec);
				sCellVoltHi2Chk(cT1sec, cT15sec);
				sCellVoltLoAlmChk(cT1sec, cT5sec);
				sCellVoltLoChk(cT1sec, cT5sec);
				sCellVoltLo2Chk(cT1sec, cT15sec);
			}
			else
			{
				sCellVoltErrHiAlmChk(cT1sec, cT5sec);
				sCellVoltHiAlmChk(cT1sec, cT5sec);
				sCellVoltHiChk(cT2sec, cT5sec);
				sCellVoltHi2Chk(cT5sec, cT15sec);
				sCellVoltLoAlmChk(cT1sec, cT5sec);
				sCellVoltLoChk(cT2sec, cT5sec);
				sCellVoltLo2Chk(cT5sec, cT15sec);
			}
			sCellChgTempHiAlmChk(cT1sec, cT5sec);
			sCellChgTempHiChk(cT2sec, cT5sec);
			sCellChgTempHi2Chk(cT5sec, cT5sec);
			sCellChgTempLoAlmChk(cT1sec, cT5sec);
			sCellChgTempLoChk(cT2sec, cT5sec);
			sCellChgTempLo2Chk(cT5sec, cT5sec);
			sCellDcgTempHiAlmChk(cT1sec, cT5sec);
			sCellDcgTempHiChk(cT2sec, cT5sec);
			sCellDcgTempHi2Chk(cT5sec, cT5sec);
			sCellDcgTempLoAlmChk(cT1sec, cT5sec);
			sCellDcgTempLoChk(cT2sec, cT5sec);
			sCellDcgTempLo2Chk(cT5sec, cT5sec);
			#endif
			
			// Protection Module
			sSetHsTempNew(swGetHsTempReal());
			sHsTempHiAlmChk(cT1sec, cT5sec);
			sHsTempHiChk(cT5sec, cT15sec);
			
			// BMS
			sBmsFaultUpdate();
			sBmsAlarmUpdate();
			sBmsNoticeUpdate();
			sBmsDataStatistic();
			sBmsTimeStore(cT5sec);
			#if (UPDATE_CURR_STATUS_AFE == 0)
			sBmsCurrentStatusChk(swGetBattCurrNew(), cT1sec, cT5sec);
			#endif
			sBmsCharge5AChk(swGetBattCurrNew(), cT1sec, cT5sec);
			sBmsIdleToOffChk(cT1min, swGetEepromIdleExitTime());
			sBmsChgToOffChk(cT1min, swGetEepromSleepExitTime());
			sBmsSoftStartChk(cT10sec, cT30sec);
			sBmsStatusUpdate();
			sBmsDataUpdate();
			
			// EMS
			sEmsCurrentStatusChk(cT1sec, cT5sec);
			sEmsDataUpdate();
			
			// SoX
			sSoxOcvChk(cT1sec, cT5sec);
			sSoxAhCalc(swGetBattCurrNew());
			sSocEstimate(swGetAvgCellTempNew());
			sSohEstimate();
			sSocFilter(cT500msec);
			
			// Balance
			sSetBalanceCellNum(swGetBmsCellVoltNum());
			#if (UPDATE_BAL_CTRL_MODE == 1)
			sBalanceStatusChk(cT1sec);
			sBalancePauseAutoRst(cT5min);
			sBalanceTestStatusAutoRst(cT5min);
			#endif
			
			// AFE
			sAfeRamProc(cT100msec);	// 执行时间15ms
			
			// 
			#if(VOLTRONIC_PROTOCOL_ENABLE==1)
				sVoltronicDataUpdate();
			#endif
			#if(LUX_PROTOCOL_ENABLE==1)
				sLuxDataUpdate();
			#endif
			#if(MEGAREVO_PROTOCOL_ENABLE==1)
				sMegarevoDataUpdate();
			#endif
			
			//mLED_TEST_OFF();
		}
	}
}

void	sGeneralJobInit(void)
{
	fGeneral.data = 0;
	
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/


/********************************************************************************
* Input interface Routines														*
********************************************************************************/


