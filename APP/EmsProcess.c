/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		EmsProcess.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define	__EMS_PROCESS_C__

/********************************************************************************
* Include head files                                                            *
********************************************************************************/
#include	"EmsProcess.h"
#include	"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define		cEMS_STRATEGY_NORMAL		0	// 普通的
#define		cEMS_STRATEGY_CONSERVED		1	// 保守的
#define		cEMS_STRATEGY_RADICAL		2	// 激进的

#define		cBMS_STATUS_BIT_NUM			16

/********************************************************************************
* Constants																		*
********************************************************************************/


/********************************************************************************
* External variables															*
********************************************************************************/
extern union BMS_DATA_TABLE strBmsData[];

/********************************************************************************
* External routine prototypes													*
********************************************************************************/


/********************************************************************************
* Input variables																*
********************************************************************************/


/********************************************************************************
* Output variables																*
********************************************************************************/
union BMS_STATUS_TABLE fEmsStatus;
union EMS_GLOBAL_STATUS_TABLE
{
	struct EMS_GLOBAL_STATUS_BIT_FEILD
	{
		// BIT0-3
		INT16U rsvd1:4;
		
		// BIT4-7
		INT16U CurrStatus:2;
		INT16U rsvd2:2;
		
		// BIT8-11
		INT16U rsvd3:4;
		
		// BIT12-15
		INT16U rsvd4:4;
	} bits;
	INT16U data;
} fEmsGlobal;

/********************************************************************************
* Internal variables															*
********************************************************************************/
union EMS_DATA_TABLE strEmsData;
union EMS_DATA_TABLE strEmsDataTemp;
union EMS_DATA_TABLE *pEmsData;

struct	EMS_BMS_DATA_ACC_FEILD
{
	INT32U ChargeVoltageLimitAcc;	// 0.1V
	INT32U DischargeVoltageLimitAcc;// 0.1V
	INT32U ChargeCurrentLimit;		// 0.1A
	INT32U DischargeCurrentLimit;	// 0.1A
	INT32S TotalCurrentAcc;
	INT32U TotalVoltageAcc;
	INT32U TotalSOCAcc;
	INT32U TotalSOHAcc;
	INT32U TotalCapacityAcc;
};
struct	EMS_BMS_DATA_ACC_FEILD strEmsBmsDataAcc;
INT8U	bEmsStrategyMethod;
INT8U	bBmsStatusLoCnt[cBMS_STATUS_BIT_NUM];

union BMS_STATUS_TABLE strBmsStatusTemp;
INT8U	bEmsIdleNum;
INT8U	bEmsChargingNum;
INT8U	bEmsDischargingNum;
INT8U	bEmsSocFullNum;
INT8U	bEmsSocEmptyNum;
INT8U	bEmsIdleNumTemp;
INT8U	bEmsChargingNumTemp;
INT8U	bEmsDischargingNumTemp;
INT8U	bEmsSocFullNumTemp;
INT8U	bEmsSocEmptyNumTemp;

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/


/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void	sEmsInitial(void)
{
	fEmsStatus.data = 0;
	fEmsGlobal.data = 0;
	pEmsData = &strEmsData;
	
	bEmsStrategyMethod = cEMS_STRATEGY_NORMAL;
	
	bEmsIdleNum = 0;
	bEmsChargingNum = 0;
	bEmsDischargingNum = 0;
	bEmsSocFullNum = 0;
	bEmsSocEmptyNum = 0;
}

