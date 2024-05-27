/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		AdcProcess.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define			__ADC_PROCESS_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"AdcProcess.h"
#include		"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
typedef void  (*pFunc1)(INT16U);//函数指针类型，指向一个参数为INT16U类型、返回类型为void的函数
//ADC采样平均次数
#define		cADC_AVG_CNT			2000	// 2000*50us=100ms
//根据不同的设置，电池电压的增益，用于将ADC原始值转换为实际电压值
#if (UPDATE_BATT_VOLT_AFE == 1)
//#define		cADC_BATT_VOLT_GAIN		640		// 4096*5/32
#define		cADC_BATT_VOLT_GAIN		1024	// 4096*(16/64)
#else
#define		cADC_BATT_VOLT_GAIN		1024	// 4096*(16/64)
#endif
//根据不同的设置电池电流的增益，用于将ADC的原始值转换为实际电流值
#if (UPDATE_BATT_CURR_AFE == 1)
//#define		cADC_BATT_CURR_10_GAIN		3816	// 4096*200/(21470*0.002Ω/10)/100
#define		cADC_BATT_CURR_10_GAIN		5986	// 4096*200/(21470*0.002Ω/20)/100
#define		cADC_BATT_CURR_15_GAIN		2862	// 4096*200/(21470*0.002Ω/15)/100
#else
#define		cADC_BATT_CURR_10_GAIN		16500	// 10*4096*(3.3V/4096)*(1kΩ/20kΩ)*(10/0.001Ω)
#define		cADC_BATT_CURR_15_GAIN		16500	// 10*4096*(3.3V/4096)*(1kΩ/20kΩ)*(10/0.001Ω)
#endif
//单体电压增益，电池组电压增益
#define		cADC_CELL_VOLT_GAIN		640		// 4096*5/32
#define 	cADC_PACK_VOLT_GAIN		1024	// 4096*(16/64)

#define		cADC_BATT_VOLT			0
#define		cADC_BATT_CURR			1
#define		cADC_CELL_VOLT01		2
#define		cADC_CELL_VOLT02		3
#define		cADC_CELL_VOLT03		4
#define		cADC_CELL_VOLT04		5
#define		cADC_CELL_VOLT05		6
#define		cADC_CELL_VOLT06		7
#define		cADC_CELL_VOLT07		8
#define		cADC_CELL_VOLT08		9
#define		cADC_CELL_VOLT09		10
#define		cADC_CELL_VOLT10		11
#define		cADC_CELL_VOLT11		12
#define		cADC_CELL_VOLT12		13
#define		cADC_CELL_VOLT13		14
#define		cADC_CELL_VOLT14		15
#define		cADC_CELL_VOLT15		16
#define		cADC_CELL_VOLT16		17
#define		cADC_CELL_TEMP01		18
#define		cADC_CELL_TEMP02		19
#define		cADC_CELL_TEMP03		20
#define		cADC_CELL_TEMP04		21
#define		cADC_CELL_TEMP05		22
#define		cADC_CELL_TEMP06		23
#define		cADC_CELL_TEMP07		24
#define		cADC_CELL_TEMP08		25
#define		cADC_HS_TEMP			26
#define		cADC_HW_VERSION1		27
#define		cADC_HW_VERSION2		28
#define		cADC_PACK_VOLT			29
#define		cADC_NUM_MAX			30

#define     cADC_TEMP_MIN			(INT16S)-40
#define     cADC_TEMP_MAX			(INT16S)120
#define     cADC_TEMP_RANGE			(INT16S)(cADC_TEMP_MAX - cADC_TEMP_MIN + 1)
#define     cADC_CELL_TEMP_RANGE	cADC_TEMP_RANGE
#define     cADC_NTC_TEMP_RANGE		cADC_TEMP_RANGE

/********************************************************************************
* Constants																		*
********************************************************************************/
const INT16U cCellTempR153B3950Tab[cADC_CELL_TEMP_RANGE] =	// LXCWF-153F3950 -40℃-120℃,22kΩ/15kΩ
{
	3949,	3938,	3928,	3916,	3904,	3891,	3878,	3864,	3849,	3833,
	3817,	3799,	3781,	3762,	3742,	3721,	3700,	3677,	3653,	3628,
	3602,	3575,	3548,	3519,	3489,	3457,	3425,	3392,	3358,	3322,
	3286,	3249,	3210,	3171,	3131,	3089,	3047,	3004,	2960,	2915,
	2870,	2824,	2777,	2730,	2682,	2634,	2585,	2536,	2487,	2437,
	2387,	2337,	2288,	2238,	2188,	2138,	2089,	2039,	1990,	1942,
	1894,	1846,	1799,	1752,	1706,	1661,	1616,	1572,	1528,	1486,
	1444,	1403,	1362,	1323,	1284,	1246,	1209,	1173,	1138,	1104,
	1070,	1037,	1005,	974,	944,	915,	886,	859,	832,	806,
	780,	756,	732,	709,	686,	664,	643,	623,	603,	584,
	565,	547,	530,	513,	497,	481,	466,	451,	437,	423,
	410,	397,	385,	373,	361,	350,	339,	329,	319,	309,
	299,	290,	281,	273,	265,	257,	249,	241,	234,	227,
	220,	214,	208,	202,	196,	190,	184,	179,	174,	169,
	164,	160,	155,	151,	146,	142,	138,	135,	131,	127,
	124,	120,	117,	114,	111,	108,	105,	102,	100,	97,
	95
};

