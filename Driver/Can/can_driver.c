/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		can_driver.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define		__CAN_DRIVER_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"can_driver.h"
#include	"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define	cCanTxRdy				0
#define	cCanTxBusy				1

/********************************************************************************
* Constants																		*
********************************************************************************/
static const INT8U TxMailBox_IdleNO_Tbl[] = {
				0xFF/*0x0:Busy*/,	0/*0x1:0*/, 	1/*010b:1*/, 
				0/*011b:0,1*/,		2/*100b:2*/,	0/*101b:0,2*/, 
				1/*110b:1,2*/,		0/*111b:0,1,2*/};

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
T_CanQueue CanTxQueue[MAX_CAN_NO];
T_CanQueue CanRxQueue[MAX_CAN_NO];
T_CanFrame CanTxBuf[MAX_CAN_NO][CAN_QUE_TX_Nframe] = {0,};	// CAN���ͻ�����
T_CanFrame CanRxBuf[MAX_CAN_NO][CAN_QUE_RX_Nframe] = {0,};	// CAN���ջ�����
//static T_CanFrame CanTxBuf[MAX_CAN_NO][CAN_QUE_TX_Nframe] = {0,};	// CAN���ͻ�����
//static T_CanFrame CanRxBuf[MAX_CAN_NO][CAN_QUE_RX_Nframe] = {0,};	// CAN���ջ�����
INT32U CAN_RecvQueOvf[MAX_CAN_NO];
INT32U CAN_SendQueOvf[MAX_CAN_NO];
INT32U CAN_RecvQueMaxNum[MAX_CAN_NO];
INT32U CAN_SendQueMaxNum[MAX_CAN_NO];

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
INT8U	CAN_QueueCreate(T_CanQueue *Queue, T_CanFrame *pDataBuf,
                          	INT16U nData
							#if (CAN_QUEUE_READ_EMPTY_EN > 0)
                          	,INT8U (* ReadEmpty)()
							#endif
							#if (CAN_QUEUE_WRITE_FULL_EN > 0)
                          	,INT8U (* WriteFull)(INT8U,INT32U)
							,INT32U   WriteFullParam
							#endif
                          	);
void	sCanStartTx(INT8U canid);
void	sCanStopTx(INT8U canid);
void	sCanStartRx(INT8U canid);
void	sCanStopRx(INT8U canid);
void	sCanStartRx0(INT8U canid);
void	sCanStopRx0(INT8U canid);
void	sCanStartRx1(INT8U canid);
void	sCanStopRx1(INT8U canid);
INT8U	sbCanGetTxRdy(INT8U canid);
void	sCanTxData(INT8U canid, T_CanFrame *pCanFrame);

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void	CAN_Initial(INT8U canid)
{
	CAN_RecvQueOvf[canid] = 0;
	CAN_SendQueOvf[canid] = 0;
	CAN_RecvQueMaxNum[canid] = 0;
	CAN_SendQueMaxNum[canid] = 0;
	
	CAN_QueueCreate(&CanTxQueue[canid], CanTxBuf[canid], CAN_QUE_TX_Nframe, 
		CAN_QueueWriteFull, CAN_TxRx_Tx);		// ���Ͷ���
	
	CAN_QueueCreate(&CanRxQueue[canid], CanRxBuf[canid], CAN_QUE_RX_Nframe, 
		CAN_QueueWriteFull, CAN_TxRx_Rx);		// ���ն���
}

