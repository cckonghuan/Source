/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		CanProtocolNew.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define		__CAN_PROTOCOL_NEW_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"Main.h"
#include	"CanProtocolNew.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define		cCAN_DELAY_TIME_1S			1000
#define		cCAN_DELAY_TIME_2S			2000
#define		cCAN_DELAY_TIME_3S			3000
#define		cCAN_DELAY_TIME_5S			5000
#define		cCAN_DELAY_TIME_10S			10000

#define		cCAN_HOST_ADDRESS			0xFF
#define		cCAN_DEVICE_BASE_ADDRESS	0x10

#define		cCAN_AUTO_ADS_OFFLINE		0
#define		cCAN_AUTO_ADS_OFF2ON		1
#define		cCAN_AUTO_ADS_ONLINE		2

#define		cCanErrProtCnt				10
#define		cCanErrProtTime_30min		1800000

enum CAN_FRAME_MODE
{
	eCAN_FRAME_COMMAND = 0x0,
	eCAN_FRAME_RESPONSE = 0x1
};

enum CAN_FUNC_CODE
{
	eCAN_FUNC_CTRL_EVENT = 0x1,
	eCAN_FUNC_USER_DATA = 0x2,
	eCAN_FUNC_RT_DATA = 0x3,
	eCAN_FUNC_DEV_INFO = 0xD
};

enum CAN_SRC_ADDR
{
	eCAN_SRC_ADDR_HOST = 0x00,
	eCAN_SRC_ADDR_AUTO = 0x01,
	eCAN_SRC_ADDR_MIN = 0x10,
	eCAN_SRC_ADDR_MAX = 0x1F,
};

enum CAN_DES_ADDR
{
	eCAN_DES_ADDR_HOST = 0x00,
	eCAN_DES_ADDR_BRD = 0xFF
};

enum CAN_MSG_CTRL_ID
{
	eCAN_MSG_SHUTDOWN_ID = 0x10,		// 0x1x:紧急事件
	eCAN_MSG_SYNC_TIME_ID = 0x30,		// 0x3x:时间同步
	eCAN_MSG_SYNC_KEY_ID = 0x31,
	eCAN_MSG_SYNC_SOC_ID = 0x32,
	eCAN_MSG_SYNC_USER_ID = 0x38,
	eCAN_MSG_PARALLEL_HOST_ID = 0x50,	// 0x5x:并机逻辑
	eCAN_MSG_PARALLEL_SLAVE_ID = 0x51,
	eCAN_MSG_PARALLEL_NEW_ID = 0x52,
	eCAN_MSG_PARALLEL_ERR_ID = 0x60		// 0x60:并机保护
};

enum CAN_MSG_EMS_ID
{
	eCAN_MSG_EMS_ID_START = 0x10,
	
	eCAN_MSG_EMS_ID01 = eCAN_MSG_EMS_ID_START,
	eCAN_MSG_EMS_ID02,
	eCAN_MSG_EMS_ID03,
	eCAN_MSG_EMS_ID04,
	eCAN_MSG_EMS_ID05,
	eCAN_MSG_EMS_ID06,
	eCAN_MSG_EMS_ID07,
	eCAN_MSG_EMS_ID08,
	
	eCAN_MSG_EMS_ID_END,
	eCAN_MSG_EMS_ID_LEN = eCAN_MSG_EMS_ID_END - eCAN_MSG_EMS_ID_START
};

enum CAN_MSG_BMS_ID
{
	eCAN_MSG_BMS_ID_START = 0x20,
	
	eCAN_MSG_BMS_ID01 = eCAN_MSG_BMS_ID_START,
	eCAN_MSG_BMS_ID02,
	eCAN_MSG_BMS_ID03,
	eCAN_MSG_BMS_ID04,
	eCAN_MSG_BMS_ID05,
	eCAN_MSG_BMS_ID06,
	eCAN_MSG_BMS_ID07,
	eCAN_MSG_BMS_ID08,
	eCAN_MSG_BMS_ID09,
	eCAN_MSG_BMS_ID10,
	eCAN_MSG_BMS_ID11,
	eCAN_MSG_BMS_ID12,
	eCAN_MSG_BMS_ID13,
	eCAN_MSG_BMS_ID14,
	eCAN_MSG_BMS_ID15,
	eCAN_MSG_BMS_ID16,
	
	eCAN_MSG_BMS_ID_END,
	eCAN_MSG_BMS_ID_LEN = eCAN_MSG_BMS_ID_END - eCAN_MSG_BMS_ID_START
};

enum CAN_MSG_USER_ID
{
	eCAN_MSG_USER_ID_START = 0x10,
	
	eCAN_MSG_USER_ID01 = eCAN_MSG_USER_ID_START,
	eCAN_MSG_USER_ID02,
	eCAN_MSG_USER_ID03,
	eCAN_MSG_USER_ID04,
	eCAN_MSG_USER_ID05,
	eCAN_MSG_USER_ID06,
	eCAN_MSG_USER_ID07,
	eCAN_MSG_USER_ID08,
	eCAN_MSG_USER_ID09,
	eCAN_MSG_USER_ID10,
	eCAN_MSG_USER_ID11,
	eCAN_MSG_USER_ID12,
	eCAN_MSG_USER_ID13,
	eCAN_MSG_USER_ID14,
	eCAN_MSG_USER_ID15,
	eCAN_MSG_USER_ID16,
	
	eCAN_MSG_USER_ID_END,
	eCAN_MSG_USER_ID_LEN = eCAN_MSG_USER_ID_END - eCAN_MSG_USER_ID_START
};

enum CAN_MSG_INFO_ID
{
	eCAN_MSG_INFO_ID_START = 0x10,
	
	eCAN_MSG_INFO_ID01 = eCAN_MSG_INFO_ID_START,
	eCAN_MSG_INFO_ID02,
	eCAN_MSG_INFO_ID03,
	eCAN_MSG_INFO_ID04,
	
	eCAN_MSG_INFO_ID_END,
	eCAN_MSG_INFO_ID_LEN = eCAN_MSG_INFO_ID_END - eCAN_MSG_INFO_ID_START
};

typedef union{
    INT32U dwData;
    struct{
		INT32U  MessageId:8;		// bit0~7	报文ID:
		INT32U  DestinAddress:8;	// bit8~15	目的地址:0x00为主机地址,0xFF为广播地址,0x01-0x0F地址预留,其他地址偏置+0x10
		INT32U  SourceAddress:8;	// bit16~23	源地址:0x00为主机地址,0xFF为广播地址,0x01-0x0F地址预留,其他地址偏置+0x10
		INT32U  FunctionCode:4;		// bit24~27	功能码:0x1控制事件,0x2用户参数,0x3实时数据,0xD设备信息
		INT32U  FrameMode:1;		// bit28	帧模型:0指令帧,1响应帧
		INT32U  KeyVer:1;			// bit29	0：软件版本，1：拔码版本
		INT32U  Rsvd:2;				// bit30~31
	}Bit;
}T_CAN_NEW_ID;

typedef union{
	INT8U 	bData[8];	// 定义字节型以满足需求
	INT16U	wData[4];	// 定义寄存器类型以提高读取效率,16位0x1234换成8位:0x34,0x12
	INT32U	dwData[2];	// 定义寄存器类型以提高读取效率,16位0x1234换成8位:0x34,0x12
}T_CAN_NEW_DATA;

typedef struct{
	T_CAN_NEW_ID Id;
	T_CAN_NEW_DATA Data;
}T_CAN_NEW_FRAME;

