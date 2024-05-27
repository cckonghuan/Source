/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		ModbusProcess.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define		__MODBUS_PROCESS_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"Main.h"
#include	"sci_driver.h"
#include	"ModbusProcess.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
// 通用协议
#define INFO_REG_BASE_ADDR		0xF800				// 系统信息数据起始地址
#define INFO_REG_LEN			0x100				// 系统信息数据长度

// 0x1000~0x1FFF for BMS type
#define EMS_REG_BASE_ADDR		0x0100				// EMS数据起始地址
#define EMS_REG_LEN				0x80				// EMS数据长度

#define BMS_REG_BASE_ADDR		0x0200				// BMS数据起始地址
#define BMS_REG_LEN				0x80				// BMS数据长度

#define BMS2_REG_BASE_ADDR		0x1300				// BMS数据起始地址
#define BMS2_REG_LEN			0x80				// BMS数据长度

#define EE_REG_BASE_ADDR		0x2000				// EEPROM数据寄存器起始地址
#define EE_REG_BASE_LEN			0xC0				// EEPROM数据寄存器长度

#define ATE_REG_BASE_ADDR		0x3300				// ATE数据寄存器起始地址
#define ATE_REG_BASE_LEN		0x100				// ATE数据寄存器长度

#define FAULT_REG_BASE_ADDR		0x1400				// 故障数据寄存器起始地址
#define FAULT_REG_BASE_LEN		0x40				// 故障数据寄存器长度

#define DBG_CMD_BASE_ADDR		0x1500				// 调试命令起始地址
#define DBG_CMD_LEN				0x100				// 调试命令长度

#define Growatt_BMS_REG_BASE_ADDR		0			                // BMS数据起始地址
#define Growatt_BMS_REG_LEN				cBMS_Growatt_FEILD_LEN		// BMS数据长度

#define Voltronic_BMS_REG_BASE_ADDR		0x0001			                // BMS数据起始地址
#define Voltronic_BMS_REG_LEN			cBMS_Voltronic_FEILD_LEN		// BMS数据长度

// SH367309数据对外协议
#define AFE_RAM_REG_BASE_ADDR	0x1600				// 故障数据寄存器起始地址
#define AFE_RAM_REG_BASE_LEN	0x33				// 故障数据寄存器长度

#define AFE_EEP_REG_BASE_ADDR	0x1700				// 故障数据寄存器起始地址
#define AFE_EEP_REG_BASE_LEN	0x1A				// 故障数据寄存器长度

// 并联模块对外BMS数据
#define BMS_ALL_REG_BASE_ADDR	0xB000				// BMS模块数据起始地址
#define BMS_ONE_REG_LEN			0x80				// BMS单个模块数据长度
#define BMS_ALL_REG_LEN			(BMS_ONE_REG_LEN*BMS_PARALLEL_NUM_MAX)// BMS所有模块数据长度

#define cMODBUS_REG_RX_MAX_LEN			((cSCI_RX_LEN - 9) >> 1)
#define cMODBUS_REG_TX_MAX_LEN			((cSCI_TX_LEN - 5) >> 1)

#define cMODBUS_ADDR_DEF				0x01
#define cMODBUS_ADDR_MIN				0x01
#define cMODBUS_ADDR_MAX				0x20
#define cMODBUS_ADDR_HOST				0x00	// 0
#define cMODBUS_ADDR_BACKUP				0xFF	// 255
#define cMODBUS_ADDR_ERR				0x00	// 16

#define cMODBUS_ADDR					0x00
#define cMODBUS_CMD						0x01
#define cMODBUS_REG_ADDR_H				0x02
#define cMODBUS_REG_ADDR_L				0x03
#define cMODBUS_REG_NUM_H				0x04
#define cMODBUS_REG_NUM_L				0x05
#define cMODBUS_CRC_H					0x02
#define cMODBUS_CRC_L					0x01

#define cMODBUS_WRITE_DATA_START		0x04
#define cMODBUS_MULTI_WRITE_DATA_START	0x07

#define cMODBUS_MIN_LEN					0x08
#define cMODBUS_CRC_LEN					0x02

#define cREAD_CMD						0x03
#define cWRITE_CMD						0x06
#define cWRITE_CMD_MULTI				0x10

#define cErr_UnCmd						0x01
#define cErr_UnDataAddr					0x02
#define cErr_UnDataValue				0x03
#define cErr_DeviceBusy					0x06
#define cErr_PassdErr					0x10
#define cErr_CrcErr						0x11
#define cErr_ParUnValid					0x12
#define cErr_SysLock					0x13



enum EEPROM_STATIC_DATA_ID						// 64words
{
	EE_STATIC_AddrStart = 0,
	EE_STATIC_EepromLength = EE_STATIC_AddrStart,
	EE_STATIC_EepromVer,
	EE_STATIC_SerialNo1,
	EE_STATIC_SerialNo2,
	EE_STATIC_SerialNo3,
	EE_STATIC_SerialNo4,
	EE_STATIC_SerialNo5,
	EE_STATIC_SerialNoLen,
	EE_STATIC_BattVoltAdj,
	EE_STATIC_BattVoltBias,
	EE_STATIC_BattChgCurrAdj,
	EE_STATIC_BattChgCurrBias,
	EE_STATIC_BattDcgCurrAdj,
	EE_STATIC_BattDcgCurrBias,
	EE_STATIC_CellVolt01Adj,
	EE_STATIC_CellVolt01Bias,
	EE_STATIC_CellVolt02Adj,
	EE_STATIC_CellVolt02Bias,
	EE_STATIC_CellVolt03Adj,
	EE_STATIC_CellVolt03Bias,
	EE_STATIC_CellVolt04Adj,
	EE_STATIC_CellVolt04Bias,
	EE_STATIC_CellVolt05Adj,
	EE_STATIC_CellVolt05Bias,
	EE_STATIC_CellVolt06Adj,
	EE_STATIC_CellVolt06Bias,
	EE_STATIC_CellVolt07Adj,
	EE_STATIC_CellVolt07Bias,
	EE_STATIC_CellVolt08Adj,
	EE_STATIC_CellVolt08Bias,
	EE_STATIC_CellVolt09Adj,
	EE_STATIC_CellVolt09Bias,
	EE_STATIC_CellVolt10Adj,
	EE_STATIC_CellVolt10Bias,
	EE_STATIC_CellVolt11Adj,
	EE_STATIC_CellVolt11Bias,
	EE_STATIC_CellVolt12Adj,
	EE_STATIC_CellVolt12Bias,
	EE_STATIC_CellVolt13Adj,
	EE_STATIC_CellVolt13Bias,
	EE_STATIC_CellVolt14Adj,
	EE_STATIC_CellVolt14Bias,
	EE_STATIC_CellVolt15Adj,
	EE_STATIC_CellVolt15Bias,
	EE_STATIC_CellVolt16Adj,
	EE_STATIC_CellVolt16Bias,
	EE_STATIC_CellTempAdj,
	EE_STATIC_CellTempBias,
	EE_STATIC_BattType,
	EE_STATIC_Rsvd13,
	EE_STATIC_Rsvd12,
	EE_STATIC_Rsvd11,
	EE_STATIC_Rsvd10,
	EE_STATIC_Rsvd9,
	EE_STATIC_Rsvd8,
	EE_STATIC_Rsvd7,
	EE_STATIC_Rsvd6,
	EE_STATIC_Rsvd5,
	EE_STATIC_Rsvd4,
	EE_STATIC_Rsvd3,
	EE_STATIC_Rsvd2,
	EE_STATIC_Rsvd1,
	EE_STATIC_RandomNum,
	EE_STATIC_CheckSum,
	EE_STATIC_AddrEnd,
	EE_STATIC_Len = EE_STATIC_AddrEnd - EE_STATIC_AddrStart
};

enum EEPROM_DYNAMIC_DATA_ID						// 64words
{
	EE_DYNAMIC_AddrStart = EE_STATIC_AddrEnd,
	EE_DYNAMIC_EepromLength = EE_DYNAMIC_AddrStart,
	EE_DYNAMIC_HacpMode,
	EE_DYNAMIC_SlaveNum,
	EE_DYNAMIC_ModuleAddr,
	EE_DYNAMIC_ModuleRes,
	EE_DYNAMIC_CellVoltPct80,
	EE_DYNAMIC_CellVoltPct20,
	EE_DYNAMIC_CellVoltHi2,
	EE_DYNAMIC_CellVoltLo2,
	EE_DYNAMIC_CellVoltHiAlm,
	EE_DYNAMIC_CellVoltHiAlmBack,
	EE_DYNAMIC_CellVoltHi,
	EE_DYNAMIC_CellVoltHiBack,
	EE_DYNAMIC_CellVoltLoAlm,
	EE_DYNAMIC_CellVoltLoAlmBack,
	EE_DYNAMIC_CellVoltLo,
	EE_DYNAMIC_CellVoltLoBack,
	EE_DYNAMIC_BattChgCurrHiAlmBack,
	EE_DYNAMIC_BattChgCurrHiAlm,
	EE_DYNAMIC_BattChgCurrHi,
	EE_DYNAMIC_BattChgCurrHi2,
	EE_DYNAMIC_BattDcgCurrHiAlmBack,
	EE_DYNAMIC_BattDcgCurrHiAlm,
	EE_DYNAMIC_BattDcgCurrHi,
	EE_DYNAMIC_BattDcgCurrHi2,
	EE_DYNAMIC_CellChgTempHiAlmBack,
	EE_DYNAMIC_CellChgTempHiAlm,
	EE_DYNAMIC_CellChgTempHi,
	EE_DYNAMIC_CellChgTempHi2,
	EE_DYNAMIC_CellChgTempLoAlmBack,
	EE_DYNAMIC_CellChgTempLoAlm,
	EE_DYNAMIC_CellChgTempLo,
	EE_DYNAMIC_CellChgTempLo2,
	EE_DYNAMIC_CellDcgTempHiAlmBack,
	EE_DYNAMIC_CellDcgTempHiAlm,
	EE_DYNAMIC_CellDcgTempHi,
	EE_DYNAMIC_CellDcgTempHi2,
	EE_DYNAMIC_CellDcgTempLoAlmBack,
	EE_DYNAMIC_CellDcgTempLoAlm,
	EE_DYNAMIC_CellDcgTempLo,
	EE_DYNAMIC_CellDcgTempLo2,
	EE_DYNAMIC_HsTempHiAlm,
	EE_DYNAMIC_HsTempHi,
	EE_DYNAMIC_BalanceEn,
	EE_DYNAMIC_BalanceVoltErr,
	EE_DYNAMIC_BalanceVoltEn,
	EE_DYNAMIC_BalanceTempEn,
	EE_DYNAMIC_CellDcgDepth,
	EE_DYNAMIC_CellTempCoef,
	EE_DYNAMIC_Reservd2,
	EE_DYNAMIC_FaultRstCnt,
	EE_DYNAMIC_FaultClrTime,
	EE_DYNAMIC_IdleExitTime,
	EE_DYNAMIC_SleepExitTime,
	EE_DYNAMIC_BreakWireMode,
	EE_DYNAMIC_BackLightEn,
	EE_DYNAMIC_MenuRtnEn,
	EE_DYNAMIC_FaultRecordEn,
	EE_DYNAMIC_CellTempShield,
	EE_DYNAMIC_CellVoltPct20Two,
	EE_DYNAMIC_Reserved2,
	EE_DYNAMIC_Reserved1,
	EE_DYNAMIC_RandomNum,
	EE_DYNAMIC_CheckSum,
	EE_DYNAMIC_AddrEnd,
	EE_DYNAMIC_Len = EE_DYNAMIC_AddrEnd - EE_DYNAMIC_AddrStart
};

enum EEPROM_REALTIME_DATA_ID					// 24words
{
	EE_REALTIME_AddrStart = EE_DYNAMIC_AddrEnd,
	EE_REALTIME_EepromLength = EE_REALTIME_AddrStart,
	EE_REALTIME_BattWhReal,
	EE_REALTIME_BattWhMax,
	EE_REALTIME_BattFullCnt,
	EE_REALTIME_BattEmptyCnt,
	EE_REALTIME_BattCycCnt,
	EE_REALTIME_BattChgAhLo,
	EE_REALTIME_BattChgAhHi,
	EE_REALTIME_BattChgWhLo,
	EE_REALTIME_BattChgWhHi,
	EE_REALTIME_BattChgTimeLo,
	EE_REALTIME_BattChgTimeHi,
	EE_REALTIME_BattDcgAhLo,
	EE_REALTIME_BattDcgAhHi,
	EE_REALTIME_BattDcgWhLo,
	EE_REALTIME_BattDcgWhHi,
	EE_REALTIME_BattDcgTimeLo,
	EE_REALTIME_BattDcgTimeHi,
	EE_REALTIME_BattRunTimeLo,
	EE_REALTIME_BattRunTimeHi,
	EE_REALTIME_Reserved3,
	EE_REALTIME_Reserved2,
	EE_REALTIME_Reserved1,
	EE_REALTIME_CheckSum,
	EE_REALTIME_AddrEnd,
	EE_REALTIME_Len = EE_REALTIME_AddrEnd - EE_REALTIME_AddrStart
};