INT8U	CAN_QueueCreate(T_CanQueue *Queue, T_CanFrame *pDataBuf,
                          	INT16U nData
							#if (CAN_QUEUE_READ_EMPTY_EN > 0)
                          	,INT8U (* ReadEmpty)()
							#endif
							#if (CAN_QUEUE_WRITE_FULL_EN > 0)
                          	,INT8U (* WriteFull)(INT8U,INT32U)
							,INT32U   WriteFullParam
							#endif
                          	)
{
	OS_CPU_SR	cpu_sr;
	
	// �жϲ����Ƿ���Ч
	if (Queue != NULL && pDataBuf != NULL)
	{
		OS_ENTER_CRITICAL();
		
		// ��ʼ���ṹ������
		Queue->Buf = pDataBuf;
		Queue->MaxData = nData;						// ������п��Դ洢��������Ŀ
		Queue->End = &Queue->Buf[Queue->MaxData];	// �������ݻ���Ľ�����ַ,�ѳ���������
		Queue->Out = Queue->Buf;
		Queue->In = Queue->Buf;
		Queue->NData = 0;
		#if (CAN_QUEUE_READ_EMPTY_EN > 0)
			Queue->ReadEmpty = ReadEmpty;
		#endif
		#if (CAN_QUEUE_WRITE_FULL_EN > 0)
			Queue->WriteFull = WriteFull;
			Queue->WriteFullParam = WriteFullParam;
		#endif
		
		OS_EXIT_CRITICAL();
		
		return QUEUE_OK;
	}
	else
	{
		return NOT_OK;
	}
}

INT8U	CAN_QueueWrite(INT8U canid, T_CanQueue *Queue, T_CanFrame *pData)
{
	INT8U err;
	T_CanFrame *pIn;
	
	err = NOT_OK;
	
	// �����Ƿ���Ч
	if (Queue != NULL)
	{
		sCanStopTx(canid);					// �رշ����ж�
		if (Queue->NData < Queue->MaxData)	// �����Ƿ���
		{
			pIn = Queue->In;
			*pIn = *pData;					// �������
			++pIn;
			if (pIn >= Queue->End)
			{
				pIn = Queue->Buf;
			}
			Queue->In = pIn;				// �������ָ��
			Queue->NData++;					// ��������
			err = QUEUE_OK;
		}
		else
		{
			err = QUEUE_FULL;					// ��
			#if (CAN_QUEUE_WRITE_FULL_EN > 0)
				if (Queue->WriteFull != NULL)	// �����û�������
				{
					err = Queue->WriteFull(canid, Queue->WriteFullParam);
				}
			#endif
		}
		sCanStartTx(canid);					// ���������ж�
	}
	
	return err;
}

INT8U	CAN_QueueWriteIsr(INT8U canid, T_CanQueue *Queue, T_CanFrame *pData)
{
	INT32U err;
	T_CanFrame *pIn;
	
	err = NOT_OK;
	
	// �����Ƿ���Ч
	if (Queue != NULL)
	{
		// �����Ƿ���
		if (Queue->NData < Queue->MaxData)		// ����
		{
			pIn = Queue->In;
			*pIn = *pData;						// �������
			++pIn;
			if (pIn >= Queue->End)
			{
				pIn = Queue->Buf;
			}
			Queue->In = pIn;					// �������ָ��
			Queue->NData++;						// ��������
			err = QUEUE_OK;
		}
		else	// ��
		{
			err = QUEUE_FULL;
			#if (CAN_QUEUE_WRITE_FULL_EN > 0)
				if (Queue->WriteFull != NULL)	// �����û�������
				{
					err = Queue->WriteFull(canid, Queue->WriteFullParam);
				}
			#endif
		}
	}
	return err;
}

INT8U	CAN_QueueRead(INT8U canid, T_CanQueue *Queue, T_CanFrame *pData)
{
	INT32U err;
	T_CanFrame *pOut, *pOutOld;
	
	err = NOT_OK;
	
	// �����Ƿ���Ч
	if (Queue != NULL)				// ��Ч
	{
		sCanStopRx(canid);			// �ر����н����ж�
		// �����Ƿ�Ϊ��
		if (Queue->NData > 0)		// ����
		{
			pOut = Queue->Out;
			pOutOld = pOut;
			++pOut;
			if (pOut >= Queue->End)
			{
				pOut = Queue->Buf;
			}
			Queue->Out = pOut;		// ��������ָ��
			Queue->NData--;			// ���ݼ���
			
			sCanStartRx(canid);		// �������н����ж�
			*pData = *pOutOld;		// �����ж��ٸ��������Լ����ж�ʱ��
			err = QUEUE_OK;
		}
		else	// ��
		{
			sCanStartRx(canid);		// �������н����ж�
			err = QUEUE_EMPTY;
			#if (CAN_QUEUE_READ_EMPTY_EN > 0)
				if (Queue->ReadEmpty != NULL)	// �����û�������
				{
					err = Queue->ReadEmpty(Queue);
				}
			#endif
		}
	}
	return err;
}