void	sEmsDataUpdate(void)
{
	INT8U i, j, num, chknum;
	INT16U	*ptr1, *ptr2;
	INT16U wTemp;
	
	for(j = 0; j < cBMS_STATUS_BIT_NUM; j++)
	{
		bBmsStatusLoCnt[j] = 0;
	}
	
	strEmsBmsDataAcc.ChargeVoltageLimitAcc = 0;
	strEmsBmsDataAcc.DischargeVoltageLimitAcc = 0;
	strEmsBmsDataAcc.ChargeCurrentLimit = 0;
	strEmsBmsDataAcc.DischargeCurrentLimit = 0;
	strEmsBmsDataAcc.TotalCurrentAcc = 0;
	strEmsBmsDataAcc.TotalVoltageAcc = 0;
	strEmsBmsDataAcc.TotalSOCAcc = 0;
	strEmsBmsDataAcc.TotalSOHAcc = 0;
	strEmsBmsDataAcc.TotalCapacityAcc = 0;
	
	// 以下参数需要做统计,所以需要赋初始值
	strEmsDataTemp.Field.ChargeCurrentLimit = 0xFFFF;
	strEmsDataTemp.Field.DischargeCurrentLimit = 0xFFFF;
	strEmsDataTemp.Field.FaultFlagHigh = 0;
	strEmsDataTemp.Field.FaultFlagLow = 0;
	strEmsDataTemp.Field.AlarmFlagHigh = 0;
	strEmsDataTemp.Field.AlarmFlagLow = 0;
	strEmsDataTemp.Field.NoticeFlagHigh = 0;
	strEmsDataTemp.Field.NoticeFlagLow = 0;
	strEmsDataTemp.Field.MaximumCellVoltageNo = 0;
	strEmsDataTemp.Field.MaximumCellVoltage = 0;
	strEmsDataTemp.Field.MinimumCellVoltageNo = 0xFFFF;
	strEmsDataTemp.Field.MinimumCellVoltage = 0xFFFF;
	strEmsDataTemp.Field.MaximumCellTemperatureNo = 0;
	strEmsDataTemp.Field.MaximumCellTemperature = 0;
	strEmsDataTemp.Field.MinimumCellTemperatureNo = 0xFFFF;
	strEmsDataTemp.Field.MinimumCellTemperature = 0xFFFF;
	
	bEmsIdleNumTemp = 0;
	bEmsChargingNumTemp = 0;
	bEmsDischargingNumTemp = 0;
	bEmsSocFullNumTemp = 0;
	bEmsSocEmptyNumTemp = 0;
	
	for(i = 0, num = 0; i < BMS_PARALLEL_NUM_MAX; i++)
	{
		if(CHKBIT(sdwGetCanParallelFlag(), i) || (sbGetCanAddress() == i))		//(sbGetCanAddress() == 0)
		{
			num++;
			
			wTemp = strBmsData[i].Field.StatusLow;
			for(j = 0; j < cBMS_STATUS_BIT_NUM; j++)
			{
				if(CHKBIT(wTemp,j))
				{
					bBmsStatusLoCnt[j]++;
				}
			}
			
			strBmsStatusTemp.data = strBmsData[i].Field.StatusLow;
			if(strBmsStatusTemp.bits.Charging == true)
			{
				bEmsChargingNumTemp++;
			}
			else if(strBmsStatusTemp.bits.Discharging == true)
			{
				bEmsDischargingNumTemp++;
			}
			else
			{
				bEmsIdleNumTemp++;
			}
			if(strBmsData[i].Field.SOC == 100)
			{
				bEmsSocFullNumTemp++;
			}
			else if(strBmsData[i].Field.SOC == 0)
			{
				bEmsSocEmptyNumTemp++;
			}
			
			// 限制电压计算平均值
			strEmsBmsDataAcc.ChargeVoltageLimitAcc += strBmsData[i].Field.ChargeVoltageLimit;
			strEmsBmsDataAcc.DischargeVoltageLimitAcc += strBmsData[i].Field.DischargeVoltageLimit;
			
			// 限制电流计算累加值
			strEmsBmsDataAcc.ChargeCurrentLimit += strBmsData[i].Field.ChargeCurrentLimit;
			strEmsBmsDataAcc.DischargeCurrentLimit += strBmsData[i].Field.DischargeCurrentLimit;
			
			// 实时电流取累加值
			strEmsBmsDataAcc.TotalCurrentAcc += strBmsData[i].Field.PackCurrent;
			strEmsBmsDataAcc.TotalVoltageAcc += strBmsData[i].Field.PackVoltage;
			strEmsBmsDataAcc.TotalSOCAcc += strBmsData[i].Field.SOC;
			strEmsBmsDataAcc.TotalSOHAcc += strBmsData[i].Field.SOH;
			strEmsBmsDataAcc.TotalCapacityAcc += ((INT32U)strBmsData[i].Field.PackCapacityHigh << 16);
			strEmsBmsDataAcc.TotalCapacityAcc += strBmsData[i].Field.PackCapacityLow;
			
			// 
			strEmsDataTemp.Field.FaultFlagHigh |= strBmsData[i].Field.FaultFlagHigh;
			strEmsDataTemp.Field.FaultFlagLow |= strBmsData[i].Field.FaultFlagLow;
			strEmsDataTemp.Field.AlarmFlagHigh |= strBmsData[i].Field.AlarmFlagHigh;
			strEmsDataTemp.Field.AlarmFlagLow |= strBmsData[i].Field.AlarmFlagLow;
			strEmsDataTemp.Field.NoticeFlagHigh |= strBmsData[i].Field.NoticeFlagHigh;
			strEmsDataTemp.Field.NoticeFlagLow |= strBmsData[i].Field.NoticeFlagLow;
			
			// 
			if(strEmsDataTemp.Field.MaximumCellVoltage < strBmsData[i].Field.MaximumCellVoltage || num == 1)
			{
				strEmsDataTemp.Field.MaximumCellVoltage = strBmsData[i].Field.MaximumCellVoltage;
				strEmsDataTemp.Field.MaximumCellVoltageNo = strBmsData[i].Field.MaximumCellVoltageNo;
				strEmsDataTemp.Field.MaximumCellVoltageNo += ((INT16U)i << 8);	// 模组编号
			}
			if(strEmsDataTemp.Field.MinimumCellVoltage > strBmsData[i].Field.MinimumCellVoltage || num == 1)
			{
				strEmsDataTemp.Field.MinimumCellVoltage = strBmsData[i].Field.MinimumCellVoltage;
				strEmsDataTemp.Field.MinimumCellVoltageNo = strBmsData[i].Field.MinimumCellVoltageNo;
				strEmsDataTemp.Field.MinimumCellVoltageNo += ((INT16U)i << 8);	// 模组编号
			}
			
			// 
			if(strEmsDataTemp.Field.MaximumCellTemperature < strBmsData[i].Field.MaximumCellTemperature || num == 1)
			{
				strEmsDataTemp.Field.MaximumCellTemperature = strBmsData[i].Field.MaximumCellTemperature;
				strEmsDataTemp.Field.MaximumCellTemperatureNo = strBmsData[i].Field.MaximumCellTemperatureNo;
				strEmsDataTemp.Field.MaximumCellTemperatureNo += ((INT16U)i << 8);	// 模组编号
			}
			if(strEmsDataTemp.Field.MinimumCellTemperature > strBmsData[i].Field.MinimumCellTemperature || num == 1)
			{
				strEmsDataTemp.Field.MinimumCellTemperature = strBmsData[i].Field.MinimumCellTemperature;
				strEmsDataTemp.Field.MinimumCellTemperatureNo = strBmsData[i].Field.MinimumCellTemperatureNo;
				strEmsDataTemp.Field.MinimumCellTemperatureNo += ((INT16U)i << 8);	// 模组编号
			}
		}
		
		if(sbGetCanAddress() == 0)
		{
			break;
		}
	}
	
	// 充放电标志可以使用不同策略
	// 保守的则有一台禁止则全部禁止
	// 普通的则有一半禁止则全部禁止
	// 激进的则有一台允许则全部允许
	if(num != 0)
	{
		if(bEmsStrategyMethod == cEMS_STRATEGY_CONSERVED)
		{
			chknum = num;
		}
		else if(bEmsStrategyMethod == cEMS_STRATEGY_RADICAL)
		{
			chknum = 1;
		}
		else if(num <= 2)
		{
			chknum = 1;
		}
		else
		{
			chknum = (num + 1) >> 1;
		}
		
		// BMS运行状态
		fEmsStatus.data = 0;
		for(j = 0; j < cBMS_STATUS_BIT_NUM; j++)
		{
			if(GEQ(bBmsStatusLoCnt[j],chknum))
			{
				SETBIT(fEmsStatus.data,j);
			}
		}
		// 充放电状态使用电流来判断
		if(fEmsGlobal.bits.CurrStatus == cBMS_CURR_CHG)
		{
			fEmsStatus.bits.Charging = true;
			fEmsStatus.bits.Discharging = false;
		}
		else if(fEmsGlobal.bits.CurrStatus == cBMS_CURR_DSG)
		{
			fEmsStatus.bits.Charging = false;
			fEmsStatus.bits.Discharging = true;
		}
		else
		{
			fEmsStatus.bits.Charging = false;
			fEmsStatus.bits.Discharging = false;
		}
		strEmsDataTemp.Field.StatusHigh = 0;
		strEmsDataTemp.Field.StatusLow = fEmsStatus.data;
		
		// 充电放电限制
		strEmsDataTemp.Field.ChargeVoltageLimit = (strEmsBmsDataAcc.ChargeVoltageLimitAcc / num);
		strEmsDataTemp.Field.DischargeVoltageLimit = (strEmsBmsDataAcc.DischargeVoltageLimitAcc / num);
		strEmsDataTemp.Field.ChargeCurrentLimit = strEmsBmsDataAcc.ChargeCurrentLimit;
		strEmsDataTemp.Field.DischargeCurrentLimit = strEmsBmsDataAcc.DischargeCurrentLimit;
		
		// 实时数据
		strEmsDataTemp.Field.TotalCurrent = strEmsBmsDataAcc.TotalCurrentAcc;
		strEmsDataTemp.Field.TotalVoltage = ((strEmsBmsDataAcc.TotalVoltageAcc / num) + 5) / 10;
		strEmsDataTemp.Field.TotalSOC = strEmsBmsDataAcc.TotalSOCAcc / num;
		strEmsDataTemp.Field.TotalSOH = strEmsBmsDataAcc.TotalSOHAcc / num;
		strEmsDataTemp.Field.TotalCapacityHigh = (strEmsBmsDataAcc.TotalCapacityAcc) >> 16;
		strEmsDataTemp.Field.TotalCapacityLow = (strEmsBmsDataAcc.TotalCapacityAcc) & 0x0000FFFF;
		
		strEmsDataTemp.Field.ParallelNumber = num;
		strEmsDataTemp.Field.ParallelStatus = (INT16U)(sdwGetCanParallelFlag() & 0x0000FFFF);
		strEmsDataTemp.Field.SerialNumber = 0;
		strEmsDataTemp.Field.SerialStatus = (INT16U)(sdwGetCanParallelFlag() >> 16);
		
		// 更新
		ptr1 = (INT16U *)&strEmsData;
		ptr2 = (INT16U *)&strEmsDataTemp;
		for(i = 0; i < cEMS_DATA_FEILD_LEN; i++)
		{
			*ptr1 = *ptr2;
			ptr1++;
			ptr2++;
		}
	}
	
	bEmsIdleNum = bEmsIdleNumTemp;
	bEmsChargingNum = bEmsChargingNumTemp;
	bEmsDischargingNum = bEmsDischargingNumTemp;
	bEmsSocFullNum = bEmsSocFullNumTemp;
	bEmsSocEmptyNum = bEmsSocEmptyNumTemp;
}

