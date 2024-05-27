/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		SuperJob.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define		__SUPER_JOB_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define         cTimerBase                      Timer_20ms
#define         cT100ms                         (100 / cTimerBase)
#define         cT200ms                         (200 / cTimerBase)
#define         cT250ms                         (250 / cTimerBase)
#define         cT300ms                         (300 / cTimerBase)
#define         cT400ms                         (400 / cTimerBase)
#define         cT500ms                         (500 / cTimerBase)
#define         cT600ms                         (600 / cTimerBase)
#define         cT1sec                          (1000 / cTimerBase)
#define         cT1sec5                         (1500 / cTimerBase)
#define         cT2sec                          (2000 / cTimerBase)
#define         cT3sec                          (3000 / cTimerBase)
#define         cT4sec                          (4000 / cTimerBase)
#define         cT5sec                          (5000 / cTimerBase)
#define         cT10sec                         (10000 / cTimerBase)
#define         cT15sec                         (15000 / cTimerBase)
#define         cT30sec                         (30000 / cTimerBase)
#define         cT1min                          (60000 / cTimerBase)
#define         cT5min                          (300000 / cTimerBase)

/********************************************************************************
* Constants																		*
********************************************************************************/


/********************************************************************************
* External variables															*
********************************************************************************/


/********************************************************************************
* External routine prototypes													*
********************************************************************************/
extern	void	IAP_BootFlagErase(void);

/********************************************************************************
* Input variables																*
********************************************************************************/
INT8U	bFaultTotalCnt;

/********************************************************************************
* Output variables																*
********************************************************************************/


/********************************************************************************
* Internal variables															*
********************************************************************************/
union SUPER_STATUS_TABLE
{
	struct SUPER_STATUS_BIT_FEILD
	{
		// BIT0-3
		INT16U bFaultRestartFlag:1;
		INT16U bFaultPowerOnFlag:1;
		INT16U Rsvd1:2;
		
		// BIT4-7
		INT16U StandbySteadyFlag:1;
		INT16U ChargeSteadyFlag:1;
		INT16U DischargeSteadyFlag:1;
		INT16U Rsvd2:1;
		
		// BIT8-11
		INT16U Rsvd3:4;
		
		// BIT12-15
		INT16U Rsvd4:4;
	} bits;
	INT16U data;
} fSuper;
INT32U	JumpAddress;
pFunction	Jump_To_Application;

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
void	sModeSwitch(void);
void	sPowerOnMode(void);
void	sStandbyMode(void);
void	sBatteryMode(void);
void	sChargeMode(void);
void	sDischargeMode(void);
void	sSleepMode(void);
void	sFaultMode(void);
void	sShutDownMode(void);
void	sTestMode(void);
void	sUpgradeMode(void);
INT8U	sbWaitSystemSteady(INT8U bFilter);
INT8U	sbSystemInitialize(void);
INT8U	sbModelIdentify(INT8U bFilter);
INT8U	sbAdcOffsetInitial(INT8U bFilter);
INT8U	sbAfeEepromInitial(INT8U bFilter);
void	sStandbySteadyChk(INT16U wFilter);
void	sDischargeSteadyChk(INT16U wFilter);
void	sChargeSteadyChk(INT16U wFilter);

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void	sSuperJob(void)
{
	sSuperJobInit();
	
	OSMaskEventPend(1 << eSuperTimer);
	OSMaskEventPend(1 << eSuperTimer);
	
	while(1)
	{
		if(bBmsMode==cPowerOnMode)
		{
			sPowerOnMode();
		}
		else if(bBmsMode==cFaultMode)
		{
			sFaultMode();
		}
		else if(bBmsMode==cStandbyMode)
		{
			sStandbyMode();
		}
		else if(bBmsMode==cBatteryMode)
		{
			sBatteryMode();
		}
		else if(bBmsMode==cChargeMode)
		{
			sChargeMode();
		}
		else if(bBmsMode==cDischargeMode)
		{
			sDischargeMode();
		}
		else if(bBmsMode==cSleepMode)
		{
			sSleepMode();
		}
		else if(bBmsMode==cShutdownMode)
		{
			sShutDownMode();
		}
		else if(bBmsMode==cTestMode)
		{
			sTestMode();
		}
		else if(bBmsMode==cUpgradeMode)
		{
			sUpgradeMode();
		}
		else
		{
			bBmsMode=cPowerOnMode;	
		}
	}
}