const INT16U cCellTempR103B3950Tab[cADC_CELL_TEMP_RANGE] =	// CWFL103F3950FCZ8 -40℃-120℃,22kΩ/10kΩ
{
	3781,	3765,	3748,	3731,	3712,	3693,	3673,	3652,	3630,	3607,
	3583,	3558,	3532,	3506,	3478,	3449,	3419,	3388,	3357,	3324,
	3290,	3255,	3219,	3182,	3144,	3105,	3066,	3025,	2983,	2941,
	2898,	2854,	2809,	2764,	2717,	2671,	2624,	2576,	2528,	2480,
	2431,	2382,	2333,	2283,	2234,	2185,	2136,	2086,	2038,	1989,
	1939,	1892,	1845,	1797,	1751,	1705,	1659,	1614,	1570,	1526,
	1483,	1441,	1399,	1359,	1319,	1280,	1242,	1204,	1168,	1132,
	1097,	1064,	1031,	998,	967,	936,	907,	878,	850,	823,
	796,	771,	746,	722,	698,	676,	654,	632,	612,	592,
	574,	554,	536,	518,	501,	485,	469,	454,	439,	425,
	411,	398,	385,	373,	361,	349,	338,	327,	317,	307,
	297,	288,	279,	270,	261,	253,	245,	238,	231,	223,
	217,	210,	203,	197,	191,	186,	180,	175,	169,	164,
	159,	155,	150,	146,	141,	137,	133,	129,	126,	122,
	119,	115,	112,	109,	106,	103,	100,	97,		95,		92,
	89,		87,		85,		82,		80,		78,		76,		74,		72,		70,
	68
};

const INT16U cNtcTempSampTab[cADC_NTC_TEMP_RANGE] =	// EWTF05-473F4B -40℃-120℃,49.9kΩ/47kΩ
{
	3979,	3971,	3963,	3954,	3944,	3934,	3924,	3912,	3901,	3888,
	3875,	3861,	3847,	3832,	3816,	3799,	3782,	3764,	3745,	3725,
	3705,	3683,	3661,	3637,	3613,	3588,	3562,	3535,	3507,	3479,
	3449,	3418,	3387,	3354,	3321,	3286,	3251,	3215,	3178,	3140,
	3101,	3062,	3022,	2981,	2939,	2897,	2854,	2811,	2767,	2722,
	2678,	2632,	2587,	2541,	2495,	2449,	2402,	2356,	2309,	2263,
	2216,	2170,	2124,	2078,	2032,	1987,	1942,	1897,	1853,	1809,
	1765,	1723,	1680,	1638,	1597,	1557,	1517,	1478,	1439,	1401,
	1364,	1327,	1292,	1257,	1222,	1189,	1156,	1124,	1093,	1062,
	1032,	1003,	974,	947,	920,	893,	868,	843,	818,	795,
	772,	750,	728,	707,	686,	666,	647,	628,	610,	592,
	575,	558,	542,	526,	511,	496,	482,	468,	454,	441,
	429,	416,	404,	393,	382,	371,	360,	350,	340,	330,
	321,	312,	303,	295,	287,	279,	271,	264,	256,	249,
	242,	236,	229,	223,	217,	211,	206,	200,	195,	190,
	185,	180,	175,	170,	166,	162,	157,	153,	149,	146,
	142
};

/********************************************************************************
* External variables															*
********************************************************************************/


/********************************************************************************
* External routine prototypes													*
********************************************************************************/


/********************************************************************************
* Input variables																*
********************************************************************************/
INT16S	wBattVoltAdj;						// 4096
INT16S	wBattVoltBias;						// Q7 1mV
INT16S	wBattChgCurrAdj;					// 4096
INT16S	wBattChgCurrBias;					// Q7 0.1A
INT16S	wBattDsgCurrAdj;					// 4096
INT16S	wBattDsgCurrBias;					// Q7 0.1A
INT16S	wCellVoltAdj[cADC_CELL_VOLT_MAX];	// 4096
INT16S	wCellVoltBias[cADC_CELL_VOLT_MAX];// Q7 1mV
INT16S	wPackVoltAdj;						// 4096
INT16S	wPackVoltBias;						// Q7 1mV

#ifdef VIRTUAL_ADC_SAMPLE
INT16S	wTestSample[cADC_NUM_MAX];
#endif

/********************************************************************************
* Output variables																*
********************************************************************************/
INT16U wAdcBuffer[cADC_CH_NUM];				// 采样缓存
INT16U wAdcResult[cADC_CH_NUM];				// 采样值
INT16S wAdcSample[cADC_NUM_MAX];			// 采样值(去偏置)
INT16S wAdcSampAccCnt;						// 采样累加计数值
INT16S wAdcSampAccCntTemp;					// 采样累加计数值
INT32S dwAdcSampAcc[cADC_NUM_MAX];			// 采样累加值
INT32S dwAdcSampAccTemp[cADC_NUM_MAX];		// 采样累加值
INT16S wAdcSampAvg[cADC_NUM_MAX];			// 采样周期平均值(采样值)
INT16S wAdcRealAvg[cADC_NUM_MAX];			// 采样周期平均值(定标值)
INT32S dwAdcRealAvgAcc[cADC_NUM_MAX];		// 采样周期平均值累加(定标值)
INT32S dwAdcRealAvgAccCnt;					// 采样周期平均值累加计数(定标值)
INT16S wAdcRealAvgFilt[cADC_NUM_MAX];		// 采样周期平均值滤波(定标值)

