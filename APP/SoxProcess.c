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
#define			cEnergySelfLossCurrSleep		5	// 5mA,低功耗模式
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
INT16S	wEnergyCompCoeff;		// -0.01%/℃, <=5.00%
INT16S	wEnergyCompTemp;		// 1℃
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
// SOX初始化
void	sSoxInit(void)
{
	//初始化变量
	wEnergySocNew = 0;
	wEnergySohNew = 100;
	wEnergyAhComp = 0;
	
	// 调用sSoxParaUpdate函数更新参数
	sSoxParaUpdate();
}

void	sSoxParaUpdate(void)
{
	// 从EEPROM中获取一些电池参数
	wEnergyPct80Volt = swGetEepromCellVoltPct80();// 获取电池80%电压
	wEnergyPct20Volt = swGetEepromCellVoltPct20();// 获取电池20%电压
	wEnergyPct20TwoVolt = swGetEepromCellVoltPct20Two();
	wEnergyPct05Volt = wEnergyPct20Volt - 100;//计算电池5%电压
	wEnergyCompCoeff = swGetEepromCellTempCoef();
	
	
	wEnergyAhRated = swGetBmsRatedAH();//获取电压对应AH值
	wEnergyAh0Pct5Rated = (wEnergyAhRated + 100) / 200;//电池AH为0.5%额定值
	wEnergyAh20PctRated = wEnergyAhRated / 5;//电池AH为20%额定值
	wEnergyAh40PctRated = wEnergyAh20PctRated * 2;//电池AH为40%额定值
	wEnergyAh60PctRated = wEnergyAh20PctRated * 3;//电池AH为60%额定值
	wEnergyAh80PctRated = wEnergyAh20PctRated * 4;//电池AH为80%额定值

	// 获取电池额定容量和实际容量
	wEnergyAhReal = swGetEepromBattAhReal(); // 获取电池额定电压
	wEnergyAhComp = wEnergyAhReal; // 获取电池实际电压
	// 获取电压最大容量
	if(swGetEepromBattAhMax() >= wEnergyAhRated)
	{
		wEnergyAhMax = wEnergyAhRated;
	}
	else
	{
		wEnergyAhMax = swGetEepromBattAhMax();
	}
	//根据电池最大容量计算各个百分比下的容量值
	wEnergyAh01Pct = ((INT32U)wEnergyAhMax * 41) >> 12;//计算电池1%容量
	wEnergyAh05Pct = ((INT32U)wEnergyAhMax * 205) >> 12;//计算电池5%容量
	wEnergyAh20Pct = ((INT32U)wEnergyAhMax * 819) >> 12;//计算电池20%容量
	wEnergyAh80Pct = ((INT32U)wEnergyAhMax * 3277) >> 12;//计算电池80%容量
	wEnergyAh99Pct = ((INT32U)wEnergyAhMax * 4055) >> 12;//计算电池99%容量
	
	//计算电池的状态参数
	wEnergySocTemp = (INT16U)(((INT32U)wEnergyAhReal * 100 + (wEnergyAhMax >> 1)) / wEnergyAhMax);// 计算电池剩余容量百分比
	wEnergySocNew = wEnergySocTemp;
	wEnergySohNew = ((INT32U)wEnergyAhMax * 100 + (wEnergyAhRated >> 1)) / wEnergyAhRated;// 计算电池剩余寿命百分比
	
	// 设置其他参数，如进入/退出暂停状态所需的电流和时间
    wOcvPauseEnterCurr = 100;   // 进入暂停状态的电流为10.0A
    wOcvPauseExitCurr = 50;     // 退出暂停状态的电流为5.0A
    wOcvPauseEnterCnt = 10;     // 进入暂停状态所需时间为1s，10*100ms
    wOcvPauseExitCnt = 600;     // 退出暂停状态所需时间为60s，600*100ms
}