void	sSuperJobInit(void)
{
	bBmsMode = cPowerOnMode;
	bFaultTotalCnt = 0;
	
}

void    sModeSwitch(void)
{
	INT8U bSwitchMode;
	
	switch(bBmsMode)
	{
		case cStandbyMode:
			if(sbGetStandbySteadyFlag())
			{
				if(sbGetKeyStatusForcedOn())
				{
					bSwitchMode = cBatteryMode;
				}
				else if(sbGetCellVoltHi())
				{
					bSwitchMode = cDischargeMode;
				}
				else if(sbGetCellVoltLo())
				{
					bSwitchMode = cChargeMode;
				}
				else
				{
					bSwitchMode = cBatteryMode;
				}
			}
			else
			{
				bSwitchMode = cStandbyMode;
			}
			
			if(bSwitchMode != bBmsMode)
			{
				if(bSwitchMode == cBatteryMode)
				{
					OSEventSend(cPrioSuper, eSuperToBattery);
				}
				else if(bSwitchMode == cChargeMode)
				{
					OSEventSend(cPrioSuper, eSuperToCharge);
				}
				else if(bSwitchMode == cDischargeMode)
				{
					OSEventSend(cPrioSuper, eSuperToDischarge);
				}
			}
		break;
		case cBatteryMode:
			if((sbGetCellVoltHi() == false) && sbGetKeyStatusForcedOn())
			{
				bSwitchMode = cBatteryMode;
			}
			else if(sbGetCellVoltHi() || sbGetBattChgCurrHi() \
				|| sbGetCellChgTempHi2() || sbGetCellChgTempLo2())
			{
				bSwitchMode = cDischargeMode;
			}
			else if((sbGetCellVoltLo() && (sbGetBmsChargeOver5A() == false)) \
				|| sbGetBattDcgCurrHi())
			{
				bSwitchMode = cChargeMode;
			}
			else
			{
				bSwitchMode = cBatteryMode;
			}
			
			if(bSwitchMode != bBmsMode)
			{
				if(bSwitchMode == cDischargeMode)
				{
					OSEventSend(cPrioSuper, eSuperToDischarge);
				}
				else if(bSwitchMode == cChargeMode)
				{
					OSEventSend(cPrioSuper, eSuperToCharge);
				}
			}
		break;
		case cDischargeMode:
			if(sbGetDischargeSteadyFlag() == true)
			{
				if((sbGetCellVoltHi() == false) && (sbGetBattChgCurrHi() == false) \
					&& (sbGetCellChgTempHi2() == false) && (sbGetCellChgTempLo2() == false))
				{
					bSwitchMode = cBatteryMode;
				}
				else
				{
					bSwitchMode = cDischargeMode;
				}
			}
			else
			{
				bSwitchMode = cDischargeMode;
			}
			
			if(bSwitchMode != bBmsMode)
			{
				if(bSwitchMode == cBatteryMode)
				{
					OSEventSend(cPrioSuper, eSuperToBattery);
				}
			}
		break;
		case cChargeMode:
			if(sbGetKeyStatusForcedOn())
			{
				bSwitchMode = cBatteryMode;
			}
			else if(sbGetChargeSteadyFlag() == true)
			{
				if((sbGetBattDcgCurrHi() == false) \
					&& (sbGetBmsChargeOver5A() || (sbGetCellVoltLo() == false)))
				{
					bSwitchMode = cBatteryMode;
				}
				else
				{
					bSwitchMode = cChargeMode;
				}
			}
			else
			{
				bSwitchMode = cChargeMode;
			}
			
			if(bSwitchMode != bBmsMode)
			{
				if(bSwitchMode == cSleepMode)
				{
					OSEventSend(cPrioSuper, eSuperToSleep);
				}
				else if(bSwitchMode == cBatteryMode)
				{
					OSEventSend(cPrioSuper, eSuperToBattery);
				}
			}
		break;
		case cSleepMode:
			bSwitchMode = cChargeMode;
			if(bSwitchMode != bBmsMode)
			{
				if(bSwitchMode == cChargeMode)
				{
					OSEventSend(cPrioSuper, eSuperToCharge);
				}
			}
		break;
		case cFaultMode:
			if(swGetBmsFaultFlag() == 0)
			{
				if(sbGetFaultPowerOnFlag())
				{
					bSwitchMode = cPowerOnMode;
				}
				else if(sbGetFaultRestartFlag())
				{
					bSwitchMode = cStandbyMode;
				}
				else
				{
					bSwitchMode = cFaultMode;
				}
			}
			else
			{
				bSwitchMode = cFaultMode;
			}
			
			if(bSwitchMode != bBmsMode)
			{
				if(bSwitchMode == cPowerOnMode)
				{
					OSEventSend(cPrioSuper, eSuperToPowerOn);
				}
				else if(bSwitchMode == cStandbyMode)
				{
					OSEventSend(cPrioSuper, eSuperToStandby);
				}
			}
		break;
		default:break;
	}
}

