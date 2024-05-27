/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		InterfaceJob.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define			__INTERFACE_JOB_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define         cTimerBase                      Timer_20ms

#define         cT50ms                          (50 / cTimerBase)
#define         cT100ms                         (100 / cTimerBase)
#define         cT200ms                         (200 / cTimerBase)
#define         cT500ms                         (500 / cTimerBase)
#define         cT1sec                          (1000 / cTimerBase)
#define         cT2sec                          (2000 / cTimerBase)
#define         cT3sec                          (3000 / cTimerBase)
#define         cT4sec                          (4000 / cTimerBase)
#define         cT5sec                          (5000 / cTimerBase)
#define         cT8sec                          (8000 / cTimerBase)
#define         cT10sec                         (10000 / cTimerBase)
#define         cT15sec                         (15000 / cTimerBase)
#define         cT30sec                         (30000 / cTimerBase)
#define         cT1min                          (60000 / cTimerBase)
#define         cT2min                          (120000 / cTimerBase)
#define         cT3min                          (180000 / cTimerBase)
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


/********************************************************************************
* Input variables																*
********************************************************************************/


/********************************************************************************
* Output variables																*
********************************************************************************/


/********************************************************************************
* Internal variables															*
********************************************************************************/
union INTERFACE_STATUS_TABLE
{
	struct INTERFACE_STATUS_BIT_FEILD
	{
		INT16U bInitializeFlag:1;
		INT16U bModBusDispMode:1;	// 0=Modbus中Realtime显示实时数据,1=显示处理数据
		INT16U bRsvd1:2;
		
		INT16U bRsvd2:4;
		
		INT16U bRsvd3:4;
		
		INT16U bRsvd4:4;
	} bits;
	INT16U data;
} fInterface;

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/


/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void	sInterfaceJob(void)
{
	INT16U	wEvent;
#if(WIFI_ENABLE==1)
	INT8U cnt;
#endif
	
	sInterfaceJobInit();
	
	while(1)
	{
		wEvent = OSEventPend();
		
		if(wEvent & (1 << eHostParsing))
		{
			sSciRouting(cHOST_SCI);
			
			if(sbGetRouteProt() == cROUTE_PROT_VOLTRONIC)
			{
				sVoltronic_ModBusParsing(cHOST_SCI);
			}
			else if(sbGetRouteProt() == cROUTE_PROT_PYLON)
			{
				#ifdef PYLON_PROTOCOL_ENABLE
					sPylontechParsing(cHOST_SCI);
				#endif
			}
            else if(sbGetRouteProt() == cROUTE_PROT_Growatt)
			{
				sGroWatt_ModBusParsing(cHOST_SCI);
			}
			else
			{
				sModBusParsing(cHOST_SCI);
			}
		}
		
		if(wEvent & (1 << eEepromStaticSave))
		{
			if(sbGetEepromRwEnable())
			{
				sSetEepromRwBusy(true);
				sEepromStaticSave();
				sSetEepromRwBusy(false);
				
				sBmsParaUpdate();
				sBmsInformationDataUpdate();
			}
		}
		
		if(wEvent & (1 << eEepromDynamicSave))
		{
			if(sbGetEepromRwEnable())
			{
				sSetEepromRwBusy(true);
				sEepromDynamicSave();
				sSetEepromRwBusy(false);
				
				sCanSyncUserParaCommand();
				sBmsParaUpdate();
				sSetBmsUserParaCnt(swGetBmsUserParaCnt() + 1);
			}
		}
		
		if(wEvent & (1 << eEepromRealtimeSave))
		{
			if(sbGetEepromRwEnable())
			{
				sSetEepromRwBusy(true);
				sEepromRealtimeSave();
				sSetEepromRwBusy(false);
				
				sRstBmsTimeStoreMin();
				sSetBmsUserParaCnt(swGetBmsUserParaCnt() + 1);
			}
		}
		
		if(wEvent & (1 << eEepromFaultRecordSave))
		{
			if(sbGetEepromRwEnable())
			{
				sSetEepromRwBusy(true);
				sEepromFaultRecordSave();
				sSetEepromRwBusy(false);
				
				sSetBmsUserParaCnt(swGetBmsUserParaCnt() + 1);
			}
		}
		
		if(wEvent & (1 << eDisplaySocSync))
		{
			sDisplaySocSync(true);
		}
		
		if(wEvent & (1 << eDisplayKeySync))
		{
			sDisplayKeySync(true);
			sDisplayTestLedFault(true);
			sDisplayTestLedNormal(true);
		}
		
		if(wEvent & (1 << eCanAutoAssign))
		{
			sCanAutoAdsAssign();
		}
		
		if(wEvent & (1 << eInterfaceTimer))
		{
			//mLED_TEST_ON();
			// CAN
			sCanParsing();
			sCanTimerPolling(cTimerBase);
			sModBusSciAddrInitChk(cTimerBase);
			
			#if(PYLON_PROTOCOL_CAN_ENABLE==1)
			sPylonProtCanPolling(cPCS_CAN, cT1sec, cT50ms);
			#endif
			
			// MODBUS
			sSciRxTask(cHOST_SCI, cT50ms);
			sCommConnectChk(cHOST_SCI, cT30sec);
			
			// EEPROM
			sEepromStaticSaveMode(cT2sec);
			sEepromDynamicSaveMode(cT2sec);
			sEepromRealtimeSaveMode(cT2sec);
			sEepromFaultRecordSaveMode(cT2sec);
			sEepromSecureLockClr(cT3min);
			
			// KEY
			sKeyPWRScanProc(cT1sec, cT8sec);
			sKeyLCDScanProc(cT50ms, cT2sec);
			sKeyForceOnTimeout(cT5min);
			sKeyTurnOffProc();
			
			// DISPLAY
			// 系统单机或者有充满或者有放空，都按单机显示
			// 系统全部充电或者全部放电或者全部待机则按并机显示，否则按单机显示
			sSetDisplayBmsMode(bBmsMode);
			if((sbGetCanAddress() == 0) || (sbGetEmsSocFullNumber() != 0) || (sbGetEmsSocEmptyNumber() != 0)
				|| ((swGetEmsDataParallelNumber() != sbGetEmsChargingNumber()) \
				&& (swGetEmsDataParallelNumber() != sbGetEmsDischargingNumber()) \
				&& (swGetEmsDataParallelNumber() != sbGetEmsIdleNumber())))
			{
				sSetDisplayBmsSoc(swGetSocNew() * 10);
				if(sbGetBmsCurrentStatus() == cBMS_CURR_DSG)
				{
					sSetDisplayBmsStatus(cDISPLAY_STATUS_DSG);
				}
				else if(sbGetBmsCurrentStatus() == cBMS_CURR_CHG)
				{
					sSetDisplayBmsStatus(cDISPLAY_STATUS_CHG);
				}
				else
				{
					sSetDisplayBmsStatus(cDISPLAY_STATUS_IDLE);
				}
			}
			else
			{
				sSetDisplayBmsSoc(swGetEmsDataTotalSOC());
				if(swGetEmsStatusCharging())
				{
					sSetDisplayBmsStatus(cDISPLAY_STATUS_CHG);
				}
				else if(swGetEmsStatusDischarging())
				{
					sSetDisplayBmsStatus(cDISPLAY_STATUS_DSG);
				}
				else
				{
					sSetDisplayBmsStatus(cDISPLAY_STATUS_IDLE);
				}
			}
			
//			if(swGetBmsLedLcdMode() == cBMS_LED_MODE)
//			{
//				sDisplaySoc(cTimerBase);
//			}
//			else
			{
				sLcdTask(cTimerBase);
			}
			
			sDisplayKey(cTimerBase);
			
			//mLED_TEST_OFF();
		}
	}
}

