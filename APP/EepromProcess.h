/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		EepromProcess.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __EEEPROM_PROCESS_H__
#define __EEEPROM_PROCESS_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef  __EEPROM_PROCESS_C__
#define EEPROM
#else
#define EEPROM extern
#endif

#define	cEepromSuccess					0
#define	cEepromFail						1

#define	cEEPROM_STATIC_NO				0
#define	cEEPROM_DYNAMIC_NO				1
#define	cEEPROM_REALTIME_NO				2
#define	cEEPROM_FAULT_RECORD_NO			3
#define	cEEPROM_TASK_NO_MAX				4

#define	cEEPROM_SAVE_MODE_DISABLE		0
#define	cEEPROM_SAVE_MODE_ENABLE		1
#define	cEEPROM_SAVE_MODE_DELAY			2
#define	cEEPROM_SAVE_MODE_ACTION		3

#define	cEepromStaticStartAddr			0x0100
#define	cEepromStaticStartAddr2			0x1100
#define	cEepromStaticTotalLength		sizeof(struct STR_STATIC_EEPROM)/2	// 64words

#define	cEepromDynamicStartAddr			0x0200
#define	cEepromDynamicStartAddr2		0x1200
#define	cEepromDynamicTotalLength		sizeof(struct STR_DYNAMIC_EEPROM)/2	// 64words

#define	cEepromRealtimeStartAddr		0x0300
#define	cEepromRealtimeStartAddr2		0x1300
#define	cEepromRealtimeTotalLength		sizeof(struct STR_REALTIME_EEPROM)/2// 24words

#define	cEepromFrStartAddr				0x0400
#define	cEepromFrStartAddr2				0x1400
#define	cEepromFrTotalLength			sizeof(struct STR_FR_EEPROM)/2		// 40words

#define	NumbOfVar1						((cEepromStaticTotalLength+cEepromDynamicTotalLength)*2)		// 128words(MAX 1023words)
#define	NumbOfVar2						(cEepromRealtimeTotalLength+cEepromFrTotalLength)				//  64words(MAX 1023words)
#define	NumbOfVar3						(cEepromRealtimeTotalLength+cEepromFrTotalLength)				//  64words(MAX 1023words)

#define	cEERsvdDef						0
#define	cEERsvdLimitLo					0
#define	cEERsvdLimitUp					65535

#define	cEERandomNumDef					0
#define	cEERandomNumLimitLo				0
#define	cEERandomNumLimitUp				65535

#define	cEEEepromVerDef					3
#define	cEEEepromVerLimitLo				1
#define	cEEEepromVerLimitUp				1000

#define	cEESerialNoDef					0
#define	cEESerialNoLimitLo				0
#define	cEESerialNoLimitUp				9999

#define	cEESerialNoLenDef				14
#define	cEESerialNoLenLimitLo			14
#define	cEESerialNoLenLimitUp			20

#define	cEEBattVoltAdjDef				4096				// Q12
#define	cEEBattVoltAdjLimitLo			2048
#define	cEEBattVoltAdjLimitUp			8192

#define	cEEBattVoltBiasDef				0
#define	cEEBattVoltBiasLimitLo			((INT16S)-32000)	// Q7,-250mV
#define	cEEBattVoltBiasLimitUp			((INT16S)32000)		// Q7,+250mV

#define	cEEBattCurrAdjDef				4096
#define	cEEBattCurrAdjLimitLo			2048
#define	cEEBattCurrAdjLimitUp			8192

#define	cEEBattCurrBiasDef				0
#define	cEEBattCurrBiasLimitLo			((INT16S)-32000)	// Q7,-25.0A
#define	cEEBattCurrBiasLimitUp			((INT16S)32000)		// Q7,+25.0A

#define	cEECellVoltAdjDef				4096
#define	cEECellVoltAdjLimitLo			2048
#define	cEECellVoltAdjLimitUp			8192

#define	cEECellVoltBiasDef				0
#define	cEECellVoltBiasLimitLo			((INT16S)-32000)	// Q7,-250mV
#define	cEECellVoltBiasLimitUp			((INT16S)32000)		// Q7,-250mV

#define	cEECellTempAdjDef				4096
#define	cEECellTempAdjLimitLo			2048
#define	cEECellTempAdjLimitUp			8192

#define	cEECellTempBiasDef				0
#define	cEECellTempBiasLimitLo			((INT16S)-32000)	// Q7,-250mV
#define	cEECellTempBiasLimitUp			((INT16S)32000)		// Q7,-250mV

#define	cEEBattTypeDef					435				//BF48300
#define	cEEBattTypeLimitLo				0
#define	cEEBattTypeLimitUp				65535

#define	cEESafeAdjDef					4096
#define	cEESafeAdjLimitLo				2048
#define	cEESafeAdjLimitUp				8192

#define	cEESafeBiasDef					0
#define	cEESafeBiasLimitLo				((INT16S)-32000)	// Q7,-250
#define	cEESafeBiasLimitUp				((INT16S)32000)		// Q7,+250

#define	cEEHacpModeDef					0
#define	cEEHacpModeLimitLo				0
#define	cEEHacpModeLimitUp				2

#define	cEESlaveNumDef					1
#define	cEESlaveNumLimitLo				1
#define	cEESlaveNumLimitUp				16

#define	cEEModuleAddrDef				29
#define	cEEModuleAddrLimitLo			1
#define	cEEModuleAddrLimitUp			29

#define	cEEModuleResDef					0
#define	cEEModuleResLimitLo				0
#define	cEEModuleResLimitUp				1

#define	cEECellVoltPct80Def				3450
#define	cEECellVoltPct80LimitLo			3200
#define	cEECellVoltPct80LimitUp			4500

#define	cEECellVoltPct20Def				3050
#define	cEECellVoltPct20LimitLo			1800
#define	cEECellVoltPct20LimitUp			3700

#define	cEECellVoltHi2Def				3750
#define	cEECellVoltHi2LimitLo			3200
#define	cEECellVoltHi2LimitUp			4500

#define	cEECellVoltLo2Def				0
#define	cEECellVoltLo2LimitLo			0
#define	cEECellVoltLo2LimitUp			3700

#define	cEECellVoltHiAlmDef				3600
#define	cEECellVoltHiAlmLimitLo			3200
#define	cEECellVoltHiAlmLimitUp			4500

#define	cEECellVoltHiAlmBackDef			3500
#define	cEECellVoltHiAlmBackLimitLo		3000
#define	cEECellVoltHiAlmBackLimitUp		4200

#define	cEECellVoltHiDef				3650
#define	cEECellVoltHiLimitLo			3200
#define	cEECellVoltHiLimitUp			4500

#define	cEECellVoltHiBackDef			3500
#define	cEECellVoltHiBackLimitLo		3000
#define	cEECellVoltHiBackLimitUp		4200