/********************************************************************************
* Internal variables															*
********************************************************************************/
INT16S wAdcCurrOffset;						// 电流采样偏置,默认偏置=2048
INT16U wAdcCurrOffsetCnt;
INT32S dwAdcCurrOffsetSum;

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
INT16S	swTemperatureCalc(INT16S wTempSamp, const INT16U *pTempResTable);

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
//初始化函数，用于初始化ADC相关的变量和参数
void	sAdcInitial(void)
{
	INT8U i;
	
	// 将ADC结果数组中的所有元素初始化为0
	for(i = 0; i < cADC_CH_NUM; i++)
	{
		wAdcResult[i] = 0;
	}
	//将采样数组、实际平均值数组和其他相关数组的所有元素初始化为0
	for(i = 0; i < cADC_NUM_MAX; i++)
	{
		wAdcSample[i] = 0;
		wAdcRealAvg[i] = 0;
		dwAdcSampAccTemp[i] = 0;
		dwAdcSampAcc[i] = 0;
		wAdcSampAvg[i] = 0;
		dwAdcRealAvgAcc[i] = 0;
		wAdcRealAvgFilt[i] = 0;
	}
	
	//将采样累积计数器、实际平均值累积计数器和其他相关累积计数器初始化为0
	wAdcSampAccCntTemp = 0;
	wAdcSampAccCnt = 0;
	dwAdcRealAvgAccCnt = 0;
	
	// 将电流偏移量和偏移量计数器初始化为0，偏移量累积和初始化为0
	wAdcCurrOffset = 2048;
	wAdcCurrOffsetCnt = 0;
	dwAdcCurrOffsetSum = 0;
	
	//如果定义了VIRTUAL_ADC_SAMPLE宏，则将ADC采样数组中的值初始化为虚拟值
#ifdef VIRTUAL_ADC_SAMPLE
	wTestSample[cADC_BATT_VOLT] = 5120;
	wTestSample[cADC_BATT_CURR] = 0;
	wTestSample[cADC_CELL_VOLT01] = 3200;
	wTestSample[cADC_CELL_VOLT02] = 3200;
	wTestSample[cADC_CELL_VOLT03] = 3200;
	wTestSample[cADC_CELL_VOLT04] = 3200;
	wTestSample[cADC_CELL_VOLT05] = 3200;
	wTestSample[cADC_CELL_VOLT06] = 3200;
	wTestSample[cADC_CELL_VOLT07] = 3200;
	wTestSample[cADC_CELL_VOLT08] = 3200;
	wTestSample[cADC_CELL_VOLT09] = 3200;
	wTestSample[cADC_CELL_VOLT10] = 3200;
	wTestSample[cADC_CELL_VOLT11] = 3200;
	wTestSample[cADC_CELL_VOLT12] = 3200;
	wTestSample[cADC_CELL_VOLT13] = 3200;
	wTestSample[cADC_CELL_VOLT14] = 3200;
	wTestSample[cADC_CELL_VOLT15] = 3200;
	wTestSample[cADC_CELL_VOLT16] = 3200;
	wTestSample[cADC_CELL_TEMP01] = 25;
	wTestSample[cADC_CELL_TEMP02] = 25;
	wTestSample[cADC_CELL_TEMP03] = 25;
	wTestSample[cADC_CELL_TEMP04] = 25;
	wTestSample[cADC_CELL_TEMP05] = 25;
	wTestSample[cADC_CELL_TEMP06] = 25;
	wTestSample[cADC_CELL_TEMP07] = 25;
	wTestSample[cADC_CELL_TEMP08] = 25;
	wTestSample[cADC_HS_TEMP] = 25;
	wTestSample[cADC_HW_VERSION1] = 1024;
	wTestSample[cADC_HW_VERSION2] = 3072;
	wTestSample[cADC_PACK_VOLT] = 5120;
#endif
}

//初始化函数，用于初始化ADC相关的系数
void	sAdcCoeffInit(void)
{
	INT8U i;
	
	// 设置电池电压调整和偏移
	sSetBattVoltAdj(swGetEepromBattVoltAdj());
	sSetBattVoltBias(swGetEepromBattVoltBias());
	//设置充电电流调整和偏移
	sSetBattChgCurrAdj(swGetEepromBattChgCurrAdj());
	sSetBattChgCurrBias(swGetEepromBattChgCurrBias());
	//设置放电电流调整和偏移
	sSetBattDsgCurrAdj(swGetEepromBattDsgCurrAdj());
	sSetBattDsgCurrBias(swGetEepromBattDsgCurrBias());

	//设置每个单体电压通道的调整和偏移
	for(i = 0; i < cADC_CELL_VOLT_MAX; i++)
	{
		sSetCellVoltAdj(i, swGetEepromCellVoltAdj(i));
		sSetCellVoltBias(i, swGetEepromCellVoltBias(i));
	}

	//设置电池组带你呀的调整和偏移
	sSetPackVoltAdj(4096);
	sSetPackVoltBias(0);
	
}

