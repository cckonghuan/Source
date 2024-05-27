/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		BalanceProcess.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define		__BALANCE_PROCESS_C__

/********************************************************************************
* Include head files                                                            *
********************************************************************************/
#include	"BalanceProcess.h"
#include	"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/


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
INT8U	bBalanceEn;
INT16U	wBalanceVoltEn;

/********************************************************************************
* Output variables																*
********************************************************************************/
INT8U	bBalanceCellNum;
INT16S	wBalanceTempNew;
INT16U	wBalanceCellStatus;
union BALANCE_STATUS_TABLE
{
	struct BALANCE_STATUS_BIT_FEILD
	{
		// BIT0-3
		INT16U	PassiveChk:1;
		INT16U	PassivePause:1;
		INT16U	PassiveTempHi:1;
		INT16U	rsvd1:1;
		
		// BIT4-7
		INT16U	BreakWireChkFlag:1;
		INT16U	BreakWireOddChk:1;
		INT16U	BreakWireEvenChk:1;
		INT16U	BreakWireRunning:1;
		
		// BIT8-11
		INT16U	BreakWireFault:1;
		INT16U	rsvd3:3;
		
		// BIT12-15
		INT16U	rsvd4:4;
	} bits;
	INT16U data;
} fBalance;

/********************************************************************************
* Internal variables															*
********************************************************************************/
INT8U	bBalanceEnChkCnt[cADC_CELL_VOLT_MAX];
INT8U	bBalanceEnBackChkCnt[cADC_CELL_VOLT_MAX];
INT16U	wBalanceTestStatus;
INT16U	wBalanceTestCount;

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
void	sBalanceVoltChk(INT8U bFilter);

/********************************************************************************
* Routines' implementations														*
********************************************************************************/

/*初始化均衡控制器*/
void	sBalanceInit(void)
{
	INT8U i;
	
	//重置均衡相关变量
	bBalanceCellNum = 0;
	wBalanceTempNew = 0;
	
	wBalanceCellStatus = 0;
	fBalance.data = 0;
	
	//重置均衡使能检测计数器数组
	for(i = 0; i < cADC_CELL_VOLT_MAX; i++)
	{
		bBalanceEnChkCnt[i] = 0;
		bBalanceEnBackChkCnt[i] = 0;
	}
	
	// 重装均衡测试状态和计数器
	wBalanceTestStatus = 0;
	wBalanceTestCount = 0;
	
	//更新均衡参数
	sBalanceParaUpdate();
}

/*更新均衡参数*/
void	sBalanceParaUpdate(void)
{
	//从EEPROM中获取均衡状态使能状态和均衡电压阈值
	bBalanceEn = (INT8U)swGetEepromBalanceEn();
	wBalanceVoltEn = (INT16S)swGetEepromBalanceVoltEn();
}

/*检查每个电池的电压是否达到均衡电压阈值*/
void	sBalanceVoltChk(INT8U bFilter)
{
	INT8U	i;
	
	// 遍历每个电池
	for(i = 0; i < bBalanceCellNum; i++)
	{
		// 如果电压高于均衡电压阈值
		if(swGetCellVoltNew(i) >= wBalanceVoltEn)
		{
			// 重置后退均衡使能检测计数器
			bBalanceEnBackChkCnt[i] = 0;
			// 如果计数器超过过滤器值，则设置均衡状态位
			if(++bBalanceEnChkCnt[i] >= bFilter)
			{
				bBalanceEnChkCnt[i] = 0;
				sSetBalanceCellStatus(1 << i);
			}
		}
		else
		{
			// 重置均衡使能检测计数器
			bBalanceEnChkCnt[i] = 0;
			// 如果计数器超过过滤器，则清除均衡状态位
			if(++bBalanceEnBackChkCnt[i] >= bFilter)
			{
				bBalanceEnBackChkCnt[i] = 0;
				sClrBalanceCellStatus(1 << i);
			}
		}
	}
}

/*检测均衡状态是否需要重置*/
void	sBalanceStatusChk(INT8U bFilter)
{
	//如果处于均衡测试状态，则重置均衡状态并设置位测试状态
	if(wBalanceTestStatus != 0)
	{
		sRstBalanceCellStatus();
		sSetBalanceCellStatus(wBalanceTestStatus);
	}
	// 如果均衡未使能，则重置均衡状态
	else if(bBalanceEn == 0)
	{
		sRstBalanceCellStatus();
	}
	// 如果处于均衡被动暂停状态，则重置均衡状态
	else if(sbGetBalancePassivePause())
	{
		sRstBalanceCellStatus();
	}
	// 如果电池电压、充电温度或放电温度过高，则重置均衡状态
	else if(sbGetCellVoltHi2() || sbGetCellChgTempHi2() || sbGetCellDcgTempHi2())
	{
		sRstBalanceCellStatus();
	}
	// 如果处于放电状态，则重置均衡状态
	else if(sbGetBmsCurrentStatus() == cBMS_CURR_DSG)
	{
		sRstBalanceCellStatus();
	}
	// 否则检查电池电压并更新均衡状态
	else
	{
		sBalanceVoltChk(bFilter);
	}
}

/*自动重置均衡被动暂停状态*/
void	sBalancePauseAutoRst(INT16U wFilter)
{
	static INT16U	wChkCnt = 0;
	
	// 如果处于均衡被动暂停状态，则根据过滤器自动重置暂停状态
	if(sbGetBalancePassivePause() == true)
	{
		if(++wChkCnt >= wFilter)
		{
			wChkCnt = 0;
			sSetBalancePassivePause(false);
		}
	}
	else
	{
		wChkCnt = 0;
	}
}

// 如果处于均衡测试状态，则根据过滤器值自动重置测试状态
void	sBalanceTestStatusAutoRst(INT16U wFilter)
{
	//如果处于均衡测试状态，则根据过滤器值自动重置测试状态
	if(wBalanceTestStatus != 0)
	{
		if(++wBalanceTestCount >= wFilter)
		{
			wBalanceTestCount = 0;
			wBalanceTestStatus = 0;
		}
	}
	else
	{
		wBalanceTestCount = 0;
	}
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
INT16U	swGetBalanceCellStatus(void)
{
	return wBalanceCellStatus;
}

INT8U	sbGetBalanceUnbalance(void)
{
	if(wBalanceCellStatus != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

INT8U	sbGetBalancePassivePause(void)
{
	return fBalance.bits.PassivePause;
}

INT8U	sbGetBreakWireFault(void)
{
	return fBalance.bits.BreakWireFault;
}

INT16U	swGetBalanceTestStatus(void)
{
	return wBalanceTestStatus;
}

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
void	sSetBalanceCellNum(INT8U bNum)
{
	bBalanceCellNum = bNum;
}

void	sSetBalanceTempNew(INT16S wTemp)
{
	wBalanceTempNew = wTemp;
}

void	sRstBalanceCellStatus(void)
{
	wBalanceCellStatus = 0;
}

void	sSetBalanceCellStatus(INT16U wStatus)
{
	wBalanceCellStatus |= wStatus;
}

void	sClrBalanceCellStatus(INT16U wStatus)
{
	wBalanceCellStatus &= ~wStatus;
}

void	sSetBalancePassivePause(INT8U bStatus)
{
	fBalance.bits.PassivePause = bStatus;
}

void	sSetBreakWireFault(INT8U bStatus)
{
	fBalance.bits.BreakWireFault = bStatus;
}

void	sSetBalanceTestStatus(INT16U bStatus)
{
	wBalanceTestCount = 0;
	wBalanceTestStatus = bStatus;
}