#define	cEECellVoltLoAlmDef				2700		//2500
#define	cEECellVoltLoAlmLimitLo			2000
#define	cEECellVoltLoAlmLimitUp			3700

#define	cEECellVoltLoAlmBackDef			2900		//2750
#define	cEECellVoltLoAlmBackLimitLo		2500
#define	cEECellVoltLoAlmBackLimitUp		3900

#define	cEECellVoltLoDef				2500		//2300
#define	cEECellVoltLoLimitLo			2000
#define	cEECellVoltLoLimitUp			3700

#define	cEECellVoltLoBackDef			2900		//2750
#define	cEECellVoltLoBackLimitLo		2500
#define	cEECellVoltLoBackLimitUp		3900

#define	cEEBattChgCurrHiAlmBackDef		1600		//400
#define	cEEBattChgCurrHiAlmBackLimitLo	0
#define	cEEBattChgCurrHiAlmBackLimitUp	2000

#define	cEEBattChgCurrHiAlmDef			2000		//500
#define	cEEBattChgCurrHiAlmLimitLo		0
#define	cEEBattChgCurrHiAlmLimitUp		2000

#define	cEEBattChgCurrHiDef				2100		//600
#define	cEEBattChgCurrHiLimitLo			0
#define	cEEBattChgCurrHiLimitUp			2500

#define	cEEBattChgCurrHi2Def			3000		//1200
#define	cEEBattChgCurrHi2LimitLo		0
#define	cEEBattChgCurrHi2LimitUp		4000

#define	cEEBattDcgCurrHiAlmBackDef		1600		//400
#define	cEEBattDcgCurrHiAlmBackLimitLo	0
#define	cEEBattDcgCurrHiAlmBackLimitUp	2000

#define	cEEBattDcgCurrHiAlmDef			2000		//500
#define	cEEBattDcgCurrHiAlmLimitLo		0
#define	cEEBattDcgCurrHiAlmLimitUp		2000

#define	cEEBattDcgCurrHiDef				2100		//600
#define	cEEBattDcgCurrHiLimitLo			0
#define	cEEBattDcgCurrHiLimitUp			2500

#define	cEEBattDcgCurrHi2Def			3000		//1200
#define	cEEBattDcgCurrHi2LimitLo		0
#define	cEEBattDcgCurrHi2LimitUp		4000

#define	cEECellChgTempHiAlmBackDef		45
#define	cEECellChgTempHiAlmBackLimitLo	25
#define	cEECellChgTempHiAlmBackLimitUp	75

#define	cEECellChgTempHiAlmDef			50
#define	cEECellChgTempHiAlmLimitLo		25
#define	cEECellChgTempHiAlmLimitUp		75

#define	cEECellChgTempHiDef				53
#define	cEECellChgTempHiLimitLo			25
#define	cEECellChgTempHiLimitUp			75

#define	cEECellChgTempHi2Def			55
#define	cEECellChgTempHi2LimitLo		25
#define	cEECellChgTempHi2LimitUp		75

#define	cEECellChgTempLoAlmBackDef		10
#define	cEECellChgTempLoAlmBackLimitLo	-25
#define	cEECellChgTempLoAlmBackLimitUp	25

#define	cEECellChgTempLoAlmDef			5
#define	cEECellChgTempLoAlmLimitLo		-25
#define	cEECellChgTempLoAlmLimitUp		25

#define	cEECellChgTempLoDef				2
#define	cEECellChgTempLoLimitLo			-25
#define	cEECellChgTempLoLimitUp			25

#define	cEECellChgTempLo2Def			0
#define	cEECellChgTempLo2LimitLo		-25
#define	cEECellChgTempLo2LimitUp		25

#define	cEECellDcgTempHiAlmBackDef		50
#define	cEECellDcgTempHiAlmBackLimitLo	25
#define	cEECellDcgTempHiAlmBackLimitUp	75

#define	cEECellDcgTempHiAlmDef			55
#define	cEECellDcgTempHiAlmLimitLo		25
#define	cEECellDcgTempHiAlmLimitUp		75

#define	cEECellDcgTempHiDef				58
#define	cEECellDcgTempHiLimitLo			25
#define	cEECellDcgTempHiLimitUp			75

#define	cEECellDcgTempHi2Def			60
#define	cEECellDcgTempHi2LimitLo		25
#define	cEECellDcgTempHi2LimitUp		75

#define	cEECellDcgTempLoAlmBackDef		-10
#define	cEECellDcgTempLoAlmBackLimitLo	-25
#define	cEECellDcgTempLoAlmBackLimitUp	25

#define	cEECellDcgTempLoAlmDef			-15
#define	cEECellDcgTempLoAlmLimitLo		-25
#define	cEECellDcgTempLoAlmLimitUp		25

#define	cEECellDcgTempLoDef				-18
#define	cEECellDcgTempLoLimitLo			-25
#define	cEECellDcgTempLoLimitUp			25

#define	cEECellDcgTempLo2Def			-20
#define	cEECellDcgTempLo2LimitLo		-25
#define	cEECellDcgTempLo2LimitUp		25

#define	cEEHsTempHiAlmDef				90
#define	cEEHsTempHiAlmLimitLo			35
#define	cEEHsTempHiAlmLimitUp			119

#define	cEEHsTempHiDef					100
#define	cEEHsTempHiLimitLo				35
#define	cEEHsTempHiLimitUp				119

#define	cEEBalanceEnDef					1
#define	cEEBalanceEnLimitLo				0
#define	cEEBalanceEnLimitUp				1

#define	cEEBalanceVoltErrDef			100
#define	cEEBalanceVoltErrLimitLo		0
#define	cEEBalanceVoltErrLimitUp		500

#define	cEEBalanceVoltEnDef				3400
#define	cEEBalanceVoltEnLimitLo			2500
#define	cEEBalanceVoltEnLimitUp			4500

#define	cEEBalanceTempEnDef				80
#define	cEEBalanceTempEnLimitLo			35
#define	cEEBalanceTempEnLimitUp			119

#define	cEECellDcgDepthDef				100
#define	cEECellDcgDepthLimitLo			5
#define	cEECellDcgDepthLimitUp			100

#define	cEECellTempCoefDef				0
#define	cEECellTempCoefLimitLo			0
#define	cEECellTempCoefLimitUp			30000

#define	cEEDynamicRsvd2Def				0
#define	cEEDynamicRsvd2LimitLo			0
#define	cEEDynamicRsvd2LimitUp			65535

#define	cEEFaultRstCntDef				0
#define	cEEFaultRstCntLimitLo			0
#define	cEEFaultRstCntLimitUp			1000

#define	cEEFaultClrTimeDef				30
#define	cEEFaultClrTimeLimitLo			5
#define	cEEFaultClrTimeLimitUp			3600