enum ATE_DATA_ID
{
	ATE_AddrStart = 0,
	ATE_Serial1 = ATE_AddrStart,
	ATE_Serial2,
	ATE_Serial3,
	ATE_Serial4,
	ATE_Serial5,
	ATE_SNLen,
	ATE_DispHwVer,
	ATE_CtrlHwVer,
	ATE_PowerHwVer,
	ATE_UpdateFW,
	ATE_ResetSystem,
	ATE_EEDefault,
	ATE_AgingMode,
	ATE_BattVoltAdj0,
	ATE_BattVoltAdj1,
	ATE_BattVoltAdj2,
	ATE_BattVoltAdj,
	ATE_BattCurr1Adj,
	ATE_BattCurr1Bias,
	ATE_BattCurr2Adj,
	ATE_BattCurr2Bias,
	ATE_SecureUnLock,
	ATE_Factory,
	ATE_BattType,
	ATE_BattVoltAdj3,
	ATE_BattCurrAdj3,
	ATE_BattCurr2Adj3,
	ATE_BattCellVoltAdj,
	ATE_CellVolt01Adj,
	ATE_CellVolt02Adj,
	ATE_CellVolt03Adj,
	ATE_CellVolt04Adj,
	ATE_CellVolt05Adj,
	ATE_CellVolt06Adj,
	ATE_CellVolt07Adj,
	ATE_CellVolt08Adj,
	ATE_CellVolt09Adj,
	ATE_CellVolt10Adj,
	ATE_CellVolt11Adj,
	ATE_CellVolt12Adj,
	ATE_CellVolt13Adj,
	ATE_CellVolt14Adj,
	ATE_CellVolt15Adj,
	ATE_CellVolt16Adj,
	ATE_AddrEnd,
	ATE_Len = ATE_AddrEnd - ATE_AddrStart
};

enum DBG_CMD_ID
{
	DBG_AddrStart = 0,	//0x00
	DBG_XXXXXXX00 = DBG_AddrStart,
	DBG_XXXXXXX01,		//0x01
	DBG_XXXXXXX02,		//0x02
	DBG_XXXXXXX03,		//0x03
	DBG_XXXXXXX04,		//0x04
	DBG_XXXXXXX05,		//0x05
	DBG_XXXXXXX06,		//0x06
	DBG_XXXXXXX07,		//0x07
	DBG_XXXXXXX08,		//0x08
	DBG_XXXXXXX09,		//0x09
	DBG_XXXXXXX0A,		//0x0A
	DBG_XXXXXXX0B, 		//0x0B
	DBG_XXXXXXX0C, 		//0x0C
	DBG_XXXXXXX0D, 		//0x0D
	DBG_XXXXXXX0E,		//0x0E
	DBG_XXXXXXX0F,		//0x0F
	
	DBG_XXXXXXX10,		//0x10
	DBG_XXXXXXX11,		//0x11
	DBG_XXXXXXX12,		//0x12
	DBG_XXXXXXX13, 		//0x13
	DBG_XXXXXXX14, 		//0x14
	DBG_XXXXXXX15, 		//0x15
	DBG_XXXXXXX16,		//0x16
	DBG_XXXXXXX17,		//0x17
	DBG_XXXXXXX18,		//0x18
	DBG_XXXXXXX19,		//0x19
	DBG_XXXXXXX1A,		//0x1A
	DBG_XXXXXXX1B,		//0x1B
	DBG_XXXXXXX1C,		//0x1C
	DBG_XXXXXXX1D,		//0x1D
	DBG_XXXXXXX1E,		//0x1E
	DBG_XXXXXXX1F,		//0x1F
	
	DBG_AddrEnd,
	DBG_Len = DBG_AddrEnd - DBG_AddrStart
};

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
INT16S wBatt1Ref;
INT16S wBatt2Ref;
INT16S wBatt1Real;
INT16S wBatt2Real;
INT16S wCellVolt1Ref[cADC_CELL_VOLT_MAX];
INT16S wCellVolt2Ref[cADC_CELL_VOLT_MAX];
INT16S wCellVolt1Real[cADC_CELL_VOLT_MAX];
INT16S wCellVolt2Real[cADC_CELL_VOLT_MAX];

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
void	sModBusRdProc(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U RxLen);
void	sModBusWrProc(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U RxLen);
void	sBuildRdFrame(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT16U (*RdFunc)(INT16U));
void	sBuildWrFrame(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U (*WrFunc)(INT16U, INT16U));
void	sBuildFaultFrame(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U ErrCode);
void	sBuildUpgradeFrame(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid);
INT16U	swEMSDataRead(INT16U RdAddr);
INT16U	swBMSDataRead(INT16U RdAddr);
INT16U	swBMS2DataRead(INT16U RdAddr);
INT16U	swBMSDataAllRead(INT16U RdAddr);
INT16U	swEESettingRead(INT16U RdAddr);
INT16U	swAfeRamDataRead(INT16U RdAddr);
INT16U	swAfeEepromDataRead(INT16U RdAddr);
INT16U	swInfoDataRead(INT16U RdAddr);
INT16U	swFaultRecordRead(INT16U RdAddr);
INT16U	swDbgCmdRead(INT16U RdAddr);
INT8U	swEESettingWrite(INT16U WrAddr, INT16U WrData);
INT8U	swATESettingWrite(INT16U WrAddr, INT16U WrData);
INT8U	swDbgCmdWrite(INT16U WrAddr, INT16U WrData);
void	sBattVoltAdj0(void);
INT8U	sbBattVoltAdj1(INT16S wVolt);
INT8U	sbBattVoltAdj2(INT16S wVolt);
INT8U	sbBattVoltAdj(INT16S wVolt);
INT8U	sbBattCurrAdj(INT16S wCurr);
void	sBattChgCurrAdj0(void);
INT8U	sbBattChgCurrAdj(INT16S wCurr);
void	sBattDsgCurrAdj0(void);
INT8U	sbBattDsgCurrAdj(INT16S wCurr);
INT8U	sGetCellVoltDefault(INT8U bNo);
void	sbBattVoltCopyToCell(void);

#define cTimer_20S					20000
#define cTimer_2S					2000
extern INT8U	bCanDataRecv;
/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void	sModBusSciAddrInitChk(INT16U wTimeBase)
{
	INT16S wSciInitCntMax;
	static INT16S s_wSciAddrInitCnt = 0;
	if(sbGetSciAddress() == cMODBUS_ADDR_ERR)
	{
		s_wSciAddrInitCnt += wTimeBase;
		if(bCanDataRecv)
		{
			wSciInitCntMax = cTimer_20S;
		}
		else
		{
			wSciInitCntMax = cTimer_2S;
		}
		if(s_wSciAddrInitCnt >= wSciInitCntMax)
		{
			s_wSciAddrInitCnt = 0;
			sSetSciAddress(1);
			sSetCanAddress(1);
            sInitial_CAN_New_Addr();
		}
	}
	else
	{
		s_wSciAddrInitCnt = 0;
		bCanDataRecv = 0;
	}
}

void	sModBusParsing(INT8U sciid)
{
	INT8U	*RxBuff;
	INT8U	*TxBuff;
	INT8U	packet_len;
	INT16U	temp;
	INT8U	bSciIdTemp;
	
	bSciIdTemp = sciid;
	RxBuff = pbGetSciRxBuff(sciid);
	TxBuff = pbGetSciTxBuff(sciid);

	if(sbGetSciAddress() == cMODBUS_ADDR_ERR)
	{
		return;
	}	
	
	// ID
	if(sbIsCanDeviceHost())
	{
		if((RxBuff[cMODBUS_ADDR] != sbGetSciAddress()) \
			&& (RxBuff[cMODBUS_ADDR] != swGetEepromModuleAddr()) \
			&& (RxBuff[cMODBUS_ADDR] != cMODBUS_ADDR_HOST) \
			&& (RxBuff[cMODBUS_ADDR] != cMODBUS_ADDR_BACKUP))
		{
			return;
		}
	}
	else
	{
		if((RxBuff[cMODBUS_ADDR] != sbGetSciAddress()) \
			&& (RxBuff[cMODBUS_ADDR] != swGetEepromModuleAddr()) \
			&& (RxBuff[cMODBUS_ADDR] != cMODBUS_ADDR_BACKUP))
		{
			return;
		}
	}
	
	switch(RxBuff[cMODBUS_CMD])
	{
		case cREAD_CMD:
			packet_len = 8;
			sModBusRdProc(RxBuff, TxBuff, bSciIdTemp, 8);
		break;
		case cWRITE_CMD:
			packet_len = 8;
			sModBusWrProc(RxBuff, TxBuff, bSciIdTemp, 8);
		break;
		case cWRITE_CMD_MULTI:
			temp = (((INT16U)RxBuff[cMODBUS_REG_NUM_H] << 8) + RxBuff[cMODBUS_REG_NUM_L]) << 1;
			temp += 9;
			if(temp > cSCI_RX_LEN)
			{
				packet_len = cSCI_RX_LEN;
			}
			else
			{
				packet_len = (INT8U)temp;
			}
			sModBusWrProc(RxBuff, TxBuff, bSciIdTemp, packet_len);
		break;
		default:
			packet_len = 8;
		break;
	}
}

void	sModBusRdProc(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U RxLen)
{
	INT16U RdAddr, RdNums, wCRC;
	
	// 
	wCRC = ((INT16U)RxBuff[RxLen - cMODBUS_CRC_H] << 8) + RxBuff[RxLen - cMODBUS_CRC_L];
	if(wCRC != swModbusCrc16(&RxBuff[cMODBUS_ADDR], (RxLen - cMODBUS_CRC_LEN)))
	{
		sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_CrcErr);
		return;
	}
	
	// 
	RdAddr = ((INT16U)RxBuff[cMODBUS_REG_ADDR_H] << 8) + RxBuff[cMODBUS_REG_ADDR_L];
	RdNums = ((INT16U)RxBuff[cMODBUS_REG_NUM_H] << 8) + RxBuff[cMODBUS_REG_NUM_L];
	
	// 
	if(RdNums > cMODBUS_REG_RX_MAX_LEN)
	{
		sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_UnDataValue);
		return;
	}
	
	// 
	if((RdAddr >= BMS_REG_BASE_ADDR) && (RdAddr < (BMS_REG_BASE_ADDR + BMS_REG_LEN)))
    {
		if((RdAddr + RdNums) <= (BMS_REG_BASE_ADDR + BMS_REG_LEN))
		{
	    	sBuildRdFrame(RxBuff, TxBuff, sciid, swBMSDataRead);
		}
		else
		{
	    	sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_ParUnValid);
		}
	}
	else if((RdAddr >= BMS2_REG_BASE_ADDR) && (RdAddr < (BMS2_REG_BASE_ADDR + BMS2_REG_LEN)))
    {
		if((RdAddr + RdNums) <= (BMS2_REG_BASE_ADDR + BMS2_REG_LEN))
		{
	    	sBuildRdFrame(RxBuff, TxBuff, sciid, swBMS2DataRead);
		}
		else
		{
	    	sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_ParUnValid);
		}
	}
	else if((RdAddr >= EMS_REG_BASE_ADDR) && (RdAddr < (EMS_REG_BASE_ADDR + EMS_REG_LEN)))
    {
		if((RdAddr + RdNums) <= (EMS_REG_BASE_ADDR + EMS_REG_LEN))
		{
	    	sBuildRdFrame(RxBuff, TxBuff, sciid, swEMSDataRead);
		}
		else
		{
	    	sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_ParUnValid);
		}
	}
	else if((RdAddr >= BMS_ALL_REG_BASE_ADDR) && (RdAddr < (BMS_ALL_REG_BASE_ADDR + BMS_ALL_REG_LEN)))
    {
		if((RdAddr + RdNums) <= (BMS_ALL_REG_BASE_ADDR + BMS_ALL_REG_LEN))
		{
	    	sBuildRdFrame(RxBuff, TxBuff, sciid, swBMSDataAllRead);
		}
		else
		{
	    	sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_ParUnValid);
		}
	}
	else if((RdAddr >= EE_REG_BASE_ADDR) && (RdAddr < (EE_REG_BASE_ADDR + EE_REG_BASE_LEN)))
    {
		if((RdAddr + RdNums) <= (EE_REG_BASE_ADDR + EE_REG_BASE_LEN))
		{
	    	sBuildRdFrame(RxBuff, TxBuff, sciid, swEESettingRead);
		}
		else
		{
	    	sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_ParUnValid);
		}
	}
	else if((RdAddr >= INFO_REG_BASE_ADDR) && (RdAddr < (INFO_REG_BASE_ADDR + INFO_REG_LEN)))
	{
		if((RdAddr + RdNums) <= (INFO_REG_BASE_ADDR + INFO_REG_LEN))
		{
			sBuildRdFrame(RxBuff, TxBuff, sciid, swInfoDataRead);
		}
		else
		{
			sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_ParUnValid);
		}
	}
	else if((RdAddr >= FAULT_REG_BASE_ADDR) && (RdAddr < (FAULT_REG_BASE_ADDR + FAULT_REG_BASE_LEN)))
	{
		if((RdAddr + RdNums) <= (FAULT_REG_BASE_ADDR + FAULT_REG_BASE_LEN))
		{
			sBuildRdFrame(RxBuff, TxBuff, sciid, swFaultRecordRead);
		}
		else
		{
			sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_ParUnValid);
		}
	}
	else if((RdAddr >= AFE_RAM_REG_BASE_ADDR) && (RdAddr < (AFE_RAM_REG_BASE_ADDR + AFE_RAM_REG_BASE_LEN)))
	{
		if((RdAddr + RdNums) <= (AFE_RAM_REG_BASE_ADDR + AFE_RAM_REG_BASE_LEN))
		{
			sBuildRdFrame(RxBuff, TxBuff, sciid, swAfeRamDataRead);
		}
		else
		{
			sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_ParUnValid);
		}
	}
	else if((RdAddr >= AFE_EEP_REG_BASE_ADDR) && (RdAddr < (AFE_EEP_REG_BASE_ADDR + AFE_EEP_REG_BASE_LEN)))
	{
		if((RdAddr + RdNums) <= (AFE_EEP_REG_BASE_ADDR + AFE_EEP_REG_BASE_LEN))
		{
			sBuildRdFrame(RxBuff, TxBuff, sciid, swAfeEepromDataRead);
		}
		else
		{
			sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_ParUnValid);
		}
	}
	else if((RdAddr >= DBG_CMD_BASE_ADDR) && (RdAddr < (DBG_CMD_BASE_ADDR + DBG_CMD_LEN)))
	{
		if((RdAddr + RdNums) <= (DBG_CMD_BASE_ADDR + DBG_CMD_LEN))
		{
			sBuildRdFrame(RxBuff, TxBuff, sciid, swDbgCmdRead);
		}
		else
		{
			sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_ParUnValid);
		}
	}
	else
	{
		sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_UnDataAddr);
	}
}

