/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		PylontechProtocol.c
Author:			BMSTeam
Date:			2023.07.01
Description:	上海派能能源科技有限公司,逆变器与电池通信协议(V3.5)
********************************************************************************/
#define		__PYLON_PROTOCOL_C__


/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define cPYLON_SOI_BIT			0x00
#define cPYLON_VERH_BIT			0x01
#define cPYLON_VERL_BIT			0x02
#define cPYLON_ADRH_BIT			0x03
#define cPYLON_ADRL_BIT			0x04
#define cPYLON_CID1H_BIT		0x05
#define cPYLON_CID1L_BIT		0x06
#define cPYLON_CID2H_BIT		0x07
#define cPYLON_CID2L_BIT		0x08
#define cPYLON_LENGTHSUM_BIT	0x09
#define cPYLON_LENGTHID3_BIT	0x0A
#define cPYLON_LENGTHID2_BIT	0x0B
#define cPYLON_LENGTHID1_BIT	0x0C

#define cPYLON_SOI_DATA				0x7E

#define cPYLON_CID1_BAT_DATA_H		0x34
#define cPYLON_CID1_BAT_DATA_L		0x36

#define cPYLON_CID2_SYS_ANALOG_H	0x36
#define cPYLON_CID2_SYS_ANALOG_L	0x31

#define cPYLON_CID2_SYS_ALARM_H		0x36
#define cPYLON_CID2_SYS_ALARM_L		0x32

#define cPYLON_CID2_SYS_STATUS_H	0x36
#define cPYLON_CID2_SYS_STATUS_L	0x33


#define cTEMP_K						2731

#define cMODBUS_ADDR_ERR			0x00	// 16

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

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
void sPylonRealTimeDataRead(INT8U *RxBuff,INT8U *TxBuff,INT8U sciid,INT8U RxLen);
void sPylonAlarmDataRead(INT8U *RxBuff,INT8U *TxBuff,INT8U sciid,INT8U RxLen);
void sPylonChgDcgDataRead(INT8U *RxBuff,INT8U *TxBuff,INT8U sciid,INT8U RxLen);
/********************************************************************************
* Routines' implementations														*
********************************************************************************/
// 计算12位ID的校验和
INT8U	sPylonClhksum(INT16U Lendid)
{
	INT8U  ID1,ID2,ID3;
	INT8U  wClhksum;
	INT16U IDSUM;

	//将16位Lendid分解为3个4位ID
	ID3 = (Lendid >> 8) & 0x0F;//高4位
	ID2 = (Lendid & 0x00FF) >> 4;//中间4位
	ID1 = Lendid & 0x000F;//低4位

	//计算ID的和并取模16
	IDSUM = ID1 + ID2 + ID3;
	IDSUM = IDSUM % 16;

	//将结果取反并加一，保留最低的4位作为校验和
	wClhksum = ~IDSUM + 1;
	wClhksum = wClhksum & 0x0F;
	
	//返回校验和
	return wClhksum;
}

//计算数据缓冲区中的校验和
INT16U	sPylonCHKSUM(INT8U *TxBuff,INT8U Length)
{
	INT8U  wTempCnt;
	INT16U wChkSum = 0;
	
	//遍历数据缓冲区中的所有字节（第一个除外）
	for(wTempCnt = 1; wTempCnt < Length; wTempCnt++)
	{
		//将字节的值累加到校验和中
		wChkSum += TxBuff[wTempCnt];
	}
	
	//对校验和取模65536，然后取反加1（是否正确，对CHKSUM的取模是不是为65535）
	wChkSum = wChkSum % 65536;
	wChkSum = ~wChkSum + 1;
	
	//返回校验和
	return wChkSum;
}

