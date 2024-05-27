/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		SoxProcess.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define			__SOX_PROCESS_C__

/********************************************************************************
* Include head files                                                            *
********************************************************************************/
#include		"Main.h"
#include		"SoxProcess.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define			cEnergySelfLossCurrSleep		5	// 5mA,�͹���ģʽ
#define			cEnergySelfLossCurrNoBl			40	// 40mA

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
INT16U	wEnergyAhReal;			// 0.1AH
INT16U	wEnergyAhMax; 			// 0.1AH
INT16U	wEnergyPct80Volt;		// 1mV
INT16U	wEnergyPct20Volt;		// 1mV
INT16U	wEnergyPct05Volt;		// 1mV
INT16S	wEnergyCompCoeff;		// -0.01%/��, <=5.00%
INT16S	wEnergyCompTemp;		// 1��
INT16U	wEnergyPct20TwoVolt;	// 

/********************************************************************************
* Output variables																*
********************************************************************************/
INT16U	wEnergySocNew;			// 1%
INT16U	wEnergySohNew;			// 1%
INT16U	wEnergyAhComp;			// 0.1AH

/********************************************************************************
* Internal variables															*
********************************************************************************/
INT16U	wEnergySocTemp;			// 1%
INT16U	wEnergyAhRated;			// 0.1AH
INT16U	wEnergyAh0Pct5Rated;	// 0.1AH
INT16U	wEnergyAh20PctRated;	// 0.1AH
INT16U	wEnergyAh40PctRated;	// 0.1AH
INT16U	wEnergyAh60PctRated;	// 0.1AH
INT16U	wEnergyAh80PctRated;	// 0.1AH
INT16U	wEnergyAhStart;			// 0.1AH
INT16U	wEnergyAhEnd;			// 0.1AH
INT32U	dwEnergyAhTime;			// 100ms
INT16U	wEnergyAh01Pct;			// 0.1AH
INT16U	wEnergyAh05Pct;			// 0.1AH
INT16U	wEnergyAh20Pct;			// 0.1AH
INT16U	wEnergyAh80Pct;			// 0.1AH
INT16U	wEnergyAh99Pct;			// 0.1AH

INT16U	wOcvPauseEnterCurr;		// 0.1A
INT16U	wOcvPauseExitCurr;		// 0.1A
INT16U	wOcvPauseEnterCnt;		// 0.1A
INT16U	wOcvPauseExitCnt;		// 0.1A

union SOX_STATUS_TABLE
{
	struct SOX_STATUS_BIT_FEILD
	{
		// BIT0-3
		INT16U	SocEmpty:1;
		INT16U	SocLess20Pct:1;
		INT16U	SocLarge80Pct:1;
		INT16U	SocFull:1;
		
		// BIT4-7
		INT16U	SohAccEn:1;
		INT16U	HaveLoadFlag:1;
		INT16U	rsvd2:2;
		
		// BIT8-11
		INT16U	SocLess05Pct:1;
		INT16U	SocOcvPause:1;
		INT16U	SocLess20Pct2:1;
		INT16U	rsvd3:1;
		
		// BIT12-15
		INT16U	rsvd4:4;
	} bits;
	INT16U data;
} fSoxStatus;

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/


/********************************************************************************
* Routines' implementations														*
********************************************************************************/
// SOX��ʼ��
void	sSoxInit(void)
{
	//��ʼ������
	wEnergySocNew = 0;
	wEnergySohNew = 100;
	wEnergyAhComp = 0;
	
	// ����sSoxParaUpdate�������²���
	sSoxParaUpdate();
}