INT8U	CAN_QueueReadIsr(INT8U canid, T_CanFrame *pData, T_CanQueue *Queue)
{
	INT32U err;
	T_CanFrame *pOut;
	
	err = NOT_OK;
	
	// �����Ƿ���Ч
	if (Queue != NULL)			// ��Ч
	{
		// �����Ƿ�Ϊ��
		if (Queue->NData > 0)	// ����
		{
			pOut = Queue->Out;
			*pData = *pOut;
			++pOut;
			if (pOut >= Queue->End)
			{
				pOut = Queue->Buf;
			}
			Queue->Out = pOut;	// ��������ָ��
			Queue->NData--;		// ���ݼ���
			err = QUEUE_OK;
		}
		else					// ��
		{
			err = QUEUE_EMPTY;
			#if (CAN_QUEUE_READ_EMPTY_EN > 0)
			if (Queue->ReadEmpty != NULL)		// �����û�������
			{
				err = Queue->ReadEmpty(Queue);
			}
			#endif
		}
	}
	return err;
}

INT8U	CAN_ReadFrame(INT8U canid, T_CanFrame *pData)
{
	INT32U err;
	T_CanQueue *Queue = &CanRxQueue[canid];
	T_CanFrame *pOut, *pOutOld;
	
	err = NOT_OK;
	
	// �����Ƿ���Ч
	if (Queue != NULL)				// ��Ч
	{
		#ifdef DBG_CAN_QUE_MAX
			// ����,��¼���ն�����󻺳���
			if(CanRxQueue[canid].NData > CAN_RecvQueMaxNum[canid])
			{
				CAN_RecvQueMaxNum[canid] = CanRxQueue[canid].NData;
			}
		#endif
		
		sCanStopRx(canid);			// �ر����н����ж�
		
		// �����Ƿ�Ϊ��
		if (Queue->NData > 0)		// ����
		{
			pOutOld = Queue->Out;
			pOut = pOutOld + 1;
			if (pOut >= Queue->End)
			{
				pOut = Queue->Buf;
			}
			Queue->Out = pOut;		// ��������ָ��
			Queue->NData--;			// ���ݼ���
			
			sCanStartRx(canid);		// �������н����ж�
			*pData = *pOutOld;		// �����ж��ٸ��������Լ����ж�ʱ��
			err = QUEUE_OK;
		}
		else						// ��
		{
			sCanStartRx(canid);		// �������н����ж�
			
			err = QUEUE_EMPTY;
			#if (CAN_QUEUE_READ_EMPTY_EN > 0)
			if (Queue->ReadEmpty != NULL)	// �����û�������
			{
				err = Queue->ReadEmpty(Queue);
			}
			#endif
		}
	}
	
	return err;
}

// ����������Ϊ�����͵�CAN���������ʽT_CanFrame/CAN_TypeDef
// ����ʱ��������п���ֱ�ӽ�����ѹ�����䣬��������ѹ�����
// �������ʧ�ܴ���Ϊ3��ȡ������
INT32U	CAN_SendFrame(INT8U canid, T_CanFrame *pCanFrame)
{
	OS_CPU_SR	cpu_sr;
	//CAN_TxMailBox_TypeDef *pTxMailBox;
	//INT32U dw_r;
	INT32U err = ERR_NONE;
	
	// ���ͱ��ĵ�����Ϊ��Ӧ�ó���ѡ��1�����õķ������䣻���ñ�ʶ����
	// ���ݳ��Ⱥʹ��������ݣ�Ȼ���CAN_TIxR�Ĵ�����TXRQλ�á�1�����������͡� 
	// ����״̬CAN_TSR,Datasheet.CN:p441
	OS_ENTER_CRITICAL();
	
	if(CanTxQueue[canid].NData > 0)
	{
		// ���в�Ϊ����ֱ��д�뷢�Ͷ���
		#ifdef DBG_CAN_QUE_MAX
			// ����,��¼���Ͷ������洢����
			if(CanTxQueue[canid].NData > CAN_SendQueMaxNum[canid])
			{
				CAN_SendQueMaxNum[canid] = CanTxQueue[canid].NData;
			}
		#endif
		
		err = CAN_QueueWrite(canid, &CanTxQueue[canid], pCanFrame);
	}
	else
	{
		// �������Ϊ��
		if(sbCanGetTxRdy(canid) == cCanTxRdy)	// �п��еķ�������
		{
			sCanTxData(canid, pCanFrame);
		}
		else		// �޿�������
		{
			err = CAN_QueueWrite(canid, &CanTxQueue[canid], pCanFrame);
		}
	}
	
	OS_EXIT_CRITICAL();
	
	return err;
}

