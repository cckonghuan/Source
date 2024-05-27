/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		ModbusProcess.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __MODBUS_PROCESS_H__
#define __MODBUS_PROCESS_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef  __MODBUS_PROCESS_C__
#define MODBUS_PROCESS
#else
#define MODBUS_PROCESS  extern
#endif

#ifndef NULL
#define NULL	0x00
#endif

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
MODBUS_PROCESS	void	sModBusSciAddrInitChk(INT16U wTimeBase);
MODBUS_PROCESS	void	sModBusParsing(INT8U sciid);
MODBUS_PROCESS	void	sCommConnectChk(INT8U sciid, INT16U wFilter);
MODBUS_PROCESS  void	sGroWatt_ModBusParsing(INT8U sciid);
MODBUS_PROCESS  void	sGrowatt_ModBusRdProc(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U RxLen);
MODBUS_PROCESS  void	sGrowatt_ModBusWrProc(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U RxLen);
MODBUS_PROCESS  void	sGrowatt_BuildRdFrame(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT16U (*RdFunc)(INT16U));
MODBUS_PROCESS  void	sGrowatt_BuildWrFrame(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U (*WrFunc)(INT16U, INT16U));
MODBUS_PROCESS  void	sGrowatt_BuildFaultFrame(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U ErrCode);
MODBUS_PROCESS  INT16U  swGrowatt_BMSDataRead(INT16U RdAddr);
MODBUS_PROCESS  INT8U	swGrowatt_EESettingWrite(INT16U WrAddr, INT16U WrData);

MODBUS_PROCESS  void	sVoltronic_ModBusParsing(INT8U sciid);
MODBUS_PROCESS  void	sVoltronic_ModBusRdProc(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U RxLen);
MODBUS_PROCESS  void	sVoltronic_BuildRdFrame(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT16U (*RdFunc)(INT16U));
MODBUS_PROCESS  void	sVoltronic_BuildFaultFrame(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U ErrCode);
MODBUS_PROCESS  INT16U  swVoltronic_BMSDataRead(INT16U RdAddr);
/********************************************************************************
* Output interface Routines														*
********************************************************************************/
MODBUS_PROCESS	INT8U	sbGetCommConnected(INT8U sciid);

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
MODBUS_PROCESS	void	sSetModBusAddress(INT8U address);
MODBUS_PROCESS	void	sSetCommConnected(INT8U sciid, INT8U bStatus);

#endif	// __MODBUS_PROCESS_H__

