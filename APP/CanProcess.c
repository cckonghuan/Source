/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		CanProcess.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define		__CAN_PROCESS_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define		cCAN_POLLING_100MS		100
#define		cCAN_POLLING_200MS		200
#define		cCAN_POLLING_500MS		500
#define		cCAN_POLLING_1S			1000

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
INT8U	bCanAddress;

INT8U	bCanDataRecv;
/********************************************************************************
* Output variables																*
********************************************************************************/


/********************************************************************************
* Internal variables															*
********************************************************************************/
union CAN_STATUS_TABLE
{
	struct CAN_STATUS_BIT_FEILD
	{
		// BIT0-3
		INT16U rsvd1:4;
		
		// BIT4-7
		INT16U rsvd2:4;
		
		// BIT8-11
		INT16U rsvd3:4;
		
		// BIT12-15
		INT16U rsvd4:4;
	} bits;
	INT16U data;
} fCanStatus;

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/


/********************************************************************************
* Routines' implementations														*
********************************************************************************/
//can��ʼ��
void	sCanInitial(void)
{
	bCanDataRecv = 0;
	sCanProtocolNewInitial();
	sCanErrCntInit();
}

void	sCanParsing(void)
{
	T_CanFrame RxFrame;
	T_CAN_FRAME RxFrameNew;
	
	while(CAN_ReadFrame(cBMS_CAN, &RxFrame) == QUEUE_OK)
	{
		bCanDataRecv = 1;
		RxFrameNew.Id = RxFrame.IxR >> 3;
		RxFrameNew.Data.dwData[0] = RxFrame.Data.u32[0];
		RxFrameNew.Data.dwData[1] = RxFrame.Data.u32[1];	
		
		sCanAutoAdsParsing(&RxFrameNew);	// �Զ���ַ����
		
		// �տ����Լ���ַΪ0ʱ,������CANͨѶ
		if(sbGetCanAddress() != 0)
		{
			sCanProtocolNewParsing(&RxFrameNew);	// ��CANЭ��
		}
	}
}

// ��ַΪ0ʱ,��������ģʽ,����״̬�¹̶���ʱ3������״̬��־������Ӧģʽ
// ��ַ��Ϊ0ʱ,��������ģʽ,����ȴ�������ɫ���Ϊ�������ߴӻ���,�����������Ӧģʽ
// ����������к�,������ɫ���Ϊ�½����豸,����Ҫ�˳�������ģʽ
// �����߼�ֻ�������������ߴӻ���֧������CAN����Э���ִ��,�����һ̨��֧������Э��
// ��������ģʽ,���ָⷢ��Ҳ������ָ��,����������
// ��������ģʽ,֧�ֲ���ָ����ѯ(��ͨѶЭ��)�Լ���ͨ��Э����ѯ,
// ����ģʽ��֧�ֲ�������(����ģ��֧�ֲ���Э��),��ϵͳ״̬�л�֧�ֲ�����ɫ
// ����ģʽ�Ҳ�֧�ֲ�������(��һ��ģ�鲻֧�ֲ���Э��),��ϵͳ״̬�л�֧�ֲ�����ɫ
void	sCanTimerPolling(INT16U wTimeBase)
{
	// �տ����Լ���ַΪ0ʱ,������CANͨѶ
	if(bBmsMode == cPowerOnMode)
	{
		return;
	}

	sCanParaErrCntClr(wTimeBase);
	sCanAutoAdsCheck(wTimeBase);	// �Զ���ַ����
	sCanParallelTimerPolling(wTimeBase, cCAN_POLLING_200MS);
	
	if(sbGetCanAddress() != 0)
	{
		sCanParallelLogic(wTimeBase);
		
//		sCanParallelTimerPolling(wTimeBase, cCAN_POLLING_200MS);
		
		// ������ַ����
		if((swGetBmsFaultFlag() & cParallelFault) == 0)
		{
			sCanEmsDataTimerPolling(wTimeBase, cCAN_POLLING_200MS);
			sCanBmsDataTimerPolling(wTimeBase, cCAN_POLLING_200MS);
			sCanUserDataTimerPolling(wTimeBase, cCAN_POLLING_200MS);
		}
	}
}
// ����CAN���͵��ػ���ָ��
void sCanToShutdownParsing(void) {
    // �����ǰBMSģʽ���ǹػ�ģʽ
    if (sbGetBmsMode() != cShutdownMode) {
        // ����BMS�����ػ���־Ϊtrue
        sSetBmsParallelShutdown(true);
        // ���ͳ������񵽹ػ��¼�
        OSEventSend(cPrioSuper, eSuperToShutdown);
    }
}

//CAN���͵��ػ�����
void	sCanToShutdownCmd(void)
{
	if(sbGetCanAddress() != 0)
	{
		sCanNewShutdownEvent();
	}
}

//������ַ��������
void	sCanParallelAddrErr(void)
{
	// �����ǰBMS���ϴ���Ϊ0
	if(sbGetBmsFaultCode() == 0)
	{
		// ����BMS���ϴ���ΪΪ��������
		sSetBmsFaultCode(cParallelFaultId);
	}
	sSetBmsFaultFlag(cParallelFault);
	OSEventSend(cPrioSuper,eSuperToFault);
}

//CAN֡д����
void	sCanWrite(INT8U canId, T_CAN_FRAME *pFrame, INT8U bLen)
{
	T_CanFrame TxFrame;
	
	TxFrame.IxR = (pFrame->Id << 3) + (CAN_Id_Extended | CAN_RTR_Data);
	TxFrame.DTxR = bLen;	// 0-8֮��
	TxFrame.Data.u32[0] = pFrame->Data.dwData[0];
	TxFrame.Data.u32[1] = pFrame->Data.dwData[1];
	CAN_SendFrame(canId, &TxFrame);
}

//���ͱ�׼CAN֡
void	sCanWriteStandard(INT8U canId, T_CAN_FRAME *pFrame, INT8U bLen)
{
	// ����CAN֡�ṹ�����
	T_CanFrame TxFrame;
	
	// ��CAN֡ID��֡���뵽TxFrame.TxR��
	TxFrame.IxR = (pFrame->Id << 21) + (CAN_Id_Standard | CAN_RTR_Data);
	// �������ݳ���
	TxFrame.DTxR = bLen;	// 0-8֮��
	// ������д��TxFrame��
	TxFrame.Data.u32[0] = pFrame->Data.dwData[0];
	TxFrame.Data.u32[1] = pFrame->Data.dwData[1];
	// ����CAN_SendFrame��������CAN֡
	CAN_SendFrame(canId, &TxFrame);
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
INT8U	sbGetCanAddress(void)
{
	return bCanAddress;
}

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
void	sSetCanAddress(INT8U bAddress)
{
	bCanAddress = bAddress;
}