void	sInterfaceJobInit(void)
{
	fInterface.data = 0;
	
	sKeyProcessInit();
	sDisplayProcessInit();
	sLcdInitial();
	AntiThiefIntal();
	
	sEepromInitial();
	if(sbEepromStaticPop() == true)
	{
		sSetEepromStaticMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	if(sbEepromDynamicPop() == true)
	{
		sSetEepromDynamicMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	if(sbEepromRealtimePop() == true)
	{
		sSetEepromRealtimeMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	if(sbEepromFaultRecordPop() == true)
	{
		sSetEepromFaultRecordMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	
	if(sbEepromStaticDataChk() == true)
	{
		sSetEepromStaticMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	if(sbEepromDynamicDataChk() == true)
	{
		sSetEepromDynamicMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	if(sbEepromRealtimeDataChk() == true)
	{
		sSetEepromRealtimeMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	if(sbEepromFaultRecordDataChk() == true)
	{
		sSetEepromFaultRecordMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	
	sAdcCoeffInit();
	sBatteryModuleInit();
	sProtectionModuleInit();
	sSoxInit();
	sBalanceInit();
	sBmsInitial();
	sEmsInitial();
	sAfeInitial();
	CAN_Initial(cBMS_CAN);
	CAN_Initial(cPCS_CAN);
	sCanInitial();
	
	sBmsInformationDataUpdate();
	
	fInterface.bits.bInitializeFlag = true;
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
INT8U	sbGetInitializeFinished(void)
{
	return fInterface.bits.bInitializeFlag;
}

INT8U	sbGetModBusDispMode(void)
{
	return fInterface.bits.bModBusDispMode;
}

/********************************************************************************
* Input interface Routines														*
********************************************************************************/

void	sSetModBusDispMode(INT8U bMode)
{
	fInterface.bits.bModBusDispMode = bMode;
}

