/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		PylontechProtocolCan.c
Author:			BMSTeam
Date:			2023.07.01
Description:	�Ϻ�������Դ�Ƽ����޹�˾,���������ͨ��Э��(CAN)
********************************************************************************/
#define		__PYLON_PROTOCOL_CAN_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define cPYLON_CAN_ALARM_ID				0x359
#define cPYLON_CAN_LIMIT_ID				0x351
#define cPYLON_CAN_SOC_SOH_ID			0x355
#define cPYLON_CAN_ANALOG_ID			0x356
#define cPYLON_CAN_STATE_ID				0x35C
#define cPYLON_CAN_BRAND_ID				0x35E


/********************************************************************************
* Constants																		*
********************************************************************************/


/********************************************************************************
* External variables															*
********************************************************************************/
extern INT8U   bCanParallelRole;

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


/********************************************************************************
* Routines' implementations														*
********************************************************************************/
//Notice:Filter2 * 6 < Filter1
// ����һ������ sPylonProtCanPolling�����ڴ��� Pylon �� CAN ������ѯ
void sPylonProtCanPolling(INT8U canid, INT8U Filter1, INT8U Filter2)
{
    INT16U data; // 16λ�޷������� data
    T_CAN_FRAME strFrame; // CAN ֡�ṹ�� strFrame
    static INT8U wCnt1 = 0; // ��̬���� wCnt1 ���ڼ���
    static INT8U wCnt2 = 0; // ��̬���� wCnt2 ���ڼ���
    static INT8U wCanTxFlag = 0; // ��̬���� wCanTxFlag ��־�Ƿ���Է�������
    static INT8U wParallelNum = 0; // ��̬���� wParallelNum ���ڱ�־������
    union BMS_STATUS_TABLE wBmsStatus; // BMS ״̬�������� wBmsStatus
    INT16U wStatus; // 16λ�޷������� wStatus
    
    // ��� CAN ID ���� cPCS_CAN��ֱ�ӷ���
    if (canid != cPCS_CAN)
    {
        return;
    }
    
    // �����ǰ�豸���� CAN ������ֱ�ӷ���
    if (sbIsCanDeviceHost() == 0)
    {
        return;
    }
    
    // ��� BMS ģʽΪ����ģʽ��ػ�ģʽ��ֱ�ӷ���
    if ((bBmsMode == cPowerOnMode) || (bBmsMode == cShutdownMode))
    {
        return;
    }
    
    // ÿ���� Filter1 ����ѯ������ wCanTxFlag Ϊ�ɷ���״̬
    if (++wCnt1 > Filter1)
    {
        wCnt1 = 0;
        wCanTxFlag = 1;
    }
    
    // ��� wCanTxFlag Ϊ�ɷ���״̬��ִ�����²���
    if (wCanTxFlag > 0)
    {
        // ÿ���� Filter2 ����ѯ��wCanTxFlag ��1
        if (++wCnt2 > Filter2)
        {
            wCnt2 = 0;
            wCanTxFlag++;
        }
        
        // ��� wCanTxFlag ����1��׼������ CAN ����
        if (wCanTxFlag == 1)
        {
            strFrame.Id = cPYLON_CAN_ALARM_ID; // ���� CAN ID
            
            // ����Ϊ���ñ���ֵ���澯ֵ�Ĺ���
            data = 0;
            wStatus = swGetEmsDataFaultFlagLow(); // ��ȡ EMS ��λ���ϱ�־
            if ((wStatus & cBattDcgCurrHiFault) != 0)
			{
				data += 1 << 7;
			}
			if((wStatus & cCellTempLoFault) != 0)
			{
				data += 1 << 4;
			}
			if((wStatus & cCellTempHiFault) != 0)
			{
				data += 1 << 3;
			}
			if((wStatus & cCellVoltLoFault) != 0)
			{
				data += 1 << 2;
			}
			if((wStatus & cCellVoltHiFault) != 0)
			{
				data += 1 << 1;
			}
			strFrame.Data.bData[0] = data;
			
			//����ֵ2
			data = 0;
			wStatus = swGetEmsDataFaultFlagLow();
			if((wStatus & cBattChgCurrHiFault) != 0)
			{
				data += 1 << 0;
			}
			if(wStatus != 0)
			{
				data += 1 << 3;
			}
			strFrame.Data.bData[1] = data;
			
			//�澯ֵ1
			data = 0;
			wStatus = swGetEmsDataAlarmFlagLow();
			if((wStatus & cBattDcgCurrHiAlarm) != 0)
			{
				data += 1 << 7;
			}
			if((wStatus & cCellTempLoAlarm) != 0)
			{
				data += 1 << 4;
			}
			if((wStatus & cCellTempHiAlarm) != 0)
			{
				data += 1 << 3;
			}
			if((wStatus & cCellVoltLoAlarm) != 0)
			{
				data += 1 << 2;
			}
			if((wStatus & cCellVoltHiAlarm) != 0)
			{
				data += 1 << 1;
			}
			strFrame.Data.bData[2] = data;
			
			//�澯ֵ2
			data = 0;
			wStatus = swGetEmsDataAlarmFlagLow();
			if((wStatus & cBattChgCurrHiAlarm) != 0)
			{
				data += 1 << 0;
			}
			if(sbGetCanParallelNum() < wParallelNum)
			{
				data += 1 << 3;
			}
			wParallelNum = sbGetCanParallelNum();
			strFrame.Data.bData[3] = data;
			
			strFrame.Data.bData[4] = sbGetCanParallelNum();
			strFrame.Data.bData[5] = 'P';
			strFrame.Data.bData[6] = 'N';
			sCanWriteStandard(canid, &strFrame, 7);
		}
		else if(wCanTxFlag == 2)
		{
			strFrame.Id = cPYLON_CAN_LIMIT_ID;
			//������Ƶ�ѹ 0.1V
			data = swGetEmsDataChargeVoltageLimit();
			strFrame.Data.wData[0] = data;
			//������Ƶ��� 0.1A
			data = swGetEmsDataChargeCurrentLimit();
			strFrame.Data.wData[1] = data;
			//�ŵ����Ƶ��� 0.1A
			data = swGetEmsDataDischargeCurrentLimit();
			strFrame.Data.wData[2] = data;
			//�ŵ����Ƶ�ѹ 0.1V
			data = swGetEmsDataDischargeVoltageLimit();
			strFrame.Data.wData[3] = data;
			sCanWriteStandard(canid, &strFrame, 8);
		}
		else if(wCanTxFlag == 3)
		{
			strFrame.Id = cPYLON_CAN_SOC_SOH_ID;
			//SOC 1%
			data = swGetEmsDataTotalSOC() / 10;
			strFrame.Data.wData[0] = data;
			//SOH 1%
			data = swGetEmsDataTotalSOH() / 10;
			strFrame.Data.wData[1] = data;
			sCanWriteStandard(canid, &strFrame, 4);
		}
		else if(wCanTxFlag == 4)
		{
			strFrame.Id = cPYLON_CAN_ANALOG_ID;
			//ģ��ƽ����ѹ 0.01V
			data = swGetEmsDataTotalVoltage() * 10;
			strFrame.Data.wData[0] = data;
			//�ܵ��� 0.1A
			data = swGetEmsDataTotalCurrent();
			strFrame.Data.wData[1] = data;
			//��оƽ���¶� 0.1��
			data = (swGetEmsDataMaximumCellTemperature() + swGetEmsDataMinimumCellTemperature()) >> 1;
			strFrame.Data.wData[2] = data;
			sCanWriteStandard(canid, &strFrame, 6);
		}
		else if(wCanTxFlag == 5)
		{
			strFrame.Id = cPYLON_CAN_STATE_ID;
			//��ŵ�״̬
			data = 0;
			wBmsStatus.data = swGetEmsDataStatusLow();
			data = ((INT16U)wBmsStatus.bits.ChargeEnable << 7) \
				+ ((INT16U)wBmsStatus.bits.DischargeEnable << 6) \
				+ ((INT16U)wBmsStatus.bits.ChargeImmediately2 << 5) \
				+ ((INT16U)wBmsStatus.bits.ChargeImmediately1 << 4) \
				+((INT16U)wBmsStatus.bits.FullCharge << 3);
			strFrame.Data.bData[0] = data;
			strFrame.Data.bData[1] = 0;
			sCanWriteStandard(canid, &strFrame, 2);
		}
		else if(wCanTxFlag == 6)
		{
			strFrame.Id = cPYLON_CAN_BRAND_ID;
			//Ʒ�� "LYLON"
			strFrame.Data.bData[0] = 'P';
			strFrame.Data.bData[1] = 'Y';
			strFrame.Data.bData[2] = 'L';
			strFrame.Data.bData[3] = 'O';
			strFrame.Data.bData[4] = 'N';
			strFrame.Data.bData[5] = ' ';
			strFrame.Data.bData[6] = ' ';
			strFrame.Data.bData[7] = ' ';
			sCanWriteStandard(canid, &strFrame, 8);
		}
	}
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/


/********************************************************************************
* Input interface Routines														*
********************************************************************************/


