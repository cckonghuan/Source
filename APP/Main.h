/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		Main.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef	__MAIN_H__
#define __MAIN_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	<stdio.h>
#include	<string.h>
#include	<ctype.h>
#include	<stdlib.h>
#include	<stdarg.h>
#include	<stdint.h>

#include	"Initial.h"
#include	"apm32e10x.h"

#include	"OS_Config.h"
#include	"OS_HEAD.H"
#include	"OS_CPU.h"

#include	"TwiDriver.h"
#include	"sci_driver.h"
#include	"eeprom1_driver.h"
#include	"eeprom2_driver.h"
#include	"eeprom3_driver.h"
#include	"can_driver.h"
#include	"lcd_driver.h"

#include	"SuperJob.h"
#include	"GeneralJob.h"
#include	"InterfaceJob.h"
#include	"IdleJob.h"

#include	"Module.h"
#include	"AdcProcess.h"
#include	"AfeProcess.h"
#include	"BmsProcess.h"
#include	"EmsProcess.h"
#include	"KeyProcess.h"
#include	"SoxProcess.h"
#include	"EepromProcess.h"
#include	"ModbusProcess.h"
#include	"BalanceProcess.h"
#include	"DisplayProcess.h"
#include	"CanProcess.h"
#include	"CanProtocolNew.h"
#include	"PylontechProtocolCan.h"
#include	"SciRoute.h"
#include	"LcdProcess.h"
#include	"AntiThief.h"
#include	"PylontechProtocol.h"
/********************************************************************************
* Macros 																		*
********************************************************************************/
#define	MACHINE_TYPE					0x0070
#define	MACHINE_SUB_TYPE				0x0000	// 机型出厂前扫码设置
#define	MACHINE_HW_VERSION				10	// for test
#define	MACHINE_SW_VERSION				113	// for test
#define	MODBUS_PROT_VERSION				100	// 0-199原对外协议,200优化后对外协议
#define	CAN_PROT_VERSION				100	// 0-199原CAN并机协议,200优化后并机协议
#define	MACHINE_DBG_VERSION				0	// 量产时复位为0	// for test
#define	LED_SOC_HW_VERSION				1	// 0=SOC中存在红灯,1=取消SOC中的红灯
#define	BMS_PARALLEL_NUM_MAX			16	// 1-32,最大只支持32个

// For APM32E103RC
// Total RAM = 0x20000000 - 0x2000FFFF = 0x10000  64kBytes
// Total ROM = 0x08000000 - 0x0803FFFF = 0x40000 256kBytes
// 0x08000000 - 0x08003FFF = 0x04000  16kBytes, use for IAP
// 0x08004000 - 0x08033FFF = 0x30000 192kBytes, use for APP
// 0x08034000 - 0x08039FFF = 0x06000  24kBytes, use for Reserved(预留)
// 0x0803A000 - 0x0803BFFF = 0x02000   8kBytes, use for EEPROM1(出厂参数/用户参数)
// 0x0803C000 - 0x0803DFFF = 0x02000   8kBytes, use for EEPROM2(自动记录数据)
// 0x0803E000 - 0x0803FFFF = 0x02000   8kBytes, use for EEPROM3(自动记录数据备份)
#define	RAM_ADDR_START			0x20000000
#define	RAM_ADDR_END			0x2000FFFF
#define	RAM_SIZE_MAX			(RAM_ADDR_END-RAM_ADDR_START+1)
#define	FLASH_ADDR_START		0x08000000
#define	FLASH_ADDR_END			0x0803FFFF
#define	FLASH_SIZE_MAX			(FLASH_ADDR_END-FLASH_ADDR_START+1)