typedef struct
{
	INT8U bStatus;			// 0表示设备离线,1表示接收到设备信息,2表示设备在线(可用于自动地址分配)
	INT8U bHWAddress;		// 0表示自动分配地址,1-15表示硬件默认地址(没有硬件地址16,软件可以分配地址16)
	INT32U dwUniqueID;		// 自动分配地址时设备唯一ID,0x00000000-0xFFFFFFFF
	INT16U wOnlineCnt;		// 设备在线计数值
	INT16U wOfflineCnt;		// 设备离线计数值
}T_CanAutoAdsRxFrame;

typedef struct
{
	INT8U bVirtualID;		// 0表示未分配地址,1-16表示已经分配地址
	INT32U dwUniqueID;		// 自动分配地址时用到的设备唯一ID,值越低分配的地址越低
}T_CanVirtAdsFrame;

/********************************************************************************
* Constants																		*
********************************************************************************/


/********************************************************************************
* External variables															*
********************************************************************************/
extern union EMS_DATA_TABLE *pEmsData;
extern union BMS_DATA_TABLE *pBmsData[];
extern union BMS_DATA_TABLE *pBmsDataLocal;
extern struct STR_DYNAMIC_EEPROM StrEepromDynamic;

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
INT8U	bCanLocalAddress;
INT8U	bCanLocalId;
INT8U   bCanParallelRole;
INT16U	wParallelPollingCnt;
INT16U	wEmsDataPollingCnt;
INT16U	wBmsDataPollingCnt;
INT16U	wUserDataPollingCnt;

INT16U	wParallelNewCnt[BMS_PARALLEL_NUM_MAX];
INT16U	wParallelSlaveCnt[BMS_PARALLEL_NUM_MAX];
INT16U	wParallelHostCnt[BMS_PARALLEL_NUM_MAX];
INT32U	dwParallelNewFlag;
INT32U	dwParallelSlaveFlag;
INT32U	dwParallelHostFlag;
INT16U	wParallelErrorCnt;
INT16U	wParallelRoleCnt;
INT32U	dwParallelFlag;
INT8U	bParallelNum;
INT8U   bParallelHostId;
INT8U	bParallelErrChkCnt;
INT32U	dwParallelErrClrCnt;

INT32U	dwCanUserIdFlag;	// 用户参数接收到的ID状态,定义为32bits,所以最多支持32个ID号

T_CanAutoAdsRxFrame strCanAutoAdsRx[cCAN_DEVICE_DOUBLE];
T_CanVirtAdsFrame strCanVirtualAds[cCAN_DEVICE_NUM];

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
void	sCanControlEventParsing(T_CAN_NEW_FRAME *pRxFrame);
void	sCanParallelParsing(T_CAN_NEW_FRAME *pRxFrame);
void	sCanEmsDataParsing(T_CAN_NEW_FRAME *pRxFrame);
void	sCanBmsDataParsing(T_CAN_NEW_FRAME *pRxFrame);
void	sCanUserDataParsing(T_CAN_NEW_FRAME *pRxFrame);
void 	sCanParaErrProtChk(void);


/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void	sCanProtocolNewInitial(void)
{
	INT8U i;
	
	bCanLocalAddress = sbGetCanAddress();
	bCanLocalId = bCanLocalAddress + cCAN_DEVICE_BASE_ADDRESS;
	
	// 轮询时间错开,防止一次发送太多CAN数据
	wParallelPollingCnt = 1;
	wEmsDataPollingCnt = 2;
	wBmsDataPollingCnt = 3;
	wUserDataPollingCnt = 4;
	
	dwParallelNewFlag = 0;
	dwParallelSlaveFlag = 0;
	dwParallelHostFlag = 0;
	wParallelErrorCnt = 0;
	wParallelRoleCnt = 0;
	for(i = 0; i < BMS_PARALLEL_NUM_MAX; i++)
	{
		wParallelNewCnt[i] = 0;
		wParallelSlaveCnt[i] = 0;
		wParallelHostCnt[i] = 0;
	}
	dwParallelFlag = 0;
	bParallelNum = 0;
	bParallelHostId = 0xff;
	
	dwCanUserIdFlag = 0;
	
	memset(&strCanAutoAdsRx, 0, sizeof(T_CanAutoAdsRxFrame) * cCAN_DEVICE_DOUBLE);
	memset(&strCanVirtualAds, 0, sizeof(T_CanVirtAdsFrame) * cCAN_DEVICE_NUM);
}

void	sCanErrCntInit(void)
{
	bParallelErrChkCnt = 0;
	dwParallelErrClrCnt = 0;
}

void	sCanProtocolNewParsing(T_CAN_FRAME *pData)
{
	T_CAN_NEW_FRAME strRxFrame;
	
	strRxFrame.Id.dwData = pData->Id;
	strRxFrame.Data.dwData[0] = pData->Data.dwData[0];
	strRxFrame.Data.dwData[1] = pData->Data.dwData[1];
	
	// 目的地址:本机地址/广播地址/虚拟主机地址(仅针对虚拟主机起作用)
	if(bCanParallelRole == cCAN_PARALLEL_HOST)
	{
		if((strRxFrame.Id.Bit.DestinAddress != bCanLocalId) \
			&& (strRxFrame.Id.Bit.DestinAddress != eCAN_DES_ADDR_BRD) \
			&& (strRxFrame.Id.Bit.DestinAddress != eCAN_DES_ADDR_HOST))
		{
			return;
		}
	}
	else
	{
		if((strRxFrame.Id.Bit.DestinAddress != bCanLocalId) \
			&& (strRxFrame.Id.Bit.DestinAddress != eCAN_DES_ADDR_BRD))
		{
			return;
		}
	}
	
	// 源地址:虚拟主机地址/设备地址(最多16台)
	if((strRxFrame.Id.Bit.SourceAddress != eCAN_SRC_ADDR_HOST) \
		&& ((strRxFrame.Id.Bit.SourceAddress < eCAN_SRC_ADDR_MIN) \
		|| (strRxFrame.Id.Bit.SourceAddress > eCAN_SRC_ADDR_MAX)))
	{
		return;
	}
	
	// 
	if(strRxFrame.Id.Bit.FunctionCode == eCAN_FUNC_CTRL_EVENT)
	{
		sCanControlEventParsing(&strRxFrame);
	}
	else if(strRxFrame.Id.Bit.FunctionCode == eCAN_FUNC_USER_DATA)
	{
		sCanUserDataParsing(&strRxFrame);
	}
	else if(strRxFrame.Id.Bit.FunctionCode == eCAN_FUNC_RT_DATA)
	{
		sCanEmsDataParsing(&strRxFrame);
		sCanBmsDataParsing(&strRxFrame);
	}
	else if(strRxFrame.Id.Bit.FunctionCode == eCAN_FUNC_DEV_INFO)
	{
		
	}
}