void	CAN_wrFilterExt32MaskMode (INT32U filter_number, 
								INT32U id, INT32U mask, INT32U fifo_number)
{
	INT32U FilterNOBitPos;
  	
	if(filter_number > 13)	// check if Filter Memory is full
	{
		return;
	}
	
	FilterNOBitPos = 1 << filter_number;
	CAN1_PERIPH->FMR  |=  CAN_FMR_FINIT;				// �������������ģʽ
	CAN1_PERIPH->FA1R &=  ~(INT32U)(FilterNOBitPos);	// ����ָ�����������Խ�������״̬
	
	CAN1_PERIPH->FS1R |= (INT32U)(FilterNOBitPos);		// ���ù�����λ��Ϊ32λ
	CAN1_PERIPH->FM1R &= ~(INT32U)(FilterNOBitPos);	// ����Ϊ����λģʽ
	
	CAN1_PERIPH->sFilterRegister[filter_number].FR1 = id;	// ID
	CAN1_PERIPH->sFilterRegister[filter_number].FR2 = mask;// Mask
	
	if(fifo_number == 0)
	{
		CAN1_PERIPH->FFA1R &= ~(INT32U)(FilterNOBitPos);	// ������������FIFO 0
	}
	else
	{
		CAN1_PERIPH->FFA1R |= (INT32U)(FilterNOBitPos);	// ������������FIFO 1
	}
	
	CAN1_PERIPH->FA1R |= (INT32U)(FilterNOBitPos);			// ����������
	CAN1_PERIPH->FMR &= ~CAN_FMR_FINIT;					// ��������ģʽ
}

void	CAN_Rx0ISR(INT8U canid)
{
	/*
	FIFO�ӿ�״̬��ʼ���ڽ��յ���һ����Ч�ı��ĺ�FIFO״̬��Ϊ�Һ�_1(pending_1)��
	Ӳ����Ӧ�ذ�CAN_RFR�Ĵ�����FMP[1:0]����Ϊ��01��(������01b)��
	������Զ�ȡFIFO������������������еı��ģ�Ȼ��ͨ����CAN_RFR�Ĵ�����RFOMλ
	���á�1�����ͷ����䣬����FIFO�ֱ�Ϊ��״̬�ˡ�������ͷ������ͬʱ��
	���յ���һ����Ч�ı��ģ���ôFIFO��Ȼ�����ڹҺ�_1״̬��������Զ�ȡFIFO
	����������������յ��ı��ġ�
	���Ӧ�ó����ͷ����䣬�ڽ��յ���һ����Ч�ı��ĺ�FIFO״̬��Ϊ
	�Һ�_2(pending_2)��Ӳ����Ӧ�ذ�FMP[1:0]����Ϊ��10��(������10b)��
	�ظ�����Ĺ��̣���������Ч�ı��İ�FIFO��Ϊ�Һ�_3״̬(FMP[1:0]��11b)��
	��ʱ����������RFOMλ����1���ͷ����䣬�Ա�FIFO�����пռ��������һ����Ч��
	���ģ�������һ����Ч�ı��ĵ���ʱ�ͻᵼ��һ�����ĵĶ�ʧ��
	*/
#if	CAN_CAN1_EN==1
	if(canid==cCAN1_ID)
	{
		while(CAN1_PERIPH->RF0R & CAN_RF0R_FMP0)		// message pending ?
		{
			CAN_QueueWriteIsr(canid, &CanRxQueue[canid], (T_CanFrame *)&CAN1_PERIPH->sFIFOMailBox[CAN_FIFO0]);
			CAN1_PERIPH->RF0R |= CAN_RF0R_RFOM0;		// Release FIFO 0 output mailbox
		}
	}
#endif
	
#if	CAN_CAN2_EN==1
	if(canid==cCAN2_ID)
	{
		while(CAN2_PERIPH->RF0R & CAN_RF0R_FMP0)		// message pending ?
		{
			CAN_QueueWriteIsr(canid, &CanRxQueue[canid], (T_CanFrame *)&CAN2_PERIPH->sFIFOMailBox[CAN_FIFO0]);
			CAN2_PERIPH->RF0R |= CAN_RF0R_RFOM0;		// Release FIFO 0 output mailbox
		}
	}
#endif
}

