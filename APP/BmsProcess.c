/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		BmsProcess.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define	__BMS_PROCESS_C__

/********************************************************************************
* Include head files                                                            *
********************************************************************************/
#include	"BmsProcess.h"
#include	"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define	cHAVE_LOAD_CURR					100
#define	cHAVE_LOAD_CURR_BACK			50

#define	cBMS_TIME_STROE_MIN				1440		// 24H

/********************************************************************************
* Constants																		*
********************************************************************************/


/********************************************************************************
* External variables															*
********************************************************************************/
extern INT32U system_time_ms;

/********************************************************************************
* External routine prototypes													*
********************************************************************************/


/********************************************************************************
* Input variables																*
********************************************************************************/
INT16U	wCommLossCount;
INT16U	wCommLossCountLimit;

/********************************************************************************
* Output variables																*
********************************************************************************/
union BMS_DATA_TABLE strBmsData[BMS_PARALLEL_NUM_MAX];
union BMS_DATA_TABLE *pBmsData[BMS_PARALLEL_NUM_MAX];
union BMS_DATA_TABLE *pBmsDataLocal;
union BMS2_DATA_TABLE strBms2Data;
union BMS2_DATA_TABLE *pBms2DataLocal;
union BMS_INFORMATION_TABLE strBmsInformationData;
//union BMS_Growatt_TABLE *pBmsGrowatt;
//union BMS_Growatt_TABLE strBmsGrowatt;
union BMS_Voltronic_TABLE *pBmsVoltronic;
union BMS_Voltronic_TABLE strBmsVoltronic;

INT8U	bBmsFaultCode;
INT16U	wBmsFaultFlag;
INT16U	wBmsAlarmFlag;
INT16U	wBmsNoticeFlag;
INT16U	wBmsTimeStoreMin;		// 定时保存倒数分钟
INT32U	dwBmsChgAhAcc;
INT32U	dwBmsDcgAhAcc;
INT32U	dwBmsChgWhAcc;
INT32U	dwBmsDcgWhAcc;
INT8U	bBmsChgTimeAcc;
INT8U	bBmsDcgTimeAcc;
INT8U	bBmsRunTimeAcc;
INT32U	dwBmsRunSec;
INT8U	bSoftStartMode;
union BMS_GLOBAL_STATUS_TABLE
{
	struct BMS_GLOBAL_STATUS_BIT_FEILD
	{
		// BIT0-3
		INT16U IsPirated:1;
		INT16U rsvd11:1;
		INT16U bSoftStart:1;
		INT16U rsvd12:1;
		
		// BIT4-7
		INT16U CurrStatus:2;
		INT16U Charge5A:1;
		INT16U rsvd2:1;
		
		// BIT8-11
		INT16U ParallelShutdown:1;
		INT16U rsvd3:3;
		
		// BIT12-15
		INT16U rsvd4:4;
	} bits;
	INT16U data;
} fBmsGlobal;
union BMS_STATUS_TABLE fBmsStatus;
union BMS_STATUS_OLD_TABLE fBmsStatusOld;

BMS_InitTypeDef strBmsInfo;

/********************************************************************************
* Internal variables															*
********************************************************************************/
INT8U	bModelRsvd00;
INT8U	bModelRsvd01;
INT8U	bModelRsvd02;
INT8U	bModelRsvd03;
INT8U	bModelRsvd04;
INT8U	bModelRsvd05;
INT8U	bModelRsvd06;
INT8U	bModelRsvd07;
INT8U	bModelRsvd08;
INT8U	bModelRsvd09;
INT8U	bModelRsvd10;
INT16U	wRecChargeVolt;		// 0.01V
INT16U	wRecCutoffVolt;		// 0.01V
INT16U	wRecChargeCurr;		// 0.1A
INT16U	wRecDischargeCurr;	// 0.1A
INT16U	wUserParaCnt;

INT16U	wRecChargeCurrRef;		// 0.1A
INT16U	wRecDischargeCurrRef;	// 0.1A

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
void	sBmsDataLocalUpdate(void);
void	sBms2DataLocalUpdate(void);
INT16U	swBmsChargeDerated(void);
INT16U	swBmsDischargeDerated(void);

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
/**
 * @brief 初始化BMS
 */
void	sBmsInitial(void)
{
	INT8U i;
	
	// 清零BMS全局状态和BMS状态
	fBmsGlobal.data = 0;
	fBmsStatus.data = 0;
	fBmsStatusOld.data = 0;
	
	// 重置BMS时间存储分钟
	sRstBmsTimeStoreMin();
	
	// 清零BMS充电和放电累积电量及能量
	dwBmsChgAhAcc = 0;
	dwBmsDcgAhAcc = 0;
	dwBmsChgWhAcc = 0;
	dwBmsDcgWhAcc = 0;
	bBmsChgTimeAcc = 0;
	bBmsDcgTimeAcc = 0;
	bBmsRunTimeAcc = 0;
	
	// 清零BMS运行秒数
	dwBmsRunSec = 0;
	
	// 设置通信丢失计数限制为最大值
	wCommLossCountLimit = 0xffff;
	wCommLossCount = 0xffff;
	
	// 设置BMS电池电压和温度数量为0
	sSetBmsCellVoltNum(0);
	sSetBmsCellTempNum(0);

	// 设置BMS均衡功能为禁用
	sSetBmsBalanceFunc(cBMS_BALANCE_DISABLE);
	// 设置BMSLED\LCD模式
	sSetBmsLedLcdMode(cBMS_LED_MODE);
	// 设置电流采样类型为10毫秒采样
	sSetBmsCurrSampType(cBMS_CURR_SAMPLE_10);
	
	// 初始化BMS本地数据指针
	pBmsDataLocal = &strBmsData[sbGetCanAddress()];
	// 初始化BMS数据指针指针数组
	for(i = 0; i < BMS_PARALLEL_NUM_MAX; i++)
	{
		pBmsData[i] = &strBmsData[i];
	}
	//初始化BMS2本地数据指针
	pBms2DataLocal = &strBms2Data;
//	// 初始化Growatt BMS数据指针
//    pBmsGrowatt = &strBmsGrowatt;
	// 初始化Voltronic BMS数据指针
	pBmsVoltronic = &strBmsVoltronic;
}

void	sBmsParaUpdate(void)
{
	OS_CPU_SR  cpu_sr;
	// 进入临界区
	OS_ENTER_CRITICAL();
	// 更新电池参数
	sBatteryParaUpdate();
	// 更新保护参数
	sProtectionParaUpdate();
	// 更新SOC参数
	sSoxParaUpdate();
	// 更新均衡参数
	sBalanceParaUpdate();
	// 推出临界区
	OS_EXIT_CRITICAL();
}

// 电芯数目通过硬件识别
// 电池类型,额定电压,额定容量,实际电压,实际容量,温感数等通过软件设置
INT8U	sBmsBattTypeChk(INT16U data)
{
	INT8U val = 0;
	
	if(swGetBmsCellVoltNum() == cBMS_CELL_VOLT_8S)
	{
		if((data >= eBATT_TYPE_24C5_MIN) && (data < eBATT_TYPE_24C5_MAX))
		{
			val = 1;
		}
		else if((data >= eBATT_TYPE_24C10_MIN) && (data < eBATT_TYPE_24C10_MAX))
		{
			val = 1;
		}
	}
	else if(swGetBmsCellVoltNum() == cBMS_CELL_VOLT_16S)
	{
		if((data >= eBATT_TYPE_48C5_MIN) && (data < eBATT_TYPE_48C5_MAX))
		{
			val = 1;
		}
		else if((data >= eBATT_TYPE_48C10_MIN) && (data < eBATT_TYPE_48C10_MAX))
		{
			val = 1;
		}
	}
	
	return val;
}

void	sBmsInfoUpdate(void)
{
	switch(swGetEepromBattType())
	{
		// BA48V/0.5C=41XX
		case eBATT_TYPE_48050_C5:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_48V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_050AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_51V2;
			strBmsInfo.wBattRatedAH = cBMS_CAP_050AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_20A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_25A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_30A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_50A;
			
		break;
		case eBATT_TYPE_48100_C5:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_48V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_100AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_51V2;
			strBmsInfo.wBattRatedAH = cBMS_CAP_100AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_40A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_50A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_55A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_100A;
			
		break;
		case eBATT_TYPE_48150_C5:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_48V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_150AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_51V2;
			strBmsInfo.wBattRatedAH = cBMS_CAP_150AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_60A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_75A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_80A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_150A;
			
		break;
		case eBATT_TYPE_48200_C5:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_48V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_200AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_51V2;
			strBmsInfo.wBattRatedAH = cBMS_CAP_200AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_80A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_100A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_105A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_200A;
			
		break;
		case eBATT_TYPE_48250_C5:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_48V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_250AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_51V2;
			strBmsInfo.wBattRatedAH = cBMS_CAP_250AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_100A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_125A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_130A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_250A;
			
		break;
		case eBATT_TYPE_48300_C5:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_48V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_300AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_51V2;
			strBmsInfo.wBattRatedAH = cBMS_CAP_300AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_120A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_150A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_155A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_300A;
			
		break;
		case eBATT_TYPE_48350_C5:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_48V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_350AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_51V2;
			strBmsInfo.wBattRatedAH = cBMS_CAP_350AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_140A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_175A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_180A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_300A;
			
		break;
		
		// BF48V/1C=43XX
		case eBATT_TYPE_48050_C10:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_48V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_050AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_51V2;
			strBmsInfo.wBattRatedAH = cBMS_CAP_050AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_40A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_50A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_55A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_100A;
			
		break;
		case eBATT_TYPE_48100_C10:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_48V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_100AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_51V2;
			strBmsInfo.wBattRatedAH = cBMS_CAP_100AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_80A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_100A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_105A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_200A;
			
		break;
		case eBATT_TYPE_48150_C10:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_48V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_150AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_51V2;
			strBmsInfo.wBattRatedAH = cBMS_CAP_150AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_120A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_150A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_155A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_300A;
			
		break;
		case eBATT_TYPE_48200_C10:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_48V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_200AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_51V2;
			strBmsInfo.wBattRatedAH = cBMS_CAP_200AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_160A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_200A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_210A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_300A;
			
		break;
		case eBATT_TYPE_48250_C10:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_48V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_250AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_51V2;
			strBmsInfo.wBattRatedAH = cBMS_CAP_250AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_160A;//cBMS_CURR_200A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_200A;//cBMS_CURR_250A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_210A;//cBMS_CURR_260A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_300A;//cBMS_CURR_375A;
			
		break;
		case eBATT_TYPE_48300_C10:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_48V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_300AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_51V2;
			strBmsInfo.wBattRatedAH = cBMS_CAP_300AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_160A;//cBMS_CURR_240A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_200A;//cBMS_CURR_300A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_210A;//cBMS_CURR_310A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_300A;//cBMS_CURR_450A;
			
		break;
		case eBATT_TYPE_48350_C10:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_48V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_350AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_51V2;
			strBmsInfo.wBattRatedAH = cBMS_CAP_350AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_160A;//cBMS_CURR_240A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_200A;//cBMS_CURR_300A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_210A;//cBMS_CURR_310A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_300A;//cBMS_CURR_450A;
			
		break;

        case eBATT_TYPE_48400_C10:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_48V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_400AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_51V2;
			strBmsInfo.wBattRatedAH = cBMS_CAP_400AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_160A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_200A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_210A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_300A;
			
		break;
		
		// BA24V/0.5C=21XX
		case eBATT_TYPE_24050_C5:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_24V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_050AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_25V6;
			strBmsInfo.wBattRatedAH = cBMS_CAP_050AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_20A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_25A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_30A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_50A;
			
		break;
		case eBATT_TYPE_24100_C5:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_24V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_100AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_25V6;
			strBmsInfo.wBattRatedAH = cBMS_CAP_100AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_40A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_50A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_55A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_100A;
			
		break;
		case eBATT_TYPE_24150_C5:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_24V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_150AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_25V6;
			strBmsInfo.wBattRatedAH = cBMS_CAP_150AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_60A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_75A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_80A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_150A;
			
		break;
		case eBATT_TYPE_24200_C5:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_24V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_200AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_25V6;
			strBmsInfo.wBattRatedAH = cBMS_CAP_200AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_80A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_100A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_105A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_200A;
			
		break;
		case eBATT_TYPE_24250_C5:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_24V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_250AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_25V6;
			strBmsInfo.wBattRatedAH = cBMS_CAP_250AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_100A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_125A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_130A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_250A;
			
		break;
		case eBATT_TYPE_24300_C5:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_24V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_300AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_25V6;
			strBmsInfo.wBattRatedAH = cBMS_CAP_300AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_120A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_150A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_155A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_300A;
			
		break;
		case eBATT_TYPE_24350_C5:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_24V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_350AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_25V6;
			strBmsInfo.wBattRatedAH = cBMS_CAP_350AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_140A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_175A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_180A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_300A;
			
		break;
		
		// BF24V/1C=23XX
		case eBATT_TYPE_24050_C10:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_24V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_050AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_25V6;
			strBmsInfo.wBattRatedAH = cBMS_CAP_050AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_40A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_50A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_55A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_100A;
			
		break;
		case eBATT_TYPE_24100_C10:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_24V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_100AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_25V6;
			strBmsInfo.wBattRatedAH = cBMS_CAP_100AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_80A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_100A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_105A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_200A;
			
		break;
		case eBATT_TYPE_24150_C10:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_24V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_150AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_25V6;
			strBmsInfo.wBattRatedAH = cBMS_CAP_150AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_120A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_150A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_155A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_300A;
			
		break;
		case eBATT_TYPE_24200_C10:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_24V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_200AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_25V6;
			strBmsInfo.wBattRatedAH = cBMS_CAP_200AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_160A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_200A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_210A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_300A;
			
		break;
		case eBATT_TYPE_24250_C10:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_24V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_250AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_25V6;
			strBmsInfo.wBattRatedAH = cBMS_CAP_250AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_160A;//cBMS_CURR_200A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_200A;//cBMS_CURR_250A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_210A;//cBMS_CURR_260A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_300A;//cBMS_CURR_375A;
			
		break;
		case eBATT_TYPE_24300_C10:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_24V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_300AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_25V6;
			strBmsInfo.wBattRatedAH = cBMS_CAP_300AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_160A;//cBMS_CURR_240A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_200A;//cBMS_CURR_300A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_210A;//cBMS_CURR_310A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_300A;//cBMS_CURR_450A;
			
		break;
		case eBATT_TYPE_24350_C10:
			strBmsInfo.wCellType = cBMS_CELL_LFP;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = cBMS_VOLT_24V;
			strBmsInfo.wBattNominalAH = cBMS_CAP_350AH;
			strBmsInfo.wBattRatedVolt = cBMS_VOLT_25V6;
			strBmsInfo.wBattRatedAH = cBMS_CAP_350AH;
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_4S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_160A;//cBMS_CURR_240A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_200A;//cBMS_CURR_300A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_210A;//cBMS_CURR_310A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_300A;//cBMS_CURR_450A;
			
		break;
		
		default:
			strBmsInfo.wCellType = cBMS_CELL_USE;
			strBmsInfo.wTempType = cBMS_TEMP_153F3950;
			strBmsInfo.wBattNominalVolt = 0;
			strBmsInfo.wBattNominalAH = 0;
			strBmsInfo.wBattRatedVolt = 0;
			strBmsInfo.wBattRatedAH = cBMS_CAP_050AH;	// 与EEPROM的SOH默认值保持一致
			strBmsInfo.wCellTempNum = cBMS_CELL_TEMP_1S;
			
			strBmsInfo.wCurrAlarmBack = cBMS_CURR_20A;
			strBmsInfo.wCurrAlarm = cBMS_CURR_25A;
			strBmsInfo.wCurrHigh1 = cBMS_CURR_30A;
			strBmsInfo.wCurrHigh2 = cBMS_CURR_50A;
			
		break;
	}
}
//初始化BMS的识别参数
void	sbBmsIdentifyInitial(void)
{
	bModelRsvd00 = 0;
	bModelRsvd01 = 0;
	bModelRsvd02 = 0;
	bModelRsvd03 = 0;
	bModelRsvd04 = 0;
	bModelRsvd05 = 0;
	bModelRsvd06 = 0;
	bModelRsvd07 = 0;
	bModelRsvd08 = 0;
	bModelRsvd09 = 0;
	bModelRsvd10 = 0;
}