void	sSoxOcvChk(INT16U wFilter1, INT16U wFilter2)
{
	// 静态变量，用于记录检查电池状态的计数
	static INT8U bSocPct05Cnt = 0;
	static INT8U bSocPct20Cnt = 0;
	static INT8U bSocPct20Cnt2 = 0;
	static INT8U bSocPct80Cnt = 0;
	static INT16U wSocOcvPauseCnt = 0;
	
	// 如果电池处于开机、休眠或待机模式，则重置状态和计数器
	if((bBmsMode == cPowerOnMode) || (bBmsMode == cSleepMode) \
		|| (bBmsMode == cStandbyMode))
	{
		// 重置状态和计数器
		bSocPct05Cnt = 0;
		bSocPct20Cnt = 0;
		bSocPct20Cnt2 = 0;
		bSocPct80Cnt = 0;
		wSocOcvPauseCnt = 0;
		// 重置状态标志
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
		// 如果不处于开机、休眠或待机模式，则进行状态检查
		// 检查电池是否为空
		fSoxStatus.bits.SocEmpty = sbGetCellVoltLoAlm();	// 2.5V
		// 检查电池是否为满
		fSoxStatus.bits.SocFull = sbGetCellVoltHiAlm();		// 3.6V
		
		// 如果电池处于暂停状态
		if(fSoxStatus.bits.SocOcvPause == true)
		{
			 // 如果电流超过进入暂停状态的阈值，则继续保持暂停状态
			if(sb16UOverLevelChk(swAbs(swGetBattCurrNew()),wOcvPauseEnterCurr,wOcvPauseEnterCnt,&wSocOcvPauseCnt) == true)
			{
				fSoxStatus.bits.SocOcvPause = true;
			}
		}
		else
		{
			// 如果电流低于退出暂停状态的阈值，则退出暂停状态
			if(sb16UUnderLevelChk(swAbs(swGetBattCurrNew()),wOcvPauseExitCurr,wOcvPauseExitCnt,&wSocOcvPauseCnt) == true)
			{
				fSoxStatus.bits.SocOcvPause = false;
			}
		}
		
		 // 如果不处于暂停状态，则进行电池电压百分比的检查
        if(fSoxStatus.bits.SocOcvPause == false)
        {
            // 检查电池是否低于5%容量
            if(fSoxStatus.bits.SocLess05Pct == true)
            {
                // 如果电压高于5%容量的阈值，则认为电池不低于5%容量
                if(sb8UOverLevelChk(swGetMinCellVoltFiltNew(),wEnergyPct05Volt,wFilter2,&bSocPct05Cnt) == true)
                {
                    fSoxStatus.bits.SocLess05Pct = false;
                }
            }
            else
            {
                // 如果电压低于5%容量的阈值，则认为电池低于5%容量
                if(sb8UUnderLevelChk(swGetMinCellVoltFiltNew(),wEnergyPct05Volt,wFilter1,&bSocPct05Cnt) == true)
                {
                    fSoxStatus.bits.SocLess05Pct = true;
                }
            }
			//后面是检查20%，80%
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
			// 如果处于暂停状态，则重置状态和计数器
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
	// 静态变量，用于累加电量以及自放电计数
	static INT32S	dwAhAcc = 0;
	static INT8U	bSelfAccCnt = 0;
	
	// 如果电池处于电池、充电或放电模式，则累加电量
	if((bBmsMode == cBatteryMode) || (bBmsMode == cChargeMode) \
		|| (bBmsMode == cDischargeMode))
	{
		dwAhAcc += wCurr;
	}
	else
	{
		// 否则，根据自放电模式进行放电处理
		if(++bSelfAccCnt >= 100)
		{
			bSelfAccCnt = 0;
			if(bBmsMode == cSleepMode)
			{
                dwAhAcc -= cEnergySelfLossCurrSleep; // 在睡眠模式下，减去自放电电流
            }
            else
            {
                dwAhAcc -= cEnergySelfLossCurrNoBl; // 在非睡眠模式下，减去自放电电流
            }
		}
	}
	
	// 如果电量累加值达到一定阈值，则更新电量及相关状态
    if(dwAhAcc <= -36000)
    {
        dwAhAcc += 36000;
        if(wEnergyAhReal > wEnergyAh01Pct)
        {
            wEnergyAhReal--; // 降低实际电量
        }
        
        fSoxStatus.bits.SohAccEn = false; // 停止累计寿命损耗
    }
	else if(dwAhAcc >= 36000)
    {
        dwAhAcc -= 36000;
        if(wEnergyAhReal < wEnergyAh99Pct)
        {
            wEnergyAhReal++; // 增加实际电量
        }
        
        if(fSoxStatus.bits.SohAccEn == true)
        {
            wEnergyAhEnd++; // 累计寿命结束时，更新结束电量
        }
        else if(swGetMinCellTempNew() >= 20)    // 如果温度高于等于20℃，启用寿命损耗累计
        {
            fSoxStatus.bits.SohAccEn = true;
        }
    }
	
	 // 根据不同状态更新寿命损耗累计相关变量
    if(fSoxStatus.bits.SohAccEn == false)
    {
        // 如果不启用寿命损耗累计，则重置相关变量
        wEnergyAhStart = wEnergyAhReal;
        wEnergyAhEnd = wEnergyAhStart;
        dwEnergyAhTime = 0;
    }
    else
    {
        // 如果启用寿命损耗累计，则根据条件更新寿命损耗累计状态
        if(++dwEnergyAhTime >= 180000) // 时间达到阈值时，停止寿命损耗累计
        {
            dwEnergyAhTime = 0;
            fSoxStatus.bits.SohAccEn = false;
        }
        if(swGetMinCellTempNew() <= 15) // 温度低于等于15℃时，停止寿命损耗累计
        {
            fSoxStatus.bits.SohAccEn = false;
        }
    }

	// 根据电量百分比状态进行调整	
	if(fSoxStatus.bits.SocEmpty == true)
	{
		wEnergyAhReal = 0;// 如果电量为零，则实际电量为零
	}
	else if(fSoxStatus.bits.SocFull == true)
	{
		wEnergyAhReal = wEnergyAhMax; // 如果电量为满，则实际电量为最大容量
	}
	else if(fSoxStatus.bits.SocLess05Pct == true)//检查电池剩余容量是否低于5%
	{
		if(wEnergyAhReal > wEnergyAh05Pct)
		{
			wEnergyAhReal = wEnergyAh05Pct;// 如果实际电量高于于5%，则实际电量调整为5%容量
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

/*根据温度系数进行补偿计算，以修正实际电量，然后根据修正后的电量计算SOC*/
void	sSocEstimate(INT16S wTemp)
{
	INT16S	wDampPct;
	INT16S	wCompPct;
	
	//如果电池温度系数不为0，则进行补偿计算
	if(wEnergyCompCoeff != 0)
	{
		// 根据温度系数计算衰减百分比
		if(wTemp < 25)
		{
			wDampPct = (INT16S)((INT32S)wEnergyCompCoeff * (25 - wEnergyCompTemp));
		}
		else
		{
			wDampPct = 0;
		}
		// 计算补偿百分比
		wCompPct = 10000 - wDampPct;
		// 确保补偿百分比在0到10000之间
		if(wCompPct > 10000)
		{
			wCompPct = 10000;
		}
		else if(wCompPct < 0)
		{
			wCompPct = 0;
		}
		// 根据补偿百分比对实际电量进行修正
		wEnergyAhComp = (INT16U)(((INT32U)wEnergyAhReal * wCompPct + 5000) / 10000);
	}
	else
	{
		// 电池温度系数为零，则不进行修正
		wEnergyAhComp = wEnergyAhReal;
	}
	
	// 根据修正后的电量计算SOC百分比
	wEnergySocTemp = (INT16U)(((INT32U)wEnergyAhComp * 100 + (wEnergyAhMax >> 1)) / wEnergyAhMax);
}

// 对SOC算法进行滤波
void	sSocFilter(INT16U wFilter)
{
	static INT16U wSocCnt = 0; // SOC计数器
	INT16U wSocTemp; // 临时存储SOC值
	
	wSocTemp = wEnergySocTemp; // 将当前SOC值存储到临时变量中

	// 根据电池工作模式不同进行不同的处理
	// 如果处于电池模式
	if(bBmsMode == cBatteryMode)
	{
		if(++wSocCnt >= wFilter) //SOC计数器达到滤波器设置值执行
		{
			wSocCnt = 0;
			if(sbGetBmsCurrentStatus() == cBMS_CURR_CHG)//如果电池正在充电
			{
				if(wSocTemp < wEnergySocTemp)
				{
					wSocTemp++; // 如果SOC小于当前SOC，则增加SOC
				}
			}
			else if(sbGetBmsCurrentStatus() == cBMS_CURR_DSG)//如果电池正在放电
			{
				if(wSocTemp > wEnergySocTemp)
				{
					wSocTemp--;// 如果SOC小于当前SOC，则增加SOC
				}
			}
			else// 其他情况（包括静止状态）
			{
				if(wSocTemp > wEnergySocTemp)
				{
					wSocTemp--;//如果临时储存的SOC大于当前SOC，则减少SOC
				}
				else if(wSocTemp < wEnergySocTemp)
				{
					wSocTemp++;//如果临时储存的SOC小于当前SOC，则增加SOC
				}
			}
		}
	}
	else if(bBmsMode == cChargeMode)// 如果电池处于充电模式
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
	else if(bBmsMode == cDischargeMode)// 如果电池处于放电模式
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
	else// 其他情况
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
	
	// 如果电池电压正常且SOH校准使能开启，则进行下一步操作，否则返回
	if((sbGetCellVoltHiAlm() == false) || (fSoxStatus.bits.SohAccEn == false))
	{
		return;
	}
	fSoxStatus.bits.SohAccEn = false; // 关闭SOH校准使能
	
	// 充电电量太小时不校准
	wChgAh1 = wEnergyAhEnd - wEnergyAhStart; // 充电电量
	wChgAh2 = wEnergyAhMax - wEnergyAhStart; // 总电量
	wChgMin = (INT16S)(((INT32S)dwEnergyAhTime + 300) / 600);		// 充电时间（分钟）to Minute
	if(wChgMin > 0)
	{
		wChgCurr = (INT16S)(((INT32S)wChgAh1 * 60) / wChgMin);		// 充电电流to Current
	}
	else
	{
		wChgCurr = 0;
	}
	wEnergyAhStart = 0;
	wEnergyAhEnd = 0;
	dwEnergyAhTime = 0;

	// 如果充电电量较低或者充电电流较低，则退出校准
	if((wChgAh1 < wEnergyAh20PctRated) \
		|| (wChgAh2 < wEnergyAh20PctRated) \
		|| (wChgCurr < wEnergyAh20PctRated))
	{
		return;
	}
	
	// 计算SOH校准幅度
	wAhAdj = ((INT32S)wChgAh1 * wEnergyAhMax) / wChgAh2;
	
	// 根据充电能量,调节校正幅度
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
	
	// 更新最大容量
	wAhMax = wEnergyAhMax;
	if(wAhAdj > wEnergyAhMax)
	{
		wAhMax += wAhStep;
	}
	else if(wAhAdj < wEnergyAhMax)
	{
		wAhMax -= wAhStep;
	}
	
	// 最大可用容量限幅(60%~100%)
	if(wAhMax > wEnergyAhRated)
	{
		wAhMax = wEnergyAhRated;
	}
	else if(wAhMax < wEnergyAh60PctRated)
	{
		wAhMax = wEnergyAh60PctRated;
	}
	wEnergyAhMax = wAhMax;
	//计算最大容量下的百分比
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