// ADC中断服务函数，用于处理ADC采样和平均
void	sAdcISR(void)
{
	INT8U bIndex;
	
	// GET SAMPLE
	// 获取样本
#ifndef VIRTUAL_ADC_SAMPLE
	
	// GET DMA BUFFER
	// 获取DMA缓冲区中的采样值
	//wAdcResult[cADC_CH_BAT_CURR]	= wAdcBuffer[cADC_CH_BAT_CURR];
	wAdcResult[cADC_CH_NTC_TEMP]	= wAdcBuffer[cADC_CH_NTC_TEMP];	// 1987 for test
	wAdcResult[cADC_CH_CELL1_TEMP]	= wAdcBuffer[cADC_CH_CELL1_TEMP];
	wAdcResult[cADC_CH_CELL2_TEMP]	= wAdcBuffer[cADC_CH_CELL2_TEMP];
	wAdcResult[cADC_CH_CELL3_TEMP]	= wAdcBuffer[cADC_CH_CELL3_TEMP];
	wAdcResult[cADC_CH_CELL4_TEMP]	= wAdcBuffer[cADC_CH_CELL4_TEMP];
	wAdcResult[cADC_CH_VERSION1]	= wAdcBuffer[cADC_CH_VERSION1];
	
	// SAMPLE
	// 电池电流采样由AFE检测
#if (UPDATE_BATT_CURR_AFE == 0)
	//wAdcSample[cADC_BATT_CURR] = (INT16S)wAdcResult[cADC_CH_BAT_CURR] - wAdcCurrOffset;
#endif
	wAdcSample[cADC_HS_TEMP] = (INT16S)wAdcResult[cADC_CH_NTC_TEMP];
#if (UPDATE_CELL_TEMP_AFE == 0)
	if((swGetEepromCellTempShield() & 0x0001) == 0)
	{
		wAdcSample[cADC_CELL_TEMP01] = (INT16S)wAdcResult[cADC_CH_CELL1_TEMP];
	}
	else
	{
		wAdcSample[cADC_CELL_TEMP01] = 1661;
	}
	if((swGetEepromCellTempShield() & 0x0002) == 0)
	{
		wAdcSample[cADC_CELL_TEMP02] = (INT16S)wAdcResult[cADC_CH_CELL2_TEMP];
	}
	else
	{
		wAdcSample[cADC_CELL_TEMP02] = 1661;
	}
	if((swGetEepromCellTempShield() & 0x0004) == 0)
	{
		wAdcSample[cADC_CELL_TEMP03] = (INT16S)wAdcResult[cADC_CH_CELL3_TEMP];
	}
	else
	{
		wAdcSample[cADC_CELL_TEMP03] = 1661;
	}
	if((swGetEepromCellTempShield() & 0x0008) == 0)
	{
		wAdcSample[cADC_CELL_TEMP04] = (INT16S)wAdcResult[cADC_CH_CELL4_TEMP];
	}
	else
	{
		wAdcSample[cADC_CELL_TEMP04] = 1661;
	}
#endif
	wAdcSample[cADC_HW_VERSION1] = (INT16S)wAdcResult[cADC_CH_VERSION1];
	
	if(wAdcSample[cADC_HS_TEMP] > 4000)		// HS温感线未接则报高温故障
	{
		wAdcSample[cADC_HS_TEMP] = 0;
	}
	if(wAdcSample[cADC_CELL_TEMP01] > 4000)	// CELL温感线未接则报高温故障
	{
		wAdcSample[cADC_CELL_TEMP01] = 0;
	}
	if(wAdcSample[cADC_CELL_TEMP02] > 4000)	// CELL温感线未接则报高温故障
	{
		wAdcSample[cADC_CELL_TEMP02] = 0;
	}
	if(wAdcSample[cADC_CELL_TEMP03] > 4000)	// CELL温感线未接则报高温故障
	{
		wAdcSample[cADC_CELL_TEMP03] = 0;
	}
	if(wAdcSample[cADC_CELL_TEMP04] > 4000)	// CELL温感线未接则报高温故障
	{
		wAdcSample[cADC_CELL_TEMP04] = 0;
	}
#else
	wAdcSample[cADC_BATT_VOLT] = ((INT32S)wTestSample[cADC_BATT_VOLT] * 4096) / cADC_BATT_VOLT_GAIN;
	if(swGetBmsCurrSampType() == cBMS_CURR_SAMPLE_15)
	{
		wAdcSample[cADC_BATT_CURR] = ((INT32S)wTestSample[cADC_BATT_CURR] * 4096) / cADC_BATT_CURR_15_GAIN;
	}
	else
	{
		wAdcSample[cADC_BATT_CURR] = ((INT32S)wTestSample[cADC_BATT_CURR] * 4096) / cADC_BATT_CURR_10_GAIN;
	}
	wAdcSample[cADC_CELL_VOLT01] = ((INT32S)wTestSample[cADC_CELL_VOLT01] * 4096) / cADC_CELL_VOLT_GAIN;
	wAdcSample[cADC_CELL_VOLT02] = ((INT32S)wTestSample[cADC_CELL_VOLT02] * 4096) / cADC_CELL_VOLT_GAIN;
	wAdcSample[cADC_CELL_VOLT03] = ((INT32S)wTestSample[cADC_CELL_VOLT03] * 4096) / cADC_CELL_VOLT_GAIN;
	wAdcSample[cADC_CELL_VOLT04] = ((INT32S)wTestSample[cADC_CELL_VOLT04] * 4096) / cADC_CELL_VOLT_GAIN;
	wAdcSample[cADC_CELL_VOLT05] = ((INT32S)wTestSample[cADC_CELL_VOLT05] * 4096) / cADC_CELL_VOLT_GAIN;
	wAdcSample[cADC_CELL_VOLT06] = ((INT32S)wTestSample[cADC_CELL_VOLT06] * 4096) / cADC_CELL_VOLT_GAIN;
	wAdcSample[cADC_CELL_VOLT07] = ((INT32S)wTestSample[cADC_CELL_VOLT07] * 4096) / cADC_CELL_VOLT_GAIN;
	wAdcSample[cADC_CELL_VOLT08] = ((INT32S)wTestSample[cADC_CELL_VOLT08] * 4096) / cADC_CELL_VOLT_GAIN;
	wAdcSample[cADC_CELL_VOLT09] = ((INT32S)wTestSample[cADC_CELL_VOLT09] * 4096) / cADC_CELL_VOLT_GAIN;
	wAdcSample[cADC_CELL_VOLT10] = ((INT32S)wTestSample[cADC_CELL_VOLT10] * 4096) / cADC_CELL_VOLT_GAIN;
	wAdcSample[cADC_CELL_VOLT11] = ((INT32S)wTestSample[cADC_CELL_VOLT11] * 4096) / cADC_CELL_VOLT_GAIN;
	wAdcSample[cADC_CELL_VOLT12] = ((INT32S)wTestSample[cADC_CELL_VOLT12] * 4096) / cADC_CELL_VOLT_GAIN;
	wAdcSample[cADC_CELL_VOLT13] = ((INT32S)wTestSample[cADC_CELL_VOLT13] * 4096) / cADC_CELL_VOLT_GAIN;
	wAdcSample[cADC_CELL_VOLT14] = ((INT32S)wTestSample[cADC_CELL_VOLT14] * 4096) / cADC_CELL_VOLT_GAIN;
	wAdcSample[cADC_CELL_VOLT15] = ((INT32S)wTestSample[cADC_CELL_VOLT15] * 4096) / cADC_CELL_VOLT_GAIN;
	wAdcSample[cADC_CELL_VOLT16] = ((INT32S)wTestSample[cADC_CELL_VOLT16] * 4096) / cADC_CELL_VOLT_GAIN;
	wAdcSample[cADC_CELL_TEMP01] = cCellTempR153B3950Tab[wTestSample[cADC_CELL_TEMP01] + 40];
	wAdcSample[cADC_CELL_TEMP02] = cCellTempR153B3950Tab[wTestSample[cADC_CELL_TEMP02] + 40];
	wAdcSample[cADC_CELL_TEMP03] = cCellTempR153B3950Tab[wTestSample[cADC_CELL_TEMP03] + 40];
	wAdcSample[cADC_CELL_TEMP04] = cCellTempR153B3950Tab[wTestSample[cADC_CELL_TEMP04] + 40];
	wAdcSample[cADC_CELL_TEMP05] = cCellTempR153B3950Tab[wTestSample[cADC_CELL_TEMP05] + 40];
	wAdcSample[cADC_CELL_TEMP06] = cCellTempR153B3950Tab[wTestSample[cADC_CELL_TEMP06] + 40];
	wAdcSample[cADC_CELL_TEMP07] = cCellTempR153B3950Tab[wTestSample[cADC_CELL_TEMP07] + 40];
	wAdcSample[cADC_CELL_TEMP08] = cCellTempR153B3950Tab[wTestSample[cADC_CELL_TEMP08] + 40];
	wAdcSample[cADC_HS_TEMP]	 = cNtcTempSampTab[wTestSample[cADC_HS_TEMP] + 40];
	wAdcSample[cADC_HW_VERSION1] = wTestSample[cADC_HW_VERSION1];
	wAdcSample[cADC_HW_VERSION2] = wTestSample[cADC_HW_VERSION2];
	wAdcSample[cADC_PACK_VOLT] = ((INT32S)wTestSample[cADC_PACK_VOLT] * 4096) / cADC_BATT_VOLT_GAIN;
#endif
	
	// AVG
	for(bIndex = 0; bIndex < cADC_NUM_MAX; bIndex++)
	{
		dwAdcSampAccTemp[bIndex] += wAdcSample[bIndex];
	}

	//如果累积计数达到指定数量，则计算平均值并发送事件
	if(++wAdcSampAccCntTemp >= cADC_AVG_CNT)
	{
		for(bIndex = 0; bIndex < cADC_NUM_MAX; bIndex++)
		{
			dwAdcSampAcc[bIndex] = dwAdcSampAccTemp[bIndex];
			dwAdcSampAccTemp[bIndex] = 0;
		}
		wAdcSampAccCnt = wAdcSampAccCntTemp;
		wAdcSampAccCntTemp = 0;
		
		// 发送ADC平均计算事件
		OSISREventSend(cPrioGeneral, eAdcAvgCalc);
	}
}