void	CAN_Rx1ISR(INT8U canid)
{
#if	CAN_CAN1_EN==1
	if(canid==cCAN1_ID)
	{
		while(CAN1_PERIPH->RF1R & CAN_RF1R_FMP1)		// message pending ?
		{
			CAN_QueueWriteIsr(canid, &CanRxQueue[canid], (T_CanFrame *)&CAN1_PERIPH->sFIFOMailBox[CAN_FIFO1]);
			CAN1_PERIPH->RF1R |= CAN_RF1R_RFOM1;		// Release FIFO 1 output mailbox
		}
	}
#endif
	
#if	CAN_CAN2_EN==1
	if(canid==cCAN2_ID)
	{
		while(CAN2_PERIPH->RF1R & CAN_RF1R_FMP1)		// message pending ?
		{
			CAN_QueueWriteIsr(canid, &CanRxQueue[canid], (T_CanFrame *)&CAN2_PERIPH->sFIFOMailBox[CAN_FIFO1]);
			CAN2_PERIPH->RF1R |= CAN_RF1R_RFOM1;		// Release FIFO 1 output mailbox
		}
	}
#endif
}

void	CAN_TxISR(INT8U canid)
{
	INT32U dw_r;
	CAN_TxMailBox_TypeDef *pTxMailBox;
	// Datasheet.CN: p445
	
#if	CAN_CAN1_EN==1
	if(canid==cCAN1_ID)
	{
		if(CanTxQueue[canid].NData > 0)
	    {
	    	// ������Ͷ��в�Ϊ��
			dw_r = (CAN1_PERIPH->TSR >> CAN_TSR_TME_BITnL) & 0x07;	// ���ҿյķ�������
			if(dw_r)
			{
				// �п��еķ�������
				dw_r = TxMailBox_IdleNO_Tbl[dw_r];			// ��ȡһ���������
				pTxMailBox = &CAN1_PERIPH->sTxMailBox[dw_r];
				CAN_QueueReadIsr(canid, (T_CanFrame *)pTxMailBox, &CanTxQueue[canid]);
				// ������
				SETBITs(pTxMailBox->TIR, CAN_TIxR_TXRQ);
			}
	    }
	    else
	    {
			/* Clear CAN_TSR_RQCPx (rc_w1) */
			CAN1_PERIPH->TSR = CAN_TSR_RQCP0|CAN_TSR_RQCP1|CAN_TSR_RQCP2;	// �����������,�Է�ֹ�ٴν����ж�
		}
	}
#endif
	
#if	CAN_CAN2_EN==1
	if(canid==cCAN2_ID)
	{
		if(CanTxQueue[canid].NData > 0)
	    {
	    	// ������Ͷ��в�Ϊ��
			dw_r = (CAN2_PERIPH->TSR >> CAN_TSR_TME_BITnL) & 0x07;	// ���ҿյķ�������
			if(dw_r)
			{
				// �п��еķ�������
				dw_r = TxMailBox_IdleNO_Tbl[dw_r];			// ��ȡһ���������
				pTxMailBox = &CAN2_PERIPH->sTxMailBox[dw_r];
				CAN_QueueReadIsr(canid, (T_CanFrame *)pTxMailBox, &CanTxQueue[canid]);
				// ������
				SETBITs(pTxMailBox->TIR, CAN_TIxR_TXRQ);
			}
	    }
	    else
	    {
			/* Clear CAN_TSR_RQCPx (rc_w1) */
			CAN2_PERIPH->TSR = CAN_TSR_RQCP0|CAN_TSR_RQCP1|CAN_TSR_RQCP2;	// �����������,�Է�ֹ�ٴν����ж�
		}
	}
#endif
}