/********************************************************************************
* Function :sPowerOnMode()                                                      *
********************************************************************************/
void    sPowerOnMode(void)
{
	INT8U	bNextMode;
	
	// Initial
	mMOS_ALL_OFF();
	sSetEepromRwEnable(true);
	sSetOpSoftFinishedFlag(false);
	
	while(1)
	{
		bNextMode = sbWaitSystemSteady(cT1sec);
		if(bNextMode == cShutdownMode)
		{
			bBmsMode = cShutdownMode;
			return;
		}
		else if(bNextMode == cUpgradeMode)
		{
			bBmsMode = cUpgradeMode;
			return;
		}
		
		bNextMode = sbSystemInitialize();
		if(bNextMode == cShutdownMode)
		{
			bBmsMode = cShutdownMode;
			return;
		}
		else if(bNextMode == cUpgradeMode)
		{
			bBmsMode = cUpgradeMode;
			return;
		}
		
		bNextMode = sbModelIdentify(cT300ms);
		if(bNextMode == cShutdownMode)
		{
			bBmsMode = cShutdownMode;
			return;
		}
		else if(bNextMode == cUpgradeMode)
		{
			bBmsMode = cUpgradeMode;
			return;
		}
		
		bNextMode = sbAdcOffsetInitial(cT200ms);
		if(bNextMode == cShutdownMode)
		{
			bBmsMode = cShutdownMode;
			return;
		}
		else if(bNextMode == cUpgradeMode)
		{
			bBmsMode = cUpgradeMode;
			return;
		}
		else if(bNextMode == cFaultMode)
		{
			bBmsMode = cFaultMode;
			return;
		}
		
		bNextMode = sbAfeEepromInitial(cT500ms);
		if(bNextMode == cShutdownMode)
		{
			bBmsMode = cShutdownMode;
			return;
		}
		else if(bNextMode == cUpgradeMode)
		{
			bBmsMode = cUpgradeMode;
			return;
		}
		else if(bNextMode == cFaultMode)
		{
			bBmsMode = cFaultMode;
			return;
		}
		
		bBmsMode = cStandbyMode;
		return;
	}
}

