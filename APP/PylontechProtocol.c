/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		PylontechProtocol.c
Author:			BMSTeam
Date:			2023.07.01
Description:	�Ϻ�������Դ�Ƽ����޹�˾,���������ͨ��Э��(V3.5)
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
// ����12λID��У���
INT8U	sPylonClhksum(INT16U Lendid)
{
	INT8U  ID1,ID2,ID3;
	INT8U  wClhksum;
	INT16U IDSUM;

	//��16λLendid�ֽ�Ϊ3��4λID
	ID3 = (Lendid >> 8) & 0x0F;//��4λ
	ID2 = (Lendid & 0x00FF) >> 4;//�м�4λ
	ID1 = Lendid & 0x000F;//��4λ

	//����ID�ĺͲ�ȡģ16
	IDSUM = ID1 + ID2 + ID3;
	IDSUM = IDSUM % 16;

	//�����ȡ������һ��������͵�4λ��ΪУ���
	wClhksum = ~IDSUM + 1;
	wClhksum = wClhksum & 0x0F;
	
	//����У���
	return wClhksum;
}

//�������ݻ������е�У���
INT16U	sPylonCHKSUM(INT8U *TxBuff,INT8U Length)
{
	INT8U  wTempCnt;
	INT16U wChkSum = 0;
	
	//�������ݻ������е������ֽڣ���һ�����⣩
	for(wTempCnt = 1; wTempCnt < Length; wTempCnt++)
	{
		//���ֽڵ�ֵ�ۼӵ�У�����
		wChkSum += TxBuff[wTempCnt];
	}
	
	//��У���ȡģ65536��Ȼ��ȡ����1���Ƿ���ȷ����CHKSUM��ȡģ�ǲ���Ϊ65535��
	wChkSum = wChkSum % 65536;
	wChkSum = ~wChkSum + 1;
	
	//����У���
	return wChkSum;
}