void	sCanParallelTimerPolling(INT16U wTimeBase, INT16U wFilter)
{
	T_CAN_FRAME strFrame;
	T_CAN_NEW_ID strId;
	
	wParallelPollingCnt += wTimeBase;
	if(wParallelPollingCnt >= wFilter)
	{
		wParallelPollingCnt = 0;
		
		strId.dwData = 0;
		strId.Bit.FrameMode = eCAN_FRAME_COMMAND;
		strId.Bit.FunctionCode = eCAN_FUNC_CTRL_EVENT;
		if(sbGetKeyAddress() == 0)	// 硬件拨码地址为零时表示自动分配地址       
		{
			strId.Bit.SourceAddress = sbGetCanAddress() + cCAN_DEVICE_BASE_ADDRESS;//eCAN_SRC_ADDR_AUTO;
			strId.Bit.KeyVer = 0;
		}
		else
		{
			strId.Bit.SourceAddress = sbGetKeyAddress() + cCAN_DEVICE_BASE_ADDRESS;
			strId.Bit.KeyVer = 1;
		}
		strId.Bit.DestinAddress = eCAN_DES_ADDR_BRD;
		if(bCanParallelRole == cCAN_PARALLEL_HOST)
		{
			strId.Bit.MessageId = eCAN_MSG_PARALLEL_HOST_ID;
		}
		else if(bCanParallelRole == cCAN_PARALLEL_SLAVE)
		{
			strId.Bit.MessageId = eCAN_MSG_PARALLEL_SLAVE_ID;
		}
		else
		{
			strId.Bit.MessageId = eCAN_MSG_PARALLEL_NEW_ID;
		}
		strFrame.Id = strId.dwData;
		
		//strFrame.Data.dwData[0] = 0x0;
		//strFrame.Data.dwData[1] = 0x0;
		
		//sCanWrite(cBMS_CAN, &strFrame, 0);
//		if(sbGetKeyAddress() == 0)			// 硬件拨码地址为零时表示自动分配地址
//		{
			strFrame.Data.dwData[0] = sGetUIDCrc32NumCValue();	// UID值,for test
			sCanWrite(cBMS_CAN, &strFrame, 4);
//		}
//		else
//		{
//			sCanWrite(cBMS_CAN, &strFrame, 0);
//		}
	}
}

void	sCanEmsDataTimerPolling(INT16U wTimeBase, INT16U wFilter)
{
	static	INT8U bId = eCAN_MSG_EMS_ID_START;
	T_CAN_FRAME strFrame;
	T_CAN_NEW_ID strId;
	INT16U data;
	INT8U bFlag;
	
	if(bCanParallelRole != cCAN_PARALLEL_HOST)
	{
		return;
	}
	
	wEmsDataPollingCnt += wTimeBase;
	if(wEmsDataPollingCnt >= wFilter)
	{
		wEmsDataPollingCnt = 0;
		
		if(++bId >= eCAN_MSG_EMS_ID_END)
		{
			bId = eCAN_MSG_EMS_ID_START;
		}
		
		strId.dwData = 0;
		strId.Bit.FrameMode = eCAN_FRAME_COMMAND;
		strId.Bit.FunctionCode = eCAN_FUNC_RT_DATA;
		strId.Bit.SourceAddress = eCAN_SRC_ADDR_HOST;
		strId.Bit.DestinAddress = eCAN_DES_ADDR_BRD;
		strId.Bit.MessageId = bId;
		strFrame.Id = strId.dwData;
		bFlag = 1;
		
		switch(bId)
		{
			case eCAN_MSG_EMS_ID01:
				data = pEmsData->Field.ChargeVoltageLimit;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pEmsData->Field.DischargeVoltageLimit;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pEmsData->Field.ChargeCurrentLimit;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pEmsData->Field.DischargeCurrentLimit;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_EMS_ID02:
				data = pEmsData->Field.StatusHigh;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pEmsData->Field.StatusLow;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pEmsData->Field.FaultFlagHigh;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pEmsData->Field.FaultFlagLow;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_EMS_ID03:
				data = pEmsData->Field.AlarmFlagHigh;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pEmsData->Field.AlarmFlagLow;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pEmsData->Field.NoticeFlagHigh;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pEmsData->Field.NoticeFlagLow;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_EMS_ID04:
				data = pEmsData->Field.TotalCurrent;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pEmsData->Field.TotalVoltage;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pEmsData->Field.Reservd01;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pEmsData->Field.Reservd02;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_EMS_ID05:
				data = pEmsData->Field.TotalSOC;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pEmsData->Field.TotalSOH;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pEmsData->Field.TotalCapacityHigh;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pEmsData->Field.TotalCapacityLow;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_EMS_ID06:
				data = pEmsData->Field.ParallelNumber;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pEmsData->Field.ParallelStatus;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pEmsData->Field.SerialNumber;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pEmsData->Field.SerialStatus;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_EMS_ID07:
				data = pEmsData->Field.MaximumCellVoltageNo;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pEmsData->Field.MaximumCellVoltage;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pEmsData->Field.MinimumCellVoltageNo;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pEmsData->Field.MinimumCellVoltage;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_EMS_ID08:
				data = pEmsData->Field.MaximumCellTemperatureNo;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pEmsData->Field.MaximumCellTemperature;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pEmsData->Field.MinimumCellTemperatureNo;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pEmsData->Field.MinimumCellTemperature;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			default:
				bFlag = 0;
			break;
		}
		
		if(bFlag == 1)
		{
			sCanWrite(cBMS_CAN, &strFrame, 8);
		}
	}
}

void	sCanBmsDataTimerPolling(INT16U wTimeBase, INT16U wFilter)
{
	static	INT8U bId = eCAN_MSG_BMS_ID_START;
	T_CAN_FRAME strFrame;
	T_CAN_NEW_ID strId;
	INT16U data;
	INT8U bFlag;
	
	wBmsDataPollingCnt += wTimeBase;
	if(wBmsDataPollingCnt >= wFilter)
	{
		wBmsDataPollingCnt = 0;
		
		if(++bId >= eCAN_MSG_BMS_ID_END)
		{
			bId = eCAN_MSG_BMS_ID_START;
		}
		
		strId.dwData = 0;
		strId.Bit.FrameMode = eCAN_FRAME_COMMAND;
		strId.Bit.FunctionCode = eCAN_FUNC_RT_DATA;
		strId.Bit.SourceAddress = bCanLocalId;
		strId.Bit.DestinAddress = eCAN_DES_ADDR_BRD;
		strId.Bit.MessageId = bId;
		strFrame.Id = strId.dwData;
		bFlag = 1;
		
		switch(bId)
		{
			case eCAN_MSG_BMS_ID01:
				data = pBmsDataLocal->Field.ChargeVoltageLimit;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.DischargeVoltageLimit;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.ChargeCurrentLimit;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.DischargeCurrentLimit;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_BMS_ID02:
				data = pBmsDataLocal->Field.StatusHigh;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.StatusLow;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.FaultFlagHigh;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.FaultFlagLow;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_BMS_ID03:
				data = pBmsDataLocal->Field.AlarmFlagHigh;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.AlarmFlagLow;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.NoticeFlagHigh;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.NoticeFlagLow;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_BMS_ID04:
				data = pBmsDataLocal->Field.PackCurrent;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.PackVoltage;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.Reservd1;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.Reservd2;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_BMS_ID05:
				data = pBmsDataLocal->Field.SOC;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.SOH;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.PackCapacityHigh;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.PackCapacityLow;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_BMS_ID06:
				data = pBmsDataLocal->Field.ParallelNumber;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.ParallelStatus;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.SerialNumber;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.SerialStatus;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_BMS_ID07:
				data = pBmsDataLocal->Field.MaximumCellVoltageNo;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.MaximumCellVoltage;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.MinimumCellVoltageNo;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.MinimumCellVoltage;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_BMS_ID08:
				data = pBmsDataLocal->Field.MaximumCellTemperatureNo;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.MaximumCellTemperature;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.MinimumCellTemperatureNo;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.MinimumCellTemperature;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_BMS_ID09:
				data = pBmsDataLocal->Field.CellVoltage01;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.CellVoltage02;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.CellVoltage03;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.CellVoltage04;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_BMS_ID10:
				data = pBmsDataLocal->Field.CellVoltage05;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.CellVoltage06;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.CellVoltage07;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.CellVoltage08;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_BMS_ID11:
				data = pBmsDataLocal->Field.CellVoltage09;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.CellVoltage10;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.CellVoltage11;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.CellVoltage12;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_BMS_ID12:
				data = pBmsDataLocal->Field.CellVoltage13;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.CellVoltage14;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.CellVoltage15;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.CellVoltage16;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_BMS_ID13:
				data = pBmsDataLocal->Field.TemperatureSensor01;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.TemperatureSensor02;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.TemperatureSensor03;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.TemperatureSensor04;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_BMS_ID14:
				data = pBmsDataLocal->Field.TemperatureSensor05;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.TemperatureSensor06;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.TemperatureSensor07;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.TemperatureSensor08;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_BMS_ID15:
				data = pBmsDataLocal->Field.Reservd5;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.Reservd6;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.Reservd7;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.Reservd8;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			case eCAN_MSG_BMS_ID16:
				data = pBmsDataLocal->Field.Reservd9;
				strFrame.Data.wData[0] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.Reservd10;
				strFrame.Data.wData[1] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.Reservd11;
				strFrame.Data.wData[2] = swHighLowByteSwap(data);
				data = pBmsDataLocal->Field.Reservd12;
				strFrame.Data.wData[3] = swHighLowByteSwap(data);
			break;
			default:
				bFlag = 0;
			break;
		}
		
		if(bFlag == 1)
		{
			sCanWrite(cBMS_CAN, &strFrame, 8);
		}
	}
}