void	sSoxParaUpdate(void)
{
	// ��EEPROM�л�ȡһЩ��ز���
	wEnergyPct80Volt = swGetEepromCellVoltPct80();// ��ȡ���80%��ѹ
	wEnergyPct20Volt = swGetEepromCellVoltPct20();// ��ȡ���20%��ѹ
	wEnergyPct20TwoVolt = swGetEepromCellVoltPct20Two();
	wEnergyPct05Volt = wEnergyPct20Volt - 100;//������5%��ѹ
	wEnergyCompCoeff = swGetEepromCellTempCoef();
	
	
	wEnergyAhRated = swGetBmsRatedAH();//��ȡ��ѹ��ӦAHֵ
	wEnergyAh0Pct5Rated = (wEnergyAhRated + 100) / 200;//���AHΪ0.5%�ֵ
	wEnergyAh20PctRated = wEnergyAhRated / 5;//���AHΪ20%�ֵ
	wEnergyAh40PctRated = wEnergyAh20PctRated * 2;//���AHΪ40%�ֵ
	wEnergyAh60PctRated = wEnergyAh20PctRated * 3;//���AHΪ60%�ֵ
	wEnergyAh80PctRated = wEnergyAh20PctRated * 4;//���AHΪ80%�ֵ

	// ��ȡ��ض������ʵ������
	wEnergyAhReal = swGetEepromBattAhReal(); // ��ȡ��ض��ѹ
	wEnergyAhComp = wEnergyAhReal; // ��ȡ���ʵ�ʵ�ѹ
	// ��ȡ��ѹ�������
	if(swGetEepromBattAhMax() >= wEnergyAhRated)
	{
		wEnergyAhMax = wEnergyAhRated;
	}
	else
	{
		wEnergyAhMax = swGetEepromBattAhMax();
	}
	//���ݵ�����������������ٷֱ��µ�����ֵ
	wEnergyAh01Pct = ((INT32U)wEnergyAhMax * 41) >> 12;//������1%����
	wEnergyAh05Pct = ((INT32U)wEnergyAhMax * 205) >> 12;//������5%����
	wEnergyAh20Pct = ((INT32U)wEnergyAhMax * 819) >> 12;//������20%����
	wEnergyAh80Pct = ((INT32U)wEnergyAhMax * 3277) >> 12;//������80%����
	wEnergyAh99Pct = ((INT32U)wEnergyAhMax * 4055) >> 12;//������99%����
	
	//�����ص�״̬����
	wEnergySocTemp = (INT16U)(((INT32U)wEnergyAhReal * 100 + (wEnergyAhMax >> 1)) / wEnergyAhMax);// ������ʣ�������ٷֱ�
	wEnergySocNew = wEnergySocTemp;
	wEnergySohNew = ((INT32U)wEnergyAhMax * 100 + (wEnergyAhRated >> 1)) / wEnergyAhRated;// ������ʣ�������ٷֱ�
	
	// �������������������/�˳���ͣ״̬����ĵ�����ʱ��
    wOcvPauseEnterCurr = 100;   // ������ͣ״̬�ĵ���Ϊ10.0A
    wOcvPauseExitCurr = 50;     // �˳���ͣ״̬�ĵ���Ϊ5.0A
    wOcvPauseEnterCnt = 10;     // ������ͣ״̬����ʱ��Ϊ1s��10*100ms
    wOcvPauseExitCnt = 600;     // �˳���ͣ״̬����ʱ��Ϊ60s��600*100ms
}