#define	FLASH_IAP_ADDR_START	0x08000000
#define	FLASH_IAP_ADDR_END		0x08003FFF
#define	FLASH_APP_ADDR_START	0x08004000
#define	FLASH_APP_ADDR_END		0x08033FFF
#define FLASH_FLG_ADDR_START	0x08004144	// don't modify
#define FLASH_FLG_ADDR_END		0x08004147	// don't modify
#define FLASH_EEP1_ADDR_START	0x0803A000
#define FLASH_EEP1_ADDR_END		0x0803BFFF
#define FLASH_EEP2_ADDR_START	0x0803C000
#define FLASH_EEP2_ADDR_END		0x0803DFFF
#define FLASH_EEP3_ADDR_START	0x0803E000
#define FLASH_EEP3_ADDR_END		0x0803FFFF

#define FLASH_IAP_INFO_ADDR		0x08000C00
#define FLASH_IAP_NAME_ADDR		0x08000C60
#define FLASH_IAP_VER_ADDR		0x08000C68
#define FLASH_IAP_TIME_ADDR		0x08000C70
#define FLASH_IAP_ENCRYPT_ADDR	0x08000C90
#define FLASH_IAP_ROLLCODE_ADDR	0x08000C98
#define FLASH_APP_RANDOM1_ADDR	0x08012000
#define FLASH_APP_RANDOM2_ADDR	0x08012400
#define FLASH_APP_TIME_ADDR		0x08012800
#define FLASH_IAP_FLG_ADDR		FLASH_FLG_ADDR_START

#if(FLASH_IAP_ADDR_START & 0x7FF)
#error ERROR: FLASH_IAP_ADDR_START must be a multiple of 2048 bytes!
#endif

#if(FLASH_APP_ADDR_START & 0x7FF)
#error ERROR: FLASH_APP_ADDR_START must be a multiple of 2048 bytes!
#endif

#if(FLASH_EEP1_ADDR_START & 0x7FF)
#error ERROR: FLASH_EEP1_ADDR_START must be a multiple of 2048 bytes!
#endif

#if(FLASH_EEP2_ADDR_START & 0x7FF)
#error ERROR: FLASH_EEP2_ADDR_START must be a multiple of 2048 bytes!
#endif

#if(FLASH_EEP3_ADDR_START & 0x7FF)
#error ERROR: FLASH_EEP3_ADDR_START must be a multiple of 2048 bytes!
#endif

#if((FLASH_IAP_INFO_ADDR < FLASH_IAP_ADDR_START) || (FLASH_IAP_INFO_ADDR > FLASH_IAP_ADDR_END))
#error ERROR: FLASH_IAP_INFO_ADDR Must be in range of FLASH_IAP_ADDR_START to FLASH_IAP_ADDR_END!
#endif

#if((FLASH_IAP_NAME_ADDR < FLASH_IAP_ADDR_START) || (FLASH_IAP_NAME_ADDR > FLASH_IAP_ADDR_END))
#error ERROR: FLASH_IAP_NAME_ADDR Must be in range of FLASH_IAP_ADDR_START to FLASH_IAP_ADDR_END!
#endif

#if((FLASH_IAP_VER_ADDR < FLASH_IAP_ADDR_START) || (FLASH_IAP_VER_ADDR > FLASH_IAP_ADDR_END))
#error ERROR: FLASH_IAP_VER_ADDR Must be in range of FLASH_IAP_ADDR_START to FLASH_IAP_ADDR_END!
#endif

#if((FLASH_IAP_TIME_ADDR < FLASH_IAP_ADDR_START) || (FLASH_IAP_TIME_ADDR > FLASH_IAP_ADDR_END))
#error ERROR: FLASH_IAP_TIME_ADDR Must be in range of FLASH_IAP_ADDR_START to FLASH_IAP_ADDR_END!
#endif

#if((FLASH_IAP_ENCRYPT_ADDR < FLASH_IAP_ADDR_START) || (FLASH_IAP_ENCRYPT_ADDR > FLASH_IAP_ADDR_END))
#error ERROR: FLASH_IAP_ENCRYPT_ADDR Must be in range of FLASH_IAP_ADDR_START to FLASH_IAP_ADDR_END!
#endif