void	sCanUserDataTimerPolling(INT16U wTimeBase, INT16U wFilter)
{
	static	INT8U id = eCAN_MSG_USER_ID_START;
	T_CAN_FRAME strFrame;
	T_CAN_NEW_ID strId;
	
	if((bCanParallelRole != cCAN_PARALLEL_SLAVE) \
		|| (dwParallelHostFlag == 0))	// 本机为从机且总线存在主机
	{
		return;
	}
	
	wUserDataPollingCnt += wTimeBase;
	if(wUserDataPollingCnt >= wFilter)
	{
		wUserDataPollingCnt = 0;
		
		// 默认从机需要同步主机设置参数,可以在此增加使能条件用于选择是否同步参数
		if(sbHighBitNum32(dwCanUserIdFlag) < eCAN_MSG_USER_ID_LEN)
		{
			if(++id >= eCAN_MSG_USER_ID_END)
			{
				id = eCAN_MSG_USER_ID_START;
			}
			
			if((dwCanUserIdFlag & ((INT32U)1 << id)) == 0)
			{
				strId.dwData = 0;
				strId.Bit.FrameMode = eCAN_FRAME_COMMAND;
				strId.Bit.FunctionCode = eCAN_FUNC_USER_DATA;
				strId.Bit.SourceAddress = bCanLocalId;
				strId.Bit.DestinAddress = eCAN_SRC_ADDR_HOST;
				strId.Bit.MessageId = id;
				strFrame.Id = strId.dwData;
				
				sCanWrite(cBMS_CAN, &strFrame, 0);
			}
		}
	}
}

void	sCanSyncDisplayKeyCommand(void)
{
	T_CAN_FRAME strFrame;
	T_CAN_NEW_ID strId;
	
	if(bCanParallelRole != cCAN_PARALLEL_HOST)
	{
		return;
	}
	
	strId.dwData = 0;
	strId.Bit.FrameMode = eCAN_FRAME_COMMAND;
	strId.Bit.FunctionCode = eCAN_FUNC_CTRL_EVENT;
	strId.Bit.SourceAddress = eCAN_SRC_ADDR_HOST;
	strId.Bit.DestinAddress = eCAN_DES_ADDR_BRD;
	strId.Bit.MessageId = eCAN_MSG_SYNC_KEY_ID;
	strFrame.Id = strId.dwData;
	
	sCanWrite(cBMS_CAN, &strFrame, 0);
}

void	sCanSyncDisplaySocCommand(void)
{
	T_CAN_FRAME strFrame;
	T_CAN_NEW_ID strId;
	
	if(bCanParallelRole != cCAN_PARALLEL_HOST)
	{
		return;
	}
	
	strId.dwData = 0;
	strId.Bit.FrameMode = eCAN_FRAME_COMMAND;
	strId.Bit.FunctionCode = eCAN_FUNC_CTRL_EVENT;
	strId.Bit.SourceAddress = eCAN_SRC_ADDR_HOST;
	strId.Bit.DestinAddress = eCAN_DES_ADDR_BRD;
	strId.Bit.MessageId = eCAN_MSG_SYNC_SOC_ID;
	strFrame.Id = strId.dwData;
	
	sCanWrite(cBMS_CAN, &strFrame, 0);
}

void	sCanSyncUserParaCommand(void)
{
	T_CAN_FRAME strFrame;
	T_CAN_NEW_ID strId;
	
	if(bCanParallelRole != cCAN_PARALLEL_HOST)
	{
		return;
	}
	
	strId.dwData = 0;
	strId.Bit.FrameMode = eCAN_FRAME_COMMAND;
	strId.Bit.FunctionCode = eCAN_FUNC_CTRL_EVENT;
	strId.Bit.SourceAddress = eCAN_SRC_ADDR_HOST;
	strId.Bit.DestinAddress = eCAN_DES_ADDR_BRD;
	strId.Bit.MessageId = eCAN_MSG_SYNC_USER_ID;
	strFrame.Id = strId.dwData;
	
	sCanWrite(cBMS_CAN, &strFrame, 0);
}

void	sCanNewShutdownEvent(void)
{
	T_CAN_FRAME strFrame;
	T_CAN_NEW_ID strId;
	
	strId.dwData = 0;
	strId.Bit.FrameMode = eCAN_FRAME_COMMAND;
	strId.Bit.FunctionCode = eCAN_FUNC_CTRL_EVENT;
	strId.Bit.SourceAddress = bCanLocalId;
	strId.Bit.DestinAddress = eCAN_DES_ADDR_BRD;
	strId.Bit.MessageId = eCAN_MSG_SHUTDOWN_ID;
	strFrame.Id = strId.dwData;
	
	sCanWrite(cBMS_CAN, &strFrame, 0);
}