INT32U	dwCanLEC = 0;
INT32U	dwCanBOFF = 0;
INT32U	dwCanEPVF = 0;
INT32U	dwCanEWGF = 0;
void	CAN_SceISR(INT8U canid)
{
	volatile INT32U *pESR;
	INT32U dw_r;
	
#if	CAN_CAN1_EN==1
	if(canid==cCAN1_ID)
	{
		pESR = &CAN1_PERIPH->ESR;
		CAN1_PERIPH->MSR = CAN_MSR_ERRI;		// ��������ж�
		dw_r = (*pESR) & CAN_ESR_LEC;
		
		// LEC�ϴδ����
		if(dw_r)	// �д�����
		{
			if(dw_r != CAN_ESR_LEC_USER)	// ��������ϴ����ò�ͬ
			{
				dw_r = RSHFT(dw_r, CAN_ESR_LEC_BITnL)-1;
				dwCanLEC++;
				SETBITs(*pESR, CAN_ESR_LEC_USER);
			}
		}
		
		// ���ߴ���
		if(CHKBITs(*pESR, CAN_ESR_BOFF))
		{
			dwCanBOFF++;
			CLRBITs(*pESR, CAN_ESR_BOFF | CAN_ESR_TEC);
		}
		
		// ��������
		if(CHKBITs(*pESR, CAN_ESR_EPVF))
		{
			dwCanEPVF++;
			CLRBITs(*pESR, CAN_ESR_EPVF | CAN_ESR_REC);
		}
		
		// ���󾯸�
		if(CHKBITs(*pESR, CAN_ESR_EWGF))
		{
			dwCanEWGF++;
			CLRBITs(*pESR, CAN_ESR_EWGF);
		}
		
		dw_r = (*pESR);
		// ���ʧ�ܴ���̫����ȡ�����͵�ǰ֡
		if( (dw_r & CAN_ESR_TEC) > (10 << CAN_ESR_TEC_BITnL) )
		{
			CLRBITs(*pESR, CAN_ESR_TEC);
			// Clear CAN_TSR_RQCPx (rc_w1)
			CAN1_PERIPH->TSR = CAN_TSR_RQCP0|CAN_TSR_RQCP1|CAN_TSR_RQCP2;
		}
	}
#endif
	
#if	CAN_CAN2_EN==1
	if(canid==cCAN2_ID)
	{
		pESR = &CAN2_PERIPH->ESR;
		CAN2_PERIPH->MSR = CAN_MSR_ERRI;		// ��������ж�
		dw_r = (*pESR) & CAN_ESR_LEC;
		
		// LEC�ϴδ����
		if(dw_r)	// �д�����
		{
			if(dw_r != CAN_ESR_LEC_USER)	// ��������ϴ����ò�ͬ
			{
				dw_r = RSHFT(dw_r, CAN_ESR_LEC_BITnL)-1;
				dwCanLEC++;
				SETBITs(*pESR, CAN_ESR_LEC_USER);
			}
		}
		
		// ���ߴ���
		if(CHKBITs(*pESR, CAN_ESR_BOFF))
		{
			dwCanBOFF++;
			CLRBITs(*pESR, CAN_ESR_BOFF | CAN_ESR_TEC);
		}
		
		// ��������
		if(CHKBITs(*pESR, CAN_ESR_EPVF))
		{
			dwCanEPVF++;
			CLRBITs(*pESR, CAN_ESR_EPVF | CAN_ESR_REC);
		}
		
		// ���󾯸�
		if(CHKBITs(*pESR, CAN_ESR_EWGF))
		{
			dwCanEWGF++;
			CLRBITs(*pESR, CAN_ESR_EWGF);
		}
		
		dw_r = (*pESR);
		// ���ʧ�ܴ���̫����ȡ�����͵�ǰ֡
		if( (dw_r & CAN_ESR_TEC) > (10 << CAN_ESR_TEC_BITnL) )
		{
			CLRBITs(*pESR, CAN_ESR_TEC);
			// Clear CAN_TSR_RQCPx (rc_w1)
			CAN2_PERIPH->TSR = CAN_TSR_RQCP0|CAN_TSR_RQCP1|CAN_TSR_RQCP2;
		}
	}
#endif
}

