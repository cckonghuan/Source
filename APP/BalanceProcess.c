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

/*��ʼ�����������*/
void	sBalanceInit(void)
{
	INT8U i;
	
	//���þ�����ر���
	bBalanceCellNum = 0;
	wBalanceTempNew = 0;
	
	wBalanceCellStatus = 0;
	fBalance.data = 0;
	
	//���þ���ʹ�ܼ�����������
	for(i = 0; i < cADC_CELL_VOLT_MAX; i++)
	{
		bBalanceEnChkCnt[i] = 0;
		bBalanceEnBackChkCnt[i] = 0;
	}
	
	// ��װ�������״̬�ͼ�����
	wBalanceTestStatus = 0;
	wBalanceTestCount = 0;
	
	//���¾������
	sBalanceParaUpdate();
}

/*���¾������*/
void	sBalanceParaUpdate(void)
{
	//��EEPROM�л�ȡ����״̬ʹ��״̬�;����ѹ��ֵ
	bBalanceEn = (INT8U)swGetEepromBalanceEn();
	wBalanceVoltEn = (INT16S)swGetEepromBalanceVoltEn();
}

/*���ÿ����صĵ�ѹ�Ƿ�ﵽ�����ѹ��ֵ*/
void	sBalanceVoltChk(INT8U bFilter)
{
	INT8U	i;
	
	// ����ÿ�����
	for(i = 0; i < bBalanceCellNum; i++)
	{
		// �����ѹ���ھ����ѹ��ֵ
		if(swGetCellVoltNew(i) >= wBalanceVoltEn)
		{
			// ���ú��˾���ʹ�ܼ�������
			bBalanceEnBackChkCnt[i] = 0;
			// �������������������ֵ�������þ���״̬λ
			if(++bBalanceEnChkCnt[i] >= bFilter)
			{
				bBalanceEnChkCnt[i] = 0;
				sSetBalanceCellStatus(1 << i);
			}
		}
		else
		{
			// ���þ���ʹ�ܼ�������
			bBalanceEnChkCnt[i] = 0;
			// ������������������������������״̬λ
			if(++bBalanceEnBackChkCnt[i] >= bFilter)
			{
				bBalanceEnBackChkCnt[i] = 0;
				sClrBalanceCellStatus(1 << i);
			}
		}
	}
}

/*������״̬�Ƿ���Ҫ����*/
void	sBalanceStatusChk(INT8U bFilter)
{
	//������ھ������״̬�������þ���״̬������λ����״̬
	if(wBalanceTestStatus != 0)
	{
		sRstBalanceCellStatus();
		sSetBalanceCellStatus(wBalanceTestStatus);
	}
	// �������δʹ�ܣ������þ���״̬
	else if(bBalanceEn == 0)
	{
		sRstBalanceCellStatus();
	}
	// ������ھ��ⱻ����ͣ״̬�������þ���״̬
	else if(sbGetBalancePassivePause())
	{
		sRstBalanceCellStatus();
	}
	// �����ص�ѹ������¶Ȼ�ŵ��¶ȹ��ߣ������þ���״̬
	else if(sbGetCellVoltHi2() || sbGetCellChgTempHi2() || sbGetCellDcgTempHi2())
	{
		sRstBalanceCellStatus();
	}
	// ������ڷŵ�״̬�������þ���״̬
	else if(sbGetBmsCurrentStatus() == cBMS_CURR_DSG)
	{
		sRstBalanceCellStatus();
	}
	// �������ص�ѹ�����¾���״̬
	else
	{
		sBalanceVoltChk(bFilter);
	}
}

/*�Զ����þ��ⱻ����ͣ״̬*/
void	sBalancePauseAutoRst(INT16U wFilter)
{
	static INT16U	wChkCnt = 0;
	
	// ������ھ��ⱻ����ͣ״̬������ݹ������Զ�������ͣ״̬
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

// ������ھ������״̬������ݹ�����ֵ�Զ����ò���״̬
void	sBalanceTestStatusAutoRst(INT16U wFilter)
{
	//������ھ������״̬������ݹ�����ֵ�Զ����ò���״̬
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