/*
 * 函数：sbBmsIdentify
 * -------------------
 * 此函数根据硬件版本识别 BMS 型号，并相应设置参数。
 *
 * 参数：
 *     bFilter：每个型号类型出现次数的阈值。
 *
 * 返回：
 *     如果成功识别 BMS 型号并设置参数，则返回 true；否则返回 false。
 */
INT8U	sbBmsIdentify(INT8U bFilter)
{
	INT8U bModelChkOk = false; // 表示 BMS 型号检查是否成功的标志
    INT16S wModelValue; // 用于存储硬件版本

	wModelValue = swGetHwVersion1Real();//获取实际硬件版本
//	wModelValue = cMODEL_RSVD02_LO_LIMIT;	//swGetHwVersion1Real();	// for test
//	sSetEepromBattType(eBATT_TYPE_48300_C10);//eBATT_TYPE_48200_C10
#if(cMODEL_RSVD00_LO_LIMIT==0)
	if(wModelValue <= cMODEL_RSVD00_UP_LIMIT)
#else
	if((wModelValue >= cMODEL_RSVD00_LO_LIMIT) && (wModelValue <= cMODEL_RSVD00_UP_LIMIT))// 根据硬件版本范围增加相应的型号计数器
#endif
	{
		bModelRsvd00++;
	}
	else if((wModelValue >= cMODEL_RSVD01_LO_LIMIT) && (wModelValue <= cMODEL_RSVD01_UP_LIMIT))
	{
		bModelRsvd01++;
	}
	else if((wModelValue >= cMODEL_RSVD02_LO_LIMIT) && (wModelValue <= cMODEL_RSVD02_UP_LIMIT))
	{
		bModelRsvd02++;
	}
	else if((wModelValue >= cMODEL_RSVD03_LO_LIMIT) && (wModelValue <= cMODEL_RSVD03_UP_LIMIT))
	{
		bModelRsvd03++;
	}
	else if((wModelValue >= cMODEL_RSVD04_LO_LIMIT) && (wModelValue <= cMODEL_RSVD04_UP_LIMIT))
	{
		bModelRsvd04++;
	}
	else if((wModelValue >= cMODEL_RSVD05_LO_LIMIT) && (wModelValue <= cMODEL_RSVD05_UP_LIMIT))
	{
		bModelRsvd05++;
	}
	else if((wModelValue >= cMODEL_RSVD06_LO_LIMIT) && (wModelValue <= cMODEL_RSVD06_UP_LIMIT))
	{
		bModelRsvd06++;
	}
	else if((wModelValue >= cMODEL_RSVD07_LO_LIMIT) && (wModelValue <= cMODEL_RSVD07_UP_LIMIT))
	{
		bModelRsvd07++;
	}
	else if((wModelValue >= cMODEL_RSVD08_LO_LIMIT) && (wModelValue <= cMODEL_RSVD08_UP_LIMIT))
	{
		bModelRsvd08++;
	}
	else if((wModelValue >= cMODEL_RSVD09_LO_LIMIT) && (wModelValue <= cMODEL_RSVD09_UP_LIMIT))
	{
		bModelRsvd09++;
	}
	else if((wModelValue >= cMODEL_RSVD10_LO_LIMIT) && (wModelValue <= cMODEL_RSVD10_UP_LIMIT))
	{
		bModelRsvd10++;
	}
	else if(wModelValue >= cMODEL_RSVD10_UP_LIMIT)
	{
		bModelRsvd10++;
	}
	
	// 机型选择只设置电芯数量,容量\温感\其他具体型号出厂时设置
	// 当某一型号计数达到阈值时，设置 BMS 参数并返回 true
	if((bModelRsvd00 >= bFilter) || (bModelRsvd01 >= bFilter) \
		|| (bModelRsvd02 >= bFilter) || (bModelRsvd03 >= bFilter) \
		|| (bModelRsvd04 >= bFilter))
	{
		sSetBmsCellVoltNum(cBMS_CELL_VOLT_16S);
		sSetBmsBalanceFunc(cBMS_BALANCE_ENABLE);
		sSetBmsLedLcdMode(cBMS_LCD_MODE);
		sSetBmsCurrSampType(cBMS_CURR_SAMPLE_10);
		bModelChkOk = true;
	}
	else if((bModelRsvd05 >= bFilter) || (bModelRsvd06 >= bFilter) \
		|| (bModelRsvd07 >= bFilter) || (bModelRsvd08 >= bFilter) \
		|| (bModelRsvd09 >= bFilter) || (bModelRsvd10 >= bFilter))
	{
		sSetBmsCellVoltNum(cBMS_CELL_VOLT_8S);
		sSetBmsBalanceFunc(cBMS_BALANCE_ENABLE);
		sSetBmsLedLcdMode(cBMS_LCD_MODE);
		sSetBmsCurrSampType(cBMS_CURR_SAMPLE_10);
		bModelChkOk = true;
	}
	//设置电池充电和放电相关参数
	sSetBattChgCurrAdj(swGetEepromBattChgCurrAdj());
	sSetBattChgCurrBias(swGetEepromBattChgCurrBias());
	sSetBattDsgCurrAdj(swGetEepromBattDsgCurrAdj());
	sSetBattDsgCurrBias(swGetEepromBattDsgCurrBias());
	
	// 若BMS型号检查成功，则更新BMS消息并返回true;否则返回false
	if(bModelChkOk == true)
	{
		sBmsInfoUpdate();
		sBmsParaUpdate();
		return true;
	}
	else
	{
		return false;
	}
}

/*
 * 函数：sBmsCharge5AChk
 * ---------------------
 * 此函数检查电流是否超过 5A，并根据检测结果设置全局标志位。
 *
 * 参数：
 *     wCurr：当前电流值。
 *     wFilter1：用于检查电流超过阈值的计数器的阈值。
 *     wFilter2：用于检查电流未超过阈值的计数器的阈值。
 */
void	sBmsCharge5AChk(INT16S wCurr, INT16U wFilter1, INT16U wFilter2)
{
	static  INT16U   wChkCnt = 0;// 检查计数器
	
	// 根据当前BMS模式和电流值设置Charge5A标志位
	if((bBmsMode != cBatteryMode) && (bBmsMode != cDischargeMode) \
		&& (bBmsMode != cChargeMode) && (bBmsMode != cSleepMode))
	{
		fBmsGlobal.bits.Charge5A = false;
		wChkCnt = 0;
	}
	else if(fBmsGlobal.bits.Charge5A == true)
	{
		if(sb16UUnderLevelChk(wCurr,50,wFilter2,&wChkCnt) == true)
		{
			fBmsGlobal.bits.Charge5A = false;
		}
	}
	else
	{
		if(sb16UOverLevelChk(wCurr,50,wFilter1,&wChkCnt) == true)
		{
			fBmsGlobal.bits.Charge5A = true;
		}
	}
}

/*
 * 函数：sBmsCurrentStatusChk
 * --------------------------
 * 此函数根据当前电流值和 BMS 模式，检查电流状态并设置全局标志位。
 *
 * 参数：
 *     wCurr：当前电流值。
 *     wFilter1：用于检查电流状态的计数器的阈值。
 *     wFilter2：用于检查电流状态的计数器的阈值。
 */
void	sBmsCurrentStatusChk(INT16S wCurr, INT16U wFilter1, INT16U wFilter2)
{
	static  INT8U   bIdleChkCnt = 0;// 空闲状态计数器
	static  INT8U   bDsgChkCnt = 0; //放电状态计数器
	static  INT8U   bChgChkCnt = 0; //充电状态计数器
	
	// 根据当前BMS模式和电流值设置电流状态
	if((bBmsMode != cBatteryMode) && (bBmsMode != cDischargeMode) \
		&& (bBmsMode != cChargeMode) && (bBmsMode != cSleepMode))
	{
		fBmsGlobal.bits.CurrStatus = cBMS_CURR_IDLE;
		bIdleChkCnt = 0;
		bChgChkCnt = 0;
		bDsgChkCnt = 0;
	}
	else if(fBmsGlobal.bits.CurrStatus == cBMS_CURR_DSG)
	{
		if(wCurr >= cBMS_IDLE_CURR_CHG)
		{
			bChgChkCnt++;
			bIdleChkCnt++;
		}
		else if(wCurr >= cBMS_IDLE_CURR_DEF)
		{
			bChgChkCnt = 0;
			bIdleChkCnt++;
		}
		else
		{
			bChgChkCnt = 0;
			bIdleChkCnt = 0;
		}
		bDsgChkCnt = 0;
		if((bBmsMode == cChargeMode) || (bBmsMode == cSleepMode))
		{
			bIdleChkCnt = wFilter2;
		}
	}
	else if(fBmsGlobal.bits.CurrStatus == cBMS_CURR_CHG)
	{
		if(wCurr <= cBMS_IDLE_CURR_DSG)
		{
			bIdleChkCnt++;
			bDsgChkCnt++;
		}
		else if(wCurr <= cBMS_IDLE_CURR_DEF)
		{
			bIdleChkCnt++;
			bDsgChkCnt = 0;
		}
		else
		{
			bIdleChkCnt = 0;
			bDsgChkCnt = 0;
		}
		bChgChkCnt = 0;
		if(bBmsMode == cDischargeMode)
		{
			bIdleChkCnt = wFilter2;
		}
	}
	else
	{
		if(wCurr <= cBMS_IDLE_CURR_DSG)
		{
			bDsgChkCnt++;
			bChgChkCnt = 0;
		}
		else if(wCurr >= cBMS_IDLE_CURR_CHG)
		{
			bChgChkCnt++;
			bDsgChkCnt = 0;
		}
		else
		{
			bDsgChkCnt = 0;
			bChgChkCnt = 0;
		}
		bIdleChkCnt = 0;
	}
	// 根据计数器设置电流状态
	if(bDsgChkCnt >= wFilter1)
	{
		bDsgChkCnt = 0;
		fBmsGlobal.bits.CurrStatus = cBMS_CURR_DSG;
	}
	else if(bChgChkCnt >= wFilter1)
	{
		bChgChkCnt = 0;
		fBmsGlobal.bits.CurrStatus = cBMS_CURR_CHG;
	}
	else if(bIdleChkCnt >= wFilter2)
	{
		bIdleChkCnt = 0;
		fBmsGlobal.bits.CurrStatus = cBMS_CURR_IDLE;
	}
}

