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
T_CanFrame CanTxBuf[MAX_CAN_NO][CAN_QUE_TX_Nframe] = {0,};	// CAN发送缓冲区
T_CanFrame CanRxBuf[MAX_CAN_NO][CAN_QUE_RX_Nframe] = {0,};	// CAN接收缓冲区
//static T_CanFrame CanTxBuf[MAX_CAN_NO][CAN_QUE_TX_Nframe] = {0,};	// CAN发送缓冲区
//static T_CanFrame CanRxBuf[MAX_CAN_NO][CAN_QUE_RX_Nframe] = {0,};	// CAN接收缓冲区
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
		CAN_QueueWriteFull, CAN_TxRx_Tx);		// 发送队列
	
	CAN_QueueCreate(&CanRxQueue[canid], CanRxBuf[canid], CAN_QUE_RX_Nframe, 
		CAN_QueueWriteFull, CAN_TxRx_Rx);		// 接收队列
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
	
	// 判断参数是否有效
	if (Queue != NULL && pDataBuf != NULL)
	{
		OS_ENTER_CRITICAL();
		
		// 初始化结构体数据
		Queue->Buf = pDataBuf;
		Queue->MaxData = nData;						// 计算队列可以存储的数据数目
		Queue->End = &Queue->Buf[Queue->MaxData];	// 计算数据缓冲的结束地址,已超出缓冲区
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
	
	// 队列是否有效
	if (Queue != NULL)
	{
		sCanStopTx(canid);					// 关闭发送中断
		if (Queue->NData < Queue->MaxData)	// 队列是否满
		{
			pIn = Queue->In;
			*pIn = *pData;					// 数据入队
			++pIn;
			if (pIn >= Queue->End)
			{
				pIn = Queue->Buf;
			}
			Queue->In = pIn;				// 调整入队指针
			Queue->NData++;					// 数据增加
			err = QUEUE_OK;
		}
		else
		{
			err = QUEUE_FULL;					// 满
			#if (CAN_QUEUE_WRITE_FULL_EN > 0)
				if (Queue->WriteFull != NULL)	// 调用用户处理函数
				{
					err = Queue->WriteFull(canid, Queue->WriteFullParam);
				}
			#endif
		}
		sCanStartTx(canid);					// 开启发送中断
	}
	
	return err;
}