/********************************************************************************
* Function :sStandbyMode()                                                      *
********************************************************************************/
void    sStandbyMode(void)
{
    INT16U  wEvent;
	
    // Initial
	mMOS_ALL_OFF();
	sSetEepromRwEnable(true);
	sSetStandbySteadyFlag(false);
	sSetOpSoftFinishedFlag(false);
	
    while(1)
    {
        wEvent = OSEventPend();
		
	#ifdef ALWAYS_CHARGE_MODE
		if(wEvent & (1 << eSuperToCharge))
        {
			bBmsMode = cChargeMode;
			return;
        }
	#endif
	#ifdef ALWAYS_DISCHARGE_MODE
		if(wEvent & (1 << eSuperToDischarge))
        {
			bBmsMode = cDischargeMode;
			return;
        }
	#endif
	#ifdef ALWAYS_SLEEP_MODE
		if(wEvent & (1 << eSuperToSleep))
        {
			bBmsMode = cSleepMode;
			return;
        }
	#endif
		
		if(wEvent & (1 << eSuperToShutdown))
        {
			bBmsMode = cShutdownMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToUpgrade))
        {
			bBmsMode = cUpgradeMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToFault))
        {
			bBmsMode = cFaultMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToCharge))
        {
			bBmsMode = cChargeMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToDischarge))
        {
			bBmsMode = cDischargeMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToBattery))
        {
			bBmsMode = cBatteryMode;
			return;
        }
		
        if(wEvent & (1 << eSuperTimer))
        {
            sModeSwitch();
			
			sStandbySteadyChk(cT3sec);
        }
    }
}

/********************************************************************************
* Function :sBatteryMode()                                                      *
********************************************************************************/
void    sBatteryMode(void)
{
    INT16U  wEvent;
	
    // Initial
    isPirated();
	sSetEepromRwEnable(true);
#if(cBMS_SOFTSTART_ENABLE == 1)
    sSetOpSoftFinishedFlag(false);
	sSetSoftStartMode(cSS_MODE_READY);
#else
	sSetOpSoftFinishedFlag(true);
	mMOS_ALL_ON();
#endif
	
    while(1)
    {
        wEvent = OSEventPend();
		
		if(wEvent & (1 << eSuperToShutdown))
        {
			bBmsMode = cShutdownMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToUpgrade))
        {
			bBmsMode = cUpgradeMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToFault))
        {
			bBmsMode = cFaultMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToDischarge))
        {
			bBmsMode = cDischargeMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToSleep))
        {
			bBmsMode = cSleepMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToCharge))
        {
			bBmsMode = cChargeMode;
			return;
        }
		
        if(wEvent & (1 << eSuperTimer))
        {
            sModeSwitch();
        }
    }
}

/********************************************************************************
* Function :sDischargeMode()                                                    *
********************************************************************************/
void    sDischargeMode(void)
{
    INT16U  wEvent;
	
	// Initial
	mMOS_CHG_OFF();
	mMOS_DSG_ON();
	sSetEepromRwEnable(true);
	sSetDischargeSteadyFlag(false);
	sSetOpSoftFinishedFlag(true);
	
    while(1)
    {
        wEvent = OSEventPend();
		
		if(wEvent & (1 << eSuperToShutdown))
        {
			bBmsMode = cShutdownMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToUpgrade))
        {
			bBmsMode = cUpgradeMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToFault))
        {
			bBmsMode = cFaultMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToBattery))
        {
			bBmsMode = cBatteryMode;
			return;
        }
		
        if(wEvent & (1 << eSuperTimer))
        {
            sModeSwitch();
			
			sDischargeSteadyChk(cT15sec);
        }
    }
}

