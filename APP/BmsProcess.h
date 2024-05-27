/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		BmsProcess.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __BMS_PROCESS_H__
#define __BMS_PROCESS_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef  __BMS_PROCESS_C__
#define BMS_PROCESS
#else
#define BMS_PROCESS extern
#endif

#define	cBMS_CURR_IDLE					0
#define	cBMS_CURR_DSG					1
#define	cBMS_CURR_CHG					2

#define	cBMS_SOFTSTART_ENABLE			0	// 输出软起使能

#define	cSS_MODE_STANDBY				0
#define	cSS_MODE_READY					1
#define	cSS_MODE_DELAY					2
#define	cSS_MODE_CHECK					3
#define	cSS_MODE_PASS					4
#define	cSS_MODE_FINISHED				5
#define	cSS_MODE_TIMEOUT				6

// Fault/Alarm Code
#define	cBattVoltHiId                   1
#define	cBattVoltLoId                   2
#define	cBattChgCurrHiId                3
#define	cBattDcgCurrHiId                4
#define	cCellVoltHiId                   5
#define	cCellVoltLoId                   6
#define	cCellTempHiId                   7
#define	cCellTempLoId                   8
#define	cHsTempHiId                    	9
#define	cHsTempLoId                    	10
#define	cSampSensorId					11
#define	cOutputResLoId					12
#define	cParallelFaultId				13

// Fault Code bit
#define	cBattVoltHiFault                0x0001
#define	cBattVoltLoFault                0x0002
#define	cBattChgCurrHiFault             0x0004
#define	cBattDcgCurrHiFault             0x0008
#define	cCellVoltHiFault                0x0010
#define	cCellVoltLoFault                0x0020
#define	cCellTempHiFault                0x0040
#define	cCellTempLoFault                0x0080
#define	cHsTempHiFault                 	0x0100
#define	cHsTempLoFault                 	0x0200
#define	cSampSensorFault          		0x0400
#define	cOutputResLoFault				0x0800
#define	cParallelFault					0x1000

// Warning Code bit
#define	cBattVoltHiAlarm                0x0001
#define	cBattVoltLoAlarm                0x0002
#define	cBattChgCurrHiAlarm             0x0004
#define	cBattDcgCurrHiAlarm             0x0008
#define	cCellVoltHiAlarm                0x0010
#define	cCellVoltLoAlarm                0x0020
#define	cCellTempHiAlarm                0x0040
#define	cCellTempLoAlarm                0x0080
#define	cHsTempHiAlarm                 	0x0100
#define	cHsTempLoAlarm               	0x0200
//#define	cFanLockedAlarm				0x0400	// 未使用
#define	cEepromRwAlarm                  0x0800
#define	cAdcTimingAlarm                 0x1000
#define	cBreakWireAlarm					0x2000
#define	cCellVoltErrHiAlarm				0x4000

#define	cBMS_IDLE_CURR_CHG				((INT16S)20)		// 0.1A
#define	cBMS_IDLE_CURR_DEF				((INT16S)0)			// 0.1A
#define	cBMS_IDLE_CURR_DSG				(-(INT16S)20)		// 0.1A

// 电池信息
enum BMS_BATT_TYPE
{
	// BA48V/0.5C=41X
	eBATT_TYPE_48C5_MIN = 410,
	eBATT_TYPE_48050_C5 = eBATT_TYPE_48C5_MIN,
	eBATT_TYPE_48100_C5,
	eBATT_TYPE_48150_C5,
	eBATT_TYPE_48200_C5,
	eBATT_TYPE_48250_C5,
	eBATT_TYPE_48300_C5,
	eBATT_TYPE_48350_C5,
	eBATT_TYPE_48C5_MAX,
	
	// BF48V/1C=43X
	eBATT_TYPE_48C10_MIN = 430,
	eBATT_TYPE_48050_C10 = eBATT_TYPE_48C10_MIN,
	eBATT_TYPE_48100_C10,
	eBATT_TYPE_48150_C10,
	eBATT_TYPE_48200_C10,
	eBATT_TYPE_48250_C10,
	eBATT_TYPE_48300_C10,
	eBATT_TYPE_48350_C10,
	eBATT_TYPE_48400_C10,
	eBATT_TYPE_48C10_MAX,
	
	// BA24V/0.5C=21X
	eBATT_TYPE_24C5_MIN = 210,
	eBATT_TYPE_24050_C5 = eBATT_TYPE_24C5_MIN,
	eBATT_TYPE_24100_C5,
	eBATT_TYPE_24150_C5,
	eBATT_TYPE_24200_C5,
	eBATT_TYPE_24250_C5,
	eBATT_TYPE_24300_C5,
	eBATT_TYPE_24350_C5,
	eBATT_TYPE_24C5_MAX,
	
	// BF24V/1C=23X
	eBATT_TYPE_24C10_MIN = 230,
	eBATT_TYPE_24050_C10 = eBATT_TYPE_24C10_MIN,
	eBATT_TYPE_24100_C10,
	eBATT_TYPE_24150_C10,
	eBATT_TYPE_24200_C10,
	eBATT_TYPE_24250_C10,
	eBATT_TYPE_24300_C10,
	eBATT_TYPE_24350_C10,
	eBATT_TYPE_24C10_MAX,
};