void	sSoxOcvChk(INT16U wFilter1, INT16U wFilter2)
{
	// ��̬���������ڼ�¼�����״̬�ļ���
	static INT8U bSocPct05Cnt = 0;
	static INT8U bSocPct20Cnt = 0;
	static INT8U bSocPct20Cnt2 = 0;
	static INT8U bSocPct80Cnt = 0;
	static INT16U wSocOcvPauseCnt = 0;
	
	// �����ش��ڿ��������߻����ģʽ��������״̬�ͼ�����
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cSleepMode) \
		|| (bBmsMode == cStandbyMode))
	{
		// ����״̬�ͼ�����
		bSocPct05Cnt = 0;
		bSocPct20Cnt = 0;
		bSocPct20Cnt2 = 0;
		bSocPct80Cnt = 0;
		wSocOcvPauseCnt = 0;
		// ����״̬��־
		fSoxStatus.bits.SocEmpty = false;
		fSoxStatus.bits.SocLess05Pct = false;
		fSoxStatus.bits.SocLess20Pct = false;
		fSoxStatus.bits.SocLarge80Pct = false;
		fSoxStatus.bits.SocFull = false;
		fSoxStatus.bits.SocLess20Pct2 = false;
		fSoxStatus.bits.SocOcvPause = true;
	}
	else
	{
		// ��������ڿ��������߻����ģʽ�������״̬���
		// ������Ƿ�Ϊ��
		fSoxStatus.bits.SocEmpty = sbGetCellVoltLoAlm();	// 2.5V
		// ������Ƿ�Ϊ��
		fSoxStatus.bits.SocFull = sbGetCellVoltHiAlm();		// 3.6V
		
		// �����ش�����ͣ״̬
		if(fSoxStatus.bits.SocOcvPause == true)
		{
			 // �����������������ͣ״̬����ֵ�������������ͣ״̬
			if(sb16UOverLevelChk(swAbs(swGetBattCurrNew()),wOcvPauseEnterCurr,wOcvPauseEnterCnt,&wSocOcvPauseCnt) == true)
			{
				fSoxStatus.bits.SocOcvPause = true;
			}
		}
		else
		{
			// ������������˳���ͣ״̬����ֵ�����˳���ͣ״̬
			if(sb16UUnderLevelChk(swAbs(swGetBattCurrNew()),wOcvPauseExitCurr,wOcvPauseExitCnt,&wSocOcvPauseCnt) == true)
			{
				fSoxStatus.bits.SocOcvPause = false;
			}
		}
		
		 // �����������ͣ״̬������е�ص�ѹ�ٷֱȵļ��
        if(fSoxStatus.bits.SocOcvPause == false)
        {
            // ������Ƿ����5%����
            if(fSoxStatus.bits.SocLess05Pct == true)
            {
                // �����ѹ����5%��������ֵ������Ϊ��ز�����5%����
                if(sb8UOverLevelChk(swGetMinCellVoltFiltNew(),wEnergyPct05Volt,wFilter2,&bSocPct05Cnt) == true)
                {
                    fSoxStatus.bits.SocLess05Pct = false;
                }
            }
            else
            {
                // �����ѹ����5%��������ֵ������Ϊ��ص���5%����
                if(sb8UUnderLevelChk(swGetMinCellVoltFiltNew(),wEnergyPct05Volt,wFilter1,&bSocPct05Cnt) == true)
                {
                    fSoxStatus.bits.SocLess05Pct = true;
                }
            }
			//�����Ǽ��20%��80%
			if(fSoxStatus.bits.SocLess20Pct == true)
			{
				if(sb8UOverLevelChk(swGetMinCellVoltFiltNew(),wEnergyPct20Volt,wFilter2,&bSocPct20Cnt) == true)
				{
					fSoxStatus.bits.SocLess20Pct = false;
				}
			}
			else
			{
				if(sb8UUnderLevelChk(swGetMinCellVoltFiltNew(),wEnergyPct20Volt,wFilter1,&bSocPct20Cnt) == true)
				{
					fSoxStatus.bits.SocLess20Pct = true;
				}
			}
			
			if(fSoxStatus.bits.SocLarge80Pct == true)
			{
				if(sb8UUnderLevelChk(swGetMaxCellVoltFiltNew(),wEnergyPct80Volt,wFilter2,&bSocPct80Cnt) == true)
				{
					fSoxStatus.bits.SocLarge80Pct = false;
				}
			}
			else
			{
				if(sb8UOverLevelChk(swGetMaxCellVoltFiltNew(),wEnergyPct80Volt,wFilter1,&bSocPct80Cnt) == true)
				{
					fSoxStatus.bits.SocLarge80Pct = true;
				}
			}
			
			if(swGetSocNew() < 20)
			{
				if(fSoxStatus.bits.SocLess20Pct2 == true)		
				{
					if(sb8UUnderLevelChk(swGetMinCellVoltFiltNew(), wEnergyPct20TwoVolt, wFilter2,&bSocPct20Cnt2) == true)
					{
						fSoxStatus.bits.SocLess20Pct2 = false;
					}
				}
				else
				{
					if(sb8UOverLevelChk(swGetMinCellVoltFiltNew(), wEnergyPct20TwoVolt, wFilter1,&bSocPct20Cnt2) == true)
					{
						fSoxStatus.bits.SocLess20Pct2 = true;
					}
				}
			}
			else
			{
				bSocPct20Cnt2 = 0;
				fSoxStatus.bits.SocLess20Pct2 = false;
			}
		}
		else
		{
			// ���������ͣ״̬��������״̬�ͼ�����
			bSocPct05Cnt = 0;
			bSocPct20Cnt = 0;
			bSocPct20Cnt2 = 0;
			bSocPct80Cnt = 0;
			fSoxStatus.bits.SocLess05Pct = false;
			fSoxStatus.bits.SocLess20Pct = false;
			fSoxStatus.bits.SocLess20Pct2 = false;
			fSoxStatus.bits.SocLarge80Pct = false;
		}
	}
}

