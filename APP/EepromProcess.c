/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		EepromProcess.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define			__EEPROM_PROCESS_C__

/********************************************************************************
* Include head files                                                            *
********************************************************************************/
#include		"EepromProcess.h"
#include		"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/


/********************************************************************************
* Constants																		*
********************************************************************************/
const union {
	struct	STR_STATIC_EEPROM	Field;
	INT16U	Table[cEepromStaticTotalLength];
} cEepromStaticDefaultTable =
{
	cEepromStaticTotalLength,		// wLength
	
	cEEEepromVerDef,				// wEepromVer
	{								// wSerialNo[5]
		cEESerialNoDef,
		cEESerialNoDef,
		cEESerialNoDef,
		cEESerialNoDef,
		cEESerialNoDef
	},
	cEESerialNoLenDef,				// wSerialNoLen
	cEEBattVoltAdjDef,				// wBattVoltAdj
	cEEBattVoltBiasDef,				// wBattVoltBias
	cEEBattCurrAdjDef,				// wBattChgCurrAdj
	cEEBattCurrBiasDef,				// wBattChgCurrBias
	cEEBattCurrAdjDef,				// wBattDsgCurrAdj
	cEEBattCurrBiasDef,				// wBattDsgCurrBias
	cEECellVoltAdjDef,				// wCellVolt01Adj
	cEECellVoltBiasDef,				// wCellVolt01Bias
	cEECellVoltAdjDef,				// wCellVolt02Adj
	cEECellVoltBiasDef,				// wCellVolt02Bias
	cEECellVoltAdjDef,				// wCellVolt03Adj
	cEECellVoltBiasDef,				// wCellVolt03Bias
	cEECellVoltAdjDef,				// wCellVolt04Adj
	cEECellVoltBiasDef,				// wCellVolt04Bias
	cEECellVoltAdjDef,				// wCellVolt05Adj
	cEECellVoltBiasDef,				// wCellVolt05Bias
	cEECellVoltAdjDef,				// wCellVolt06Adj
	cEECellVoltBiasDef,				// wCellVolt06Bias
	cEECellVoltAdjDef,				// wCellVolt07Adj
	cEECellVoltBiasDef,				// wCellVolt07Bias
	cEECellVoltAdjDef,				// wCellVolt08Adj
	cEECellVoltBiasDef,				// wCellVolt08Bias
	cEECellVoltAdjDef,				// wCellVolt09Adj
	cEECellVoltBiasDef,				// wCellVolt09Bias
	cEECellVoltAdjDef,				// wCellVolt10Adj
	cEECellVoltBiasDef,				// wCellVolt10Bias
	cEECellVoltAdjDef,				// wCellVolt11Adj
	cEECellVoltBiasDef,				// wCellVolt11Bias
	cEECellVoltAdjDef,				// wCellVolt12Adj
	cEECellVoltBiasDef,				// wCellVolt12Bias
	cEECellVoltAdjDef,				// wCellVolt13Adj
	cEECellVoltBiasDef,				// wCellVolt13Bias
	cEECellVoltAdjDef,				// wCellVolt14Adj
	cEECellVoltBiasDef,				// wCellVolt14Bias
	cEECellVoltAdjDef,				// wCellVolt15Adj
	cEECellVoltBiasDef,				// wCellVolt15Bias
	cEECellVoltAdjDef,				// wCellVolt16Adj
	cEECellVoltBiasDef,				// wCellVolt16Bias
	cEECellTempAdjDef,				// wCellTempAdj
	cEECellTempBiasDef,				// wCellTempBias
	cEEBattTypeDef,					// wBattType
	{								// wReserved[13]
		cEERsvdDef,
		cEERsvdDef,
		cEERsvdDef,
		cEERsvdDef,
		cEERsvdDef,
		cEERsvdDef,
		cEERsvdDef,
		cEERsvdDef,
		cEERsvdDef,
		cEERsvdDef,
		cEERsvdDef,
		cEERsvdDef,
		cEERsvdDef
	},
	cEERandomNumDef,				// wRandomNum
	
	0								// wCheckSum
};

const union {
	struct	STR_DYNAMIC_EEPROM	Field;
	INT16U	Table[cEepromDynamicTotalLength];
} cEepromDynamicDefaultTable =
{
	cEepromDynamicTotalLength,		// wLength
	
	cEEHacpModeDef,					// wHacpMode
	cEESlaveNumDef,					// wSlaveNum
	cEEModuleAddrDef,				// wModuleAddr
	cEEModuleResDef,				// wModuleRes
	cEECellVoltPct80Def,			// wCellVoltPct80
	cEECellVoltPct20Def,			// wCellVoltPct20
	cEECellVoltHi2Def,				// wCellVoltHi2
	cEECellVoltLo2Def,				// wCellVoltLo2
	cEECellVoltHiAlmDef,			// wCellVoltHiAlm
	cEECellVoltHiAlmBackDef,		// wCellVoltHiAlmBack
	cEECellVoltHiDef,				// wCellVoltHi
	cEECellVoltHiBackDef,			// wCellVoltHiBack
	cEECellVoltLoAlmDef,			// wCellVoltLoAlm
	cEECellVoltLoAlmBackDef,		// wCellVoltLoAlmBack
	cEECellVoltLoDef,				// wCellVoltLo
	cEECellVoltLoBackDef,			// wCellVoltLoBack
	cEEBattChgCurrHiAlmBackDef,		// wBattChgCurrHiAlmBack
	cEEBattChgCurrHiAlmDef,			// wBattChgCurrHiAlm
	cEEBattChgCurrHiDef,			// wBattChgCurrHi
	cEEBattChgCurrHi2Def,			// wBattChgCurrHi2
	cEEBattDcgCurrHiAlmBackDef,		// wBattDcgCurrHiAlmBack
	cEEBattDcgCurrHiAlmDef,			// wBattDcgCurrHiAlm
	cEEBattDcgCurrHiDef,			// wBattDcgCurrHi
	cEEBattDcgCurrHi2Def,			// wBattDcgCurrHi2
	cEECellChgTempHiAlmBackDef,		// wCellChgTempHiAlmBack
	cEECellChgTempHiAlmDef,			// wCellChgTempHiAlm
	cEECellChgTempHiDef,			// wCellChgTempHi
	cEECellChgTempHi2Def,			// wCellChgTempHi2
	cEECellChgTempLoAlmBackDef,		// wCellChgTempLoAlmBack
	cEECellChgTempLoAlmDef,			// wCellChgTempLoAlm
	cEECellChgTempLoDef,			// wCellChgTempLo
	cEECellChgTempLo2Def,			// wCellChgTempLo2
	cEECellDcgTempHiAlmBackDef,		// wCellDcgTempHiAlmBack
	cEECellDcgTempHiAlmDef,			// wCellDcgTempHiAlm
	cEECellDcgTempHiDef,			// wCellDcgTempHi
	cEECellDcgTempHi2Def,			// wCellDcgTempHi2
	cEECellDcgTempLoAlmBackDef,		// wCellDcgTempLoAlmBack
	cEECellDcgTempLoAlmDef,			// wCellDcgTempLoAlm
	cEECellDcgTempLoDef,			// wCellDcgTempLo
	cEECellDcgTempLo2Def,			// wCellDcgTempLo2
	cEEHsTempHiAlmDef,				// wHsTempHiAlm
	cEEHsTempHiDef,					// wHsTempHi
	cEEBalanceEnDef,				// wBalanceEn
	cEEBalanceVoltErrDef,			// wBalanceVoltErr
	cEEBalanceVoltEnDef,			// wBalanceVoltEn
	cEEBalanceTempEnDef,			// wBalanceTempEn
	cEECellDcgDepthDef,				// wCellDcgDepth
	cEECellTempCoefDef,				// wCellTempCoef
	cEEDynamicRsvd2Def,				// wReserved2
	cEEFaultRstCntDef,				// wFaultRstCnt
	cEEFaultClrTimeDef,				// wFaultClrTime
	cEEIdleExitTimeDef,				// wIdleExitTime
	cEESleepExitTimeDef,			// wSleepExitTime
	cEEBreakWireModeDef,			// wBreakWireMode
	cEEBackLightEnableDef,			// wBackLightEn
	cEEMenuRtnEnableDef,			// wMenuRtnEn
	cEEFaultRecordEnableDef,		// wFaultRecordEn
	cEECellTempShieldDef,			// wCellTempShield
	cEECellVoltPct20TwoDef, 		// 
	{								// wReserved[2]
		cEERsvdDef,
		cEERsvdDef
	},
	cEERandomNumDef,				// wRandomNum
	
	0								// wCheckSum
};

const union {
	struct	STR_REALTIME_EEPROM	Field;
	INT16U	Table[cEepromRealtimeTotalLength];
} cEepromRealtimeDefaultTable =
{
	cEepromRealtimeTotalLength,		// wLength
	
	cEEBattAhRealDef,				// wBattAhReal
	cEEBattAhMaxDef,				// wBattAhMax
	cEEBattFullCntDef,				// wBattFullCnt
	cEEBattEmptyCntDef,				// wBattEmptyCnt
	cEEBattCycCntDef,				// wBattCycCnt
	cEEBattChgAhLoDef,				// wBattChgAhLo
	cEEBattChgAhHiDef,				// wBattChgAhHi
	cEEBattChgWhLoDef,				// wBattChgWhLo
	cEEBattChgWhHiDef,				// wBattChgWhHi
	cEEBattChgTimeLoDef,			// wBattChgTimeLo
	cEEBattChgTimeHiDef,			// wBattChgTimeHi
	cEEBattDcgAhLoDef,				// wBattDcgAhLo
	cEEBattDcgAhHiDef,				// wBattDcgAhHi
	cEEBattDcgWhLoDef,				// wBattDcgWhLo
	cEEBattDcgWhHiDef,				// wBattDcgWhHi
	cEEBattDcgTimeLoDef,			// wBattDcgTimeLo
	cEEBattDcgTimeHiDef,			// wBattDcgTimeHi
	cEEBattRunTimeLoDef,			// wBattRunTimeLo
	cEEBattRunTimeHiDef,			// wBattRunTimeHi
	{								// wReserved[3]
		cEERsvdDef,
		cEERsvdDef,
		cEERsvdDef
	},
	
	0								// wCheckSum
};

const union {
	struct	STR_FR_EEPROM	Field;
	INT16U	Table[cEepromFrTotalLength];
} cEepromFrDefaultTable =
{
	cEepromFrTotalLength,			// wLength
	
	cEEFaultRecordDef,				// INT16U wFrMode;
	cEEFaultRecordDef,				// INT16U wFrAlarmCode;
	cEEFaultRecordDef,				// INT16U wFrFaultFlag;
	cEEFaultRecordDef,				// INT16U wFrAlarmFlag;
	cEEFaultRecordDef,				// INT16U wFrBmsStatus;
	cEEFaultRecordDef,				// INT16U wFrBattVolt;
	cEEFaultRecordDef,				// INT16U wFrBattCurr;
	cEEFaultRecordDef,				// INT16U wFrBattSoC;
	cEEFaultRecordDef,				// INT16U wFrBattSoH;
	cEEFaultRecordDef,				// INT16U wFrHsTemp;
	cEEFaultRecordDef,				// INT16U wFrBalanceTemp;
	cEEFaultRecordDef,				// INT16U wFrCellVolt1;
	cEEFaultRecordDef,				// INT16U wFrCellVolt2;
	cEEFaultRecordDef,				// INT16U wFrCellVolt3;
	cEEFaultRecordDef,				// INT16U wFrCellVolt4;
	cEEFaultRecordDef,				// INT16U wFrCellVolt5;
	cEEFaultRecordDef,				// INT16U wFrCellVolt6;
	cEEFaultRecordDef,				// INT16U wFrCellVolt7;
	cEEFaultRecordDef,				// INT16U wFrCellVolt8;
	cEEFaultRecordDef,				// INT16U wFrCellVolt9;
	cEEFaultRecordDef,				// INT16U wFrCellVolt10;
	cEEFaultRecordDef,				// INT16U wFrCellVolt11;
	cEEFaultRecordDef,				// INT16U wFrCellVolt12;
	cEEFaultRecordDef,				// INT16U wFrCellVolt13;
	cEEFaultRecordDef,				// INT16U wFrCellVolt14;
	cEEFaultRecordDef,				// INT16U wFrCellVolt15;
	cEEFaultRecordDef,				// INT16U wFrCellVolt16;
	cEEFaultRecordDef,				// INT16U wFrCellTemp1;
	cEEFaultRecordDef,				// INT16U wFrCellTemp2;
	cEEFaultRecordDef,				// INT16U wFrCellTemp3;
	cEEFaultRecordDef,				// INT16U wFrCellTemp4;
	{								// wReserved[7]
		cEERsvdDef,
		cEERsvdDef,
		cEERsvdDef,
		cEERsvdDef,
		cEERsvdDef,
		cEERsvdDef,
		cEERsvdDef
	},
	
	0								// wCheckSum
};

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
INT16U	VirtAddVarTab1[NumbOfVar1];
INT16U	VirtAddVarTab2[NumbOfVar2];
INT16U	VirtAddVarTab3[NumbOfVar3];
INT8U	bEepromTaskPrio[cEEPROM_TASK_NO_MAX];
INT8U	bEepromTaskEvent[cEEPROM_TASK_NO_MAX];

struct	STR_STATIC_EEPROM	StrEepromStatic;
struct	STR_DYNAMIC_EEPROM	StrEepromDynamic;
struct	STR_REALTIME_EEPROM	StrEepromRealtime;
struct	STR_FR_EEPROM		StrEepromFaultRecord;
struct	STR_STATIC_EEPROM	StrEepromStaticTemp;
struct	STR_DYNAMIC_EEPROM	StrEepromDynamicTemp;
struct	STR_REALTIME_EEPROM	StrEepromRealtimeTemp;
struct	STR_FR_EEPROM		StrEepromFaultRecordTemp;

union EEPROM_STATUS_TABLE
{
	struct EEPROM_STATUS_BIT_FEILD
	{
		// BIT0-3
		INT16U bRwEnable:1;
		INT16U bRwBusy:1;
		INT16U bSecureLock:1;
		INT16U bSecureLockRst:1;
		
		// BIT4-7
		INT16U bEEStaticMode:2;		// 00禁止,01需要保存,10延时计数,11执行保存
		INT16U bEEDynamicMode:2;	// 00禁止,01需要保存,10延时计数,11执行保存
		
		// BIT8-11
		INT16U bEERealtimeMode:2;	// 00禁止,01需要保存,10延时计数,11执行保存
		INT16U bEEFaultRecordMode:2;// 00禁止,01需要保存,10延时计数,11执行保存
		
		// BIT12-15
		INT16U bRsvd4:4;
	} bits;
	INT16U data;
} fEepromStatus;

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
INT8U	sbEepromRead(INT16U wAddr, INT16U wLength, INT16U *pwBuffer);
INT8U	sbEepromWrite(INT16U wAddr, INT16U wLength, INT16U *pwBuffer);
INT8U	sbEepromCheckSumChk(INT16U wAddr, INT16U wLength);
INT8U	sbEepromCheckSumChk2(INT16U *pwBuffer, INT16U wLength);
void	sEepromStaticDefaultWrite(const INT16U *Table, INT16U wIndex, INT16U wLen);
void	sEepromDynamicDefaultWrite(const INT16U *Table, INT16U wIndex, INT16U wLen);
void	sEepromRealtimeDefaultWrite(const INT16U *Table, INT16U wIndex, INT16U wLen);
void	sEepromFrDefaultWrite(const INT16U *Table, INT16U wIndex, INT16U wLen);

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void	sEepromInitial(void)
{
	INT16U	i;
	INT16U	*ptr1, *ptr2;
	
	fEepromStatus.data = 0x0000;
	sSetEepromSecureLock(true);
	sSetEepromRwEnable(true);
	
	ptr1 = (INT16U *)&StrEepromStatic.wLength;
	ptr2 = (INT16U *)&StrEepromStaticTemp.wLength;
	for(i = 0; i < cEepromStaticTotalLength; i++)
	{
		*ptr1 = 0;
		*ptr2 = 0;
		ptr1++;
		ptr2++;
	}
	
	ptr1 = (INT16U *)&StrEepromDynamic.wLength;
	ptr2 = (INT16U *)&StrEepromDynamicTemp.wLength;
	for(i = 0; i < cEepromDynamicTotalLength; i++)
	{
		*ptr1 = 0;
		*ptr2 = 0;
		ptr1++;
		ptr2++;
	}
	ptr1 = NULL;
	ptr2 = NULL;
	
	ptr1 = (INT16U *)&StrEepromRealtime.wLength;
	ptr2 = (INT16U *)&StrEepromRealtimeTemp.wLength;
	for(i = 0; i < cEepromRealtimeTotalLength; i++)
	{
		*ptr1 = 0;
		*ptr2 = 0;
		ptr1++;
		ptr2++;
	}
	ptr1 = NULL;
	ptr2 = NULL;
	
	ptr1 = (INT16U *)&StrEepromFaultRecord.wLength;
	ptr2 = (INT16U *)&StrEepromFaultRecordTemp.wLength;
	for(i = 0; i < cEepromFrTotalLength; i++)
	{
		*ptr1 = 0;
		*ptr2 = 0;
		ptr1++;
		ptr2++;
	}
	ptr1 = NULL;
	ptr2 = NULL;
	
	for(i = 0; i < NumbOfVar1; i++)
	{
		if(i < cEepromStaticTotalLength)
		{
			VirtAddVarTab1[i] = cEepromStaticStartAddr + i;
		}
		else if(i < (cEepromStaticTotalLength + cEepromStaticTotalLength))
		{
			VirtAddVarTab1[i] = cEepromStaticStartAddr2 + i - cEepromStaticTotalLength;
		}
		else if(i < (cEepromStaticTotalLength + cEepromStaticTotalLength + cEepromDynamicTotalLength))
		{
			VirtAddVarTab1[i] = cEepromDynamicStartAddr + i - cEepromStaticTotalLength - cEepromStaticTotalLength;
		}
		else if(i < (cEepromStaticTotalLength + cEepromStaticTotalLength + cEepromDynamicTotalLength + cEepromDynamicTotalLength))
		{
			VirtAddVarTab1[i] = cEepromDynamicStartAddr2 + i - cEepromStaticTotalLength - cEepromStaticTotalLength - cEepromDynamicTotalLength;
		}
	}
	
	for(i = 0; i < NumbOfVar2; i++)
	{
		if(i < cEepromRealtimeTotalLength)
		{
			VirtAddVarTab2[i] = cEepromRealtimeStartAddr + i;
		}
		else if(i < (cEepromRealtimeTotalLength + cEepromFrTotalLength))
		{
			VirtAddVarTab2[i] = cEepromFrStartAddr + i - cEepromRealtimeTotalLength;
		}
	}
	
	for(i = 0; i < NumbOfVar3; i++)
	{
		if(i < cEepromRealtimeTotalLength)
		{
			VirtAddVarTab3[i] = cEepromRealtimeStartAddr2 + i;
		}
		else if(i < (cEepromRealtimeTotalLength + cEepromFrTotalLength))
		{
			VirtAddVarTab3[i] = cEepromFrStartAddr2 + i - cEepromRealtimeTotalLength;
		}
	}
	
	mFLASH_UNLOCK();
	EE1_Init();
	EE2_Init();
	EE3_Init();
	mFLASH_LOCK();
}

