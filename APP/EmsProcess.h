/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		EmsProcess.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __EMS_PROCESS_H__
#define __EMS_PROCESS_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef  __EMS_PROCESS_C__
#define EMS_PROCESS
#else
#define EMS_PROCESS extern
#endif

struct EMS_DATA_FEILD
{
	INT16U ChargeVoltageLimit;		// 0.1V
	INT16U DischargeVoltageLimit;	// 0.1V
	INT16U ChargeCurrentLimit;		// 0.1A
	INT16U DischargeCurrentLimit;	// 0.1A
	INT16U StatusHigh;
	INT16U StatusLow;
	INT16U FaultFlagHigh;
	INT16U FaultFlagLow;
	INT16U AlarmFlagHigh;
	INT16U AlarmFlagLow;
	INT16U NoticeFlagHigh;
	INT16U NoticeFlagLow;
	INT16S TotalCurrent;
	INT16U TotalVoltage;
	INT16U Reservd01;
	INT16U Reservd02;
	INT16U TotalSOC;
	INT16U TotalSOH;
	INT16U TotalCapacityHigh;
	INT16U TotalCapacityLow;
	INT16U ParallelNumber;
	INT16U ParallelStatus;
	INT16U SerialNumber;
	INT16U SerialStatus;
	INT16U MaximumCellVoltageNo;
	INT16U MaximumCellVoltage;
	INT16U MinimumCellVoltageNo;
	INT16U MinimumCellVoltage;
	INT16U MaximumCellTemperatureNo;
	INT16S MaximumCellTemperature;
	INT16U MinimumCellTemperatureNo;
	INT16S MinimumCellTemperature;
};

#define	cEMS_DATA_FEILD_LEN			sizeof(struct EMS_DATA_FEILD)/2		// 32words