void	sModBusWrProc(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U RxLen)
{
	INT16U WrAddr, WrNums, wCRC;
	
	// 
	wCRC = ((INT16U)RxBuff[RxLen - cMODBUS_CRC_H] << 8) + RxBuff[RxLen - cMODBUS_CRC_L];
	if(wCRC != swModbusCrc16(&RxBuff[cMODBUS_ADDR], (RxLen - cMODBUS_CRC_LEN)))
	{
		sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_CrcErr);
		return;
	}
	
	// 
	WrAddr = ((INT16U)RxBuff[cMODBUS_REG_ADDR_H] << 8) + RxBuff[cMODBUS_REG_ADDR_L];
	if(RxBuff[cMODBUS_CMD] == cWRITE_CMD)
	{
		WrNums = 1;
	}
	else
	{
		WrNums = (RxBuff[cMODBUS_REG_NUM_H] << 8) + RxBuff[cMODBUS_REG_NUM_L];
	}
	
	// 
	if(WrNums > cMODBUS_REG_TX_MAX_LEN)
	{
		sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_UnDataValue);
		return;
	}
	
	// 
	if((WrAddr >= EE_REG_BASE_ADDR + EE_STATIC_AddrStart) && (WrAddr < (EE_REG_BASE_ADDR + EE_STATIC_AddrStart + EE_STATIC_Len)))
	{
		if((WrAddr + WrNums) <= (EE_REG_BASE_ADDR + EE_STATIC_AddrStart + EE_STATIC_Len))
		{
			sBuildWrFrame(RxBuff, TxBuff, sciid, swEESettingWrite);
		}
		else
		{
			sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_ParUnValid);
		}
	}
	else if((WrAddr >= EE_REG_BASE_ADDR + EE_DYNAMIC_AddrStart) && (WrAddr < (EE_REG_BASE_ADDR + EE_DYNAMIC_AddrStart + EE_DYNAMIC_Len)))
	{
		if((WrAddr + WrNums) <= (EE_REG_BASE_ADDR + EE_DYNAMIC_AddrStart + EE_DYNAMIC_Len))
		{
			sBuildWrFrame(RxBuff, TxBuff, sciid, swEESettingWrite);
		}
		else
		{
			sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_ParUnValid);
		}
	}
	else if((WrAddr >= EE_REG_BASE_ADDR) && (WrAddr < (EE_REG_BASE_ADDR + EE_REG_BASE_LEN)))
	{
		if((WrAddr + WrNums) <= (EE_REG_BASE_ADDR + EE_REG_BASE_LEN))
		{
			sBuildWrFrame(RxBuff, TxBuff, sciid, swEESettingWrite);
		}
		else
		{
			sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_ParUnValid);
		}
	}
	else if((WrAddr >= ATE_REG_BASE_ADDR) && (WrAddr < (ATE_REG_BASE_ADDR + ATE_REG_BASE_LEN)))
	{
		if((WrAddr + WrNums) <= (ATE_REG_BASE_ADDR + ATE_REG_BASE_LEN))
		{
			sBuildWrFrame(RxBuff, TxBuff, sciid, swATESettingWrite);
		}
		else
		{
			sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_ParUnValid);
		}
	}
	else if((WrAddr >= DBG_CMD_BASE_ADDR) && (WrAddr < (DBG_CMD_BASE_ADDR + DBG_CMD_LEN)))
	{
		if((WrAddr + WrNums) <= (DBG_CMD_BASE_ADDR + DBG_CMD_LEN))
		{
			sBuildWrFrame(RxBuff, TxBuff, sciid, swDbgCmdWrite);
		}
		else
		{
			sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_ParUnValid);
		}
	}
	else
	{
		sBuildFaultFrame(RxBuff, TxBuff, sciid, cErr_ParUnValid);
	}
}

void	sBuildRdFrame(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT16U (*RdFunc)(INT16U))
{
	INT16U RdAddr, RdNums, RdData, wCRC;
	INT8U i, TxLen;
	
	RdAddr = ((INT16U)RxBuff[cMODBUS_REG_ADDR_H] << 8) + RxBuff[cMODBUS_REG_ADDR_L];
	RdNums = ((INT16U)RxBuff[cMODBUS_REG_NUM_H] << 8) + RxBuff[cMODBUS_REG_NUM_L];
	
	TxBuff[cMODBUS_ADDR] = RxBuff[cMODBUS_ADDR];
	TxBuff[cMODBUS_CMD] = RxBuff[cMODBUS_CMD];
	TxBuff[2] = (RdNums << 1) & 0x00FF;
	TxLen = 3;
	
	for(i = 0; i < RdNums; i++)
	{
		RdData = (*RdFunc)(RdAddr);
		RdAddr++;
		
		TxBuff[TxLen++] = RdData >> 8;
		TxBuff[TxLen++] = RdData & 0x00FF;
	}
	
	wCRC = swModbusCrc16(&TxBuff[cMODBUS_ADDR], TxLen);
	TxBuff[TxLen++] = wCRC >> 8;
	TxBuff[TxLen++] = wCRC & 0x00FF;
	
	if(sSciGetTxStatus(sciid) != cSciTxBusy)
	{
		sSciWrite(sciid, TxBuff, TxLen);
		sSetCommConnected(sciid, true);
		sRstCommLossCount();
	}
}

void	sBuildWrFrame(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U (*WrFunc)(INT16U, INT16U))
{
	INT8U *pBuff = NULL;
	INT16U WrAddr, WrNums, WrData, wCRC;
	INT8U i, Status;
	
	WrAddr = ((INT16U)RxBuff[cMODBUS_REG_ADDR_H] << 8) + RxBuff[cMODBUS_REG_ADDR_L];
	if(RxBuff[cMODBUS_CMD] == cWRITE_CMD)
	{
		WrNums = 1;
		pBuff = &RxBuff[cMODBUS_WRITE_DATA_START];
	}
	else
	{
		WrNums = ((INT16U)RxBuff[cMODBUS_REG_NUM_H] << 8) + RxBuff[cMODBUS_REG_NUM_L];
		pBuff = &RxBuff[cMODBUS_MULTI_WRITE_DATA_START];
	}
	
	for(i = 0; i < WrNums; i++)
	{
		WrData = ((INT16U)pBuff[0] << 8) + pBuff[1];
		pBuff += 2;
		
		Status = (*WrFunc)(WrAddr, WrData);
		WrAddr++;
		
		if(Status != 0)
		{
			sBuildFaultFrame(RxBuff, TxBuff, sciid, Status);
			return;
		}
	}
	
	TxBuff[cMODBUS_ADDR] = RxBuff[cMODBUS_ADDR];
	TxBuff[cMODBUS_CMD] = RxBuff[cMODBUS_CMD];
	TxBuff[cMODBUS_REG_ADDR_H] = RxBuff[cMODBUS_REG_ADDR_H];
	TxBuff[cMODBUS_REG_ADDR_L] = RxBuff[cMODBUS_REG_ADDR_L];
	TxBuff[cMODBUS_REG_NUM_H] = RxBuff[cMODBUS_REG_NUM_H];
	TxBuff[cMODBUS_REG_NUM_L] = RxBuff[cMODBUS_REG_NUM_L];
	wCRC = swModbusCrc16(&TxBuff[cMODBUS_ADDR], 6);
	TxBuff[6] = wCRC >> 8;
	TxBuff[7] = wCRC & 0x00FF;
	
	if(sSciGetTxStatus(sciid) != cSciTxBusy)
	{
		sSciWrite(sciid, TxBuff, 8);
		sSetCommConnected(sciid, true);
		sRstCommLossCount();
	}
}

void	sBuildFaultFrame(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U ErrCode)
{
	INT16U wCRC;
	
	TxBuff[cMODBUS_ADDR] = RxBuff[cMODBUS_ADDR];
	TxBuff[cMODBUS_CMD] = RxBuff[cMODBUS_CMD] + 0x80;
	TxBuff[2] = ErrCode;
	wCRC = swModbusCrc16(&TxBuff[cMODBUS_ADDR], 3);
	TxBuff[3] = wCRC >> 8;
	TxBuff[4] = wCRC & 0x00FF;
	
	if(sSciGetTxStatus(sciid) != cSciTxBusy)
	{
		sSciWrite(sciid, TxBuff, 5);
	}
}

void	sBuildUpgradeFrame(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid)
{
	TxBuff[0] = 'A';
	TxBuff[1] = '\r';
	
	if(sSciGetTxStatus(sciid) != cSciTxBusy)
	{
		sSciWrite(sciid, TxBuff, 2);
	}
}

INT16U	swInfoDataRead(INT16U RdAddr)
{
	RdAddr -= INFO_REG_BASE_ADDR;
	return swGetBmsInformationData(RdAddr);
}

INT16U swEMSDataRead(INT16U RdAddr)
{
	RdAddr -= EMS_REG_BASE_ADDR;
	return swGetEmsData(RdAddr);
}

INT16U swBMSDataRead(INT16U RdAddr)
{
	RdAddr -= BMS_REG_BASE_ADDR;
	return swGetBmsDataLocal(RdAddr);
}

INT16U swBMS2DataRead(INT16U RdAddr)
{
	RdAddr -= BMS2_REG_BASE_ADDR;
	return swGetBms2DataLocal(RdAddr);
}

INT16U swBMSDataAllRead(INT16U RdAddr)
{
	INT8U bNo;
	INT16U wAddr;
	
	if(RdAddr < BMS_ALL_REG_BASE_ADDR)
	{
		return 0;
	}
	
	bNo = (RdAddr - BMS_ALL_REG_BASE_ADDR) / BMS_ONE_REG_LEN;
	wAddr = (RdAddr - BMS_ALL_REG_BASE_ADDR) % BMS_ONE_REG_LEN;
	return swGetBmsData(bNo, wAddr);
}

INT16U	swFaultRecordRead(INT16U RdAddr)
{
	RdAddr -= FAULT_REG_BASE_ADDR;
	return swGetEepromFrMapData(RdAddr);
}

INT16U	swEESettingRead(INT16U RdAddr)
{
	INT16U wTemp;
	
	RdAddr -= EE_REG_BASE_ADDR;
	if(RdAddr < 0x40)
	{
		wTemp = swGetEepromStaticMapData(RdAddr - EE_STATIC_AddrStart);
	}
	else if(RdAddr < 0x80)
	{
		wTemp = swGetEepromDynamicMapData(RdAddr - EE_DYNAMIC_AddrStart);
	}
	else if(RdAddr < 0xC0)
	{
		wTemp = swGetEepromRealtimeMapData(RdAddr - EE_REALTIME_AddrStart);
	}
	else
	{
		wTemp = 0xFFFF;
	}
	
	return wTemp;
}

INT16U	swAfeRamDataRead(INT16U RdAddr)
{
	RdAddr -= AFE_RAM_REG_BASE_ADDR;
	return swGetAfeRamData(RdAddr);
}

INT16U	swAfeEepromDataRead(INT16U RdAddr)
{
	RdAddr -= AFE_EEP_REG_BASE_ADDR;
	return swGetAfeEepromData(RdAddr);
}