// 电池类型
#define	cBMS_CELL_USE					0	// 自定义电池
#define	cBMS_CELL_LFP					1	// 磷酸铁锂电池
#define	cBMS_CELL_NCM					2	// 三元(镍钴锰酸锂LiNiCoMnO2)锂电池
#define	cBMS_CELL_LMO					3	// 锰酸锂电池
#define	cBMS_CELL_LCO					4	// 钴酸锂
#define	cBMS_CELL_TYPE_MAX				5

// 电芯温感类型
#define	cBMS_TEMP_153F3950				0
#define	cBMS_TEMP_103F3950				1
#define	cBMS_TEMP_TYPE_MAX				2

// 电池电压0.1V
#define	cBMS_VOLT_12V					120
#define	cBMS_VOLT_12V8					128
#define	cBMS_VOLT_24V					240
#define	cBMS_VOLT_25V6					256
#define	cBMS_VOLT_48V					480
#define	cBMS_VOLT_51V2					512

// 电池容量0.1A
#define	cBMS_CAP_050AH					500
#define	cBMS_CAP_080AH					800
#define	cBMS_CAP_085AH					850
#define	cBMS_CAP_090AH					900
#define	cBMS_CAP_100AH					1000
#define	cBMS_CAP_120AH					1200
#define	cBMS_CAP_130AH					1300
#define	cBMS_CAP_140AH					1400
#define	cBMS_CAP_150AH					1500
#define	cBMS_CAP_170AH					1700
#define	cBMS_CAP_200AH					2000
#define	cBMS_CAP_220AH					2200
#define	cBMS_CAP_240AH					2400
#define	cBMS_CAP_250AH					2500
#define	cBMS_CAP_260AH					2600
#define	cBMS_CAP_300AH					3000
#define	cBMS_CAP_350AH					3500
#define	cBMS_CAP_400AH					4000

// 电芯节数
#define	cBMS_CELL_VOLT_4S				4
#define	cBMS_CELL_VOLT_8S				8
#define	cBMS_CELL_VOLT_15S				15
#define	cBMS_CELL_VOLT_16S				16

// 温感数
#define	cBMS_CELL_TEMP_1S				1
#define	cBMS_CELL_TEMP_2S				2
#define	cBMS_CELL_TEMP_3S				3
#define	cBMS_CELL_TEMP_4S				4

// 均衡功能
#define	cBMS_BALANCE_DISABLE			0
#define	cBMS_BALANCE_ENABLE				1

// LED/LCD模式
#define	cBMS_LED_MODE					0
#define	cBMS_LCD_MODE					1

// 电流采样系统10/15个2mΩ电阻
#define	cBMS_CURR_SAMPLE_10				0
#define	cBMS_CURR_SAMPLE_15				1

// 电流
#define	cBMS_CURR_20A					200
#define	cBMS_CURR_25A					250
#define	cBMS_CURR_30A					300
#define	cBMS_CURR_40A					400
#define	cBMS_CURR_50A					500
#define	cBMS_CURR_55A					550
#define	cBMS_CURR_60A					600
#define	cBMS_CURR_65A					650
#define	cBMS_CURR_70A					700
#define	cBMS_CURR_75A					750
#define	cBMS_CURR_80A					800
#define	cBMS_CURR_85A					850
#define	cBMS_CURR_100A					1000
#define	cBMS_CURR_105A					1050
#define	cBMS_CURR_120A					1200
#define	cBMS_CURR_125A					1250
#define	cBMS_CURR_130A					1300
#define	cBMS_CURR_140A					1400
#define	cBMS_CURR_150A					1500
#define	cBMS_CURR_155A					1550
#define	cBMS_CURR_160A					1600
#define	cBMS_CURR_175A					1750
#define	cBMS_CURR_180A					1800
#define	cBMS_CURR_200A					2000
#define	cBMS_CURR_210A					2100
#define	cBMS_CURR_240A					2400
#define	cBMS_CURR_250A					2500
#define	cBMS_CURR_260A					2600
#define	cBMS_CURR_300A					3000
#define	cBMS_CURR_310A					3100
#define	cBMS_CURR_350A					3500
#define	cBMS_CURR_375A					3750
#define	cBMS_CURR_400A					4000
#define	cBMS_CURR_450A					4500