void	sSoxAhCalc(INT16S wCurr)
{
	// ��̬�����������ۼӵ����Լ��Էŵ����
	static INT32S	dwAhAcc = 0;
	static INT8U	bSelfAccCnt = 0;
	
	// �����ش��ڵ�ء�����ŵ�ģʽ�����ۼӵ���
	if((bBmsMode == cBatteryMode) || (bBmsMode == cChargeMode) \
		|| (bBmsMode == cDischargeMode))
	{
		dwAhAcc += wCurr;
	}
	else
	{
		// ���򣬸����Էŵ�ģʽ���зŵ紦��
		if(++bSelfAccCnt >= 100)
		{
			bSelfAccCnt = 0;
			if(bBmsMode == cSleepMode)
			{
                dwAhAcc -= cEnergySelfLossCurrSleep; // ��˯��ģʽ�£���ȥ�Էŵ����
            }
            else
            {
                dwAhAcc -= cEnergySelfLossCurrNoBl; // �ڷ�˯��ģʽ�£���ȥ�Էŵ����
            }
		}
	}
	
	// ��������ۼ�ֵ�ﵽһ����ֵ������µ��������״̬
    if(dwAhAcc <= -36000)
    {
        dwAhAcc += 36000;
        if(wEnergyAhReal > wEnergyAh01Pct)
        {
            wEnergyAhReal--; // ����ʵ�ʵ���
        }
        
        fSoxStatus.bits.SohAccEn = false; // ֹͣ�ۼ��������
    }
	else if(dwAhAcc >= 36000)
    {
        dwAhAcc -= 36000;
        if(wEnergyAhReal < wEnergyAh99Pct)
        {
            wEnergyAhReal++; // ����ʵ�ʵ���
        }
        
        if(fSoxStatus.bits.SohAccEn == true)
        {
            wEnergyAhEnd++; // �ۼ���������ʱ�����½�������
        }
        else if(swGetMinCellTempNew() >= 20)    // ����¶ȸ��ڵ���20�棬������������ۼ�
        {
            fSoxStatus.bits.SohAccEn = true;
        }
    }
	
	 // ���ݲ�ͬ״̬������������ۼ���ر���
    if(fSoxStatus.bits.SohAccEn == false)
    {
        // �����������������ۼƣ���������ر���
        wEnergyAhStart = wEnergyAhReal;
        wEnergyAhEnd = wEnergyAhStart;
        dwEnergyAhTime = 0;
    }
    else
    {
        // ���������������ۼƣ����������������������ۼ�״̬
        if(++dwEnergyAhTime >= 180000) // ʱ��ﵽ��ֵʱ��ֹͣ��������ۼ�
        {
            dwEnergyAhTime = 0;
            fSoxStatus.bits.SohAccEn = false;
        }
        if(swGetMinCellTempNew() <= 15) // �¶ȵ��ڵ���15��ʱ��ֹͣ��������ۼ�
        {
            fSoxStatus.bits.SohAccEn = false;
        }
    }

	// ���ݵ����ٷֱ�״̬���е���	
	if(fSoxStatus.bits.SocEmpty == true)
	{
		wEnergyAhReal = 0;// �������Ϊ�㣬��ʵ�ʵ���Ϊ��
	}
	else if(fSoxStatus.bits.SocFull == true)
	{
		wEnergyAhReal = wEnergyAhMax; // �������Ϊ������ʵ�ʵ���Ϊ�������
	}
	else if(fSoxStatus.bits.SocLess05Pct == true)//�����ʣ�������Ƿ����5%
	{
		if(wEnergyAhReal > wEnergyAh05Pct)
		{
			wEnergyAhReal = wEnergyAh05Pct;// ���ʵ�ʵ���������5%����ʵ�ʵ�������Ϊ5%����
		}
	}
	else if(fSoxStatus.bits.SocLess20Pct == true)
	{
		if(wEnergyAhReal > wEnergyAh20Pct)
		{
			wEnergyAhReal = wEnergyAh20Pct;
		}
	}
	else if(fSoxStatus.bits.SocLess20Pct2 == true)
	{
		if(wEnergyAhReal < wEnergyAh20Pct)
		{
			wEnergyAhReal = wEnergyAh20Pct;
		}
	}
	else if(fSoxStatus.bits.SocLarge80Pct == true)
	{
		if(wEnergyAhReal < wEnergyAh80Pct)
		{
			wEnergyAhReal = wEnergyAh80Pct;
		}
	}
}