// ADC平均计算函数，用于计算ADC采样的平均值并校正
void	sAdcAvgCalc(void)
{
	INT8U bIndex;
	
	//如果有效采样数量不为0
	if(wAdcSampAccCnt > 0)
	{
		// 计算每个通道的平均值
		for(bIndex = 0; bIndex < cADC_NUM_MAX; bIndex++)
		{
			wAdcSampAvg[bIndex] = (INT16S)(dwAdcSampAcc[bIndex]	/ wAdcSampAccCnt);
		}
		
		// BattVolt
		wAdcRealAvg[cADC_BATT_VOLT] = (INT16S)(((INT32S)wAdcSampAvg[cADC_BATT_VOLT] * wBattVoltAdj + (INT32S)wBattVoltBias * 32 + 2048) >> 12);
		if(wAdcRealAvg[cADC_BATT_VOLT] < 0)
		{
			wAdcRealAvg[cADC_BATT_VOLT] = 0;
		}
		
		// BattCurr
		if(wAdcSampAvg[cADC_BATT_CURR] > 0)
		{
			wAdcRealAvg[cADC_BATT_CURR] = (INT16S)(((INT32S)wAdcSampAvg[cADC_BATT_CURR] * wBattChgCurrAdj + (INT32S)wBattChgCurrBias * 32 + 2048) >> 12);
		}
		else
		{
			wAdcRealAvg[cADC_BATT_CURR] = (INT16S)(((INT32S)wAdcSampAvg[cADC_BATT_CURR] * wBattDsgCurrAdj - (INT32S)wBattDsgCurrBias * 32 + 2048) >> 12);
		}
		
		// CellVolt
		for(bIndex = cADC_CELL_VOLT01; bIndex <= cADC_CELL_VOLT16; bIndex++)
		{
			wAdcRealAvg[bIndex] = (INT16S)(((INT32S)wAdcSampAvg[bIndex] * wCellVoltAdj[bIndex - cADC_CELL_VOLT01] + (INT32S)wCellVoltBias[bIndex - cADC_CELL_VOLT01] * 32 + 2048) >> 12);
		}
		
		// CellTemp
		for(bIndex = cADC_CELL_TEMP01; bIndex <= cADC_CELL_TEMP08; bIndex++)
		{
			if(swGetBmsTempType()==cBMS_TEMP_103F3950)
			{
				wAdcRealAvg[bIndex]	= swTemperatureCalc(wAdcSampAvg[bIndex], cCellTempR103B3950Tab);
			}
			else
			{
				wAdcRealAvg[bIndex]	= swTemperatureCalc(wAdcSampAvg[bIndex], cCellTempR153B3950Tab);
			}
		}
		
		// HsTemp
		wAdcRealAvg[cADC_HS_TEMP] = swTemperatureCalc(wAdcSampAvg[cADC_HS_TEMP], cNtcTempSampTab);
		
		// HwVerion1
		wAdcRealAvg[cADC_HW_VERSION1] = wAdcSampAvg[cADC_HW_VERSION1];
		
		// HwVerion2
		wAdcRealAvg[cADC_HW_VERSION2] = wAdcSampAvg[cADC_HW_VERSION2];
		
		// PackVolt
		wAdcRealAvg[cADC_PACK_VOLT] = (INT16S)(((INT32S)wAdcSampAvg[cADC_PACK_VOLT] * wPackVoltAdj + (INT32S)wPackVoltBias * 32 + 2048) >> 12);
	}
}