void	sEepromTaskInitial(INT8U bTask, INT8U bPrio, INT8U bEvent)
{
	bEepromTaskPrio[bTask] = bPrio;
	bEepromTaskEvent[bTask] = bEvent;
}

INT8U	sbEepromStaticPop(void)
{
    INT16U  wEepromDefaultIndex = 0;
    INT16U  wEepromDefaultLen = cEepromStaticTotalLength - 1;
	
	if(sbEepromRead(cEepromStaticStartAddr, 1, (INT16U *)&StrEepromStatic.wLength) == cEepromSuccess)
	{
		if((StrEepromStatic.wLength >= 3) && (StrEepromStatic.wLength <= 0x0100))
		{
			if(sbEepromRead(cEepromStaticStartAddr, StrEepromStatic.wLength, (INT16U *)&StrEepromStatic.wLength) == cEepromSuccess)
			{
				if(sbEepromCheckSumChk2((INT16U *)&StrEepromStatic.wLength, StrEepromStatic.wLength) == true)
				{
					if(StrEepromStatic.wLength < cEepromStaticTotalLength)
					{
						wEepromDefaultIndex = StrEepromStatic.wLength - 1;
						wEepromDefaultLen = cEepromStaticTotalLength - StrEepromStatic.wLength;
						StrEepromStatic.wLength = cEepromStaticTotalLength;
					}
					else if(StrEepromStatic.wLength > cEepromStaticTotalLength)
					{
						wEepromDefaultIndex = 0;
						wEepromDefaultLen = cEepromStaticTotalLength - 1;
					}
					else
					{
						return false;
					}
				}
			}
		}
	}
	
	// 读备用地址
	if(sbEepromRead(cEepromStaticStartAddr2, 1, (INT16U *)&StrEepromStatic.wLength) == cEepromSuccess)
	{
		if((StrEepromStatic.wLength >= 3) && (StrEepromStatic.wLength <= 0x0100))
		{
			if(sbEepromRead(cEepromStaticStartAddr2, StrEepromStatic.wLength, (INT16U *)&StrEepromStatic.wLength) == cEepromSuccess)
			{
				if(sbEepromCheckSumChk2((INT16U *)&StrEepromStatic.wLength, StrEepromStatic.wLength) == true)
				{
					if(StrEepromStatic.wLength < cEepromStaticTotalLength)
					{
						wEepromDefaultIndex = StrEepromStatic.wLength - 1;
						wEepromDefaultLen = cEepromStaticTotalLength - StrEepromStatic.wLength;
						StrEepromStatic.wLength = cEepromStaticTotalLength;
					}
					else if(StrEepromStatic.wLength > cEepromStaticTotalLength)
					{
						wEepromDefaultIndex = 0;
						wEepromDefaultLen = cEepromStaticTotalLength - 1;
					}
					else
					{
						return false;
					}
				}
			}
		}
	}
	
    sEepromStaticDefaultWrite(cEepromStaticDefaultTable.Table, wEepromDefaultIndex, wEepromDefaultLen);
	return true;
}

INT8U	sbEepromDynamicPop(void)
{
    INT16U  wEepromDefaultIndex = 0;
    INT16U  wEepromDefaultLen = cEepromDynamicTotalLength - 1;
	
	if(sbEepromRead(cEepromDynamicStartAddr, 1, (INT16U *)&StrEepromDynamic.wLength) == cEepromSuccess)
	{
		if((StrEepromDynamic.wLength >= 3) && (StrEepromDynamic.wLength <= 0x0100))
		{
			if(sbEepromRead(cEepromDynamicStartAddr, StrEepromDynamic.wLength, (INT16U *)&StrEepromDynamic.wLength) == cEepromSuccess)
			{
				if(sbEepromCheckSumChk2((INT16U *)&StrEepromDynamic.wLength, StrEepromDynamic.wLength) == true)
				{
					if(StrEepromDynamic.wLength < cEepromDynamicTotalLength)
					{
						wEepromDefaultIndex = StrEepromDynamic.wLength - 1;
						wEepromDefaultLen = cEepromDynamicTotalLength - StrEepromDynamic.wLength;
						StrEepromDynamic.wLength = cEepromDynamicTotalLength;
					}
					else if(StrEepromDynamic.wLength > cEepromDynamicTotalLength)
					{
						wEepromDefaultIndex = 0;
						wEepromDefaultLen = cEepromDynamicTotalLength - 1;
					}
					else
					{
						return false;
					}
				}
			}
		}
	}
	
	// 读备用地址
	if(sbEepromRead(cEepromDynamicStartAddr2, 1, (INT16U *)&StrEepromDynamic.wLength) == cEepromSuccess)
	{
		if((StrEepromDynamic.wLength >= 3) && (StrEepromDynamic.wLength <= 0x0100))
		{
			if(sbEepromRead(cEepromDynamicStartAddr2, StrEepromDynamic.wLength, (INT16U *)&StrEepromDynamic.wLength) == cEepromSuccess)
			{
				if(sbEepromCheckSumChk2((INT16U *)&StrEepromDynamic.wLength, StrEepromDynamic.wLength) == true)
				{
					if(StrEepromDynamic.wLength < cEepromDynamicTotalLength)
					{
						wEepromDefaultIndex = StrEepromDynamic.wLength - 1;
						wEepromDefaultLen = cEepromDynamicTotalLength - StrEepromDynamic.wLength;
						StrEepromDynamic.wLength = cEepromDynamicTotalLength;
					}
					else if(StrEepromDynamic.wLength > cEepromDynamicTotalLength)
					{
						wEepromDefaultIndex = 0;
						wEepromDefaultLen = cEepromDynamicTotalLength - 1;
					}
					else
					{
						return false;
					}
				}
			}
		}
	}
	
    sEepromDynamicDefaultWrite(cEepromDynamicDefaultTable.Table, wEepromDefaultIndex, wEepromDefaultLen);
	return true;
}

INT8U	sbEepromRealtimePop(void)
{
    INT16U  wEepromDefaultIndex = 0;
    INT16U  wEepromDefaultLen = cEepromRealtimeTotalLength - 1;
	
	if(sbEepromRead(cEepromRealtimeStartAddr, 1, (INT16U *)&StrEepromRealtime.wLength) == cEepromSuccess)
	{
		if((StrEepromRealtime.wLength >= 3) && (StrEepromRealtime.wLength <= 0x0100))
		{
			if(sbEepromRead(cEepromRealtimeStartAddr, StrEepromRealtime.wLength, (INT16U *)&StrEepromRealtime.wLength) == cEepromSuccess)
			{
				if(sbEepromCheckSumChk2((INT16U *)&StrEepromRealtime.wLength, StrEepromRealtime.wLength) == true)
				{
					if(StrEepromRealtime.wLength < cEepromRealtimeTotalLength)
					{
						wEepromDefaultIndex = StrEepromRealtime.wLength - 1;
						wEepromDefaultLen = cEepromRealtimeTotalLength - StrEepromRealtime.wLength;
						StrEepromRealtime.wLength = cEepromRealtimeTotalLength;
					}
					else if(StrEepromRealtime.wLength > cEepromRealtimeTotalLength)
					{
						wEepromDefaultIndex = 0;
						wEepromDefaultLen = cEepromRealtimeTotalLength - 1;
					}
					else
					{
						return false;
					}
				}
			}
		}
	}
	
	// 读备用地址
	if(sbEepromRead(cEepromRealtimeStartAddr2, 1, (INT16U *)&StrEepromRealtime.wLength) == cEepromSuccess)
	{
		if((StrEepromRealtime.wLength >= 3) && (StrEepromRealtime.wLength <= 0x0100))
		{
			if(sbEepromRead(cEepromRealtimeStartAddr2, StrEepromRealtime.wLength, (INT16U *)&StrEepromRealtime.wLength) == cEepromSuccess)
			{
				if(sbEepromCheckSumChk2((INT16U *)&StrEepromRealtime.wLength, StrEepromRealtime.wLength) == true)
				{
					if(StrEepromRealtime.wLength < cEepromRealtimeTotalLength)
					{
						wEepromDefaultIndex = StrEepromRealtime.wLength - 1;
						wEepromDefaultLen = cEepromRealtimeTotalLength - StrEepromRealtime.wLength;
						StrEepromRealtime.wLength = cEepromRealtimeTotalLength;
					}
					else if(StrEepromRealtime.wLength > cEepromRealtimeTotalLength)
					{
						wEepromDefaultIndex = 0;
						wEepromDefaultLen = cEepromRealtimeTotalLength - 1;
					}
					else
					{
						return false;
					}
				}
			}
		}
	}
	
    sEepromRealtimeDefaultWrite(cEepromRealtimeDefaultTable.Table, wEepromDefaultIndex, wEepromDefaultLen);
	return true;
}

INT8U	sbEepromFaultRecordPop(void)
{
    INT16U  wEepromDefaultIndex = 0;
    INT16U  wEepromDefaultLen = cEepromFrTotalLength - 1;
	
	if(sbEepromRead(cEepromFrStartAddr, 1, (INT16U *)&StrEepromFaultRecord.wLength) == cEepromSuccess)
	{
		if((StrEepromFaultRecord.wLength >= 3) && (StrEepromFaultRecord.wLength <= 0x0100))
		{
			if(sbEepromRead(cEepromFrStartAddr, StrEepromFaultRecord.wLength, (INT16U *)&StrEepromFaultRecord.wLength) == cEepromSuccess)
			{
				if(sbEepromCheckSumChk2((INT16U *)&StrEepromFaultRecord.wLength, StrEepromFaultRecord.wLength) == true)
				{
					if(StrEepromFaultRecord.wLength < cEepromFrTotalLength)
					{
						wEepromDefaultIndex = StrEepromFaultRecord.wLength - 1;
						wEepromDefaultLen = cEepromFrTotalLength - StrEepromFaultRecord.wLength;
						StrEepromFaultRecord.wLength = cEepromFrTotalLength;
					}
					else if(StrEepromFaultRecord.wLength > cEepromFrTotalLength)
					{
						wEepromDefaultIndex = 0;
						wEepromDefaultLen = cEepromFrTotalLength - 1;
					}
					else
					{
						return false;
					}
				}
			}
		}
	}
	
	// 读备用地址
	if(sbEepromRead(cEepromFrStartAddr2, 1, (INT16U *)&StrEepromFaultRecord.wLength) == cEepromSuccess)
	{
		if((StrEepromFaultRecord.wLength >= 3) && (StrEepromFaultRecord.wLength <= 0x0100))
		{
			if(sbEepromRead(cEepromFrStartAddr2, StrEepromFaultRecord.wLength, (INT16U *)&StrEepromFaultRecord.wLength) == cEepromSuccess)
			{
				if(sbEepromCheckSumChk2((INT16U *)&StrEepromFaultRecord.wLength, StrEepromFaultRecord.wLength) == true)
				{
					if(StrEepromFaultRecord.wLength < cEepromFrTotalLength)
					{
						wEepromDefaultIndex = StrEepromFaultRecord.wLength - 1;
						wEepromDefaultLen = cEepromFrTotalLength - StrEepromFaultRecord.wLength;
						StrEepromFaultRecord.wLength = cEepromFrTotalLength;
					}
					else if(StrEepromFaultRecord.wLength > cEepromFrTotalLength)
					{
						wEepromDefaultIndex = 0;
						wEepromDefaultLen = cEepromFrTotalLength - 1;
					}
					else
					{
						return false;
					}
				}
			}
		}
	}
	
    sEepromFrDefaultWrite(cEepromFrDefaultTable.Table, wEepromDefaultIndex, wEepromDefaultLen);
	return true;
}