void	sPylontechParsing(INT8U sciid)
{
	INT8U	*RxBuff;
	INT8U	*TxBuff;
	INT8U	bSciIdTemp;
	INT8U 	wAddress;
	
	//��ȡ���պͷ��ͻ�����
	bSciIdTemp = sciid;
	RxBuff = pbGetSciRxBuff(sciid);
	TxBuff = pbGetSciTxBuff(sciid);
	
	//���֡��ʼλ
	if(RxBuff[cPYLON_SOI_BIT] != cPYLON_SOI_DATA)
	{
		return;
	}
	
//	if((sbGetKeyAddress() == 0) || (sbGetKeyAddress() >= 16))
//	{
//		return;
//	}

//	wAddress = sbGetKeyAddress() + 0x30;

	//����豸��ַ
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

	//���CID1����
	if((RxBuff[cPYLON_CID1H_BIT] != cPYLON_CID1_BAT_DATA_H) \
		|| (RxBuff[cPYLON_CID1L_BIT] != cPYLON_CID1_BAT_DATA_L))
	{
		return;
	}
	
	//����CID2�������͵�����Ӧ�Ĵ�����
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
 * @brief ��ȡPylonʵʱ���ݲ�����д����Ӧ������
 * 
 * @param RxBuff ���ջ�����ָ��
 * @param TxBuff ��Ӧ������ָ��
 * @param sciid ����ID
 * @param RxLen ���ճ���
 */
void	sPylonRealTimeDataRead(INT8U *RxBuff,INT8U *TxBuff,INT8U sciid,INT8U RxLen)
{
	INT8U wID1,wID2,wID3;
	INT8U TxLen;
	INT16S wTemp, TempMin, TempMax;
	INT32S dwTemp;
	
	//�ӽ��ճ�������ȡID1,ID2,ID3
	wID3 = (RxLen >> 8) & 0x0F;
	wID2 = (RxLen & 0x00FF) >> 4;
	wID1 = RxLen & 0x000F;
	
	//�����ջ������Ĳ������ݸ��Ƶ���Ӧ������
	TxBuff[cPYLON_SOI_BIT] = RxBuff[cPYLON_SOI_BIT];
	TxBuff[cPYLON_VERH_BIT] = RxBuff[cPYLON_VERH_BIT];
	TxBuff[cPYLON_VERL_BIT] = RxBuff[cPYLON_VERL_BIT];
	TxBuff[cPYLON_ADRH_BIT] = RxBuff[cPYLON_ADRH_BIT];
	TxBuff[cPYLON_ADRL_BIT] = RxBuff[cPYLON_ADRL_BIT];
	TxBuff[cPYLON_CID1H_BIT] = RxBuff[cPYLON_CID1H_BIT];
	TxBuff[cPYLON_CID1L_BIT] = RxBuff[cPYLON_CID1L_BIT];
	TxBuff[cPYLON_CID2H_BIT] = '0';//����Ϊ�̶�ֵ
	TxBuff[cPYLON_CID2L_BIT] = '0';//����Ϊ�̶�ֵ

	//��У��ͳ���ID3��ID2��ID1ת��ΪASCII��д����Ӧ������
	sNumToAscii_16Hex(sPylonClhksum(RxLen),&TxBuff[cPYLON_LENGTHSUM_BIT]);
	sNumToAscii_16Hex(wID3,&TxBuff[cPYLON_LENGTHID3_BIT]);
	sNumToAscii_16Hex(wID2,&TxBuff[cPYLON_LENGTHID2_BIT]);
	sNumToAscii_16Hex(wID1,&TxBuff[cPYLON_LENGTHID1_BIT]);

	//������Ӧ����
	TxLen = cPYLON_LENGTHID1_BIT + 1;
	
	//INFO
	// �����ϵͳ��ƽ����ѹ 0.001V
	dwTemp = swGetBattVoltFiltNew();
	dwTemp *= 10;
	if(dwTemp > 0xFFFF) dwTemp = 0xFFFF;
	sbNumToXXXX_16Hex((INT16U)(dwTemp & 0xFFFF),&TxBuff[TxLen]);
//	dwTemp = swGetEmsDataTotalVoltage();
//	dwTemp *= 100;
//	if(dwTemp > 0xFFFF) dwTemp = 0xFFFF;
//	sbNumToXXXX_16Hex((INT16U)(dwTemp & 0xFFFF),&TxBuff[TxLen]);
	TxLen += 4;
	// �����ϵͳ�ܵ��� 0.01A
	wTemp = swGetBattCurrFiltNew() * 10;
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
//	dwTemp = swGetEmsDataTotalCurrent();
//	dwTemp *= 10;
//	if(dwTemp > 32000) dwTemp = 32000;
//	else if(dwTemp < -32000) dwTemp = -32000;
//	sbNumToXXXX_16Hex((INT16U)(dwTemp & 0xFFFF),&TxBuff[TxLen]);
	TxLen += 4;
	// �����ϵͳ SOC 1%
	wTemp = swGetSocNew();
	//wTemp = swGetEmsDataTotalSOC()/10;
	sbNumToXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 2;
	// ƽ��ѭ������
	wTemp = swGetEepromBattCycCnt();
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// ���ѭ������
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// ƽ��SOH 1%
	wTemp = swGetSohNew();
	//wTemp = swGetEmsDataTotalSOH()/10;
	sbNumToXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 2;
	// ��СSOH 1%
	sbNumToXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 2;
	// ��о��ߵ�ѹ
	wTemp = swGetMaxCellVoltNew();
	//wTemp = swGetEmsDataMaximumCellVoltage();
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// ��о��ߵ�ѹ����ģ��
	wTemp = sbGetMaxCellVoltNo();
	//wTemp = swGetEmsDataMaximumCellVoltageNo();
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// ��о��͵�ѹ
	wTemp = swGetMinCellVoltNew();
	//wTemp = swGetEmsDataMinimumCellVoltage();
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// ��о��͵�ѹ����ģ��
	wTemp = sbGetMinCellVoltNo();
	//wTemp = swGetEmsDataMinimumCellVoltageNo();
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// ��оƽ���¶� 1K
	TempMax = (swGetMaxCellTempNew() * 10) + cTEMP_K;
	TempMin = (swGetMinCellTempNew() * 10) + cTEMP_K;
	//TempMax = swGetEmsDataMaximumCellTemperature() + cTEMP_K;
	//TempMin = swGetEmsDataMinimumCellTemperature() + cTEMP_K;
	sbNumToXXXX_16Hex((TempMax + TempMin) >> 1,&TxBuff[TxLen]);
	TxLen += 4;
	// ��о����¶� 1K
	sbNumToXXXX_16Hex(TempMax,&TxBuff[TxLen]);
	TxLen += 4;
	// ��о����¶�����ģ��
	wTemp = sbGetMaxCellTempNo();
	//wTemp = swGetEmsDataMaximumCellTemperatureNo();
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// ��о����¶� 1K
	sbNumToXXXX_16Hex(TempMin,&TxBuff[TxLen]);
	TxLen += 4;
	// ��о����¶�����ģ��
	wTemp = sbGetMinCellTempNo();
	//wTemp = swGetEmsDataMinimumCellTemperatureNo();
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// MOSFETƽ���¶� 1K
	wTemp = (swGetHsTempNew() * 10) + cTEMP_K;
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// MOSFET����¶� 1K
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// MOSFET����¶�����ģ��
	sbNumToXXXX_16Hex(0x0306,&TxBuff[TxLen]);
	TxLen += 4;
	// MOSFET����¶� 1K
	sbNumToXXXX_16Hex(wTemp,&TxBuff[TxLen]);
	TxLen += 4;
	// MOSFET����¶�����ģ��
	sbNumToXXXX_16Hex(0x0106,&TxBuff[TxLen]);
	TxLen += 4;
	// BMSƽ���¶� 1K
	sbNumToXXXX_16Hex((250+cTEMP_K),&TxBuff[TxLen]);
	TxLen += 4;
	// BMS����¶� 1K
	sbNumToXXXX_16Hex((250+cTEMP_K),&TxBuff[TxLen]);
	TxLen += 4;
	// BMS����¶�����ģ��
	sbNumToXXXX_16Hex(0x0307,&TxBuff[TxLen]);
	TxLen += 4;
	// BMS����¶� 1K
	sbNumToXXXX_16Hex((250+cTEMP_K),&TxBuff[TxLen]);
	TxLen += 4;
	// BMS����¶�����ģ��
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
	
	//��ȡRxLen�ĸ�4λ���м�4λ����4λ�ֱ����WID3��WID2��WID3
	wID3 = (RxLen >> 8) & 0x0F;
	wID2 = (RxLen & 0x00FF) >> 4;
	wID1 = RxLen & 0x000F;
	
	//����ȡRxBuff�еĲ������ݸ��Ƶ�TxBuff��
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
	//���㱨��״̬
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
	//���㱣��״̬
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

	//������״̬1ת��Ϊ16����ADCII�벢�洢��TxBuff�У�����TxLen���������漸��ͬ��
	sbNumToXX_16Hex(wAlarmstatus_1,&TxBuff[TxLen]);
	TxLen += 2;
	sbNumToXX_16Hex(wAlarmstatus_2,&TxBuff[TxLen]);
	TxLen += 2;
	sbNumToXX_16Hex(wProtecstatus_1,&TxBuff[TxLen]);
	TxLen += 2;
	sbNumToXX_16Hex(wProtecstatus_2,&TxBuff[TxLen]);
	TxLen += 2;
	
	//CHKSUM
	//���㲢�洢У��͵�TxBuff�У�����TxLen����
	sNumToAscii_16Hex(sPylonCHKSUM(&TxBuff[0],TxLen),&TxBuff[TxLen]);
	TxLen += 4;
	
	//����Ϣĩβ��ӻس���������TxLen����
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
	//��RxLen����ȡ��4λ����4λ����4λ����wID3��wID2��wID1
	wID3 = (RxLen >> 8) & 0x0F;
	wID2 = (RxLen & 0x00FF) >> 4;
	wID1 = RxLen & 0x000F;
	
	//��RxBuff�еĲ������ݸ��Ƶ�TxBuff��
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
	//�����ͷŵ���Ϣת��λ16����ASCII�벢�洢��TxBuff�У�����TxLen����
	sbNumToXXXX_16Hex(swGetRecChargeVolt() * 10, &TxBuff[TxLen]);
	TxLen += 4;
	sbNumToXXXX_16Hex(swGetRecCutoffVolt() * 10, &TxBuff[TxLen]);
	TxLen += 4;
	sbNumToXXXX_16Hex(swGetRecChargeCurr(), &TxBuff[TxLen]);
	TxLen += 4;
	sbNumToXXXX_16Hex(swGetRecDischargeCurr(), &TxBuff[TxLen]);
	TxLen += 4;
	//��ʼ��״̬����Ϊ0
	bStatus = 0;��

	//����BMS�ĳ�硢�ŵ�״̬�Լ�������������״̬�����ĸ�λ
	bStatus += (swGetBmsChargeEnable() << 7);             // �����ʹ��״̬����7λ��ӵ�bStatus��
	bStatus += (swGetBmsDischargeEnable() << 6);          // ���ŵ�ʹ��״̬����6λ��ӵ�bStatus��
	bStatus += (swGetBmsChargeImmediately1() << 5);       // ���������״̬����5λ��ӵ�bStatus��
	bStatus += (swGetBmsFullCharge() << 4);               // ����ȫ���״̬����4λ��ӵ�bStatus��
	
	//��״̬����ת��Ϊ16����ASCII�벢�洢��TxBuff�У�����TxLen
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
	// ���㲢�洢У��͵�TxBuff�У�����TxLen����
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