void	sCanParallelLogic(INT16U wTimeBase)
{
	INT8U i;
	
	// CAN设备角色标志检查
	for(i = 0; i < BMS_PARALLEL_NUM_MAX; i++)
	{
		wParallelHostCnt[i] += wTimeBase;
		wParallelSlaveCnt[i] += wTimeBase;
		wParallelNewCnt[i] += wTimeBase;
		if(wParallelHostCnt[i] >= cCAN_DELAY_TIME_3S)
		{
			wParallelHostCnt[i] = 0;
			CLRBIT(dwParallelHostFlag, i);
		}
		if(wParallelSlaveCnt[i] >= cCAN_DELAY_TIME_3S)
		{
			wParallelSlaveCnt[i] = 0;
			CLRBIT(dwParallelSlaveFlag, i);
		}
		if(wParallelNewCnt[i] >= cCAN_DELAY_TIME_3S)
		{
			wParallelNewCnt[i] = 0;
			CLRBIT(dwParallelNewFlag, i);
		}
	}
	
	// 并机逻辑处理
	// 1 无固定主机,则刚上电后前几秒竞争主机,角色晋升:从新接入设备->从设备->主设备
	// 2 有固定主机,则主机默认固定为主设备,且当存在多个主机时,直接报错不做额外自动处理
	// 2 有固定主机,则从机默认新接入设备,角色晋升:新接入设备->从设备(必须接收到主设备指令)
	switch(bCanParallelRole)
	{
		case cCAN_PARALLEL_NEW:
			// 本机ID与总线主机/从机/新接入ID存在地址冲突
			if(CHKBIT((dwParallelHostFlag|dwParallelSlaveFlag|dwParallelNewFlag),bCanLocalAddress))
			{
				wParallelRoleCnt = 0;
				wParallelErrorCnt += wTimeBase;
				if(wParallelErrorCnt >= cCAN_DELAY_TIME_10S)
				{
					wParallelErrorCnt = 0;
					sCanParallelErrSync();
					sCanParaErrProtChk();
					//sCanParallelAddrErr();
				}
			}
			else
			{
				wParallelErrorCnt = 0;
				wParallelRoleCnt += wTimeBase;
				if(wParallelRoleCnt >= cCAN_DELAY_TIME_3S)
				{
					wParallelRoleCnt = 0;
					bCanParallelRole = cCAN_PARALLEL_SLAVE;
				}
			}
		break;
		case cCAN_PARALLEL_SLAVE:
			// 从机设备角色变更：
			// 1、地址冲突后降级为新接入设备
			// 2、主机丢失后升级为主机设备
			
			// 本机ID与总线主机/从机ID存在地址冲突
			if(CHKBIT((dwParallelHostFlag|dwParallelSlaveFlag),bCanLocalAddress))
			{
				wParallelErrorCnt += wTimeBase;
				if(wParallelErrorCnt >= cCAN_DELAY_TIME_10S)
				{
					wParallelErrorCnt = 0;
					bCanParallelRole = cCAN_PARALLEL_NEW;
					sCanParallelErrSync();
					sCanParaErrProtChk();
					//sCanParallelAddrErr();
				}
			}
			else
			{
				wParallelErrorCnt = 0;
			}
			
			// 总线主机丢失
			if(dwParallelHostFlag == 0)
			{
				wParallelRoleCnt += wTimeBase;
				if(wParallelRoleCnt >= cCAN_DELAY_TIME_2S)
				{
					wParallelRoleCnt = 0;
					// 主机丢失,从机ID中最小的升级为主机
					if(CHKBITMIN(dwParallelSlaveFlag, bCanLocalAddress))
					{
						bCanParallelRole = cCAN_PARALLEL_HOST;
					}
				}
			}
			else
			{
				wParallelRoleCnt = 0;
			}
		break;
		case cCAN_PARALLEL_HOST:
			// 主机设备角色变更，通常不会主动退出,只有以下3种情况可退出
			// 1设备复位,设备角色初始化为新接入设备
			// 2多主机存在,本机ID与总线主机ID存在冲突,则报地址冲突故障
			// 3多主机存在,本机ID与总线主机ID不冲突,但本机ID较大,则本机降为新接入设备
			// 多主机存在,通常可能是由于正常运行中把主机通讯线拔掉后,其他从机竞争出主机后又把通讯线插上
			
			// 本机ID与总线主机ID存在地址冲突
			if(CHKBIT(dwParallelHostFlag,bCanLocalAddress))
			{
				wParallelErrorCnt += wTimeBase;
				if(wParallelErrorCnt >= cCAN_DELAY_TIME_10S)
				{
					wParallelErrorCnt = 0;
					bCanParallelRole = cCAN_PARALLEL_NEW;
					sCanParallelErrSync();
					sCanParaErrProtChk();
					//sCanParallelAddrErr();
				}
			}
			else
			{
				wParallelErrorCnt = 0;
			}
			
			// 本机ID是否总线主机ID中最小的,如果不是最小的,则本机降级为新接入设备
			if(CHKBITMINNOT(dwParallelHostFlag, bCanLocalAddress))
			{
				wParallelRoleCnt += wTimeBase;
				if(wParallelRoleCnt >= cCAN_DELAY_TIME_5S)
				{
					wParallelRoleCnt = 0;
					bCanParallelRole = cCAN_PARALLEL_NEW;
				}
			}
			else
			{
				wParallelRoleCnt = 0;
			}
		break;
		default:
			bCanParallelRole = cCAN_PARALLEL_NEW;
		break;
	}
	
	// 并机状态/数量/主机ID整理
	dwParallelFlag = dwParallelSlaveFlag|dwParallelHostFlag;
	SETBIT(dwParallelFlag, bCanLocalAddress);
	if(bCanParallelRole == cCAN_PARALLEL_HOST)
	{
		bParallelHostId = bCanLocalAddress;
	}
	else
	{
		bParallelHostId = sbFirstBitHighNo32(dwParallelHostFlag);
	}
	bParallelNum = sbHighBitNum32(dwParallelFlag);
}

void	sCanParallelParsing(T_CAN_NEW_FRAME *pRxFrame)
{
	INT8U n;
	
	n = pRxFrame->Id.Bit.SourceAddress - cCAN_DEVICE_BASE_ADDRESS;
	if(n >= BMS_PARALLEL_NUM_MAX)
	{
		return;
	}
	
	if(pRxFrame->Id.Bit.MessageId == eCAN_MSG_PARALLEL_HOST_ID)
	{
		SETBIT(dwParallelHostFlag, n);
		wParallelHostCnt[n] = 0;
	}
	else if(pRxFrame->Id.Bit.MessageId == eCAN_MSG_PARALLEL_SLAVE_ID)
	{
		SETBIT(dwParallelSlaveFlag, n);
		wParallelSlaveCnt[n] = 0;
	}
	else if(pRxFrame->Id.Bit.MessageId == eCAN_MSG_PARALLEL_NEW_ID)
	{
		SETBIT(dwParallelNewFlag, n);
		wParallelNewCnt[n] = 0;
	}
}

void	sCanControlEventParsing(T_CAN_NEW_FRAME *pRxFrame)
{
	switch(pRxFrame->Id.Bit.MessageId)
	{
		case eCAN_MSG_SHUTDOWN_ID:
			sCanToShutdownParsing();
		break;
		case eCAN_MSG_SYNC_TIME_ID:
			
		break;
		case eCAN_MSG_SYNC_KEY_ID:
			if(bCanParallelRole != cCAN_PARALLEL_HOST)
			{
				OSEventSend(cPrioInterface, eDisplayKeySync);
			}
		break;
		case eCAN_MSG_SYNC_SOC_ID:
			if(bCanParallelRole != cCAN_PARALLEL_HOST)
			{
				OSEventSend(cPrioInterface, eDisplaySocSync);
			}
		break;
		case eCAN_MSG_SYNC_USER_ID:	// 主机用户参数有更新,重新同步参数
			dwCanUserIdFlag = 0;
		break;
		case eCAN_MSG_PARALLEL_HOST_ID:
			sCanParallelParsing(pRxFrame);
		break;
		case eCAN_MSG_PARALLEL_SLAVE_ID:
			sCanParallelParsing(pRxFrame);
		break;
		case eCAN_MSG_PARALLEL_NEW_ID:
			sCanParallelParsing(pRxFrame);
		break;
		case eCAN_MSG_PARALLEL_ERR_ID:
			sCanParaErrProtChk();
		break;
		default:break;
	}
}