/********************************************************************************
* Function :sChargeMode()                                                       *
********************************************************************************/
void    sChargeMode(void)
{
    INT16U  wEvent;
	
	// Initial
	mMOS_DSG_OFF();
	mMOS_CHG_ON();
	sSetEepromRwEnable(true);
	sSetChargeSteadyFlag(false);
	sSetOpSoftFinishedFlag(true);
	
    while(1)
    {
        wEvent = OSEventPend();
		
		if(wEvent & (1 << eSuperToShutdown))
        {
			bBmsMode = cShutdownMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToUpgrade))
        {
			bBmsMode = cUpgradeMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToFault))
        {
			bBmsMode = cFaultMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToSleep))
        {
			bBmsMode = cSleepMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToBattery))
        {
			bBmsMode = cBatteryMode;
			return;
        }
		
        if(wEvent & (1 << eSuperTimer))
        {
            sModeSwitch();
			
			sChargeSteadyChk(cT15sec);
        }
    }
}

/********************************************************************************
* Function :sSleepMode()                                                        *
********************************************************************************/
void    sSleepMode(void)
{
    INT16U  wEvent;
	INT16U	wSleepCnt;
	
    // Initial
	mMOS_DSG_OFF();
	mMOS_CHG_ON();
	sSetOpSoftFinishedFlag(true);
	
	for(wSleepCnt = 0; wSleepCnt < cT500ms; wSleepCnt++)
	{
		OSMaskEventPend(1 << eSuperTimer);
	}
	
	while(1)
	{
		if(sbGetEepromRwBusy())
		{
			OSMaskEventPend(1 << eSuperTimer);
		}
		else
		{
			sSetEepromRwEnable(false);
			break;
		}
	}
	
    while(1)
    {
        wEvent = OSEventPend();
		
		if(wEvent & (1 << eSuperToShutdown))
        {
			bBmsMode = cShutdownMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToUpgrade))
        {
			bBmsMode = cUpgradeMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToFault))
        {
			bBmsMode = cFaultMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToCharge))
        {
			bBmsMode = cChargeMode;
			return;
        }
		
        if(wEvent & (1 << eSuperTimer))
        {
            sModeSwitch();
        }
    }
}