void	sEepromStaticSave(void)
{
	INT16U	i, wCheckSum;
	INT16U	wAddr, wData;
	INT16U	*pbTemp1, *pbTemp2;
	INT8U	bFlag = 0;
	
	// COPY DATA
	pbTemp1 = (INT16U *)&StrEepromStatic.wLength;
	pbTemp2 = (INT16U *)&StrEepromStaticTemp.wLength;
	for(i = 0; i < cEepromStaticTotalLength - 1; i++)
	{
		*pbTemp2 = *pbTemp1;
		pbTemp1++;
		pbTemp2++;
	}
	
	// CHECK SUM
	wCheckSum = 0;
	pbTemp2 = (INT16U *)&StrEepromStaticTemp.wLength;
	for(i = 0; i < cEepromStaticTotalLength - 1; i++)
	{
		wCheckSum += *pbTemp2;
		pbTemp2++;
	}
	StrEepromStaticTemp.wCheckSum = wCheckSum;
	StrEepromStatic.wCheckSum = wCheckSum;
	
	// WRITE PROC
	wCheckSum = ~StrEepromStaticTemp.wCheckSum;
	i = 3;
	do
	{
		pbTemp2 = (INT16U *)&StrEepromStaticTemp.wLength;
		for(wAddr = cEepromStaticStartAddr; wAddr < (cEepromStaticStartAddr + cEepromStaticTotalLength); wAddr++)
		{
			if(sbEepromRead(wAddr, 1, (INT16U *)&wData) == cEepromSuccess)
			{
				if(*pbTemp2 != wData)
				{
					if(sbEepromWrite(wAddr, 1, pbTemp2) == cEepromSuccess)
					{
						;
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				if(sbEepromWrite(wAddr, 1, pbTemp2) == cEepromSuccess)
				{
					;
				}
				else
				{
					break;
				}
			}
			pbTemp2++;
		}
		if(sbEepromCheckSumChk(cEepromStaticStartAddr, StrEepromStaticTemp.wLength) == true)
		{
			bFlag++;
			break;
		}
	}while(--i != 0);
	
	// 备份
	i = 3;
	do
	{
		pbTemp2 = (INT16U *)&StrEepromStaticTemp.wLength;
		for(wAddr = cEepromStaticStartAddr2; wAddr < (cEepromStaticStartAddr2 + cEepromStaticTotalLength); wAddr++)
		{
			if(sbEepromRead(wAddr, 1, (INT16U *)&wData) == cEepromSuccess)
			{
				if(*pbTemp2 != wData)
				{
					if(sbEepromWrite(wAddr, 1, pbTemp2) == cEepromSuccess)
					{
						;
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				if(sbEepromWrite(wAddr, 1, pbTemp2) == cEepromSuccess)
				{
					;
				}
				else
				{
					break;
				}
			}
			pbTemp2++;
		}
		if(sbEepromCheckSumChk(cEepromStaticStartAddr2, StrEepromStaticTemp.wLength) == true)
		{
			bFlag++;
			break;
		}
	}while(--i != 0);
	
	if(bFlag == 0)
	{
		sSetBmsNoticeFlag(cEepromRwAlarm);
	}
	else
	{
		sClrBmsNoticeFlag(cEepromRwAlarm);
	}
}

void	sEepromDynamicSave(void)
{
	INT16U	i, wCheckSum;
	INT16U	wAddr, wData;
	INT16U	*pbTemp1, *pbTemp2;
	INT8U	bFlag = 0;
	
	// COPY DATA
	pbTemp1 = (INT16U *)&StrEepromDynamic.wLength;
	pbTemp2 = (INT16U *)&StrEepromDynamicTemp.wLength;
	for(i = 0; i < cEepromDynamicTotalLength - 1; i++)
	{
		*pbTemp2 = *pbTemp1;
		pbTemp1++;
		pbTemp2++;
	}
	
	// CHECK SUM
	wCheckSum = 0;
	pbTemp2 = (INT16U *)&StrEepromDynamicTemp.wLength;
	for(i = 0; i < cEepromDynamicTotalLength - 1; i++)
	{
		wCheckSum += *pbTemp2;
		pbTemp2++;
	}
	StrEepromDynamicTemp.wCheckSum = wCheckSum;
	StrEepromDynamic.wCheckSum = wCheckSum;
	
	// WRITE PROC
	wCheckSum = ~StrEepromDynamicTemp.wCheckSum;
	i = 3;
	do
	{
		pbTemp2 = (INT16U *)&StrEepromDynamicTemp.wLength;
		for(wAddr = cEepromDynamicStartAddr; wAddr < (cEepromDynamicStartAddr + cEepromDynamicTotalLength); wAddr++)
		{
			if(sbEepromRead(wAddr, 1, (INT16U *)&wData) == cEepromSuccess)
			{
				if(*pbTemp2 != wData)
				{
					if(sbEepromWrite(wAddr, 1, pbTemp2) == cEepromSuccess)
					{
						;
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				if(sbEepromWrite(wAddr, 1, pbTemp2) == cEepromSuccess)
				{
					;
				}
				else
				{
					break;
				}
			}
			pbTemp2++;
		}
		if(sbEepromCheckSumChk(cEepromDynamicStartAddr, StrEepromDynamicTemp.wLength) == true)
		{
			bFlag++;
			break;
		}
	}while(--i != 0);
	
	// 备份
	i = 3;
	do
	{
		pbTemp2 = (INT16U *)&StrEepromDynamicTemp.wLength;
		for(wAddr = cEepromDynamicStartAddr2; wAddr < (cEepromDynamicStartAddr2 + cEepromDynamicTotalLength); wAddr++)
		{
			if(sbEepromRead(wAddr, 1, (INT16U *)&wData) == cEepromSuccess)
			{
				if(*pbTemp2 != wData)
				{
					if(sbEepromWrite(wAddr, 1, pbTemp2) == cEepromSuccess)
					{
						;
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				if(sbEepromWrite(wAddr, 1, pbTemp2) == cEepromSuccess)
				{
					;
				}
				else
				{
					break;
				}
			}
			pbTemp2++;
		}
		if(sbEepromCheckSumChk(cEepromDynamicStartAddr2, StrEepromDynamicTemp.wLength) == true)
		{
			bFlag++;
			break;
		}
	}while(--i != 0);
	
	if(bFlag == 0)
	{
		sSetBmsNoticeFlag(cEepromRwAlarm);
	}
	else
	{
		sClrBmsNoticeFlag(cEepromRwAlarm);
	}
}

void	sEepromRealtimeSave(void)
{
	INT16U	i, wCheckSum;
	INT16U	wAddr, wData;
	INT16U	*pbTemp1, *pbTemp2;
	INT8U	bFlag = 0;
	
	// COPY DATA
	pbTemp1 = (INT16U *)&StrEepromRealtime.wLength;
	pbTemp2 = (INT16U *)&StrEepromRealtimeTemp.wLength;
	for(i = 0; i < cEepromRealtimeTotalLength - 1; i++)
	{
		*pbTemp2 = *pbTemp1;
		pbTemp1++;
		pbTemp2++;
	}
	
	// CHECK SUM
	wCheckSum = 0;
	pbTemp2 = (INT16U *)&StrEepromRealtimeTemp.wLength;
	for(i = 0; i < cEepromRealtimeTotalLength - 1; i++)
	{
		wCheckSum += *pbTemp2;
		pbTemp2++;
	}
	StrEepromRealtimeTemp.wCheckSum = wCheckSum;
	StrEepromRealtime.wCheckSum = wCheckSum;
	
	// WRITE PROC
	wCheckSum = ~StrEepromRealtimeTemp.wCheckSum;
	i = 3;
	do
	{
		pbTemp2 = (INT16U *)&StrEepromRealtimeTemp.wLength;
		for(wAddr = cEepromRealtimeStartAddr; wAddr < (cEepromRealtimeStartAddr + cEepromRealtimeTotalLength); wAddr++)
		{
			if(sbEepromRead(wAddr, 1, (INT16U *)&wData) == cEepromSuccess)
			{
				if(*pbTemp2 != wData)
				{
					if(sbEepromWrite(wAddr, 1, pbTemp2) == cEepromSuccess)
					{
						;
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				if(sbEepromWrite(wAddr, 1, pbTemp2) == cEepromSuccess)
				{
					;
				}
				else
				{
					break;
				}
			}
			pbTemp2++;
		}
		if(sbEepromCheckSumChk(cEepromRealtimeStartAddr, StrEepromRealtimeTemp.wLength) == true)
		{
			bFlag++;
			break;
		}
	}while(--i != 0);
	
	// 备份
	i = 3;
	do
	{
		pbTemp2 = (INT16U *)&StrEepromRealtimeTemp.wLength;
		for(wAddr = cEepromRealtimeStartAddr2; wAddr < (cEepromRealtimeStartAddr2 + cEepromRealtimeTotalLength); wAddr++)
		{
			if(sbEepromRead(wAddr, 1, (INT16U *)&wData) == cEepromSuccess)
			{
				if(*pbTemp2 != wData)
				{
					if(sbEepromWrite(wAddr, 1, pbTemp2) == cEepromSuccess)
					{
						;
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				if(sbEepromWrite(wAddr, 1, pbTemp2) == cEepromSuccess)
				{
					;
				}
				else
				{
					break;
				}
			}
			pbTemp2++;
		}
		if(sbEepromCheckSumChk(cEepromRealtimeStartAddr2, StrEepromRealtimeTemp.wLength) == true)
		{
			bFlag++;
			break;
		}
	}while(--i != 0);
	
	if(bFlag == 0)
	{
		sSetBmsNoticeFlag(cEepromRwAlarm);
	}
	else
	{
		sClrBmsNoticeFlag(cEepromRwAlarm);
	}
}

void	sEepromFaultRecordSave(void)
{
	INT16U	i, wCheckSum;
	INT16U	wAddr, wData;
	INT16U	*pbTemp1, *pbTemp2;
	INT8U	bFlag = 0;
	
	// COPY DATA
	pbTemp1 = (INT16U *)&StrEepromFaultRecord.wLength;
	pbTemp2 = (INT16U *)&StrEepromFaultRecordTemp.wLength;
	for(i = 0; i < cEepromFrTotalLength - 1; i++)
	{
		*pbTemp2 = *pbTemp1;
		pbTemp1++;
		pbTemp2++;
	}
	
	// CHECK SUM
	wCheckSum = 0;
	pbTemp2 = (INT16U *)&StrEepromFaultRecordTemp.wLength;
	for(i = 0; i < cEepromFrTotalLength - 1; i++)
	{
		wCheckSum += *pbTemp2;
		pbTemp2++;
	}
	StrEepromFaultRecordTemp.wCheckSum = wCheckSum;
	StrEepromFaultRecord.wCheckSum = wCheckSum;
	
	// WRITE PROC
	wCheckSum = ~StrEepromFaultRecordTemp.wCheckSum;
	i = 3;
	do
	{
		pbTemp2 = (INT16U *)&StrEepromFaultRecordTemp.wLength;
		for(wAddr = cEepromFrStartAddr; wAddr < (cEepromFrStartAddr + cEepromFrTotalLength); wAddr++)
		{
			if(sbEepromRead(wAddr, 1, (INT16U *)&wData) == cEepromSuccess)
			{
				if(*pbTemp2 != wData)
				{
					if(sbEepromWrite(wAddr, 1, pbTemp2) == cEepromSuccess)
					{
						;
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				if(sbEepromWrite(wAddr, 1, pbTemp2) == cEepromSuccess)
				{
					;
				}
				else
				{
					break;
				}
			}
			pbTemp2++;
		}
		if(sbEepromCheckSumChk(cEepromFrStartAddr, StrEepromFaultRecordTemp.wLength) == true)
		{
			bFlag++;
			break;
		}
	}while(--i != 0);
	
	// 备份
	i = 3;
	do
	{
		pbTemp2 = (INT16U *)&StrEepromFaultRecordTemp.wLength;
		for(wAddr = cEepromFrStartAddr2; wAddr < (cEepromFrStartAddr2 + cEepromFrTotalLength); wAddr++)
		{
			if(sbEepromRead(wAddr, 1, (INT16U *)&wData) == cEepromSuccess)
			{
				if(*pbTemp2 != wData)
				{
					if(sbEepromWrite(wAddr, 1, pbTemp2) == cEepromSuccess)
					{
						;
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				if(sbEepromWrite(wAddr, 1, pbTemp2) == cEepromSuccess)
				{
					;
				}
				else
				{
					break;
				}
			}
			pbTemp2++;
		}
		if(sbEepromCheckSumChk(cEepromFrStartAddr2, StrEepromFaultRecordTemp.wLength) == true)
		{
			bFlag++;
			break;
		}
	}while(--i != 0);
	
	if(bFlag == 0)
	{
		sSetBmsNoticeFlag(cEepromRwAlarm);
	}
	else
	{
		sClrBmsNoticeFlag(cEepromRwAlarm);
	}
}

INT8U	sbEepromRead(INT16U wAddr, INT16U wLength, INT16U *pwBuffer)
{
	INT8U	i;
	INT16U	wReadStatus = 0;
	INT16U	wAddrCurr = wAddr;
	INT16U*	pBuffCurr = pwBuffer;
	
	mFLASH_UNLOCK();
	for(i = 0; i < wLength; i++)
	{
		if(((wAddr >= cEepromStaticStartAddr) && (wAddr < (cEepromStaticStartAddr + 0x100))) \
			|| ((wAddr >= cEepromDynamicStartAddr) && (wAddr < (cEepromDynamicStartAddr + 0x100))))
		{
			wReadStatus = EE1_ReadVariable(wAddrCurr, pBuffCurr);
		}
		else if(((wAddr >= cEepromStaticStartAddr2) && (wAddr < (cEepromStaticStartAddr2 + 0x100))) \
			|| ((wAddr >= cEepromDynamicStartAddr2) && (wAddr < (cEepromDynamicStartAddr2 + 0x100))))
		{
			wReadStatus = EE1_ReadVariable(wAddrCurr, pBuffCurr);
		}
		else if(((wAddr >= cEepromRealtimeStartAddr) && (wAddr < (cEepromRealtimeStartAddr + 0x100))) \
			|| ((wAddr >= cEepromFrStartAddr) && (wAddr < (cEepromFrStartAddr + 0x100))))
		{
			wReadStatus = EE2_ReadVariable(wAddrCurr, pBuffCurr);
		}
		else if(((wAddr >= cEepromRealtimeStartAddr2) && (wAddr < (cEepromRealtimeStartAddr2 + 0x100))) \
			|| ((wAddr >= cEepromFrStartAddr2) && (wAddr < (cEepromFrStartAddr2 + 0x100))))
		{
			wReadStatus = EE3_ReadVariable(wAddrCurr, pBuffCurr);
		}
		if(wReadStatus != 0)
		{
			mFLASH_LOCK();
			return cEepromFail;
		}
		wAddrCurr++;
		pBuffCurr++;
	}
	
	mFLASH_LOCK();
	return cEepromSuccess;
}

INT8U	sbEepromWrite(INT16U wAddr, INT16U wLength, INT16U *pwBuffer)
{
	INT8U	i;
	INT16U	wReadStatus = 0;
	INT16U	wAddrCurr = wAddr;
	INT16U*	pBuffCurr = pwBuffer;
	
	mFLASH_UNLOCK();
	for(i = 0; i < wLength; i++)
	{
		if(((wAddr >= cEepromStaticStartAddr) && (wAddr < (cEepromStaticStartAddr + 0x100))) \
			|| ((wAddr >= cEepromDynamicStartAddr) && (wAddr < (cEepromDynamicStartAddr + 0x100))))
		{
			wReadStatus = EE1_WriteVariable(wAddrCurr, *pBuffCurr);
		}
		else if(((wAddr >= cEepromStaticStartAddr2) && (wAddr < (cEepromStaticStartAddr2 + 0x100))) \
			|| ((wAddr >= cEepromDynamicStartAddr2) && (wAddr < (cEepromDynamicStartAddr2 + 0x100))))
		{
			wReadStatus = EE1_WriteVariable(wAddrCurr, *pBuffCurr);
		}
		else if(((wAddr >= cEepromRealtimeStartAddr) && (wAddr < (cEepromRealtimeStartAddr + 0x100))) \
			|| ((wAddr >= cEepromFrStartAddr) && (wAddr < (cEepromFrStartAddr + 0x100))))
		{
			wReadStatus = EE2_WriteVariable(wAddrCurr, *pBuffCurr);
		}
		else if(((wAddr >= cEepromRealtimeStartAddr2) && (wAddr < (cEepromRealtimeStartAddr2 + 0x100))) \
			|| ((wAddr >= cEepromFrStartAddr2) && (wAddr < (cEepromFrStartAddr2 + 0x100))))
		{
			wReadStatus = EE3_WriteVariable(wAddrCurr, *pBuffCurr);
		}
		if(wReadStatus != cEepromSuccess)
		{
			mFLASH_LOCK();
			return cEepromFail;
		}
		wAddrCurr++;
		pBuffCurr++;
	}
	
	mFLASH_LOCK();
	return cEepromSuccess;
}

INT8U	sbEepromCheckSumChk(INT16U wAddr, INT16U wLength)
{
	INT16U	i;
	INT16U	wData;
	INT16U	wCheckSum;
	INT16U	wCheckSumTemp;
	INT16U	wAddrCurr = wAddr;
	INT16U	wDataLen = wLength - 1;
	
	wCheckSumTemp = 0;
	for(i = 0; i < wDataLen; i++)
	{
		sbEepromRead(wAddrCurr, 1, &wData);
		wAddrCurr++;
		wCheckSumTemp += wData;
	}
	
	sbEepromRead(wAddrCurr, 1, &wCheckSum);
	if(wCheckSum == wCheckSumTemp)
	{
		return(true);
	}
	else
	{
		return(false);
	}
}

INT8U	sbEepromCheckSumChk2(INT16U *pwBuffer, INT16U wLength)
{
	INT8U	i;
	INT16U	wCheckSum;
	INT16U*	pBuffCurr = pwBuffer;
	INT16U	wDataLen = wLength - 1;
	
	wCheckSum = 0;
	for(i = 0; i < wDataLen; i++)
	{
		wCheckSum += *pBuffCurr;
		pBuffCurr++;
	}
	if(wCheckSum == *pBuffCurr)
	{
		return(true);
	}
	else
	{
		return(false);
	}
}

void	sEepromStaticDefaultWrite(const INT16U *Table, INT16U wIndex, INT16U wLen)
{
	INT16U	i;
	INT16U	*pwTemp;
	
	pwTemp = (INT16U *)&StrEepromStatic.wLength;
	for(i = 0; i < wLen; i++)
	{
		*(pwTemp + wIndex) = Table[wIndex];
		wIndex++;
	}
}

void	sEepromDynamicDefaultWrite(const INT16U *Table, INT16U wIndex, INT16U wLen)
{
	INT16U	i;
	INT16U	*pwTemp;
	
	pwTemp = (INT16U *)&StrEepromDynamic.wLength;
	for(i = 0; i < wLen; i++)
	{
		*(pwTemp + wIndex) = Table[wIndex];
		wIndex++;
	}
}

void	sEepromRealtimeDefaultWrite(const INT16U *Table, INT16U wIndex, INT16U wLen)
{
	INT16U	i;
	INT16U	*pwTemp;
	
	pwTemp = (INT16U *)&StrEepromRealtime.wLength;
	for(i = 0; i < wLen; i++)
	{
		*(pwTemp + wIndex) = Table[wIndex];
		wIndex++;
	}
}

void	sEepromFrDefaultWrite(const INT16U *Table, INT16U wIndex, INT16U wLen)
{
	INT16U	i;
	INT16U	*pwTemp;
	
	pwTemp = (INT16U *)&StrEepromFaultRecord.wLength;
	for(i = 0; i < wLen; i++)
	{
		*(pwTemp + wIndex) = Table[wIndex];
		wIndex++;
	}
}

void	sEepromSecureLockClr(INT16U	wFilter)
{
	static INT16U wCnt = 0;
	
	if(fEepromStatus.bits.bSecureLockRst == true)
	{
		wCnt = 0;
		fEepromStatus.bits.bSecureLockRst = false;
	}
	else if(fEepromStatus.bits.bSecureLock == false)
	{
		if(++wCnt >= wFilter)
		{
			wCnt = 0;
			fEepromStatus.bits.bSecureLock = true;
		}
	}
}

INT8U	sbEepromStaticDataChk(void)
{
	INT16U wTemp;
	INT8U bEEStaticChangeFlag = false;
	INT8U i;
	
	wTemp = swGetEepromVer();
#if(cEEEepromVerLimitLo==0)
	if(wTemp > cEEEepromVerLimitUp)
#else
	if((wTemp < cEEEepromVerLimitLo) || (wTemp > cEEEepromVerLimitUp))
#endif
	{
		sSetEepromVer(cEEEepromVerDef);
		bEEStaticChangeFlag = true;
	}
	
	wTemp = swGetEepromSerialNo1();
#if(cEESerialNoLimitLo==0)
	if(wTemp > cEESerialNoLimitUp)
#else
	if((wTemp < cEESerialNoLimitLo) || (wTemp > cEESerialNoLimitUp))
#endif
	{
		sSetEepromSerialNo1(cEESerialNoDef);
		bEEStaticChangeFlag = true;
	}
	
	wTemp = swGetEepromSerialNo2();
#if(cEESerialNoLimitLo==0)
	if(wTemp > cEESerialNoLimitUp)
#else
	if((wTemp < cEESerialNoLimitLo) || (wTemp > cEESerialNoLimitUp))
#endif
	{
		sSetEepromSerialNo2(cEESerialNoDef);
		bEEStaticChangeFlag = true;
	}
	
	wTemp = swGetEepromSerialNo3();
#if(cEESerialNoLimitLo==0)
	if(wTemp > cEESerialNoLimitUp)
#else
	if((wTemp < cEESerialNoLimitLo) || (wTemp > cEESerialNoLimitUp))
#endif
	{
		sSetEepromSerialNo3(cEESerialNoDef);
		bEEStaticChangeFlag = true;
	}
	
	wTemp = swGetEepromSerialNo4();
#if(cEESerialNoLimitLo==0)
	if(wTemp > cEESerialNoLimitUp)
#else
	if((wTemp < cEESerialNoLimitLo) || (wTemp > cEESerialNoLimitUp))
#endif
	{
		sSetEepromSerialNo4(cEESerialNoDef);
		bEEStaticChangeFlag = true;
	}
	
	wTemp = swGetEepromSerialNo5();
#if(cEESerialNoLimitLo==0)
	if(wTemp > cEESerialNoLimitUp)
#else
	if((wTemp < cEESerialNoLimitLo) || (wTemp > cEESerialNoLimitUp))
#endif
	{
		sSetEepromSerialNo5(cEESerialNoDef);
		bEEStaticChangeFlag = true;
	}
	
	wTemp = swGetEepromSerialNoLen();
#if(cEESerialNoLenLimitLo==0)
	if(wTemp > cEESerialNoLenLimitUp)
#else
	if((wTemp < cEESerialNoLenLimitLo) || (wTemp > cEESerialNoLenLimitUp))
#endif
	{
		sSetEepromSerialNoLen(cEESerialNoLenDef);
		bEEStaticChangeFlag = true;
	}
	
	wTemp = swGetEepromBattVoltAdj();
#if(cEEBattVoltAdjLimitLo==0)
	if(wTemp > cEEBattVoltAdjLimitUp)
#else
	if((wTemp < cEEBattVoltAdjLimitLo) || (wTemp > cEEBattVoltAdjLimitUp))
#endif
	{
		sSetEepromBattVoltAdj(cEEBattVoltAdjDef);
		bEEStaticChangeFlag = true;
	}
	
	wTemp = swGetEepromBattVoltBias();
	if(((INT16S)wTemp < cEEBattVoltBiasLimitLo) || ((INT16S)wTemp > cEEBattVoltBiasLimitUp))
	{
		sSetEepromBattVoltBias(cEEBattVoltBiasDef);
		bEEStaticChangeFlag = true;
	}
	
	wTemp = swGetEepromBattChgCurrAdj();
#if(cEEBattCurrAdjLimitLo==0)
	if(wTemp > cEEBattCurrAdjLimitUp)
#else
	if((wTemp < cEEBattCurrAdjLimitLo) || (wTemp > cEEBattCurrAdjLimitUp))
#endif
	{
		sSetEepromBattChgCurrAdj(cEEBattCurrAdjDef);
		bEEStaticChangeFlag = true;
	}
	
	wTemp = swGetEepromBattChgCurrBias();
#if(cEEBattCurrBiasLimitLo==0)
	if((INT16S)wTemp > cEEBattCurrBiasLimitUp)
#else
	if(((INT16S)wTemp < cEEBattCurrBiasLimitLo) || ((INT16S)wTemp > cEEBattCurrBiasLimitUp))
#endif
	{
		sSetEepromBattChgCurrBias(cEEBattCurrBiasDef);
		bEEStaticChangeFlag = true;
	}
	
	wTemp = swGetEepromBattDsgCurrAdj();
#if(cEEBattCurrAdjLimitLo==0)
	if(wTemp > cEEBattCurrAdjLimitUp)
#else
	if((wTemp < cEEBattCurrAdjLimitLo) || (wTemp > cEEBattCurrAdjLimitUp))
#endif
	{
		sSetEepromBattDsgCurrAdj(cEEBattCurrAdjDef);
		bEEStaticChangeFlag = true;
	}
	
	wTemp = swGetEepromBattDsgCurrBias();
#if(cEEBattCurrBiasLimitLo==0)
	if((INT16S)wTemp > cEEBattCurrBiasLimitUp)
#else
	if(((INT16S)wTemp < cEEBattCurrBiasLimitLo) || ((INT16S)wTemp > cEEBattCurrBiasLimitUp))
#endif
	{
		sSetEepromBattDsgCurrBias(cEEBattCurrBiasDef);
		bEEStaticChangeFlag = true;
	}
	
	for(i = 0; i < 16; i++)
	{
		wTemp = swGetEepromCellVoltAdj(i);
	#if(cEECellVoltAdjLimitLo==0)
		if(wTemp > cEECellVoltAdjLimitUp)
	#else
		if((wTemp < cEECellVoltAdjLimitLo) || (wTemp > cEECellVoltAdjLimitUp))
	#endif
		{
			sSetEepromCellVoltAdj(i, cEECellVoltAdjDef);
			bEEStaticChangeFlag = true;
		}
		
		wTemp = swGetEepromCellVoltBias(i);
	#if(cEECellVoltBiasLimitLo==0)
		if((INT16S)wTemp > cEECellVoltBiasLimitUp)
	#else
		if(((INT16S)wTemp < cEECellVoltBiasLimitLo) || ((INT16S)wTemp > cEECellVoltBiasLimitUp))
	#endif
		{
			sSetEepromCellVoltBias(i, cEECellVoltBiasDef);
			bEEStaticChangeFlag = true;
		}
	}
	
	return bEEStaticChangeFlag;
}

INT8U	sbEepromDynamicDataChk(void)
{
	INT16U wTemp;
	INT8U bEEDynamicChangeFlag = false;
	
	wTemp = swGetEepromHacpMode();
#if(cEEHacpModeLimitLo==0)
	if(wTemp > cEEHacpModeLimitUp)
#else
	if((wTemp < cEEHacpModeLimitLo) || (wTemp > cEEHacpModeLimitUp))
#endif
	{
		sSetEepromHacpMode(cEEHacpModeDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromSlaveNum();
#if(cEESlaveNumLimitLo==0)
	if(wTemp > cEESlaveNumLimitUp)
#else
	if((wTemp < cEESlaveNumLimitLo) || (wTemp > cEESlaveNumLimitUp))
#endif
	{
		sSetEepromSlaveNum(cEESlaveNumDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromModuleAddr();
#if(cEEModuleAddrLimitLo==0)
	if(wTemp > cEEModuleAddrLimitUp)
#else
	if((wTemp < cEEModuleAddrLimitLo) || (wTemp > cEEModuleAddrLimitUp))
#endif
	{
		sSetEepromModuleAddr(cEEModuleAddrDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromModuleRes();
#if(cEEModuleResLimitLo==0)
	if(wTemp > cEEModuleResLimitUp)
#else
	if((wTemp < cEEModuleResLimitLo) || (wTemp > cEEModuleResLimitUp))
#endif
	{
		sSetEepromModuleRes(cEEModuleResDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellVoltPct80();
#if(cEECellVoltPct80LimitLo==0)
	if(wTemp > cEECellVoltPct80LimitUp)
#else
	if((wTemp < cEECellVoltPct80LimitLo) || (wTemp > cEECellVoltPct80LimitUp))
#endif
	{
		sSetEepromCellVoltPct80(cEECellVoltPct80Def);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellVoltPct20();
#if(cEECellVoltPct20LimitLo==0)
	if(wTemp > cEECellVoltPct20LimitUp)
#else
	if((wTemp < cEECellVoltPct20LimitLo) || (wTemp > cEECellVoltPct20LimitUp))
#endif
	{
		sSetEepromCellVoltPct20(cEECellVoltPct20Def);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellVoltHi2();
#if(cEECellVoltHi2LimitLo==0)
	if(wTemp > cEECellVoltHi2LimitUp)
#else
	if((wTemp < cEECellVoltHi2LimitLo) || (wTemp > cEECellVoltHi2LimitUp))
#endif
	{
		sSetEepromCellVoltHi2(cEECellVoltHi2Def);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellVoltLo2();
#if(cEECellVoltLo2LimitLo==0)
	if(wTemp > cEECellVoltLo2LimitUp)
#else
	if((wTemp < cEECellVoltLo2LimitLo) || (wTemp > cEECellVoltLo2LimitUp))
#endif
	{
		sSetEepromCellVoltLo2(cEECellVoltLo2Def);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellVoltHiAlm();
#if(cEECellVoltHiAlmLimitLo==0)
	if(wTemp > cEECellVoltHiAlmLimitUp)
#else
	if((wTemp < cEECellVoltHiAlmLimitLo) || (wTemp > cEECellVoltHiAlmLimitUp))
#endif
	{
		sSetEepromCellVoltHiAlm(cEECellVoltHiAlmDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellVoltHiAlmBack();
#if(cEECellVoltHiAlmBackLimitLo==0)
	if(wTemp > cEECellVoltHiAlmBackLimitUp)
#else
	if((wTemp < cEECellVoltHiAlmBackLimitLo) || (wTemp > cEECellVoltHiAlmBackLimitUp))
#endif
	{
		sSetEepromCellVoltHiAlmBack(cEECellVoltHiAlmBackDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellVoltHi();
#if(cEECellVoltHiLimitLo==0)
	if(wTemp > cEECellVoltHiLimitUp)
#else
	if((wTemp < cEECellVoltHiLimitLo) || (wTemp > cEECellVoltHiLimitUp))
#endif
	{
		sSetEepromCellVoltHi(cEECellVoltHiDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellVoltHiBack();
#if(cEECellVoltHiBackLimitLo==0)
	if(wTemp > cEECellVoltHiBackLimitUp)
#else
	if((wTemp < cEECellVoltHiBackLimitLo) || (wTemp > cEECellVoltHiBackLimitUp))
#endif
	{
		sSetEepromCellVoltHiBack(cEECellVoltHiBackDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellVoltLoAlm();
#if(cEECellVoltLoAlmLimitLo==0)
	if(wTemp > cEECellVoltLoAlmLimitUp)
#else
	if((wTemp < cEECellVoltLoAlmLimitLo) || (wTemp > cEECellVoltLoAlmLimitUp))
#endif
	{
		sSetEepromCellVoltLoAlm(cEECellVoltLoAlmDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellVoltLoAlmBack();
#if(cEECellVoltLoAlmBackLimitLo==0)
	if(wTemp > cEECellVoltLoAlmBackLimitUp)
#else
	if((wTemp < cEECellVoltLoAlmBackLimitLo) || (wTemp > cEECellVoltLoAlmBackLimitUp))
#endif
	{
		sSetEepromCellVoltLoAlmBack(cEECellVoltLoAlmBackDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellVoltLo();
#if(cEECellVoltLoLimitLo==0)
	if(wTemp > cEECellVoltLoLimitUp)
#else
	if((wTemp < cEECellVoltLoLimitLo) || (wTemp > cEECellVoltLoLimitUp))
#endif
	{
		sSetEepromCellVoltLo(cEECellVoltLoDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellVoltLoBack();
#if(cEECellVoltLoBackLimitLo==0)
	if(wTemp > cEECellVoltLoBackLimitUp)
#else
	if((wTemp < cEECellVoltLoBackLimitLo) || (wTemp > cEECellVoltLoBackLimitUp))
#endif
	{
		sSetEepromCellVoltLoBack(cEECellVoltLoBackDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromBattChgCurrHiAlmBack();
#if(cEEBattChgCurrHiAlmBackLimitLo==0)
	if(wTemp > cEEBattChgCurrHiAlmBackLimitUp)
#else
	if((wTemp < cEEBattChgCurrHiAlmBackLimitLo) || (wTemp > cEEBattChgCurrHiAlmBackLimitUp))
#endif
	{
		sSetEepromBattChgCurrHiAlmBack(cEEBattChgCurrHiAlmBackDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromBattChgCurrHiAlm();
#if(cEEBattChgCurrHiAlmLimitLo==0)
	if(wTemp > cEEBattChgCurrHiAlmLimitUp)
#else
	if((wTemp < cEEBattChgCurrHiAlmLimitLo) || (wTemp > cEEBattChgCurrHiAlmLimitUp))
#endif
	{
		sSetEepromBattChgCurrHiAlm(cEEBattChgCurrHiAlmDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromBattChgCurrHi();
#if(cEEBattChgCurrHiLimitLo==0)
	if(wTemp > cEEBattChgCurrHiLimitUp)
#else
	if((wTemp < cEEBattChgCurrHiLimitLo) || (wTemp > cEEBattChgCurrHiLimitUp))
#endif
	{
		sSetEepromBattChgCurrHi(cEEBattChgCurrHiDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromBattChgCurrHi2();
#if(cEEBattChgCurrHi2LimitLo==0)
	if(wTemp > cEEBattChgCurrHi2LimitUp)
#else
	if((wTemp < cEEBattChgCurrHi2LimitLo) || (wTemp > cEEBattChgCurrHi2LimitUp))
#endif
	{
		sSetEepromBattChgCurrHi2(cEEBattChgCurrHi2Def);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromBattDcgCurrHiAlmBack();
#if(cEEBattDcgCurrHiAlmBackLimitLo==0)
	if(wTemp > cEEBattDcgCurrHiAlmBackLimitUp)
#else
	if((wTemp < cEEBattDcgCurrHiAlmBackLimitLo) || (wTemp > cEEBattDcgCurrHiAlmBackLimitUp))
#endif
	{
		sSetEepromBattDcgCurrHiAlmBack(cEEBattDcgCurrHiAlmBackDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromBattDcgCurrHiAlm();
#if(cEEBattDcgCurrHiAlmLimitLo==0)
	if(wTemp > cEEBattDcgCurrHiAlmLimitUp)
#else
	if((wTemp < cEEBattDcgCurrHiAlmLimitLo) || (wTemp > cEEBattDcgCurrHiAlmLimitUp))
#endif
	{
		sSetEepromBattDcgCurrHiAlm(cEEBattDcgCurrHiAlmDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromBattDcgCurrHi();
#if(cEEBattDcgCurrHiLimitLo==0)
	if(wTemp > cEEBattDcgCurrHiLimitUp)
#else
	if((wTemp < cEEBattDcgCurrHiLimitLo) || (wTemp > cEEBattDcgCurrHiLimitUp))
#endif
	{
		sSetEepromBattDcgCurrHi(cEEBattDcgCurrHiDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromBattDcgCurrHi2();
#if(cEEBattDcgCurrHi2LimitLo==0)
	if(wTemp > cEEBattDcgCurrHi2LimitUp)
#else
	if((wTemp < cEEBattDcgCurrHi2LimitLo) || (wTemp > cEEBattDcgCurrHi2LimitUp))
#endif
	{
		sSetEepromBattDcgCurrHi2(cEEBattDcgCurrHi2Def);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellChgTempHiAlmBack();
#if(cEECellChgTempHiAlmBackLimitLo==0)
	if(wTemp > cEECellChgTempHiAlmBackLimitUp)
#else
	if((wTemp < cEECellChgTempHiAlmBackLimitLo) || (wTemp > cEECellChgTempHiAlmBackLimitUp))
#endif
	{
		sSetEepromCellChgTempHiAlmBack(cEECellChgTempHiAlmBackDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellChgTempHiAlm();
#if(cEECellChgTempHiAlmLimitLo==0)
	if(wTemp > cEECellChgTempHiAlmLimitUp)
#else
	if((wTemp < cEECellChgTempHiAlmLimitLo) || (wTemp > cEECellChgTempHiAlmLimitUp))
#endif
	{
		sSetEepromCellChgTempHiAlm(cEECellChgTempHiAlmDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellChgTempHi();
#if(cEECellChgTempHiLimitLo==0)
	if(wTemp > cEECellChgTempHiLimitUp)
#else
	if((wTemp < cEECellChgTempHiLimitLo) || (wTemp > cEECellChgTempHiLimitUp))
#endif
	{
		sSetEepromCellChgTempHi(cEECellChgTempHiDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellChgTempHi2();
#if(cEECellChgTempHi2LimitLo==0)
	if(wTemp > cEECellChgTempHi2LimitUp)
#else
	if((wTemp < cEECellChgTempHi2LimitLo) || (wTemp > cEECellChgTempHi2LimitUp))
#endif
	{
		sSetEepromCellChgTempHi2(cEECellChgTempHi2Def);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellChgTempLoAlmBack();
#if((INT16S)cEECellChgTempLoAlmBackLimitLo==0)
	if((INT16S)wTemp > (INT16S)cEECellChgTempLoAlmBackLimitUp)
#else
	if(((INT16S)wTemp < (INT16S)cEECellChgTempLoAlmBackLimitLo) || ((INT16S)wTemp > (INT16S)cEECellChgTempLoAlmBackLimitUp))
#endif
	{
		sSetEepromCellChgTempLoAlmBack((INT16S)cEECellChgTempLoAlmBackDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellChgTempLoAlm();
#if((INT16S)cEECellChgTempLoAlmLimitLo==0)
	if((INT16S)wTemp > (INT16S)cEECellChgTempLoAlmLimitUp)
#else
	if(((INT16S)wTemp < (INT16S)cEECellChgTempLoAlmLimitLo) || ((INT16S)wTemp > (INT16S)cEECellChgTempLoAlmLimitUp))
#endif
	{
		sSetEepromCellChgTempLoAlm((INT16S)cEECellChgTempLoAlmDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellChgTempLo();
#if((INT16S)cEECellChgTempLoLimitLo==0)
	if((INT16S)wTemp > (INT16S)cEECellChgTempLoLimitUp)
#else
	if(((INT16S)wTemp < (INT16S)cEECellChgTempLoLimitLo) || ((INT16S)wTemp > (INT16S)cEECellChgTempLoLimitUp))
#endif
	{
		sSetEepromCellChgTempLo((INT16S)cEECellChgTempLoDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellChgTempLo2();
#if((INT16S)cEECellChgTempLo2LimitLo==0)
	if((INT16S)wTemp > (INT16S)cEECellChgTempLo2LimitUp)
#else
	if(((INT16S)wTemp < (INT16S)cEECellChgTempLo2LimitLo) || ((INT16S)wTemp > (INT16S)cEECellChgTempLo2LimitUp))
#endif
	{
		sSetEepromCellChgTempLo2((INT16S)cEECellChgTempLo2Def);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellDcgTempHiAlmBack();
#if(cEECellDcgTempHiAlmBackLimitLo==0)
	if(wTemp > cEECellDcgTempHiAlmBackLimitUp)
#else
	if((wTemp < cEECellDcgTempHiAlmBackLimitLo) || (wTemp > cEECellDcgTempHiAlmBackLimitUp))
#endif
	{
		sSetEepromCellDcgTempHiAlmBack(cEECellDcgTempHiAlmBackDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellDcgTempHiAlm();
#if(cEECellDcgTempHiAlmLimitLo==0)
	if(wTemp > cEECellDcgTempHiAlmLimitUp)
#else
	if((wTemp < cEECellDcgTempHiAlmLimitLo) || (wTemp > cEECellDcgTempHiAlmLimitUp))
#endif
	{
		sSetEepromCellDcgTempHiAlm(cEECellDcgTempHiAlmDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellDcgTempHi();
#if(cEECellDcgTempHiLimitLo==0)
	if(wTemp > cEECellDcgTempHiLimitUp)
#else
	if((wTemp < cEECellDcgTempHiLimitLo) || (wTemp > cEECellDcgTempHiLimitUp))
#endif
	{
		sSetEepromCellDcgTempHi(cEECellDcgTempHiDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellDcgTempHi2();
#if(cEECellDcgTempHi2LimitLo==0)
	if(wTemp > cEECellDcgTempHi2LimitUp)
#else
	if((wTemp < cEECellDcgTempHi2LimitLo) || (wTemp > cEECellDcgTempHi2LimitUp))
#endif
	{
		sSetEepromCellDcgTempHi2(cEECellDcgTempHi2Def);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellDcgTempLoAlmBack();
#if((INT16S)cEECellDcgTempLoAlmBackLimitLo==0)
	if((INT16S)wTemp > (INT16S)cEECellDcgTempLoAlmBackLimitUp)
#else
	if(((INT16S)wTemp < (INT16S)cEECellDcgTempLoAlmBackLimitLo) || ((INT16S)wTemp > (INT16S)cEECellDcgTempLoAlmBackLimitUp))
#endif
	{
		sSetEepromCellDcgTempLoAlmBack((INT16S)cEECellDcgTempLoAlmBackDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellDcgTempLoAlm();
#if((INT16S)cEECellDcgTempLoAlmLimitLo==0)
	if((INT16S)wTemp > (INT16S)cEECellDcgTempLoAlmLimitUp)
#else
	if(((INT16S)wTemp < (INT16S)cEECellDcgTempLoAlmLimitLo) || ((INT16S)wTemp > (INT16S)cEECellDcgTempLoAlmLimitUp))
#endif
	{
		sSetEepromCellDcgTempLoAlm((INT16S)cEECellDcgTempLoAlmDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellDcgTempLo();
#if((INT16S)cEECellDcgTempLoLimitLo==0)
	if((INT16S)wTemp > (INT16S)cEECellDcgTempLoLimitUp)
#else
	if(((INT16S)wTemp < (INT16S)cEECellDcgTempLoLimitLo) || ((INT16S)wTemp > (INT16S)cEECellDcgTempLoLimitUp))
#endif
	{
		sSetEepromCellDcgTempLo((INT16S)cEECellDcgTempLoDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellDcgTempLo2();
#if((INT16S)cEECellDcgTempLo2LimitLo==0)
	if((INT16S)wTemp > (INT16S)cEECellDcgTempLo2LimitUp)
#else
	if(((INT16S)wTemp < (INT16S)cEECellDcgTempLo2LimitLo) || ((INT16S)wTemp > (INT16S)cEECellDcgTempLo2LimitUp))
#endif
	{
		sSetEepromCellDcgTempLo2((INT16S)cEECellDcgTempLo2Def);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromHsTempHiAlm();
#if(cEEHsTempHiAlmLimitLo==0)
	if(wTemp > cEEHsTempHiAlmLimitUp)
#else
	if((wTemp < cEEHsTempHiAlmLimitLo) || (wTemp > cEEHsTempHiAlmLimitUp))
#endif
	{
		sSetEepromHsTempHiAlm(cEEHsTempHiAlmDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromHsTempHi();
#if(cEEHsTempHiLimitLo==0)
	if(wTemp > cEEHsTempHiLimitUp)
#else
	if((wTemp < cEEHsTempHiLimitLo) || (wTemp > cEEHsTempHiLimitUp))
#endif
	{
		sSetEepromHsTempHi(cEEHsTempHiDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromBalanceEn();
#if(cEEBalanceEnLimitLo==0)
	if(wTemp > cEEBalanceEnLimitUp)
#else
	if((wTemp < cEEBalanceEnLimitLo) || (wTemp > cEEBalanceEnLimitUp))
#endif
	{
		sSetEepromBalanceEn(cEEBalanceEnDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromBalanceVoltErr();
#if(cEEBalanceVoltErrLimitLo==0)
	if(wTemp > cEEBalanceVoltErrLimitUp)
#else
	if((wTemp < cEEBalanceVoltErrLimitLo) || (wTemp > cEEBalanceVoltErrLimitUp))
#endif
	{
		sSetEepromBalanceVoltErr(cEEBalanceVoltErrDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromBalanceVoltEn();
#if(cEEBalanceVoltEnLimitLo==0)
	if(wTemp > cEEBalanceVoltEnLimitUp)
#else
	if((wTemp < cEEBalanceVoltEnLimitLo) || (wTemp > cEEBalanceVoltEnLimitUp))
#endif
	{
		sSetEepromBalanceVoltEn(cEEBalanceVoltEnDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromBalanceTempEn();
#if(cEEBalanceTempEnLimitLo==0)
	if(wTemp > cEEBalanceTempEnLimitUp)
#else
	if((wTemp < cEEBalanceTempEnLimitLo) || (wTemp > cEEBalanceTempEnLimitUp))
#endif
	{
		sSetEepromBalanceTempEn(cEEBalanceTempEnDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellDcgDepth();
#if(cEECellDcgDepthLimitLo==0)
	if(wTemp > cEECellDcgDepthLimitUp)
#else
	if((wTemp < cEECellDcgDepthLimitLo) || (wTemp > cEECellDcgDepthLimitUp))
#endif
	{
		sSetEepromCellDcgDepth(cEECellDcgDepthDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellTempCoef();
#if(cEECellTempCoefLimitLo==0)
	if(wTemp > cEECellTempCoefLimitUp)
#else
	if((wTemp < cEECellTempCoefLimitLo) || (wTemp > cEECellTempCoefLimitUp))
#endif
	{
		sSetEepromCellTempCoef(cEECellTempCoefDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromDynamicRsvd2();
#if(cEEDynamicRsvd2LimitLo==0)
	if(wTemp > cEEDynamicRsvd2LimitUp)
#else
	if((wTemp < cEEDynamicRsvd2LimitLo) || (wTemp > cEEDynamicRsvd2LimitUp))
#endif
	{
		sSetEepromDynamicRsvd2(cEEDynamicRsvd2Def);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromFaultRstCnt();
#if(cEEFaultRstCntLimitLo==0)
	if(wTemp > cEEFaultRstCntLimitUp)
#else
	if((wTemp < cEEFaultRstCntLimitLo) || (wTemp > cEEFaultRstCntLimitUp))
#endif
	{
		sSetEepromFaultRstCnt(cEEFaultRstCntDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromFaultClrTime();
#if(cEEFaultClrTimeLimitLo==0)
	if(wTemp > cEEFaultClrTimeLimitUp)
#else
	if((wTemp < cEEFaultClrTimeLimitLo) || (wTemp > cEEFaultClrTimeLimitUp))
#endif
	{
		sSetEepromFaultClrTime(cEEFaultClrTimeDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromIdleExitTime();
#if(cEEIdleExitTimeLimitLo==0)
	if(wTemp > cEEIdleExitTimeLimitUp)
#else
	if((wTemp < cEEIdleExitTimeLimitLo) || (wTemp > cEEIdleExitTimeLimitUp))
#endif
	{
		sSetEepromIdleExitTime(cEEIdleExitTimeDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromSleepExitTime();
#if(cEESleepExitTimeLimitLo==0)
	if(wTemp > cEESleepExitTimeLimitUp)
#else
	if((wTemp < cEESleepExitTimeLimitLo) || (wTemp > cEESleepExitTimeLimitUp))
#endif
	{
		sSetEepromSleepExitTime(cEESleepExitTimeDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromBreakWireMode();
#if(cEEBreakWireModeLimitLo==0)
	if(wTemp > cEEBreakWireModeLimitUp)
#else
	if((wTemp < cEEBreakWireModeLimitLo) || (wTemp > cEEBreakWireModeLimitUp))
#endif
	{
		sSetEepromBreakWireMode(cEEBreakWireModeDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromBackLightEn();
#if(cEEBackLightEnableLimitLo==0)
	if(wTemp > cEEBackLightEnableLimitUp)
#else
	if((wTemp < cEEBackLightEnableLimitLo) || (wTemp > cEEBackLightEnableLimitUp))
#endif
	{
		sSetEepromBackLightEn(cEEBackLightEnableDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromMenuRtnEn();
#if(cEEMenuRtnEnableLimitLo==0)
	if(wTemp > cEEMenuRtnEnableLimitUp)
#else
	if((wTemp < cEEMenuRtnEnableLimitLo) || (wTemp > cEEMenuRtnEnableLimitUp))
#endif
	{
		sSetEepromMenuRtnEn(cEEMenuRtnEnableDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromFaultRecordEn();
#if(cEEFaultRecordEnableLimitLo==0)
	if(wTemp > cEEFaultRecordEnableLimitUp)
#else
	if((wTemp < cEEFaultRecordEnableLimitLo) || (wTemp > cEEFaultRecordEnableLimitUp))
#endif
	{
		sSetEepromFaultRecordEn(cEEFaultRecordEnableDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellTempShield();
	if((wTemp != 0x0001) && (wTemp != 0x0002) && (wTemp != 0x0004) && (wTemp != 0x0008))
	{
		sSetEepromCellTempShield(cEECellTempShieldDef);
		bEEDynamicChangeFlag = true;
	}
	
	wTemp = swGetEepromCellVoltPct20Two();
#if(cEECellVoltPct20TwoLimitLo==0)
	if(wTemp > cEECellVoltPct20TwoLimitUp)
#else
	if((wTemp < cEECellVoltPct20TwoLimitLo) || (wTemp > cEECellVoltPct20TwoLimitUp))
#endif
	{
		sSetEepromCellVoltPct20Two(cEECellVoltPct20TwoDef);
		bEEDynamicChangeFlag = true;
	}
	
	return bEEDynamicChangeFlag;
}

INT8U	sbEepromRealtimeDataChk(void)
{
	//INT16U wTemp;
	INT8U bEERealtimeChangeFlag = false;
	/*
	if(swGetEepromBattAhReal() > swGetEepromBattRatedAH())
	{
		sSetEepromBattAhReal(swGetBmsRatedAH());
		bEERealtimeChangeFlag = true;
	}
	
	if(swGetEepromBattAhMax() > swGetEepromBattRatedAH())
	{
		sSetEepromBattAhMax(swGetEepromBattRatedAH());
		bEERealtimeChangeFlag = true;
	}
	
	if(swGetEepromBattAhReal() > swGetEepromBattAhMax())
	{
		sSetEepromBattAhReal(swGetEepromBattAhMax());
		bEERealtimeChangeFlag = true;
	}
	*/
	return bEERealtimeChangeFlag;
}

INT8U	sbEepromFaultRecordDataChk(void)
{
	//INT16U wTemp;
	INT8U bEEFaultRecordChangeFlag = false;
	
	return bEEFaultRecordChangeFlag;
}

void	sEepromStaticSaveMode(INT16U wFilter)
{
	static INT16U wDelay = 0;
	
	switch(fEepromStatus.bits.bEEStaticMode)
	{
		case cEEPROM_SAVE_MODE_DISABLE:
			
		break;
		case cEEPROM_SAVE_MODE_ENABLE:
			if(wFilter == 0)
			{
				fEepromStatus.bits.bEEStaticMode = cEEPROM_SAVE_MODE_ACTION;
			}
			else
			{
				wDelay = 0;
				fEepromStatus.bits.bEEStaticMode = cEEPROM_SAVE_MODE_DELAY;
			}
		break;
		case cEEPROM_SAVE_MODE_DELAY:
			if(++wDelay >= wFilter)
			{
				wDelay = 0;
				fEepromStatus.bits.bEEStaticMode = cEEPROM_SAVE_MODE_ACTION;
			}
		break;
		case cEEPROM_SAVE_MODE_ACTION:
			OSEventSend(bEepromTaskPrio[cEEPROM_STATIC_NO], bEepromTaskEvent[cEEPROM_STATIC_NO]);
			fEepromStatus.bits.bEEStaticMode = cEEPROM_SAVE_MODE_DISABLE;
		break;
		default:
			fEepromStatus.bits.bEEStaticMode = cEEPROM_SAVE_MODE_DISABLE;
		break;
	}
}

void	sEepromDynamicSaveMode(INT16U wFilter)
{
	static INT16U wDelay = 0;
	
	switch(fEepromStatus.bits.bEEDynamicMode)
	{
		case cEEPROM_SAVE_MODE_DISABLE:
			
		break;
		case cEEPROM_SAVE_MODE_ENABLE:
			if(wFilter == 0)
			{
				fEepromStatus.bits.bEEDynamicMode = cEEPROM_SAVE_MODE_ACTION;
			}
			else
			{
				wDelay = 0;
				fEepromStatus.bits.bEEDynamicMode = cEEPROM_SAVE_MODE_DELAY;
			}
		break;
		case cEEPROM_SAVE_MODE_DELAY:
			if(++wDelay >= wFilter)
			{
				wDelay = 0;
				fEepromStatus.bits.bEEDynamicMode = cEEPROM_SAVE_MODE_ACTION;
			}
		break;
		case cEEPROM_SAVE_MODE_ACTION:
			OSEventSend(bEepromTaskPrio[cEEPROM_DYNAMIC_NO], bEepromTaskEvent[cEEPROM_DYNAMIC_NO]);
			fEepromStatus.bits.bEEDynamicMode = cEEPROM_SAVE_MODE_DISABLE;
		break;
		default:
			fEepromStatus.bits.bEEDynamicMode = cEEPROM_SAVE_MODE_DISABLE;
		break;
	}
}

void	sEepromRealtimeSaveMode(INT16U wFilter)
{
	static INT16U wDelay = 0;
	
	switch(fEepromStatus.bits.bEERealtimeMode)
	{
		case cEEPROM_SAVE_MODE_DISABLE:
			
		break;
		case cEEPROM_SAVE_MODE_ENABLE:
			if(wFilter == 0)
			{
				fEepromStatus.bits.bEERealtimeMode = cEEPROM_SAVE_MODE_ACTION;
			}
			else
			{
				wDelay = 0;
				fEepromStatus.bits.bEERealtimeMode = cEEPROM_SAVE_MODE_DELAY;
			}
		break;
		case cEEPROM_SAVE_MODE_DELAY:
			if(++wDelay >= wFilter)
			{
				wDelay = 0;
				fEepromStatus.bits.bEERealtimeMode = cEEPROM_SAVE_MODE_ACTION;
			}
		break;
		case cEEPROM_SAVE_MODE_ACTION:
			OSEventSend(bEepromTaskPrio[cEEPROM_REALTIME_NO], bEepromTaskEvent[cEEPROM_REALTIME_NO]);
			fEepromStatus.bits.bEERealtimeMode = cEEPROM_SAVE_MODE_DISABLE;
		break;
		default:
			fEepromStatus.bits.bEERealtimeMode = cEEPROM_SAVE_MODE_DISABLE;
		break;
	}
}

void	sEepromFaultRecordSaveMode(INT16U wFilter)
{
	static INT16U wDelay = 0;
	
	switch(fEepromStatus.bits.bEEFaultRecordMode)
	{
		case cEEPROM_SAVE_MODE_DISABLE:
			
		break;
		case cEEPROM_SAVE_MODE_ENABLE:
			if(wFilter == 0)
			{
				fEepromStatus.bits.bEERealtimeMode = cEEPROM_SAVE_MODE_ACTION;
			}
			else
			{
				wDelay = 0;
				fEepromStatus.bits.bEEFaultRecordMode = cEEPROM_SAVE_MODE_DELAY;
			}
		break;
		case cEEPROM_SAVE_MODE_DELAY:
			if(++wDelay >= wFilter)
			{
				wDelay = 0;
				fEepromStatus.bits.bEEFaultRecordMode = cEEPROM_SAVE_MODE_ACTION;
			}
		break;
		case cEEPROM_SAVE_MODE_ACTION:
			OSEventSend(bEepromTaskPrio[cEEPROM_FAULT_RECORD_NO], bEepromTaskEvent[cEEPROM_FAULT_RECORD_NO]);
			fEepromStatus.bits.bEEFaultRecordMode = cEEPROM_SAVE_MODE_DISABLE;
		break;
		default:
			fEepromStatus.bits.bEEFaultRecordMode = cEEPROM_SAVE_MODE_DISABLE;
		break;
	}
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
INT8U	sbGetEepromRwEnable(void)
{
	return(fEepromStatus.bits.bRwEnable);
}

INT8U	sbGetEepromRwBusy(void)
{
	return(fEepromStatus.bits.bRwBusy);
}

INT8U	sbGetEepromSecureLock(void)
{
	return(fEepromStatus.bits.bSecureLock);
}

INT16U	swGetEepromStaticMapData(INT16U wAddrOffset)
{
	INT16U	*pwTemp;
	
	if(wAddrOffset < cEepromStaticTotalLength)
	{
		pwTemp = (INT16U *)&StrEepromStatic.wLength;
		pwTemp += wAddrOffset;
		return(*pwTemp);
	}
	else
	{
		return 0xFFFF;
	}
}

INT16U	swGetEepromDynamicMapData(INT16U wAddrOffset)
{
	INT16U	*pwTemp;
	
	if(wAddrOffset < cEepromDynamicTotalLength)
	{
		pwTemp = (INT16U *)&StrEepromDynamic.wLength;
		pwTemp += wAddrOffset;
		return(*pwTemp);
	}
	else
	{
		return 0xFFFF;
	}
}

INT16U	swGetEepromRealtimeMapData(INT16U wAddrOffset)
{
	INT16U	*pwTemp;
	
	if(wAddrOffset < cEepromRealtimeTotalLength)
	{
		pwTemp = (INT16U *)&StrEepromRealtime.wLength;
		pwTemp += wAddrOffset;
		return(*pwTemp);
	}
	else
	{
		return 0xFFFF;
	}
}

INT16U	swGetEepromFrMapData(INT16U wAddrOffset)
{
	INT16U	*pwTemp;
	
	if(wAddrOffset < cEepromFrTotalLength)
	{
		pwTemp = (INT16U *)&StrEepromFaultRecord.wLength;
		pwTemp += wAddrOffset;
		return(*pwTemp);
	}
	else
	{
		return 0xFFFF;
	}
}

INT16U	swGetEepromVer(void)
{
	return(StrEepromStatic.wEepromVer);
}

INT16U	swGetEepromSerialNo1(void)
{
	return(StrEepromStatic.wSerialNo[0]);
}

INT16U	swGetEepromSerialNo2(void)
{
	return(StrEepromStatic.wSerialNo[1]);
}

INT16U	swGetEepromSerialNo3(void)
{
	return(StrEepromStatic.wSerialNo[2]);
}

INT16U	swGetEepromSerialNo4(void)
{
	return(StrEepromStatic.wSerialNo[3]);
}

INT16U	swGetEepromSerialNo5(void)
{
	return(StrEepromStatic.wSerialNo[4]);
}

INT16U	swGetEepromSerialNoLen(void)
{
	return(StrEepromStatic.wSerialNoLen);
}

INT16U	swGetEepromBattVoltAdj(void)
{
	return(StrEepromStatic.wBattVoltAdj);
}

INT16S	swGetEepromBattVoltBias(void)
{
	return(StrEepromStatic.wBattVoltBias);
}

INT16U	swGetEepromBattChgCurrAdj(void)
{
	return(StrEepromStatic.wBattChgCurrAdj);
}

INT16S	swGetEepromBattChgCurrBias(void)
{
	return(StrEepromStatic.wBattChgCurrBias);
}

INT16U	swGetEepromBattDsgCurrAdj(void)
{
	return(StrEepromStatic.wBattDsgCurrAdj);
}

INT16S	swGetEepromBattDsgCurrBias(void)
{
	return(StrEepromStatic.wBattDsgCurrBias);
}

INT16U	swGetEepromCellVoltAdj(INT8U bNo)
{
	return *((&StrEepromStatic.wCellVolt01Adj) + (bNo << 1));
}

INT16S	swGetEepromCellVoltBias(INT8U bNo)
{
	return *((&StrEepromStatic.wCellVolt01Bias) + (bNo << 1));
}

INT16U	swGetEepromCellVolt01Adj(void)
{
	return(StrEepromStatic.wCellVolt01Adj);
}

INT16S	swGetEepromCellVolt01Bias(void)
{
	return(StrEepromStatic.wCellVolt01Bias);
}

INT16U	swGetEepromCellVolt02Adj(void)
{
	return(StrEepromStatic.wCellVolt02Adj);
}

INT16S	swGetEepromCellVolt02Bias(void)
{
	return(StrEepromStatic.wCellVolt02Bias);
}

INT16U	swGetEepromCellVolt03Adj(void)
{
	return(StrEepromStatic.wCellVolt03Adj);
}

INT16S	swGetEepromCellVolt03Bias(void)
{
	return(StrEepromStatic.wCellVolt03Bias);
}

INT16U	swGetEepromCellVolt04Adj(void)
{
	return(StrEepromStatic.wCellVolt04Adj);
}

INT16S	swGetEepromCellVolt04Bias(void)
{
	return(StrEepromStatic.wCellVolt04Bias);
}

INT16U	swGetEepromCellVolt05Adj(void)
{
	return(StrEepromStatic.wCellVolt05Adj);
}

INT16S	swGetEepromCellVolt05Bias(void)
{
	return(StrEepromStatic.wCellVolt05Bias);
}

INT16U	swGetEepromCellVolt06Adj(void)
{
	return(StrEepromStatic.wCellVolt06Adj);
}

INT16S	swGetEepromCellVolt06Bias(void)
{
	return(StrEepromStatic.wCellVolt06Bias);
}

INT16U	swGetEepromCellVolt07Adj(void)
{
	return(StrEepromStatic.wCellVolt07Adj);
}

INT16S	swGetEepromCellVolt07Bias(void)
{
	return(StrEepromStatic.wCellVolt07Bias);
}

INT16U	swGetEepromCellVolt08Adj(void)
{
	return(StrEepromStatic.wCellVolt08Adj);
}

INT16S	swGetEepromCellVolt08Bias(void)
{
	return(StrEepromStatic.wCellVolt08Bias);
}

INT16U	swGetEepromCellVolt09Adj(void)
{
	return(StrEepromStatic.wCellVolt09Adj);
}

INT16S	swGetEepromCellVolt09Bias(void)
{
	return(StrEepromStatic.wCellVolt09Bias);
}

INT16U	swGetEepromCellVolt10Adj(void)
{
	return(StrEepromStatic.wCellVolt10Adj);
}

INT16S	swGetEepromCellVolt10Bias(void)
{
	return(StrEepromStatic.wCellVolt10Bias);
}

INT16U	swGetEepromCellVolt11Adj(void)
{
	return(StrEepromStatic.wCellVolt11Adj);
}

INT16S	swGetEepromCellVolt11Bias(void)
{
	return(StrEepromStatic.wCellVolt11Bias);
}

INT16U	swGetEepromCellVolt12Adj(void)
{
	return(StrEepromStatic.wCellVolt12Adj);
}

INT16S	swGetEepromCellVolt12Bias(void)
{
	return(StrEepromStatic.wCellVolt12Bias);
}

INT16U	swGetEepromCellVolt13Adj(void)
{
	return(StrEepromStatic.wCellVolt13Adj);
}

INT16S	swGetEepromCellVolt13Bias(void)
{
	return(StrEepromStatic.wCellVolt13Bias);
}

INT16U	swGetEepromCellVolt14Adj(void)
{
	return(StrEepromStatic.wCellVolt14Adj);
}

INT16S	swGetEepromCellVolt14Bias(void)
{
	return(StrEepromStatic.wCellVolt14Bias);
}

INT16U	swGetEepromCellVolt15Adj(void)
{
	return(StrEepromStatic.wCellVolt15Adj);
}

INT16S	swGetEepromCellVolt15Bias(void)
{
	return(StrEepromStatic.wCellVolt15Bias);
}

INT16U	swGetEepromCellVolt16Adj(void)
{
	return(StrEepromStatic.wCellVolt16Adj);
}

INT16S	swGetEepromCellVolt16Bias(void)
{
	return(StrEepromStatic.wCellVolt16Bias);
}

INT16U	swGetEepromCellTempAdj(void)
{
	return(StrEepromStatic.wCellTempAdj);
}

INT16S	swGetEepromCellTempBias(void)
{
	return(StrEepromStatic.wCellTempBias);
}

INT16S	swGetEepromBattType(void)
{
	return(StrEepromStatic.wBattType);
}

INT16S	swGetEepromStaticRandomNum(void)
{
	return(StrEepromStatic.wRandomNum);
}

INT16U	swGetEepromHacpMode(void)
{
	return(StrEepromDynamic.wHacpMode);
}

INT16U	swGetEepromSlaveNum(void)
{
	return(StrEepromDynamic.wSlaveNum);
}

INT16U	swGetEepromModuleAddr(void)
{
	return(StrEepromDynamic.wModuleAddr);
}

INT16U	swGetEepromModuleRes(void)
{
	return(StrEepromDynamic.wModuleRes);
}

INT16U	swGetEepromCellVoltPct80(void)
{
	return(StrEepromDynamic.wCellVoltPct80);
}

INT16U	swGetEepromCellVoltPct20(void)
{
	return(StrEepromDynamic.wCellVoltPct20);
}

INT16U	swGetEepromCellVoltHi2(void)
{
	return(StrEepromDynamic.wCellVoltHi2);
}

INT16U	swGetEepromCellVoltLo2(void)
{
	return(StrEepromDynamic.wCellVoltLo2);
}

INT16U	swGetEepromCellVoltHiAlm(void)
{
	return(StrEepromDynamic.wCellVoltHiAlm);
}

INT16U	swGetEepromCellVoltHiAlmBack(void)
{
	return(StrEepromDynamic.wCellVoltHiAlmBack);
}

INT16U	swGetEepromCellVoltHi(void)
{
	return(StrEepromDynamic.wCellVoltHi);
}

INT16U	swGetEepromCellVoltHiBack(void)
{
	return(StrEepromDynamic.wCellVoltHiBack);
}

INT16U	swGetEepromCellVoltLoAlm(void)
{
	return(StrEepromDynamic.wCellVoltLoAlm);
}

INT16U	swGetEepromCellVoltLoAlmBack(void)
{
	return(StrEepromDynamic.wCellVoltLoAlmBack);
}

INT16U	swGetEepromCellVoltLo(void)
{
	return(StrEepromDynamic.wCellVoltLo);
}

INT16U	swGetEepromCellVoltLoBack(void)
{
	return(StrEepromDynamic.wCellVoltLoBack);
}

INT16U	swGetEepromBattChgCurrHiAlmBack(void)
{
	return(StrEepromDynamic.wBattChgCurrHiAlmBack);
}

INT16U	swGetEepromBattChgCurrHiAlm(void)
{
	return(StrEepromDynamic.wBattChgCurrHiAlm);
}

INT16U	swGetEepromBattChgCurrHi(void)
{
	return(StrEepromDynamic.wBattChgCurrHi);
}

INT16U	swGetEepromBattChgCurrHi2(void)
{
	return(StrEepromDynamic.wBattChgCurrHi2);
}

INT16U	swGetEepromBattDcgCurrHiAlmBack(void)
{
	return(StrEepromDynamic.wBattDcgCurrHiAlmBack);
}

INT16U	swGetEepromBattDcgCurrHiAlm(void)
{
	return(StrEepromDynamic.wBattDcgCurrHiAlm);
}

INT16U	swGetEepromBattDcgCurrHi(void)
{
	return(StrEepromDynamic.wBattDcgCurrHi);
}

INT16U	swGetEepromBattDcgCurrHi2(void)
{
	return(StrEepromDynamic.wBattDcgCurrHi2);
}

INT16U	swGetEepromCellChgTempHiAlmBack(void)
{
	return(StrEepromDynamic.wCellChgTempHiAlmBack);
}

INT16U	swGetEepromCellChgTempHiAlm(void)
{
	return(StrEepromDynamic.wCellChgTempHiAlm);
}

INT16U	swGetEepromCellChgTempHi(void)
{
	return(StrEepromDynamic.wCellChgTempHi);
}

INT16U	swGetEepromCellChgTempHi2(void)
{
	return(StrEepromDynamic.wCellChgTempHi2);
}

INT16S	swGetEepromCellChgTempLoAlmBack(void)
{
	return(StrEepromDynamic.wCellChgTempLoAlmBack);
}

INT16S	swGetEepromCellChgTempLoAlm(void)
{
	return(StrEepromDynamic.wCellChgTempLoAlm);
}

INT16S	swGetEepromCellChgTempLo(void)
{
	return(StrEepromDynamic.wCellChgTempLo);
}

INT16S	swGetEepromCellChgTempLo2(void)
{
	return(StrEepromDynamic.wCellChgTempLo2);
}

INT16U	swGetEepromCellDcgTempHiAlmBack(void)
{
	return(StrEepromDynamic.wCellDcgTempHiAlmBack);
}

INT16U	swGetEepromCellDcgTempHiAlm(void)
{
	return(StrEepromDynamic.wCellDcgTempHiAlm);
}

INT16U	swGetEepromCellDcgTempHi(void)
{
	return(StrEepromDynamic.wCellDcgTempHi);
}

INT16U	swGetEepromCellDcgTempHi2(void)
{
	return(StrEepromDynamic.wCellDcgTempHi2);
}

INT16S	swGetEepromCellDcgTempLoAlmBack(void)
{
	return(StrEepromDynamic.wCellDcgTempLoAlmBack);
}

INT16S	swGetEepromCellDcgTempLoAlm(void)
{
	return(StrEepromDynamic.wCellDcgTempLoAlm);
}

INT16S	swGetEepromCellDcgTempLo(void)
{
	return(StrEepromDynamic.wCellDcgTempLo);
}

INT16S	swGetEepromCellDcgTempLo2(void)
{
	return(StrEepromDynamic.wCellDcgTempLo2);
}

INT16U	swGetEepromHsTempHiAlm(void)
{
	return(StrEepromDynamic.wHsTempHiAlm);
}

INT16U	swGetEepromHsTempHi(void)
{
	return(StrEepromDynamic.wHsTempHi);
}

INT16U	swGetEepromBalanceEn(void)
{
	return(StrEepromDynamic.wBalanceEn);
}

INT16U	swGetEepromBalanceVoltErr(void)
{
	return(StrEepromDynamic.wBalanceVoltErr);
}

INT16U	swGetEepromBalanceVoltEn(void)
{
	return(StrEepromDynamic.wBalanceVoltEn);
}

INT16U	swGetEepromBalanceTempEn(void)
{
	return(StrEepromDynamic.wBalanceTempEn);
}

INT16U	swGetEepromCellDcgDepth(void)
{
	return(StrEepromDynamic.wCellDcgDepth);
}

INT16S	swGetEepromCellTempCoef(void)
{
	return(StrEepromDynamic.wCellTempCoef);
}

INT16U	swGetEepromDynamicRsvd2(void)
{
	return(StrEepromDynamic.wReserved2);
}

INT16U	swGetEepromFaultRstCnt(void)
{
	return(StrEepromDynamic.wFaultRstCnt);
}

INT16U	swGetEepromFaultClrTime(void)
{
	return(StrEepromDynamic.wFaultClrTime);
}

INT16U	swGetEepromIdleExitTime(void)
{
	return(StrEepromDynamic.wIdleExitTime);
}

INT16U	swGetEepromSleepExitTime(void)
{
	return(StrEepromDynamic.wSleepExitTime);
}

INT16U	swGetEepromBreakWireMode(void)
{
	return(StrEepromDynamic.wBreakWireMode);
}

INT16U	swGetEepromBackLightEn(void)
{
	return(StrEepromDynamic.wBackLightEn);
}

INT16U	swGetEepromMenuRtnEn(void)
{
	return(StrEepromDynamic.wMenuRtnEn);
}

INT16U	swGetEepromFaultRecordEn(void)
{
	return(StrEepromDynamic.wFaultRecordEn);
}

INT16U	swGetEepromCellTempShield(void)
{
	return(StrEepromDynamic.wCellTempShield);
}

INT16U	swGetEepromCellVoltPct20Two(void)
{
	return(StrEepromDynamic.wCellVoltPct20Two);
}

INT16U	swGetEepromBattAhReal(void)
{
	return(StrEepromRealtime.wBattAhReal);
}

INT16U	swGetEepromBattAhMax(void)
{
	return(StrEepromRealtime.wBattAhMax);
}

INT16U	swGetEepromBattFullCnt(void)
{
	return(StrEepromRealtime.wBattFullCnt);
}

INT16U	swGetEepromBattEmptyCnt(void)
{
	return(StrEepromRealtime.wBattEmptyCnt);
}

INT16U	swGetEepromBattCycCnt(void)
{
	return(StrEepromRealtime.wBattCycCnt);
}

INT16U	swGetEepromBattChgAhHi(void)
{
	return(StrEepromRealtime.wBattChgAhHi);
}

INT16U	swGetEepromBattChgAhLo(void)
{
	return(StrEepromRealtime.wBattChgAhLo);
}

INT32U	sdwGetEepromBattChgAh(void)
{
	return(((INT32U)StrEepromRealtime.wBattChgAhHi << 16) + StrEepromRealtime.wBattChgAhLo);
}

INT16U	swGetEepromBattChgWhHi(void)
{
	return(StrEepromRealtime.wBattChgWhHi);
}

INT16U	swGetEepromBattChgWhLo(void)
{
	return(StrEepromRealtime.wBattChgWhLo);
}

INT32U	sdwGetEepromBattChgWh(void)
{
	return(((INT32U)StrEepromRealtime.wBattChgWhHi << 16) + StrEepromRealtime.wBattChgWhLo);
}

INT16U	swGetEepromBattChgTimeHi(void)
{
	return(StrEepromRealtime.wBattChgTimeHi);
}

INT16U	swGetEepromBattChgTimeLo(void)
{
	return(StrEepromRealtime.wBattChgTimeLo);
}

INT32U	sdwGetEepromBattChgTime(void)
{
	return(((INT32U)StrEepromRealtime.wBattChgTimeHi << 16) + StrEepromRealtime.wBattChgTimeLo);
}

INT16U	swGetEepromBattDcgAhHi(void)
{
	return(StrEepromRealtime.wBattDcgAhHi);
}

INT16U	swGetEepromBattDcgAhLo(void)
{
	return(StrEepromRealtime.wBattDcgAhLo);
}

INT32U	sdwGetEepromBattDcgAh(void)
{
	return(((INT32U)StrEepromRealtime.wBattDcgAhHi << 16) + StrEepromRealtime.wBattDcgAhLo);
}

INT16U	swGetEepromBattDcgWhHi(void)
{
	return(StrEepromRealtime.wBattDcgWhHi);
}

INT16U	swGetEepromBattDcgWhLo(void)
{
	return(StrEepromRealtime.wBattDcgWhLo);
}

INT32U	sdwGetEepromBattDcgWh(void)
{
	return(((INT32U)StrEepromRealtime.wBattDcgWhHi << 16) + StrEepromRealtime.wBattDcgWhLo);
}

INT16U	swGetEepromBattDcgTimeHi(void)
{
	return(StrEepromRealtime.wBattDcgTimeHi);
}

INT16U	swGetEepromBattDcgTimeLo(void)
{
	return(StrEepromRealtime.wBattDcgTimeLo);
}

INT32U	sdwGetEepromBattDcgTime(void)
{
	return(((INT32U)StrEepromRealtime.wBattDcgTimeHi << 16) + StrEepromRealtime.wBattDcgTimeLo);
}

INT16U	swGetEepromBattRunTimeHi(void)
{
	return(StrEepromRealtime.wBattRunTimeHi);
}

INT16U	swGetEepromBattRunTimeLo(void)
{
	return(StrEepromRealtime.wBattRunTimeLo);
}

INT32U	sdwGetEepromBattRunTime(void)
{
	return(((INT32U)StrEepromRealtime.wBattRunTimeHi << 16) + StrEepromRealtime.wBattRunTimeLo);
}

INT16U	swGetEepromFrBmsMode(void)
{
	return(StrEepromFaultRecord.wFrBmsMode);
}

INT16U	swGetEepromFrAlarmCode(void)
{
	return(StrEepromFaultRecord.wFrAlarmCode);
}

INT16U	swGetEepromFrFaultFlag(void)
{
	return(StrEepromFaultRecord.wFrFaultFlag);
}

INT16U	swGetEepromFrAlarmFlag(void)
{
	return(StrEepromFaultRecord.wFrAlarmFlag);
}

INT16U	swGetEepromFrBmsStatus(void)
{
	return(StrEepromFaultRecord.wFrBmsStatus);
}

INT16U	swGetEepromFrBattVolt(void)
{
	return(StrEepromFaultRecord.wFrBattVolt);
}

INT16U	swGetEepromFrBattCurr(void)
{
	return(StrEepromFaultRecord.wFrBattCurr);
}

INT16U	swGetEepromFrBattSoc(void)
{
	return(StrEepromFaultRecord.wBattSoC);
}

INT16U	swGetEepromFrBattSoh(void)
{
	return(StrEepromFaultRecord.wBattSoH);
}

INT16U	swGetEepromFrHsTemp(void)
{
	return(StrEepromFaultRecord.wFrHsTemp);
}

INT16U	swGetEepromFrBalanceTemp(void)
{
	return(StrEepromFaultRecord.wFrBalanceTemp);
}

INT16U	swGetEepromFrCellVolt1(void)
{
	return(StrEepromFaultRecord.wFrCellVolt1);
}

INT16U	swGetEepromFrCellVolt2(void)
{
	return(StrEepromFaultRecord.wFrCellVolt2);
}

INT16U	swGetEepromFrCellVolt3(void)
{
	return(StrEepromFaultRecord.wFrCellVolt3);
}

INT16U	swGetEepromFrCellVolt4(void)
{
	return(StrEepromFaultRecord.wFrCellVolt4);
}

INT16U	swGetEepromFrCellVolt5(void)
{
	return(StrEepromFaultRecord.wFrCellVolt5);
}

INT16U	swGetEepromFrCellVolt6(void)
{
	return(StrEepromFaultRecord.wFrCellVolt6);
}

INT16U	swGetEepromFrCellVolt7(void)
{
	return(StrEepromFaultRecord.wFrCellVolt7);
}

INT16U	swGetEepromFrCellVolt8(void)
{
	return(StrEepromFaultRecord.wFrCellVolt8);
}

INT16U	swGetEepromFrCellVolt9(void)
{
	return(StrEepromFaultRecord.wFrCellVolt9);
}

INT16U	swGetEepromFrCellVolt10(void)
{
	return(StrEepromFaultRecord.wFrCellVolt10);
}

INT16U	swGetEepromFrCellVolt11		(void)
{
	return(StrEepromFaultRecord.wFrCellVolt11);
}

INT16U	swGetEepromFrCellVolt12(void)
{
	return(StrEepromFaultRecord.wFrCellVolt12);
}

INT16U	swGetEepromFrCellVolt13(void)
{
	return(StrEepromFaultRecord.wFrCellVolt13);
}

INT16U	swGetEepromFrCellVolt14(void)
{
	return(StrEepromFaultRecord.wFrCellVolt14);
}

INT16U	swGetEepromFrCellVolt15(void)
{
	return(StrEepromFaultRecord.wFrCellVolt15);
}

INT16U	swGetEepromFrCellVolt16(void)
{
	return(StrEepromFaultRecord.wFrCellVolt16);
}

INT16U	swGetEepromFrCellTemp1(void)
{
	return(StrEepromFaultRecord.wFrCellTemp1);
}

INT16U	swGetEepromFrCellTemp2(void)
{
	return(StrEepromFaultRecord.wFrCellTemp2);
}

INT16U	swGetEepromFrCellTemp3(void)
{
	return(StrEepromFaultRecord.wFrCellTemp3);
}

INT16U	swGetEepromFrCellTemp4(void)
{
	return(StrEepromFaultRecord.wFrCellTemp4);
}

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
void	sDefaultEepromStatic(void)
{
	sEepromStaticDefaultWrite(cEepromStaticDefaultTable.Table,0,cEepromStaticTotalLength - 2);
}

void	sDefaultEepromDynamic(void)
{
	sEepromDynamicDefaultWrite(cEepromDynamicDefaultTable.Table,0,cEepromDynamicTotalLength - 2);
}

void	sDefaultEepromRealtime(void)
{
	sEepromRealtimeDefaultWrite(cEepromRealtimeDefaultTable.Table,0,cEepromRealtimeTotalLength - 2);
}

void	sDefaultEepromFaultRecord(void)
{
	sEepromFrDefaultWrite(cEepromFrDefaultTable.Table,0,cEepromFrTotalLength - 2);
}

void	sSetEepromRwEnable(INT8U bStatus)
{
	fEepromStatus.bits.bRwEnable = bStatus;
}

void	sSetEepromRwBusy(INT8U bStatus)
{
	fEepromStatus.bits.bRwBusy = bStatus;
}

void	sSetEepromSecureLock(INT8U bStatus)
{
	fEepromStatus.bits.bSecureLock = bStatus;
	fEepromStatus.bits.bSecureLockRst = ~fEepromStatus.bits.bSecureLock;
}

void	sSetEepromStaticMode(INT8U bMode)
{
	fEepromStatus.bits.bEEStaticMode = bMode;
}

void	sSetEepromDynamicMode(INT8U bMode)
{
	fEepromStatus.bits.bEEDynamicMode = bMode;
}

void	sSetEepromRealtimeMode(INT8U bMode)
{
	fEepromStatus.bits.bEERealtimeMode = bMode;
}

void	sSetEepromFaultRecordMode(INT8U bMode)
{
	fEepromStatus.bits.bEEFaultRecordMode = bMode;
}

void	sSetEepromVer(INT16U wVer)
{
	StrEepromStatic.wEepromVer = wVer;
}

void	sSetEepromSerialNo1(INT16U wSN)
{
	StrEepromStatic.wSerialNo[0] = wSN;
}

void	sSetEepromSerialNo2(INT16U wSN)
{
	StrEepromStatic.wSerialNo[1] = wSN;
}

void	sSetEepromSerialNo3(INT16U wSN)
{
	StrEepromStatic.wSerialNo[2] = wSN;
}

void	sSetEepromSerialNo4(INT16U wSN)
{
	StrEepromStatic.wSerialNo[3] = wSN;
}

void	sSetEepromSerialNo5(INT16U wSN)
{
	StrEepromStatic.wSerialNo[4] = wSN;
}

void	sSetEepromSerialNoLen(INT16U wSNLen)
{
	StrEepromStatic.wSerialNoLen = wSNLen;
}

void	sSetEepromBattVoltAdj(INT16U wAdj)
{
	StrEepromStatic.wBattVoltAdj = wAdj;
}

void	sSetEepromBattVoltBias(INT16S wBias)
{
	StrEepromStatic.wBattVoltBias = wBias;
}

void	sSetEepromBattChgCurrAdj(INT16U wAdj)
{
	StrEepromStatic.wBattChgCurrAdj = wAdj;
}

void	sSetEepromBattChgCurrBias(INT16S wBias)
{
	StrEepromStatic.wBattChgCurrBias = wBias;
}

void	sSetEepromBattDsgCurrAdj(INT16U wAdj)
{
	StrEepromStatic.wBattDsgCurrAdj = wAdj;
}

void	sSetEepromBattDsgCurrBias(INT16S wBias)
{
	StrEepromStatic.wBattDsgCurrBias = wBias;
}

void	sSetEepromCellVoltAdj(INT8U bNo, INT16U wAdj)
{
	*((&StrEepromStatic.wCellVolt01Adj) + (bNo << 1)) = wAdj;
}

void	sSetEepromCellVoltBias(INT8U bNo, INT16S wBias)
{
	*((&StrEepromStatic.wCellVolt01Bias) + (bNo << 1)) = wBias;
}

void	sSetEepromCellVolt01Adj(INT16U wAdj)
{
	StrEepromStatic.wCellVolt01Adj = wAdj;
}

void	sSetEepromCellVolt01Bias(INT16S wBias)
{
	StrEepromStatic.wCellVolt01Bias = wBias;
}

void	sSetEepromCellVolt02Adj(INT16U wAdj)
{
	StrEepromStatic.wCellVolt02Adj = wAdj;
}

void	sSetEepromCellVolt02Bias(INT16S wBias)
{
	StrEepromStatic.wCellVolt02Bias = wBias;
}

void	sSetEepromCellVolt03Adj(INT16U wAdj)
{
	StrEepromStatic.wCellVolt03Adj = wAdj;
}

void	sSetEepromCellVolt03Bias(INT16S wBias)
{
	StrEepromStatic.wCellVolt03Bias = wBias;
}

void	sSetEepromCellVolt04Adj(INT16U wAdj)
{
	StrEepromStatic.wCellVolt04Adj = wAdj;
}

void	sSetEepromCellVolt04Bias(INT16S wBias)
{
	StrEepromStatic.wCellVolt04Bias = wBias;
}

void	sSetEepromCellVolt05Adj(INT16U wAdj)
{
	StrEepromStatic.wCellVolt05Adj = wAdj;
}

void	sSetEepromCellVolt05Bias(INT16S wBias)
{
	StrEepromStatic.wCellVolt05Bias = wBias;
}

void	sSetEepromCellVolt06Adj(INT16U wAdj)
{
	StrEepromStatic.wCellVolt06Adj = wAdj;
}

void	sSetEepromCellVolt06Bias(INT16S wBias)
{
	StrEepromStatic.wCellVolt06Bias = wBias;
}

void	sSetEepromCellVolt07Adj(INT16U wAdj)
{
	StrEepromStatic.wCellVolt07Adj = wAdj;
}

void	sSetEepromCellVolt07Bias(INT16S wBias)
{
	StrEepromStatic.wCellVolt07Bias = wBias;
}

void	sSetEepromCellVolt08Adj(INT16U wAdj)
{
	StrEepromStatic.wCellVolt08Adj = wAdj;
}

void	sSetEepromCellVolt08Bias(INT16S wBias)
{
	StrEepromStatic.wCellVolt08Bias = wBias;
}

void	sSetEepromCellVolt09Adj(INT16U wAdj)
{
	StrEepromStatic.wCellVolt09Adj = wAdj;
}

void	sSetEepromCellVolt09Bias(INT16S wBias)
{
	StrEepromStatic.wCellVolt09Bias = wBias;
}

void	sSetEepromCellVolt10Adj(INT16U wAdj)
{
	StrEepromStatic.wCellVolt10Adj = wAdj;
}

void	sSetEepromCellVolt10Bias(INT16S wBias)
{
	StrEepromStatic.wCellVolt10Bias = wBias;
}

void	sSetEepromCellVolt11Adj(INT16U wAdj)
{
	StrEepromStatic.wCellVolt11Adj = wAdj;
}

void	sSetEepromCellVolt11Bias(INT16S wBias)
{
	StrEepromStatic.wCellVolt11Bias = wBias;
}

void	sSetEepromCellVolt12Adj(INT16U wAdj)
{
	StrEepromStatic.wCellVolt12Adj = wAdj;
}

void	sSetEepromCellVolt12Bias(INT16S wBias)
{
	StrEepromStatic.wCellVolt12Bias = wBias;
}

void	sSetEepromCellVolt13Adj(INT16U wAdj)
{
	StrEepromStatic.wCellVolt13Adj = wAdj;
}

void	sSetEepromCellVolt13Bias(INT16S wBias)
{
	StrEepromStatic.wCellVolt13Bias = wBias;
}

void	sSetEepromCellVolt14Adj(INT16U wAdj)
{
	StrEepromStatic.wCellVolt14Adj = wAdj;
}

void	sSetEepromCellVolt14Bias(INT16S wBias)
{
	StrEepromStatic.wCellVolt14Bias = wBias;
}

void	sSetEepromCellVolt15Adj(INT16U wAdj)
{
	StrEepromStatic.wCellVolt15Adj = wAdj;
}

void	sSetEepromCellVolt15Bias(INT16S wBias)
{
	StrEepromStatic.wCellVolt15Bias = wBias;
}

void	sSetEepromCellVolt16Adj(INT16U wAdj)
{
	StrEepromStatic.wCellVolt16Adj = wAdj;
}

void	sSetEepromCellVolt16Bias(INT16S wBias)
{
	StrEepromStatic.wCellVolt16Bias = wBias;
}

void	sSetEepromCellTempAdj(INT16U wAdj)
{
	StrEepromStatic.wCellTempAdj = wAdj;
}

void	sSetEepromCellTempBias(INT16S wBias)
{
	StrEepromStatic.wCellTempBias = wBias;
}

void	sSetEepromBattType(INT16S wType)
{
	StrEepromStatic.wBattType = wType;
}

void	sSetEepromStaticRandomNum(INT16U wData)
{
	StrEepromStatic.wRandomNum = wData;
}

void	sSetEepromHacpMode(INT16U wMode)
{
	StrEepromDynamic.wHacpMode = wMode;
}

void	sSetEepromSlaveNum(INT16U wNum)
{
	StrEepromDynamic.wSlaveNum = wNum;
}

void	sSetEepromModuleAddr(INT16U wAddr)
{
	StrEepromDynamic.wModuleAddr = wAddr;
}

void	sSetEepromModuleRes(INT16U wEn)
{
	StrEepromDynamic.wModuleRes = wEn;
}

void	sSetEepromCellVoltPct80(INT16U wVolt)
{
	StrEepromDynamic.wCellVoltPct80 = wVolt;
}

void	sSetEepromCellVoltPct20(INT16U wVolt)
{
	StrEepromDynamic.wCellVoltPct20 = wVolt;
}

void	sSetEepromCellVoltHi2(INT16U wVolt)
{
	StrEepromDynamic.wCellVoltHi2 = wVolt;
}

void	sSetEepromCellVoltLo2(INT16U wVolt)
{
	StrEepromDynamic.wCellVoltLo2 = wVolt;
}

void	sSetEepromCellVoltHiAlm(INT16U wVolt)
{
	StrEepromDynamic.wCellVoltHiAlm = wVolt;
}

void	sSetEepromCellVoltHiAlmBack(INT16U wVolt)
{
	StrEepromDynamic.wCellVoltHiAlmBack = wVolt;
}

void	sSetEepromCellVoltHi(INT16U wVolt)
{
	StrEepromDynamic.wCellVoltHi = wVolt;
}

void	sSetEepromCellVoltHiBack(INT16U wVolt)
{
	StrEepromDynamic.wCellVoltHiBack = wVolt;
}

void	sSetEepromCellVoltLoAlm(INT16U wVolt)
{
	StrEepromDynamic.wCellVoltLoAlm = wVolt;
}

void	sSetEepromCellVoltLoAlmBack(INT16U wVolt)
{
	StrEepromDynamic.wCellVoltLoAlmBack = wVolt;
}

void	sSetEepromCellVoltLo(INT16U wVolt)
{
	StrEepromDynamic.wCellVoltLo = wVolt;
}

void	sSetEepromCellVoltLoBack(INT16U wVolt)
{
	StrEepromDynamic.wCellVoltLoBack = wVolt;
}

void	sSetEepromBattChgCurrHiAlmBack(INT16U wCurr)
{
	StrEepromDynamic.wBattChgCurrHiAlmBack = wCurr;
}

void	sSetEepromBattChgCurrHiAlm(INT16U wCurr)
{
	StrEepromDynamic.wBattChgCurrHiAlm = wCurr;
}

void	sSetEepromBattChgCurrHi(INT16U wCurr)
{
	StrEepromDynamic.wBattChgCurrHi = wCurr;
}

void	sSetEepromBattChgCurrHi2(INT16U wCurr)
{
	StrEepromDynamic.wBattChgCurrHi2 = wCurr;
}

void	sSetEepromBattDcgCurrHiAlmBack(INT16U wCurr)
{
	StrEepromDynamic.wBattDcgCurrHiAlmBack = wCurr;
}

void	sSetEepromBattDcgCurrHiAlm(INT16U wCurr)
{
	StrEepromDynamic.wBattDcgCurrHiAlm = wCurr;
}

void	sSetEepromBattDcgCurrHi(INT16U wCurr)
{
	StrEepromDynamic.wBattDcgCurrHi = wCurr;
}

void	sSetEepromBattDcgCurrHi2(INT16U wCurr)
{
	StrEepromDynamic.wBattDcgCurrHi2 = wCurr;
}

void	sSetEepromCellChgTempHiAlmBack(INT16U wTemp)
{
	StrEepromDynamic.wCellChgTempHiAlmBack = wTemp;
}

void	sSetEepromCellChgTempHiAlm(INT16U wTemp)
{
	StrEepromDynamic.wCellChgTempHiAlm = wTemp;
}

void	sSetEepromCellChgTempHi(INT16U wTemp)
{
	StrEepromDynamic.wCellChgTempHi = wTemp;
}

void	sSetEepromCellChgTempHi2(INT16U wTemp)
{
	StrEepromDynamic.wCellChgTempHi2 = wTemp;
}

void	sSetEepromCellChgTempLoAlmBack(INT16S wTemp)
{
	StrEepromDynamic.wCellChgTempLoAlmBack = wTemp;
}

void	sSetEepromCellChgTempLoAlm(INT16S wTemp)
{
	StrEepromDynamic.wCellChgTempLoAlm = wTemp;
}

void	sSetEepromCellChgTempLo(INT16S wTemp)
{
	StrEepromDynamic.wCellChgTempLo = wTemp;
}

void	sSetEepromCellChgTempLo2(INT16S wTemp)
{
	StrEepromDynamic.wCellChgTempLo2 = wTemp;
}

void	sSetEepromCellDcgTempHiAlmBack(INT16U wTemp)
{
	StrEepromDynamic.wCellDcgTempHiAlmBack = wTemp;
}

void	sSetEepromCellDcgTempHiAlm(INT16U wTemp)
{
	StrEepromDynamic.wCellDcgTempHiAlm = wTemp;
}

void	sSetEepromCellDcgTempHi(INT16U wTemp)
{
	StrEepromDynamic.wCellDcgTempHi = wTemp;
}

void	sSetEepromCellDcgTempHi2(INT16U wTemp)
{
	StrEepromDynamic.wCellDcgTempHi2 = wTemp;
}

void	sSetEepromCellDcgTempLoAlmBack(INT16S wTemp)
{
	StrEepromDynamic.wCellDcgTempLoAlmBack = wTemp;
}

void	sSetEepromCellDcgTempLoAlm(INT16S wTemp)
{
	StrEepromDynamic.wCellDcgTempLoAlm = wTemp;
}

void	sSetEepromCellDcgTempLo(INT16S wTemp)
{
	StrEepromDynamic.wCellDcgTempLo = wTemp;
}

void	sSetEepromCellDcgTempLo2(INT16S wTemp)
{
	StrEepromDynamic.wCellDcgTempLo2 = wTemp;
}

void	sSetEepromHsTempHiAlm(INT16U wTemp)
{
	StrEepromDynamic.wHsTempHiAlm = wTemp;
}

void	sSetEepromHsTempHi(INT16U wTemp)
{
	StrEepromDynamic.wHsTempHi = wTemp;
}

void	sSetEepromBalanceEn(INT16U wEnable)
{
	StrEepromDynamic.wBalanceEn = wEnable;
}

void	sSetEepromBalanceVoltErr(INT16U wVolt)
{
	StrEepromDynamic.wBalanceVoltErr = wVolt;
}

void	sSetEepromBalanceVoltEn(INT16U wVolt)
{
	StrEepromDynamic.wBalanceVoltEn = wVolt;
}

void	sSetEepromBalanceTempEn(INT16U wTemp)
{
	StrEepromDynamic.wBalanceTempEn = wTemp;
}

void	sSetEepromCellDcgDepth(INT16U wDepth)
{
	StrEepromDynamic.wCellDcgDepth = wDepth;
}

void	sSetEepromCellTempCoef(INT16S wCoef)
{
	StrEepromDynamic.wCellTempCoef = wCoef;
}

void	sSetEepromDynamicRsvd2(INT16U wAddr)
{
	StrEepromDynamic.wReserved2 = wAddr;
}

void	sSetEepromFaultRstCnt(INT16U wCnt)
{
	StrEepromDynamic.wFaultRstCnt = wCnt;
}

void	sSetEepromFaultClrTime(INT16U wTime)
{
	StrEepromDynamic.wFaultClrTime = wTime;
}

void	sSetEepromIdleExitTime(INT16U wTime)
{
	StrEepromDynamic.wIdleExitTime = wTime;
}

void	sSetEepromSleepExitTime(INT16U wTime)
{
	StrEepromDynamic.wSleepExitTime = wTime;
}

void	sSetEepromBreakWireMode(INT16U wMode)
{
	StrEepromDynamic.wBreakWireMode = wMode;
}

void	sSetEepromBackLightEn(INT16U wStatus)
{
	StrEepromDynamic.wBackLightEn = wStatus;
}

void	sSetEepromMenuRtnEn(INT16U wStatus)
{
	StrEepromDynamic.wMenuRtnEn = wStatus;
}

void	sSetEepromFaultRecordEn(INT16U wStatus)
{
	StrEepromDynamic.wFaultRecordEn = wStatus;
}

void	sSetEepromCellTempShield(INT16U wStatus)
{
	StrEepromDynamic.wCellTempShield = wStatus;
}

void	sSetEepromCellVoltPct20Two(INT16U wVolt)
{
	StrEepromDynamic.wCellVoltPct20Two = wVolt;
}

void	sSetEepromBattAhReal(INT16U wWH)
{
	StrEepromRealtime.wBattAhReal = wWH;
}

void	sSetEepromBattAhMax(INT16U wWH)
{
	StrEepromRealtime.wBattAhMax = wWH;
}

void	sSetEepromBattFullCnt(INT16U wCnt)
{
	StrEepromRealtime.wBattFullCnt = wCnt;
}

void	sSetEepromBattEmptyCnt(INT16U wCnt)
{
	StrEepromRealtime.wBattEmptyCnt = wCnt;
}

void	sSetEepromBattCycCnt(INT16U wCnt)
{
	StrEepromRealtime.wBattCycCnt = wCnt;
}

void	sSetEepromBattChgAhHi(INT16U wAh)
{
	StrEepromRealtime.wBattChgAhHi = wAh;
}

void	sSetEepromBattChgAhLo(INT16U wAh)
{
	StrEepromRealtime.wBattChgAhLo = wAh;
}

void	sSetEepromBattChgAh(INT32U dwAh)
{
	StrEepromRealtime.wBattChgAhLo = (INT16U)(dwAh & 0x0000FFFF);
	StrEepromRealtime.wBattChgAhHi = (INT16U)(dwAh >> 16);
}

void	sSetEepromBattChgWhHi(INT16U wWh)
{
	StrEepromRealtime.wBattChgWhHi = wWh;
}

void	sSetEepromBattChgWhLo(INT16U wWh)
{
	StrEepromRealtime.wBattChgWhLo = wWh;
}

void	sSetEepromBattChgWh(INT32U dwWh)
{
	StrEepromRealtime.wBattChgWhLo = (INT16U)(dwWh & 0x0000FFFF);
	StrEepromRealtime.wBattChgWhHi = (INT16U)(dwWh >> 16);
}

void	sSetEepromBattChgTimeHi(INT16U wTime)
{
	StrEepromRealtime.wBattChgWhHi = wTime;
}

void	sSetEepromBattChgTimeLo(INT16U wTime)
{
	StrEepromRealtime.wBattChgWhLo = wTime;
}

void	sSetEepromBattChgTime(INT32U dwTime)
{
	StrEepromRealtime.wBattChgTimeLo = (INT16U)(dwTime & 0x0000FFFF);
	StrEepromRealtime.wBattChgTimeHi = (INT16U)(dwTime >> 16);
}

void	sSetEepromBattDcgAhHi(INT16U wAh)
{
	StrEepromRealtime.wBattDcgAhHi = wAh;
}

void	sSetEepromBattDcgAhLo(INT16U wAh)
{
	StrEepromRealtime.wBattDcgAhLo = wAh;
}

void	sSetEepromBattDcgAh(INT32U dwAh)
{
	StrEepromRealtime.wBattDcgAhLo = (INT16U)(dwAh & 0x0000FFFF);
	StrEepromRealtime.wBattDcgAhHi = (INT16U)(dwAh >> 16);
}

void	sSetEepromBattDcgWhHi(INT16U wWh)
{
	StrEepromRealtime.wBattDcgWhHi = wWh;
}

void	sSetEepromBattDcgWhLo(INT16U wWh)
{
	StrEepromRealtime.wBattDcgWhLo = wWh;
}

void	sSetEepromBattDcgWh(INT32U dwWh)
{
	StrEepromRealtime.wBattDcgWhLo = (INT16U)(dwWh & 0x0000FFFF);
	StrEepromRealtime.wBattDcgWhHi = (INT16U)(dwWh >> 16);
}

void	sSetEepromBattDcgTimeHi(INT16U wTime)
{
	StrEepromRealtime.wBattDcgTimeHi = wTime;
}

void	sSetEepromBattDcgTimeLo(INT16U wTime)
{
	StrEepromRealtime.wBattDcgTimeLo = wTime;
}

void	sSetEepromBattDcgTime(INT32U dwTime)
{
	StrEepromRealtime.wBattDcgTimeLo = (INT16U)(dwTime & 0x0000FFFF);
	StrEepromRealtime.wBattDcgTimeHi = (INT16U)(dwTime >> 16);
}

void	sSetEepromBattRunTimeHi(INT16U wTime)
{
	StrEepromRealtime.wBattRunTimeHi = wTime;
}

void	sSetEepromBattRunTimeLo(INT16U wTime)
{
	StrEepromRealtime.wBattRunTimeLo = wTime;
}

void	sSetEepromBattRunTime(INT32U dwTime)
{
	StrEepromRealtime.wBattRunTimeLo = (INT16U)(dwTime & 0x0000FFFF);
	StrEepromRealtime.wBattRunTimeHi = (INT16U)(dwTime >> 16);
}

void	sSetEepromFrBmsMode(INT16U wMode)
{
	StrEepromFaultRecord.wFrBmsMode = wMode;
}

void	sSetEepromFrAlarmCode(INT16U wCode)
{
	StrEepromFaultRecord.wFrAlarmCode = wCode;
}

void	sSetEepromFrFaultFlag(INT16U wFlag)
{
	StrEepromFaultRecord.wFrFaultFlag = wFlag;
}

void	sSetEepromFrAlarmFlag(INT16U wFlag)
{
	StrEepromFaultRecord.wFrAlarmFlag = wFlag;
}

void	sSetEepromFrBmsStatus(INT16U wStatus)
{
	StrEepromFaultRecord.wFrBmsStatus = wStatus;
}

void	sSetEepromFrBattVolt(INT16U wVolt)
{
	StrEepromFaultRecord.wFrBattVolt = wVolt;
}

void	sSetEepromFrBattCurr(INT16U wCurr)
{
	StrEepromFaultRecord.wFrBattCurr = wCurr;
}

void	sSetEepromFrBattSoc(INT16U wSoC)
{
	StrEepromFaultRecord.wBattSoC = wSoC;
}

void	sSetEepromFrBattSoh(INT16U wSoH)
{
	StrEepromFaultRecord.wBattSoH = wSoH;
}

void	sSetEepromFrHsTemp(INT16U wTemp)
{
	StrEepromFaultRecord.wFrHsTemp = wTemp;
}

void	sSetEepromFrBalanceTemp(INT16U wTemp)
{
	StrEepromFaultRecord.wFrBalanceTemp = wTemp;
}

void	sSetEepromFrCellVolt1(INT16U wVolt)
{
	StrEepromFaultRecord.wFrCellVolt1 = wVolt;
}

void	sSetEepromFrCellVolt2(INT16U wVolt)
{
	StrEepromFaultRecord.wFrCellVolt2 = wVolt;
}

void	sSetEepromFrCellVolt3(INT16U wVolt)
{
	StrEepromFaultRecord.wFrCellVolt3 = wVolt;
}

void	sSetEepromFrCellVolt4(INT16U wVolt)
{
	StrEepromFaultRecord.wFrCellVolt4 = wVolt;
}

void	sSetEepromFrCellVolt5(INT16U wVolt)
{
	StrEepromFaultRecord.wFrCellVolt5 = wVolt;
}

void	sSetEepromFrCellVolt6(INT16U wVolt)
{
	StrEepromFaultRecord.wFrCellVolt6 = wVolt;
}

void	sSetEepromFrCellVolt7(INT16U wVolt)
{
	StrEepromFaultRecord.wFrCellVolt7 = wVolt;
}

void	sSetEepromFrCellVolt8(INT16U wVolt)
{
	StrEepromFaultRecord.wFrCellVolt8 = wVolt;
}

void	sSetEepromFrCellVolt9(INT16U wVolt)
{
	StrEepromFaultRecord.wFrCellVolt9 = wVolt;
}

void	sSetEepromFrCellVolt10(INT16U wVolt)
{
	StrEepromFaultRecord.wFrCellVolt10 = wVolt;
}

void	sSetEepromFrCellVolt11(INT16U wVolt)
{
	StrEepromFaultRecord.wFrCellVolt11 = wVolt;
}

void	sSetEepromFrCellVolt12(INT16U wVolt)
{
	StrEepromFaultRecord.wFrCellVolt12 = wVolt;
}

void	sSetEepromFrCellVolt13(INT16U wVolt)
{
	StrEepromFaultRecord.wFrCellVolt13 = wVolt;
}

void	sSetEepromFrCellVolt14(INT16U wVolt)
{
	StrEepromFaultRecord.wFrCellVolt14 = wVolt;
}

void	sSetEepromFrCellVolt15(INT16U wVolt)
{
	StrEepromFaultRecord.wFrCellVolt15 = wVolt;
}

void	sSetEepromFrCellVolt16(INT16U wVolt)
{
	StrEepromFaultRecord.wFrCellVolt16 = wVolt;
}

void	sSetEepromFrCellTemp1(INT16U wTemp)
{
	StrEepromFaultRecord.wFrCellTemp1 = wTemp;
}

void	sSetEepromFrCellTemp2(INT16U wTemp)
{
	StrEepromFaultRecord.wFrCellTemp2 = wTemp;
}

void	sSetEepromFrCellTemp3(INT16U wTemp)
{
	StrEepromFaultRecord.wFrCellTemp3 = wTemp;
}

void	sSetEepromFrCellTemp4(INT16U wTemp)
{
	StrEepromFaultRecord.wFrCellTemp4 = wTemp;
}

