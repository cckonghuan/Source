/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		AdcProcess.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __ADC_PROCESS_H__
#define __ADC_PROCESS_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef  __ADC_PROCESS_C__
#define ADC_PROCESS
#else
#define ADC_PROCESS  extern
#endif

//#define	cADC_CH_BAT_CURR		0
#define	cADC_CH_NTC_TEMP		0
#define	cADC_CH_VERSION1		1
#define	cADC_CH_CELL1_TEMP		2
#define	cADC_CH_CELL2_TEMP		3
#define	cADC_CH_CELL3_TEMP		4
#define	cADC_CH_CELL4_TEMP		5
#define	cADC_CH_NUM				6

#define	cADC_CELL_VOLT_CH01		0
#define	cADC_CELL_VOLT_CH02		1
#define	cADC_CELL_VOLT_CH03		2
#define	cADC_CELL_VOLT_CH04		3
#define	cADC_CELL_VOLT_CH05		4
#define	cADC_CELL_VOLT_CH06		5
#define	cADC_CELL_VOLT_CH07		6
#define	cADC_CELL_VOLT_CH08		7
#define	cADC_CELL_VOLT_CH09		8
#define	cADC_CELL_VOLT_CH10		9
#define	cADC_CELL_VOLT_CH11		10
#define	cADC_CELL_VOLT_CH12		11
#define	cADC_CELL_VOLT_CH13		12
#define	cADC_CELL_VOLT_CH14		13
#define	cADC_CELL_VOLT_CH15		14
#define	cADC_CELL_VOLT_CH16		15
#define	cADC_CELL_VOLT_MAX		16