INT8U	CAN_QueueWriteFull(INT8U canid, INT32U dwTxRx)
{
#if	CAN_CAN1_EN==1
	if(canid==cCAN1_ID)
	{
		if(dwTxRx == CAN_TxRx_Tx)
		{
			CAN_SendQueOvf[canid]++;
			if(CAN_SendQueOvf[canid] >= 100)
			{
				SETBITs(CAN1_PERIPH->MCR, 0x00000001);
				//__NOP;
				//__NOP;
				//__NOP;
				CLRBITs(CAN1_PERIPH->MCR, 0x00000001);
			}
		}
		else
		{
			CAN_RecvQueOvf[canid]++;
			if(CAN_RecvQueOvf[canid] >= 100)
			{
				SETBITs(CAN1_PERIPH->MCR, 0x00000001);
				//__NOP;
				//__NOP;
				//__NOP;
				CLRBITs(CAN1_PERIPH->MCR, 0x00000001);
			}
		}
	}
#endif
	
#if	CAN_CAN2_EN==1
	if(canid==cCAN2_ID)
	{
		if(dwTxRx == CAN_TxRx_Tx)
		{
			CAN_SendQueOvf[canid]++;
			if(CAN_SendQueOvf[canid] >= 100)
			{
				SETBITs(CAN2_PERIPH->MCR, 0x00000001);
				//__NOP;
				//__NOP;
				//__NOP;
				CLRBITs(CAN2_PERIPH->MCR, 0x00000001);
			}
		}
		else
		{
			CAN_RecvQueOvf[canid]++;
			if(CAN_RecvQueOvf[canid] >= 100)
			{
				SETBITs(CAN2_PERIPH->MCR, 0x00000001);
				//__NOP;
				//__NOP;
				//__NOP;
				CLRBITs(CAN2_PERIPH->MCR, 0x00000001);
			}
		}
	}
#endif
	
	return QUEUE_OK;
}

void	sCanStartTx(INT8U canid)
{
#if	CAN_CAN1_EN==1
	if(canid==cCAN1_ID)
	{
		CAN1_INT_TX_ENABLE();
	}
#endif
	
#if	CAN_CAN2_EN==1
	if(canid==cCAN2_ID)
	{
		CAN2_INT_TX_ENABLE();
	}
#endif
}

void	sCanStopTx(INT8U canid)
{
#if	CAN_CAN1_EN==1
	if(canid==cCAN1_ID)
	{
		CAN1_INT_TX_DISABLE();
	}
#endif
	
#if	CAN_CAN2_EN==1
	if(canid==cCAN2_ID)
	{
		CAN2_INT_TX_DISABLE();
	}
#endif
}

void	sCanStartRx(INT8U canid)
{
#if	CAN_CAN1_EN==1
	if(canid==cCAN1_ID)
	{
		CAN1_INT_RX_ENABLE();
	}
#endif
	
#if	CAN_CAN2_EN==1
	if(canid==cCAN2_ID)
	{
		CAN2_INT_RX_ENABLE();
	}
#endif
}

void	sCanStopRx(INT8U canid)
{
#if	CAN_CAN1_EN==1
	if(canid==cCAN1_ID)
	{
		CAN1_INT_RX_DISABLE();
	}
#endif
	
#if	CAN_CAN2_EN==1
	if(canid==cCAN2_ID)
	{
		CAN2_INT_RX_DISABLE();
	}
#endif
}

void	sCanStartRx0(INT8U canid)
{
#if	CAN_CAN1_EN==1
	if(canid==cCAN1_ID)
	{
		CAN1_INT_RX0_ENABLE();
	}
#endif
	
#if	CAN_CAN2_EN==1
	if(canid==cCAN2_ID)
	{
		CAN2_INT_RX0_ENABLE();
	}
#endif
}

void	sCanStopRx0(INT8U canid)
{
#if	CAN_CAN1_EN==1
	if(canid==cCAN1_ID)
	{
		CAN1_INT_RX0_DISABLE();
	}
#endif
	
#if	CAN_CAN2_EN==1
	if(canid==cCAN2_ID)
	{
		CAN2_INT_RX0_DISABLE();
	}
#endif
}