void	sCanEmsDataParsing(T_CAN_NEW_FRAME *pRxFrame)
{
	INT16U data;
	
	if(pRxFrame->Id.Bit.FrameMode == eCAN_FRAME_RESPONSE)
	{
		return;
	}
	
	if((pRxFrame->Id.Bit.MessageId < eCAN_MSG_EMS_ID_START) \
		|| (pRxFrame->Id.Bit.MessageId >= eCAN_MSG_EMS_ID_END))
	{
		return;
	}
	
	if(bCanParallelRole == cCAN_PARALLEL_HOST)
	{
		return;
	}
	
	switch(pRxFrame->Id.Bit.MessageId)
	{
		case eCAN_MSG_EMS_ID01:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pEmsData->Field.ChargeVoltageLimit = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pEmsData->Field.DischargeVoltageLimit = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pEmsData->Field.ChargeCurrentLimit = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pEmsData->Field.DischargeCurrentLimit = data;
		break;
		case eCAN_MSG_EMS_ID02:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pEmsData->Field.StatusHigh = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pEmsData->Field.StatusLow = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pEmsData->Field.FaultFlagHigh = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pEmsData->Field.FaultFlagLow = data;
		break;
		case eCAN_MSG_EMS_ID03:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pEmsData->Field.AlarmFlagHigh = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pEmsData->Field.AlarmFlagLow = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pEmsData->Field.NoticeFlagHigh = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pEmsData->Field.NoticeFlagLow = data;
		break;
		case eCAN_MSG_EMS_ID04:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pEmsData->Field.TotalCurrent = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pEmsData->Field.TotalVoltage = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pEmsData->Field.Reservd01 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pEmsData->Field.Reservd02 = data;
		break;
		case eCAN_MSG_EMS_ID05:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pEmsData->Field.TotalSOC = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pEmsData->Field.TotalSOH = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pEmsData->Field.TotalCapacityHigh = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pEmsData->Field.TotalCapacityLow = data;
		break;
		case eCAN_MSG_EMS_ID06:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pEmsData->Field.ParallelNumber = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pEmsData->Field.ParallelStatus = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pEmsData->Field.SerialNumber = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pEmsData->Field.SerialStatus = data;
		break;
		case eCAN_MSG_EMS_ID07:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pEmsData->Field.MaximumCellVoltageNo = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pEmsData->Field.MaximumCellVoltage = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pEmsData->Field.MinimumCellVoltageNo = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pEmsData->Field.MinimumCellVoltage = data;
		break;
		case eCAN_MSG_EMS_ID08:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pEmsData->Field.MaximumCellTemperatureNo = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pEmsData->Field.MaximumCellTemperature = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pEmsData->Field.MinimumCellTemperatureNo = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pEmsData->Field.MinimumCellTemperature = data;
		break;
		default:break;
	}
}

void	sCanBmsDataParsing(T_CAN_NEW_FRAME *pRxFrame)
{
	union BMS_DATA_TABLE *pData;
	INT16U data;
	
	if(pRxFrame->Id.Bit.FrameMode == eCAN_FRAME_RESPONSE)
	{
		return;
	}
	
	if((pRxFrame->Id.Bit.MessageId < eCAN_MSG_BMS_ID_START) \
		|| (pRxFrame->Id.Bit.MessageId >= eCAN_MSG_BMS_ID_END))
	{
		return;
	}
	
	data = pRxFrame->Id.Bit.SourceAddress - cCAN_DEVICE_BASE_ADDRESS;
	if((data >= BMS_PARALLEL_NUM_MAX) || (data == bCanLocalAddress))
	{
		return;
	}
	pData = pBmsData[data];
	
	switch(pRxFrame->Id.Bit.MessageId)
	{
		case eCAN_MSG_BMS_ID01:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pData->Field.ChargeVoltageLimit = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pData->Field.DischargeVoltageLimit = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pData->Field.ChargeCurrentLimit = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pData->Field.DischargeCurrentLimit = data;
		break;
		case eCAN_MSG_BMS_ID02:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pData->Field.StatusHigh = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pData->Field.StatusLow = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pData->Field.FaultFlagHigh = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pData->Field.FaultFlagLow = data;
		break;
		case eCAN_MSG_BMS_ID03:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pData->Field.AlarmFlagHigh = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pData->Field.AlarmFlagLow = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pData->Field.NoticeFlagHigh = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pData->Field.NoticeFlagLow = data;
		break;
		case eCAN_MSG_BMS_ID04:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pData->Field.PackCurrent = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pData->Field.PackVoltage = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pData->Field.Reservd1 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pData->Field.Reservd2 = data;
		break;
		case eCAN_MSG_BMS_ID05:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pData->Field.SOC = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pData->Field.SOH = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pData->Field.PackCapacityHigh = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pData->Field.PackCapacityLow = data;
		break;
		case eCAN_MSG_BMS_ID06:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pData->Field.ParallelNumber = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pData->Field.ParallelStatus = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pData->Field.SerialNumber = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pData->Field.SerialStatus = data;
		break;
		case eCAN_MSG_BMS_ID07:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pData->Field.MaximumCellVoltageNo = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pData->Field.MaximumCellVoltage = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pData->Field.MinimumCellVoltageNo = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pData->Field.MinimumCellVoltage = data;
		break;
		case eCAN_MSG_BMS_ID08:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pData->Field.MaximumCellTemperatureNo = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pData->Field.MaximumCellTemperature = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pData->Field.MinimumCellTemperatureNo = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pData->Field.MinimumCellTemperature = data;
		break;
		case eCAN_MSG_BMS_ID09:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pData->Field.CellVoltage01 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pData->Field.CellVoltage02 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pData->Field.CellVoltage03 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pData->Field.CellVoltage04 = data;
		break;
		case eCAN_MSG_BMS_ID10:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pData->Field.CellVoltage05 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pData->Field.CellVoltage06 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pData->Field.CellVoltage07 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pData->Field.CellVoltage08 = data;
		break;
		case eCAN_MSG_BMS_ID11:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pData->Field.CellVoltage09 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pData->Field.CellVoltage10 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pData->Field.CellVoltage11 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pData->Field.CellVoltage12 = data;
		break;
		case eCAN_MSG_BMS_ID12:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pData->Field.CellVoltage13 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pData->Field.CellVoltage14 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pData->Field.CellVoltage15 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pData->Field.CellVoltage16 = data;
		break;
		case eCAN_MSG_BMS_ID13:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pData->Field.TemperatureSensor01 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pData->Field.TemperatureSensor02 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pData->Field.TemperatureSensor03 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pData->Field.TemperatureSensor04 = data;
		break;
		case eCAN_MSG_BMS_ID14:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pData->Field.TemperatureSensor05 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pData->Field.TemperatureSensor06 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pData->Field.TemperatureSensor07 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pData->Field.TemperatureSensor08 = data;
		break;
		case eCAN_MSG_BMS_ID15:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pData->Field.Reservd5 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pData->Field.Reservd6 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pData->Field.Reservd7 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pData->Field.Reservd8 = data;
		break;
		case eCAN_MSG_BMS_ID16:
			data = swHighLowByteSwap(pRxFrame->Data.wData[0]);
			pData->Field.Reservd9 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[1]);
			pData->Field.Reservd10 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[2]);
			pData->Field.Reservd11 = data;
			data = swHighLowByteSwap(pRxFrame->Data.wData[3]);
			pData->Field.Reservd12 = data;
		break;
		default:break;
	}
}