void	sEmsCurrentStatusChk(INT16U wFilter1, INT16U wFilter2)
{
	static  INT8U   bIdleChkCnt = 0;
	static  INT8U   bDsgChkCnt = 0;
	static  INT8U   bChgChkCnt = 0;
	INT16S wCurr;
	
	wCurr = strEmsDataTemp.Field.TotalCurrent;
	if(fEmsGlobal.bits.CurrStatus == cBMS_CURR_DSG)
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
	}
	else if(fEmsGlobal.bits.CurrStatus == cBMS_CURR_CHG)
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
	
	if(bDsgChkCnt >= wFilter1)
	{
		bDsgChkCnt = 0;
		fEmsGlobal.bits.CurrStatus = cBMS_CURR_DSG;
	}
	else if(bChgChkCnt >= wFilter1)
	{
		bChgChkCnt = 0;
		fEmsGlobal.bits.CurrStatus = cBMS_CURR_CHG;
	}
	else if(bIdleChkCnt >= wFilter2)
	{
		bIdleChkCnt = 0;
		fEmsGlobal.bits.CurrStatus = cBMS_CURR_IDLE;
	}
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
INT16U	swGetEmsData(INT16U wAddrOffset)
{
	INT16U	*pwTemp;
	
	if(wAddrOffset < cEMS_DATA_FEILD_LEN)
	{
		pwTemp = (INT16U *)pEmsData;
		pwTemp += wAddrOffset;
		return(*pwTemp);
	}
	else
	{
		return 0xFFFF;
	}
}

