/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		Main.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define	__MAIN_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/


/********************************************************************************
* Constants																		*
********************************************************************************/
const INT8U RANDOM1[4] __attribute__((at(FLASH_APP_RANDOM1_ADDR))) = {0xFF,0xFF,0xFF,0xFF};
const INT8U RANDOM2[4] __attribute__((at(FLASH_APP_RANDOM2_ADDR))) = {0xFF,0xFF,0xFF,0xFF};
const INT8U APPTIME1[11] __attribute__((at(FLASH_APP_TIME_ADDR))) = __DATE__;
const INT8U APPTIME2[1] __attribute__((at(FLASH_APP_TIME_ADDR+11))) = {0x20};
const INT8U APPTIME3[8] __attribute__((at(FLASH_APP_TIME_ADDR+12))) = __TIME__;

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


/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/


/********************************************************************************
* Routines' implementations														*
********************************************************************************/
int main(void)
{
	NVIC_ConfigVectorTable(NVIC_VECT_TAB_FLASH, FLASH_APP_ADDR_START - NVIC_VECT_TAB_FLASH);
	
	sSYSTEM_Config();
	
	sSYSTICK_Config();
	
	sAdcInitial();
	
	sSciInitial(cHOST_SCI, MAX_SCI1_RX_SIZE,cPrioInterface, eHostParsing);
	sEepromTaskInitial(cEEPROM_STATIC_NO, cPrioInterface, eEepromStaticSave);
	sEepromTaskInitial(cEEPROM_DYNAMIC_NO, cPrioInterface, eEepromDynamicSave);
	sEepromTaskInitial(cEEPROM_REALTIME_NO, cPrioInterface, eEepromRealtimeSave);
	sEepromTaskInitial(cEEPROM_FAULT_RECORD_NO, cPrioInterface, eEepromFaultRecordSave);
	
	sNVIC_Config();
	
	OSInit();
	
	OSTaskCreate(sSuperJob,MaxSuperJobTaskSize,cPrioSuper);
	OSTimerStart(cPrioSuper,0,Timer_20ms);
	
	OSTaskCreate(sGeneralJob,MaxGeneralJobTaskSize,cPrioGeneral);
	OSTimerStart(cPrioGeneral,1,Timer_100ms);
	
	OSTaskCreate(sInterfaceJob,MaxInterfaceJobTaskSize,cPrioInterface);
	OSTimerStart(cPrioInterface,14,Timer_20ms);
	
	OSTaskCreate(sIdleJob,MaxIdleJobTaskSize,cPrioIdle);
	OSTimerStart(cPrioIdle,18,Timer_100ms);
	
	OSStart();
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/


/********************************************************************************
* Input interface Routines														*
********************************************************************************/