INT8U	CAN_QueueWriteIsr(INT8U canid, T_CanQueue *Queue, T_CanFrame *pData)
{
	INT32U err;
	T_CanFrame *pIn;
	
	err = NOT_OK;
	
	// 队列是否有效
	if (Queue != NULL)
	{
		// 队列是否满
		if (Queue->NData < Queue->MaxData)		// 不满
		{
			pIn = Queue->In;
			*pIn = *pData;						// 数据入队
			++pIn;
			if (pIn >= Queue->End)
			{
				pIn = Queue->Buf;
			}
			Queue->In = pIn;					// 调整入队指针
			Queue->NData++;						// 数据增加
			err = QUEUE_OK;
		}
		else	// 满
		{
			err = QUEUE_FULL;
			#if (CAN_QUEUE_WRITE_FULL_EN > 0)
				if (Queue->WriteFull != NULL)	// 调用用户处理函数
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
	
	// 队列是否有效
	if (Queue != NULL)				// 有效
	{
		sCanStopRx(canid);			// 关闭所有接收中断
		// 队列是否为空
		if (Queue->NData > 0)		// 不空
		{
			pOut = Queue->Out;
			pOutOld = pOut;
			++pOut;
			if (pOut >= Queue->End)
			{
				pOut = Queue->Buf;
			}
			Queue->Out = pOut;		// 调整出队指针
			Queue->NData--;			// 数据减少
			
			sCanStartRx(canid);		// 开启所有接收中断
			*pData = *pOutOld;		// 开启中断再复制数据以减少中断时间
			err = QUEUE_OK;
		}
		else	// 空
		{
			sCanStartRx(canid);		// 开启所有接收中断
			err = QUEUE_EMPTY;
			#if (CAN_QUEUE_READ_EMPTY_EN > 0)
				if (Queue->ReadEmpty != NULL)	// 调用用户处理函数
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
	
	// 队列是否有效
	if (Queue != NULL)			// 有效
	{
		// 队列是否为空
		if (Queue->NData > 0)	// 不空
		{
			pOut = Queue->Out;
			*pData = *pOut;
			++pOut;
			if (pOut >= Queue->End)
			{
				pOut = Queue->Buf;
			}
			Queue->Out = pOut;	// 调整出队指针
			Queue->NData--;		// 数据减少
			err = QUEUE_OK;
		}
		else					// 空
		{
			err = QUEUE_EMPTY;
			#if (CAN_QUEUE_READ_EMPTY_EN > 0)
			if (Queue->ReadEmpty != NULL)		// 调用用户处理函数
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
	
	// 队列是否有效
	if (Queue != NULL)				// 有效
	{
		#ifdef DBG_CAN_QUE_MAX
			// 调试,记录接收队列最大缓冲数
			if(CanRxQueue[canid].NData > CAN_RecvQueMaxNum[canid])
			{
				CAN_RecvQueMaxNum[canid] = CanRxQueue[canid].NData;
			}
		#endif
		
		sCanStopRx(canid);			// 关闭所有接收中断
		
		// 队列是否为空
		if (Queue->NData > 0)		// 不空
		{
			pOutOld = Queue->Out;
			pOut = pOutOld + 1;
			if (pOut >= Queue->End)
			{
				pOut = Queue->Buf;
			}
			Queue->Out = pOut;		// 调整出队指针
			Queue->NData--;			// 数据减少
			
			sCanStartRx(canid);		// 开启所有接收中断
			*pData = *pOutOld;		// 开启中断再复制数据以减少中断时间
			err = QUEUE_OK;
		}
		else						// 空
		{
			sCanStartRx(canid);		// 开启所有接收中断
			
			err = QUEUE_EMPTY;
			#if (CAN_QUEUE_READ_EMPTY_EN > 0)
			if (Queue->ReadEmpty != NULL)	// 调用用户处理函数
			{
				err = Queue->ReadEmpty(Queue);
			}
			#endif
		}
	}
	
	return err;
}

// 将数据设置为欲发送的CAN发送邮箱格式T_CanFrame/CAN_TypeDef
// 发送时如果邮箱有空则直接将数据压入邮箱，否则将数据压入队列
// 如果发送失败次数为3则取消发送
INT32U	CAN_SendFrame(INT8U canid, T_CanFrame *pCanFrame)
{
	OS_CPU_SR	cpu_sr;
	//CAN_TxMailBox_TypeDef *pTxMailBox;
	//INT32U dw_r;
	INT32U err = ERR_NONE;
	
	// 发送报文的流程为：应用程序选择1个空置的发送邮箱；设置标识符，
	// 数据长度和待发送数据；然后对CAN_TIxR寄存器的TXRQ位置’1’，来请求发送。 
	// 发送状态CAN_TSR,Datasheet.CN:p441
	OS_ENTER_CRITICAL();
	
	if(CanTxQueue[canid].NData > 0)
	{
		// 队列不为空则直接写入发送队列
		#ifdef DBG_CAN_QUE_MAX
			// 调试,记录发送队列最大存储数量
			if(CanTxQueue[canid].NData > CAN_SendQueMaxNum[canid])
			{
				CAN_SendQueMaxNum[canid] = CanTxQueue[canid].NData;
			}
		#endif
		
		err = CAN_QueueWrite(canid, &CanTxQueue[canid], pCanFrame);
	}
	else
	{
		// 如果队列为空
		if(sbCanGetTxRdy(canid) == cCanTxRdy)	// 有空闲的发送邮箱
		{
			sCanTxData(canid, pCanFrame);
		}
		else		// 无空闲邮箱
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
	CAN1_PERIPH->FMR  |=  CAN_FMR_FINIT;				// 进入过滤器设置模式
	CAN1_PERIPH->FA1R &=  ~(INT32U)(FilterNOBitPos);	// 禁用指定过滤器，以进入设置状态
	
	CAN1_PERIPH->FS1R |= (INT32U)(FilterNOBitPos);		// 设置过滤器位宽为32位
	CAN1_PERIPH->FM1R &= ~(INT32U)(FilterNOBitPos);	// 设置为屏蔽位模式
	
	CAN1_PERIPH->sFilterRegister[filter_number].FR1 = id;	// ID
	CAN1_PERIPH->sFilterRegister[filter_number].FR2 = mask;// Mask
	
	if(fifo_number == 0)
	{
		CAN1_PERIPH->FFA1R &= ~(INT32U)(FilterNOBitPos);	// 过滤器分配至FIFO 0
	}
	else
	{
		CAN1_PERIPH->FFA1R |= (INT32U)(FilterNOBitPos);	// 过滤器分配至FIFO 1
	}
	
	CAN1_PERIPH->FA1R |= (INT32U)(FilterNOBitPos);			// 开启过滤器
	CAN1_PERIPH->FMR &= ~CAN_FMR_FINIT;					// 进入正常模式
}

void	CAN_Rx0ISR(INT8U canid)
{
	/*
	FIFO从空状态开始，在接收到第一个有效的报文后，FIFO状态变为挂号_1(pending_1)，
	硬件相应地把CAN_RFR寄存器的FMP[1:0]设置为’01’(二进制01b)。
	软件可以读取FIFO输出邮箱来读出邮箱中的报文，然后通过对CAN_RFR寄存器的RFOM位
	设置’1’来释放邮箱，这样FIFO又变为空状态了。如果在释放邮箱的同时，
	又收到了一个有效的报文，那么FIFO仍然保留在挂号_1状态，软件可以读取FIFO
	输出邮箱来读出新收到的报文。
	如果应用程序不释放邮箱，在接收到下一个有效的报文后，FIFO状态变为
	挂号_2(pending_2)，硬件相应地把FMP[1:0]设置为’10’(二进制10b)。
	重复上面的过程，第三个有效的报文把FIFO变为挂号_3状态(FMP[1:0]＝11b)。
	此时，软件必须对RFOM位设置1来释放邮箱，以便FIFO可以有空间来存放下一个有效的
	报文；否则，下一个有效的报文到来时就会导致一个报文的丢失。
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
	    	// 如果发送队列不为空
			dw_r = (CAN1_PERIPH->TSR >> CAN_TSR_TME_BITnL) & 0x07;	// 查找空的发送邮箱
			if(dw_r)
			{
				// 有空闲的发送邮箱
				dw_r = TxMailBox_IdleNO_Tbl[dw_r];			// 获取一个空邮箱号
				pTxMailBox = &CAN1_PERIPH->sTxMailBox[dw_r];
				CAN_QueueReadIsr(canid, (T_CanFrame *)pTxMailBox, &CanTxQueue[canid]);
				// 请求发送
				SETBITs(pTxMailBox->TIR, CAN_TIxR_TXRQ);
			}
	    }
	    else
	    {
			/* Clear CAN_TSR_RQCPx (rc_w1) */
			CAN1_PERIPH->TSR = CAN_TSR_RQCP0|CAN_TSR_RQCP1|CAN_TSR_RQCP2;	// 清除发送请求,以防止再次进入中断
		}
	}
#endif
	
#if	CAN_CAN2_EN==1
	if(canid==cCAN2_ID)
	{
		if(CanTxQueue[canid].NData > 0)
	    {
	    	// 如果发送队列不为空
			dw_r = (CAN2_PERIPH->TSR >> CAN_TSR_TME_BITnL) & 0x07;	// 查找空的发送邮箱
			if(dw_r)
			{
				// 有空闲的发送邮箱
				dw_r = TxMailBox_IdleNO_Tbl[dw_r];			// 获取一个空邮箱号
				pTxMailBox = &CAN2_PERIPH->sTxMailBox[dw_r];
				CAN_QueueReadIsr(canid, (T_CanFrame *)pTxMailBox, &CanTxQueue[canid]);
				// 请求发送
				SETBITs(pTxMailBox->TIR, CAN_TIxR_TXRQ);
			}
	    }
	    else
	    {
			/* Clear CAN_TSR_RQCPx (rc_w1) */
			CAN2_PERIPH->TSR = CAN_TSR_RQCP0|CAN_TSR_RQCP1|CAN_TSR_RQCP2;	// 清除发送请求,以防止再次进入中断
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
		CAN1_PERIPH->MSR = CAN_MSR_ERRI;		// 清除错误中断
		dw_r = (*pESR) & CAN_ESR_LEC;
		
		// LEC上次错误号
		if(dw_r)	// 有错误发生
		{
			if(dw_r != CAN_ESR_LEC_USER)	// 错误号与上次设置不同
			{
				dw_r = RSHFT(dw_r, CAN_ESR_LEC_BITnL)-1;
				dwCanLEC++;
				SETBITs(*pESR, CAN_ESR_LEC_USER);
			}
		}
		
		// 离线错误
		if(CHKBITs(*pESR, CAN_ESR_BOFF))
		{
			dwCanBOFF++;
			CLRBITs(*pESR, CAN_ESR_BOFF | CAN_ESR_TEC);
		}
		
		// 被动错误
		if(CHKBITs(*pESR, CAN_ESR_EPVF))
		{
			dwCanEPVF++;
			CLRBITs(*pESR, CAN_ESR_EPVF | CAN_ESR_REC);
		}
		
		// 错误警告
		if(CHKBITs(*pESR, CAN_ESR_EWGF))
		{
			dwCanEWGF++;
			CLRBITs(*pESR, CAN_ESR_EWGF);
		}
		
		dw_r = (*pESR);
		// 如果失败次数太多则取消发送当前帧
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
		CAN2_PERIPH->MSR = CAN_MSR_ERRI;		// 清除错误中断
		dw_r = (*pESR) & CAN_ESR_LEC;
		
		// LEC上次错误号
		if(dw_r)	// 有错误发生
		{
			if(dw_r != CAN_ESR_LEC_USER)	// 错误号与上次设置不同
			{
				dw_r = RSHFT(dw_r, CAN_ESR_LEC_BITnL)-1;
				dwCanLEC++;
				SETBITs(*pESR, CAN_ESR_LEC_USER);
			}
		}
		
		// 离线错误
		if(CHKBITs(*pESR, CAN_ESR_BOFF))
		{
			dwCanBOFF++;
			CLRBITs(*pESR, CAN_ESR_BOFF | CAN_ESR_TEC);
		}
		
		// 被动错误
		if(CHKBITs(*pESR, CAN_ESR_EPVF))
		{
			dwCanEPVF++;
			CLRBITs(*pESR, CAN_ESR_EPVF | CAN_ESR_REC);
		}
		
		// 错误警告
		if(CHKBITs(*pESR, CAN_ESR_EWGF))
		{
			dwCanEWGF++;
			CLRBITs(*pESR, CAN_ESR_EWGF);
		}
		
		dw_r = (*pESR);
		// 如果失败次数太多则取消发送当前帧
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

INT8U	sbCanGetTxRdy(INT8U canid)		// 检查是否有空闲邮箱
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
		// 如果不使用操作系统，且不在中断中调用此函数发送数据则可不关闭中断
		
		dw_r = (CAN1_PERIPH->TSR >> CAN_TSR_TME_BITnL) & 0x07;
		pTxMailBox = &CAN1_PERIPH->sTxMailBox[TxMailBox_IdleNO_Tbl[dw_r]];
		//pTxMailBox->TIR = pCanFrame->IxR;
		pTxMailBox->TDTR = pCanFrame->DTxR;
		pTxMailBox->TDLR = pCanFrame->Data.u32[0];
		pTxMailBox->TDHR = pCanFrame->Data.u32[1];
		//SETBITs(pTxMailBox->TIR, CAN_TIxR_TXRQ);
		pTxMailBox->TIR = pCanFrame->IxR | CAN_TIxR_TXRQ;
		
		//CAN_INT_TX_ENABLE();	// 开启发送中断
	}
#endif
	
#if	CAN_CAN2_EN==1
	if(canid==cCAN2_ID)
	{
		//CAN_INT_TX_DISABLE();	
		// 如果不使用操作系统，且不在中断中调用此函数发送数据则可不关闭中断
		
		dw_r = (CAN2_PERIPH->TSR >> CAN_TSR_TME_BITnL) & 0x07;
		pTxMailBox = &CAN2_PERIPH->sTxMailBox[TxMailBox_IdleNO_Tbl[dw_r]];
		//pTxMailBox->TIR = pCanFrame->IxR;
		pTxMailBox->TDTR = pCanFrame->DTxR;
		pTxMailBox->TDLR = pCanFrame->Data.u32[0];
		pTxMailBox->TDHR = pCanFrame->Data.u32[1];
		//SETBITs(pTxMailBox->TIR, CAN_TIxR_TXRQ);
		pTxMailBox->TIR = pCanFrame->IxR | CAN_TIxR_TXRQ;
		
		//CAN_INT_TX_ENABLE();	// 开启发送中断
	}
#endif

}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/


/********************************************************************************
* Input interface Routines														*
********************************************************************************/