INT16U	swGetEmsStatusChargeEnable(void)
{
	return fEmsStatus.bits.ChargeEnable;
}

INT16U	swGetEmsStatusDischargeEnable(void)
{
	return fEmsStatus.bits.DischargeEnable;
}

INT16U	swGetEmsStatusFullCharge(void)
{
	return fEmsStatus.bits.FullCharge;
}

INT16U	swGetEmsStatusChargeImmediately1(void)
{
	return fEmsStatus.bits.ChargeImmediately1;
}

INT16U	swGetEmsStatusCharging(void)
{
	return fEmsStatus.bits.Charging;
}

INT16U	swGetEmsStatusDischarging(void)
{
	return fEmsStatus.bits.Discharging;
}

INT16U	swGetEmsDataChargeVoltageLimit(void)
{
	return pEmsData->Field.ChargeVoltageLimit;
}

INT16U	swGetEmsDataDischargeVoltageLimit(void)
{
	return pEmsData->Field.DischargeVoltageLimit;
}

INT16U	swGetEmsDataChargeCurrentLimit(void)
{
	return pEmsData->Field.ChargeCurrentLimit;
}

INT16U	swGetEmsDataDischargeCurrentLimit(void)
{
	return pEmsData->Field.DischargeCurrentLimit;
}