void	sPylontechParsing(INT8U sciid)
{
	INT8U	*RxBuff;
	INT8U	*TxBuff;
	INT8U	bSciIdTemp;
	INT8U 	wAddress;
	
	//获取接收和发送缓冲区
	bSciIdTemp = sciid;
	RxBuff = pbGetSciRxBuff(sciid);
	TxBuff = pbGetSciTxBuff(sciid);
	
	//检查帧起始位
	if(RxBuff[cPYLON_SOI_BIT] != cPYLON_SOI_DATA)
	{
		return;
	}
	
//	if((sbGetKeyAddress() == 0) || (sbGetKeyAddress() >= 16))
//	{
//		return;
//	}

//	wAddress = sbGetKeyAddress() + 0x30;

	//检查设备地址
	if((sbGetSciAddress() == 0) || (sbGetSciAddress() >= 16))
	{
		return;
	}
	
	wAddress = sbGetSciAddress() + 0x30;

	if(RxBuff[cPYLON_ADRH_BIT] != wAddress)
	{
		return;
	}
	
	if(RxBuff[cPYLON_ADRL_BIT] != 0x32)
	{
		return;
	}

	//检查CID1数据
	if((RxBuff[cPYLON_CID1H_BIT] != cPYLON_CID1_BAT_DATA_H) \
		|| (RxBuff[cPYLON_CID1L_BIT] != cPYLON_CID1_BAT_DATA_L))
	{
		return;
	}
	
	//根据CID2数据类型调用响应的处理函数
	if((RxBuff[cPYLON_CID2H_BIT] == cPYLON_CID2_SYS_ANALOG_H) \
		&& (RxBuff[cPYLON_CID2L_BIT] == cPYLON_CID2_SYS_ANALOG_L))
	{
		sPylonRealTimeDataRead(RxBuff, TxBuff, bSciIdTemp, 0x62);
	}
	else if((RxBuff[cPYLON_CID2H_BIT] == cPYLON_CID2_SYS_ALARM_H) \
		&& (RxBuff[cPYLON_CID2L_BIT] == cPYLON_CID2_SYS_ALARM_L))
	{
		sPylonAlarmDataRead(RxBuff, TxBuff, bSciIdTemp, 0x08);
	}
	else if((RxBuff[cPYLON_CID2H_BIT] == cPYLON_CID2_SYS_STATUS_H) \
		&& (RxBuff[cPYLON_CID2L_BIT] == cPYLON_CID2_SYS_STATUS_L))
	{
		sPylonChgDcgDataRead(RxBuff, TxBuff, bSciIdTemp, 0x12);
	}
	else
	{
		return;
	}
	
}

/**
 * @brief 读取Pylon实时数据并将其写入响应缓冲区
 * 
 * @param RxBuff 接收缓冲区指针
 * @param TxBuff 响应缓冲区指针
 * @param sciid 串口ID
 * @param RxLen 接收长度
 */