union EMS_DATA_TABLE
{
	struct	EMS_DATA_FEILD	Field;
	INT16U	Table[cEMS_DATA_FEILD_LEN];
};

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
EMS_PROCESS	void	sEmsInitial(void);
EMS_PROCESS	void	sEmsDataUpdate(void);
EMS_PROCESS	void	sEmsCurrentStatusChk(INT16U wFilter1, INT16U wFilter2);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
EMS_PROCESS	INT16U	swGetEmsData(INT16U wAddrOffset);
EMS_PROCESS	INT16U	swGetEmsStatusChargeEnable(void);
EMS_PROCESS	INT16U	swGetEmsStatusDischargeEnable(void);
EMS_PROCESS	INT16U	swGetEmsStatusFullCharge(void);
EMS_PROCESS	INT16U	swGetEmsStatusChargeImmediately1(void);
EMS_PROCESS	INT16U	swGetEmsStatusCharging(void);
EMS_PROCESS	INT16U	swGetEmsStatusDischarging(void);
EMS_PROCESS	INT16U	swGetEmsDataChargeVoltageLimit(void);
EMS_PROCESS	INT16U	swGetEmsDataDischargeVoltageLimit(void);
EMS_PROCESS	INT16U	swGetEmsDataChargeCurrentLimit(void);
EMS_PROCESS	INT16U	swGetEmsDataDischargeCurrentLimit(void);
EMS_PROCESS	INT16U	swGetEmsDataStatusHigh(void);
EMS_PROCESS	INT16U	swGetEmsDataStatusLow(void);
EMS_PROCESS	INT16U	swGetEmsDataFaultFlagHigh(void);
EMS_PROCESS	INT16U	swGetEmsDataFaultFlagLow(void);
EMS_PROCESS	INT16U	swGetEmsDataAlarmFlagHigh(void);
EMS_PROCESS	INT16U	swGetEmsDataAlarmFlagLow(void);
EMS_PROCESS	INT16U	swGetEmsDataNoticeFlagHigh(void);
EMS_PROCESS	INT16U	swGetEmsDataNoticeFlagLow(void);
EMS_PROCESS	INT16S	swGetEmsDataTotalCurrent(void);
EMS_PROCESS	INT16U	swGetEmsDataTotalVoltage(void);
EMS_PROCESS	INT16U	swGetEmsDataReservd01(void);
EMS_PROCESS	INT16U	swGetEmsDataReservd02(void);
EMS_PROCESS	INT16U	swGetEmsDataTotalSOC(void);
EMS_PROCESS	INT16U	swGetEmsDataTotalSOH(void);
EMS_PROCESS	INT16U	swGetEmsDataTotalCapacityHigh(void);
EMS_PROCESS	INT16U	swGetEmsDataTotalCapacityLow(void);
EMS_PROCESS	INT16U	swGetEmsDataMaximumCellVoltageNo(void);
EMS_PROCESS	INT16U	swGetEmsDataMaximumCellVoltage(void);
EMS_PROCESS	INT16U	swGetEmsDataMinimumCellVoltageNo(void);
EMS_PROCESS	INT16U	swGetEmsDataMinimumCellVoltage(void);
EMS_PROCESS	INT16U	swGetEmsDataMaximumCellTemperatureNo(void);
EMS_PROCESS	INT16S	swGetEmsDataMaximumCellTemperature(void);
EMS_PROCESS	INT16U	swGetEmsDataMinimumCellTemperatureNo(void);
EMS_PROCESS	INT16S	swGetEmsDataMinimumCellTemperature(void);
EMS_PROCESS	INT16U	swGetEmsDataParallelNumber(void);
EMS_PROCESS	INT16U	swGetEmsDataParallelStatus(void);
EMS_PROCESS	INT16U	swGetEmsDataSerialNumber(void);
EMS_PROCESS	INT16U	swGetEmsDataSerialStatus(void);
EMS_PROCESS	INT8U	sbGetEmsIdleNumber(void);
EMS_PROCESS	INT8U	sbGetEmsChargingNumber(void);
EMS_PROCESS	INT8U	sbGetEmsDischargingNumber(void);
EMS_PROCESS	INT8U	sbGetEmsSocFullNumber(void);
EMS_PROCESS	INT8U	sbGetEmsSocEmptyNumber(void);

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
EMS_PROCESS	void	sSetEmsDataChargeVoltageLimit(INT16U wData);
EMS_PROCESS	void	sSetEmsDataDischargeVoltageLimit(INT16U wData);
EMS_PROCESS	void	sSetEmsDataChargeCurrentLimit(INT16U wData);
EMS_PROCESS	void	sSetEmsDataDischargeCurrentLimit(INT16U wData);
EMS_PROCESS	void	sSetEmsDataStatusHigh(INT16U wData);
EMS_PROCESS	void	sSetEmsDataStatusLow(INT16U wData);
EMS_PROCESS	void	sSetEmsDataFaultFlagHigh(INT16U wData);
EMS_PROCESS	void	sSetEmsDataFaultFlagLow(INT16U wData);
EMS_PROCESS	void	sSetEmsDataAlarmFlagHigh(INT16U wData);
EMS_PROCESS	void	sSetEmsDataAlarmFlagLow(INT16U wData);
EMS_PROCESS	void	sSetEmsDataNoticeFlagHigh(INT16U wData);
EMS_PROCESS	void	sSetEmsDataNoticeFlagLow(INT16U wData);
EMS_PROCESS	void	sSetEmsDataTotalCurrent(INT16U wData);
EMS_PROCESS	void	sSetEmsDataTotalVoltage(INT16U wData);
EMS_PROCESS	void	sSetEmsDataReservd01(INT16U wData);
EMS_PROCESS	void	sSetEmsDataReservd02(INT16U wData);
EMS_PROCESS	void	sSetEmsDataTotalSOC(INT16U wData);
EMS_PROCESS	void	sSetEmsDataTotalSOH(INT16U wData);
EMS_PROCESS	void	sSetEmsDataTotalCapacityHigh(INT16U wData);
EMS_PROCESS	void	sSetEmsDataTotalCapacityLow(INT16U wData);
EMS_PROCESS	void	sSetEmsDataMaximumCellVoltageNo(INT16U wData);
EMS_PROCESS	void	sSetEmsDataMaximumCellVoltage(INT16U wData);
EMS_PROCESS	void	sSetEmsDataMinimumCellVoltageNo(INT16U wData);
EMS_PROCESS	void	sSetEmsDataMinimumCellVoltage(INT16U wData);
EMS_PROCESS	void	sSetEmsDataMaximumCellTemperatureNo(INT16U wData);
EMS_PROCESS	void	sSetEmsDataMaximumCellTemperature(INT16U wData);
EMS_PROCESS	void	sSetEmsDataMinimumCellTemperatureNo(INT16U wData);
EMS_PROCESS	void	sSetEmsDataMinimumCellTemperature(INT16U wData);
EMS_PROCESS	void	sSetEmsDataParallelNumber(INT16U wData);
EMS_PROCESS	void	sSetEmsDataParallelStatus(INT16U wData);
EMS_PROCESS	void	sSetEmsDataSerialNumber(INT16U wData);
EMS_PROCESS	void	sSetEmsDataSerialStatus(INT16U wData);

#endif	// __EMS_PROCESS_H__