#define	cEEIdleExitTimeDef				1440
#define	cEEIdleExitTimeLimitLo			0
#define	cEEIdleExitTimeLimitUp			43200

#define	cEESleepExitTimeDef				5
#define	cEESleepExitTimeLimitLo			0
#define	cEESleepExitTimeLimitUp			43200

#define	cEEBreakWireModeDef				0
#define	cEEBreakWireModeLimitLo			0
#define	cEEBreakWireModeLimitUp			1

#define	cEEBackLightEnableDef			0
#define	cEEBackLightEnableLimitLo		0
#define	cEEBackLightEnableLimitUp		1

#define	cEEMenuRtnEnableDef				0
#define	cEEMenuRtnEnableLimitLo			0
#define	cEEMenuRtnEnableLimitUp			1

#define	cEEFaultRecordEnableDef			0
#define	cEEFaultRecordEnableLimitLo		0
#define	cEEFaultRecordEnableLimitUp		1

#define	cEECellTempShieldDef			0
#define	cEECellTempShieldLimitLo		0
#define	cEECellTempShieldLimitUp		0xFFFF

#define	cEECellVoltPct20TwoDef			3200
#define	cEECellVoltPct20TwoLimitLo		1800
#define	cEECellVoltPct20TwoLimitUp		3700

#define	cEEBattAhRealDef				0
#define	cEEBattAhRealLimitLo			0
#define	cEEBattAhRealLimitUp			10000

#define	cEEBattAhMaxDef					3000	// 300.0AH
#define	cEEBattAhMaxLimitLo				0
#define	cEEBattAhMaxLimitUp				10000

#define	cEEBattFullCntDef				0
#define	cEEBattFullCntLimitLo			0
#define	cEEBattFullCntLimitUp			65535

#define	cEEBattEmptyCntDef				0
#define	cEEBattEmptyCntLimitLo			0
#define	cEEBattEmptyCntLimitUp			65535

#define	cEEBattCycCntDef				0
#define	cEEBattCycCntLimitLo			0
#define	cEEBattCycCntLimitUp			65535

#define	cEEBattChgAhLoDef				0		// 0.01AH
#define	cEEBattChgAhLoLimitLo			0
#define	cEEBattChgAhLoLimitUp			65535

#define	cEEBattChgAhHiDef				0
#define	cEEBattChgAhHiLimitLo			0
#define	cEEBattChgAhHiLimitUp			65535

#define	cEEBattChgWhLoDef				0		// 0.01W
#define	cEEBattChgWhLoLimitLo			0
#define	cEEBattChgWhLoLimitUp			65535

#define	cEEBattChgWhHiDef				0
#define	cEEBattChgWhHiLimitLo			0
#define	cEEBattChgWhHiLimitUp			65535

#define	cEEBattChgTimeLoDef				0		// 1s
#define	cEEBattChgTimeLoLimitLo			0
#define	cEEBattChgTimeLoLimitUp			65535

#define	cEEBattChgTimeHiDef				0
#define	cEEBattChgTimeHiLimitLo			0
#define	cEEBattChgTimeHiLimitUp			65535

#define	cEEBattDcgAhLoDef				0		// 0.01AH
#define	cEEBattDcgAhLoLimitLo			0
#define	cEEBattDcgAhLoLimitUp			65535

#define	cEEBattDcgAhHiDef				0
#define	cEEBattDcgAhHiLimitLo			0
#define	cEEBattDcgAhHiLimitUp			65535

#define	cEEBattDcgWhLoDef				0		// 0.01W
#define	cEEBattDcgWhLoLimitLo			0
#define	cEEBattDcgWhLoLimitUp			65535

#define	cEEBattDcgWhHiDef				0
#define	cEEBattDcgWhHiLimitLo			0
#define	cEEBattDcgWhHiLimitUp			65535

#define	cEEBattDcgTimeLoDef				0		// 1s
#define	cEEBattDcgTimeLoLimitLo			0
#define	cEEBattDcgTimeLoLimitUp			65535

#define	cEEBattDcgTimeHiDef				0
#define	cEEBattDcgTimeHiLimitLo			0
#define	cEEBattDcgTimeHiLimitUp			65535

#define	cEEBattRunTimeLoDef				0		// 1s
#define	cEEBattRunTimeLoLimitLo			0
#define	cEEBattRunTimeLoLimitUp			65535

#define	cEEBattRunTimeHiDef				0
#define	cEEBattRunTimeHiLimitLo			0
#define	cEEBattRunTimeHiLimitUp			65535

#define	cEEFaultRecordDef				0
#define	cEEFaultRecordLimitLo			0
#define	cEEFaultRecordLimitUp			65535


struct	STR_STATIC_EEPROM
{
	INT16U wLength;
	
	INT16U wEepromVer;
	INT16U wSerialNo[5];
	INT16U wSerialNoLen;
	INT16U wBattVoltAdj;
	INT16S wBattVoltBias;
	INT16U wBattChgCurrAdj;
	INT16S wBattChgCurrBias;
	INT16U wBattDsgCurrAdj;
	INT16S wBattDsgCurrBias;
	INT16U wCellVolt01Adj;
	INT16S wCellVolt01Bias;
	INT16U wCellVolt02Adj;
	INT16S wCellVolt02Bias;
	INT16U wCellVolt03Adj;
	INT16S wCellVolt03Bias;
	INT16U wCellVolt04Adj;
	INT16S wCellVolt04Bias;
	INT16U wCellVolt05Adj;
	INT16S wCellVolt05Bias;
	INT16U wCellVolt06Adj;
	INT16S wCellVolt06Bias;
	INT16U wCellVolt07Adj;
	INT16S wCellVolt07Bias;
	INT16U wCellVolt08Adj;
	INT16S wCellVolt08Bias;
	INT16U wCellVolt09Adj;
	INT16S wCellVolt09Bias;
	INT16U wCellVolt10Adj;
	INT16S wCellVolt10Bias;
	INT16U wCellVolt11Adj;
	INT16S wCellVolt11Bias;
	INT16U wCellVolt12Adj;
	INT16S wCellVolt12Bias;
	INT16U wCellVolt13Adj;
	INT16S wCellVolt13Bias;
	INT16U wCellVolt14Adj;
	INT16S wCellVolt14Bias;
	INT16U wCellVolt15Adj;
	INT16S wCellVolt15Bias;
	INT16U wCellVolt16Adj;
	INT16S wCellVolt16Bias;
	INT16U wCellTempAdj;
	INT16S wCellTempBias;
	INT16U wBattType;
	INT16U wReserved[13];
	INT16U wRandomNum;
	
	INT16U wCheckSum;
};

struct	STR_DYNAMIC_EEPROM
{
	INT16U wLength;
	