// 机型识别
#define	cMODEL_RSVD00_LO_LIMIT			0
#define	cMODEL_RSVD00_UP_LIMIT			205
#define	cMODEL_RSVD01_LO_LIMIT			cMODEL_RSVD00_UP_LIMIT
#define	cMODEL_RSVD01_UP_LIMIT			614
#define	cMODEL_RSVD02_LO_LIMIT			cMODEL_RSVD01_UP_LIMIT
#define	cMODEL_RSVD02_UP_LIMIT			1024
#define	cMODEL_RSVD03_LO_LIMIT			cMODEL_RSVD02_UP_LIMIT
#define	cMODEL_RSVD03_UP_LIMIT			1434
#define	cMODEL_RSVD04_LO_LIMIT			cMODEL_RSVD03_UP_LIMIT
#define	cMODEL_RSVD04_UP_LIMIT			1843
#define	cMODEL_RSVD05_LO_LIMIT			cMODEL_RSVD04_UP_LIMIT
#define	cMODEL_RSVD05_UP_LIMIT			2253
#define	cMODEL_RSVD06_LO_LIMIT			cMODEL_RSVD05_UP_LIMIT
#define	cMODEL_RSVD06_UP_LIMIT			2662
#define	cMODEL_RSVD07_LO_LIMIT			cMODEL_RSVD06_UP_LIMIT
#define	cMODEL_RSVD07_UP_LIMIT			3072
#define	cMODEL_RSVD08_LO_LIMIT			cMODEL_RSVD07_UP_LIMIT
#define	cMODEL_RSVD08_UP_LIMIT			3482
#define	cMODEL_RSVD09_LO_LIMIT			cMODEL_RSVD08_UP_LIMIT
#define	cMODEL_RSVD09_UP_LIMIT			3891
#define	cMODEL_RSVD10_LO_LIMIT			cMODEL_RSVD09_UP_LIMIT
#define	cMODEL_RSVD10_UP_LIMIT			4096

typedef struct
{
	INT16U wCellType;			// 电芯类型
	INT16U wTempType;			// 温感类型
	INT16U wBattNominalVolt;	// 名义|标称电压0.1V
	INT16U wBattNominalAH;		// 名义|标称容量0.1AH
	INT16U wBattRatedVolt;		// 实际|额定电压0.1V
	INT16U wBattRatedAH;		// 实际|额定容量0.1AH
	INT16U wCellVoltNum;		// 电芯节数(硬件识别,不可设置)
	INT16U wCellTempNum;		// 温感数|出厂设置
	INT16U wBalanceFunc;		// 是否具备均衡功能|硬件识别,不可设置
	INT16U wLedLcdMode;			// LED/LCD模式选择|硬件识别,不可设置
	INT16U wCurrSampType;		// 电流采样类型,10/15个2mΩ|硬件识别,不可设置
	
	INT16U wCurrAlarmBack;		// 告警电流恢复/0.1A
	INT16U wCurrAlarm;			// 告警电流/0.1A
	INT16U wCurrHigh1;			// 电流过流1/0.1A
	INT16U wCurrHigh2;			// 电流过流2/0.1A
	
	// 举例:LPBF48200
	// 电芯类型磷酸铁锂电池,温感153F3950
	// 标称电压48V,标称容量200AH
	// 额定电压51.2V,额定容量170AH
	// 电芯节数16串,温感数8个
	// 电流保护值:40A/50A/50A/100A
	// 电流保护值:50A/60A/60A/120A
	// 电流保护值:70A/80A/80A/120A
}BMS_InitTypeDef;

union BMS_STATUS_TABLE
{
	struct BMS_STATUS_BIT_FEILD
	{
		// BIT0-3
		INT16U rsvd1:3;
		INT16U FullCharge:1;			// 定时/SOC误差大使能,充满停止
		
		// BIT4-7
		INT16U ChargeImmediately2:1;	// 低于10%使能,高于等于20%停止
		INT16U ChargeImmediately1:1;	// 低于 5%使能,高于等于10%停止
		INT16U DischargeEnable:1;
		INT16U ChargeEnable:1;
		
		// BIT8-11
		INT16U DischargeMOS:1;
		INT16U ChargeMOS:1;
		INT16U DischargeSoftMOS:1;
		INT16U ChargeSoftMOS:1;
		
		// BIT12-15
		INT16U Discharging:1;
		INT16U Charging:1;
		INT16U rsvd4:2;
	} bits;
	INT16U data;
} ;

union BMS_STATUS_OLD_TABLE
{
	struct BMS_STATUS_OLD_BIT_FEILD
	{
		// BIT0-3
		INT16U bChgAllowFlag:1;
		INT16U bChgForceFlag:1;
		INT16U bDcgAllowFlag:1;
		INT16U bRsvd1:1;
		
		// BIT4-7
		INT16U bOpSoftFlag:1;
		INT16U bSocTimerAdjFlag:1;
		INT16U bBattStatus:2;
		
		// BIT8-11
		INT16U bRsvd3:4;
		
		// BIT12-15
		INT16U bRsvd4:4;
	} bits;
	INT16U data;
} ;