void	sPylonRealTimeDataRead(INT8U *RxBuff,INT8U *TxBuff,INT8U sciid,INT8U RxLen)
{
	INT8U wID1,wID2,wID3;
	INT8U TxLen;
	INT16S wTemp, TempMin, TempMax;
	INT32S dwTemp;
	
	//从接收长度中提取ID1,ID2,ID3
	wID3 = (RxLen >> 8) & 0x0F;
	wID2 = (RxLen & 0x00FF) >> 4;
	wID1 = RxLen & 0x000F;
	
	//将接收缓冲区的部分数据复制到响应缓冲区
	TxBuff[cPYLON_SOI_BIT] = RxBuff[cPYLON_SOI_BIT];
	TxBuff[cPYLON_VERH_BIT] = RxBuff[cPYLON_VERH_BIT];
	TxBuff[cPYLON_VERL_BIT] = RxBuff[cPYLON_VERL_BIT];
	TxBuff[cPYLON_ADRH_BIT] = RxBuff[cPYLON_ADRH_BIT];
	TxBuff[cPYLON_ADRL_BIT] = RxBuff[cPYLON_ADRL_BIT];
	TxBuff[cPYLON_CID1H_BIT] = RxBuff[cPYLON_CID1H_BIT];
	TxBuff[cPYLON_CID1L_BIT] = RxBuff[cPYLON_CID1L_BIT];
	TxBuff[cPYLON_CID2H_BIT] = '0';//设置为固定值
	TxBuff[cPYLON_CID2L_BIT] = '0';//设置为固定值

	//将校验和长度ID3、ID2、ID1转化为ASCII码写入响应缓冲区
	sNumToAscii_16Hex(sPylonClhksum(RxLen),&TxBuff[cPYLON_LENGTHSUM_BIT]);
	sNumToAscii_16Hex(wID3,&TxBuff[cPYLON_LENGTHID3_BIT]);
	sNumToAscii_16Hex(wID2,&TxBuff[cPYLON_LENGTHID2_BIT]);
	sNumToAscii_16Hex(wID1,&TxBuff[cPYLON_LENGTHID1_BIT]);

	//设置响应长度
	TxLen = cPYLON_LENGTHID1_BIT + 1;
	
	//INFO
	// 电池组系统总平均电压 0.001V
	dwTemp = swGetBattVoltFiltNew();
	dwTemp *= 10;
	if(dwTemp > 0xFFFF) dwTemp = 0xFFFF;
	sbNumToXXXX_16Hex((INT16U)(dwTemp & 0xFFFF),&TxBuff[TxLen]);
//	dwTemp = swGetEmsDataTotalVoltage();
//	dwTemp *= 100;
//	if(dwTemp > 0xFFFF) dwTemp = 0xFFFF;
//	sbNumToXXXX_16Hex((INT16U)(dwTemp & 0xFFFF),&TxBuff[TxLen]);
	TxLen += 4;
	// 电池组系统总电流 0.01A
	wTemp = swGetBattCurrFiltNew() * 10;
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
//	dwTemp = swGetEmsDataTotalCurrent();
//	dwTemp *= 10;
//	if(dwTemp > 32000) dwTemp = 32000;
//	else if(dwTemp < -32000) dwTemp = -32000;
//	sbNumToXXXX_16Hex((INT16U)(dwTemp & 0xFFFF),&TxBuff[TxLen]);
	TxLen += 4;
	// 电池组系统 SOC 1%
	wTemp = swGetSocNew();
	//wTemp = swGetEmsDataTotalSOC()/10;
	sbNumToXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 2;
	// 平均循环次数
	wTemp = swGetEepromBattCycCnt();
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// 最大循环次数
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// 平均SOH 1%
	wTemp = swGetSohNew();
	//wTemp = swGetEmsDataTotalSOH()/10;
	sbNumToXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 2;
	// 最小SOH 1%
	sbNumToXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 2;
	// 单芯最高电压
	wTemp = swGetMaxCellVoltNew();
	//wTemp = swGetEmsDataMaximumCellVoltage();
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// 单芯最高电压所在模块
	wTemp = sbGetMaxCellVoltNo();
	//wTemp = swGetEmsDataMaximumCellVoltageNo();
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// 单芯最低电压
	wTemp = swGetMinCellVoltNew();
	//wTemp = swGetEmsDataMinimumCellVoltage();
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// 单芯最低电压所在模块
	wTemp = sbGetMinCellVoltNo();
	//wTemp = swGetEmsDataMinimumCellVoltageNo();
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// 单芯平均温度 1K
	TempMax = (swGetMaxCellTempNew() * 10) + cTEMP_K;
	TempMin = (swGetMinCellTempNew() * 10) + cTEMP_K;
	//TempMax = swGetEmsDataMaximumCellTemperature() + cTEMP_K;
	//TempMin = swGetEmsDataMinimumCellTemperature() + cTEMP_K;
	sbNumToXXXX_16Hex((TempMax + TempMin) >> 1,&TxBuff[TxLen]);
	TxLen += 4;
	// 单芯最高温度 1K
	sbNumToXXXX_16Hex(TempMax,&TxBuff[TxLen]);
	TxLen += 4;
	// 单芯最高温度所在模块
	wTemp = sbGetMaxCellTempNo();
	//wTemp = swGetEmsDataMaximumCellTemperatureNo();
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// 单芯最低温度 1K
	sbNumToXXXX_16Hex(TempMin,&TxBuff[TxLen]);
	TxLen += 4;
	// 单芯最低温度所在模块
	wTemp = sbGetMinCellTempNo();
	//wTemp = swGetEmsDataMinimumCellTemperatureNo();
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// MOSFET平均温度 1K
	wTemp = (swGetHsTempNew() * 10) + cTEMP_K;
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// MOSFET最高温度 1K
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// MOSFET最高温度所在模块
	sbNumToXXXX_16Hex(0x0306,&TxBuff[TxLen]);
	TxLen += 4;
	// MOSFET最低温度 1K
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// MOSFET最低温度所在模块
	sbNumToXXXX_16Hex(0x0106,&TxBuff[TxLen]);
	TxLen += 4;
	// BMS平均温度 1K
	sbNumToXXXX_16Hex((250+cTEMP_K),&TxBuff[TxLen]);
	TxLen += 4;
	// BMS最高温度 1K
	sbNumToXXXX_16Hex((250+cTEMP_K),&TxBuff[TxLen]);
	TxLen += 4;
	// BMS最高温度所在模块
	sbNumToXXXX_16Hex(0x0307,&TxBuff[TxLen]);
	TxLen += 4;
	// BMS最低温度 1K
	sbNumToXXXX_16Hex((250+cTEMP_K),&TxBuff[TxLen]);
	TxLen += 4;
	// BMS最低温度所在模块
	sbNumToXXXX_16Hex(0x0107,&TxBuff[TxLen]);
	TxLen += 4;
	
	//CHKSUM
	sbNumToXXXX_16Hex(sPylonCHKSUM(TxBuff,TxLen),&TxBuff[TxLen]);
	TxLen += 4;
	
	TxBuff[TxLen] = 0x0D;
	TxLen += 1;
	
	if(sSciGetTxStatus(sciid) != cSciTxBusy)
	{
		sSciWrite(sciid, TxBuff, TxLen);
	}
}