INT8U	swEESettingWrite(INT16U WrAddr, INT16U WrData)
{
	INT8U ret = 0;
	INT8U DoEEDynamicSave = 0;
	INT8U DoEERealtimeSave = 0;
	
	WrAddr -= EE_REG_BASE_ADDR;
	switch(WrAddr)
	{
		case EE_DYNAMIC_HacpMode:
			#if(cEEHacpModeLimitLo==0)
			if(WrData <= cEEHacpModeLimitUp)
			#else
			if((WrData >= cEEHacpModeLimitLo) && (WrData <= cEEHacpModeLimitUp))
			#endif
			{
				sSetEepromHacpMode(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_SlaveNum:
			#if(cEESlaveNumLimitLo==0)
			if(WrData <= cEESlaveNumLimitUp)
			#else
			if((WrData >= cEESlaveNumLimitLo) && (WrData <= cEESlaveNumLimitUp))
			#endif
			{
				sSetEepromSlaveNum(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_ModuleAddr:
			#if(cEEModuleAddrLimitLo==0)
			if(WrData <= cEEModuleAddrLimitUp)
			#else
			if((WrData >= cEEModuleAddrLimitLo) && (WrData <= cEEModuleAddrLimitUp))
			#endif
			{
				sSetEepromModuleAddr(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_ModuleRes:
			#if(cEEModuleResLimitLo==0)
			if(WrData <= cEEModuleResLimitUp)
			#else
			if((WrData >= cEEModuleResLimitLo) && (WrData <= cEEModuleResLimitUp))
			#endif
			{
				sSetEepromModuleRes(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellVoltPct80:
			#if(cEECellVoltPct80LimitLo==0)
			if(WrData <= cEECellVoltPct80LimitUp)
			#else
			if((WrData >= cEECellVoltPct80LimitLo) && (WrData <= cEECellVoltPct80LimitUp))
			#endif
			{
				sSetEepromCellVoltPct80(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellVoltPct20:
			#if(cEECellVoltPct20LimitLo==0)
			if(WrData <= cEECellVoltPct20LimitUp)
			#else
			if((WrData >= cEECellVoltPct20LimitLo) && (WrData <= cEECellVoltPct20LimitUp))
			#endif
			{
				sSetEepromCellVoltPct20(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellVoltHi2:
			#if(cEECellVoltHi2LimitLo==0)
			if(WrData <= cEECellVoltHi2LimitUp)
			#else
			if((WrData >= cEECellVoltHi2LimitLo) && (WrData <= cEECellVoltHi2LimitUp))
			#endif
			{
				sSetEepromCellVoltHi2(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellVoltLo2:
			#if(cEECellVoltLo2LimitLo==0)
			if(WrData <= cEECellVoltLo2LimitUp)
			#else
			if((WrData >= cEECellVoltLo2LimitLo) && (WrData <= cEECellVoltLo2LimitUp))
			#endif
			{
				sSetEepromCellVoltLo2(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellVoltHiAlm:
			#if(cEECellVoltHiAlmLimitLo==0)
			if(WrData <= cEECellVoltHiAlmLimitUp)
			#else
			if((WrData >= cEECellVoltHiAlmLimitLo) && (WrData <= cEECellVoltHiAlmLimitUp))
			#endif
			{
				sSetEepromCellVoltHiAlm(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellVoltHiAlmBack:
			#if(cEECellVoltHiAlmBackLimitLo==0)
			if(WrData <= cEECellVoltHiAlmBackLimitUp)
			#else
			if((WrData >= cEECellVoltHiAlmBackLimitLo) && (WrData <= cEECellVoltHiAlmBackLimitUp))
			#endif
			{
				sSetEepromCellVoltHiAlmBack(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellVoltHi:
			#if(cEECellVoltHiLimitLo==0)
			if(WrData <= cEECellVoltHiLimitUp)
			#else
			if((WrData >= cEECellVoltHiLimitLo) && (WrData <= cEECellVoltHiLimitUp))
			#endif
			{
				sSetEepromCellVoltHi(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellVoltHiBack:
			#if(cEECellVoltHiBackLimitLo==0)
			if(WrData <= cEECellVoltHiBackLimitUp)
			#else
			if((WrData >= cEECellVoltHiBackLimitLo) && (WrData <= cEECellVoltHiBackLimitUp))
			#endif
			{
				sSetEepromCellVoltHiBack(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellVoltLoAlm:
			#if(cEECellVoltLoAlmLimitLo==0)
			if(WrData <= cEECellVoltLoAlmLimitUp)
			#else
			if((WrData >= cEECellVoltLoAlmLimitLo) && (WrData <= cEECellVoltLoAlmLimitUp))
			#endif
			{
				sSetEepromCellVoltLoAlm(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellVoltLoAlmBack:
			#if(cEECellVoltLoAlmBackLimitLo==0)
			if(WrData <= cEECellVoltLoAlmBackLimitUp)
			#else
			if((WrData >= cEECellVoltLoAlmBackLimitLo) && (WrData <= cEECellVoltLoAlmBackLimitUp))
			#endif
			{
				sSetEepromCellVoltLoAlmBack(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellVoltLo:
			#if(cEECellVoltLoLimitLo==0)
			if(WrData <= cEECellVoltLoLimitUp)
			#else
			if((WrData >= cEECellVoltLoLimitLo) && (WrData <= cEECellVoltLoLimitUp))
			#endif
			{
				sSetEepromCellVoltLo(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellVoltLoBack:
			#if(cEECellVoltLoBackLimitLo==0)
			if(WrData <= cEECellVoltLoBackLimitUp)
			#else
			if((WrData >= cEECellVoltLoBackLimitLo) && (WrData <= cEECellVoltLoBackLimitUp))
			#endif
			{
				sSetEepromCellVoltLoBack(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_BattChgCurrHiAlmBack:
			#if(cEEBattChgCurrHiAlmBackLimitLo==0)
			if(WrData <= cEEBattChgCurrHiAlmBackLimitUp)
			#else
			if((WrData >= cEEBattChgCurrHiAlmBackLimitLo) && (WrData <= cEEBattChgCurrHiAlmBackLimitUp))
			#endif
			{
				sSetEepromBattChgCurrHiAlmBack(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_BattChgCurrHiAlm:
			#if(cEEBattChgCurrHiAlmLimitLo==0)
			if(WrData <= cEEBattChgCurrHiAlmLimitUp)
			#else
			if((WrData >= cEEBattChgCurrHiAlmLimitLo) && (WrData <= cEEBattChgCurrHiAlmLimitUp))
			#endif
			{
				sSetEepromBattChgCurrHiAlm(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_BattChgCurrHi:
			#if(cEEBattChgCurrHiLimitLo==0)
			if(WrData <= cEEBattChgCurrHiLimitUp)
			#else
			if((WrData >= cEEBattChgCurrHiLimitLo) && (WrData <= cEEBattChgCurrHiLimitUp))
			#endif
			{
				sSetEepromBattChgCurrHi(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_BattChgCurrHi2:
			#if(cEEBattChgCurrHi2LimitLo==0)
			if(WrData <= cEEBattChgCurrHi2LimitUp)
			#else
			if((WrData >= cEEBattChgCurrHi2LimitLo) && (WrData <= cEEBattChgCurrHi2LimitUp))
			#endif
			{
				sSetEepromBattChgCurrHi2(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_BattDcgCurrHiAlmBack:
			#if(cEEBattDcgCurrHiAlmBackLimitLo==0)
			if(WrData <= cEEBattDcgCurrHiAlmBackLimitUp)
			#else
			if((WrData >= cEEBattDcgCurrHiAlmBackLimitLo) && (WrData <= cEEBattDcgCurrHiAlmBackLimitUp))
			#endif
			{
				sSetEepromBattDcgCurrHiAlmBack(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_BattDcgCurrHiAlm:
			#if(cEEBattDcgCurrHiAlmLimitLo==0)
			if(WrData <= cEEBattDcgCurrHiAlmLimitUp)
			#else
			if((WrData >= cEEBattDcgCurrHiAlmLimitLo) && (WrData <= cEEBattDcgCurrHiAlmLimitUp))
			#endif
			{
				sSetEepromBattDcgCurrHiAlm(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_BattDcgCurrHi:
			#if(cEEBattDcgCurrHiLimitLo==0)
			if(WrData <= cEEBattDcgCurrHiLimitUp)
			#else
			if((WrData >= cEEBattDcgCurrHiLimitLo) && (WrData <= cEEBattDcgCurrHiLimitUp))
			#endif
			{
				sSetEepromBattDcgCurrHi(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_BattDcgCurrHi2:
			#if(cEEBattDcgCurrHi2LimitLo==0)
			if(WrData <= cEEBattDcgCurrHi2LimitUp)
			#else
			if((WrData >= cEEBattDcgCurrHi2LimitLo) && (WrData <= cEEBattDcgCurrHi2LimitUp))
			#endif
			{
				sSetEepromBattDcgCurrHi2(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellChgTempHiAlmBack:
			if(((INT16S)WrData >= cEECellChgTempHiAlmBackLimitLo) && ((INT16S)WrData <= cEECellChgTempHiAlmBackLimitUp))
			{
				sSetEepromCellChgTempHiAlmBack(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellChgTempHiAlm:
			if(((INT16S)WrData >= cEECellChgTempHiAlmLimitLo) && ((INT16S)WrData <= cEECellChgTempHiAlmLimitUp))
			{
				sSetEepromCellChgTempHiAlm(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellChgTempHi:
			if(((INT16S)WrData >= cEECellChgTempHiLimitLo) && ((INT16S)WrData <= cEECellChgTempHiLimitUp))
			{
				sSetEepromCellChgTempHi(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellChgTempHi2:
			if(((INT16S)WrData >= cEECellChgTempHi2LimitLo) && ((INT16S)WrData <= cEECellChgTempHi2LimitUp))
			{
				sSetEepromCellChgTempHi2(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellChgTempLoAlmBack:
			if(((INT16S)WrData >= cEECellChgTempLoAlmBackLimitLo) && ((INT16S)WrData <= cEECellChgTempLoAlmBackLimitUp))
			{
				sSetEepromCellChgTempLoAlmBack((INT16S)WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellChgTempLoAlm:
			if(((INT16S)WrData >= cEECellChgTempLoAlmLimitLo) && ((INT16S)WrData <= cEECellChgTempLoAlmLimitUp))
			{
				sSetEepromCellChgTempLoAlm((INT16S)WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellChgTempLo:
			if(((INT16S)WrData >= cEECellChgTempLoLimitLo) && ((INT16S)WrData <= cEECellChgTempLoLimitUp))
			{
				sSetEepromCellChgTempLo((INT16S)WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellChgTempLo2:
			if(((INT16S)WrData >= cEECellChgTempLo2LimitLo) && ((INT16S)WrData <= cEECellChgTempLo2LimitUp))
			{
				sSetEepromCellChgTempLo2((INT16S)WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellDcgTempHiAlmBack:
			if(((INT16S)WrData >= cEECellDcgTempHiAlmBackLimitLo) && ((INT16S)WrData <= cEECellDcgTempHiAlmBackLimitUp))
			{
				sSetEepromCellDcgTempHiAlmBack(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellDcgTempHiAlm:
			if(((INT16S)WrData >= cEECellDcgTempHiAlmLimitLo) && ((INT16S)WrData <= cEECellDcgTempHiAlmLimitUp))
			{
				sSetEepromCellDcgTempHiAlm(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellDcgTempHi:
			if(((INT16S)WrData >= cEECellDcgTempHiLimitLo) && ((INT16S)WrData <= cEECellDcgTempHiLimitUp))
			{
				sSetEepromCellDcgTempHi(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellDcgTempHi2:
			if(((INT16S)WrData >= cEECellDcgTempHi2LimitLo) && ((INT16S)WrData <= cEECellDcgTempHi2LimitUp))
			{
				sSetEepromCellDcgTempHi2(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellDcgTempLoAlmBack:
			if(((INT16S)WrData >= cEECellDcgTempLoAlmBackLimitLo) && ((INT16S)WrData <= cEECellDcgTempLoAlmBackLimitUp))
			{
				sSetEepromCellDcgTempLoAlmBack((INT16S)WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellDcgTempLoAlm:
			if(((INT16S)WrData >= cEECellDcgTempLoAlmLimitLo) && ((INT16S)WrData <= cEECellDcgTempLoAlmLimitUp))
			{
				sSetEepromCellDcgTempLoAlm((INT16S)WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellDcgTempLo:
			if(((INT16S)WrData >= cEECellDcgTempLoLimitLo) && ((INT16S)WrData <= cEECellDcgTempLoLimitUp))
			{
				sSetEepromCellDcgTempLo((INT16S)WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellDcgTempLo2:
			if(((INT16S)WrData >= cEECellDcgTempLo2LimitLo) && ((INT16S)WrData <= cEECellDcgTempLo2LimitUp))
			{
				sSetEepromCellDcgTempLo2((INT16S)WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_HsTempHiAlm:
			if(((INT16S)WrData >= cEEHsTempHiAlmLimitLo) && ((INT16S)WrData <= cEEHsTempHiAlmLimitUp))
			{
				sSetEepromHsTempHiAlm(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_HsTempHi:
			if(((INT16S)WrData >= cEEHsTempHiLimitLo) && ((INT16S)WrData <= cEEHsTempHiLimitUp))
			{
				sSetEepromHsTempHi(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_BalanceEn:
			#if(cEEBalanceEnLimitLo==0)
			if(WrData <= cEEBalanceEnLimitUp)
			#else
			if((WrData >= cEEBalanceEnLimitLo) && (WrData <= cEEBalanceEnLimitUp))
			#endif
			{
				if(swGetBmsBalanceFunc() == cBMS_BALANCE_DISABLE)
				{
					WrData = 0;
				}
				sSetEepromBalanceEn(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_BalanceVoltErr:
			#if(cEEBalanceVoltErrLimitLo==0)
			if(WrData <= cEEBalanceVoltErrLimitUp)
			#else
			if((WrData >= cEEBalanceVoltErrLimitLo) && (WrData <= cEEBalanceVoltErrLimitUp))
			#endif
			{
				sSetEepromBalanceVoltErr(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_BalanceVoltEn:
			#if(cEEBalanceVoltEnLimitLo==0)
			if(WrData <= cEEBalanceVoltEnLimitUp)
			#else
			if((WrData >= cEEBalanceVoltEnLimitLo) && (WrData <= cEEBalanceVoltEnLimitUp))
			#endif
			{
				sSetEepromBalanceVoltEn(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_BalanceTempEn:
			#if(cEEBalanceTempEnLimitLo==0)
			if(WrData <= cEEBalanceTempEnLimitUp)
			#else
			if((WrData >= cEEBalanceTempEnLimitLo) && (WrData <= cEEBalanceTempEnLimitUp))
			#endif
			{
				sSetEepromBalanceTempEn(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellDcgDepth:
			#if(cEECellDcgDepthLimitLo==0)
			if(WrData <= cEECellDcgDepthLimitUp)
			#else
			if((WrData >= cEECellDcgDepthLimitLo) && (WrData <= cEECellDcgDepthLimitUp))
			#endif
			{
				sSetEepromCellDcgDepth(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellTempCoef:
			#if(cEECellTempCoefLimitLo==0)
			if(WrData <= cEECellTempCoefLimitUp)
			#else
			if((WrData >= cEECellTempCoefLimitLo) && (WrData <= cEECellTempCoefLimitUp))
			#endif
			{
				sSetEepromCellTempCoef(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_Reservd2:
			#if(cEEDynamicRsvd2LimitLo==0)
			if(WrData <= cEEDynamicRsvd2LimitUp)
			#else
			if((WrData >= cEEDynamicRsvd2LimitLo) && (WrData <= cEEDynamicRsvd2LimitUp))
			#endif
			{
				sSetEepromDynamicRsvd2(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_FaultRstCnt:
			#if(cEEFaultRstCntLimitLo==0)
			if(WrData <= cEEFaultRstCntLimitUp)
			#else
			if((WrData >= cEEFaultRstCntLimitLo) && (WrData <= cEEFaultRstCntLimitUp))
			#endif
			{
				sSetEepromFaultRstCnt(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_FaultClrTime:
			#if(cEEFaultClrTimeLimitLo==0)
			if(WrData <= cEEFaultClrTimeLimitUp)
			#else
			if((WrData >= cEEFaultClrTimeLimitLo) && (WrData <= cEEFaultClrTimeLimitUp))
			#endif
			{
				sSetEepromFaultClrTime(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_IdleExitTime:
			#if(cEEIdleExitTimeLimitLo==0)
			if(WrData <= cEEIdleExitTimeLimitUp)
			#else
			if((WrData >= cEEIdleExitTimeLimitLo) && (WrData <= cEEIdleExitTimeLimitUp))
			#endif
			{
				sSetEepromIdleExitTime(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_SleepExitTime:
			#if(cEESleepExitTimeLimitLo==0)
			if(WrData <= cEESleepExitTimeLimitUp)
			#else
			if((WrData >= cEESleepExitTimeLimitLo) && (WrData <= cEESleepExitTimeLimitUp))
			#endif
			{
				sSetEepromSleepExitTime(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_BreakWireMode:
			#if(cEEBreakWireModeLimitLo==0)
			if(WrData <= cEEBreakWireModeLimitUp)
			#else
			if((WrData >= cEEBreakWireModeLimitLo) && (WrData <= cEEBreakWireModeLimitUp))
			#endif
			{
				sSetEepromBreakWireMode(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_BackLightEn:
			#if(cEEBackLightEnableLimitLo==0)
			if(WrData <= cEEBackLightEnableLimitUp)
			#else
			if((WrData >= cEEBackLightEnableLimitLo) && (WrData <= cEEBackLightEnableLimitUp))
			#endif
			{
				sSetEepromBackLightEn(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_MenuRtnEn:
			#if(cEEMenuRtnEnableLimitLo==0)
			if(WrData <= cEEMenuRtnEnableLimitUp)
			#else
			if((WrData >= cEEMenuRtnEnableLimitLo) && (WrData <= cEEMenuRtnEnableLimitUp))
			#endif
			{
				sSetEepromMenuRtnEn(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_FaultRecordEn:
			#if(cEEFaultRecordEnableLimitLo==0)
			if(WrData <= cEEFaultRecordEnableLimitUp)
			#else
			if((WrData >= cEEFaultRecordEnableLimitLo) && (WrData <= cEEFaultRecordEnableLimitUp))
			#endif
			{
				sSetEepromFaultRecordEn(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellTempShield:
			if((WrData == 0x0000) || (WrData == 0x0001) || (WrData == 0x0002) || (WrData == 0x0004) || (WrData == 0x0008))
			{
				sSetEepromCellTempShield(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_DYNAMIC_CellVoltPct20Two:
			#if(cEECellVoltPct20TwoLimitLo==0)
				if(WrData <= cEECellVoltPct20TwoLimitUp)
			#else
				if((WrData >= cEECellVoltPct20TwoLimitLo) && (WrData <= cEECellVoltPct20TwoLimitUp))
			#endif
			{
				sSetEepromCellVoltPct20Two(WrData);
				DoEEDynamicSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_REALTIME_BattWhReal:
			#if(cEEBattAhRealLimitLo==0)
			if(WrData <= cEEBattAhRealLimitUp)
			#else
			if((WrData >= cEEBattAhRealLimitLo) && (WrData <= cEEBattAhRealLimitUp))
			#endif
			{
				if(sbGetEepromSecureLock() == false)
				{
					sSetSoxAhReal(WrData);
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_REALTIME_BattWhMax:
			#if(cEEBattAhMaxLimitLo==0)
			if(WrData <= cEEBattAhMaxLimitUp)
			#else
			if((WrData >= cEEBattAhMaxLimitLo) && (WrData <= cEEBattAhMaxLimitUp))
			#endif
			{
				if(sbGetEepromSecureLock() == false)
				{
					sSetSoxAhMax(WrData);
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_REALTIME_BattFullCnt:
			#if(cEEBattFullCntLimitLo==0)
			if(WrData <= cEEBattFullCntLimitUp)
			#else
			if((WrData >= cEEBattFullCntLimitLo) && (WrData <= cEEBattFullCntLimitUp))
			#endif
			{
				if(sbGetEepromSecureLock() == false)
				{
					sSetEepromBattFullCnt(WrData);
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_REALTIME_BattEmptyCnt:
			#if(cEEBattEmptyCntLimitLo==0)
			if(WrData <= cEEBattEmptyCntLimitUp)
			#else
			if((WrData >= cEEBattEmptyCntLimitLo) && (WrData <= cEEBattEmptyCntLimitUp))
			#endif
			{
				if(sbGetEepromSecureLock() == false)
				{
					sSetEepromBattEmptyCnt(WrData);
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_REALTIME_BattCycCnt:
			#if(cEEBattCycCntLimitLo==0)
			if(WrData <= cEEBattCycCntLimitUp)
			#else
			if((WrData >= cEEBattCycCntLimitLo) && (WrData <= cEEBattCycCntLimitUp))
			#endif
			{
				if(sbGetEepromSecureLock() == false)
				{
					sSetEepromBattCycCnt(WrData);
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_REALTIME_BattChgAhLo:
			#if(cEEBattChgAhLoLimitLo==0)
			if(WrData <= cEEBattChgAhLoLimitUp)
			#else
			if((WrData >= cEEBattChgAhLoLimitLo) && (WrData <= cEEBattChgAhLoLimitUp))
			#endif
			{
				if(sbGetEepromSecureLock() == false)
				{
					sSetEepromBattChgAhLo(WrData);
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_REALTIME_BattChgAhHi:
			#if(cEEBattChgAhHiLimitLo==0)
			if(WrData <= cEEBattChgAhHiLimitUp)
			#else
			if((WrData >= cEEBattChgAhHiLimitLo) && (WrData <= cEEBattChgAhHiLimitUp))
			#endif
			{
				if(sbGetEepromSecureLock() == false)
				{
					sSetEepromBattChgAhHi(WrData);
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_REALTIME_BattChgWhLo:
			#if(cEEBattChgWhLoLimitLo==0)
			if(WrData <= cEEBattChgWhLoLimitUp)
			#else
			if((WrData >= cEEBattChgWhLoLimitLo) && (WrData <= cEEBattChgWhLoLimitUp))
			#endif
			{
				if(sbGetEepromSecureLock() == false)
				{
					sSetEepromBattChgWhLo(WrData);
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_REALTIME_BattChgWhHi:
			#if(cEEBattChgWhHiLimitLo==0)
			if(WrData <= cEEBattChgWhHiLimitUp)
			#else
			if((WrData >= cEEBattChgWhHiLimitLo) && (WrData <= cEEBattChgWhHiLimitUp))
			#endif
			{
				if(sbGetEepromSecureLock() == false)
				{
					sSetEepromBattChgWhHi(WrData);
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_REALTIME_BattChgTimeLo:
			#if(cEEBattChgTimeLoLimitLo==0)
			if(WrData <= cEEBattChgTimeLoLimitUp)
			#else
			if((WrData >= cEEBattChgTimeLoLimitLo) && (WrData <= cEEBattChgTimeLoLimitUp))
			#endif
			{
				if(sbGetEepromSecureLock() == false)
				{
					sSetEepromBattChgTimeLo(WrData);
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_REALTIME_BattChgTimeHi:
			#if(cEEBattChgTimeHiLimitLo==0)
			if(WrData <= cEEBattChgTimeHiLimitUp)
			#else
			if((WrData >= cEEBattChgTimeHiLimitLo) && (WrData <= cEEBattChgTimeHiLimitUp))
			#endif
			{
				if(sbGetEepromSecureLock() == false)
				{
					sSetEepromBattChgTimeHi(WrData);
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_REALTIME_BattDcgAhLo:
			#if(cEEBattDcgAhLoLimitLo==0)
			if(WrData <= cEEBattDcgAhLoLimitUp)
			#else
			if((WrData >= cEEBattDcgAhLoLimitLo) && (WrData <= cEEBattDcgAhLoLimitUp))
			#endif
			{
				if(sbGetEepromSecureLock() == false)
				{
					sSetEepromBattDcgAhLo(WrData);
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_REALTIME_BattDcgAhHi:
			#if(cEEBattDcgAhHiLimitLo==0)
			if(WrData <= cEEBattDcgAhHiLimitUp)
			#else
			if((WrData >= cEEBattDcgAhHiLimitLo) && (WrData <= cEEBattDcgAhHiLimitUp))
			#endif
			{
				if(sbGetEepromSecureLock() == false)
				{
					sSetEepromBattDcgAhHi(WrData);
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_REALTIME_BattDcgWhLo:
			#if(cEEBattDcgWhLoLimitLo==0)
			if(WrData <= cEEBattDcgWhLoLimitUp)
			#else
			if((WrData >= cEEBattDcgWhLoLimitLo) && (WrData <= cEEBattDcgWhLoLimitUp))
			#endif
			{
				if(sbGetEepromSecureLock() == false)
				{
					sSetEepromBattDcgWhLo(WrData);
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_REALTIME_BattDcgWhHi:
			#if(cEEBattDcgAhHiLimitLo==0)
			if(WrData <= cEEBattDcgAhHiLimitUp)
			#else
			if((WrData >= cEEBattDcgAhHiLimitLo) && (WrData <= cEEBattDcgAhHiLimitUp))
			#endif
			{
				if(sbGetEepromSecureLock() == false)
				{
					sSetEepromBattDcgWhHi(WrData);
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_REALTIME_BattDcgTimeLo:
			#if(cEEBattDcgTimeLoLimitLo==0)
			if(WrData <= cEEBattDcgTimeLoLimitUp)
			#else
			if((WrData >= cEEBattDcgTimeLoLimitLo) && (WrData <= cEEBattDcgTimeLoLimitUp))
			#endif
			{
				if(sbGetEepromSecureLock() == false)
				{
					sSetEepromBattDcgTimeLo(WrData);
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_REALTIME_BattDcgTimeHi:
			#if(cEEBattDcgTimeHiLimitLo==0)
			if(WrData <= cEEBattDcgTimeHiLimitUp)
			#else
			if((WrData >= cEEBattDcgTimeHiLimitLo) && (WrData <= cEEBattDcgTimeHiLimitUp))
			#endif
			{
				if(sbGetEepromSecureLock() == false)
				{
					sSetEepromBattDcgTimeHi(WrData);
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_REALTIME_BattRunTimeLo:
			#if(cEEBattRunTimeLoLimitLo==0)
			if(WrData <= cEEBattRunTimeLoLimitUp)
			#else
			if((WrData >= cEEBattRunTimeLoLimitLo) && (WrData <= cEEBattRunTimeLoLimitUp))
			#endif
			{
				if(sbGetEepromSecureLock() == false)
				{
					sSetEepromBattRunTimeLo(WrData);
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case EE_REALTIME_BattRunTimeHi:
			#if(cEEBattRunTimeHiLimitLo==0)
			if(WrData <= cEEBattRunTimeHiLimitUp)
			#else
			if((WrData >= cEEBattRunTimeHiLimitLo) && (WrData <= cEEBattRunTimeHiLimitUp))
			#endif
			{
				if(sbGetEepromSecureLock() == false)
				{
					sSetEepromBattRunTimeHi(WrData);
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		default:break;
	}
	
	if(DoEEDynamicSave != 0)
	{
		sSetEepromDynamicMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	
	if(DoEERealtimeSave != 0)
	{
		sSetEepromRealtimeMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	
	return ret;
}

INT8U	swATESettingWrite(INT16U WrAddr, INT16U WrData)
{
	INT8U ret = 0;
	INT8U DoEEStaticSave = false;
	INT8U DoEEDynamicSave = false;
	INT8U DoEERealtimeSave = false;
	INT8U DoEEFaultRecordSave = false;
	INT16U	wTempCmd, wTempData;
	
	WrAddr -= ATE_REG_BASE_ADDR;
	switch(WrAddr)
	{
		case ATE_Serial1:
			#if(cEESerialNoLimitLo==0)
			if(WrData <= cEESerialNoLimitUp)
			#else
			if((WrData >= cEESerialNoLimitLo) && (WrData <= cEESerialNoLimitUp))
			#endif
			{
				sSetEepromSerialNo1(WrData);
				DoEEStaticSave = true;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case ATE_Serial2:
			#if(cEESerialNoLimitLo==0)
			if(WrData <= cEESerialNoLimitUp)
			#else
			if((WrData >= cEESerialNoLimitLo) && (WrData <= cEESerialNoLimitUp))
			#endif
			{
				sSetEepromSerialNo2(WrData);
				DoEEStaticSave = true;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case ATE_Serial3:
			#if(cEESerialNoLimitLo==0)
			if(WrData <= cEESerialNoLimitUp)
			#else
			if((WrData >= cEESerialNoLimitLo) && (WrData <= cEESerialNoLimitUp))
			#endif
			{
				sSetEepromSerialNo3(WrData);
				DoEEStaticSave = true;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case ATE_Serial4:
			#if(cEESerialNoLimitLo==0)
			if(WrData <= cEESerialNoLimitUp)
			#else
			if((WrData >= cEESerialNoLimitLo) && (WrData <= cEESerialNoLimitUp))
			#endif
			{
				sSetEepromSerialNo4(WrData);
				DoEEStaticSave = true;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case ATE_Serial5:
			#if(cEESerialNoLimitLo==0)
			if(WrData <= cEESerialNoLimitUp)
			#else
			if((WrData >= cEESerialNoLimitLo) && (WrData <= cEESerialNoLimitUp))
			#endif
			{
				sSetEepromSerialNo5(WrData);
				DoEEStaticSave = true;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case ATE_SNLen:
			#if(cEESerialNoLimitLo==0)
			if(WrData <= cEESerialNoLimitUp)
			#else
			if((WrData >= cEESerialNoLimitLo) && (WrData <= cEESerialNoLimitUp))
			#endif
			{
				sSetEepromSerialNoLen(WrData);
				DoEEStaticSave = true;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case ATE_DispHwVer:
			ret = cErr_UnDataAddr;
		break;
		case ATE_CtrlHwVer:
			ret = cErr_UnDataAddr;
		break;
		case ATE_PowerHwVer:
			ret = cErr_UnDataAddr;
		break;
		case ATE_UpdateFW:
			if(WrData == cEnable)
			{
				OSEventSend(cPrioSuper, eSuperToUpgrade);
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case ATE_ResetSystem:
			if(WrData == 1)
			{
				while(1)
				{
					;	// Tirg Watchdog Reset
				}
			}
			else if(WrData == 2)	// 广播关机指令
			{
				sSetBmsParallelShutdown(true);
				OSEventSend(cPrioSuper, eSuperToShutdown);
			}
			else if(WrData == 3)	// 只关本机系统
			{
				OSEventSend(cPrioSuper, eSuperToShutdown);
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case ATE_EEDefault:
			if((sbGetEepromSecureLock() == false) \
				&& (WrData > 0x00) && (WrData <= 0x0F))
			{
				if((WrData & 0x01) != 0)
				{
					sDefaultEepromDynamic();
					DoEEDynamicSave = 1;
				}
				if((WrData & 0x02) != 0)
				{
					sDefaultEepromRealtime();
					DoEERealtimeSave = 1;
				}
				if((WrData & 0x04) != 0)
				{
					sDefaultEepromFaultRecord();
					DoEEFaultRecordSave = 1;
				}
				if((WrData & 0x08) != 0)
				{
					sDefaultEepromStatic();
					DoEEStaticSave = 1;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case ATE_AgingMode:
			if(WrData == cEnable)
			{
				
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case ATE_BattVoltAdj0:
			if(WrData == cEnable)
			{
				sBattVoltAdj0();
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case ATE_BattVoltAdj1:
			ret = sbBattVoltAdj1(WrData);
		break;
		case ATE_BattVoltAdj2:
			ret = sbBattVoltAdj2(WrData);
		break;
		case ATE_BattVoltAdj:
			ret = cErr_ParUnValid;
		break;
		case ATE_SecureUnLock:
			if(WrData == 1234)	// unlock
			{
				// 先输入密码,密码正确后，才允许解锁设备,默认不解锁
				// 1\当检测到EEPROM是默认值,则可能烧录程序泄露或者EEPROM出错(概率很低)
				// 则允许程序运行3-6个月,然后不工作
				// 2\当检测到EEPROM是默认值,
				sSetEepromSecureLock(false);
			}
			else
			{
				sSetEepromSecureLock(true);
				ret = cErr_ParUnValid;
			}
		break;
		case ATE_Factory:
			if((sbGetEepromSecureLock() == false) && (WrData == cEnable))
			{
				sDefaultEepromDynamic();
				DoEEDynamicSave = 1;
			}
			else if((sbGetEepromSecureLock() == false) && (WrData == 2))
			{
				sDefaultEepromRealtime();
				DoEERealtimeSave = 1;
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case ATE_BattType:
			if(sbGetEepromSecureLock() == false)
			{
				if(sBmsBattTypeChk(WrData))
				{
					sSetEepromBattType(WrData);
					sBmsInfoUpdate();
					sSetEepromBattAhMax(swGetBmsRatedAH());
					sSoxParaUpdate();
					sSetSoxAhMax(swGetBmsRatedAH());
					sSetEepromBattChgCurrHiAlmBack(swGetBmsCurrAlarmBack());
					sSetEepromBattChgCurrHiAlm(swGetBmsCurrAlarm());
					sSetEepromBattChgCurrHi(swGetBmsCurrHigh1());
					sSetEepromBattChgCurrHi2(swGetBmsCurrHigh2());
					sSetEepromBattDcgCurrHiAlmBack(swGetBmsCurrAlarmBack());
					sSetEepromBattDcgCurrHiAlm(swGetBmsCurrAlarm());
					sSetEepromBattDcgCurrHi(swGetBmsCurrHigh1());
					sSetEepromBattDcgCurrHi2(swGetBmsCurrHigh2());
					DoEEStaticSave = true;
					DoEEDynamicSave = true;
					DoEERealtimeSave = true;
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case ATE_BattVoltAdj3:
			if(WrData == 1)
			{
				sBattVoltAdj0();
			}
			else
			{
				wTempCmd = WrData / 10000;
				wTempData = WrData % 10000;
				if(wTempCmd == 1)
				{
					ret = sbBattVoltAdj1(wTempData);
				}
				else if(wTempCmd == 2)
				{
					ret = sbBattVoltAdj2(wTempData);
				}
				else if(wTempCmd == 0)
				{
					ret = sbBattVoltAdj(wTempData);
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
		break;
		case ATE_BattCurrAdj3:
			if(WrData == 1)
			{
				sBattChgCurrAdj0();
				sBattDsgCurrAdj0();
			}
			else
			{
				wTempCmd = WrData / 10000;
				wTempData = WrData % 10000;
				if(wTempCmd == 0)
				{
					ret = sbBattCurrAdj(wTempData);
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
		break;
		case ATE_BattCurr2Adj3:
			wTempCmd = WrData / 10000;
			wTempData = WrData % 10000;
			if(wTempCmd == 1)
			{
				ret = sbBattChgCurrAdj(wTempData);
			}
			else if(wTempCmd == 2)
			{
				ret = sbBattDsgCurrAdj(wTempData);
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		break;
		case ATE_BattCellVoltAdj:
			if(WrData == 1)
			{
				sBattVoltAdj0();
				sbBattVoltCopyToCell();
			}
			else
			{
				wTempCmd = WrData / 10000;
				wTempData = WrData % 10000;
				if(wTempCmd == 1)
				{
					ret = sbBattVoltAdj1(wTempData);
				}
				else if(wTempCmd == 2)
				{
					ret = sbBattVoltAdj2(wTempData);
					if(ret == 0)
					{
						sbBattVoltCopyToCell();
					}
				}
				else if(wTempCmd == 0)
				{
					ret = sbBattVoltAdj(wTempData);
					if(ret == 0)
					{
						sbBattVoltCopyToCell();
					}
				}
				else
				{
					ret = cErr_ParUnValid;
				}
			}
		break;
		default:break;
	}
	
	if(DoEEStaticSave != 0)
	{
		sSetEepromStaticMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	if(DoEEDynamicSave != 0)
	{
		sSetEepromDynamicMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	if(DoEERealtimeSave != 0)
	{
		sSetEepromRealtimeMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	if(DoEEFaultRecordSave != 0)
	{
		sSetEepromFaultRecordMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	
	return ret;
}

INT16U	swDbgCmdRead(INT16U RdAddr)
{
	INT16U wTemp;
	
	RdAddr -= DBG_CMD_BASE_ADDR;
	switch(RdAddr)
	{
		case DBG_XXXXXXX00:
			wTemp = swGetBalanceCellStatus();
		break;
		case DBG_XXXXXXX01:
			wTemp = swGetHwVersion2Real();
		break;
		case DBG_XXXXXXX02:
			wTemp = 0;
		break;
		case DBG_XXXXXXX03:
			wTemp = swGetAdcCurrOffset();
		break;
		case DBG_XXXXXXX04:
			wTemp = swGetHwVersion1Real();
		break;
		case DBG_XXXXXXX05:
			wTemp = mFLASH_GET_READ_PRO();	// 1=FLASH读保护生效
		break;
		case DBG_XXXXXXX06:
			wTemp = 0;
		break;
		case DBG_XXXXXXX07:
			wTemp = 9;
		break;
		case DBG_XXXXXXX08:
			wTemp = 0;
		break;
		case DBG_XXXXXXX09:
			wTemp = 0;
		break;
		case DBG_XXXXXXX0A:
			wTemp = 0;
		break;
		case DBG_XXXXXXX0B:
			wTemp = 0;
		break;
		case DBG_XXXXXXX0C:
			wTemp = 0;
		break;
		case DBG_XXXXXXX0D:
			wTemp = 0;
		break;
		case DBG_XXXXXXX0E:
			wTemp = 0;
		break;
		case DBG_XXXXXXX0F:
			wTemp = 0;
		break;
		case DBG_XXXXXXX10:
			wTemp = 0;
		break;
		case DBG_XXXXXXX11:
			wTemp = 0;
		break;
		case DBG_XXXXXXX12:
			wTemp = 0;
		break;
		case DBG_XXXXXXX13:
			wTemp = 0;
		break;
		case DBG_XXXXXXX14:
			wTemp = 0;
		break;
		case DBG_XXXXXXX15:
			wTemp = 0;
		break;
		case DBG_XXXXXXX16:
			wTemp = 0;
		break;
		case DBG_XXXXXXX17:
			wTemp = 0;
		break;
		case DBG_XXXXXXX18:
			wTemp = 0;
		break;
		case DBG_XXXXXXX19:
			wTemp = 0;
		break;
		case DBG_XXXXXXX1A:
			wTemp = 0;
		break;
		case DBG_XXXXXXX1B:
			wTemp = 0;
		break;
		case DBG_XXXXXXX1C:
			wTemp = 0;
		break;
		case DBG_XXXXXXX1D:
			wTemp = 0;
		break;
		case DBG_XXXXXXX1E:
			wTemp = 0;
		break;
		case DBG_XXXXXXX1F:
			wTemp = 0;
		break;
		default:
			wTemp = 0xFFFF;
		break;
	}
	
	return wTemp;
}

INT8U	swDbgCmdWrite(INT16U WrAddr, INT16U WrData)
{
	INT8U ret = 0;
	
	WrAddr -= DBG_CMD_BASE_ADDR;
	switch(WrAddr)
	{
		case DBG_XXXXXXX00:
			sSetBalanceTestStatus(WrData);
		break;
		case DBG_XXXXXXX01:
			;
		break;
		case DBG_XXXXXXX02:
			;
		break;
		case DBG_XXXXXXX03:
			;
		break;
		case DBG_XXXXXXX04:
			;
		break;
		case DBG_XXXXXXX05:
			;
		break;
		case DBG_XXXXXXX06:
			;
		break;
		case DBG_XXXXXXX07:
			;
		break;
		case DBG_XXXXXXX08:
			;
		break;
		case DBG_XXXXXXX09:
			;
		break;
		case DBG_XXXXXXX0A:
			;
		break;
		case DBG_XXXXXXX0B:
			;
		break;
		case DBG_XXXXXXX0C:
			;
		break;
		case DBG_XXXXXXX0D:
			;
		break;
		case DBG_XXXXXXX0E:
			;
		break;
		case DBG_XXXXXXX0F:
			;
		break;
		case DBG_XXXXXXX10:
			;
		break;
		case DBG_XXXXXXX11:
			;
		break;
		case DBG_XXXXXXX12:
			;
		break;
		case DBG_XXXXXXX13:
			;
		break;
		case DBG_XXXXXXX14:
			;
		break;
		case DBG_XXXXXXX15:
			;
		break;
		case DBG_XXXXXXX16:
			;
		break;
		case DBG_XXXXXXX17:
			;
		break;
		case DBG_XXXXXXX18:
			;
		break;
		case DBG_XXXXXXX19:
			;
		break;
		case DBG_XXXXXXX1A:
			;
		break;
		case DBG_XXXXXXX1B:
			;
		break;
		case DBG_XXXXXXX1C:
			;
		break;
		case DBG_XXXXXXX1D:
			;
		break;
		case DBG_XXXXXXX1E:
			;
		break;
		case DBG_XXXXXXX1F:
			;
		break;
		default:break;
	}
	
	return ret;
}

void	sBattVoltAdj0(void)
{
	if((swGetEepromBattVoltAdj() != cEEBattVoltAdjDef) \
		|| (swGetEepromBattVoltBias() != cEEBattVoltBiasDef))
	{
		sSetBalancePassivePause(true);
		
		sSetEepromBattVoltAdj(cEEBattVoltAdjDef);
		sSetEepromBattVoltBias(cEEBattVoltBiasDef);
		
		sSetEepromStaticMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	
	sSetBattVoltAdj(cEEBattVoltAdjDef);
	sSetBattVoltBias(cEEBattVoltBiasDef);
	wBatt1Ref = 0;
	wBatt2Ref = 0;
	wBatt1Real = 0;
	wBatt2Real = 0;
}

INT8U	sbBattVoltAdj1(INT16S wVolt)
{
	INT8U	ret = 0;
	
	if((wVolt > 0) && (wVolt <= 7500) \
		&& (swGetEepromBattVoltAdj() == cEEBattVoltAdjDef) \
		&& (swGetEepromBattVoltBias() == cEEBattVoltBiasDef))
	{
		wBatt1Ref = wVolt;
		wBatt1Real = swGetBattVoltFiltNew();
		if(swAbs(wBatt1Real - wBatt1Ref) > 500)
		{
			ret = cErr_ParUnValid;
		}
	}
	else
	{
		ret = cErr_ParUnValid;
	}
	
	return ret;
}

INT8U	sbBattVoltAdj2(INT16S wVolt)
{
	INT8U	ret = 0;
	INT16S	wAdj, wBias;
	
	if((wVolt > 0) && (wVolt <= 7500) && (wBatt1Ref > 0) \
		&& (swGetEepromBattVoltAdj() == cEEBattVoltAdjDef) \
		&& (swGetEepromBattVoltBias() == cEEBattVoltBiasDef))
	{
		wBatt2Ref = wVolt;
		wBatt2Real = swGetBattVoltFiltNew();
		if((swAbs(wBatt2Real - wBatt2Ref) <= 500) \
			&& (swAbs(wBatt2Real - wBatt1Real) >= 200) \
			&& (swAbs(wBatt2Ref - wBatt1Ref) >= 200))
		{
			wAdj = (INT16S)(((INT32S)swAbs(wBatt2Ref - wBatt1Ref) << 12) / swAbs(wBatt2Real - wBatt1Real));
			wBias = (((INT32S)wBatt2Ref << 12) - ((INT32S)wBatt2Real * wAdj)) >> 5;
			if((wAdj > cEEBattVoltAdjLimitLo) && (wAdj < cEEBattVoltAdjLimitUp) \
				&& (wBias > cEEBattVoltBiasLimitLo) && (wBias < cEEBattVoltBiasLimitUp))
			{
				if(((INT16S)swGetEepromBattVoltAdj() != wAdj) || (swGetEepromBattVoltBias() != wBias))
				{
					sSetEepromBattVoltAdj(wAdj);
					sSetEepromBattVoltBias(wBias);
					
					sSetBattVoltAdj(wAdj);
					sSetBattVoltBias(wBias);
					
					sSetEepromStaticMode(cEEPROM_SAVE_MODE_ENABLE);
				}
			}
			else
			{
				ret = cErr_ParUnValid;
			}
		}
		else
		{
			ret = cErr_ParUnValid;
		}
	}
	else
	{
		ret = cErr_ParUnValid;
	}
	
	return ret;
}

INT8U	sbBattVoltAdj(INT16S wVolt)
{
	INT8U	ret = 0;
	INT16S	wAdj;
	
	if((swAbs((INT16S)wVolt - (INT16S)swGetBattVoltFiltNew()) <= 500) \
		&& (swGetBattVoltFiltNew() >= 1000) && (wVolt >= 1000))
	{
		wAdj = (INT16S)(((INT32S)wVolt * swGetEepromBattVoltAdj()) / swGetBattVoltFiltNew());
		if(wAdj > cEEBattVoltAdjLimitUp)
		{
			wAdj = cEEBattVoltAdjLimitUp;
		}
		else if(wAdj < cEEBattVoltAdjLimitLo)
		{
			wAdj = cEEBattVoltAdjLimitLo;
		}
		
		sSetBattVoltAdj(wAdj);
		sSetEepromBattVoltAdj(wAdj);
		sSetEepromStaticMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	else
	{
		ret = cErr_ParUnValid;
	}
	
	return ret;
}

INT8U	sbBattCurrAdj(INT16S wCurr)
{
	INT8U	ret = 0;
	INT16S	wAdj, wAdjPre;
	INT16S	wCurrAbs;
	
	wCurrAbs = swAbs(swGetBattCurrFiltNew());
 	if((wCurr >= 250) && (wCurrAbs >= 250))
	{
		if(swGetBattCurrFiltNew() > 0)
		{
			wAdjPre = swGetEepromBattChgCurrAdj();
		}
		else
		{
			wAdjPre = swGetEepromBattDsgCurrAdj();
		}
		wAdj = (INT16S)(((INT32S)wCurr * wAdjPre) / wCurrAbs);
		if(wAdj > cEEBattCurrAdjLimitUp)
		{
			wAdj = cEEBattCurrAdjLimitUp;
		}
		else if(wAdj < cEEBattCurrAdjLimitLo)
		{
			wAdj = cEEBattCurrAdjLimitLo;
		}
		
		sSetEepromBattChgCurrAdj(wAdj);
		sSetBattChgCurrAdj(wAdj);
		sSetEepromBattDsgCurrAdj(wAdj);
		sSetBattDsgCurrAdj(wAdj);
		
		sSetEepromStaticMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	else
	{
		ret = cErr_ParUnValid;
	}
	
	return ret;
}

void	sBattChgCurrAdj0(void)
{
	if((swGetEepromBattChgCurrAdj() != cEEBattCurrAdjDef) \
		|| (swGetEepromBattChgCurrBias() != cEEBattCurrBiasDef))
	{
		sSetEepromBattChgCurrAdj(cEEBattCurrAdjDef);
		sSetEepromBattChgCurrBias(cEEBattCurrBiasDef);
		
		sSetEepromStaticMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	
	sSetBattChgCurrAdj(cEEBattCurrAdjDef);
	sSetBattChgCurrBias(cEEBattCurrBiasDef);
}

INT8U	sbBattChgCurrAdj(INT16S wCurr)
{
	INT8U	ret = 0;
	INT16S	wAdj;
	INT16S	wCurrAbs;
	
	wCurrAbs = swAbs(swGetBattCurrFiltNew());
 	if((wCurr >= 250) && (wCurrAbs >= 250) && (swGetBattCurrFiltNew() > 0))
	{
		wAdj = (INT16S)(((INT32S)wCurr * swGetEepromBattChgCurrAdj()) / wCurrAbs);
		if(wAdj > cEEBattCurrAdjLimitUp)
		{
			wAdj = cEEBattCurrAdjLimitUp;
		}
		else if(wAdj < cEEBattCurrAdjLimitLo)
		{
			wAdj = cEEBattCurrAdjLimitLo;
		}
		
		sSetEepromBattChgCurrAdj(wAdj);
		sSetBattChgCurrAdj(wAdj);
		
		sSetEepromStaticMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	else
	{
		ret = cErr_ParUnValid;
	}
	
	return ret;
}

void	sBattDsgCurrAdj0(void)
{
	if((swGetEepromBattDsgCurrAdj() != cEEBattCurrAdjDef) \
		|| (swGetEepromBattDsgCurrBias() != cEEBattCurrBiasDef))
	{
		sSetEepromBattDsgCurrAdj(cEEBattCurrAdjDef);
		sSetEepromBattDsgCurrBias(cEEBattCurrBiasDef);
		
		sSetEepromStaticMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	
	sSetBattDsgCurrAdj(cEEBattCurrAdjDef);
	sSetBattDsgCurrBias(cEEBattCurrBiasDef);
}

INT8U	sbBattDsgCurrAdj(INT16S wCurr)
{
	INT8U	ret = 0;
	INT16S	wAdj;
	INT16S	wCurrAbs;
	
	wCurrAbs = swAbs(swGetBattCurrFiltNew());
	if((wCurr >= 250) && (wCurrAbs >= 250) && (swGetBattCurrFiltNew() < 0))
	{
		wAdj = (INT16S)(((INT32S)wCurr * swGetEepromBattDsgCurrAdj()) / wCurrAbs);
		if(wAdj > cEEBattCurrAdjLimitUp)
		{
			wAdj = cEEBattCurrAdjLimitUp;
		}
		else if(wAdj < cEEBattCurrAdjLimitLo)
		{
			wAdj = cEEBattCurrAdjLimitLo;
		}
		
		sSetEepromBattDsgCurrAdj(wAdj);
		sSetBattDsgCurrAdj(wAdj);
		
		sSetEepromStaticMode(cEEPROM_SAVE_MODE_ENABLE);
	}
	else
	{
		ret = cErr_ParUnValid;
	}
	
	return ret;
}

INT8U	sGetCellVoltDefault(INT8U bNo)
{
	INT8U	bEEDefault = false;
	
	if((swGetEepromCellVoltAdj(bNo) == cEEBattVoltAdjDef) \
		&& (swGetEepromCellVoltBias(bNo) == cEEBattVoltBiasDef))
	{
		bEEDefault = true;
	}
	return bEEDefault;
}

void	sbBattVoltCopyToCell(void)
{
	INT8U	bIndex;
	
	for(bIndex = 0; bIndex < cADC_CELL_VOLT_MAX; bIndex++)
	{
		sSetEepromCellVoltAdj(bIndex, swGetEepromBattVoltAdj());
		sSetEepromCellVoltBias(bIndex, swGetEepromBattVoltBias());
		sSetCellVoltAdj(bIndex, swGetEepromBattVoltAdj());
		sSetCellVoltBias(bIndex, swGetEepromBattVoltBias());
	}
	
	sSetEepromStaticMode(cEEPROM_SAVE_MODE_ENABLE);
}

void	sCommConnectChk(INT8U sciid, INT16U wFilter)
{
	
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
INT8U	sbGetCommConnected(INT8U sciid)
{
	return 0;
}

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
void	sSetCommConnected(INT8U sciid, INT8U bStatus)
{
	
}

void	sGroWatt_ModBusParsing(INT8U sciid)
{
	INT8U	*RxBuff;
	INT8U	*TxBuff;
	INT8U	packet_len;
	INT16U	temp;
	INT8U	bSciIdTemp;
	
	bSciIdTemp = sciid;
	RxBuff = pbGetSciRxBuff(sciid);
	TxBuff = pbGetSciTxBuff(sciid);

	if(sbGetSciAddress() == cMODBUS_ADDR_ERR)
	{
		return;
	}	
	
	// ID
	if(sbIsCanDeviceHost())
	{
		if((RxBuff[cMODBUS_ADDR] != sbGetSciAddress()) \
			&& (RxBuff[cMODBUS_ADDR] != swGetEepromModuleAddr()) \
			&& (RxBuff[cMODBUS_ADDR] != cMODBUS_ADDR_HOST) \
			&& (RxBuff[cMODBUS_ADDR] != cMODBUS_ADDR_BACKUP))
		{
			return;
		}
	}
	else
	{
		if((RxBuff[cMODBUS_ADDR] != sbGetSciAddress()) \
			&& (RxBuff[cMODBUS_ADDR] != swGetEepromModuleAddr()) \
			&& (RxBuff[cMODBUS_ADDR] != cMODBUS_ADDR_BACKUP))
		{
			return;
		}
	}
	
	switch(RxBuff[cMODBUS_CMD])
	{
		case cREAD_CMD:
			packet_len = 8;
			sGrowatt_ModBusRdProc(RxBuff, TxBuff, bSciIdTemp, 8);
		break;
		case cWRITE_CMD:
			packet_len = 8;
			sGrowatt_ModBusWrProc(RxBuff, TxBuff, bSciIdTemp, 8);
		break;
		case cWRITE_CMD_MULTI:
			temp = (((INT16U)RxBuff[cMODBUS_REG_NUM_H] << 8) + RxBuff[cMODBUS_REG_NUM_L]) << 1;
			temp += 9;
			if(temp > cSCI_RX_LEN)
			{
				packet_len = cSCI_RX_LEN;
			}
			else
			{
				packet_len = (INT8U)temp;
			}
			sGrowatt_ModBusWrProc(RxBuff, TxBuff, bSciIdTemp, packet_len);
		break;
		default:
			packet_len = 8;
		break;
	}
}

void	sGrowatt_ModBusRdProc(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U RxLen)
{
	INT16U RdAddr, RdNums, wCRC;
	
	//CRC校验
	wCRC = ((INT16U)RxBuff[RxLen - cMODBUS_CRC_L] << 8) + RxBuff[RxLen - cMODBUS_CRC_H];
	if(wCRC != swModbusCrc16(&RxBuff[cMODBUS_ADDR], (RxLen - cMODBUS_CRC_LEN)))
	{
		sGrowatt_BuildFaultFrame(RxBuff, TxBuff, sciid, cErr_CrcErr);
		return;
	}
	
	// 
	RdAddr = ((INT16U)RxBuff[cMODBUS_REG_ADDR_H] << 8) + RxBuff[cMODBUS_REG_ADDR_L];
	RdNums = ((INT16U)RxBuff[cMODBUS_REG_NUM_H] << 8) + RxBuff[cMODBUS_REG_NUM_L];
	
	// 
	if(RdNums > cMODBUS_REG_RX_MAX_LEN)
	{
		sGrowatt_BuildFaultFrame(RxBuff, TxBuff, sciid, cErr_UnDataValue);
		return;
	}
	
	// 
	if(RdAddr < (Growatt_BMS_REG_BASE_ADDR + Growatt_BMS_REG_LEN))
    {
		if((RdAddr + RdNums) <= (Growatt_BMS_REG_BASE_ADDR + Growatt_BMS_REG_LEN))
		{
	    	sGrowatt_BuildRdFrame(RxBuff, TxBuff, sciid, swGrowatt_BMSDataRead);
		}
		else
		{
	    	sGrowatt_BuildFaultFrame(RxBuff, TxBuff, sciid, cErr_ParUnValid);
		}
	}
	else
	{
		sGrowatt_BuildFaultFrame(RxBuff, TxBuff, sciid, cErr_UnDataAddr);
	}
}

void	sGrowatt_ModBusWrProc(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U RxLen)
{
	INT16U WrAddr, WrNums, wCRC;
	
	// 
	wCRC = ((INT16U)RxBuff[RxLen - cMODBUS_CRC_L] << 8) + RxBuff[RxLen - cMODBUS_CRC_H];
	if(wCRC != swModbusCrc16(&RxBuff[cMODBUS_ADDR], (RxLen - cMODBUS_CRC_LEN)))
	{
		sGrowatt_BuildFaultFrame(RxBuff, TxBuff, sciid, cErr_CrcErr);
		return;
	}
	
	// 
	WrAddr = ((INT16U)RxBuff[cMODBUS_REG_ADDR_H] << 8) + RxBuff[cMODBUS_REG_ADDR_L];
	if(RxBuff[cMODBUS_CMD] == cWRITE_CMD)
	{
		WrNums = 1;
	}
	else
	{
		WrNums = (RxBuff[cMODBUS_REG_NUM_H] << 8) + RxBuff[cMODBUS_REG_NUM_L];
	}
	
	// 
	if(WrNums > cMODBUS_REG_TX_MAX_LEN)
	{
		sGrowatt_BuildFaultFrame(RxBuff, TxBuff, sciid, cErr_UnDataValue);
		return;
	}
	
	// 
	if((WrAddr >= EE_REG_BASE_ADDR + EE_STATIC_AddrStart) && (WrAddr < (EE_REG_BASE_ADDR + EE_STATIC_AddrStart + EE_STATIC_Len)))
	{
		if((WrAddr + WrNums) <= (EE_REG_BASE_ADDR + EE_STATIC_AddrStart + EE_STATIC_Len))
		{
			sGrowatt_BuildWrFrame(RxBuff, TxBuff, sciid, swGrowatt_EESettingWrite);
		}
		else
		{
			sGrowatt_BuildFaultFrame(RxBuff, TxBuff, sciid, cErr_ParUnValid);
		}
	}
	else
	{
		sGrowatt_BuildFaultFrame(RxBuff, TxBuff, sciid, cErr_ParUnValid);
	}
}

void	sGrowatt_BuildRdFrame(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT16U (*RdFunc)(INT16U))
{
	INT16U RdAddr, RdNums, RdData, wCRC;
	INT8U i, TxLen;
	
	RdAddr = ((INT16U)RxBuff[cMODBUS_REG_ADDR_H] << 8) + RxBuff[cMODBUS_REG_ADDR_L];
	RdNums = ((INT16U)RxBuff[cMODBUS_REG_NUM_H] << 8) + RxBuff[cMODBUS_REG_NUM_L];
	
	TxBuff[cMODBUS_ADDR] = RxBuff[cMODBUS_ADDR];
	TxBuff[cMODBUS_CMD] = RxBuff[cMODBUS_CMD];
	TxBuff[2] = (RdNums << 1) & 0x00FF;
	TxLen = 3;
	
	for(i = 0; i < RdNums; i++)
	{
		RdData = (*RdFunc)(RdAddr);
		RdAddr++;
		
		TxBuff[TxLen++] = RdData >> 8;
		TxBuff[TxLen++] = RdData & 0x00FF;
	}
	
	wCRC = swModbusCrc16(&TxBuff[cMODBUS_ADDR], TxLen);
    TxBuff[TxLen++] = wCRC & 0x00FF;
	TxBuff[TxLen++] = wCRC >> 8;
	
	if(sSciGetTxStatus(sciid) != cSciTxBusy)
	{
		sSciWrite(sciid, TxBuff, TxLen);
		sSetCommConnected(sciid, true);
		sRstCommLossCount();
	}
}

void	sGrowatt_BuildWrFrame(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U (*WrFunc)(INT16U, INT16U))
{
	INT8U *pBuff = NULL;
	INT16U WrAddr, WrNums, WrData, wCRC;
	INT8U i, Status;
	
	WrAddr = ((INT16U)RxBuff[cMODBUS_REG_ADDR_H] << 8) + RxBuff[cMODBUS_REG_ADDR_L];
	if(RxBuff[cMODBUS_CMD] == cWRITE_CMD)
	{
		WrNums = 1;
		pBuff = &RxBuff[cMODBUS_WRITE_DATA_START];
	}
	else
	{
		WrNums = ((INT16U)RxBuff[cMODBUS_REG_NUM_H] << 8) + RxBuff[cMODBUS_REG_NUM_L];
		pBuff = &RxBuff[cMODBUS_MULTI_WRITE_DATA_START];
	}
	
	for(i = 0; i < WrNums; i++)
	{
		WrData = ((INT16U)pBuff[0] << 8) + pBuff[1];
		pBuff += 2;
		
		Status = (*WrFunc)(WrAddr, WrData);
		WrAddr++;
		
		if(Status != 0)
		{
			sGrowatt_BuildFaultFrame(RxBuff, TxBuff, sciid, Status);
			return;
		}
	}
	
	TxBuff[cMODBUS_ADDR] = RxBuff[cMODBUS_ADDR];
	TxBuff[cMODBUS_CMD] = RxBuff[cMODBUS_CMD];
	TxBuff[cMODBUS_REG_ADDR_H] = RxBuff[cMODBUS_REG_ADDR_H];
	TxBuff[cMODBUS_REG_ADDR_L] = RxBuff[cMODBUS_REG_ADDR_L];
	TxBuff[cMODBUS_REG_NUM_H] = RxBuff[cMODBUS_REG_NUM_H];
	TxBuff[cMODBUS_REG_NUM_L] = RxBuff[cMODBUS_REG_NUM_L];
	wCRC = swModbusCrc16(&TxBuff[cMODBUS_ADDR], 6);
    TxBuff[6] = wCRC & 0x00FF;
	TxBuff[7] = wCRC >> 8;
	
	if(sSciGetTxStatus(sciid) != cSciTxBusy)
	{
		sSciWrite(sciid, TxBuff, 8);
		sSetCommConnected(sciid, true);
		sRstCommLossCount();
	}
}

void	sGrowatt_BuildFaultFrame(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U ErrCode)
{
	INT16U wCRC;
	
	TxBuff[cMODBUS_ADDR] = RxBuff[cMODBUS_ADDR];
	TxBuff[cMODBUS_CMD] = RxBuff[cMODBUS_CMD] + 0x80;
	TxBuff[2] = ErrCode;
	wCRC = swModbusCrc16(&TxBuff[cMODBUS_ADDR], 3);
	TxBuff[3] = wCRC & 0x00FF;
    TxBuff[4] = wCRC >> 8;
	
	if(sSciGetTxStatus(sciid) != cSciTxBusy)
	{
		sSciWrite(sciid, TxBuff, 5);
	}
}

INT16U swGrowatt_BMSDataRead(INT16U RdAddr)
{
	return swGetGrowatt_BmsDataLocal(RdAddr);
}

INT8U	swGrowatt_EESettingWrite(INT16U WrAddr, INT16U WrData)
{
	return 0;
}

void	sVoltronic_ModBusParsing(INT8U sciid)
{
	INT8U	*RxBuff;
	INT8U	*TxBuff;
	INT8U	bSciIdTemp;
	
	bSciIdTemp = sciid;
	RxBuff = pbGetSciRxBuff(sciid);
	TxBuff = pbGetSciTxBuff(sciid);

	if(sbGetSciAddress() == cMODBUS_ADDR_ERR)
	{
		return;
	}	
	
	// ID
	if(sbIsCanDeviceHost())
	{
		if((RxBuff[cMODBUS_ADDR] != sbGetSciAddress()) \
			&& (RxBuff[cMODBUS_ADDR] != swGetEepromModuleAddr()) \
			&& (RxBuff[cMODBUS_ADDR] != cMODBUS_ADDR_HOST) \
			&& (RxBuff[cMODBUS_ADDR] != cMODBUS_ADDR_BACKUP))
		{
			return;
		}
	}
	else
	{
		if((RxBuff[cMODBUS_ADDR] != sbGetSciAddress()) \
			&& (RxBuff[cMODBUS_ADDR] != swGetEepromModuleAddr()) \
			&& (RxBuff[cMODBUS_ADDR] != cMODBUS_ADDR_BACKUP))
		{
			return;
		}
	}
	
	switch(RxBuff[cMODBUS_CMD])
	{
		case cREAD_CMD:
			sVoltronic_ModBusRdProc(RxBuff, TxBuff, bSciIdTemp, 8);
		break;
		default:
		break;
	}
}

void	sVoltronic_ModBusRdProc(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U RxLen)
{
	INT16U RdAddr, RdNums, wCRC;
	
	// 
	wCRC = ((INT16U)RxBuff[RxLen - cMODBUS_CRC_H] << 8) + RxBuff[RxLen - cMODBUS_CRC_L];
	if(wCRC != swModbusCrc16(&RxBuff[cMODBUS_ADDR], (RxLen - cMODBUS_CRC_LEN)))
	{
		sVoltronic_BuildFaultFrame(RxBuff, TxBuff, sciid, 3);
		return;
	}
	
	// 
	RdAddr = ((INT16U)RxBuff[cMODBUS_REG_ADDR_H] << 8) + RxBuff[cMODBUS_REG_ADDR_L];
	RdNums = ((INT16U)RxBuff[cMODBUS_REG_NUM_H] << 8) + RxBuff[cMODBUS_REG_NUM_L];
	
	// 
	if(RdNums > cMODBUS_REG_RX_MAX_LEN)
	{
		sVoltronic_BuildFaultFrame(RxBuff, TxBuff, sciid, 2);
		return;
	}
	
	// 
	if(RdAddr < (Voltronic_BMS_REG_BASE_ADDR + Voltronic_BMS_REG_LEN))
    {
		if((RdAddr + RdNums) <= (Voltronic_BMS_REG_BASE_ADDR + Voltronic_BMS_REG_LEN))
		{
	    	sVoltronic_BuildRdFrame(RxBuff, TxBuff, sciid, swVoltronic_BMSDataRead);
		}
		else
		{
	    	sVoltronic_BuildFaultFrame(RxBuff, TxBuff, sciid, 2);
		}
	}
	else
	{
		sVoltronic_BuildFaultFrame(RxBuff, TxBuff, sciid, 2);
	}
}

void	sVoltronic_BuildRdFrame(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT16U (*RdFunc)(INT16U))
{
	INT16U RdAddr, RdNums, RdData, wCRC;
	INT8U i, TxLen;
	
	RdAddr = ((INT16U)RxBuff[cMODBUS_REG_ADDR_H] << 8) + RxBuff[cMODBUS_REG_ADDR_L];
	RdNums = ((INT16U)RxBuff[cMODBUS_REG_NUM_H] << 8) + RxBuff[cMODBUS_REG_NUM_L];
	
	TxBuff[cMODBUS_ADDR] = RxBuff[cMODBUS_ADDR];
	TxBuff[cMODBUS_CMD] = RxBuff[cMODBUS_CMD];
	TxBuff[3] = (RdNums << 1) >> 8;
    TxBuff[2] = (RdNums << 1) & 0x00FF;
	TxLen = 4;
	
	for(i = 0; i < RdNums; i++)
	{
		RdData = (*RdFunc)(RdAddr);
		RdAddr++;
		
		TxBuff[TxLen++] = RdData >> 8;
		TxBuff[TxLen++] = RdData & 0x00FF;
	}
	
	wCRC = swModbusCrc16(&TxBuff[cMODBUS_ADDR], TxLen);
	TxBuff[TxLen++] = wCRC >> 8;
    TxBuff[TxLen++] = wCRC & 0x00FF;
	
	if(sSciGetTxStatus(sciid) != cSciTxBusy)
	{
		sSciWrite(sciid, TxBuff, TxLen);
		sSetCommConnected(sciid, true);
		sRstCommLossCount();
	}
}

void	sVoltronic_BuildFaultFrame(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U ErrCode)
{
	INT16U wCRC;
	
	TxBuff[cMODBUS_ADDR] = RxBuff[cMODBUS_ADDR];
	TxBuff[cMODBUS_CMD] = RxBuff[cMODBUS_CMD] + 0x80;
	TxBuff[2] = ErrCode;
	wCRC = swModbusCrc16(&TxBuff[cMODBUS_ADDR], 3);
	TxBuff[4] = wCRC & 0x00FF;
    TxBuff[3] = wCRC >> 8;
	
	if(sSciGetTxStatus(sciid) != cSciTxBusy)
	{
		sSciWrite(sciid, TxBuff, 5);
	}
}

INT16U swVoltronic_BMSDataRead(INT16U RdAddr)
{
	return swGetVoltronic_BmsDataLocal(RdAddr);
}