void	sBmsFaultUpdate(void)	// BMS停止充放电
{
	INT8U bToFaultMode = false;
	
	if(sbGetBattVoltHi())
	{
		if(sbGetBmsFaultCode() == 0)
		{
			sSetBmsFaultCode(cBattVoltHiId);
		}
		sSetBmsFaultFlag(cBattVoltHiFault);
		if(bBmsMode != cFaultMode)
		{
			bToFaultMode = true;
		}
	}
	else
	{
		sClrBmsFaultFlag(cBattVoltHiFault);
	}
	
	if(sbGetHsTempHi())
	{
		if(sbGetBmsFaultCode() == 0)
		{
			sSetBmsFaultCode(cHsTempHiId);
		}
		sSetBmsFaultFlag(cHsTempHiFault);
		if(bBmsMode != cFaultMode)
		{
			bToFaultMode = true;
		}
	}
	else
	{
		sClrBmsFaultFlag(cHsTempHiFault);
	}
	
	if(sbGetCellVoltHi2())
	{
		if(sbGetBmsFaultCode() == 0)
		{
			sSetBmsFaultCode(cCellVoltHiId);
		}
		sSetBmsFaultFlag(cCellVoltHiFault);
		if(bBmsMode != cFaultMode)
		{
			bToFaultMode = true;
		}
	}
	else
	{
		sClrBmsFaultFlag(cCellVoltHiFault);
	}
	
	if(sbGetCellVoltLo2())
	{
		if(sbGetBmsFaultCode() == 0)
		{
			sSetBmsFaultCode(cCellVoltLoId);
		}
		sSetBmsFaultFlag(cCellVoltLoFault);
		if(bBmsMode != cFaultMode)
		{
			bToFaultMode = true;
		}
	}
	else
	{
		sClrBmsFaultFlag(cCellVoltLoFault);
	}
	
	if(sbGetBattChgCurrHi2())
	{
		if(sbGetBmsFaultCode() == 0)
		{
			sSetBmsFaultCode(cBattChgCurrHiId);
		}
		sSetBmsFaultFlag(cBattChgCurrHiFault);
		if(bBmsMode != cFaultMode)
		{
			bToFaultMode = true;
		}
	}
	else
	{
		sClrBmsFaultFlag(cBattChgCurrHiFault);
	}
	
	if(sbGetBattDcgCurrHi2())
	{
		if(sbGetBmsFaultCode() == 0)
		{
			sSetBmsFaultCode(cBattDcgCurrHiId);
		}
		sSetBmsFaultFlag(cBattDcgCurrHiFault);
		if(bBmsMode != cFaultMode)
		{
			bToFaultMode = true;
		}
	}
	else
	{
		sClrBmsFaultFlag(cBattDcgCurrHiFault);
	}
	
	if(sbGetCellDcgTempHi2())
	{
		if(sbGetBmsFaultCode() == 0)
		{
			sSetBmsFaultCode(cCellTempHiId);
		}
		sSetBmsFaultFlag(cCellTempHiFault);
		if(bBmsMode != cFaultMode)
		{
			bToFaultMode = true;
		}
	}
	else
	{
		sClrBmsFaultFlag(cCellTempHiFault);
	}
	
	if(sbGetCellDcgTempLo2())
	{
		if(sbGetBmsFaultCode() == 0)
		{
			sSetBmsFaultCode(cCellTempLoId);
		}
		sSetBmsFaultFlag(cCellTempLoFault);
		if(bBmsMode != cFaultMode)
		{
			bToFaultMode = true;
		}
	}
	else
	{
		sClrBmsFaultFlag(cCellTempLoFault);
	}
	
	if(sbGetBreakWireFault())
	{
		if(sbGetBmsFaultCode() == 0)
		{
			//sSetBmsFaultCode(cBreakWireFaultId);
		}
		//sSetBmsFaultFlag(cBreakWireFault);
		if(bBmsMode != cFaultMode)
		{
			bToFaultMode = true;
		}
	}
	else
	{
		//sClrBmsFaultFlag(cBreakWireFault);
	}
	
	if(bToFaultMode == true)
	{
		bToFaultMode = false;
		OSEventSend(cPrioSuper,eSuperToFault);
	}
}

void	sBmsAlarmUpdate(void)	// 逆变器/充电器停止充放
{
	if(sbGetCellVoltHi())
	{
		sSetBmsAlarmFlag(cCellVoltHiAlarm);
	}
	else
	{
		sClrBmsAlarmFlag(cCellVoltHiAlarm);
	}
	
	if(sbGetCellVoltLo())
	{
		sSetBmsAlarmFlag(cCellVoltLoAlarm);
	}
	else
	{
		sClrBmsAlarmFlag(cCellVoltLoAlarm);
	}
	
	if(sbGetBattChgCurrHi())
	{
		sSetBmsAlarmFlag(cBattChgCurrHiAlarm);
	}
	else
	{
		sClrBmsAlarmFlag(cBattChgCurrHiAlarm);
	}
	
	if(sbGetBattDcgCurrHi())
	{
		sSetBmsAlarmFlag(cBattDcgCurrHiAlarm);
	}
	else
	{
		sClrBmsAlarmFlag(cBattDcgCurrHiAlarm);
	}
	
	if(sbGetCellChgTempHi() || sbGetCellDcgTempHi())
	{
		sSetBmsAlarmFlag(cCellTempHiAlarm);
	}
	else
	{
		sClrBmsAlarmFlag(cCellTempHiAlarm);
	}
	
	if(sbGetCellChgTempLo() || sbGetCellDcgTempLo())
	{
		sSetBmsAlarmFlag(cCellTempLoAlarm);
	}
	else
	{
		sClrBmsAlarmFlag(cCellTempLoAlarm);
	}
	
	if(sbGetHsTempHi())
	{
		sSetBmsAlarmFlag(cHsTempHiAlarm);
	}
	else
	{
		sClrBmsAlarmFlag(cHsTempHiAlarm);
	}
	
	if(sbGetBreakWireFault())
	{
		sSetBmsAlarmFlag(cBreakWireAlarm);
	}
	else
	{
		sClrBmsAlarmFlag(cBreakWireAlarm);
	}
	
	if(sbGetAfeEepromInitAlarm() \
		|| sbGetAfeRamRwAlarm() || sbGetAfeEepromWrAlarm())
	{
		sSetBmsAlarmFlag(cAdcTimingAlarm);
	}
	else
	{
		sClrBmsAlarmFlag(cAdcTimingAlarm);
	}
}

void	sBmsNoticeUpdate(void)	// 逆变器/充电器开始降额运行
{
	if(sbGetCellVoltHiAlm())
	{
		sSetBmsNoticeFlag(cCellVoltHiAlarm);
	}
	else
	{
		sClrBmsNoticeFlag(cCellVoltHiAlarm);
	}
	
	if(sbGetCellVoltLoAlm())
	{
		sSetBmsNoticeFlag(cCellVoltLoAlarm);
	}
	else
	{
		sClrBmsNoticeFlag(cCellVoltLoAlarm);
	}
	
	if(sbGetBattChgCurrHiAlm())
	{
		sSetBmsNoticeFlag(cBattChgCurrHiAlarm);
	}
	else
	{
		sClrBmsNoticeFlag(cBattChgCurrHiAlarm);
	}
	
	if(sbGetBattDcgCurrHiAlm())
	{
		sSetBmsNoticeFlag(cBattDcgCurrHiAlarm);
	}
	else
	{
		sClrBmsNoticeFlag(cBattDcgCurrHiAlarm);
	}
	
	if(sbGetCellTempHiAlm())
	{
		sSetBmsNoticeFlag(cCellTempHiAlarm);
	}
	else
	{
		sClrBmsNoticeFlag(cCellTempHiAlarm);
	}
	
	if(sbGetCellTempLoAlm())
	{
		sSetBmsNoticeFlag(cCellTempLoAlarm);
	}
	else
	{
		sClrBmsNoticeFlag(cCellTempLoAlarm);
	}
	
	if(sbGetHsTempHiAlm())
	{
		sSetBmsNoticeFlag(cHsTempHiAlarm);
	}
	else
	{
		sClrBmsNoticeFlag(cHsTempHiAlarm);
	}
	
	if(sbGetCellVoltErrHiAlm())
	{
		sSetBmsNoticeFlag(cCellVoltErrHiAlarm);
	}
	else
	{
		sClrBmsNoticeFlag(cCellVoltErrHiAlarm);
	}
}

/*
 * 函数：sBmsDataStatistic
 * ----------------------
 * 此函数用于统计 BMS 运行时的各项数据，包括运行时间、充放电量、充放电能量、充放电时间等。
 */
void	sBmsDataStatistic(void)
{
	INT16S wTemp;// 临时变量
	
	// BMS run time Statistic（bms运行时间）
	if(++bBmsRunTimeAcc >= 10)
	{
		dwBmsRunSec++;
		bBmsRunTimeAcc = 0;
		sSetEepromBattRunTime(sdwGetEepromBattRunTime() + 1);
	}
	//当BMS处于电池模式、充电模式或放电模式时进行统计
	if((bBmsMode == cBatteryMode) || (bBmsMode == cChargeMode) \
		|| (bBmsMode == cDischargeMode))
	{
		// Charge & Discharge AH real Statistic。充放电实际电量
		if(swGetEepromBattAhReal() != swGetSoxAhReal())
		{
			sSetEepromBattAhReal(swGetSoxAhReal());
		}
		
		// Charge & Discharge AH max Statistic。最大充放电电量
		if(swGetEepromBattAhMax() != swGetSoxAhMax())
		{
			sSetEepromBattAhMax(swGetSoxAhMax());
			sSetEepromRealtimeMode(cEEPROM_SAVE_MODE_ENABLE);
		}
		
		// Charge & Discharge AH Statistic。充放电电量统计
		wTemp = swGetBattCurrNew();
		if(wTemp > 0)
		{
			dwBmsChgAhAcc += wTemp;
			if(dwBmsChgAhAcc >= 3600)			// 0.01AH
			{
				dwBmsChgAhAcc -= 3600;
				sSetEepromBattChgAh(sdwGetEepromBattChgAh() + 1);
			}
		}
		else
		{
			dwBmsDcgAhAcc += swAbs(wTemp);
			if(dwBmsDcgAhAcc >= 3600)	// 0.01AH
			{
				dwBmsDcgAhAcc -= 3600;
				sSetEepromBattDcgAh(sdwGetEepromBattDcgAh() + 1);
			}
		}
		//充放电能量统计
		// Charge & Discharge WH Statistic
		wTemp = ((INT32S)((INT16S)swGetBattVoltNew()) * swGetBattCurrNew() + 500) / 1000;
		if(wTemp > 0)
		{
			dwBmsChgWhAcc += wTemp;
			if(dwBmsChgWhAcc >= 36000)	// 1WH
			{
				dwBmsChgWhAcc -= 36000;
				sSetEepromBattChgWh(sdwGetEepromBattChgWh() + 1);
			}
		}
		else
		{
			dwBmsDcgWhAcc += swAbs(wTemp);
			if(dwBmsDcgWhAcc >= 36000)	// 1WH
			{
				dwBmsDcgWhAcc -= 36000;
				sSetEepromBattDcgWh(sdwGetEepromBattDcgWh() + 1);
			}
		}
		
		// Charge & Discharge Time Statistic 充放电时间统计
		if(sbGetBmsCurrentStatus() == cBMS_CURR_CHG)
		{
			if(++bBmsChgTimeAcc >= 10)
			{
				bBmsChgTimeAcc = 0;
				sSetEepromBattChgTime(sdwGetEepromBattChgTime() + 1);
			}
		}
		else if(sbGetBmsCurrentStatus() == cBMS_CURR_DSG)
		{
			if(++bBmsDcgTimeAcc >= 10)
			{
				bBmsDcgTimeAcc = 0;
				sSetEepromBattDcgTime(sdwGetEepromBattDcgTime() + 1);
			}
		}
	}
}
/*
 * 函数：sBmsTimeStore
 * -------------------
 * 此函数用于定时存储电池相关数据，如实时电量百分比和充放电循环次数。
 * 参数：
 *   - wFilter: 存储间隔，以分钟为单位
 */
void	sBmsTimeStore(INT16U wFilter)
{
	static INT16U wMinuteCnt = 0;// 分钟计数
	INT16U wTemp;// 临时变量
	
	if(++wMinuteCnt >= wFilter)
	{
		wMinuteCnt = 0;
		
		// 实时电量百分比存储
		wTemp = ((INT32U)swGetEepromBattAhReal() * 100) / swGetEepromBattAhMax();
		if(wTemp != swGetSocNew())
		{
			sSetEepromRealtimeMode(cEEPROM_SAVE_MODE_ENABLE);
		}
		
		// 充放电循环存储
		wTemp = (sdwGetEepromBattChgAh() + sdwGetEepromBattDcgAh()) / ((INT32U)swGetBmsRatedAH() * 20);
		sSetEepromBattCycCnt(wTemp);
	}
}
/*
 * 函数：sBmsChgToOffChk
 * --------------------
 * 此函数用于检测充电模式或休眠模式下的长时间非充电状态，并触发关机事件。
 * 参数：
 *   - wFilter1: 判断非充电状态的时间阈值，以分钟为单位
 *   - wFilter2: 触发关机的时间阈值，以分钟为单位
 */
void	sBmsChgToOffChk(INT16U wFilter1, INT16U wFilter2)
{
	static INT16U wToShutdownCnt = 0;
	static INT16U wToShutdownTime = 0;
	INT8U bCharging;
	
	// 充电状态
	if(sbGetBmsCurrentStatus() == cBMS_CURR_CHG)
	{
		bCharging = true;
	}
	else
	{
		bCharging = false;
	}
	/*
	bCharging = false;
	if(sbGetCanAddress() == 0)	// 单机
	{
		if(sbGetBmsCurrentStatus() == cBMS_CURR_CHG)
		{
			bCharging = true;
		}
	}
	else	// 并机
	{
		bCharging = swGetEmsStatusCharging();
	}
	*/
	
	// 充电模式或者休眠模式下如果长时间不处于充电状态,则关机
	if(((bBmsMode != cChargeMode) && (bBmsMode != cSleepMode)) \
		|| (wFilter2 == 0) || (bCharging == true))
	{
		wToShutdownCnt = 0;
		wToShutdownTime = 0;
		return;
	}
	
	// 判断非充电状态并触发关机事件
	if(++wToShutdownCnt >= wFilter1)	// 1min
	{
		wToShutdownCnt = 0;
		if(++wToShutdownTime >= wFilter2)
		{
			wToShutdownTime = 0;
			OSEventSend(cPrioSuper, eSuperToShutdown);
		}
	}
}
/*
 * 函数：sBmsIdleToOffChk
 * ---------------------
 * 此函数用于检测电池长时间处于空闲状态且电量较低时触发关机事件。
 * 参数：
 *   - wFilter1: 判断空闲状态的时间阈值，以分钟为单位
 *   - wFilter2: 触发关机的时间阈值，以分钟为单位
 */