void	sPylonAlarmDataRead(INT8U *RxBuff,INT8U *TxBuff,INT8U sciid,INT8U RxLen)
{
	INT8U wID1,wID2,wID3;
	INT8U TxLen;
	INT8U wAlarmstatus_1 = 0;
	INT8U wAlarmstatus_2 = 0;
	INT8U wProtecstatus_1 = 0;
	INT8U wProtecstatus_2 = 0;
	
	//提取RxLen的高4位，中间4位，低4位分别存入WID3、WID2、WID3
	wID3 = (RxLen >> 8) & 0x0F;
	wID2 = (RxLen & 0x00FF) >> 4;
	wID1 = RxLen & 0x000F;
	
	//将提取RxBuff中的部分数据复制到TxBuff中
	TxBuff[cPYLON_SOI_BIT] = RxBuff[cPYLON_SOI_BIT];
	TxBuff[cPYLON_VERH_BIT] = RxBuff[cPYLON_VERH_BIT];
	TxBuff[cPYLON_VERL_BIT] = RxBuff[cPYLON_VERL_BIT];
	TxBuff[cPYLON_ADRH_BIT] = RxBuff[cPYLON_ADRH_BIT];
	TxBuff[cPYLON_ADRL_BIT] = RxBuff[cPYLON_ADRL_BIT];
	TxBuff[cPYLON_CID1H_BIT] = RxBuff[cPYLON_CID1H_BIT];
	TxBuff[cPYLON_CID1L_BIT] = RxBuff[cPYLON_CID1L_BIT];
	TxBuff[cPYLON_CID2H_BIT] = '0';
	TxBuff[cPYLON_CID2L_BIT] = '0';
	sNumToAscii_16Hex(sPylonClhksum(RxLen),&TxBuff[cPYLON_LENGTHSUM_BIT]);
	sNumToAscii_16Hex(wID3,&TxBuff[cPYLON_LENGTHID3_BIT]);
	sNumToAscii_16Hex(wID2,&TxBuff[cPYLON_LENGTHID2_BIT]);
	sNumToAscii_16Hex(wID1,&TxBuff[cPYLON_LENGTHID1_BIT]);
	TxLen = cPYLON_LENGTHID1_BIT + 1;
	
	//INFO
	//计算报警状态
	wAlarmstatus_1 = ((swGetBmsAlarmFlag() & cBattVoltHiAlarm)<<7) \
		+ ((swGetBmsAlarmFlag() & cBattVoltLoAlarm)<<6) \
		+ ((swGetBmsAlarmFlag() & cCellVoltHiAlarm)<<5) \
		+ ((swGetBmsAlarmFlag() & cCellVoltLoAlarm)<<4) \
		+ ((swGetBmsAlarmFlag() & cCellTempHiAlarm)<<3) \
		+ ((swGetBmsAlarmFlag() & cCellTempLoAlarm)<<2) \
		+ ((swGetBmsAlarmFlag() & cHsTempHiAlarm)<<1);
	wAlarmstatus_2 = ((swGetBmsAlarmFlag() & cBattChgCurrHiAlarm)<<6) \
		+ ((swGetBmsAlarmFlag() & cBattDcgCurrHiAlarm)<<5) \
		+ ((swGetBmsAlarmFlag() & cAdcTimingAlarm)<<4);
	//计算保护状态
	wProtecstatus_1 = ((swGetBmsFaultFlag() & cBattVoltHiFault)<<7) \
		+ ((swGetBmsFaultFlag() & cBattVoltLoFault)<<6) \
		+ ((swGetBmsFaultFlag() & cCellVoltHiFault)<<5) \
		+ ((swGetBmsFaultFlag() & cCellVoltLoFault)<<4) \
		+ ((swGetBmsFaultFlag() & cCellTempHiFault)<<3) \
		+ ((swGetBmsFaultFlag() & cCellTempLoFault)<<2) \
		+ ((swGetBmsFaultFlag() & cHsTempHiFault)<<1);
	wProtecstatus_2 = ((swGetBmsFaultFlag() & cBattChgCurrHiFault)<<6) \
		+ ((swGetBmsFaultFlag() & cBattDcgCurrHiFault)<<5) \
		+ ((swGetBmsFaultFlag() & cSampSensorFault)<<3);

//	wAlarmstatus_1 = ((swGetEmsDataAlarmFlagLow() & cBattVoltHiAlarm)<<7) \
//		+ ((swGetEmsDataAlarmFlagLow() & cBattVoltLoAlarm)<<6) \
//		+ ((swGetEmsDataAlarmFlagLow() & cCellVoltHiAlarm)<<5) \
//		+ ((swGetEmsDataAlarmFlagLow() & cCellVoltLoAlarm)<<4) \
//		+ ((swGetEmsDataAlarmFlagLow() & cCellTempHiAlarm)<<3) \
//		+ ((swGetEmsDataAlarmFlagLow() & cCellTempLoAlarm)<<2) \
//		+ ((swGetEmsDataAlarmFlagLow() & cHsTempHiAlarm)<<1);
//	wAlarmstatus_2 = ((swGetEmsDataAlarmFlagLow() & cBattChgCurrHiAlarm)<<6) \
//		+ ((swGetEmsDataAlarmFlagLow() & cBattDcgCurrHiAlarm)<<5) \
//		+ ((swGetEmsDataAlarmFlagLow() & cAdcTimingAlarm)<<4);
//	wProtecstatus_1 = ((swGetEmsDataFaultFlagLow() & cBattVoltHiFault)<<7) \
//		+ ((swGetEmsDataFaultFlagLow() & cBattVoltLoFault)<<6) \
//		+ ((swGetEmsDataFaultFlagLow() & cCellVoltHiFault)<<5) \
//		+ ((swGetEmsDataFaultFlagLow() & cCellVoltLoFault)<<4) \
//		+ ((swGetEmsDataFaultFlagLow() & cCellTempHiFault)<<3) \
//		+ ((swGetEmsDataFaultFlagLow() & cCellTempLoFault)<<2) \
//		+ ((swGetEmsDataFaultFlagLow() & cHsTempHiFault)<<1);
//	wProtecstatus_2 = ((swGetEmsDataFaultFlagLow() & cBattChgCurrHiFault)<<6) \
//		+ ((swGetEmsDataFaultFlagLow() & cBattDcgCurrHiFault)<<5) \
//		+ ((swGetEmsDataFaultFlagLow() & cSampSensorFault)<<3);

	//将报警状态1转换为16进制ADCII码并存储到TxBuff中，增加TxLen计数，下面几个同理
	sbNumToXX_16Hex(wAlarmstatus_1,&TxBuff[TxLen]);
	TxLen += 2;
	sbNumToXX_16Hex(wAlarmstatus_2,&TxBuff[TxLen]);
	TxLen += 2;
	sbNumToXX_16Hex(wProtecstatus_1,&TxBuff[TxLen]);
	TxLen += 2;
	sbNumToXX_16Hex(wProtecstatus_2,&TxBuff[TxLen]);
	TxLen += 2;
	
	//CHKSUM
	//计算并存储校验和到TxBuff中，增加TxLen计数
	sNumToAscii_16Hex(sPylonCHKSUM(&TxBuff[0],TxLen),&TxBuff[TxLen]);
	TxLen += 4;
	
	//在消息末尾添加回车符，增加TxLen计数
	TxBuff[TxLen] = 0x0D;
	TxLen += 1;
	
	if(sSciGetTxStatus(sciid) != cSciTxBusy)
	{
		sSciWrite(sciid, TxBuff, TxLen);
	}
}