	INT16U wHacpMode;
	INT16U wSlaveNum;
	INT16U wModuleAddr;
	INT16U wModuleRes;
	INT16U wCellVoltPct80;
	INT16U wCellVoltPct20;
	INT16U wCellVoltHi2;
	INT16U wCellVoltLo2;
	INT16U wCellVoltHiAlm;
	INT16U wCellVoltHiAlmBack;
	INT16U wCellVoltHi;
	INT16U wCellVoltHiBack;
	INT16U wCellVoltLoAlm;
	INT16U wCellVoltLoAlmBack;
	INT16U wCellVoltLo;
	INT16U wCellVoltLoBack;
	INT16U wBattChgCurrHiAlmBack;
	INT16U wBattChgCurrHiAlm;
	INT16U wBattChgCurrHi;
	INT16U wBattChgCurrHi2;
	INT16U wBattDcgCurrHiAlmBack;
	INT16U wBattDcgCurrHiAlm;
	INT16U wBattDcgCurrHi;
	INT16U wBattDcgCurrHi2;
	INT16U wCellChgTempHiAlmBack;
	INT16U wCellChgTempHiAlm;
	INT16U wCellChgTempHi;
	INT16U wCellChgTempHi2;
	INT16S wCellChgTempLoAlmBack;
	INT16S wCellChgTempLoAlm;
	INT16S wCellChgTempLo;
	INT16S wCellChgTempLo2;
	INT16U wCellDcgTempHiAlmBack;
	INT16U wCellDcgTempHiAlm;
	INT16U wCellDcgTempHi;
	INT16U wCellDcgTempHi2;
	INT16S wCellDcgTempLoAlmBack;
	INT16S wCellDcgTempLoAlm;
	INT16S wCellDcgTempLo;
	INT16S wCellDcgTempLo2;
	INT16U wHsTempHiAlm;
	INT16U wHsTempHi;
	INT16U wBalanceEn;
	INT16U wBalanceVoltErr;
	INT16U wBalanceVoltEn;
	INT16U wBalanceTempEn;
	INT16U wCellDcgDepth;
	INT16S wCellTempCoef;
	INT16U wReserved2;
	INT16U wFaultRstCnt;
	INT16U wFaultClrTime;
	INT16U wIdleExitTime;
	INT16U wSleepExitTime;
	INT16U wBreakWireMode;
	INT16U wBackLightEn;
	INT16U wMenuRtnEn;
	INT16U wFaultRecordEn;
	INT16U wCellTempShield;
	INT16U wCellVoltPct20Two;
	INT16U wReserved[2];
	INT16U wRandomNum;
	
	INT16U wCheckSum;
};

struct	STR_REALTIME_EEPROM
{
	INT16U wLength;
	
	INT16U wBattAhReal;			// Battery AH real
	INT16U wBattAhMax;			// Battery AH max
	INT16U wBattFullCnt;		// Battery Full count
	INT16U wBattEmptyCnt;		// Battery Empty count
	INT16U wBattCycCnt;			// Battery Cycle count
	INT16U wBattChgAhLo;		// Battery charge AH low word
	INT16U wBattChgAhHi;		// Battery charge AH high word
	INT16U wBattChgWhLo;		// Battery charge WH low word
	INT16U wBattChgWhHi;		// Battery charge WH high word
	INT16U wBattChgTimeLo;		// Battery charge time low word
	INT16U wBattChgTimeHi;		// Battery charge time high word
	INT16U wBattDcgAhLo;		// Battery discharge AH low word
	INT16U wBattDcgAhHi;		// Battery discharge AH high word
	INT16U wBattDcgWhLo;		// Battery discharge WH low word
	INT16U wBattDcgWhHi;		// Battery discharge WH high word
	INT16U wBattDcgTimeLo;		// Battery discharge time low word
	INT16U wBattDcgTimeHi;		// Battery discharge time high word
	INT16U wBattRunTimeLo;		// Battery run time low word
	INT16U wBattRunTimeHi;		// Battery run time high word
	INT16U wReserved[3];
	
	INT16U wCheckSum;
};

struct	STR_FR_EEPROM
{
	INT16U wLength;
	
	INT16U wFrBmsMode;
	INT16U wFrAlarmCode;
	INT16U wFrFaultFlag;
	INT16U wFrAlarmFlag;
	INT16U wFrBmsStatus;
	INT16U wFrBattVolt;
	INT16U wFrBattCurr;
	INT16U wBattSoC;
	INT16U wBattSoH;
	INT16U wFrHsTemp;
	INT16U wFrBalanceTemp;
	INT16U wFrCellVolt1;
	INT16U wFrCellVolt2;
	INT16U wFrCellVolt3;
	INT16U wFrCellVolt4;
	INT16U wFrCellVolt5;
	INT16U wFrCellVolt6;
	INT16U wFrCellVolt7;
	INT16U wFrCellVolt8;
	INT16U wFrCellVolt9;
	INT16U wFrCellVolt10;
	INT16U wFrCellVolt11;
	INT16U wFrCellVolt12;
	INT16U wFrCellVolt13;
	INT16U wFrCellVolt14;
	INT16U wFrCellVolt15;
	INT16U wFrCellVolt16;
	INT16U wFrCellTemp1;
	INT16U wFrCellTemp2;
	INT16U wFrCellTemp3;
	INT16U wFrCellTemp4;
	INT16U wReserved[7];
	