void	sCanUserDataParsing(T_CAN_NEW_FRAME *pRxFrame)
{
	T_CAN_FRAME strTxFrame;
	T_CAN_NEW_ID strId;
	INT16U id;
	INT16U *p;
	INT8U i;
	
	if((pRxFrame->Id.Bit.MessageId < eCAN_MSG_USER_ID_START) \
		|| (pRxFrame->Id.Bit.MessageId >= eCAN_MSG_USER_ID_END))
	{
		return;
	}
	
	if(pRxFrame->Id.Bit.SourceAddress >= (cCAN_DEVICE_BASE_ADDRESS  + BMS_PARALLEL_NUM_MAX))
	{
		return;
	}
	
	id = (pRxFrame->Id.Bit.MessageId - eCAN_MSG_USER_ID_START);
	if(id >= (cEepromDynamicTotalLength >> 2))	// 一次传4words,所以右移2位
	{
		return;
	}
	
	if((pRxFrame->Id.Bit.FrameMode == eCAN_FRAME_COMMAND) \
		&& (bCanParallelRole == cCAN_PARALLEL_HOST))
	{
		strId.dwData = 0;
		strId.Bit.FrameMode = eCAN_FRAME_RESPONSE;
		strId.Bit.FunctionCode = eCAN_FUNC_USER_DATA;
		strId.Bit.SourceAddress = eCAN_SRC_ADDR_HOST;
		strId.Bit.DestinAddress = pRxFrame->Id.Bit.SourceAddress;
		strId.Bit.MessageId = pRxFrame->Id.Bit.MessageId;
		strTxFrame.Id = strId.dwData;
		
		p = ((INT16U *)&StrEepromDynamic) + (id << 2);
		for(i = 0; i < 4; i++)
		{
			strTxFrame.Data.wData[i] = swHighLowByteSwap(*p++);
		}
		sCanWrite(cBMS_CAN, &strTxFrame, 8);
	}
	
	if((pRxFrame->Id.Bit.FrameMode == eCAN_FRAME_RESPONSE) \
		&& (bCanParallelRole == cCAN_PARALLEL_SLAVE))
	{
		p = ((INT16U *)&StrEepromDynamic) + (id << 2);
		for(i = 0; i < 4; i++)
		{
			if((p >= (INT16U *)&StrEepromDynamic.wCellVoltPct80) \
				&& (p <= (INT16U *)&StrEepromDynamic.wFaultRecordEn))
			{
				*p = swHighLowByteSwap(pRxFrame->Data.wData[i]);
			}
			p++;
		}
		
		SETBIT(dwCanUserIdFlag, id);
		sSetEepromDynamicMode(cEEPROM_SAVE_MODE_ENABLE);
	}
}

void	sCanAutoAdsParsing(T_CAN_FRAME *pData)
{
	OS_CPU_SR cpu_sr;
	T_CAN_NEW_FRAME strRxFrame;
	INT8U i, bNewDevice;
	INT8U bAddr;
	INT32U dwUID;
	
	strRxFrame.Id.dwData = pData->Id;
	strRxFrame.Data.dwData[0] = pData->Data.dwData[0];
	strRxFrame.Data.dwData[1] = pData->Data.dwData[1];
	
	// 功能码判断,仅支持控制指令
	if(strRxFrame.Id.Bit.FunctionCode != eCAN_FUNC_CTRL_EVENT)
	{
		return;
	}
	
	// 目的地址判断,仅支持广播地址
	if(strRxFrame.Id.Bit.DestinAddress != eCAN_DES_ADDR_BRD)
	{
		return;
	}
	
	// 报文ID判断,仅支持并机报文
	if((strRxFrame.Id.Bit.MessageId != eCAN_MSG_PARALLEL_HOST_ID) \
		&& (strRxFrame.Id.Bit.MessageId != eCAN_MSG_PARALLEL_SLAVE_ID) \
		&& (strRxFrame.Id.Bit.MessageId != eCAN_MSG_PARALLEL_NEW_ID))
	{
		return;
	}
	
	// 源地址判断
	if(strRxFrame.Id.Bit.KeyVer == 0)	//软件版本
	{
		bAddr = 0;	// 无硬件分配地址
		dwUID = strRxFrame.Data.dwData[0];	
	}
	else if((strRxFrame.Id.Bit.SourceAddress > eCAN_SRC_ADDR_MIN) && (strRxFrame.Id.Bit.SourceAddress <= eCAN_SRC_ADDR_MAX))
	{
		bAddr = strRxFrame.Id.Bit.SourceAddress - eCAN_SRC_ADDR_MIN;// 1-15
		dwUID = 0;
	}
//	else if(strRxFrame.Id.Bit.SourceAddress == eCAN_SRC_ADDR_AUTO)	// 未分配地址时,使用0x01作为源地址发送数据
//	{
//		bAddr = 0;	// 无硬件分配地址
//		dwUID = strRxFrame.Data.dwData[0];
//	}
	else
	{
		return;
	}


	strCanAutoAdsRx[0].bHWAddress = sbGetKeyAddress();
	strCanAutoAdsRx[0].dwUniqueID = sGetUIDCrc32NumCValue();
	strCanAutoAdsRx[0].wOfflineCnt = 0;
	if(strCanAutoAdsRx[0].bStatus == cCAN_AUTO_ADS_OFFLINE)
	{
		strCanAutoAdsRx[0].bStatus = cCAN_AUTO_ADS_OFF2ON;
		strCanAutoAdsRx[0].wOnlineCnt = 0;
	}
	
	// 搜索是否新设备
	bNewDevice = 1;
	for(i = 1; i < cCAN_DEVICE_DOUBLE; i++)	// 本机设备信息自动写入数组0
	{
		if((strCanAutoAdsRx[i].bHWAddress == bAddr) && (strCanAutoAdsRx[i].dwUniqueID == dwUID))
		{
			bNewDevice = 0;
			OS_ENTER_CRITICAL();
			if(strCanAutoAdsRx[i].bStatus == cCAN_AUTO_ADS_OFFLINE)
			{
				strCanAutoAdsRx[i].bStatus = cCAN_AUTO_ADS_OFF2ON;
			}
			strCanAutoAdsRx[i].wOfflineCnt = 0;
			OS_EXIT_CRITICAL();
			break;	// 退出循环
		}
	}
	
	if(bNewDevice == 1)	// 新设备加入
	{
		for(i = 1; i < cCAN_DEVICE_DOUBLE; i++)	// 本机设备信息自动写入数组0
		{
			if(strCanAutoAdsRx[i].bStatus == cCAN_AUTO_ADS_OFFLINE)	// 写入新设备数据
			{
				OS_ENTER_CRITICAL();
				strCanAutoAdsRx[i].bHWAddress = bAddr;
				strCanAutoAdsRx[i].dwUniqueID = dwUID;
				strCanAutoAdsRx[i].wOfflineCnt = 0;
				strCanAutoAdsRx[i].wOnlineCnt = 0;
				strCanAutoAdsRx[i].bStatus = cCAN_AUTO_ADS_OFF2ON;
				OS_EXIT_CRITICAL();
				break;	// 退出循环
			}
		}
	}
}

