/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		PylontechProtocolCan.c
Author:			BMSTeam
Date:			2023.07.01
Description:	上海派能能源科技有限公司,逆变器与电池通信协议(CAN)
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
// 定义一个函数 sPylonProtCanPolling，用于处理 Pylon 的 CAN 总线轮询
void sPylonProtCanPolling(INT8U canid, INT8U Filter1, INT8U Filter2)
{
    INT16U data; // 16位无符号整数 data
    T_CAN_FRAME strFrame; // CAN 帧结构体 strFrame
    static INT8U wCnt1 = 0; // 静态变量 wCnt1 用于计数
    static INT8U wCnt2 = 0; // 静态变量 wCnt2 用于计数
    static INT8U wCanTxFlag = 0; // 静态变量 wCanTxFlag 标志是否可以发送数据
    static INT8U wParallelNum = 0; // 静态变量 wParallelNum 用于标志并行数
    union BMS_STATUS_TABLE wBmsStatus; // BMS 状态表联合体 wBmsStatus
    INT16U wStatus; // 16位无符号整数 wStatus
    
    // 如果 CAN ID 不是 cPCS_CAN，直接返回
    if (canid != cPCS_CAN)
    {
        return;
    }
    
    // 如果当前设备不是 CAN 主机，直接返回
    if (sbIsCanDeviceHost() == 0)
    {
        return;
    }
    
    // 如果 BMS 模式为开机模式或关机模式，直接返回
    if ((bBmsMode == cPowerOnMode) || (bBmsMode == cShutdownMode))
    {
        return;
    }
    
    // 每经过 Filter1 次轮询，设置 wCanTxFlag 为可发送状态
    if (++wCnt1 > Filter1)
    {
        wCnt1 = 0;
        wCanTxFlag = 1;
    }
    
    // 如果 wCanTxFlag 为可发送状态，执行以下操作
    if (wCanTxFlag > 0)
    {
        // 每经过 Filter2 次轮询，wCanTxFlag 加1
        if (++wCnt2 > Filter2)
        {
            wCnt2 = 0;
            wCanTxFlag++;
        }
        
        // 如果 wCanTxFlag 等于1，准备发送 CAN 数据
        if (wCanTxFlag == 1)
        {
            strFrame.Id = cPYLON_CAN_ALARM_ID; // 设置 CAN ID
            
            // 以下为设置保护值、告警值的过程
            data = 0;
            wStatus = swGetEmsDataFaultFlagLow(); // 获取 EMS 低位故障标志
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
			
			//保护值2
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
			
			//告警值1
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
			
			//告警值2
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
			//充电限制电压 0.1V
			data = swGetEmsDataChargeVoltageLimit();
			strFrame.Data.wData[0] = data;
			//充电限制电流 0.1A
			data = swGetEmsDataChargeCurrentLimit();
			strFrame.Data.wData[1] = data;
			//放电限制电流 0.1A
			data = swGetEmsDataDischargeCurrentLimit();
			strFrame.Data.wData[2] = data;
			//放电限制电压 0.1V
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
			//模块平均电压 0.01V
			data = swGetEmsDataTotalVoltage() * 10;
			strFrame.Data.wData[0] = data;
			//总电流 0.1A
			data = swGetEmsDataTotalCurrent();
			strFrame.Data.wData[1] = data;
			//电芯平均温度 0.1℃
			data = (swGetEmsDataMaximumCellTemperature() + swGetEmsDataMinimumCellTemperature()) >> 1;
			strFrame.Data.wData[2] = data;
			sCanWriteStandard(canid, &strFrame, 6);
		}
		else if(wCanTxFlag == 5)
		{
			strFrame.Id = cPYLON_CAN_STATE_ID;
			//充放电状态
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
			//品牌 "LYLON"
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