struct	BMS_DATA_FEILD
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
	INT16S PackCurrent;
	INT16U PackVoltage;
	INT16U Reservd1;
	INT16U Reservd2;
	INT16U SOC;
	INT16U SOH;
	INT16U PackCapacityHigh;
	INT16U PackCapacityLow;
	INT16U ParallelNumber;
	INT16U ParallelStatus;
	INT16U SerialNumber;	// Reservd3
	INT16U SerialStatus;	// Reservd4
	INT16U MaximumCellVoltageNo;
	INT16U MaximumCellVoltage;
	INT16U MinimumCellVoltageNo;
	INT16U MinimumCellVoltage;
	INT16U MaximumCellTemperatureNo;
	INT16S MaximumCellTemperature;
	INT16U MinimumCellTemperatureNo;
	INT16S MinimumCellTemperature;
	INT16U CellVoltage01;
	INT16U CellVoltage02;
	INT16U CellVoltage03;
	INT16U CellVoltage04;
	INT16U CellVoltage05;
	INT16U CellVoltage06;
	INT16U CellVoltage07;
	INT16U CellVoltage08;
	INT16U CellVoltage09;
	INT16U CellVoltage10;
	INT16U CellVoltage11;
	INT16U CellVoltage12;
	INT16U CellVoltage13;
	INT16U CellVoltage14;
	INT16U CellVoltage15;
	INT16U CellVoltage16;
	INT16S TemperatureSensor01;
	INT16S TemperatureSensor02;
	INT16S TemperatureSensor03;
	INT16S TemperatureSensor04;
	INT16S TemperatureSensor05;
	INT16S TemperatureSensor06;
	INT16S TemperatureSensor07;
	INT16S TemperatureSensor08;
	INT16U Reservd5;
	INT16U Reservd6;
	INT16U Reservd7;
	INT16U Reservd8;
	INT16U Reservd9;
	INT16U Reservd10;
	INT16U Reservd11;
	INT16U Reservd12;
};

struct	BMS2_DATA_FEILD
{
	INT16U SettingDataSn;
	INT16U BmsMode;
	INT16U BmsFlag;
	INT16U BmsFaultCode;
	INT16U BmsFaultFlag;
	INT16U BmsAlarmFlag;
	INT16U BattVolt;
	INT16S BattCurr;
	INT16U Reservd1;
	INT16U Reservd2;
	INT16U AmbTemp;
	INT16U BmsSoc;
	INT16U BmsSoh;
	INT16U BmsCycleCnt;
	INT16U BmsChgAhLo;
	INT16U BmsChgAhHi;
	INT16U BmsChgWhLo;
	INT16U BmsChgWhHi;
	INT16U BmsChgTimeLo;
	INT16U BmsChgTimeHi;
	INT16U BmsDcgAhLo;
	INT16U BmsDcgAhHi;
	INT16U BmsDcgWhLo;
	INT16U BmsDcgWhHi;
	INT16U BmsDcgTimeLo;
	INT16U BmsDcgTimeHi;
	INT16U BmsRunTimeLo;
	INT16U BmsRunTimeHi;
	INT16U ChargeVolt;
	INT16U CutoffVolt;
	INT16U MaxChargeCurr;
	INT16U MaxDischargeCurr;
	INT16U CellVoltMax;
	INT16U CellVoltMaxNo;
	INT16U CellVoltMin;
	INT16U CellVoltMinNo;
	INT16S CellTempMax;
	INT16U CellTempMaxNo;
	INT16S CellTempMin;
	INT16U CellTempMinNo;
	INT16U Reservd3;
	INT16U Reservd4;
	INT16U CellVolt01;
	INT16U CellVolt02;
	INT16U CellVolt03;
	INT16U CellVolt04;
	INT16U CellVolt05;
	INT16U CellVolt06;
	INT16U CellVolt07;
	INT16U CellVolt08;
	INT16U CellVolt09;
	INT16U CellVolt10;
	INT16U CellVolt11;
	INT16U CellVolt12;
	INT16U CellVolt13;
	INT16U CellVolt14;
	INT16U CellVolt15;
	INT16U CellVolt16;
	INT16S CellTemp01;
	INT16S CellTemp02;
	INT16S CellTemp03;
	INT16S CellTemp04;
	INT16S CellTemp05;
	INT16S CellTemp06;
	INT16S CellTemp07;
	INT16S CellTemp08;
	INT16S CellTemp09;
	INT16S CellTemp10;
	INT16S CellTemp11;
	INT16S CellTemp12;
	INT16S CellTemp13;
	INT16S CellTemp14;
	INT16S CellTemp15;
	INT16S CellTemp16;
};

struct	BMS_INFORMATION_FEILD
{
	INT16U wType;
	INT16U wSubType;
	INT16U wCommProVer;
	INT16U wCommInfo;
	INT16U wSerial1;
	INT16U wSerial2;
	INT16U wSerial3;
	INT16U wSerial4;
	INT16U wSerial5;
	INT16U wSNLen;
	INT16U wDispSwVer;
	INT16U wMCU1SwVer;
	INT16U wMCU2SwVer;
	INT16U wDispHwVer;
	INT16U wCtrlHwVer;
	INT16U wPowerHwVer;
};

#define	cBMS_DATA_FEILD_LEN			sizeof(struct BMS_DATA_FEILD)/2			// 48words
#define	cBMS2_DATA_FEILD_LEN		sizeof(struct BMS2_DATA_FEILD)/2			// 48words
#define	cBMS_INFORMATION_FEILD_LEN	sizeof(struct BMS_INFORMATION_FEILD)/2	// 

union BMS_DATA_TABLE
{
	struct	BMS_DATA_FEILD	Field;
	INT16U	Table[cBMS_DATA_FEILD_LEN];
};