/**
 * @brief 对ADC的实际平均值进行滤波
 * 
 * @param bFilter 滤波器的阶数
 */
void	sAdcAvgFilter(INT8U bFilter)
{
	INT8U bIndex;
	
	// 累加实际平均值
	for(bIndex = 0; bIndex < cADC_NUM_MAX; bIndex++)
	{
		dwAdcRealAvgAcc[bIndex] += wAdcRealAvg[bIndex];
	}
	
	//如果累加次数达到滤波器阶数
	if(++dwAdcRealAvgAccCnt >= bFilter)
	{
		// 计算滤波后的平均值
		for(bIndex = 0; bIndex < cADC_NUM_MAX; bIndex++)
		{
			wAdcRealAvgFilt[bIndex] = (INT16S)(dwAdcRealAvgAcc[bIndex]  / dwAdcRealAvgAccCnt);
			dwAdcRealAvgAcc[bIndex] = 0;
		}
		dwAdcRealAvgAccCnt = 0;
	}
}
/**
 * @brief 根据采样值计算温度
 * 
 * @param wTempSamp 温度采样值
 * @param pTempResTable 温度与采样值对应的表
 * @return INT16S 返回温度值
 */
INT16S	swTemperatureCalc(INT16S wTempSamp, const INT16U *pTempResTable)
{
	INT16U wLowIndex, wHighIndex, wMidIndex;
	INT16S wTemperature;
	
	wLowIndex = 0;
	wHighIndex = cADC_TEMP_RANGE - 1;

	// 如果采样值大于等于表中的最小值
	if(wTempSamp >= pTempResTable[0])
	{
		wTemperature = cADC_TEMP_MIN;// 返回温度最小值
	}

	//如果采样值大于等于表中的最小值
	else if(wTempSamp <= pTempResTable[cADC_TEMP_RANGE - 1])
	{
		wTemperature = cADC_TEMP_MAX;// 返回最大温度值
	}
	else
	{
		// 二分搜索
		while((wLowIndex + 1) < wHighIndex)
		{
			wMidIndex = (wLowIndex + wHighIndex) >> 1;
			if(wTempSamp == pTempResTable[wMidIndex])
			{
				wTemperature = (INT16S)(wMidIndex + cADC_TEMP_MIN);
				return wTemperature;
			}
			if(wTempSamp < pTempResTable[wMidIndex])
			{
				wLowIndex = wMidIndex;
			}
			else
			{
				wHighIndex = wMidIndex;
			}
		}
		// 线性插值
		wTemperature = (INT16S)(wLowIndex + cADC_TEMP_MIN);
		if(wTempSamp < ((pTempResTable[wLowIndex] + pTempResTable[wLowIndex + 1]) >> 1))
		{
			wTemperature++;
		}
	}
	return wTemperature;
}

/**
 * @brief 初始化ADC的偏移值
 */
void	sAdcOffsetInitial(void)
{
	wAdcCurrOffsetCnt = 0;
	dwAdcCurrOffsetSum = 0;
}