	INT16U wCheckSum;
};

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
EEPROM	void	sEepromInitial(void);
EEPROM	void	sEepromTaskInitial(INT8U bTask, INT8U bPrio, INT8U bEvent);
EEPROM	INT8U	sbEepromStaticPop(void);
EEPROM	INT8U	sbEepromDynamicPop(void);
EEPROM	INT8U	sbEepromRealtimePop(void);
EEPROM	INT8U	sbEepromFaultRecordPop(void);
EEPROM	void	sEepromStaticSave(void);
EEPROM	void	sEepromDynamicSave(void);
EEPROM	void	sEepromRealtimeSave(void);
EEPROM	void	sEepromFaultRecordSave(void);
EEPROM	void	sEepromSecureLockClr(INT16U	wFilter);
EEPROM	INT8U	sbEepromStaticDataChk(void);
EEPROM	INT8U	sbEepromDynamicDataChk(void);
EEPROM	INT8U	sbEepromRealtimeDataChk(void);
EEPROM	INT8U	sbEepromFaultRecordDataChk(void);
EEPROM	void	sEepromStaticSaveMode(INT16U wFilter);
EEPROM	void	sEepromDynamicSaveMode(INT16U wFilter);
EEPROM	void	sEepromRealtimeSaveMode(INT16U wFilter);
EEPROM	void	sEepromFaultRecordSaveMode(INT16U wFilter);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
EEPROM	INT8U	sbGetEepromRwEnable(void);
EEPROM	INT8U	sbGetEepromRwBusy(void);
EEPROM	INT8U	sbGetEepromSecureLock(void);
EEPROM	INT16U	swGetEepromStaticMapData(INT16U wAddrOffset);
EEPROM	INT16U	swGetEepromDynamicMapData(INT16U wAddrOffset);
EEPROM	INT16U	swGetEepromRealtimeMapData(INT16U wAddrOffset);
EEPROM	INT16U	swGetEepromFrMapData(INT16U wAddrOffset);
EEPROM	INT16U	swGetEepromVer(void);
EEPROM	INT16U	swGetEepromSerialNo1(void);
EEPROM	INT16U	swGetEepromSerialNo2(void);
EEPROM	INT16U	swGetEepromSerialNo3(void);
EEPROM	INT16U	swGetEepromSerialNo4(void);
EEPROM	INT16U	swGetEepromSerialNo5(void);
EEPROM	INT16U	swGetEepromSerialNoLen(void);
EEPROM	INT16U	swGetEepromBattVoltAdj(void);
EEPROM	INT16S	swGetEepromBattVoltBias(void);
EEPROM	INT16U	swGetEepromBattChgCurrAdj(void);
EEPROM	INT16S	swGetEepromBattChgCurrBias(void);
EEPROM	INT16U	swGetEepromBattDsgCurrAdj(void);
EEPROM	INT16S	swGetEepromBattDsgCurrBias(void);
EEPROM	INT16U	swGetEepromCellVoltAdj(INT8U bNo);
EEPROM	INT16S	swGetEepromCellVoltBias(INT8U bNo);
EEPROM	INT16U	swGetEepromCellVolt01Adj(void);
EEPROM	INT16S	swGetEepromCellVolt01Bias(void);
EEPROM	INT16U	swGetEepromCellVolt02Adj(void);
EEPROM	INT16S	swGetEepromCellVolt02Bias(void);
EEPROM	INT16U	swGetEepromCellVolt03Adj(void);
EEPROM	INT16S	swGetEepromCellVolt03Bias(void);
EEPROM	INT16U	swGetEepromCellVolt04Adj(void);
EEPROM	INT16S	swGetEepromCellVolt04Bias(void);
EEPROM	INT16U	swGetEepromCellVolt05Adj(void);
EEPROM	INT16S	swGetEepromCellVolt05Bias(void);
EEPROM	INT16U	swGetEepromCellVolt06Adj(void);
EEPROM	INT16S	swGetEepromCellVolt06Bias(void);
EEPROM	INT16U	swGetEepromCellVolt07Adj(void);
EEPROM	INT16S	swGetEepromCellVolt07Bias(void);
EEPROM	INT16U	swGetEepromCellVolt08Adj(void);
EEPROM	INT16S	swGetEepromCellVolt08Bias(void);
EEPROM	INT16U	swGetEepromCellVolt09Adj(void);
EEPROM	INT16S	swGetEepromCellVolt09Bias(void);
EEPROM	INT16U	swGetEepromCellVolt10Adj(void);
EEPROM	INT16S	swGetEepromCellVolt10Bias(void);
EEPROM	INT16U	swGetEepromCellVolt11Adj(void);
EEPROM	INT16S	swGetEepromCellVolt11Bias(void);
EEPROM	INT16U	swGetEepromCellVolt12Adj(void);
EEPROM	INT16S	swGetEepromCellVolt12Bias(void);
EEPROM	INT16U	swGetEepromCellVolt13Adj(void);
EEPROM	INT16S	swGetEepromCellVolt13Bias(void);
EEPROM	INT16U	swGetEepromCellVolt14Adj(void);
EEPROM	INT16S	swGetEepromCellVolt14Bias(void);
EEPROM	INT16U	swGetEepromCellVolt15Adj(void);
EEPROM	INT16S	swGetEepromCellVolt15Bias(void);
EEPROM	INT16U	swGetEepromCellVolt16Adj(void);
EEPROM	INT16S	swGetEepromCellVolt16Bias(void);
EEPROM	INT16S	swGetEepromBattType(void);
EEPROM	INT16S	swGetEepromStaticRandomNum(void);
EEPROM	INT16U	swGetEepromHacpMode(void);
EEPROM  INT16U	swGetEepromSlaveNum(void);
EEPROM  INT16U	swGetEepromModuleAddr(void);
EEPROM  INT16U	swGetEepromModuleRes(void);
EEPROM  INT16U	swGetEepromCellVoltPct80(void);
EEPROM  INT16U	swGetEepromCellVoltPct20(void);
EEPROM  INT16U	swGetEepromCellVoltHi2(void);
EEPROM  INT16U	swGetEepromCellVoltLo2(void);
EEPROM  INT16U	swGetEepromCellVoltHiAlm(void);
EEPROM  INT16U	swGetEepromCellVoltHiAlmBack(void);
EEPROM  INT16U	swGetEepromCellVoltHi(void);
EEPROM  INT16U	swGetEepromCellVoltHiBack(void);
EEPROM  INT16U	swGetEepromCellVoltLoAlm(void);
EEPROM  INT16U	swGetEepromCellVoltLoAlmBack(void);
EEPROM  INT16U	swGetEepromCellVoltLo(void);
EEPROM  INT16U	swGetEepromCellVoltLoBack(void);
EEPROM  INT16U	swGetEepromBattChgCurrHiAlmBack(void);
EEPROM  INT16U	swGetEepromBattChgCurrHiAlm(void);
EEPROM  INT16U	swGetEepromBattChgCurrHi(void);
EEPROM  INT16U	swGetEepromBattChgCurrHi2(void);
EEPROM  INT16U	swGetEepromBattDcgCurrHiAlmBack(void);
EEPROM  INT16U	swGetEepromBattDcgCurrHiAlm(void);
EEPROM  INT16U	swGetEepromBattDcgCurrHi(void);
EEPROM  INT16U	swGetEepromBattDcgCurrHi2(void);
EEPROM  INT16U	swGetEepromCellChgTempHiAlmBack(void);
EEPROM  INT16U	swGetEepromCellChgTempHiAlm(void);
EEPROM  INT16U	swGetEepromCellChgTempHi(void);
EEPROM  INT16U	swGetEepromCellChgTempHi2(void);
EEPROM  INT16S	swGetEepromCellChgTempLoAlmBack(void);
EEPROM  INT16S	swGetEepromCellChgTempLoAlm(void);
EEPROM  INT16S	swGetEepromCellChgTempLo(void);
EEPROM  INT16S	swGetEepromCellChgTempLo2(void);
EEPROM  INT16U	swGetEepromCellDcgTempHiAlmBack(void);
EEPROM  INT16U	swGetEepromCellDcgTempHiAlm(void);
EEPROM  INT16U	swGetEepromCellDcgTempHi(void);
EEPROM  INT16U	swGetEepromCellDcgTempHi2(void);
EEPROM  INT16S	swGetEepromCellDcgTempLoAlmBack(void);
EEPROM  INT16S	swGetEepromCellDcgTempLoAlm(void);
EEPROM  INT16S	swGetEepromCellDcgTempLo(void);
EEPROM  INT16S	swGetEepromCellDcgTempLo2(void);
EEPROM  INT16U	swGetEepromHsTempHiAlm(void);
EEPROM  INT16U	swGetEepromHsTempHi(void);
EEPROM  INT16U	swGetEepromBalanceEn(void);
EEPROM  INT16U	swGetEepromBalanceVoltErr(void);
EEPROM  INT16U	swGetEepromBalanceVoltEn(void);
EEPROM  INT16U	swGetEepromBalanceTempEn(void);
EEPROM  INT16U	swGetEepromCellDcgDepth(void);
EEPROM  INT16S	swGetEepromCellTempCoef(void);
EEPROM  INT16U	swGetEepromDynamicRsvd2(void);
EEPROM  INT16U	swGetEepromFaultRstCnt(void);
EEPROM  INT16U	swGetEepromFaultClrTime(void);
EEPROM  INT16U	swGetEepromIdleExitTime(void);
EEPROM  INT16U	swGetEepromSleepExitTime(void);
EEPROM  INT16U	swGetEepromBreakWireMode(void);
EEPROM  INT16U	swGetEepromBackLightEn(void);
EEPROM  INT16U	swGetEepromMenuRtnEn(void);
EEPROM  INT16U	swGetEepromFaultRecordEn(void);
EEPROM  INT16U	swGetEepromCellTempShield(void);
EEPROM	INT16U	swGetEepromCellVoltPct20Two(void);
EEPROM  INT16U	swGetEepromBattAhReal(void);
EEPROM  INT16U	swGetEepromBattAhMax(void);
EEPROM  INT16U	swGetEepromBattFullCnt(void);
EEPROM  INT16U	swGetEepromBattEmptyCnt(void);
EEPROM  INT16U	swGetEepromBattCycCnt(void);
EEPROM  INT16U	swGetEepromBattChgAhHi(void);
EEPROM  INT16U	swGetEepromBattChgAhLo(void);
EEPROM  INT32U	sdwGetEepromBattChgAh(void);
EEPROM  INT16U	swGetEepromBattChgWhHi(void);
EEPROM  INT16U	swGetEepromBattChgWhLo(void);
EEPROM  INT32U	sdwGetEepromBattChgWh(void);
EEPROM  INT16U	swGetEepromBattChgTimeHi(void);
EEPROM  INT16U	swGetEepromBattChgTimeLo(void);
EEPROM  INT32U	sdwGetEepromBattChgTime(void);
EEPROM  INT16U	swGetEepromBattDcgAhHi(void);
EEPROM  INT16U	swGetEepromBattDcgAhLo(void);
EEPROM  INT32U	sdwGetEepromBattDcgAh(void);
EEPROM  INT16U	swGetEepromBattDcgWhHi(void);
EEPROM  INT16U	swGetEepromBattDcgWhLo(void);
EEPROM  INT32U	sdwGetEepromBattDcgWh(void);
EEPROM  INT16U	swGetEepromBattDcgTimeHi(void);
EEPROM  INT16U	swGetEepromBattDcgTimeLo(void);
EEPROM  INT32U	sdwGetEepromBattDcgTime(void);
EEPROM  INT16U	swGetEepromBattRunTimeHi(void);
EEPROM  INT16U	swGetEepromBattRunTimeLo(void);
EEPROM  INT32U	sdwGetEepromBattRunTime(void);
EEPROM  INT16U	swGetEepromFrBmsMode(void);
EEPROM  INT16U	swGetEepromFrAlarmCode(void);
EEPROM  INT16U	swGetEepromFrFaultFlag(void);
EEPROM  INT16U	swGetEepromFrAlarmFlag(void);
EEPROM  INT16U	swGetEepromFrBmsStatus(void);
EEPROM  INT16U	swGetEepromFrBattVolt(void);
EEPROM  INT16U	swGetEepromFrBattCurr(void);
EEPROM  INT16U	swGetEepromFrBattSoc(void);
EEPROM  INT16U	swGetEepromFrBattSoh(void);
EEPROM  INT16U	swGetEepromFrHsTemp(void);
EEPROM  INT16U	swGetEepromFrBalanceTemp(void);
EEPROM  INT16U	swGetEepromFrCellVolt1(void);
EEPROM  INT16U	swGetEepromFrCellVolt2(void);
EEPROM  INT16U	swGetEepromFrCellVolt3(void);
EEPROM  INT16U	swGetEepromFrCellVolt4(void);
EEPROM  INT16U	swGetEepromFrCellVolt5(void);
EEPROM  INT16U	swGetEepromFrCellVolt6(void);
EEPROM  INT16U	swGetEepromFrCellVolt7(void);
EEPROM  INT16U	swGetEepromFrCellVolt8(void);
EEPROM  INT16U	swGetEepromFrCellVolt9(void);
EEPROM  INT16U	swGetEepromFrCellVolt10(void);
EEPROM  INT16U	swGetEepromFrCellVolt11(void);
EEPROM  INT16U	swGetEepromFrCellVolt12(void);
EEPROM  INT16U	swGetEepromFrCellVolt13(void);
EEPROM  INT16U	swGetEepromFrCellVolt14(void);
EEPROM  INT16U	swGetEepromFrCellVolt15(void);
EEPROM  INT16U	swGetEepromFrCellVolt16(void);
EEPROM  INT16U	swGetEepromFrCellTemp1(void);
EEPROM  INT16U	swGetEepromFrCellTemp2(void);
EEPROM  INT16U	swGetEepromFrCellTemp3(void);
EEPROM  INT16U	swGetEepromFrCellTemp4(void);

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
EEPROM	void	sDefaultEepromStatic(void);
EEPROM	void	sDefaultEepromDynamic(void);
EEPROM	void	sDefaultEepromRealtime(void);
EEPROM	void	sDefaultEepromFaultRecord(void);
EEPROM	void	sSetEepromRwEnable(INT8U bStatus);
EEPROM	void	sSetEepromRwBusy(INT8U bStatus);
EEPROM	void	sSetEepromSecureLock(INT8U bStatus);
EEPROM	void	sSetEepromStaticMode(INT8U bMode);
EEPROM	void	sSetEepromDynamicMode(INT8U bMode);
EEPROM	void	sSetEepromRealtimeMode(INT8U bMode);
EEPROM	void	sSetEepromFaultRecordMode(INT8U bMode);
EEPROM	void	sSetEepromVer(INT16U wVer);
EEPROM	void	sSetEepromSerialNo1(INT16U wSN);
EEPROM	void	sSetEepromSerialNo2(INT16U wSN);
EEPROM	void	sSetEepromSerialNo3(INT16U wSN);
EEPROM	void	sSetEepromSerialNo4(INT16U wSN);
EEPROM	void	sSetEepromSerialNo5(INT16U wSN);
EEPROM	void	sSetEepromSerialNoLen(INT16U wSNLen);
EEPROM	void	sSetEepromBattVoltAdj(INT16U wAdj);
EEPROM	void	sSetEepromBattVoltBias(INT16S wBias);
EEPROM	void	sSetEepromBattChgCurrAdj(INT16U wAdj);
EEPROM	void	sSetEepromBattChgCurrBias(INT16S wBias);
EEPROM	void	sSetEepromBattDsgCurrAdj(INT16U wAdj);
EEPROM	void	sSetEepromBattDsgCurrBias(INT16S wBias);
EEPROM	void	sSetEepromCellVoltAdj(INT8U bNo, INT16U wAdj);
EEPROM	void	sSetEepromCellVoltBias(INT8U bNo, INT16S wBias);
EEPROM	void	sSetEepromCellVolt01Adj(INT16U wAdj);
EEPROM	void	sSetEepromCellVolt01Bias(INT16S wBias);
EEPROM	void	sSetEepromCellVolt02Adj(INT16U wAdj);
EEPROM	void	sSetEepromCellVolt02Bias(INT16S wBias);
EEPROM	void	sSetEepromCellVolt03Adj(INT16U wAdj);
EEPROM	void	sSetEepromCellVolt03Bias(INT16S wBias);
EEPROM	void	sSetEepromCellVolt04Adj(INT16U wAdj);
EEPROM	void	sSetEepromCellVolt04Bias(INT16S wBias);
EEPROM	void	sSetEepromCellVolt05Adj(INT16U wAdj);
EEPROM	void	sSetEepromCellVolt05Bias(INT16S wBias);
EEPROM	void	sSetEepromCellVolt06Adj(INT16U wAdj);
EEPROM	void	sSetEepromCellVolt06Bias(INT16S wBias);
EEPROM	void	sSetEepromCellVolt07Adj(INT16U wAdj);
EEPROM	void	sSetEepromCellVolt07Bias(INT16S wBias);
EEPROM	void	sSetEepromCellVolt08Adj(INT16U wAdj);
EEPROM	void	sSetEepromCellVolt08Bias(INT16S wBias);
EEPROM	void	sSetEepromCellVolt09Adj(INT16U wAdj);
EEPROM	void	sSetEepromCellVolt09Bias(INT16S wBias);
EEPROM	void	sSetEepromCellVolt10Adj(INT16U wAdj);
EEPROM	void	sSetEepromCellVolt10Bias(INT16S wBias);
EEPROM	void	sSetEepromCellVolt11Adj(INT16U wAdj);
EEPROM	void	sSetEepromCellVolt11Bias(INT16S wBias);
EEPROM	void	sSetEepromCellVolt12Adj(INT16U wAdj);
EEPROM	void	sSetEepromCellVolt12Bias(INT16S wBias);
EEPROM	void	sSetEepromCellVolt13Adj(INT16U wAdj);
EEPROM	void	sSetEepromCellVolt13Bias(INT16S wBias);
EEPROM	void	sSetEepromCellVolt14Adj(INT16U wAdj);
EEPROM	void	sSetEepromCellVolt14Bias(INT16S wBias);
EEPROM	void	sSetEepromCellVolt15Adj(INT16U wAdj);
EEPROM	void	sSetEepromCellVolt15Bias(INT16S wBias);
EEPROM	void	sSetEepromCellVolt16Adj(INT16U wAdj);
EEPROM	void	sSetEepromCellVolt16Bias(INT16S wBias);
EEPROM	void	sSetEepromCellTempAdj(INT16U wAdj);
EEPROM	void	sSetEepromCellTempBias(INT16S wBias);
EEPROM	void	sSetEepromBattType(INT16S wType);
EEPROM	void	sSetEepromStaticRandomNum(INT16U wData);
EEPROM	void	sSetEepromHacpMode(INT16U wMode);
EEPROM  void	sSetEepromSlaveNum(INT16U wNum);
EEPROM  void	sSetEepromModuleAddr(INT16U wAddr);
EEPROM  void	sSetEepromModuleRes(INT16U wEn);
EEPROM  void	sSetEepromCellVoltPct80(INT16U wVolt);
EEPROM  void	sSetEepromCellVoltPct20(INT16U wVolt);
EEPROM  void	sSetEepromCellVoltHi2(INT16U wVolt);
EEPROM  void	sSetEepromCellVoltLo2(INT16U wVolt);
EEPROM  void	sSetEepromCellVoltHiAlm(INT16U wVolt);
EEPROM  void	sSetEepromCellVoltHiAlmBack(INT16U wVolt);
EEPROM  void	sSetEepromCellVoltHi(INT16U wVolt);
EEPROM  void	sSetEepromCellVoltHiBack(INT16U wVolt);
EEPROM  void	sSetEepromCellVoltLoAlm(INT16U wVolt);
EEPROM  void	sSetEepromCellVoltLoAlmBack(INT16U wVolt);
EEPROM  void	sSetEepromCellVoltLo(INT16U wVolt);
EEPROM  void	sSetEepromCellVoltLoBack(INT16U wVolt);
EEPROM  void	sSetEepromBattChgCurrHiAlmBack(INT16U wCurr);
EEPROM  void	sSetEepromBattChgCurrHiAlm(INT16U wCurr);
EEPROM  void	sSetEepromBattChgCurrHi(INT16U wCurr);
EEPROM  void	sSetEepromBattChgCurrHi2(INT16U wCurr);
EEPROM  void	sSetEepromBattDcgCurrHiAlmBack(INT16U wCurr);
EEPROM  void	sSetEepromBattDcgCurrHiAlm(INT16U wCurr);
EEPROM  void	sSetEepromBattDcgCurrHi(INT16U wCurr);
EEPROM  void	sSetEepromBattDcgCurrHi2(INT16U wCurr);
EEPROM  void	sSetEepromCellChgTempHiAlmBack(INT16U wTemp);
EEPROM  void	sSetEepromCellChgTempHiAlm(INT16U wTemp);
EEPROM  void	sSetEepromCellChgTempHi(INT16U wTemp);
EEPROM  void	sSetEepromCellChgTempHi2(INT16U wTemp);
EEPROM  void	sSetEepromCellChgTempLoAlmBack(INT16S wTemp);
EEPROM  void	sSetEepromCellChgTempLoAlm(INT16S wTemp);
EEPROM  void	sSetEepromCellChgTempLo(INT16S wTemp);
EEPROM  void	sSetEepromCellChgTempLo2(INT16S wTemp);
EEPROM  void	sSetEepromCellDcgTempHiAlmBack(INT16U wTemp);
EEPROM  void	sSetEepromCellDcgTempHiAlm(INT16U wTemp);
EEPROM  void	sSetEepromCellDcgTempHi(INT16U wTemp);
EEPROM  void	sSetEepromCellDcgTempHi2(INT16U wTemp);
EEPROM  void	sSetEepromCellDcgTempLoAlmBack(INT16S wTemp);
EEPROM  void	sSetEepromCellDcgTempLoAlm(INT16S wTemp);
EEPROM  void	sSetEepromCellDcgTempLo(INT16S wTemp);
EEPROM  void	sSetEepromCellDcgTempLo2(INT16S wTemp);
EEPROM  void	sSetEepromHsTempHiAlm(INT16U wTemp);
EEPROM  void	sSetEepromHsTempHi(INT16U wTemp);
EEPROM  void	sSetEepromBalanceEn(INT16U wEnable);
EEPROM  void	sSetEepromBalanceVoltErr(INT16U wVolt);
EEPROM  void	sSetEepromBalanceVoltEn(INT16U wVolt);
EEPROM  void	sSetEepromBalanceTempEn(INT16U wTemp);
EEPROM  void	sSetEepromCellDcgDepth(INT16U wDepth);
EEPROM	void	sSetEepromCellTempCoef(INT16S wCoef);
EEPROM  void	sSetEepromDynamicRsvd2(INT16U wStatus);
EEPROM  void	sSetEepromFaultRstCnt(INT16U wCnt);
EEPROM  void	sSetEepromFaultClrTime(INT16U wTime);
EEPROM  void	sSetEepromIdleExitTime(INT16U wTime);
EEPROM  void	sSetEepromSleepExitTime(INT16U wTime);
EEPROM  void	sSetEepromBreakWireMode(INT16U wMode);
EEPROM  void	sSetEepromBackLightEn(INT16U wStatus);
EEPROM  void	sSetEepromMenuRtnEn(INT16U wStatus);
EEPROM  void	sSetEepromFaultRecordEn(INT16U wStatus);
EEPROM  void	sSetEepromCellTempShield(INT16U wStatus);
EEPROM	void	sSetEepromCellVoltPct20Two(INT16U wVolt);
EEPROM  void	sSetEepromBattAhReal(INT16U wWH);
EEPROM  void	sSetEepromBattAhMax(INT16U wWH);
EEPROM  void	sSetEepromBattFullCnt(INT16U wCnt);
EEPROM  void	sSetEepromBattEmptyCnt(INT16U wCnt);
EEPROM  void	sSetEepromBattCycCnt(INT16U wCnt);
EEPROM  void	sSetEepromBattChgAhHi(INT16U wAh);
EEPROM  void	sSetEepromBattChgAhLo(INT16U wAh);
EEPROM  void	sSetEepromBattChgAh(INT32U dwAh);
EEPROM  void	sSetEepromBattChgWhHi(INT16U wWh);
EEPROM  void	sSetEepromBattChgWhLo(INT16U wWh);
EEPROM  void	sSetEepromBattChgWh(INT32U dwWh);
EEPROM  void	sSetEepromBattChgTimeHi(INT16U wTime);
EEPROM  void	sSetEepromBattChgTimeLo(INT16U wTime);
EEPROM  void	sSetEepromBattChgTime(INT32U dwTime);
EEPROM  void	sSetEepromBattDcgAhHi(INT16U wAh);
EEPROM  void	sSetEepromBattDcgAhLo(INT16U wAh);
EEPROM  void	sSetEepromBattDcgAh(INT32U dwAh);
EEPROM  void	sSetEepromBattDcgWhHi(INT16U wWh);
EEPROM  void	sSetEepromBattDcgWhLo(INT16U wWh);
EEPROM  void	sSetEepromBattDcgWh(INT32U dwWh);
EEPROM  void	sSetEepromBattDcgTimeHi(INT16U wTime);
EEPROM  void	sSetEepromBattDcgTimeLo(INT16U wTime);
EEPROM  void	sSetEepromBattDcgTime(INT32U dwTime);
EEPROM  void	sSetEepromBattRunTimeHi(INT16U wTime);
EEPROM  void	sSetEepromBattRunTimeLo(INT16U wTime);
EEPROM  void	sSetEepromBattRunTime(INT32U dwTime);
EEPROM  void	sSetEepromFrBmsMode(INT16U wMode);
EEPROM  void	sSetEepromFrAlarmCode(INT16U wCode);
EEPROM  void	sSetEepromFrFaultFlag(INT16U wFlag);
EEPROM  void	sSetEepromFrAlarmFlag(INT16U wFlag);
EEPROM  void	sSetEepromFrBmsStatus(INT16U wStatus);
EEPROM  void	sSetEepromFrBattVolt(INT16U wVolt);
EEPROM  void	sSetEepromFrBattCurr(INT16U wCurr);
EEPROM  void	sSetEepromFrBattSoc(INT16U wSoC);
EEPROM  void	sSetEepromFrBattSoh(INT16U wSoH);
EEPROM  void	sSetEepromFrHsTemp(INT16U wTemp);
EEPROM  void	sSetEepromFrBalanceTemp(INT16U wTemp);
EEPROM  void	sSetEepromFrCellVolt1(INT16U wVolt);
EEPROM  void	sSetEepromFrCellVolt2(INT16U wVolt);
EEPROM  void	sSetEepromFrCellVolt3(INT16U wVolt);
EEPROM  void	sSetEepromFrCellVolt4(INT16U wVolt);
EEPROM  void	sSetEepromFrCellVolt5(INT16U wVolt);
EEPROM  void	sSetEepromFrCellVolt6(INT16U wVolt);
EEPROM  void	sSetEepromFrCellVolt7(INT16U wVolt);
EEPROM  void	sSetEepromFrCellVolt8(INT16U wVolt);
EEPROM  void	sSetEepromFrCellVolt9(INT16U wVolt);
EEPROM  void	sSetEepromFrCellVolt10(INT16U wVolt);
EEPROM  void	sSetEepromFrCellVolt11(INT16U wVolt);
EEPROM  void	sSetEepromFrCellVolt12(INT16U wVolt);
EEPROM  void	sSetEepromFrCellVolt13(INT16U wVolt);
EEPROM  void	sSetEepromFrCellVolt14(INT16U wVolt);
EEPROM  void	sSetEepromFrCellVolt15(INT16U wVolt);
EEPROM  void	sSetEepromFrCellVolt16(INT16U wVolt);
EEPROM  void	sSetEepromFrCellTemp1(INT16U wTemp);
EEPROM  void	sSetEepromFrCellTemp2(INT16U wTemp);
EEPROM  void	sSetEepromFrCellTemp3(INT16U wTemp);
EEPROM  void	sSetEepromFrCellTemp4(INT16U wTemp);

#endif	// __EEEPROM_PROCESS_H__