union BMS2_DATA_TABLE
{
	struct	BMS2_DATA_FEILD	Field;
	INT16U	Table[cBMS2_DATA_FEILD_LEN];
};

union BMS_INFORMATION_TABLE
{
	struct	BMS_INFORMATION_FEILD	Field;
	INT16U	Table[cBMS_INFORMATION_FEILD_LEN];
};

//struct	Growatt_BMS_DATA_FEILD
//{
//    INT16U Reserved1;
//	INT16U FW_VER;
//	INT16U Gauge_Version;
//	INT16U FR_Version_Low;
//	INT16U FR_Version_High;
//	INT16U Date_Time1;
//	INT16U Date_Time2;
//	INT16U Date_Time3;
//	INT16S Date_Time4;
//	INT16U Bar_Code12;
//	INT16U Bar_Code34;
//	INT16U Bar_Code56;
//	INT16U Bar_Code78;
//	INT16U Company_Code_Low;
//	INT16U Company_Code_High;
//	INT16U Using_Cap;
//    
//	INT16U Gauge_IC_current;//10mA
//    union BMS_time
//    {
//        struct BMS_time_BIT_FEILD
//        {
//            INT16U secod:6;
//            INT16U minute:6;
//            INT16U hour:5;
//            INT16U day:5;
//            INT16U Month:4;
//            INT16U Year:6;
//        } bits;
//        INT32U dataTime;
//    }DataTime;
//    
//	union BMS_status
//    {
//        struct BMS_status_BIT_FEILD
//        {
//            INT16U status:2;
//            INT16U Err_Bit_falg:1;
//            INT16U cell_banlace_status:1;
//            INT16U sleep_status:1;
//            INT16U op_dischg_status:1;
//            INT16U op_chg_status:1;
//            INT16U Battery_terminal_status:1;
//            INT16U Master_box_Operation_Mode:2;
//            INT16U SP_status:2;
//            INT16U Force_chg_status:1;
//            INT16U Reserved:3;
//        } bits;
//        INT16U PackStatus;
//    }BMS_STATUS;
//    
//	union BMS_err
//    {
//        struct BMS_err_BIT_FEILD
//        {
//            INT16U over_dischg_curr:1;
//            INT16U short_circuit:1;
//            INT16U over_volt:1;
//            INT16U under_volt:1;
//            
//            INT16U over_temp_dischg:1;
//            INT16U over_temp_chg:1;
//            INT16U under_temp_dischg:1;
//            INT16U under_temp_chg:1;
//            
//            INT16U soft_start_fail:1;
//            INT16U Permanent_Fault:1;
//            INT16U Delta_V_Fail:1;
//            INT16U over_current_charge:1;

//            INT16U over_temp_mos:1;
//            INT16U over_temp_enviorment:1;
//            INT16U under_temp_enviorment:1;
//            INT16U Reserved:1;
//        } bits;
//        INT16U Err_word;
//    }BMS_Err;
//    
//	INT16U SOC;//%
//	INT16U Voltage;//10mV
//	INT16U Current;//10mA
//	INT16U Temperature;//℃
//	INT16U ChargeCurrentLimit;
//	INT16U Gauge_RM;//10mAH
//	INT16U Gauge_FCC;//10mAH

//    union BMS_ver
//    {
//        struct BMS_ver_BIT_FEILD
//        {
//            INT16U hardware_ver:8;
//            INT16U software_ver:8;
//        } bits;
//        INT16U Ver_word;
//    }BMS_Ver;
//    
//	INT16U Delta;
//	INT16U Cycle_Count;
//	INT16U RSVD_For_Master_Box;
//	INT16U SOH;
//	INT16U CV_Voltage;//10mV
//	INT16U Warning;
//	INT16U MaxDischargeCurr;
//    INT16U Extended_Error;
//    INT16U CellVoltMax;
//    INT16U CellVoltMin;
//    INT16U CellVoltMaxNo;
//	INT16U CellVoltMinNo;
//    INT16U Cell_Series;
//    INT16U wReserved1[7];
//    
//	INT16U wReserved2[0x40];
//	INT16U CellVolt01;
//	INT16U CellVolt02;
//	INT16U CellVolt03;
//	INT16U CellVolt04;
//	INT16U CellVolt05;
//	INT16U CellVolt06;
//	INT16U CellVolt07;
//	INT16U CellVolt08;
//	INT16U CellVolt09;
//	INT16U CellVolt10;
//	INT16U CellVolt11;
//	INT16U CellVolt12;
//	INT16U CellVolt13;
//	INT16U CellVolt14;
//	INT16U CellVolt15;
//	INT16U CellVolt16;
//};
//#define	cBMS_Growatt_FEILD_LEN	sizeof(struct Growatt_BMS_DATA_FEILD)/2
//union BMS_Growatt_TABLE
//{
//	struct	Growatt_BMS_DATA_FEILD	Field;
//	INT16U	Table[cBMS_Growatt_FEILD_LEN];
//};

struct	Voltronic_BMS_DATA_FEILD
{
    INT16U Protocol_type;
	INT16U Protocol_version;
	INT16U BMS_Fw_Version_High;
	INT16U BMS_Fw_Version_Low;
    INT16U BMS_Hw_Version_High;
	INT16U BMS_Hw_Version_Low;
    