void	sBmsIdleToOffChk(INT16U wFilter1, INT16U wFilter2)
{
	static INT16U wToShutdownCnt = 0;// 空闲状态计数
	static INT16U wToShutdownTime = 0;// 触发关机计数
	INT8U bIdling, bSoc;// 空闲状态标志，电量百分比
	
	// 充电状态
	bIdling = false;
	if(sbGetCanAddress() == 0)	// 单机
	{
		bSoc = swGetSocNew();
		if(sbGetBmsCurrentStatus() == cBMS_CURR_IDLE)
		{
			bIdling = true;
		}
	}
	else	// 并机
	{
		bSoc = (swGetEmsDataTotalSOC() + 5) / 10;
		if((swGetEmsStatusCharging() == false) \
			&& (swGetEmsStatusDischarging() == false))
		{
			bIdling = true;
		}
	}
	
	// 电池包长时间不使用(电流很小)且电池包SOC≤20%则关机
	if((bSoc <= 20) && (bIdling == true) && (wFilter2 != 0))
	{
		wToShutdownCnt++;
	}
	else
	{
		wToShutdownCnt = 0;
		wToShutdownTime = 0;
	}
	
	if(wToShutdownCnt >= wFilter1)	// 1min
	{
		wToShutdownCnt = 0;
		if(++wToShutdownTime >= wFilter2)
		{
			wToShutdownTime = 0;
			sSetBmsParallelShutdown(true);
			OSEventSend(cPrioSuper, eSuperToShutdown);
		}
	}
}

/*
 * 函数：sBmsLossToOffChk
 * --------------------
 * 此函数用于检测通信丢失情况并触发关机事件。
 * 参数：
 *   - wFilter: 通信丢失计数阈值
 */
void	sBmsLossToOffChk(INT16U wFilter)
{
	// 将通信丢失计数阈值设置为函数参数
    wCommLossCountLimit = wFilter;
    
    // 如果当前通信丢失计数超过阈值，则将其设置为阈值
    if (wCommLossCount > wCommLossCountLimit) {
        wCommLossCount = wCommLossCountLimit;
    }

    // 如果当前通信丢失计数大于0，则将其递减；否则触发关机事件
	if(wCommLossCount > 0)
	{
		wCommLossCount--;
	}
	else
	{
		OSEventSend(cPrioSuper, eSuperToShutdown);
	}
}

void	sBmsSoftStartChk(INT16U wFilter, INT16U wTimeOut)
{
	static INT16U wChkCnt = 0;
	//如果BMS模式为电池模式
	if(bBmsMode == cBatteryMode)
	{
		switch(bSoftStartMode)//软启动模式
		{
			case cSS_MODE_STANDBY:
				;
			break;
			case cSS_MODE_READY:
				wChkCnt = 0;
				mMOS_SOFT_ALL_ON();
				bSoftStartMode = cSS_MODE_DELAY;
			break;
			case cSS_MODE_DELAY:
				if(++wChkCnt >= wFilter)
				{
					wChkCnt = 0;
					bSoftStartMode = cSS_MODE_CHECK;
				}
			break;
			case cSS_MODE_CHECK:
				if((swGetPackVoltReal() + 800) >= swGetBattVoltReal())
				{
					wChkCnt = 0;
					bSoftStartMode = cSS_MODE_PASS;
				}
				else if(++wChkCnt >= wTimeOut)
				{
					wChkCnt = 0;
					bSoftStartMode = cSS_MODE_TIMEOUT;
				}
			break;
			case cSS_MODE_PASS:
				if(++wChkCnt >= 10)		// 10*20ms=200ms
				{
					wChkCnt = 0;
					bSoftStartMode = cSS_MODE_FINISHED;
				}
			break;
			case cSS_MODE_FINISHED:
				isPirated();
				mMOS_ALL_ON();
				mMOS_SOFT_ALL_OFF();
				sSetOpSoftFinishedFlag(true);
				bSoftStartMode = cSS_MODE_STANDBY;
			break;
			case cSS_MODE_TIMEOUT:
				if(sbGetBmsFaultCode() == 0)
				{
					sSetBmsFaultCode(cOutputResLoId);
				}
				sSetBmsFaultFlag(cOutputResLoFault);
				OSEventSend(cPrioSuper,eSuperToFault);
				bSoftStartMode = cSS_MODE_STANDBY;
			break;
			default:
				bSoftStartMode = cSS_MODE_STANDBY;
			break;
		}
	}
	else
	{
		mMOS_SOFT_ALL_OFF();
		bSoftStartMode = cSS_MODE_STANDBY;
	}
}

//BMS状态更新
void	sBmsStatusUpdate(void)
{
	// 
	if(sbGetBmsCurrentStatus() == cBMS_CURR_DSG)
	{
		fBmsStatus.bits.Charging = false;
		fBmsStatus.bits.Discharging = true;
	}
	else if(sbGetBmsCurrentStatus() == cBMS_CURR_CHG)
	{
		fBmsStatus.bits.Charging = true;
		fBmsStatus.bits.Discharging = false;
	}
	else
	{
		fBmsStatus.bits.Charging = false;
		fBmsStatus.bits.Discharging = false;
	}
	
	// 
	fBmsStatus.bits.DischargeMOS = mIS_MOS_DSG_ON();
	fBmsStatus.bits.ChargeMOS = mIS_MOS_CHG_ON();
	fBmsStatus.bits.DischargeSoftMOS = mIS_MOS_SOFT_DSG_ON();
	fBmsStatus.bits.ChargeSoftMOS = mIS_MOS_SOFT_CHG_ON();
	
	if((bBmsMode == cBatteryMode) || (bBmsMode == cDischargeMode) \
		|| (bBmsMode == cChargeMode))
	{
		// Charge enable
		if((sbGetBmsSoftStartFlag() != true) \
			|| sbGetAfeRamRwAlarm() || sbGetAfeEepromWrAlarm() \
			|| sbGetAfeEepromInitAlarm() || sbGetBreakWireFault() \
			|| sbGetCellVoltHiAlm() || sbGetBattChgCurrHi() \
			|| sbGetCellChgTempHi() || sbGetCellChgTempLo())
		{
			fBmsStatus.bits.ChargeEnable = false;
		}
		else
		{
			fBmsStatus.bits.ChargeEnable = true;
		}
		
		// Discharge enable
		if((sbGetBmsSoftStartFlag() != true) \
			|| sbGetAfeRamRwAlarm() || sbGetAfeEepromWrAlarm() \
			|| sbGetAfeEepromInitAlarm() || sbGetBreakWireFault() \
			|| sbGetCellVoltLoAlm() || sbGetBattDcgCurrHi() \
			|| sbGetCellDcgTempHi() || sbGetCellDcgTempLo())
		{
			fBmsStatus.bits.DischargeEnable = false;
		}
		else
		{
			fBmsStatus.bits.DischargeEnable = true;
		}
	}
	else
	{
		fBmsStatus.bits.ChargeEnable = false;
		fBmsStatus.bits.DischargeEnable = false;
	}
	
	// Recommended voltage & current
	wRecChargeVolt = (INT32U)360 * swGetBmsCellVoltNum();	// CV=57.6V
	wRecCutoffVolt = (INT32U)300 * swGetBmsCellVoltNum();	// STOP=48V
	wRecChargeCurrRef = swBmsChargeDerated();			// 通过接口传到通讯上
	wRecDischargeCurrRef = swBmsDischargeDerated();		// 通过接口传到通讯上
	// Charge current
	if(fBmsStatus.bits.ChargeEnable == true)
	{
		if(wRecChargeCurr < wRecChargeCurrRef)
		{
			wRecChargeCurr++;
		}
		else if(wRecChargeCurr > wRecChargeCurrRef)
		{
			wRecChargeCurr--;
		}
	}
	else
	{
		wRecChargeCurr = 0;
	}
	// Discharge current
	if(fBmsStatus.bits.DischargeEnable == true)
	{
		if(wRecDischargeCurr < wRecDischargeCurrRef)
		{
			wRecDischargeCurr++;
		}
		else if(wRecDischargeCurr > wRecDischargeCurrRef)
		{
			wRecDischargeCurr--;
		}
	}
	else
	{
		wRecDischargeCurr = 0;
	}
	
	// Charge flag
	if(fBmsStatus.bits.ChargeEnable)
	{
		if(swGetSocNew() <= 5)
		{
			fBmsStatus.bits.ChargeImmediately1 = true;
		}
		else if(swGetSocNew() >= 10)
		{
			fBmsStatus.bits.ChargeImmediately1 = false;
		}
		
		if(swGetSocNew() <= 10)
		{
			fBmsStatus.bits.ChargeImmediately2 = true;
		}
		else if(swGetSocNew() >= 20)
		{
			fBmsStatus.bits.ChargeImmediately2 = false;
		}
	}
	else
	{
		fBmsStatus.bits.FullCharge = false;
		fBmsStatus.bits.ChargeImmediately2 = false;
		fBmsStatus.bits.ChargeImmediately1 = false;
	}
	
	// 转换成旧BmsStatus格式,用于旧MODBUS协议用
	if(fBmsStatus.bits.Discharging)
	{
		fBmsStatusOld.bits.bBattStatus = cBMS_CURR_DSG;
	}
	else if(fBmsStatus.bits.Charging)
	{
		fBmsStatusOld.bits.bBattStatus = cBMS_CURR_CHG;
	}
	else
	{
		fBmsStatusOld.bits.bBattStatus = cBMS_CURR_IDLE;
	}
	
	fBmsStatusOld.bits.bOpSoftFlag = fBmsStatus.bits.DischargeMOS;
	fBmsStatusOld.bits.bChgAllowFlag = fBmsStatus.bits.ChargeEnable;
	fBmsStatusOld.bits.bChgForceFlag = fBmsStatus.bits.FullCharge;
	fBmsStatusOld.bits.bDcgAllowFlag = fBmsStatus.bits.DischargeEnable;
}
/*
 * 函数：swBmsChargeDerated
 * -----------------------
 * 此函数用于计算根据电池状态限制后的充电电流。
 * 返回：
 *   - 计算后的充电电流（单位：mA）
 */
INT16U	swBmsChargeDerated(void)
{
	INT16U wChargeCurr1 = 0;
	INT16U wChargeCurr2 = 0;
	INT16U wChargeCurr3 = 0;
	INT16S wMaxVolt;
	INT16S wMinVolt;
	
	wMaxVolt = swGetMaxCellVoltNew();//最高电压
	wMinVolt = swGetMinCellVoltNew();//最低电压
	if(wMaxVolt <= 3400)
	{
		wChargeCurr1 = swGetEepromBattChgCurrHiAlm();//获取充电电流
	}
	else if(wMaxVolt >= swGetEepromCellVoltHiAlm())//如果最高电压大于EEPROM电压
	{
		wChargeCurr1 = 50;
	}
	else
	{
		wChargeCurr1 = ((INT32S)((INT16S)swGetEepromCellVoltHiAlm() - wMaxVolt) << 7) / ((INT16S)swGetEepromCellVoltHiAlm() - 3400);
		wChargeCurr1 = ((INT32U)wChargeCurr1 * swGetEepromBattChgCurrHiAlm()) >> 7;
	}
	// 计算根据最低电压限制后的充电电流
	if(wMinVolt >= 3000)
	{
		wChargeCurr2 = swGetEepromBattChgCurrHiAlm();
	}
	else if(wMinVolt <= swGetEepromCellVoltLoAlm())
	{
		wChargeCurr2 = 50;
	}
	else
	{
		wChargeCurr2 = ((INT32S)(wMinVolt - swGetEepromCellVoltLoAlm()) << 7) / (3000 - (INT16S)swGetEepromCellVoltLoAlm());
		wChargeCurr2 = ((INT32U)wChargeCurr2 * swGetEepromBattChgCurrHiAlm()) >> 7;
	}
	// 根据充电温度限制调整充电电流
	if(sbGetCellChgTempHiAlm() || sbGetCellChgTempLoAlm())//充电高位和低位其中有一个为高
	{
		wChargeCurr3 = swGetEepromBattChgCurrHiAlm() >> 1;
	}
	else
	{
		wChargeCurr3 = swGetEepromBattChgCurrHiAlm();
	}
	// 取三者中的最小值作为充电电流
	if(wChargeCurr1 > wChargeCurr2)
	{
		wChargeCurr1 = wChargeCurr2;
	}
	if(wChargeCurr1 > wChargeCurr3)
	{
		wChargeCurr1 = wChargeCurr3;
	}
	if(wChargeCurr1 < 50)
	{
		wChargeCurr1 = 50;
	}
	return wChargeCurr1;
}

// 计算放电时的允许最大电流
INT16U	swBmsDischargeDerated(void)
{
	INT16U wDischargeCurr1 = 0;// 存储第一种情况下的放电电流
	INT16U wDischargeCurr2 = 0;// 存储第二章情况下的放电电流
	
	if(swGetMinCellVoltNew() >= 3000)//最小电压>30V
	{
		wDischargeCurr1 = swGetEepromBattDcgCurrHiAlm();// 设为存储在EEPROM中的放电高限电流
    }
	else
	{
		wDischargeCurr1 = swGetEepromBattDcgCurrHiAlm() >> 1;// 否则将放电电流设为放电高限电流的一半
	}
	
	 // 检查是否有任何一个单体电池的放电温度高限或低限报警触发
    if (sbGetCellDcgTempHiAlm() || sbGetCellDcgTempLoAlm()) 
	{
        wDischargeCurr2 = swGetEepromBattDcgCurrHiAlm() >> 1; // 如果是，则将放电电流设为放电高限电流的一半
    } 
	else 
	{
        wDischargeCurr2 = swGetEepromBattDcgCurrHiAlm(); // 否则将放电电流设为存储在EEPROM中的放电高限电流
    }
	// 返回两种情况下的放电电流的较小值
	if(wDischargeCurr1 > wDischargeCurr2)
	{
		wDischargeCurr1 = wDischargeCurr2;
	}
	return wDischargeCurr1;
}