/********************************************************************************
* Function :sFaultMode()                                                        *
********************************************************************************/
void    sFaultMode(void)
{
    INT16U  wEvent;
	INT8U	bFirstFaultCode = 0;
	INT16U	wFaultHoldCnt = 0;
	INT16U	wFaultClrTime = 0;
	INT16U	wFaultClrCnt1 = 0;
	INT16U	wFaultClrCnt2 = 0;
	
    // Initial
	mMOS_ALL_OFF();
	
	if(sdwGetBmsRunSec() >= 300)	// 300s
	{
		// 每次开机后只允许记录一次故障（首发故障）,记录过后只有重启才能再次记录
		sSetEepromRwEnable(true);	// 需要优化,如果反复进入故障,EEPROM读写次数会超过
		OSEventSend(cPrioInterface, eEepromFaultRecordSave);
	}
	
	sSetOpSoftFinishedFlag(false);
	sSetFaultPowerOnFlag(false);
	sSetFaultRestartFlag(false);
	bFirstFaultCode = sbGetBmsFaultCode();
	wFaultClrTime = (INT16U)((INT32U)swGetEepromFaultClrTime() * cT1sec);
	bFaultTotalCnt++;
	
    while(1)
    {
        wEvent = OSEventPend();
		
		if(wEvent & (1 << eSuperToShutdown))
        {
			bBmsMode = cShutdownMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToUpgrade))
        {
			bBmsMode = cUpgradeMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToCharge))
        {
			bBmsMode = cChargeMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToPowerOn))
        {
			bBmsMode = cPowerOnMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToStandby))
        {
			bBmsMode = cStandbyMode;
			return;
        }
		
        if(wEvent & (1 << eSuperTimer))
        {
            sModeSwitch();
			
			if((bFirstFaultCode != sbGetBmsFaultCode()) \
				&& (sbGetBmsFaultCode() != 0))
			{
				if(++wFaultHoldCnt >= cT5sec)
				{
					wFaultHoldCnt = 0;
					sSetBmsFaultCode(bFirstFaultCode);
				}
			}
			else
			{
				wFaultHoldCnt = 0;
			}
			
			if((swGetEepromFaultRstCnt() == 0) || (bFaultTotalCnt < swGetEepromFaultRstCnt()))
			{
				if((swGetBmsFaultFlag() & (cBattChgCurrHiFault|cBattDcgCurrHiFault|cSampSensorFault|cOutputResLoFault)) != 0)
				{
					if(++wFaultClrCnt1 >= wFaultClrTime)
					{
						wFaultClrCnt1 = 0;
						
						sClrBmsFaultFlag(cBattChgCurrHiFault|cBattDcgCurrHiFault|cSampSensorFault|cOutputResLoFault);
						if(sbGetBmsFaultCode() == cBattChgCurrHiId)
						{
							sClrBmsFaultCode(cBattChgCurrHiId);
						}
						else if(sbGetBmsFaultCode() == cBattDcgCurrHiId)
						{
							sClrBmsFaultCode(cBattDcgCurrHiId);
						}
						else if(sbGetBmsFaultCode() == cSampSensorId)
						{
							sClrBmsFaultCode(cSampSensorId);
						}
						else if(sbGetBmsFaultCode() == cOutputResLoId)
						{
							sClrBmsFaultCode(cOutputResLoId);
						}
						
						if(swGetBmsFaultFlag() == 0)
						{
							if(bFirstFaultCode == cSampSensorId)
							{
								sSetFaultPowerOnFlag(true);
							}
							else
							{
								sSetFaultRestartFlag(true);
							}
						}
					}
				}
				else
				{
					wFaultClrCnt1 = 0;
				}
				
				if(swGetBmsFaultFlag() == 0)
				{
					if(++wFaultClrCnt2 >= wFaultClrTime)
					{
						wFaultClrCnt2 = 0;
						sClrBmsFaultCode(0xFF);
						sSetFaultRestartFlag(true);
					}
					else if(sbGetKeyStatusForcedOn())
					{
						if(sbGetBmsFaultCode() == cCellVoltLoId)
						{
							wFaultClrCnt2 = 0;
							sClrBmsFaultCode(0xFF);
							sSetFaultRestartFlag(true);
						}
					}
				}
				else
				{
					wFaultClrCnt2 = 0;
				}
			}
			else
			{
				if(++wFaultClrCnt1 >= cT1min)
				{
					wFaultClrCnt1 = 0;
					if(++wFaultClrCnt2 >= 1440)		// 24H
					{
						wFaultClrCnt2 = 0;
						
						bFaultTotalCnt = 0;			// 延时24小时自动清除故障次数
					}
				}
			}
        }
    }
}


/********************************************************************************
* Function :sShutDownMode()                                                     *
********************************************************************************/
void    sShutDownMode(void)
{
	INT8U i;
	
    // Initial
	mMOS_ALL_OFF();
	
	for(i = 0; i < cT200ms; i++)
	{
		OSMaskEventPend(1 << eSuperTimer);
	}
	
	// 检查是否需要广播关机指令
	if(sbGetBmsParallelShutdown())
	{
		sCanToShutdownCmd();
	}
	sSetBmsParallelShutdown(false);
	
	if(sdwGetBmsRunSec() >= 30)	// 30s
	{
		sSetEepromRwEnable(true);
		OSEventSend(cPrioInterface, eEepromRealtimeSave);
		OSEventSend(cPrioInterface, eEepromFaultRecordSave);
	}
	
	sSetOpSoftFinishedFlag(false);
	
	for(i = 0; i < cT500ms; i++)
	{
		OSMaskEventPend(1 << eSuperTimer);
	}
	
	while(1)
	{
		if(sbGetEepromRwBusy())
		{
			OSMaskEventPend(1 << eSuperTimer);
		}
		else
		{
			sSetEepromRwEnable(false);
			break;
		}
	}
	
	for(i = 0; i < cT200ms; i++)
	{
		OSMaskEventPend(1 << eSuperTimer);
	}
	
	mSPS_SHUTDOWN_ENABLE();
	
	bBmsMode = cStandbyMode;
}