#if((FLASH_IAP_ROLLCODE_ADDR < FLASH_IAP_ADDR_START) || (FLASH_IAP_ROLLCODE_ADDR > FLASH_IAP_ADDR_END))
#error ERROR: FLASH_IAP_ROLLCODE_ADDR Must be in range of FLASH_IAP_ADDR_START to FLASH_IAP_ADDR_END!
#endif

#if((FLASH_APP_RANDOM1_ADDR <= FLASH_FLG_ADDR_END) || (FLASH_APP_RANDOM1_ADDR > FLASH_APP_ADDR_END))
#error ERROR: FLASH_APP_RANDOM1_ADDR Must be in range of FLASH_APP_ADDR_START to FLASH_APP_ADDR_END!
#endif

#if((FLASH_APP_RANDOM2_ADDR <= FLASH_FLG_ADDR_END) || (FLASH_APP_RANDOM2_ADDR > FLASH_APP_ADDR_END))
#error ERROR: FLASH_APP_RANDOM2_ADDR Must be in range of FLASH_APP_ADDR_START to FLASH_APP_ADDR_END!
#endif

#if((FLASH_APP_TIME_ADDR <= FLASH_FLG_ADDR_END) || (FLASH_APP_TIME_ADDR > FLASH_APP_ADDR_END))
#error ERROR: FLASH_APP_TIME_ADDR Must be in range of FLASH_APP_ADDR_START to FLASH_APP_ADDR_END!
#endif

#if((FLASH_IAP_FLG_ADDR < FLASH_FLG_ADDR_START) || (FLASH_IAP_FLG_ADDR > FLASH_FLG_ADDR_END))
#error ERROR: FLASH_IAP_FLG_ADDR Must be in range of FLASH_FLG_ADDR_START to FLASH_FLG_ADDR_END!
#endif

// Task priority define
#define cPrioSuper						0
#define cPrioGeneral					1
#define cPrioInterface					2
#define cPrioIdle						3

// Task stack size define
#define MaxSuperJobTaskSize				256//250
#define MaxGeneralJobTaskSize			256//250
#define MaxInterfaceJobTaskSize			256//250
#define MaxIdleJobTaskSize				256//250

// Task timer define
#define Timer_1ms						1
#define Timer_2ms						2
#define Timer_5ms						5
#define Timer_10ms						10
#define Timer_20ms						20
#define Timer_50ms						50
#define Timer_100ms						100
#define Timer_200ms						200
#define Timer_250ms						250
#define Timer_500ms						500
#define Timer_1sec						1000
#define Timer_2sec						2000

// SUPER task event define
#define	eSuperTimer						0
#define	eSuperToStandby					1
#define	eSuperToBattery					2
#define	eSuperToCharge					3
#define	eSuperToDischarge				4
#define	eSuperToSleep					5
#define	eSuperToFault					6
#define	eSuperToShutdown				7
#define	eSuperToTest					8
#define	eSuperToUpgrade					9
#define	eSuperToPowerOn					10

// SYNC task event define
#define	eSyncTimer						0

// GENERAL task event define
#define	eGeneralTimer                   0
#define	eAdcAvgCalc                     1

// INTERFACE task event define
#define	eInterfaceTimer					0
#define	eHostParsing					1
#define	eWifiParsing					2
#define	eEepromStaticSave				3
#define	eEepromDynamicSave				4
#define	eEepromRealtimeSave				5
#define	eEepromFaultRecordSave			6
#define	eDisplaySocSync					7
#define	eDisplayKeySync					8
#define	ePcsParsing						9
#define	eCanAutoAssign					10

// IDLE task event define
#define	eIdleTimer						0

/********************************************************************************
* Routines' implementations														*
********************************************************************************/


/********************************************************************************
* Output interface Routines														*
********************************************************************************/


/********************************************************************************
* Input interface Routines														*
********************************************************************************/


#endif	// __MAIN_H__