/*�����¶�ϵ�����в������㣬������ʵ�ʵ�����Ȼ�����������ĵ�������SOC*/
void	sSocEstimate(INT16S wTemp)
{
	INT16S	wDampPct;
	INT16S	wCompPct;
	
	//�������¶�ϵ����Ϊ0������в�������
	if(wEnergyCompCoeff != 0)
	{
		// �����¶�ϵ������˥���ٷֱ�
		if(wTemp < 25)
		{
			wDampPct = (INT16S)((INT32S)wEnergyCompCoeff * (25 - wEnergyCompTemp));
		}
		else
		{
			wDampPct = 0;
		}
		// ���㲹���ٷֱ�
		wCompPct = 10000 - wDampPct;
		// ȷ�������ٷֱ���0��10000֮��
		if(wCompPct > 10000)
		{
			wCompPct = 10000;
		}
		else if(wCompPct < 0)
		{
			wCompPct = 0;
		}
		// ���ݲ����ٷֱȶ�ʵ�ʵ�����������
		wEnergyAhComp = (INT16U)(((INT32U)wEnergyAhReal * wCompPct + 5000) / 10000);
	}
	else
	{
		// ����¶�ϵ��Ϊ�㣬�򲻽�������
		wEnergyAhComp = wEnergyAhReal;
	}
	
	// ����������ĵ�������SOC�ٷֱ�
	wEnergySocTemp = (INT16U)(((INT32U)wEnergyAhComp * 100 + (wEnergyAhMax >> 1)) / wEnergyAhMax);
}

// ��SOC�㷨�����˲�
void	sSocFilter(INT16U wFilter)
{
	static INT16U wSocCnt = 0; // SOC������
	INT16U wSocTemp; // ��ʱ�洢SOCֵ
	
	wSocTemp = wEnergySocTemp; // ����ǰSOCֵ�洢����ʱ������

	// ���ݵ�ع���ģʽ��ͬ���в�ͬ�Ĵ���
	// ������ڵ��ģʽ
	if(bBmsMode == cBatteryMode)
	{
		if(++wSocCnt >= wFilter) //SOC�������ﵽ�˲�������ִֵ��
		{
			wSocCnt = 0;
			if(sbGetBmsCurrentStatus() == cBMS_CURR_CHG)//���������ڳ��
			{
				if(wSocTemp < wEnergySocTemp)
				{
					wSocTemp++; // ���SOCС�ڵ�ǰSOC��������SOC
				}
			}
			else if(sbGetBmsCurrentStatus() == cBMS_CURR_DSG)//���������ڷŵ�
			{
				if(wSocTemp > wEnergySocTemp)
				{
					wSocTemp--;// ���SOCС�ڵ�ǰSOC��������SOC
				}
			}
			else// ���������������ֹ״̬��
			{
				if(wSocTemp > wEnergySocTemp)
				{
					wSocTemp--;//�����ʱ�����SOC���ڵ�ǰSOC�������SOC
				}
				else if(wSocTemp < wEnergySocTemp)
				{
					wSocTemp++;//�����ʱ�����SOCС�ڵ�ǰSOC��������SOC
				}
			}
		}
	}
	else if(bBmsMode == cChargeMode)// �����ش��ڳ��ģʽ
	{
		if(++wSocCnt >= wFilter)
		{
			wSocCnt = 0;
			if(wSocTemp < wEnergySocTemp)
			{
				wSocTemp++;
			}
		}
	}
	else if(bBmsMode == cDischargeMode)// �����ش��ڷŵ�ģʽ
	{
		if(++wSocCnt >= wFilter)
		{
			wSocCnt = 0;
			if(wSocTemp > wEnergySocTemp)
			{
				wSocTemp--;
			}
		}
	}
	else// �������
	{
		wSocCnt = 0;
		wSocTemp = wEnergySocTemp;
	}
	
	if(wSocTemp > 100)
	{
		wSocTemp = 100;
	}
	wEnergySocNew = wSocTemp;
}