void	sCanAutoAdsCheck(INT16U wTimeBase)
{
	OS_CPU_SR cpu_sr;
	INT8U i;
	
	for(i = 1; i < cCAN_DEVICE_DOUBLE; i++)
	{
		// 5秒没有接收到指令,清零
		if(strCanAutoAdsRx[i].bStatus != cCAN_AUTO_ADS_OFFLINE)
		{
			strCanAutoAdsRx[i].wOfflineCnt += wTimeBase;
			if(strCanAutoAdsRx[i].wOfflineCnt >= cCAN_DELAY_TIME_5S)
			{
				OS_ENTER_CRITICAL();
				strCanAutoAdsRx[i].bStatus = cCAN_AUTO_ADS_OFFLINE;
				strCanAutoAdsRx[i].bHWAddress = sbGetKeyAddress();
				strCanAutoAdsRx[i].dwUniqueID = 0;
				strCanAutoAdsRx[i].wOfflineCnt = 0;
				strCanAutoAdsRx[i].wOnlineCnt = 0;
				OS_EXIT_CRITICAL();
				OSEventSend(cPrioInterface, eCanAutoAssign);
			}
		}
	}
	
	for(i = 0; i < cCAN_DEVICE_DOUBLE; i++)
	{
		// 接收到指令,延时10秒变更状态
		if(strCanAutoAdsRx[i].bStatus == cCAN_AUTO_ADS_OFF2ON)
		{
			strCanAutoAdsRx[i].wOnlineCnt += wTimeBase;
			if(strCanAutoAdsRx[i].wOnlineCnt >= cCAN_DELAY_TIME_10S)
			{
				OS_ENTER_CRITICAL();
				strCanAutoAdsRx[i].wOnlineCnt = 0;
				strCanAutoAdsRx[i].bStatus = cCAN_AUTO_ADS_ONLINE;
				OS_EXIT_CRITICAL();
				OSEventSend(cPrioInterface, eCanAutoAssign);
			}
		}
	}
	
	
}

void	sCanAutoAdsAssign(void)
{
	OS_CPU_SR cpu_sr;
	INT8U i, j, bMinId, bSwAddrCnt;
	INT32U dwValueMin;
	INT16U wHwAddrStatus;
	
	// 清除数据
	for(i = 0; i < cCAN_DEVICE_NUM; i++)
	{
		strCanVirtualAds[i].bVirtualID = 0;
		strCanVirtualAds[i].dwUniqueID = 0xFFFFFFFF;

	}
	
	// 获取可以分配地址的原始数据
	bSwAddrCnt = 0;
	wHwAddrStatus = 0;
	for(i = 0, j = 0; i < cCAN_DEVICE_DOUBLE; i++)
	{
		if(strCanAutoAdsRx[i].bStatus == cCAN_AUTO_ADS_ONLINE)
		{
			strCanVirtualAds[j].dwUniqueID = strCanAutoAdsRx[i].dwUniqueID;
			strCanVirtualAds[j].bVirtualID = strCanAutoAdsRx[i].bHWAddress;
			if(strCanVirtualAds[j].bVirtualID > 0)	// 1-15表示硬件分配地址,0表示软件分配地址
			{
				wHwAddrStatus |= (1 << (strCanVirtualAds[j].bVirtualID - 1));	// 硬件地址位置
			}
			else
			{
				bSwAddrCnt++;	// 软件地址个数
			}
			if(++j >= cCAN_DEVICE_NUM)		// 最多16个可分配地址
			{
				break;
			}
		}
	}
	
	// 地址分配处理,硬件地址优先,其次按UID最小值优先,地址优先从1开始
	for(; bSwAddrCnt > 0; bSwAddrCnt--)
	{
		// 查找未分配地址中的最小CRC值
		bMinId = 16;
		dwValueMin = 0xFFFFFFFF;
		for(i = 0; i < cCAN_DEVICE_NUM; i++)
		{
			if(strCanVirtualAds[i].bVirtualID == 0)		// 无硬件地址分配,根据CAN总线自动分配
			{
				if(dwValueMin > strCanVirtualAds[i].dwUniqueID)
				{
					dwValueMin = strCanVirtualAds[i].dwUniqueID;
					bMinId = i;
				}
			}
		}
		
		// 根据查找到的最小UID值分配地址,地址分配完成后,不再查找最小CRC
		for(j = 0; j < cCAN_DEVICE_NUM; j++)
		{
			if(CHKNBIT(wHwAddrStatus, j))		// 该位置为空,表示未分配地址,可用
			{
				strCanVirtualAds[bMinId].bVirtualID = j + 1;	// 地址1-16,非0-15
				wHwAddrStatus |= 1 << j;  
				break;
			}
		}
	}
	
	// 本机地址以及所有分配地址状态
	for(i = 0, j = 0; i < cCAN_DEVICE_NUM; i++)
	{
		if(strCanVirtualAds[i].dwUniqueID == strCanAutoAdsRx[0].dwUniqueID)	// 数组0是本机UID,或者用本机UID参数代替
		{
			j = strCanVirtualAds[i].bVirtualID;
			//j = i + 1;	// 地址1-16,非0-15
			break;
		}
		
	}
	if(sbGetKeyAddress() != 0)	// 硬件分配地址
	{
		j = sbGetKeyAddress();
	}
	
	if(sbGetCanAddress() != j)	//sbGetKeyAddress()
	{
		OS_ENTER_CRITICAL();
		sSetCanAddress(j);
		if(j == 0)
		{
			j = 16;
		}

		sSetSciAddress(j);		
		bCanLocalAddress = sbGetCanAddress();
		bCanLocalId = bCanLocalAddress + cCAN_DEVICE_BASE_ADDRESS;
		sSetBmsLocalPointer(sbGetCanAddress());
		OS_EXIT_CRITICAL();
	}
}

void	sCanParallelErrSync(void)
{
	T_CAN_FRAME strFrame;
	T_CAN_NEW_ID strId;

	if(sbGetKeyAddress() != 0)
	{
		return;
	}
	strId.dwData = 0;
	strId.Bit.FrameMode = eCAN_FRAME_COMMAND;
	strId.Bit.FunctionCode = eCAN_FUNC_CTRL_EVENT;
	strId.Bit.SourceAddress = eCAN_SRC_ADDR_HOST;
	strId.Bit.KeyVer = 0;
	strId.Bit.DestinAddress = eCAN_DES_ADDR_BRD;
	strId.Bit.MessageId = eCAN_MSG_PARALLEL_ERR_ID;
	strFrame.Id = strId.dwData;
	
	strFrame.Data.dwData[0] = sGetUIDCrc32NumCValue();	// UID值,for test
	sCanWrite(cBMS_CAN, &strFrame, 4);
}

void sCanParaErrProtChk(void)
{
	if(++bParallelErrChkCnt >= cCanErrProtCnt || sbGetKeyAddress() != 0)
	{
		bParallelErrChkCnt = 0;
		dwParallelErrClrCnt = 0;
		sCanParallelAddrErr();
	}
	else
	{
		sKeyProcessInit();
//		sSetCanAddress(0);
//		sSetSciAddress(0);	//1
		sCanProtocolNewInitial();
	}
}

void sCanParaErrCntClr(INT16U wTimeBase)
{
	dwParallelErrClrCnt += wTimeBase;
	if(dwParallelErrClrCnt > cCanErrProtTime_30min)
	{
		dwParallelErrClrCnt = 0;
		bParallelErrChkCnt = 0;
	}
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
INT32U	sdwGetCanParallelFlag(void)
{
	return dwParallelFlag;
}

INT8U	sbGetCanParallelNum(void)
{
	return bParallelNum;
}

INT8U	sbGetCanParallelHostId(void)
{
	return bParallelHostId;
}

INT8U	sbGetCanDeviceRole(void)
{
	return bCanParallelRole;
}

INT8U	sbIsCanDeviceHost(void)
{
	return EQU(bCanParallelRole, cCAN_PARALLEL_HOST);
}

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
void sInitial_CAN_New_Addr(void)
{
    bCanLocalAddress = sbGetCanAddress();
    bCanLocalId = bCanLocalAddress + cCAN_DEVICE_BASE_ADDRESS;
    sSetBmsLocalPointer(sbGetCanAddress());
}