/********************************************************************************
* Function :sTestMode()                                                         *
********************************************************************************/
void    sTestMode(void)
{
    INT16U  wEvent;
	
    // Initial
	mMOS_ALL_OFF();
	
	sSetEepromRwEnable(false);
	sSetOpSoftFinishedFlag(false);
	
    while(1)
    {
        wEvent = OSEventPend();
		
		if(wEvent & (1 << eSuperToShutdown))
        {
			bBmsMode = cShutdownMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToUpgrade))
        {
			bBmsMode = cUpgradeMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToFault))
        {
			bBmsMode = cFaultMode;
			return;
        }
		
		if(wEvent & (1 << eSuperToStandby))
        {
			bBmsMode = cStandbyMode;
			return;
        }
		
        if(wEvent & (1 << eSuperTimer))
        {
            sModeSwitch();
        }
    }
}

/********************************************************************************
* Function :sUpgradeMode()                                                      *
********************************************************************************/
void    sUpgradeMode(void)
{
	INT8U i;
	
	// Initial
	// 初始化
	mMOS_ALL_OFF();
	mLED_ALARM_OFF();
	mLED_STATUS_ON();
	
	//等待时间
	for(i = 0; i < cT500ms; i++)
	{
		OSMaskEventPend(1 << eSuperTimer);
	}
	
	// Erase Boot Flag，擦除引导标志
	//mFLASH_UNLOCK();
	//mFLASH_PAGE_ERASE(FLASH_FLG_ADDR_START);
	//mFLASH_LOCK();
	
	// Jump to user application，跳转用户应用程序
	JumpAddress = *(__IO INT32U*) (FLASH_IAP_ADDR_START + 4);
	Jump_To_Application = (pFunction) JumpAddress;
	
	// Initialize user application's Stack Pointer
	__set_MSP(*(__IO INT32U*) FLASH_IAP_ADDR_START);
	Jump_To_Application();
	
	while(1);
}

INT8U	sbWaitSystemSteady(INT8U bFilter)
{
	TASK_EVENT	wEvent;
	INT8U	bWaitCnt = 0;
	
	while(1)
	{
		wEvent = OSEventPend();
		
		if(wEvent & (1 << eSuperToShutdown))
		{
			return cShutdownMode;
		}
		
		if(wEvent & (1 << eSuperToUpgrade))
        {
			return cUpgradeMode;
        }
		
		if(wEvent & (1 << eSuperTimer))
		{
			if(++bWaitCnt >= bFilter)
			{
				return cPowerOnMode;
			}
		}
	}
}

INT8U	sbSystemInitialize(void)
{
	TASK_EVENT	wEvent;
	
	while(1)
	{
		wEvent = OSEventPend();
		
		if(wEvent & (1 << eSuperToShutdown))
		{
			return cShutdownMode;
		}
		
		if(wEvent & (1 << eSuperToUpgrade))
        {
			return cUpgradeMode;
        }
		
		if(wEvent & (1 << eSuperTimer))
		{
			if(sbGetInitializeFinished())
			{
				return cPowerOnMode;
			}
		}
	}
}

INT8U	sbModelIdentify(INT8U bFilter)
{
	TASK_EVENT	wEvent;
	sbBmsIdentifyInitial();
	
	while(1)
	{
		wEvent = OSEventPend();
		
		if(wEvent & (1 << eSuperToShutdown))
		{
			return cShutdownMode;
		}
		
		if(wEvent & (1 << eSuperToUpgrade))
        {
			return cUpgradeMode;
        }
		
		if(wEvent & (1 << eSuperTimer))
		{
			if(sbBmsIdentify(bFilter))
			{
				return cPowerOnMode;
			}
		}
	}
}