void	sPylonChgDcgDataRead(INT8U *RxBuff, INT8U *TxBuff, INT8U sciid, INT8U RxLen) 
{
	INT8U wID1,wID2,wID3;
	INT8U TxLen;
	INT8U bStatus;
//	INT32S dwTemp;
	//从RxLen中提取高4位、中4位、低4位存入wID3、wID2、wID1
	wID3 = (RxLen >> 8) & 0x0F;
	wID2 = (RxLen & 0x00FF) >> 4;
	wID1 = RxLen & 0x000F;
	
	//将RxBuff中的部分数据复制到TxBuff中
	TxBuff[cPYLON_SOI_BIT] = RxBuff[cPYLON_SOI_BIT];
	TxBuff[cPYLON_VERH_BIT] = RxBuff[cPYLON_VERH_BIT];
	TxBuff[cPYLON_VERL_BIT] = RxBuff[cPYLON_VERL_BIT];
	TxBuff[cPYLON_ADRH_BIT] = RxBuff[cPYLON_ADRH_BIT];
	TxBuff[cPYLON_ADRL_BIT] = RxBuff[cPYLON_ADRL_BIT];
	TxBuff[cPYLON_CID1H_BIT] = RxBuff[cPYLON_CID1H_BIT];
	TxBuff[cPYLON_CID1L_BIT] = RxBuff[cPYLON_CID1L_BIT];
	TxBuff[cPYLON_CID2H_BIT] = '0';
	TxBuff[cPYLON_CID2L_BIT] = '0';
	sNumToAscii_16Hex(sPylonClhksum(RxLen),&TxBuff[cPYLON_LENGTHSUM_BIT]);
	sNumToAscii_16Hex(wID3,&TxBuff[cPYLON_LENGTHID3_BIT]);
	sNumToAscii_16Hex(wID2,&TxBuff[cPYLON_LENGTHID2_BIT]);
	sNumToAscii_16Hex(wID1,&TxBuff[cPYLON_LENGTHID1_BIT]);
	TxLen = cPYLON_LENGTHID1_BIT + 1;
	
	//INFO	
	//将充电和放电信息转化位16进制ASCII码并存储到TxBuff中，增加TxLen计数
	sbNumToXXXX_16Hex(swGetRecChargeVolt() * 10, &TxBuff[TxLen]);
	TxLen += 4;
	sbNumToXXXX_16Hex(swGetRecCutoffVolt() * 10, &TxBuff[TxLen]);
	TxLen += 4;
	sbNumToXXXX_16Hex(swGetRecChargeCurr(), &TxBuff[TxLen]);
	TxLen += 4;
	sbNumToXXXX_16Hex(swGetRecDischargeCurr(), &TxBuff[TxLen]);
	TxLen += 4;
	//初始化状态变量为0
	bStatus = 0;、

	//根据BMS的充电、放电状态以及其他参数设置状态变量的各位
	bStatus += (swGetBmsChargeEnable() << 7);             // 将充电使能状态左移7位后加到bStatus中
	bStatus += (swGetBmsDischargeEnable() << 6);          // 将放电使能状态左移6位后加到bStatus中
	bStatus += (swGetBmsChargeImmediately1() << 5);       // 将立即充电状态左移5位后加到bStatus中
	bStatus += (swGetBmsFullCharge() << 4);               // 将完全充电状态左移4位后加到bStatus中
	
	//将状态变量转化为16进制ASCII码并存储到TxBuff中，增加TxLen
	sbNumToXX_16Hex(bStatus,&TxBuff[TxLen]);
	TxLen += 2;
//	dwTemp = swGetEmsDataChargeVoltageLimit();
//	dwTemp *= 100;
//	if(dwTemp > 0xFFFF) dwTemp = 0xFFFF;
//	else if(dwTemp < 0) dwTemp = 0;
//	sbNumToXXXX_16Hex((INT16U)(dwTemp&0xFFFF), &TxBuff[TxLen]);
//	TxLen += 4;
//	dwTemp = swGetEmsDataDischargeVoltageLimit();
//	dwTemp *= 100;
//	if(dwTemp > 0xFFFF) dwTemp = 0xFFFF;
//	else if(dwTemp < 0) dwTemp = 0;
//	sbNumToXXXX_16Hex((INT16U)(dwTemp&0xFFFF), &TxBuff[TxLen]);
//	TxLen += 4;
//	dwTemp = swGetEmsDataChargeCurrentLimit();
//	if(dwTemp > 0xFFFF) dwTemp = 0xFFFF;
//	else if(dwTemp < 0) dwTemp = 0;
//	sbNumToXXXX_16Hex((INT16U)(dwTemp&0xFFFF), &TxBuff[TxLen]);
//	TxLen += 4;
//	dwTemp = swGetEmsDataDischargeCurrentLimit();
//	if(dwTemp > 0xFFFF) dwTemp = 0xFFFF;
//	else if(dwTemp < 0) dwTemp = 0;
//	sbNumToXXXX_16Hex((INT16U)(dwTemp&0xFFFF), &TxBuff[TxLen]);
//	TxLen += 4;
//	bStatus = 0;
//	bStatus += (swGetEmsStatusChargeEnable() << 7);
//	bStatus += (swGetEmsStatusDischargeEnable() << 6);
//	bStatus += (swGetEmsStatusChargeImmediately1() << 5);
//	bStatus += (swGetEmsStatusFullCharge() << 4);
//	sbNumToXX_16Hex(bStatus,&TxBuff[TxLen]);
//	TxLen += 2;	

	//CHKSUM
	// 计算并存储校验和到TxBuff中，增加TxLen计数
	sbNumToXXXX_16Hex(sPylonCHKSUM(&TxBuff[0],TxLen),&TxBuff[TxLen]);
	TxLen += 4;
	
	TxBuff[TxLen] = 0x0D;
	TxLen += 1;
	
	if(sSciGetTxStatus(sciid) != cSciTxBusy)
	{
		sSciWrite(sciid, TxBuff, TxLen);
	}
}



/********************************************************************************
* Output interface Routines														*
********************************************************************************/


/********************************************************************************
* Input interface Routines														*
********************************************************************************/