#define	cADC_CELL_TEMP_CH01		0
#define	cADC_CELL_TEMP_CH02		1
#define	cADC_CELL_TEMP_CH03		2
#define	cADC_CELL_TEMP_CH04		3
#define	cADC_CELL_TEMP_CH05		4
#define	cADC_CELL_TEMP_CH06		5
#define	cADC_CELL_TEMP_CH07		6
#define	cADC_CELL_TEMP_CH08		7
#define	cADC_CELL_TEMP_MAX		8

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
ADC_PROCESS	void	sAdcInitial(void);
ADC_PROCESS	void	sAdcISR(void);
ADC_PROCESS	void	sAdcAvgCalc(void);
ADC_PROCESS	void	sAdcAvgFilter(INT8U bFilter);
ADC_PROCESS	void	sAdcCoeffInit(void);
ADC_PROCESS	void	sAdcOffsetInitial(void);
ADC_PROCESS	INT8U	sbAdcOffsetCalc(INT8U bFilter);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
ADC_PROCESS	INT16S	swGetAdcCurrOffset(void);
ADC_PROCESS	INT16S	swGetBattCurrSampAvg(void);
ADC_PROCESS	INT16S	swGetBattVoltSample(void);
ADC_PROCESS	INT16S	swGetBattCurrSample(void);
ADC_PROCESS	INT16S	swGetCellVoltSample(INT8U bNo);
ADC_PROCESS	INT16S	swGetCellTempSample(INT8U bNo);
ADC_PROCESS	INT16S	swGetHsTempSample(void);
ADC_PROCESS	INT16S	swGetHwVersion1Sample(void);
ADC_PROCESS	INT16S	swGetHwVersion2Sample(void);
ADC_PROCESS	INT16S	swGetPackVoltSample(void);
ADC_PROCESS	INT16S	swGetBattVoltReal(void);
ADC_PROCESS	INT16S	swGetBattCurrReal(void);
ADC_PROCESS	INT16S	swGetCellVoltReal(INT8U bNo);
ADC_PROCESS	INT16S	swGetCellTempReal(INT8U bNo);
ADC_PROCESS	INT16S	swGetHsTempReal(void);
ADC_PROCESS	INT16S	swGetHwVersion1Real(void);
ADC_PROCESS	INT16S	swGetHwVersion2Real(void);
ADC_PROCESS	INT16S	swGetPackVoltReal(void);
ADC_PROCESS	INT16S	swGetBattVoltRealFilt(void);
ADC_PROCESS	INT16S	swGetBattCurrRealFilt(void);
ADC_PROCESS	INT16S	swGetCellVoltRealFilt(INT8U bNo);
ADC_PROCESS	INT16S	swGetCellTempRealFilt(INT8U bNo);
ADC_PROCESS	INT16S	swGetCellVolt01Sample(void);
ADC_PROCESS	INT16S	swGetCellVolt02Sample(void);
ADC_PROCESS	INT16S	swGetCellVolt03Sample(void);
ADC_PROCESS	INT16S	swGetCellVolt04Sample(void);
ADC_PROCESS	INT16S	swGetCellVolt05Sample(void);
ADC_PROCESS	INT16S	swGetCellVolt06Sample(void);
ADC_PROCESS	INT16S	swGetCellVolt07Sample(void);
ADC_PROCESS	INT16S	swGetCellVolt08Sample(void);
ADC_PROCESS	INT16S	swGetCellVolt09Sample(void);
ADC_PROCESS	INT16S	swGetCellVolt10Sample(void);
ADC_PROCESS	INT16S	swGetCellVolt11Sample(void);
ADC_PROCESS	INT16S	swGetCellVolt12Sample(void);
ADC_PROCESS	INT16S	swGetCellVolt13Sample(void);
ADC_PROCESS	INT16S	swGetCellVolt14Sample(void);
ADC_PROCESS	INT16S	swGetCellVolt15Sample(void);
ADC_PROCESS	INT16S	swGetCellVolt16Sample(void);
ADC_PROCESS	INT16S	swGetCellTemp01Sample(void);
ADC_PROCESS	INT16S	swGetCellTemp02Sample(void);
ADC_PROCESS	INT16S	swGetCellTemp03Sample(void);
ADC_PROCESS	INT16S	swGetCellTemp04Sample(void);
ADC_PROCESS	INT16S	swGetCellTemp05Sample(void);
ADC_PROCESS	INT16S	swGetCellTemp06Sample(void);
ADC_PROCESS	INT16S	swGetCellTemp07Sample(void);
ADC_PROCESS	INT16S	swGetCellTemp08Sample(void);
ADC_PROCESS	INT16S	swGetCellVolt01Real(void);
ADC_PROCESS	INT16S	swGetCellVolt02Real(void);
ADC_PROCESS	INT16S	swGetCellVolt03Real(void);
ADC_PROCESS	INT16S	swGetCellVolt04Real(void);
ADC_PROCESS	INT16S	swGetCellVolt05Real(void);
ADC_PROCESS	INT16S	swGetCellVolt06Real(void);
ADC_PROCESS	INT16S	swGetCellVolt07Real(void);
ADC_PROCESS	INT16S	swGetCellVolt08Real(void);
ADC_PROCESS	INT16S	swGetCellVolt09Real(void);
ADC_PROCESS	INT16S	swGetCellVolt10Real(void);
ADC_PROCESS	INT16S	swGetCellVolt11Real(void);
ADC_PROCESS	INT16S	swGetCellVolt12Real(void);
ADC_PROCESS	INT16S	swGetCellVolt13Real(void);
ADC_PROCESS	INT16S	swGetCellVolt14Real(void);
ADC_PROCESS	INT16S	swGetCellVolt15Real(void);
ADC_PROCESS	INT16S	swGetCellVolt16Real(void);
ADC_PROCESS	INT16S	swGetCellTemp01Real(void);
ADC_PROCESS	INT16S	swGetCellTemp02Real(void);
ADC_PROCESS	INT16S	swGetCellTemp03Real(void);
ADC_PROCESS	INT16S	swGetCellTemp04Real(void);
ADC_PROCESS	INT16S	swGetCellTemp05Real(void);
ADC_PROCESS	INT16S	swGetCellTemp06Real(void);
ADC_PROCESS	INT16S	swGetCellTemp07Real(void);
ADC_PROCESS	INT16S	swGetCellTemp08Real(void);
ADC_PROCESS	INT16S	swGetBattCurrResult(void);

ADC_PROCESS	INT16U	*swGetAdcSimulationValue(void);

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
ADC_PROCESS	void	sSetBattVoltAdj(INT16S wAdj);
ADC_PROCESS	void	sSetBattVoltBias(INT16S wBias);
ADC_PROCESS	void	sSetBattChgCurrAdj(INT16S wAdj);
ADC_PROCESS	void	sSetBattChgCurrBias(INT16S wBias);
ADC_PROCESS	void	sSetBattDsgCurrAdj(INT16S wAdj);
ADC_PROCESS	void	sSetBattDsgCurrBias(INT16S wBias);
ADC_PROCESS	void	sSetCellVoltAdj(INT8U bNo, INT16S wAdj);
ADC_PROCESS	void	sSetCellVoltBias(INT8U bNo, INT16S wBias);
ADC_PROCESS	void	sSetPackVoltAdj(INT16S wAdj);
ADC_PROCESS	void	sSetPackVoltBias(INT16S wBias);
ADC_PROCESS	void	sSetBattVoltSample(INT16S wVolt);
ADC_PROCESS	void	sSetBattCurrSample(INT16S wCurr);
ADC_PROCESS	void	sSetCellVoltSample(INT8U bNo, INT16S wVolt);
ADC_PROCESS	void	sSetCellTempSample(INT8U bNo, INT16S wTemp);

#endif	// __ADC_PROCESS_H__

