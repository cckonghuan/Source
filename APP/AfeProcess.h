/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		AfeProcess.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __AFE_PROCESS_H__
#define __AFE_PROCESS_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef  __AFE_PROCESS_C__
#define AFE_PROCESS
#else
#define AFE_PROCESS extern
#endif


#ifdef VIRTUAL_ADC_SAMPLE
#define	UPDATE_BATT_VOLT_AFE		0	// 0=电池电压由MCU采集
#define	UPDATE_BATT_CURR_AFE		0	// 0=电池电流由MCU采集
#define	UPDATE_CELL_VOLT_AFE		0	// 0=电芯电压由MCU采集
#define	UPDATE_CELL_TEMP_AFE		0	// 0=电芯温度由MCU采集
#define	UPDATE_ALARM_STATUS_AFE		0	// 0=告警状态由MCU检测
#define	UPDATE_CURR_STATUS_AFE		0	// 0=充放状态由MCU检测
#define	UPDATE_BAL_CTRL_MODE		1	// 0=全部AFE控制,1=使能MCU控制/逻辑AFE控制
#else
#define	UPDATE_BATT_VOLT_AFE		1	// 1=电池电压由AFE采集
#define	UPDATE_BATT_CURR_AFE		1	// 1=电池电流由AFE采集
#define	UPDATE_CELL_VOLT_AFE		1	// 1=电芯电压由AFE采集
#define	UPDATE_CELL_TEMP_AFE		0	// 0=电芯温度由MCU采集
#define	UPDATE_ALARM_STATUS_AFE		0	// 0=告警状态由MCU检测
#define	UPDATE_CURR_STATUS_AFE		0	// 0=充放状态由MCU检测
#define	UPDATE_BAL_CTRL_MODE		1	// 0=全部AFE控制,1=使能MCU控制/逻辑AFE控制
#endif

#define	AFE_INIT_WRITTING		0
#define	AFE_INIT_WRITE_PASS		1
#define	AFE_INIT_WRITE_FAIL		2

#define	AFE_EEPROM_ADDR			0x00
#define	AFE_EEPROM_LEN			0x1A
#define	AFE_RAM_ADDR			0x40
#define	AFE_RAM_LEN				0x33

#define	AFE_EEPROM_OTC			0x11
#define	AFE_EEPROM_OTCR			0x12
#define	AFE_EEPROM_UTC			0x13
#define	AFE_EEPROM_UTCR			0x14
#define	AFE_EEPROM_OTD			0x15
#define	AFE_EEPROM_OTDR			0x16
#define	AFE_EEPROM_UTD			0x17
#define	AFE_EEPROM_UTDR			0x18
#define	AFE_EEPROM_TR			0x19

#define	AFE_RAM_CONF			0x40
#define	AFE_RAM_BALANCEH		0x41
#define	AFE_RAM_BALANCEL		0x42
#define	AFE_RAM_BSTATUS1		0x43
#define	AFE_RAM_BSTATUS2		0x44
#define	AFE_RAM_BSTATUS3		0x45
#define	AFE_RAM_TEMP1			0x46
#define	AFE_RAM_TEMP2			0x48
#define	AFE_RAM_TEMP3			0x4A
#define	AFE_RAM_CURR			0x4C
#define	AFE_RAM_CELL1			0x4E
#define	AFE_RAM_CELL2			0x50
#define	AFE_RAM_CELL3			0x52
#define	AFE_RAM_CELL4			0x54
#define	AFE_RAM_CELL5			0x56
#define	AFE_RAM_CELL6			0x58
#define	AFE_RAM_CELL7			0x5A
#define	AFE_RAM_CELL8			0x5C
#define	AFE_RAM_CELL9			0x5E
#define	AFE_RAM_CELL10			0x60
#define	AFE_RAM_CELL11			0x62
#define	AFE_RAM_CELL12			0x64
#define	AFE_RAM_CELL13			0x66
#define	AFE_RAM_CELL14			0x68
#define	AFE_RAM_CELL15			0x6A
#define	AFE_RAM_CELL16			0x6C
#define	AFE_RAM_ADC2			0x6E
#define	AFE_RAM_BFLAG1			0x70
#define	AFE_RAM_BFLAG2			0x71
#define	AFE_RAM_RSTSTAT			0x72

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
AFE_PROCESS	void	sAfeInitial(void);
AFE_PROCESS	void	sAfeRamProc(INT8U bFilter);
AFE_PROCESS	INT8U	sbAfeEepromProc(INT8U bFilter);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
AFE_PROCESS	INT8U	sbGetAfeEepromInitAlarm(void);
AFE_PROCESS	INT8U	sbGetAfeRamRwAlarm(void);
AFE_PROCESS	INT8U	sbGetAfeEepromWrAlarm(void);
AFE_PROCESS	INT16U	swGetAfeRamData(INT16U RdAddr);
AFE_PROCESS	INT16U	swGetAfeEepromData(INT16U RdAddr);

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
AFE_PROCESS	void	sSetAfeEepromInitAlarm(INT8U bStatus);
AFE_PROCESS	void	sSetAfeRamRwAlarm(INT8U bStatus);
AFE_PROCESS	void	sSetAfeEepromWrAlarm(INT8U bStatus);

#endif	// __AFE_PROCESS_H__