void	sSohEstimate(void)
{
	INT16S wChgAh1, wChgAh2;
	INT16S wChgCurr, wChgMin;
	INT16S wAhAdj, wAhStep;
	INT16S wAhMax;
	
	// �����ص�ѹ������SOHУ׼ʹ�ܿ������������һ�����������򷵻�
	if((sbGetCellVoltHiAlm() == false) || (fSoxStatus.bits.SohAccEn == false))
	{
		return;
	}
	fSoxStatus.bits.SohAccEn = false; // �ر�SOHУ׼ʹ��
	
	// ������̫Сʱ��У׼
	wChgAh1 = wEnergyAhEnd - wEnergyAhStart; // ������
	wChgAh2 = wEnergyAhMax - wEnergyAhStart; // �ܵ���
	wChgMin = (INT16S)(((INT32S)dwEnergyAhTime + 300) / 600);		// ���ʱ�䣨���ӣ�to Minute
	if(wChgMin > 0)
	{
		wChgCurr = (INT16S)(((INT32S)wChgAh1 * 60) / wChgMin);		// ������to Current
	}
	else
	{
		wChgCurr = 0;
	}
	wEnergyAhStart = 0;
	wEnergyAhEnd = 0;
	dwEnergyAhTime = 0;

	// ����������ϵͻ��߳������ϵͣ����˳�У׼
	if((wChgAh1 < wEnergyAh20PctRated) \
		|| (wChgAh2 < wEnergyAh20PctRated) \
		|| (wChgCurr < wEnergyAh20PctRated))
	{
		return;
	}
	
	// ����SOHУ׼����
	wAhAdj = ((INT32S)wChgAh1 * wEnergyAhMax) / wChgAh2;
	
	// ���ݳ������,����У������
	if(wChgAh1 >= wEnergyAh80PctRated)
	{
		wAhStep = wEnergyAh0Pct5Rated;
	}
	else if(wChgAh1 >= wEnergyAh40PctRated)
	{
		wAhStep = (wEnergyAh0Pct5Rated + 1) >> 1;
	}
	else
	{
		wAhStep = (wEnergyAh0Pct5Rated + 2) >> 2;
	}
	
	// �����������
	wAhMax = wEnergyAhMax;
	if(wAhAdj > wEnergyAhMax)
	{
		wAhMax += wAhStep;
	}
	else if(wAhAdj < wEnergyAhMax)
	{
		wAhMax -= wAhStep;
	}
	
	// �����������޷�(60%~100%)
	if(wAhMax > wEnergyAhRated)
	{
		wAhMax = wEnergyAhRated;
	}
	else if(wAhMax < wEnergyAh60PctRated)
	{
		wAhMax = wEnergyAh60PctRated;
	}
	wEnergyAhMax = wAhMax;
	//������������µİٷֱ�
	wEnergyAh01Pct = ((INT32U)wEnergyAhMax * 41) >> 12;
	wEnergyAh05Pct = ((INT32U)wEnergyAhMax * 205) >> 12;
	wEnergyAh20Pct = ((INT32U)wEnergyAhMax * 819) >> 12;
	wEnergyAh80Pct = ((INT32U)wEnergyAhMax * 3277) >> 12;
	wEnergyAh99Pct = ((INT32U)wEnergyAhMax * 4055) >> 12;
	wEnergySohNew = ((INT32U)wEnergyAhMax * 100 + (wEnergyAhRated >> 1)) / wEnergyAhRated;
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
INT16U	swGetSocNew(void)
{
	return(wEnergySocNew);
}

INT16U	swGetSohNew(void)
{
	return(wEnergySohNew);
}

INT16U	swGetSoxAhReal(void)
{
	return(wEnergyAhReal);
}

INT16U	swGetSoxAhMax(void)
{
	return(wEnergyAhMax);
}

INT16U	swGetSoxAhComp(void)
{
	return(wEnergyAhComp);
}

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
void	sSetSoxAhReal(INT16U wAH)
{
	OS_CPU_SR  cpu_sr;
	INT16U wSoC;
	
	if(wAH > wEnergyAhMax)
	{
		wAH = wEnergyAhMax;
	}
	wSoC = (INT8U)(((INT32U)wAH * 100 + (wEnergyAhMax >> 1)) / wEnergyAhMax);
	
	OS_ENTER_CRITICAL();
	sSetEepromBattAhReal(wAH);
	wEnergyAhReal = wAH;
	wEnergySocTemp = wSoC;
	wEnergySocNew = wSoC;
	OS_EXIT_CRITICAL();
}

void	sSetSoxAhMax(INT16U wAH)
{
	OS_CPU_SR  cpu_sr;
	INT16U wSoH;
	
	if(wAH > wEnergyAhRated)
	{
		wAH = wEnergyAhRated;
	}
	wSoH = (INT8U)(((INT32U)wAH * 100 + (wEnergyAhRated >> 1)) / wEnergyAhRated);
	
	OS_ENTER_CRITICAL();
	sSetEepromBattAhMax(wAH);
	wEnergyAhMax = wAH;
	wEnergySohNew = wSoH;
	OS_EXIT_CRITICAL();
	
	sSetSoxAhReal(wEnergyAhReal);
}

void	sSetSoxCompTemp(INT16S wTemp)
{
	wEnergyCompTemp = wTemp;
}