    INT16U wReserved1[10];
    
    INT16U Cell_Num;
    INT16U CellVolt[20];//0.1V

    INT16U Temp_Sensor_Num;
    INT16U Sensor_Temp[10];//0.1K

    INT16U Charge_Current;//100mA
    INT16U Discharge_Current;//100mA
    INT16U Voltage;//100mV
    INT16U SOC;//%
    INT16U Total_Capacity_H;//mAH
		INT16U Total_Capacity_L;
		
    INT16U wReserved2[10];

    INT16U Cell_Num_Warn;
    INT16U Cell_1_2_State;
    INT16U Cell_3_4_State;
    INT16U Cell_5_6_State;
    INT16U Cell_7_8_State;
    INT16U Cell_9_10_State;
    INT16U Cell_11_12_State;
    INT16U Cell_13_14_State;
    INT16U Cell_15_16_State;
    INT16U Cell_17_18_State;
    INT16U Cell_19_20_State;
		
		INT16U wReserved3[5];
    
    INT16U Temp_Sensor_Num_Warn;
    INT16U Temp_1_2_State;
    INT16U Temp_3_4_State;
    INT16U Temp_5_6_State;
    INT16U Temp_7_8_State;
    INT16U Temp_9_10_State;
		
		INT16U wReserved4[10];

    INT16U Module_Charge_Volt_State;
    INT16U Module_DisCharge_Volt_State;
    INT16U Cell_Charge_Volt_State;
    INT16U Cell_DisCharge_Volt_State;
    INT16U Module_Charge_Curr_State;
    INT16U Module_DisCharge_Curr_State;
    INT16U Module_Charge_Temp_State;
    INT16U Module_DisCharge_Temp_State;
    INT16U Cell_Charge_Temp_State;
    INT16U Cell_DisCharge_Temp_State;

    INT16U wReserved5[6];

    INT16U Charge_Voltage_Limit;//100mV
    INT16U Discharge_Voltage_Limit;//100mV
    INT16U Charge_Current_Limit;//100mA
    INT16U DisCharge_Current_Limit;//100mA

    union BMS_Dischg_Sts
    {
        struct BMS_Dischg_Sts_BIT_FEILD
        {
            INT16U wReserved_Bit            :3;
            INT16U Full_Chg_Request         :1;
            
            INT16U Chg_immediately2_Request :1;
            INT16U Chg_immediately_Request  :1;
            INT16U Discharge_Ena            :1;
            INT16U Charge_Ena               :1;

