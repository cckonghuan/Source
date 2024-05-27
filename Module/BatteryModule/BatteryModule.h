/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		BatteryModule.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __BATTERY_MODULE_H__
#define __BATTERY_MODULE_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef  __BATTERY_MODULE_C__
#define BATTERY
#else
#define BATTERY extern
#endif

#define         cBATTERY_IDLE				0
#define         cBATTERY_DISCHARGING		1
#define         cBATTERY_CHARGING			2

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
BATTERY	void    sBatteryModuleInit(void);
BATTERY	void    sBatteryParaUpdate(void);
BATTERY	void    sScanCellVoltAvgMaxMin(INT8U bNum);
BATTERY	void    sScanCellTempAvgMaxMin(INT8U bNum);
BATTERY	void    sBattVoltHiChk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sBattVoltLoChk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sCellVoltHiAlmChk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sCellVoltHiChk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sCellVoltHi2Chk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sCellVoltLoAlmChk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sCellVoltLoChk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sCellVoltLo2Chk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sBattChgCurrHiAlmChk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sBattChgCurrHiChk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sBattChgCurrHi2Chk(INT16U wFilte1, INT16U wFilter2);
BATTERY	void    sBattDcgCurrHiAlmChk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sBattDcgCurrHiChk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sBattDcgCurrHi2Chk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sCellChgTempHiAlmChk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sCellChgTempHiChk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sCellChgTempHi2Chk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sCellChgTempLoAlmChk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sCellChgTempLoChk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sCellChgTempLo2Chk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sCellDcgTempHiAlmChk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sCellDcgTempHiChk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sCellDcgTempHi2Chk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sCellDcgTempLoAlmChk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sCellDcgTempLoChk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sCellDcgTempLo2Chk(INT16U wFilter1, INT16U wFilter2);
BATTERY	void    sCellVoltErrHiAlmChk(INT16U wFilter1, INT16U wFilter2);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
BATTERY	INT16S	swGetBattVoltNew(void);
BATTERY	INT16S	swGetBattCurrNew(void);
BATTERY	INT16S	swGetCellVoltNew(INT8U bNo);
BATTERY	INT16S	swGetCellTempNew(INT8U bNo);
BATTERY	INT16S	swGetBattVoltFiltNew(void);
BATTERY	INT16S	swGetBattCurrFiltNew(void);
BATTERY	INT16S	swGetCellVoltFiltNew(INT8U bNo);
BATTERY	INT16S	swGetCellTempFiltNew(INT8U bNo);
BATTERY	INT16S	swGetAvgCellVoltNew(void);
BATTERY	INT16S	swGetMaxCellVoltNew(void);
BATTERY	INT8U	sbGetMaxCellVoltNo(void);
BATTERY	INT16S	swGetMinCellVoltNew(void);
BATTERY	INT8U	sbGetMinCellVoltNo(void);
BATTERY	INT16S	swGetAvgCellTempNew(void);
BATTERY	INT16S	swGetMaxCellTempNew(void);
BATTERY	INT8U	sbGetMaxCellTempNo(void);
BATTERY	INT16S	swGetMinCellTempNew(void);
BATTERY	INT8U	sbGetMinCellTempNo(void);
BATTERY	INT16S	swGetAvgCellVoltFiltNew(void);
BATTERY	INT16S	swGetMaxCellVoltFiltNew(void);
BATTERY	INT8U	sbGetMaxCellVoltFiltNo(void);
BATTERY	INT16S	swGetMinCellVoltFiltNew(void);
BATTERY	INT8U	sbGetMinCellVoltFiltNo(void);
BATTERY	INT16S	swGetAvgCellTempFiltNew(void);
BATTERY	INT16S	swGetMaxCellTempFiltNew(void);
BATTERY	INT8U	sbGetMaxCellTempFiltNo(void);
BATTERY	INT16S	swGetMinCellTempFiltNew(void);
BATTERY	INT8U	sbGetMinCellTempFiltNo(void);
BATTERY	INT8U	sbGetBattVoltHi(void);
BATTERY	INT8U	sbGetBattVoltLo(void);
BATTERY	INT8U	sbGetCellVoltHiAlm(void);
BATTERY	INT8U	sbGetCellVoltHi(void);
BATTERY	INT8U	sbGetCellVoltHi2(void);
BATTERY	INT8U	sbGetCellVoltLoAlm(void);
BATTERY	INT8U	sbGetCellVoltLo(void);
BATTERY	INT8U	sbGetCellVoltLo2(void);
BATTERY	INT8U	sbGetBattChgCurrHiAlm(void);
BATTERY	INT8U	sbGetBattChgCurrHi(void);
BATTERY	INT8U	sbGetBattChgCurrHi2(void);
BATTERY	INT8U	sbGetBattDcgCurrHiAlm(void);
BATTERY	INT8U	sbGetBattDcgCurrHi(void);
BATTERY	INT8U	sbGetBattDcgCurrHi2(void);
BATTERY	INT8U	sbGetCellChgTempHiAlm(void);
BATTERY	INT8U	sbGetCellChgTempHi(void);
BATTERY	INT8U	sbGetCellChgTempHi2(void);
BATTERY	INT8U	sbGetCellChgTempLoAlm(void);
BATTERY	INT8U	sbGetCellChgTempLo(void);
BATTERY	INT8U	sbGetCellChgTempLo2(void);
BATTERY	INT8U	sbGetCellDcgTempHiAlm(void);
BATTERY	INT8U	sbGetCellDcgTempHi(void);
BATTERY	INT8U	sbGetCellDcgTempHi2(void);
BATTERY	INT8U	sbGetCellDcgTempLoAlm(void);
BATTERY	INT8U	sbGetCellDcgTempLo(void);
BATTERY	INT8U	sbGetCellDcgTempLo2(void);
BATTERY	INT8U	sbGetCellVoltErrHiAlm(void);
BATTERY	INT8U	sbGetCellTempHiAlm(void);
BATTERY	INT8U	sbGetCellTempLoAlm(void);

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
BATTERY	void    sSetBattVoltNew(INT16S wVolt);
BATTERY	void    sSetBattCurrNew(INT16S wCurr);
BATTERY	void    sSetCellVoltNew(INT8U bNo, INT16S wVolt);
BATTERY	void    sSetCellTempNew(INT8U bNo, INT16S wTemp);
BATTERY	void    sSetBattVoltFiltNew(INT16S wVolt);
BATTERY	void    sSetBattCurrFiltNew(INT16S wCurr);
BATTERY	void    sSetCellVoltFiltNew(INT8U bNo, INT16S wVolt);
BATTERY	void    sSetCellTempFiltNew(INT8U bNo, INT16S wTemp);
BATTERY	void    sSetBatteryStatus(INT8U bStatus);
BATTERY	void    sSetBattVoltLo2Disable(INT8U bStatus);
BATTERY	void    sSetCellVoltLo2Disable(INT8U bStatus);
BATTERY	void    sSetCellVoltHi(INT8U bStatus);
BATTERY	void    sSetCellVoltHi2(INT8U bStatus);
BATTERY	void    sSetCellVoltLo(INT8U bStatus);
BATTERY	void    sSetCellVoltLo2(INT8U bStatus);
BATTERY	void    sSetBattChgCurrHi(INT8U bStatus);
BATTERY	void    sSetBattChgCurrHi2(INT8U bStatus);
BATTERY	void    sSetBattDcgCurrHi(INT8U bStatus);
BATTERY	void    sSetBattDcgCurrHi2(INT8U bStatus);
BATTERY	void    sSetCellChgTempHi(INT8U bStatus);
BATTERY	void    sSetCellChgTempHi2(INT8U bStatus);
BATTERY	void    sSetCellChgTempLo(INT8U bStatus);
BATTERY	void    sSetCellChgTempLo2(INT8U bStatus);
BATTERY	void    sSetCellDcgTempHi(INT8U bStatus);
BATTERY	void    sSetCellDcgTempHi2(INT8U bStatus);
BATTERY	void    sSetCellDcgTempLo(INT8U bStatus);
BATTERY	void    sSetCellDcgTempLo2(INT8U bStatus);

#endif	// __BATTERY_MODULE_H__