//更新BMS数据
void	sBmsDataUpdate(void)
{
	INT8U i;// 循环变量
	
	// 遍历所有并机板的数据
	for(i = 0; i < BMS_PARALLEL_NUM_MAX; i++)
	{
		// 如果当前并机板的CAN地址不是i
		if(sbGetCanAddress() != i)
		{
			// 检查并设置CAN并机标志位中的第i位是否置位
			if(CHKNBIT(sdwGetCanParallelFlag(), i))
			{
				// 清空第i个并机板数据结构的内存
				memset(&strBmsData[i], 0, sizeof(union BMS_DATA_TABLE));
			}
		}
	}
	
	sBmsDataLocalUpdate();
	sBms2DataLocalUpdate();
//    sBmsGrowatt_Data_Update();
    sBmsVoltronic_Data_Update();
}

//// 古瑞瓦特数据更新
//void	sBmsGrowatt_Data_Update(void)
//{
//	INT16U *q;
//	INT8U i;
//	
//	pBmsGrowatt->Field.Reserved1 = 0;
//    pBmsGrowatt->Field.FW_VER = 1;
//    pBmsGrowatt->Field.Gauge_Version = 1;
//    pBmsGrowatt->Field.FR_Version_Low = 1;
//    pBmsGrowatt->Field.FR_Version_High = 0;
//    pBmsGrowatt->Field.Date_Time1 = 0;
//    pBmsGrowatt->Field.Date_Time2 = 0;
//    pBmsGrowatt->Field.Date_Time3 = 0;
//    pBmsGrowatt->Field.Date_Time4 = 0;
//    pBmsGrowatt->Field.Bar_Code12 = 0;
//    pBmsGrowatt->Field.Bar_Code34 = 0;
//    pBmsGrowatt->Field.Bar_Code56 = 0;
//    pBmsGrowatt->Field.Bar_Code78 = 0;
//    pBmsGrowatt->Field.Company_Code_Low = 0;
//    pBmsGrowatt->Field.Company_Code_High = 0;
//    pBmsGrowatt->Field.Using_Cap = 0;

//    pBmsGrowatt->Field.Gauge_IC_current = 0;
//    pBmsGrowatt->Field.DataTime.bits.secod = 0;
//    pBmsGrowatt->Field.DataTime.bits.minute = 0;
//    pBmsGrowatt->Field.DataTime.bits.hour = 0;
//    pBmsGrowatt->Field.DataTime.bits.day = 0;
//    pBmsGrowatt->Field.DataTime.bits.Month = 0;
//    pBmsGrowatt->Field.DataTime.bits.Year = 0;
//    
//    pBmsGrowatt->Field.BMS_STATUS.bits.status = 0;
//    pBmsGrowatt->Field.BMS_STATUS.bits.Err_Bit_falg = 0;
//    pBmsGrowatt->Field.BMS_STATUS.bits.cell_banlace_status = 0;
//    pBmsGrowatt->Field.BMS_STATUS.bits.sleep_status = 0;
//    pBmsGrowatt->Field.BMS_STATUS.bits.op_dischg_status = 0;
//    pBmsGrowatt->Field.BMS_STATUS.bits.op_chg_status = 0;
//    pBmsGrowatt->Field.BMS_STATUS.bits.Battery_terminal_status = 0;
//    pBmsGrowatt->Field.BMS_STATUS.bits.Master_box_Operation_Mode = 0;
//    if(swGetEmsStatusDischarging())
//    {
//        pBmsGrowatt->Field.BMS_STATUS.bits.SP_status = 3;
//    }
//    else if(swGetEmsStatusCharging())
//    {
//        pBmsGrowatt->Field.BMS_STATUS.bits.SP_status = 2;
//    }
//    else
//    {
//        pBmsGrowatt->Field.BMS_STATUS.bits.SP_status = 1;
//    }
//    pBmsGrowatt->Field.BMS_STATUS.bits.Force_chg_status = 0;
//    pBmsGrowatt->Field.BMS_STATUS.bits.Reserved = 0;
//    
//    pBmsGrowatt->Field.BMS_Err.bits.over_dischg_curr = sbGetBattDcgCurrHi();
//    pBmsGrowatt->Field.BMS_Err.bits.short_circuit = 0;
//    pBmsGrowatt->Field.BMS_Err.bits.over_volt = sbGetBattVoltHi();
//    pBmsGrowatt->Field.BMS_Err.bits.under_volt = sbGetBattVoltLo();
//    pBmsGrowatt->Field.BMS_Err.bits.over_temp_dischg = sbGetCellDcgTempHi();
//    pBmsGrowatt->Field.BMS_Err.bits.over_temp_chg = sbGetCellChgTempHi();
//    pBmsGrowatt->Field.BMS_Err.bits.under_temp_dischg = sbGetCellDcgTempLo();
//    pBmsGrowatt->Field.BMS_Err.bits.under_temp_chg = sbGetCellChgTempLo();
//    pBmsGrowatt->Field.BMS_Err.bits.soft_start_fail = 0;
//    pBmsGrowatt->Field.BMS_Err.bits.Permanent_Fault = 0;
//    pBmsGrowatt->Field.BMS_Err.bits.Delta_V_Fail = 0;
//    pBmsGrowatt->Field.BMS_Err.bits.over_current_charge = sbGetBattChgCurrHi();
//    pBmsGrowatt->Field.BMS_Err.bits.over_temp_mos = sbGetHsTempHi();
//    pBmsGrowatt->Field.BMS_Err.bits.over_temp_enviorment = 0;
//    pBmsGrowatt->Field.BMS_Err.bits.under_temp_enviorment = 0;
//    
//    pBmsGrowatt->Field.SOC = swGetSocNew();
//    pBmsGrowatt->Field.Voltage = swGetBattVoltNew();
//    pBmsGrowatt->Field.Current = swGetBattCurrFiltNew();
//    pBmsGrowatt->Field.Temperature = swGetHsTempNew();;
//    pBmsGrowatt->Field.ChargeCurrentLimit = wRecChargeCurr*10;
//    
//    pBmsGrowatt->Field.Gauge_RM = 0;
//    pBmsGrowatt->Field.Gauge_FCC = 0;
//    pBmsGrowatt->Field.BMS_Ver.bits.hardware_ver = MACHINE_HW_VERSION;
//    pBmsGrowatt->Field.BMS_Ver.bits.software_ver = MACHINE_SW_VERSION;
//    pBmsGrowatt->Field.Delta = 0;
//    pBmsGrowatt->Field.Cycle_Count = swGetEepromBattCycCnt();
//    pBmsGrowatt->Field.RSVD_For_Master_Box = 0;
//    pBmsGrowatt->Field.SOH = swGetSohNew();
//    pBmsGrowatt->Field.CV_Voltage = wRecChargeVolt;
//    pBmsGrowatt->Field.Warning = 0;
//    pBmsGrowatt->Field.MaxDischargeCurr = wRecDischargeCurr*10;
//    pBmsGrowatt->Field.Extended_Error = 0;
//    pBmsGrowatt->Field.CellVoltMax = swGetMaxCellVoltNew();
//    pBmsGrowatt->Field.CellVoltMin = swGetMinCellVoltNew();
//    pBmsGrowatt->Field.CellVoltMaxNo = sbGetMaxCellVoltNo();
//    pBmsGrowatt->Field.CellVoltMinNo = sbGetMinCellVoltNo();
//    pBmsGrowatt->Field.Cell_Series = cADC_CELL_VOLT_MAX;

//    q = &pBmsGrowatt->Field.CellVolt01;
//	for(i = 0; i < cADC_CELL_VOLT_MAX; i++)
//	{
//		if(i < swGetBmsCellVoltNum())
//		{
//			*q = swGetCellVoltFiltNew(i);
//		}
//		else
//		{
//			*q = 0;
//		}
//		q++;
//	}
//}