INT8U	sbAdcOffsetInitial(INT8U bFilter)
{
	TASK_EVENT	wEvent;
	
	sAdcOffsetInitial();
	
	while(1)
	{
		wEvent = OSEventPend();
		
		if(wEvent & (1 << eSuperToShutdown))
		{
			return cShutdownMode;
		}
		
		if(wEvent & (1 << eSuperToUpgrade))
        {
			return cUpgradeMode;
        }
		
		if(wEvent & (1 << eSuperTimer))
		{
			if(sbAdcOffsetCalc(bFilter))
			{
				return cPowerOnMode;
			}
		}
	}
}

INT8U	sbAfeEepromInitial(INT8U bFilter)
{
	TASK_EVENT	wEvent;
	INT8U	bTemp;
	
	while(1)
	{
		wEvent = OSEventPend();
		
		if(wEvent & (1 << eSuperToShutdown))
		{
			return cShutdownMode;
		}
		
		if(wEvent & (1 << eSuperToUpgrade))
        {
			return cUpgradeMode;
        }
		
		if(wEvent & (1 << eSuperTimer))
		{
			bTemp = sbAfeEepromProc(bFilter);
			if(bTemp == AFE_INIT_WRITE_PASS)
			{
				return cPowerOnMode;
			}
			else if(bTemp == AFE_INIT_WRITE_FAIL)
			{
				return cPowerOnMode;
			}
		}
	}
}

void	sStandbySteadyChk(INT16U wFilter)
{
	static INT16U wToBattModeCnt = 0;
	
	if(sbGetStandbySteadyFlag() == false)
    {
        if(++wToBattModeCnt >= wFilter)
        {
            wToBattModeCnt = 0;
            sSetStandbySteadyFlag(true);
        }
    }
}

void	sDischargeSteadyChk(INT16U wFilter)
{
	static INT16U wSteadyCnt = 0;
	
	if(sbGetDischargeSteadyFlag() == false)
    {
        if(++wSteadyCnt >= wFilter)
        {
            wSteadyCnt = 0;
            sSetDischargeSteadyFlag(true);
        }
    }
}

void	sChargeSteadyChk(INT16U wFilter)
{
	static INT16U wSteadyCnt = 0;
	
	if(sbGetChargeSteadyFlag() == false)
    {
        if(++wSteadyCnt >= wFilter)
        {
            wSteadyCnt = 0;
            sSetChargeSteadyFlag(true);
        }
    }
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
INT8U	sbGetFaultRestartFlag(void)
{
	return fSuper.bits.bFaultRestartFlag;
}

INT8U	sbGetFaultPowerOnFlag(void)
{
	return fSuper.bits.bFaultPowerOnFlag;
}

INT8U	sbGetStandbySteadyFlag(void)
{
	return fSuper.bits.StandbySteadyFlag;
}

INT8U	sbGetChargeSteadyFlag(void)
{
	return fSuper.bits.ChargeSteadyFlag;
}

INT8U	sbGetDischargeSteadyFlag(void)
{
	return fSuper.bits.DischargeSteadyFlag;
}

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
void	sSetFaultRestartFlag(INT8U bFlag)
{
	fSuper.bits.bFaultRestartFlag = bFlag;
}

void	sSetFaultPowerOnFlag(INT8U bFlag)
{
	fSuper.bits.bFaultPowerOnFlag = bFlag;
}

void	sSetStandbySteadyFlag(INT8U bFlag)
{
	fSuper.bits.StandbySteadyFlag = bFlag;
}

void	sSetChargeSteadyFlag(INT8U bFlag)
{
	fSuper.bits.ChargeSteadyFlag = bFlag;
}

void	sSetDischargeSteadyFlag(INT8U bFlag)
{
	fSuper.bits.DischargeSteadyFlag = bFlag;
}