void	sCanStartRx1(INT8U canid)
{
#if	CAN_CAN1_EN==1
	if(canid==cCAN1_ID)
	{
		CAN1_INT_RX1_ENABLE();
	}
#endif
	
#if	CAN_CAN2_EN==1
	if(canid==cCAN2_ID)
	{
		CAN2_INT_RX1_ENABLE();
	}
#endif
}

void	sCanStopRx1(INT8U canid)
{
#if	CAN_CAN1_EN==1
	if(canid==cCAN1_ID)
	{
		CAN1_INT_RX1_DISABLE();
	}
#endif
	
#if	CAN_CAN2_EN==1
	if(canid==cCAN2_ID)
	{
		CAN2_INT_RX1_DISABLE();
	}
#endif
}

INT8U	sbCanGetTxRdy(INT8U canid)		// ����Ƿ��п�������
{
#if	CAN_CAN1_EN==1
	if(canid==cCAN1_ID)
	{
		if(((CAN1_PERIPH->TSR >> CAN_TSR_TME_BITnL) & 0x07) == 0)
		{
			return cCanTxBusy;
		}
		else
		{
			return cCanTxRdy;
		}
	}
#endif
	
#if	CAN_CAN2_EN==1
	if(canid==cCAN2_ID)
	{
		if(((CAN2_PERIPH->TSR >> CAN_TSR_TME_BITnL) & 0x07) == 0)
		{
			return cCanTxBusy;
		}
		else
		{
			return cCanTxRdy;
		}
	}
#endif
	
	return cCanTxBusy;
}

void	sCanTxData(INT8U canid, T_CanFrame *pCanFrame)
{
	CAN_TxMailBox_TypeDef *pTxMailBox;
	INT32U dw_r;
	
#if	CAN_CAN1_EN==1
	if(canid==cCAN1_ID)
	{
		//CAN_INT_TX_DISABLE();	
		// �����ʹ�ò���ϵͳ���Ҳ����ж��е��ô˺�������������ɲ��ر��ж�
		
		dw_r = (CAN1_PERIPH->TSR >> CAN_TSR_TME_BITnL) & 0x07;
		pTxMailBox = &CAN1_PERIPH->sTxMailBox[TxMailBox_IdleNO_Tbl[dw_r]];
		//pTxMailBox->TIR = pCanFrame->IxR;
		pTxMailBox->TDTR = pCanFrame->DTxR;
		pTxMailBox->TDLR = pCanFrame->Data.u32[0];
		pTxMailBox->TDHR = pCanFrame->Data.u32[1];
		//SETBITs(pTxMailBox->TIR, CAN_TIxR_TXRQ);
		pTxMailBox->TIR = pCanFrame->IxR | CAN_TIxR_TXRQ;
		
		//CAN_INT_TX_ENABLE();	// ���������ж�
	}
#endif
	
#if	CAN_CAN2_EN==1
	if(canid==cCAN2_ID)
	{
		//CAN_INT_TX_DISABLE();	
		// �����ʹ�ò���ϵͳ���Ҳ����ж��е��ô˺�������������ɲ��ر��ж�
		
		dw_r = (CAN2_PERIPH->TSR >> CAN_TSR_TME_BITnL) & 0x07;
		pTxMailBox = &CAN2_PERIPH->sTxMailBox[TxMailBox_IdleNO_Tbl[dw_r]];
		//pTxMailBox->TIR = pCanFrame->IxR;
		pTxMailBox->TDTR = pCanFrame->DTxR;
		pTxMailBox->TDLR = pCanFrame->Data.u32[0];
		pTxMailBox->TDHR = pCanFrame->Data.u32[1];
		//SETBITs(pTxMailBox->TIR, CAN_TIxR_TXRQ);
		pTxMailBox->TIR = pCanFrame->IxR | CAN_TIxR_TXRQ;
		
		//CAN_INT_TX_ENABLE();	// ���������ж�
	}
#endif

}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/


/********************************************************************************
* Input interface Routines														*
********************************************************************************/