            INT16U wReserved_Bit2            :8;
        } bits;
        INT16U Dischg_Sts_word;
    }BMS_Discharge_Status;
};
#define	cBMS_Voltronic_FEILD_LEN	sizeof(struct Voltronic_BMS_DATA_FEILD)/2
union BMS_Voltronic_TABLE
{
	struct	Voltronic_BMS_DATA_FEILD	Field;
	INT16U	Table[cBMS_Voltronic_FEILD_LEN];
};

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
BMS_PROCESS	void	sBmsInitial(void);
BMS_PROCESS	void	sBmsParaUpdate(void);
BMS_PROCESS	INT8U	sBmsBattTypeChk(INT16U data);
BMS_PROCESS	void	sBmsInfoUpdate(void);
BMS_PROCESS	void	sbBmsIdentifyInitial(void);
BMS_PROCESS	INT8U	sbBmsIdentify(INT8U bFilter);
BMS_PROCESS	void	sBmsCharge5AChk(INT16S wCurr, INT16U wFilter1, INT16U wFilter2);
BMS_PROCESS	void	sBmsCurrentStatusChk(INT16S wCurr, INT16U wFilter1, INT16U wFilter2);
BMS_PROCESS	void	sBmsFaultUpdate(void);
BMS_PROCESS	void	sBmsAlarmUpdate(void);
BMS_PROCESS	void	sBmsNoticeUpdate(void);
BMS_PROCESS	void	sBmsDataStatistic(void);
BMS_PROCESS	void	sBmsTimeStore(INT16U wFilter);
BMS_PROCESS	void	sBmsChgToOffChk(INT16U wFilter1, INT16U wFilter2);
BMS_PROCESS	void	sBmsIdleToOffChk(INT16U wFilter1, INT16U wFilter2);
BMS_PROCESS	void	sBmsLossToOffChk(INT16U wFilter);
BMS_PROCESS	void	sBmsSoftStartChk(INT16U wFilter, INT16U wTimeOut);
BMS_PROCESS	void	sBmsStatusUpdate(void);
BMS_PROCESS	void	sBmsDataUpdate(void);
BMS_PROCESS	void	sBmsInformationDataUpdate(void);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
BMS_PROCESS	INT8U	sbGetBmsParallelShutdown(void);
BMS_PROCESS	INT32U	GetSystemTime(void);
BMS_PROCESS	INT16U	swGetBmsUserParaCnt(void);
BMS_PROCESS	INT16U	swGetBmsCellType(void);
BMS_PROCESS	INT16U	swGetBmsTempType(void);
BMS_PROCESS	INT16U	swGetBmsNominalVolt(void);
BMS_PROCESS	INT16U	swGetBmsNominalAH(void);
BMS_PROCESS	INT16U	swGetBmsRatedVolt(void);
BMS_PROCESS	INT16U	swGetBmsRatedAH(void);
BMS_PROCESS	INT16U	swGetBmsCellVoltNum(void);
BMS_PROCESS	INT16U	swGetBmsCellTempNum(void);
BMS_PROCESS	INT16U	swGetBmsBalanceFunc(void);
BMS_PROCESS	INT16U	swGetBmsLedLcdMode(void);
BMS_PROCESS	INT16U	swGetBmsCurrSampType(void);
BMS_PROCESS	INT16U	swGetBmsCurrAlarmBack(void);
BMS_PROCESS	INT16U	swGetBmsCurrAlarm(void);
BMS_PROCESS	INT16U	swGetBmsCurrHigh1(void);
BMS_PROCESS	INT16U	swGetBmsCurrHigh2(void);
BMS_PROCESS	INT8U	sbGetBmsChargeOver5A(void);
BMS_PROCESS	INT8U	sbGetBmsCurrentStatus(void);
BMS_PROCESS	INT8U	sbGetBmsPirated(void);
BMS_PROCESS	INT8U	sbGetBmsSoftStartFlag(void);
BMS_PROCESS	INT8U	sbGetBmsFaultCode(void);
BMS_PROCESS	INT16U	swGetBmsFaultFlag(void);
BMS_PROCESS	INT16U	swGetBmsAlarmFlag(void);
BMS_PROCESS	INT16U	swGetBmsNoticeFlag(void);
BMS_PROCESS	INT32U	sdwGetBmsRunSec(void);
BMS_PROCESS	INT16U	swGetBmsStatusOld(void);
BMS_PROCESS	INT16U	swGetRecChargeVolt(void);
BMS_PROCESS	INT16U  swGetRecCutoffVolt(void);
BMS_PROCESS	INT16U  swGetRecChargeCurr(void);
BMS_PROCESS	INT16U  swGetRecDischargeCurr(void);
BMS_PROCESS	INT16U  swGetBmsChargeEnable(void);
BMS_PROCESS	INT16U  swGetBmsDischargeEnable(void);
BMS_PROCESS	INT16U  swGetBmsChargeImmediately1(void);
BMS_PROCESS	INT16U  swGetBmsChargeImmediately2(void);
BMS_PROCESS	INT16U  swGetBmsFullCharge(void);
BMS_PROCESS	INT16U	swGetBmsDataLocal(INT16U wAddrOffset);
BMS_PROCESS INT16U	swGetBms2DataLocal(INT16U wAddrOffset);
BMS_PROCESS	INT16U	swGetBmsData(INT8U bNo, INT16U wAddrOffset);
BMS_PROCESS	INT16U	swGetBmsInformationData(INT16U wAddrOffset);
BMS_PROCESS	INT16U	swGetBmsDataChargeVoltageLimit(INT8U bNo);
BMS_PROCESS	INT16U  swGetBmsDataDischargeVoltageLimit(INT8U bNo);
BMS_PROCESS	INT16U  swGetBmsDataChargeCurrentLimit(INT8U bNo);
BMS_PROCESS	INT16U  swGetBmsDataDischargeCurrentLimit(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataBmsStatusHigh(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataBmsStatusLow(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataFaultFlagHigh(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataFaultFlagLow(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataAlarmFlagHigh(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataAlarmFlagLow(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataNoticeFlagHigh(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataNoticeFlagLow(INT8U bNo);
BMS_PROCESS	INT16S	swGetBmsDataPackCurrent(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataPackVoltage(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataSOC(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataSOH(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataPackCapacityHigh(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataPackCapacityLow(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataMaximumCellVoltageNo(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataMaximumCellVoltage(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataMinimumCellVoltageNo(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataMinimumCellVoltage(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataMaximumCellTemperatureNo(INT8U bNo);
BMS_PROCESS	INT16S	swGetBmsDataMaximumCellTemperature(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataMinimumCellTemperatureNo(INT8U bNo);
BMS_PROCESS	INT16S	swGetBmsDataMinimumCellTemperature(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataParallelNumber(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataParallelStatus(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataSerialNumber(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataSerialStatus(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataCellNumber(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataTemperatureSensorNumber(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataCellVoltage01(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataCellVoltage02(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataCellVoltage03(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataCellVoltage03(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataCellVoltage04(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataCellVoltage05(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataCellVoltage06(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataCellVoltage07(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataCellVoltage08(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataCellVoltage09(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataCellVoltage10(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataCellVoltage11(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataCellVoltage12(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataCellVoltage13(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataCellVoltage14(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataCellVoltage15(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataCellVoltage16(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataTemperatureSensor01(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataTemperatureSensor02(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataTemperatureSensor03(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataTemperatureSensor04(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataTemperatureSensor05(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataTemperatureSensor06(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataTemperatureSensor07(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataTemperatureSensor08(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataReservd5(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataReservd6(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataReservd7(INT8U bNo);
BMS_PROCESS	INT16U	swGetBmsDataReservd8(INT8U bNo);
BMS_PROCESS INT16U	swGetChargeImmediately1(  void);
BMS_PROCESS INT16U	swGetChargeImmediately2(  void);

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
BMS_PROCESS void	sSetBmsLocalPointer(INT8U bAddr);
BMS_PROCESS void	sSetBmsParallelShutdown(INT8U bType);
BMS_PROCESS void	sSetBmsUserParaCnt(INT16U wCnt);
BMS_PROCESS	void	sSetBmsCellVoltNum(INT16U wNum);
BMS_PROCESS	void	sSetBmsCellTempNum(INT16U wNum);
BMS_PROCESS	void	sSetBmsBalanceFunc(INT16U wStatus);
BMS_PROCESS	void	sSetBmsLedLcdMode(INT16U wMode);
BMS_PROCESS	void	sSetBmsCurrSampType(INT16U wStatus);
BMS_PROCESS	void	sSetBmsManagerPirated(INT8U bStatus);
BMS_PROCESS	void	sSetOpSoftFinishedFlag(INT8U bFlag);
BMS_PROCESS	void	sRstBmsTimeStoreMin(void);
BMS_PROCESS	void	sSetBmsFaultCodeIsr(INT8U bCode);
BMS_PROCESS	void	sClrBmsFaultCode(INT8U bCode);
BMS_PROCESS	void	sSetBmsFaultFlag(INT16U wFlag);
BMS_PROCESS	void	sSetBmsFaultFlagIsr(INT16U wFlag);
BMS_PROCESS	void	sClrBmsFaultFlag(INT16U wFlag);
BMS_PROCESS	void	sSetBmsNoticeFlag(INT16U wFlag);
BMS_PROCESS	void	sSetBmsNoticeFlagIsr(INT16U wFlag);
BMS_PROCESS	void	sClrBmsNoticeFlag(INT16U wFlag);
BMS_PROCESS	void	sSetBmsAlarmFlag(INT16U wFlag);
BMS_PROCESS	void	sSetBmsAlarmFlagIsr(INT16U wFlag);
BMS_PROCESS	void	sClrBmsAlarmFlag(INT16U wFlag);
BMS_PROCESS	void	sSetBmsCurrentStatus(INT8U bStatus);
BMS_PROCESS	void	sRstCommLossCount(void);
BMS_PROCESS	void	sSetBmsFaultCode(INT8U bCode);
BMS_PROCESS	void	sSetSoftStartMode(INT8U bMode);
BMS_PROCESS	void	sSetBmsDataChargeVoltageLimit(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataDischargeVoltageLimit(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataChargeCurrentLimit(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataDischargeCurrentLimit(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataBmsStatusHigh(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataBmsStatusLow(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataFaultFlagHigh(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataFaultFlagLow(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataAlarmFlagHigh(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataAlarmFlagLow(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataNoticeFlagHigh(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataNoticeFlagLow(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataPackCurrent(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataPackVoltage(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataSOC(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataSOH(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataPackCapacityHigh(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataPackCapacityLow(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataMaximumCellVoltageNo(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataMaximumCellVoltage(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataMinimumCellVoltageNo(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataMinimumCellVoltage(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataMaximumCellTemperatureNo(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataMaximumCellTemperature(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataMinimumCellTemperatureNo(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataMinimumCellTemperature(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataParallelNumber(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataParallelStatus(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataSerialNumber(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataSerialStatus(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataCellNumber(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataTemperatureSensorNumber(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataCellVoltage01(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataCellVoltage02(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataCellVoltage03(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataCellVoltage04(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataCellVoltage05(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataCellVoltage06(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataCellVoltage07(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataCellVoltage08(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataCellVoltage09(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataCellVoltage10(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataCellVoltage11(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataCellVoltage12(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataCellVoltage13(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataCellVoltage14(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataCellVoltage15(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataCellVoltage16(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataTemperatureSensor01(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataTemperatureSensor02(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataTemperatureSensor03(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataTemperatureSensor04(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataTemperatureSensor05(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataTemperatureSensor06(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataTemperatureSensor07(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataTemperatureSensor08(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataReservd5(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataReservd6(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataReservd7(INT8U bNo, INT16U wData);
BMS_PROCESS	void	sSetBmsDataReservd8(INT8U bNo, INT16U wData);
//BMS_PROCESS INT16U	swGetGrowatt_BmsDataLocal(INT16U wAddrOffset);
//BMS_PROCESS void	sBmsGrowatt_Data_Update(void);
BMS_PROCESS INT16U	swGetVoltronic_BmsDataLocal(INT16U wAddrOffset);
BMS_PROCESS void	sBmsVoltronic_Data_Update(void);

#endif	// __BMS_PROCESS_H__