INT16U	swGetEmsDataStatusHigh(void)
{
	return pEmsData->Field.StatusHigh;
}

INT16U	swGetEmsDataStatusLow(void)
{
	return pEmsData->Field.StatusLow;
}

INT16U	swGetEmsDataFaultFlagHigh(void)
{
	return pEmsData->Field.FaultFlagHigh;
}

INT16U	swGetEmsDataFaultFlagLow(void)
{
	return pEmsData->Field.FaultFlagLow;
}

INT16U	swGetEmsDataAlarmFlagHigh(void)
{
	return pEmsData->Field.AlarmFlagHigh;
}

INT16U	swGetEmsDataAlarmFlagLow(void)
{
	return pEmsData->Field.AlarmFlagLow;
}

INT16U	swGetEmsDataNoticeFlagHigh(void)
{
	return pEmsData->Field.NoticeFlagHigh;
}

INT16U	swGetEmsDataNoticeFlagLow(void)
{
	return pEmsData->Field.NoticeFlagLow;
}

INT16S	swGetEmsDataTotalCurrent(void)
{
	return pEmsData->Field.TotalCurrent;
}

INT16U	swGetEmsDataTotalVoltage(void)
{
	return pEmsData->Field.TotalVoltage;
}

INT16U	swGetEmsDataReservd01(void)
{
	return pEmsData->Field.Reservd01;
}

INT16U	swGetEmsDataReservd02(void)
{
	return pEmsData->Field.Reservd02;
}

INT16U	swGetEmsDataTotalSOC(void)
{
	return pEmsData->Field.TotalSOC;
}

INT16U	swGetEmsDataTotalSOH(void)
{
	return pEmsData->Field.TotalSOH;
}

INT16U	swGetEmsDataTotalCapacityHigh(void)
{
	return pEmsData->Field.TotalCapacityHigh;
}

INT16U	swGetEmsDataTotalCapacityLow(void)
{
	return pEmsData->Field.TotalCapacityLow;
}

INT16U	swGetEmsDataMaximumCellVoltageNo(void)
{
	return pEmsData->Field.MaximumCellVoltageNo;
}

INT16U	swGetEmsDataMaximumCellVoltage(void)
{
	return pEmsData->Field.MaximumCellVoltage;
}

INT16U	swGetEmsDataMinimumCellVoltageNo(void)
{
	return pEmsData->Field.MinimumCellVoltageNo;
}

INT16U	swGetEmsDataMinimumCellVoltage(void)
{
	return pEmsData->Field.MinimumCellVoltage;
}

INT16U	swGetEmsDataMaximumCellTemperatureNo(void)
{
	return pEmsData->Field.MaximumCellTemperatureNo;
}

INT16S	swGetEmsDataMaximumCellTemperature(void)
{
	return pEmsData->Field.MaximumCellTemperature;
}

INT16U	swGetEmsDataMinimumCellTemperatureNo(void)
{
	return pEmsData->Field.MinimumCellTemperatureNo;
}

INT16S	swGetEmsDataMinimumCellTemperature(void)
{
	return pEmsData->Field.MinimumCellTemperature;
}

INT16U	swGetEmsDataParallelNumber(void)
{
	return pEmsData->Field.ParallelNumber;
}

INT16U	swGetEmsDataParallelStatus(void)
{
	return pEmsData->Field.ParallelStatus;
}

INT16U	swGetEmsDataSerialNumber(void)
{
	return pEmsData->Field.SerialNumber;
}

INT16U	swGetEmsDataSerialStatus(void)
{
	return pEmsData->Field.SerialStatus;
}

INT8U	sbGetEmsIdleNumber(void)
{
	return bEmsIdleNum;
}

INT8U	sbGetEmsChargingNumber(void)
{
	return bEmsChargingNum;
}

INT8U	sbGetEmsDischargingNumber(void)
{
	return bEmsDischargingNum;
}

INT8U	sbGetEmsSocFullNumber(void)
{
	return bEmsSocFullNum;
}