//日月源数据更新
void	sBmsVoltronic_Data_Update(void)
{
	INT8U i;
	pBmsVoltronic->Field.Protocol_type = 0;
    pBmsVoltronic->Field.Protocol_version = 1;
    pBmsVoltronic->Field.BMS_Fw_Version_High = 1;
    pBmsVoltronic->Field.BMS_Fw_Version_Low = 1;
    pBmsVoltronic->Field.BMS_Hw_Version_High = 0;
    pBmsVoltronic->Field.BMS_Hw_Version_Low = 0;

    for (i = 0; i < 10; i++)
    {
        pBmsVoltronic->Field.wReserved1[i] = 0;
    }

    pBmsVoltronic->Field.Cell_Num = swGetBmsCellVoltNum();
    for (i = 0; i < 20; i++)
    {
        if(i <  pBmsVoltronic->Field.Cell_Num)
        {
            pBmsVoltronic->Field.CellVolt[i] = swGetCellVoltFiltNew(i)/100;
        }
        else
        {
            pBmsVoltronic->Field.CellVolt[i] = 0;
        }
    }
    
    pBmsVoltronic->Field.Temp_Sensor_Num = swGetBmsCellTempNum();
    for (i = 0; i < 10; i++)
    {
        if(i <  pBmsVoltronic->Field.Temp_Sensor_Num)
        {
            pBmsVoltronic->Field.Sensor_Temp[i] = swGetCellTempFiltNew(i)*10 + 2731;
        }
        else
        {
            pBmsVoltronic->Field.Sensor_Temp[i] = 0;
        }
    }

    if(swGetBattCurrFiltNew() > 0)
    {
        pBmsVoltronic->Field.Charge_Current = swGetBattCurrFiltNew();
        pBmsVoltronic->Field.Discharge_Current = 0;
    }
    else
    {
        pBmsVoltronic->Field.Charge_Current = 0;
        pBmsVoltronic->Field.Discharge_Current = abs(swGetBattCurrFiltNew());
    }
    pBmsVoltronic->Field.Voltage = swGetBattVoltNew();
    pBmsVoltronic->Field.SOC = swGetSocNew();
    pBmsVoltronic->Field.Total_Capacity_H = (strBmsInfo.wBattRatedAH >>8) * 100;//是否需要取一个高八位和低八位
		pBmsVoltronic->Field.Total_Capacity_L = (strBmsInfo.wBattRatedAH & 0x00FF)*100;

    for (i = 0; i < 10; i++)
    {
        pBmsVoltronic->Field.wReserved2[i] = 0;
    }

    pBmsVoltronic->Field.Cell_Num_Warn = swGetBmsCellVoltNum();
    if(sbGetCellVoltLo())
    {
        if((0 == sbGetMinCellVoltNo()) || (1 == sbGetMinCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_1_2_State = 0x01;
        }
        if((2 == sbGetMinCellVoltNo()) || (3 == sbGetMinCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_3_4_State = 0x01;
        }
        if((4 == sbGetMinCellVoltNo()) || (5 == sbGetMinCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_5_6_State = 0x01;
        }
        if((6 == sbGetMinCellVoltNo()) || (7 == sbGetMinCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_7_8_State = 0x01;
        }
        if((8 == sbGetMinCellVoltNo()) || (9 == sbGetMinCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_9_10_State = 0x01;
        }
        if((10 == sbGetMinCellVoltNo()) || (11 == sbGetMinCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_11_12_State = 0x01;
        }
        if((12 == sbGetMinCellVoltNo()) || (13 == sbGetMinCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_13_14_State = 0x01;
        }
        if((14 == sbGetMinCellVoltNo()) || (15 == sbGetMinCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_15_16_State = 0x01;
        }
        if((16 == sbGetMinCellVoltNo()) || (17 == sbGetMinCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_17_18_State = 0x01;
        }
        if((18 == sbGetMinCellVoltNo()) || (19 == sbGetMinCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_19_20_State = 0x01;
        }
    }
    else if(sbGetCellVoltHi())
    {
        if((0 == sbGetMaxCellVoltNo()) || (1 == sbGetMaxCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_1_2_State = 0x02;
        }
        if((2 == sbGetMaxCellVoltNo()) || (3 == sbGetMaxCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_3_4_State = 0x02;
        }
        if((4 == sbGetMaxCellVoltNo()) || (5 == sbGetMaxCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_5_6_State = 0x02;
        }
        if((6 == sbGetMaxCellVoltNo()) || (7 == sbGetMaxCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_7_8_State = 0x02;
        }
        if((8 == sbGetMaxCellVoltNo()) || (9 == sbGetMaxCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_9_10_State = 0x02;
        }
        if((10 == sbGetMaxCellVoltNo()) || (11 == sbGetMaxCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_11_12_State = 0x02;
        }
        if((12 == sbGetMaxCellVoltNo()) || (13 == sbGetMaxCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_13_14_State = 0x02;
        }
        if((14 == sbGetMaxCellVoltNo()) || (15 == sbGetMaxCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_15_16_State = 0x02;
        }
        if((16 == sbGetMaxCellVoltNo()) || (17 == sbGetMaxCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_17_18_State = 0x02;
        }
        if((18 == sbGetMaxCellVoltNo()) || (19 == sbGetMaxCellVoltNo()))
        {
            pBmsVoltronic->Field.Cell_19_20_State = 0x02;
        }
    }
    else
    {
        pBmsVoltronic->Field.Cell_1_2_State = 0;
        pBmsVoltronic->Field.Cell_3_4_State = 0;
        pBmsVoltronic->Field.Cell_5_6_State = 0;
        pBmsVoltronic->Field.Cell_7_8_State = 0;
        pBmsVoltronic->Field.Cell_9_10_State = 0;
        pBmsVoltronic->Field.Cell_11_12_State = 0;
        pBmsVoltronic->Field.Cell_13_14_State = 0;
        pBmsVoltronic->Field.Cell_15_16_State = 0;
        pBmsVoltronic->Field.Cell_17_18_State = 0;
        pBmsVoltronic->Field.Cell_19_20_State = 0;
    }
		for (i = 0; i < 5; i++)
    {
        pBmsVoltronic->Field.wReserved3[i] = 0;
    }

    pBmsVoltronic->Field.Temp_Sensor_Num_Warn = swGetBmsCellTempNum();
    if(sbGetCellDcgTempHi() || sbGetCellChgTempHi())
    {
        if((0 == sbGetMaxCellTempNo()) || (1 == sbGetMaxCellTempNo()))
        {
            pBmsVoltronic->Field.Temp_1_2_State = 0x02;
        }
        if((2 == sbGetMaxCellTempNo()) || (3 == sbGetMaxCellTempNo()))
        {
            pBmsVoltronic->Field.Temp_3_4_State = 0x02;
        }
        if((4 == sbGetMaxCellTempNo()) || (5 == sbGetMaxCellTempNo()))
        {
            pBmsVoltronic->Field.Temp_5_6_State = 0x02;
        }
        if((6 == sbGetMaxCellTempNo()) || (7 == sbGetMaxCellTempNo()))
        {
            pBmsVoltronic->Field.Temp_7_8_State = 0x02;
        }
        if((8 == sbGetMaxCellTempNo()) || (9 == sbGetMaxCellTempNo()))
        {
            pBmsVoltronic->Field.Temp_9_10_State = 0x02;
        }
    }
    else if(sbGetCellDcgTempLo() || sbGetCellChgTempLo())
    {
        if((0 == sbGetMinCellTempNo()) || (1 == sbGetMinCellTempNo()))
        {
            pBmsVoltronic->Field.Temp_1_2_State = 0x01;
        }
        if((2 == sbGetMinCellTempNo()) || (3 == sbGetMinCellTempNo()))
        {
            pBmsVoltronic->Field.Temp_3_4_State = 0x01;
        }
        if((4 == sbGetMinCellTempNo()) || (5 == sbGetMinCellTempNo()))
        {
            pBmsVoltronic->Field.Temp_5_6_State = 0x01;
        }
        if((6 == sbGetMinCellTempNo()) || (7 == sbGetMinCellTempNo()))
        {
            pBmsVoltronic->Field.Temp_7_8_State = 0x01;
        }
        if((8 == sbGetMinCellTempNo()) || (9 == sbGetMinCellTempNo()))
        {
            pBmsVoltronic->Field.Temp_9_10_State = 0x01;
        }
    }
    else
    {
        pBmsVoltronic->Field.Temp_1_2_State = 0;
        pBmsVoltronic->Field.Temp_3_4_State = 0;
        pBmsVoltronic->Field.Temp_5_6_State = 0;
        pBmsVoltronic->Field.Temp_7_8_State = 0;
        pBmsVoltronic->Field.Temp_9_10_State = 0;
    }

    if(sbGetBattVoltHi())
    {
        pBmsVoltronic->Field.Module_Charge_Volt_State = 0x02;
    }
    else
    {
        pBmsVoltronic->Field.Module_Charge_Volt_State = 0;
    }

    if(sbGetBattVoltHi())
    {
        pBmsVoltronic->Field.Module_DisCharge_Volt_State = 0x02;
    }
    else if(sbGetBattVoltLo())
    {
        pBmsVoltronic->Field.Module_DisCharge_Volt_State = 0x01;
    }
    else
    {
        pBmsVoltronic->Field.Module_DisCharge_Volt_State = 0;
    }

    if(sbGetCellVoltHi())
    {
        pBmsVoltronic->Field.Cell_Charge_Volt_State = 0x02;
    }
    else
    {
        pBmsVoltronic->Field.Cell_Charge_Volt_State = 0;
    }

    if(sbGetCellVoltHi())
    {
        pBmsVoltronic->Field.Cell_DisCharge_Volt_State = 0x02;
    }
    else if(sbGetCellVoltLo())
    {
        pBmsVoltronic->Field.Cell_DisCharge_Volt_State = 0x01;
    }
    else
    {
        pBmsVoltronic->Field.Cell_DisCharge_Volt_State = 0;
    }

    if(sbGetBattChgCurrHi())
    {
        pBmsVoltronic->Field.Module_Charge_Curr_State = 0x02;
    }
    else
    {
        pBmsVoltronic->Field.Module_Charge_Curr_State = 0;
    }

    if(sbGetBattDcgCurrHi())
    {
        pBmsVoltronic->Field.Module_DisCharge_Curr_State = 0x02;
    }
    else
    {
        pBmsVoltronic->Field.Module_DisCharge_Curr_State = 0;
    }

    if(sbGetCellChgTempHi())
    {
        pBmsVoltronic->Field.Module_Charge_Temp_State = 0x02;
        pBmsVoltronic->Field.Cell_Charge_Temp_State = 0x02;
    }
    else if(sbGetCellChgTempLo())
    {
        pBmsVoltronic->Field.Module_Charge_Temp_State = 0x01;
        pBmsVoltronic->Field.Cell_Charge_Temp_State = 0x01;
    }
    else
    {
        pBmsVoltronic->Field.Module_Charge_Temp_State = 0;
        pBmsVoltronic->Field.Cell_Charge_Temp_State = 0;
    }
		for (i = 0; i < 10; i++)
    {
        pBmsVoltronic->Field.wReserved4[i] = 0;
    }
    if(sbGetCellDcgTempHi())
    {
        pBmsVoltronic->Field.Module_DisCharge_Temp_State = 0x02;
        pBmsVoltronic->Field.Cell_DisCharge_Temp_State = 0x02;
    }
    else if(sbGetCellDcgTempLo())
    {
        pBmsVoltronic->Field.Module_DisCharge_Temp_State = 0x01;
        pBmsVoltronic->Field.Cell_DisCharge_Temp_State = 0x01;
    }
    else
    {
        pBmsVoltronic->Field.Module_DisCharge_Temp_State = 0;
        pBmsVoltronic->Field.Cell_DisCharge_Temp_State = 0;
    }
		 for (i = 0; i < 6; i++)
    {
        pBmsVoltronic->Field.wReserved5[i] = 0;
    }
    pBmsVoltronic->Field.Charge_Voltage_Limit = wRecChargeVolt/10;
    pBmsVoltronic->Field.Discharge_Voltage_Limit = wRecCutoffVolt/10;
    pBmsVoltronic->Field.Charge_Current_Limit = wRecChargeCurr;
    pBmsVoltronic->Field.DisCharge_Current_Limit = wRecDischargeCurr;

    pBmsVoltronic->Field.BMS_Discharge_Status.bits.Full_Chg_Request = fBmsStatus.bits.FullCharge;
    pBmsVoltronic->Field.BMS_Discharge_Status.bits.Chg_immediately2_Request = fBmsStatus.bits.ChargeImmediately2;
    pBmsVoltronic->Field.BMS_Discharge_Status.bits.Chg_immediately_Request = fBmsStatus.bits.ChargeImmediately1;
    pBmsVoltronic->Field.BMS_Discharge_Status.bits.Discharge_Ena = fBmsStatus.bits.DischargeEnable;
    pBmsVoltronic->Field.BMS_Discharge_Status.bits.Charge_Ena = fBmsStatus.bits.ChargeEnable;
}

// BMS数据更新
void	sBmsDataLocalUpdate(void)
{
	INT16S *p, wTemp;
	INT16U *q;
	INT32U dwTemp;
	INT8U i;
	
	pBmsDataLocal->Field.ChargeVoltageLimit = (wRecChargeVolt + 5) / 10;
	pBmsDataLocal->Field.DischargeVoltageLimit = (wRecCutoffVolt + 5) / 10;
	pBmsDataLocal->Field.ChargeCurrentLimit = wRecChargeCurr;
	pBmsDataLocal->Field.DischargeCurrentLimit = wRecDischargeCurr;
	pBmsDataLocal->Field.StatusHigh = 0;
	pBmsDataLocal->Field.StatusLow = fBmsStatus.data;
	pBmsDataLocal->Field.FaultFlagHigh = 0;
	pBmsDataLocal->Field.FaultFlagLow = swGetBmsFaultFlag();
	pBmsDataLocal->Field.AlarmFlagHigh = 0;
	pBmsDataLocal->Field.AlarmFlagLow = swGetBmsAlarmFlag();
	pBmsDataLocal->Field.NoticeFlagHigh = 0;
	pBmsDataLocal->Field.NoticeFlagLow = swGetBmsNoticeFlag();
	if(fBmsStatus.bits.ChargeEnable || fBmsStatus.bits.DischargeEnable)
	{
		pBmsDataLocal->Field.PackCurrent = swGetBattCurrFiltNew();	// 充电为正
	}
	else
	{
		pBmsDataLocal->Field.PackCurrent = 0;
	}
	pBmsDataLocal->Field.PackVoltage = swGetBattVoltFiltNew();
	pBmsDataLocal->Field.Reservd1 = sbGetSciAddress();
	pBmsDataLocal->Field.Reservd2 = sbGetCanAddress();
	pBmsDataLocal->Field.SOC = swGetSocNew() * 10;
	pBmsDataLocal->Field.SOH = swGetSohNew() * 10;
	dwTemp = (INT32U)swGetBmsNominalAH() * 100;
	pBmsDataLocal->Field.PackCapacityHigh = (INT16U)(dwTemp >> 16);
	pBmsDataLocal->Field.PackCapacityLow = (INT16U)(dwTemp & 0x0000FFFF);
	
	pBmsDataLocal->Field.ParallelNumber = swGetEmsDataParallelNumber();
	pBmsDataLocal->Field.ParallelStatus = swGetEmsDataParallelStatus();
	pBmsDataLocal->Field.SerialNumber = swGetEmsDataSerialNumber();
	pBmsDataLocal->Field.SerialStatus = swGetEmsDataSerialStatus();
	
	pBmsDataLocal->Field.MaximumCellVoltageNo = sbGetMaxCellVoltNo();
	pBmsDataLocal->Field.MaximumCellVoltage = swGetMaxCellVoltNew();
	pBmsDataLocal->Field.MinimumCellVoltageNo = sbGetMinCellVoltNo();
	pBmsDataLocal->Field.MinimumCellVoltage = swGetMinCellVoltNew();
	pBmsDataLocal->Field.MaximumCellTemperatureNo = sbGetMaxCellTempNo();
	pBmsDataLocal->Field.MaximumCellTemperature = (INT16S)swGetMaxCellTempNew() * 10;
	pBmsDataLocal->Field.MinimumCellTemperatureNo = sbGetMinCellTempNo();
	pBmsDataLocal->Field.MinimumCellTemperature = (INT16S)swGetMinCellTempNew() * 10;
	
	q = &pBmsDataLocal->Field.CellVoltage01;
	for(i = 0; i < cADC_CELL_VOLT_MAX; i++)
	{
		if(i < swGetBmsCellVoltNum())
		{
			*q = swGetCellVoltFiltNew(i);
		}
		else
		{
			*q = 0;
		}
		q++;
	}
	
	p = &pBmsDataLocal->Field.TemperatureSensor01;
	for(i = 0; i < cADC_CELL_TEMP_MAX; i++)
	{
		if(i < swGetBmsCellTempNum())
		{
			wTemp = swGetCellTempFiltNew(i);
			if(wTemp != 0x7FFF)
			{
				wTemp = wTemp * 10;
			}
			*p = wTemp;
		}
		else
		{
			*p = 0;
		}
		p++;
	}
	
	pBmsDataLocal->Field.Reservd5 = 0;
	pBmsDataLocal->Field.Reservd6 = 0;
	pBmsDataLocal->Field.Reservd7 = 0;
	pBmsDataLocal->Field.Reservd8 = 0;
	pBmsDataLocal->Field.Reservd9 = 0;
	pBmsDataLocal->Field.Reservd10 = 0;
	pBmsDataLocal->Field.Reservd11 = 0;
	pBmsDataLocal->Field.Reservd12 = 0;
}

// BMS2数据更新
void	sBms2DataLocalUpdate(void)
{
	INT16S *p;
	INT16U *q;
	INT8U i;
	
	pBms2DataLocal->Field.SettingDataSn = 0;
	pBms2DataLocal->Field.BmsMode = bBmsMode;
	pBms2DataLocal->Field.BmsFlag = fBmsStatusOld.data;
	pBms2DataLocal->Field.BmsFaultCode = sbGetBmsFaultCode();
	pBms2DataLocal->Field.BmsFaultFlag = swGetBmsFaultFlag();
	pBms2DataLocal->Field.BmsAlarmFlag = swGetBmsAlarmFlag();
	pBms2DataLocal->Field.BattVolt = swGetBattVoltNew();
	if(fBmsStatus.bits.ChargeEnable || fBmsStatus.bits.DischargeEnable)
	{
		pBms2DataLocal->Field.BattCurr = -swGetBattCurrFiltNew();	// 充电为负
	}
	else
	{
		pBms2DataLocal->Field.BattCurr = 0;
	}
	pBms2DataLocal->Field.Reservd1 = 0;
	pBms2DataLocal->Field.Reservd2 = 0;
	pBms2DataLocal->Field.AmbTemp = swGetHsTempNew();
	pBms2DataLocal->Field.BmsSoc = swGetSocNew();
	pBms2DataLocal->Field.BmsSoh = swGetSohNew();
	pBms2DataLocal->Field.BmsCycleCnt = swGetEepromBattCycCnt();
	pBms2DataLocal->Field.BmsChgAhLo = 0;
	pBms2DataLocal->Field.BmsChgAhHi = 0;
	pBms2DataLocal->Field.BmsChgWhLo = 0;
	pBms2DataLocal->Field.BmsChgWhHi = 0;
	pBms2DataLocal->Field.BmsChgTimeLo = 0;
	pBms2DataLocal->Field.BmsChgTimeHi = 0;
	pBms2DataLocal->Field.BmsDcgAhLo = 0;
	pBms2DataLocal->Field.BmsDcgAhHi = 0;
	pBms2DataLocal->Field.BmsDcgWhLo = 0;
	pBms2DataLocal->Field.BmsDcgWhHi = 0;
	pBms2DataLocal->Field.BmsDcgTimeLo = 0;
	pBms2DataLocal->Field.BmsDcgTimeHi = 0;
	pBms2DataLocal->Field.BmsRunTimeLo = 0;
	pBms2DataLocal->Field.BmsRunTimeHi = 0;
	pBms2DataLocal->Field.ChargeVolt = wRecChargeVolt;
	pBms2DataLocal->Field.CutoffVolt = wRecCutoffVolt;
	pBms2DataLocal->Field.MaxChargeCurr = wRecChargeCurr;
	pBms2DataLocal->Field.MaxDischargeCurr = wRecDischargeCurr;
	pBms2DataLocal->Field.CellVoltMax = swGetMaxCellVoltNew();
	pBms2DataLocal->Field.CellVoltMaxNo = sbGetMaxCellVoltNo();
	pBms2DataLocal->Field.CellVoltMin = swGetMinCellVoltNew();
	pBms2DataLocal->Field.CellVoltMinNo = sbGetMinCellVoltNo();
	pBms2DataLocal->Field.CellTempMax = swGetMaxCellTempNew();
	pBms2DataLocal->Field.CellTempMaxNo = sbGetMaxCellTempNo();
	pBms2DataLocal->Field.CellTempMin = swGetMinCellTempNew();
	pBms2DataLocal->Field.CellTempMinNo = sbGetMinCellTempNo();
	pBms2DataLocal->Field.Reservd3 = 0;
	pBms2DataLocal->Field.Reservd4 = 0;
	q = &pBms2DataLocal->Field.CellVolt01;
	for(i = 0; i < cADC_CELL_VOLT_MAX; i++)
	{
		if(i < swGetBmsCellVoltNum())
		{
			*q = swGetCellVoltFiltNew(i);
		}
		else
		{
			*q = 0;
		}
		q++;
	}
	
	p = &pBms2DataLocal->Field.CellTemp01;
	for(i = 0; i < cADC_CELL_TEMP_MAX; i++)
	{
		if(i < swGetBmsCellTempNum())
		{
			*p = swGetCellTempFiltNew(i);
		}
		else
		{
			*p = 0;
		}
		p++;
	}

	pBms2DataLocal->Field.CellTemp09 = 0;
	pBms2DataLocal->Field.CellTemp10 = 0;
	pBms2DataLocal->Field.CellTemp11 = 0;
	pBms2DataLocal->Field.CellTemp12 = 0;
	pBms2DataLocal->Field.CellTemp13 = 0;
	pBms2DataLocal->Field.CellTemp14 = 0;
	pBms2DataLocal->Field.CellTemp15 = 0;
	pBms2DataLocal->Field.CellTemp16 = 0;
}

// 更新BMS信息数据
void	sBmsInformationDataUpdate(void)
{
	// 设置BMS信息数据字段值
	strBmsInformationData.Field.wType = MACHINE_TYPE;
	strBmsInformationData.Field.wSubType = swGetEepromBattType();
	strBmsInformationData.Field.wCommProVer = swGetEepromVer();
	strBmsInformationData.Field.wCommInfo = 0xFFFF;
	strBmsInformationData.Field.wSerial1 = swGetEepromSerialNo1();
	strBmsInformationData.Field.wSerial2 = swGetEepromSerialNo2();
	strBmsInformationData.Field.wSerial3 = swGetEepromSerialNo3();
	strBmsInformationData.Field.wSerial4 = swGetEepromSerialNo4();
	strBmsInformationData.Field.wSerial5 = swGetEepromSerialNo5();
	strBmsInformationData.Field.wSNLen = swGetEepromSerialNoLen();
	strBmsInformationData.Field.wDispSwVer = 0xFFFF;
	strBmsInformationData.Field.wMCU1SwVer = MACHINE_SW_VERSION;
	strBmsInformationData.Field.wMCU2SwVer = 0xFFFF;
	strBmsInformationData.Field.wDispHwVer = MACHINE_HW_VERSION;
	strBmsInformationData.Field.wCtrlHwVer = 0xFFFF;
	strBmsInformationData.Field.wPowerHwVer = 0xFFFF;
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
INT8U	sbGetBmsParallelShutdown(void)
{
	return(fBmsGlobal.bits.ParallelShutdown);
}

INT32U	GetSystemTime(void)
{
	return(system_time_ms);
}

INT16U	swGetBmsUserParaCnt(void)
{
	return wUserParaCnt;
}

INT16U	swGetBmsCellType(void)
{
	return(strBmsInfo.wCellType);
}

INT16U	swGetBmsTempType(void)
{
	return(strBmsInfo.wTempType);
}

INT16U	swGetBmsNominalVolt(void)
{
	return(strBmsInfo.wBattNominalVolt);
}

INT16U	swGetBmsNominalAH(void)
{
	return(strBmsInfo.wBattNominalAH);
}

INT16U	swGetBmsRatedVolt(void)
{
	return(strBmsInfo.wBattRatedVolt);
}

INT16U	swGetBmsRatedAH(void)
{
	return(strBmsInfo.wBattRatedAH);
}

INT16U	swGetBmsCellVoltNum(void)
{
	return(strBmsInfo.wCellVoltNum);
}

INT16U	swGetBmsCellTempNum(void)
{
	return(strBmsInfo.wCellTempNum);
}

INT16U	swGetBmsBalanceFunc(void)
{
	return(strBmsInfo.wBalanceFunc);
}

INT16U	swGetBmsLedLcdMode(void)
{
	return(strBmsInfo.wLedLcdMode);
}

INT16U	swGetBmsCurrSampType(void)
{
	return(strBmsInfo.wCurrSampType);
}

INT16U	swGetBmsCurrAlarmBack(void)
{
	return(strBmsInfo.wCurrAlarmBack);
}

INT16U	swGetBmsCurrAlarm(void)
{
	return(strBmsInfo.wCurrAlarm);
}

INT16U	swGetBmsCurrHigh1(void)
{
	return(strBmsInfo.wCurrHigh1);
}

INT16U	swGetBmsCurrHigh2(void)
{
	return(strBmsInfo.wCurrHigh2);
}

INT8U	sbGetBmsChargeOver5A(void)
{
	return fBmsGlobal.bits.Charge5A;
}

INT8U	sbGetBmsCurrentStatus(void)
{
	return fBmsGlobal.bits.CurrStatus;
}

INT8U	sbGetBmsPirated(void)
{
	return fBmsGlobal.bits.IsPirated;
}

INT8U	sbGetBmsSoftStartFlag(void)
{
	return fBmsGlobal.bits.bSoftStart;
}

INT8U	sbGetBmsFaultCode(void)
{
	return(bBmsFaultCode);
}

INT16U	swGetBmsFaultFlag(void)
{
	return(wBmsFaultFlag);
}

INT16U	swGetBmsAlarmFlag(void)
{
	return(wBmsAlarmFlag);
}

INT16U	swGetBmsNoticeFlag(void)
{
	return(wBmsNoticeFlag);
}

INT32U	sdwGetBmsRunSec(void)
{
	return dwBmsRunSec;
}

INT16U	swGetBmsStatusOld(void)
{
	return fBmsStatusOld.data;
}

INT16U	swGetRecChargeVolt(void)
{
	return wRecChargeVolt;
}

INT16U  swGetRecCutoffVolt(void)
{
	return wRecCutoffVolt;
}

INT16U  swGetRecChargeCurr(void)
{
	return wRecChargeCurr;
}

INT16U  swGetRecDischargeCurr(void)
{
	return wRecDischargeCurr;
}

INT16U  swGetBmsChargeEnable(void)
{
	return fBmsStatus.bits.ChargeEnable;
}

INT16U  swGetBmsDischargeEnable(void)
{
	return fBmsStatus.bits.DischargeEnable;
}

INT16U  swGetBmsChargeImmediately1(void)
{
	return fBmsStatus.bits.ChargeImmediately1;
}

INT16U  swGetBmsChargeImmediately2(void)
{
	return fBmsStatus.bits.ChargeImmediately2;
}

INT16U  swGetBmsFullCharge(void)
{
	return fBmsStatus.bits.FullCharge;
}

INT16U	swGetBmsDataLocal(INT16U wAddrOffset)
{
	INT16U	*pwTemp;
	
	if(wAddrOffset < cBMS_DATA_FEILD_LEN)
	{
		pwTemp = (INT16U *)pBmsDataLocal;
		pwTemp += wAddrOffset;
		return(*pwTemp);
	}
	else
	{
		return 0xFFFF;
	}
}

//INT16U	swGetGrowatt_BmsDataLocal(INT16U wAddrOffset)
//{
//	INT16U	*pwTemp;
//	
//	if(wAddrOffset < cBMS_Growatt_FEILD_LEN)
//	{
//		pwTemp = (INT16U *)pBmsGrowatt;
//		pwTemp += wAddrOffset;
//		return(*pwTemp);
//	}
//	else
//	{
//		return 0xFFFF;
//	}
//}

INT16U	swGetVoltronic_BmsDataLocal(INT16U wAddrOffset)
{
	INT16U	*pwTemp;
	
	if(wAddrOffset < cBMS_Voltronic_FEILD_LEN)
	{
		pwTemp = (INT16U *)pBmsVoltronic;
		pwTemp += wAddrOffset;
		return(*pwTemp);
	}
	else
	{
		return 0xFFFF;
	}
}

INT16U	swGetBms2DataLocal(INT16U wAddrOffset)
{
	INT16U	*pwTemp;
	
	if(wAddrOffset < cBMS2_DATA_FEILD_LEN)
	{
		pwTemp = (INT16U *)pBms2DataLocal;
		pwTemp += wAddrOffset;
		return(*pwTemp);
	}
	else
	{
		return 0xFFFF;
	}
}

INT16U	swGetBmsData(INT8U bNo, INT16U wAddrOffset)
{
	INT16U	*pwTemp;
	INT16U	wStatus;
	
	if(wAddrOffset < cBMS_DATA_FEILD_LEN)
	{
		wStatus = swGetEmsDataParallelStatus();
		if(CHKBIT(wStatus, bNo))	// 设备存在
		{
			pwTemp = (INT16U *)pBmsData[bNo];
			pwTemp += wAddrOffset;
			return(*pwTemp);
		}
		return 0xFFFF;
	}
	else
	{
		return 0xFFFF;
	}
}

INT16U	swGetBmsInformationData(INT16U wAddrOffset)
{
	INT16U	*pwTemp;
	
	if(wAddrOffset < cBMS_INFORMATION_FEILD_LEN)
	{
		pwTemp = (INT16U *)&strBmsInformationData;
		pwTemp += wAddrOffset;
		return(*pwTemp);
	}
	else
	{
		return 0xFFFF;
	}
}

INT16U	swGetBmsDataChargeVoltageLimit(INT8U bNo)
{
	return pBmsData[bNo]->Field.ChargeVoltageLimit;
}

INT16U  swGetBmsDataDischargeVoltageLimit(INT8U bNo)
{
	return pBmsData[bNo]->Field.DischargeVoltageLimit;
}

INT16U  swGetBmsDataChargeCurrentLimit(INT8U bNo)
{
	return pBmsData[bNo]->Field.ChargeCurrentLimit;
}

INT16U  swGetBmsDataDischargeCurrentLimit(INT8U bNo)
{
	return pBmsData[bNo]->Field.DischargeCurrentLimit;
}

INT16U	swGetBmsDataBmsStatusHigh(INT8U bNo)
{
	return pBmsData[bNo]->Field.StatusHigh;
}

INT16U	swGetBmsDataBmsStatusLow(INT8U bNo)
{
	return pBmsData[bNo]->Field.StatusLow;
}

INT16U	swGetBmsDataFaultFlagHigh(INT8U bNo)
{
	return pBmsData[bNo]->Field.FaultFlagHigh;
}

INT16U	swGetBmsDataFaultFlagLow(INT8U bNo)
{
	return pBmsData[bNo]->Field.FaultFlagLow;
}

INT16U	swGetBmsDataAlarmFlagHigh(INT8U bNo)
{
	return pBmsData[bNo]->Field.AlarmFlagHigh;
}

INT16U	swGetBmsDataAlarmFlagLow(INT8U bNo)
{
	return pBmsData[bNo]->Field.AlarmFlagLow;
}

INT16U	swGetBmsDataNoticeFlagHigh(INT8U bNo)
{
	return pBmsData[bNo]->Field.NoticeFlagHigh;
}

INT16U	swGetBmsDataNoticeFlagLow(INT8U bNo)
{
	return pBmsData[bNo]->Field.NoticeFlagLow;
}

INT16S	swGetBmsDataPackCurrent(INT8U bNo)
{
	return pBmsData[bNo]->Field.PackCurrent;
}

INT16U	swGetBmsDataPackVoltage(INT8U bNo)
{
	return pBmsData[bNo]->Field.PackVoltage;
}

INT16U	swGetBmsDataSOC(INT8U bNo)
{
	return pBmsData[bNo]->Field.SOC;
}

INT16U	swGetBmsDataSOH(INT8U bNo)
{
	return pBmsData[bNo]->Field.SOH;
}

INT16U	swGetBmsDataPackCapacityHigh(INT8U bNo)
{
	return pBmsData[bNo]->Field.PackCapacityHigh;
}

INT16U	swGetBmsDataPackCapacityLow(INT8U bNo)
{
	return pBmsData[bNo]->Field.PackCapacityLow;
}

INT16U	swGetBmsDataMaximumCellVoltageNo(INT8U bNo)
{
	return pBmsData[bNo]->Field.MaximumCellVoltageNo;
}

INT16U	swGetBmsDataMaximumCellVoltage(INT8U bNo)
{
	return pBmsData[bNo]->Field.MaximumCellVoltage;
}

INT16U	swGetBmsDataMinimumCellVoltageNo(INT8U bNo)
{
	return pBmsData[bNo]->Field.MinimumCellVoltageNo;
}

INT16U	swGetBmsDataMinimumCellVoltage(INT8U bNo)
{
	return pBmsData[bNo]->Field.MinimumCellVoltage;
}

INT16U	swGetBmsDataMaximumCellTemperatureNo(INT8U bNo)
{
	return pBmsData[bNo]->Field.MaximumCellTemperatureNo;
}

INT16S	swGetBmsDataMaximumCellTemperature(INT8U bNo)
{
	return pBmsData[bNo]->Field.MaximumCellTemperature;
}

INT16U	swGetBmsDataMinimumCellTemperatureNo(INT8U bNo)
{
	return pBmsData[bNo]->Field.MinimumCellTemperatureNo;
}

INT16S	swGetBmsDataMinimumCellTemperature(INT8U bNo)
{
	return pBmsData[bNo]->Field.MinimumCellTemperature;
}

INT16U	swGetBmsDataParallelNumber(INT8U bNo)
{
	return pBmsData[bNo]->Field.ParallelNumber;
}

INT16U	swGetBmsDataParallelStatus(INT8U bNo)
{
	return pBmsData[bNo]->Field.ParallelStatus;
}

INT16U	swGetBmsDataSerialNumber(INT8U bNo)
{
	return pBmsData[bNo]->Field.SerialNumber;
}

INT16U	swGetBmsDataSerialStatus(INT8U bNo)
{
	return pBmsData[bNo]->Field.SerialStatus;
}

INT16U	swGetBmsDataCellVoltage01(INT8U bNo)
{
	return pBmsData[bNo]->Field.CellVoltage01;
}

INT16U	swGetBmsDataCellVoltage02(INT8U bNo)
{
	return pBmsData[bNo]->Field.CellVoltage02;
}

INT16U	swGetBmsDataCellVoltage03(INT8U bNo)
{
	return pBmsData[bNo]->Field.CellVoltage03;
}

INT16U	swGetBmsDataCellVoltage04(INT8U bNo)
{
	return pBmsData[bNo]->Field.CellVoltage04;
}

INT16U	swGetBmsDataCellVoltage05(INT8U bNo)
{
	return pBmsData[bNo]->Field.CellVoltage05;
}

INT16U	swGetBmsDataCellVoltage06(INT8U bNo)
{
	return pBmsData[bNo]->Field.CellVoltage06;
}

INT16U	swGetBmsDataCellVoltage07(INT8U bNo)
{
	return pBmsData[bNo]->Field.CellVoltage07;
}

INT16U	swGetBmsDataCellVoltage08(INT8U bNo)
{
	return pBmsData[bNo]->Field.CellVoltage08;
}

INT16U	swGetBmsDataCellVoltage09(INT8U bNo)
{
	return pBmsData[bNo]->Field.CellVoltage09;
}

INT16U	swGetBmsDataCellVoltage10(INT8U bNo)
{
	return pBmsData[bNo]->Field.CellVoltage10;
}

INT16U	swGetBmsDataCellVoltage11(INT8U bNo)
{
	return pBmsData[bNo]->Field.CellVoltage11;
}

INT16U	swGetBmsDataCellVoltage12(INT8U bNo)
{
	return pBmsData[bNo]->Field.CellVoltage12;
}

INT16U	swGetBmsDataCellVoltage13(INT8U bNo)
{
	return pBmsData[bNo]->Field.CellVoltage13;
}

INT16U	swGetBmsDataCellVoltage14(INT8U bNo)
{
	return pBmsData[bNo]->Field.CellVoltage14;
}

INT16U	swGetBmsDataCellVoltage15(INT8U bNo)
{
	return pBmsData[bNo]->Field.CellVoltage15;
}

INT16U	swGetBmsDataCellVoltage16(INT8U bNo)
{
	return pBmsData[bNo]->Field.CellVoltage16;
}

INT16U	swGetBmsDataTemperatureSensor01(INT8U bNo)
{
	return pBmsData[bNo]->Field.TemperatureSensor01;
}

INT16U	swGetBmsDataTemperatureSensor02(INT8U bNo)
{
	return pBmsData[bNo]->Field.TemperatureSensor02;
}

INT16U	swGetBmsDataTemperatureSensor03(INT8U bNo)
{
	return pBmsData[bNo]->Field.TemperatureSensor03;
}

INT16U	swGetBmsDataTemperatureSensor04(INT8U bNo)
{
	return pBmsData[bNo]->Field.TemperatureSensor04;
}

INT16U	swGetBmsDataTemperatureSensor05(INT8U bNo)
{
	return pBmsData[bNo]->Field.TemperatureSensor05;
}

INT16U	swGetBmsDataTemperatureSensor06(INT8U bNo)
{
	return pBmsData[bNo]->Field.TemperatureSensor06;
}

INT16U	swGetBmsDataTemperatureSensor07(INT8U bNo)
{
	return pBmsData[bNo]->Field.TemperatureSensor07;
}

INT16U	swGetBmsDataTemperatureSensor08(INT8U bNo)
{
	return pBmsData[bNo]->Field.TemperatureSensor08;
}

INT16U	swGetBmsDataReservd5(INT8U bNo)
{
	return pBmsData[bNo]->Field.Reservd5;
}

INT16U	swGetBmsDataReservd6(INT8U bNo)
{
	return pBmsData[bNo]->Field.Reservd6;
}

INT16U	swGetBmsDataReservd7(INT8U bNo)
{
	return pBmsData[bNo]->Field.Reservd7;
}

INT16U	swGetBmsDataReservd8(INT8U bNo)
{
	return pBmsData[bNo]->Field.Reservd8;
}

INT16U	swGetChargeImmediately1(  void)
{
	return fBmsStatus.bits.ChargeImmediately1;
}

INT16U	swGetChargeImmediately2(  void)
{
	return fBmsStatus.bits.ChargeImmediately2;
}

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
void	sSetBmsLocalPointer(INT8U bAddr)
{
	pBmsDataLocal = &strBmsData[bAddr];
}

void	sSetBmsParallelShutdown(INT8U bStatus)
{
	fBmsGlobal.bits.ParallelShutdown = bStatus;
}

void	sSetBmsUserParaCnt(INT16U wCnt)
{
	wUserParaCnt = wCnt;
}

void	sSetBmsCellVoltNum(INT16U wNum)
{
	strBmsInfo.wCellVoltNum = wNum;
}

void	sSetBmsCellTempNum(INT16U wNum)
{
	strBmsInfo.wCellTempNum = wNum;
}

void	sSetBmsBalanceFunc(INT16U wStatus)
{
	strBmsInfo.wBalanceFunc = wStatus;
}

void	sSetBmsLedLcdMode(INT16U wMode)
{
	strBmsInfo.wLedLcdMode = wMode;
}

void	sSetBmsCurrSampType(INT16U wStatus)
{
	strBmsInfo.wCurrSampType = wStatus;
}

void	sSetBmsManagerPirated(INT8U bStatus)
{
	fBmsGlobal.bits.IsPirated = bStatus;
}

void	sSetOpSoftFinishedFlag(INT8U bFlag)
{
	fBmsGlobal.bits.bSoftStart = bFlag;
}

void	sRstBmsTimeStoreMin(void)
{
	wBmsTimeStoreMin = cBMS_TIME_STROE_MIN;
}

void	sSetBmsFaultCode(INT8U bCode)
{
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();
	bBmsFaultCode = bCode;
	OS_EXIT_CRITICAL();
}

void	sSetBmsFaultCodeIsr(INT8U bCode)
{
	bBmsFaultCode = bCode;
}

void	sClrBmsFaultCode(INT8U bCode)
{
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();
	if((bCode == 0xFF) || (bCode == bBmsFaultCode))
	{
		bBmsFaultCode = 0;
	}
	OS_EXIT_CRITICAL();
}

void	sSetBmsFaultFlag(INT16U wFlag)
{
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();
	wBmsFaultFlag |= wFlag;
	OS_EXIT_CRITICAL();
}

void	sSetBmsFaultFlagIsr(INT16U wFlag)
{
	wBmsFaultFlag |= wFlag;
}

void	sClrBmsFaultFlag(INT16U wFlag)
{
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();
	wBmsFaultFlag &= ~wFlag;
	OS_EXIT_CRITICAL();
}

void	sSetBmsAlarmFlag(INT16U wFlag)
{
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();
	wBmsAlarmFlag |= wFlag;
	OS_EXIT_CRITICAL();
}

void	sSetBmsAlarmFlagIsr(INT16U wFlag)
{
	wBmsAlarmFlag |= wFlag;
}

void	sClrBmsAlarmFlag(INT16U wFlag)
{
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();
	wBmsAlarmFlag &= ~wFlag;
	OS_EXIT_CRITICAL();
}

void	sSetBmsNoticeFlag(INT16U wFlag)
{
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();
	wBmsNoticeFlag |= wFlag;
	OS_EXIT_CRITICAL();
}

void	sSetBmsNoticeFlagIsr(INT16U wFlag)
{
	wBmsNoticeFlag |= wFlag;
}

void	sClrBmsNoticeFlag(INT16U wFlag)
{
	OS_CPU_SR  cpu_sr;
	OS_ENTER_CRITICAL();
	wBmsNoticeFlag &= ~wFlag;
	OS_EXIT_CRITICAL();
}

void	sSetBmsCurrentStatus(INT8U bStatus)
{
	fBmsGlobal.bits.CurrStatus = bStatus;
}

void	sRstCommLossCount(void)
{
	wCommLossCount = wCommLossCountLimit;
}

void	sSetSoftStartMode(INT8U bMode)
{
	bSoftStartMode = bMode;
}

void	sSetBmsDataChargeVoltageLimit(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.ChargeVoltageLimit = wData;
}

void	sSetBmsDataDischargeVoltageLimit(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.DischargeVoltageLimit = wData;
}

void	sSetBmsDataChargeCurrentLimit(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.ChargeCurrentLimit = wData;
}

void	sSetBmsDataDischargeCurrentLimit(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.DischargeCurrentLimit = wData;
}

void	sSetBmsDataBmsStatusHigh(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.StatusHigh = wData;
}

void	sSetBmsDataBmsStatusLow(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.StatusLow = wData;
}

void	sSetBmsDataFaultFlagHigh(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.FaultFlagHigh = wData;
}

void	sSetBmsDataFaultFlagLow(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.FaultFlagLow = wData;
}

void	sSetBmsDataAlarmFlagHigh(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.AlarmFlagHigh = wData;
}

void	sSetBmsDataAlarmFlagLow(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.AlarmFlagLow = wData;
}

void	sSetBmsDataNoticeFlagHigh(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.NoticeFlagHigh = wData;
}

void	sSetBmsDataNoticeFlagLow(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.NoticeFlagLow = wData;
}

void	sSetBmsDataPackCurrent(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.PackCurrent = wData;
}

void	sSetBmsDataPackVoltage(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.PackVoltage = wData;
}

void	sSetBmsDataSOC(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.SOC = wData;
}

void	sSetBmsDataSOH(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.SOH = wData;
}

void	sSetBmsDataPackCapacityHigh(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.PackCapacityHigh = wData;
}

void	sSetBmsDataPackCapacityLow(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.PackCapacityLow = wData;
}

void	sSetBmsDataMaximumCellVoltageNo(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.MaximumCellVoltageNo = wData;
}

void	sSetBmsDataMaximumCellVoltage(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.MaximumCellVoltage = wData;
}

void	sSetBmsDataMinimumCellVoltageNo(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.MinimumCellVoltageNo = wData;
}

void	sSetBmsDataMinimumCellVoltage(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.MinimumCellVoltage = wData;
}

void	sSetBmsDataMaximumCellTemperatureNo(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.MaximumCellTemperatureNo = wData;
}

void	sSetBmsDataMaximumCellTemperature(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.MaximumCellTemperature = wData;
}

void	sSetBmsDataMinimumCellTemperatureNo(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.MinimumCellTemperatureNo = wData;
}

void	sSetBmsDataMinimumCellTemperature(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.MinimumCellTemperature = wData;
}

void	sSetBmsDataParallelNumber(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.ParallelNumber = wData;
}

void	sSetBmsDataParallelStatus(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.ParallelStatus = wData;
}

void	sSetBmsDataSerialNumber(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.SerialNumber = wData;
}

void	sSetBmsDataSerialStatus(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.SerialStatus = wData;
}

void	sSetBmsDataCellVoltage01(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.CellVoltage01 = wData;
}

void	sSetBmsDataCellVoltage02(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.CellVoltage02 = wData;
}

void	sSetBmsDataCellVoltage03(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.CellVoltage03 = wData;
}

void	sSetBmsDataCellVoltage04(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.CellVoltage04 = wData;
}

void	sSetBmsDataCellVoltage05(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.CellVoltage05 = wData;
}

void	sSetBmsDataCellVoltage06(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.CellVoltage06 = wData;
}

void	sSetBmsDataCellVoltage07(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.CellVoltage07 = wData;
}

void	sSetBmsDataCellVoltage08(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.CellVoltage08 = wData;
}

void	sSetBmsDataCellVoltage09(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.CellVoltage09 = wData;
}

void	sSetBmsDataCellVoltage10(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.CellVoltage10 = wData;
}

void	sSetBmsDataCellVoltage11(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.CellVoltage11 = wData;
}

void	sSetBmsDataCellVoltage12(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.CellVoltage12 = wData;
}

void	sSetBmsDataCellVoltage13(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.CellVoltage13 = wData;
}

void	sSetBmsDataCellVoltage14(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.CellVoltage14 = wData;
}

void	sSetBmsDataCellVoltage15(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.CellVoltage15 = wData;
}

void	sSetBmsDataCellVoltage16(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.CellVoltage16 = wData;
}

void	sSetBmsDataTemperatureSensor01(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.TemperatureSensor01 = wData;
}

void	sSetBmsDataTemperatureSensor02(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.TemperatureSensor02 = wData;
}

void	sSetBmsDataTemperatureSensor03(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.TemperatureSensor03 = wData;
}

void	sSetBmsDataTemperatureSensor04(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.TemperatureSensor04 = wData;
}

void	sSetBmsDataTemperatureSensor05(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.TemperatureSensor05 = wData;
}

void	sSetBmsDataTemperatureSensor06(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.TemperatureSensor06 = wData;
}

void	sSetBmsDataTemperatureSensor07(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.TemperatureSensor07 = wData;
}

void	sSetBmsDataTemperatureSensor08(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.TemperatureSensor08 = wData;
}

void	sSetBmsDataReservd5(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.Reservd5 = wData;
}

void	sSetBmsDataReservd6(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.Reservd6 = wData;
}

void	sSetBmsDataReservd7(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.Reservd7 = wData;
}

void	sSetBmsDataReservd8(INT8U bNo, INT16U wData)
{
	pBmsData[bNo]->Field.Reservd8 = wData;
}
INT16S GetBattCurr1(void)
{
	return pBms2DataLocal->Field.BattCurr;
}