// 计算电流偏移量的函数
INT8U	sbAdcOffsetCalc(INT8U bFilter)
{
	// 如果未使用AFE进行电池电流的采样
	#if (UPDATE_BATT_CURR_AFE == 0)
	OS_CPU_SR	cpu_sr; // 保存中断状态的变量
	INT16S wCurrBiasTemp; // 临时电流偏移量
	
	// 将当前的电池电流采样值累加到偏移量总和中
	dwAdcCurrOffsetSum += swGetBattCurrSampAvg();
	// 如果累加的采样次数达到了指定的滤波器长度
	if(++wAdcCurrOffsetCnt >= bFilter)
	{
		// 计算平均偏移值
		wCurrBiasTemp = ((dwAdcCurrOffsetSum + (wAdcCurrOffsetCnt >> 1)) / wAdcCurrOffsetCnt);
		// 根据范围进行限制
		if(wCurrBiasTemp > 200)
		{
			wCurrBiasTemp = 200;
		}
		else if(wCurrBiasTemp < -200)
		{
			wCurrBiasTemp = -200;
		}
		// 重置偏移量累计值和计数器
		dwAdcCurrOffsetSum = 0;
		wAdcCurrOffsetCnt = 0;
		
		// 进入临界区，更新电流偏移量
		OS_ENTER_CRITICAL();
		wAdcCurrOffset += wCurrBiasTemp;
		OS_EXIT_CRITICAL();
		
		return true; // 返回True表示偏移计算完成
	}
	else
	{
		return false; // 返回false表示偏移计算未完成
	}
#else
	// 如果使用了AFE进行电池电流采样，则直接返回ture，表示偏移计算不需要执行
	UNUSED(bFilter);
	return true;
#endif
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
INT16S	swGetAdcCurrOffset(void)
{
	return wAdcCurrOffset;
}

INT16S	swGetBattCurrSampAvg(void)
{
	return wAdcSampAvg[cADC_BATT_CURR];
}

INT16S	swGetBattVoltSample(void)
{
	return wAdcSample[cADC_BATT_VOLT];
}

INT16S	swGetBattCurrSample(void)
{
	return wAdcSample[cADC_BATT_CURR];
}

INT16S	swGetCellVoltSample(INT8U bNo)
{
	return wAdcSample[cADC_CELL_VOLT01+bNo];
}

INT16S	swGetCellTempSample(INT8U bNo)
{
	return wAdcSample[cADC_CELL_TEMP01+bNo];
}

INT16S	swGetHsTempSample(void)
{
	return wAdcSample[cADC_HS_TEMP];
}

INT16S	swGetHwVersion1Sample(void)
{
	return wAdcSample[cADC_HW_VERSION1];
}

INT16S	swGetHwVersion2Sample(void)
{
	return wAdcSample[cADC_HW_VERSION2];
}

INT16S	swGetPackVoltSample(void)
{
	return wAdcSample[cADC_PACK_VOLT];
}

INT16S	swGetBattVoltReal(void)
{
	return wAdcRealAvg[cADC_BATT_VOLT];
}

INT16S	swGetBattCurrReal(void)
{
	return wAdcRealAvg[cADC_BATT_CURR];
}

INT16S	swGetCellVoltReal(INT8U bNo)
{
	return wAdcRealAvg[cADC_CELL_VOLT01+bNo];
}

INT16S	swGetCellTempReal(INT8U bNo)
{
	return wAdcRealAvg[cADC_CELL_TEMP01+bNo];
}

INT16S	swGetHsTempReal(void)
{
	return wAdcRealAvg[cADC_HS_TEMP];
}

INT16S	swGetHwVersion1Real(void)
{
	return wAdcRealAvg[cADC_HW_VERSION1];
}

INT16S	swGetHwVersion2Real(void)
{
	return wAdcRealAvg[cADC_HW_VERSION2];
}

INT16S	swGetPackVoltReal(void)
{
	return wAdcRealAvg[cADC_PACK_VOLT];
}

INT16S	swGetBattVoltRealFilt(void)
{
	return wAdcRealAvgFilt[cADC_BATT_VOLT];
}

INT16S	swGetBattCurrRealFilt(void)
{
	return wAdcRealAvgFilt[cADC_BATT_CURR];
}

INT16S	swGetCellVoltRealFilt(INT8U bNo)
{
	return wAdcRealAvgFilt[cADC_CELL_VOLT01+bNo];
}

INT16S	swGetCellTempRealFilt(INT8U bNo)
{
	return wAdcRealAvgFilt[cADC_CELL_TEMP01+bNo];
}

INT16S	swGetCellVolt01Sample(void)
{
	return wAdcSample[cADC_CELL_VOLT01];
}

INT16S	swGetCellVolt02Sample(void)
{
	return wAdcSample[cADC_CELL_VOLT02];
}

INT16S	swGetCellVolt03Sample(void)
{
	return wAdcSample[cADC_CELL_VOLT03];
}

INT16S	swGetCellVolt04Sample(void)
{
	return wAdcSample[cADC_CELL_VOLT04];
}

INT16S	swGetCellVolt05Sample(void)
{
	return wAdcSample[cADC_CELL_VOLT05];
}

INT16S	swGetCellVolt06Sample(void)
{
	return wAdcSample[cADC_CELL_VOLT06];
}

INT16S	swGetCellVolt07Sample(void)
{
	return wAdcSample[cADC_CELL_VOLT07];
}

INT16S	swGetCellVolt08Sample(void)
{
	return wAdcSample[cADC_CELL_VOLT08];
}

INT16S	swGetCellVolt09Sample(void)
{
	return wAdcSample[cADC_CELL_VOLT09];
}

INT16S	swGetCellVolt10Sample(void)
{
	return wAdcSample[cADC_CELL_VOLT10];
}

INT16S	swGetCellVolt11Sample(void)
{
	return wAdcSample[cADC_CELL_VOLT11];
}

INT16S	swGetCellVolt12Sample(void)
{
	return wAdcSample[cADC_CELL_VOLT12];
}

INT16S	swGetCellVolt13Sample(void)
{
	return wAdcSample[cADC_CELL_VOLT13];
}

INT16S	swGetCellVolt14Sample(void)
{
	return wAdcSample[cADC_CELL_VOLT14];
}

INT16S	swGetCellVolt15Sample(void)
{
	return wAdcSample[cADC_CELL_VOLT15];
}

INT16S	swGetCellVolt16Sample(void)
{
	return wAdcSample[cADC_CELL_VOLT16];
}

INT16S	swGetCellTemp01Sample(void)
{
	return wAdcSample[cADC_CELL_TEMP01];
}

INT16S	swGetCellTemp02Sample(void)
{
	return wAdcSample[cADC_CELL_TEMP02];
}

INT16S	swGetCellTemp03Sample(void)
{
	return wAdcSample[cADC_CELL_TEMP03];
}

INT16S	swGetCellTemp04Sample(void)
{
	return wAdcSample[cADC_CELL_TEMP04];
}

INT16S	swGetCellTemp05Sample(void)
{
	return wAdcSample[cADC_CELL_TEMP05];
}

INT16S	swGetCellTemp06Sample(void)
{
	return wAdcSample[cADC_CELL_TEMP06];
}

INT16S	swGetCellTemp07Sample(void)
{
	return wAdcSample[cADC_CELL_TEMP07];
}

INT16S	swGetCellTemp08Sample(void)
{
	return wAdcSample[cADC_CELL_TEMP08];
}

INT16S	swGetCellVolt01Real(void)
{
	return wAdcRealAvg[cADC_CELL_VOLT01];
}

INT16S	swGetCellVolt02Real(void)
{
	return wAdcRealAvg[cADC_CELL_VOLT02];
}

INT16S	swGetCellVolt03Real(void)
{
	return wAdcRealAvg[cADC_CELL_VOLT03];
}

INT16S	swGetCellVolt04Real(void)
{
	return wAdcRealAvg[cADC_CELL_VOLT04];
}

INT16S	swGetCellVolt05Real(void)
{
	return wAdcRealAvg[cADC_CELL_VOLT05];
}

INT16S	swGetCellVolt06Real(void)
{
	return wAdcRealAvg[cADC_CELL_VOLT06];
}

INT16S	swGetCellVolt07Real(void)
{
	return wAdcRealAvg[cADC_CELL_VOLT07];
}

INT16S	swGetCellVolt08Real(void)
{
	return wAdcRealAvg[cADC_CELL_VOLT08];
}

INT16S	swGetCellVolt09Real(void)
{
	return wAdcRealAvg[cADC_CELL_VOLT09];
}

INT16S	swGetCellVolt10Real(void)
{
	return wAdcRealAvg[cADC_CELL_VOLT10];
}

INT16S	swGetCellVolt11Real(void)
{
	return wAdcRealAvg[cADC_CELL_VOLT11];
}

INT16S	swGetCellVolt12Real(void)
{
	return wAdcRealAvg[cADC_CELL_VOLT12];
}

INT16S	swGetCellVolt13Real(void)
{
	return wAdcRealAvg[cADC_CELL_VOLT13];
}

INT16S	swGetCellVolt14Real(void)
{
	return wAdcRealAvg[cADC_CELL_VOLT14];
}

INT16S	swGetCellVolt15Real(void)
{
	return wAdcRealAvg[cADC_CELL_VOLT15];
}

INT16S	swGetCellVolt16Real(void)
{
	return wAdcRealAvg[cADC_CELL_VOLT16];
}

INT16S	swGetCellTemp01Real(void)
{
	return wAdcRealAvg[cADC_CELL_TEMP01];
}

INT16S	swGetCellTemp02Real(void)
{
	return wAdcRealAvg[cADC_CELL_TEMP02];
}

INT16S	swGetCellTemp03Real(void)
{
	return wAdcRealAvg[cADC_CELL_TEMP03];
}

INT16S	swGetCellTemp04Real(void)
{
	return wAdcRealAvg[cADC_CELL_TEMP04];
}

INT16S	swGetCellTemp05Real(void)
{
	return wAdcRealAvg[cADC_CELL_TEMP05];
}

INT16S	swGetCellTemp06Real(void)
{
	return wAdcRealAvg[cADC_CELL_TEMP06];
}

INT16S	swGetCellTemp07Real(void)
{
	return wAdcRealAvg[cADC_CELL_TEMP07];
}

INT16S	swGetCellTemp08Real(void)
{
	return wAdcRealAvg[cADC_CELL_TEMP08];
}

INT16S	swGetBattCurrResult(void)
{
	return 0;	// wAdcResult[cADC_CH_BAT_CURR];
}

INT16U	*swGetAdcSimulationValue(void)
{
	return wAdcResult;
}

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
void	sSetBattVoltAdj(INT16S wAdj)
{
    wBattVoltAdj = ((INT32S)wAdj * cADC_BATT_VOLT_GAIN + 2048) >> 12;
}

void	sSetBattVoltBias(INT16S wBias)
{
    wBattVoltBias = wBias;
}

void	sSetBattChgCurrAdj(INT16S wAdj)
{
	if(swGetBmsCurrSampType() == cBMS_CURR_SAMPLE_15)
	{
		wBattChgCurrAdj = ((INT32S)wAdj * cADC_BATT_CURR_15_GAIN + 2048) >> 12;
	}
	else
	{
		wBattChgCurrAdj = ((INT32S)wAdj * cADC_BATT_CURR_10_GAIN + 2048) >> 12;
	}
}

void	sSetBattChgCurrBias(INT16S wBias)
{
    wBattChgCurrBias = wBias;
}

void	sSetBattDsgCurrAdj(INT16S wAdj)
{
	if(swGetBmsCurrSampType() == cBMS_CURR_SAMPLE_15)
	{
		wBattDsgCurrAdj = ((INT32S)wAdj * cADC_BATT_CURR_15_GAIN + 2048) >> 12;
	}
	else
	{
		wBattDsgCurrAdj = ((INT32S)wAdj * cADC_BATT_CURR_10_GAIN + 2048) >> 12;
	}
}

void	sSetBattDsgCurrBias(INT16S wBias)
{
	wBattDsgCurrBias = wBias;
}

void	sSetCellVoltAdj(INT8U bNo, INT16S wAdj)
{
	wCellVoltAdj[bNo] = ((INT32S)wAdj * cADC_CELL_VOLT_GAIN + 2048) >> 12;
}

void	sSetCellVoltBias(INT8U bNo, INT16S wBias)
{
	wCellVoltBias[bNo] = wBias;
}

void	sSetPackVoltAdj(INT16S wAdj)
{
    wPackVoltAdj = ((INT32S)wAdj * cADC_PACK_VOLT_GAIN + 2048) >> 12;
}

void	sSetPackVoltBias(INT16S wBias)
{
    wPackVoltBias = wBias;
}

void	sSetBattVoltSample(INT16S wVolt)
{
	wAdcSample[cADC_BATT_VOLT] = wVolt;
}

void	sSetBattCurrSample(INT16S wCurr)
{
	wAdcSample[cADC_BATT_CURR] = wCurr;
}

void	sSetCellVoltSample(INT8U bNo, INT16S wVolt)
{
	wAdcSample[cADC_CELL_VOLT01 + bNo] = wVolt;
}

void	sSetCellTempSample(INT8U bNo, INT16S wTemp)
{
	wAdcSample[cADC_CELL_TEMP01 + bNo] = wTemp;
}