INT8U	sbGetEmsSocEmptyNumber(void)
{
	return bEmsSocEmptyNum;
}

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
void	sSetEmsDataChargeVoltageLimit(INT16U wData)
{
	pEmsData->Field.ChargeVoltageLimit = wData;
}

void	sSetEmsDataDischargeVoltageLimit(INT16U wData)
{
	pEmsData->Field.DischargeVoltageLimit = wData;
}

void	sSetEmsDataChargeCurrentLimit(INT16U wData)
{
	pEmsData->Field.ChargeCurrentLimit = wData;
}

void	sSetEmsDataDischargeCurrentLimit(INT16U wData)
{
	pEmsData->Field.DischargeCurrentLimit = wData;
}

void	sSetEmsDataStatusHigh(INT16U wData)
{
	pEmsData->Field.StatusHigh = wData;
}

void	sSetEmsDataStatusLow(INT16U wData)
{
	pEmsData->Field.StatusLow = wData;
}

void	sSetEmsDataFaultFlagHigh(INT16U wData)
{
	pEmsData->Field.FaultFlagHigh = wData;
}

void	sSetEmsDataFaultFlagLow(INT16U wData)
{
	pEmsData->Field.FaultFlagLow = wData;
}

void	sSetEmsDataAlarmFlagHigh(INT16U wData)
{
	pEmsData->Field.AlarmFlagHigh = wData;
}

void	sSetEmsDataAlarmFlagLow(INT16U wData)
{
	pEmsData->Field.AlarmFlagLow = wData;
}

void	sSetEmsDataNoticeFlagHigh(INT16U wData)
{
	pEmsData->Field.NoticeFlagHigh = wData;
}

void	sSetEmsDataNoticeFlagLow(INT16U wData)
{
	pEmsData->Field.NoticeFlagLow = wData;
}

void	sSetEmsDataTotalCurrent(INT16U wData)
{
	pEmsData->Field.TotalCurrent = wData;
}

void	sSetEmsDataTotalVoltage(INT16U wData)
{
	pEmsData->Field.TotalVoltage = wData;
}

void	sSetEmsDataReservd01(INT16U wData)
{
	pEmsData->Field.Reservd01 = wData;
}

void	sSetEmsDataReservd02(INT16U wData)
{
	pEmsData->Field.Reservd02 = wData;
}

void	sSetEmsDataTotalSOC(INT16U wData)
{
	pEmsData->Field.TotalSOC = wData;
}

void	sSetEmsDataTotalSOH(INT16U wData)
{
	pEmsData->Field.TotalSOH = wData;
}

void	sSetEmsDataTotalCapacityHigh(INT16U wData)
{
	pEmsData->Field.TotalCapacityHigh = wData;
}

void	sSetEmsDataTotalCapacityLow(INT16U wData)
{
	pEmsData->Field.TotalCapacityLow = wData;
}

void	sSetEmsDataMaximumCellVoltageNo(INT16U wData)
{
	pEmsData->Field.MaximumCellVoltageNo = wData;
}

void	sSetEmsDataMaximumCellVoltage(INT16U wData)
{
	pEmsData->Field.MaximumCellVoltage = wData;
}

void	sSetEmsDataMinimumCellVoltageNo(INT16U wData)
{
	pEmsData->Field.MinimumCellVoltageNo = wData;
}

void	sSetEmsDataMinimumCellVoltage(INT16U wData)
{
	pEmsData->Field.MinimumCellVoltage = wData;
}

void	sSetEmsDataMaximumCellTemperatureNo(INT16U wData)
{
	pEmsData->Field.MaximumCellTemperatureNo = wData;
}

void	sSetEmsDataMaximumCellTemperature(INT16U wData)
{
	pEmsData->Field.MaximumCellTemperature = wData;
}

void	sSetEmsDataMinimumCellTemperatureNo(INT16U wData)
{
	pEmsData->Field.MinimumCellTemperatureNo = wData;
}

void	sSetEmsDataMinimumCellTemperature(INT16U wData)
{
	pEmsData->Field.MinimumCellTemperature = wData;
}

void	sSetEmsDataParallelNumber(INT16U wData)
{
	pEmsData->Field.ParallelNumber = wData;
}

void	sSetEmsDataParallelStatus(INT16U wData)
{
	pEmsData->Field.ParallelStatus = wData;
}

void	sSetEmsDataSerialNumber(INT16U wData)
{
	pEmsData->Field.SerialNumber = wData;
}

void	sSetEmsDataSerialStatus(INT16U wData)
{
	pEmsData->Field.SerialStatus = wData;
}

