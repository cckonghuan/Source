/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		sci_driver.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define		SCI_DRIVER

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"sci_config.h"                                       
#include	"sci_driver.h"
#include	"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define cQBufNormal				0
#define cQBufFull				1
#define	cQBufEmpty				2

// return value of read operation
#define	cSciRxRdy				0
#define	cSciRxBusy				1
#define	cSciRxBufEmpty			1

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
typedef struct{
	INT8U	*pIn;
	INT8U	*pOut;
	INT8U	*pStart;
	INT16U	length;
	INT16U	size;
}QUEUE;

typedef struct{
	INT8U	bTxStatus;
	INT8U	bTxLength;
	INT8U	*pbTx;
	QUEUE	*pqRx;
}SciDrvStruct;

SciDrvStruct SciList[MAX_SCI_NO];
SciDrvStruct *pSciIndex[MAX_SCI_NO];
QUEUE QList[MAX_SCI_NO];
INT8U bSciRxBuf[MAX_SCI_RX_SIZE];
INT8U *pSciBuf = bSciRxBuf;
INT8U bSciNo = 0;

typedef struct{
	INT8U bRxLen;
	INT8U bRxLenTemp;
	INT8U bRxBuf[cSCI_RX_LEN];
	INT8U bTxBuf[cSCI_TX_LEN];
	INT8U bTaskPrio;
	INT8U bRxEvent;
	INT8U bTimeout;
	INT8U bConnectFlag;
	INT16U wDisconnectCnt;
}SciDataStruct;
SciDataStruct strSciDataList[MAX_SCI_NO];

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
void	sQInit(QUEUE *pq,INT8U *start,INT16U size); 
INT8U	sQDataIn(QUEUE	*pq,INT8U data); 
INT8U	sQDataOut(QUEUE *pq,INT8U *pdata);
INT8U	sSciRead(INT8U	sciid,INT8U *pBuf);
INT8U	sbSciGetRxData(INT8U sciid);
INT8U	sbSciGetRxRdy(INT8U sciid);
INT8U	sbSciGetTxRdy(INT8U sciid);
void	sSciTxData(INT8U sciid,INT8U data);
void	sSciStopTx(INT8U sciid);
void	sSciStartTx(INT8U sciid);

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void	sSciInitial(INT8U sciid, INT8U rx_size, INT8U prio, INT8U event)
{
	QUEUE *pq;
	SciDrvStruct *psci;
	SciDataStruct *pSciData;
	
	psci=&SciList[bSciNo];
	pSciIndex[sciid]=psci;
	
	psci->pqRx=&QList[bSciNo];
	pq=psci->pqRx;
	sQInit(pq,pSciBuf,rx_size);
	pSciBuf+=rx_size;
	bSciNo++;
	
	psci->bTxStatus=cSciTxRdy;
	psci->bTxLength=0;
	
	pSciData = &strSciDataList[sciid];
	memset(&strSciDataList[sciid], 0, sizeof(SciDataStruct));
	pSciData->bTaskPrio = prio;
	pSciData->bRxEvent = event;
}

void	sSciRxTask(INT8U sciid, INT8U bTimeout)
{
	INT8U bTemp = cSciRxRdy;
	SciDataStruct *pSciData;
	
	pSciData = &strSciDataList[sciid];
	
	while(bTemp == cSciRxRdy)
	{
		bTemp = sSciRead(sciid, &pSciData->bRxBuf[pSciData->bRxLenTemp]);
		if(bTemp == cSciRxRdy)
		{
			pSciData->bTimeout = bTimeout;
			if(pSciData->bTimeout < 1)
			{
				pSciData->bTimeout = 1;
			}
			
			if(pSciData->bRxLenTemp < cSCI_RX_LEN)
			{
				pSciData->bRxLenTemp++;
			}
		}
	}
	
	if(pSciData->bRxLenTemp != 0)
	{
		if(pSciData->bTimeout == 0)
		{
			pSciData->bRxLen = pSciData->bRxLenTemp;
			pSciData->bRxLenTemp = 0;
			OSEventSend(pSciData->bTaskPrio,pSciData->bRxEvent);
		}
		else
		{
			pSciData->bTimeout--;
		}
	}
}

/************************************************************************************
*Function name: sQInit																*
*Parameters:	pq: pointer to queue structure to be initialized					*
*				start:start address of ring buffer									*
*				size:the size of the ring buffer									*
*Description:	initialize a queue structure										*
*************************************************************************************/
void	sQInit(QUEUE *pq,INT8U *start,INT16U size)
{
	pq->pIn=start;
	pq->pOut=start;
	pq->pStart=start;
	pq->length=0;
	pq->size=size;
}

/************************************************************************************
*Function name: sQDataIn															*
*Parameters:	pq: pointer to queue structure to be initialized					*
*				data:the data to be inserted into the queue 						*
*Returns:		cQBufNormal:data has been inserted into the queue					*
*				cQBufFull:the buffer is full										*
*Description:	insert a data into the queue										*
*************************************************************************************/
INT8U	sQDataIn(QUEUE	*pq,INT8U data)
{
	if(pq->length==pq->size)
	{
		if(pq->pIn==pq->pStart)
		{
			*(pq->pStart+pq->size-1)=data;
		}
		else
		{
			*(pq->pIn-1)=data;
		}
		return(cQBufFull);
	}
	else
	{
		*(pq->pIn)=data;
		pq->length++;
		if(pq->pIn==pq->pStart+pq->size-1)
		{
			pq->pIn=pq->pStart;
		}
		else
		{
			
			pq->pIn++;
		}
		return(cQBufNormal);
	}
}
/************************************************************************************
*Function name: sQDataOut															*
*Parameters:	pq: pointer to queue structure to be initialized					*
*				pdata:the address to save the data									*
*Returns:		cQBufNormal:data has been inserted into the queue					*
*				cQBufEmpty:the buffer is empty										*
*Description:	Get a data from the queue											*
*************************************************************************************/
INT8U	sQDataOut(QUEUE *pq,INT8U *pdata)
{
	if(pq->length==0)
	{
		return(cQBufEmpty);
	}
	*pdata=*(pq->pOut);
	pq->length--;
	if(pq->pOut==pq->pStart+pq->size-1)
	{
		pq->pOut=pq->pStart;
	}
	else
	{
		pq->pOut++;
	}	
	return(cQBufNormal);
}

/************************************************************************************
*Function Name: sSciRxISR															*
*Parameters:	sciid:		sci id													*
*Description:	This function is executed in Sci rx interrupt io2sci rx compare 	*
*				interrupt.															*
*************************************************************************************/
void	sSciRxISR(INT8U sciid)
{
	INT8U	data;
	SciDrvStruct		*psci;
	QUEUE			*pq;
	
	psci=pSciIndex[sciid];
	pq=psci->pqRx;
	
	if(sbSciGetRxRdy(sciid))
	{
		data=sbSciGetRxData(sciid);
		sQDataIn(pq,data);
	}
}

	
/************************************************************************************
*Function Name: sSciRead															*
*Parameters:	sciid:		sci id													*
*				*pBuf:		address to save data received							*
*Returns:		cSciRxBufEmpty: receive  buffer is empty							*
*				cSciRxRdy:		get one byte data successfully						*
*Description:	This function is executed in AP 									*
*************************************************************************************/
INT8U	sSciRead(INT8U	sciid,INT8U *pBuf)
{
	QUEUE			*pq;
	INT8U	temp;
	SciDrvStruct		*psci;	
	OS_CPU_SR cpu_sr;
	
	psci=pSciIndex[sciid];
	pq=psci->pqRx;
	
	
	OS_ENTER_CRITICAL();
	temp=sQDataOut(pq,pBuf);
	OS_EXIT_CRITICAL();
	
	if(temp==cQBufEmpty)
	{
		return(cSciRxBufEmpty);
	}
	else 
	{
		return(cSciRxRdy);
	}
}
	

/************************************************************************************
*Function Name: sSciTxISR															*
*Parameters:	sciid:		sci id													*
*Description:	This function is executed in Sci Tx interrupt io2sci Tx compare 	*
*				interrupt.															*
*************************************************************************************/
void	sSciTxISR(INT8U sciid)
{
	SciDrvStruct		*psci;
	
	psci=pSciIndex[sciid];		
	
	if(sbSciGetTxRdy(sciid)==cSciTxRdy)
	{
		if(psci->bTxLength==0)
		{
			psci->bTxStatus=cSciTxRdy;			
			sSciStopTx(sciid);
		}
		else
		{
			sSciTxData(sciid,*(psci->pbTx));
			(psci->pbTx)++;
			(psci->bTxLength)--;
		}
	}	
}
/************************************************************************************
*Function Name: sSciWrite															*
*Parameters:	sciid:		sci id													*
*				*pstart:	start address of data to be sent						*
*				wLength:	the length of data to be send							*
*Returns:		cSciTxBufFull:	transmit  buffer is empty							*
*				cSciTxRdy:		send one byte data successfully 					*
*Description:	This function is executed in AP 									*
*************************************************************************************/
INT8U	sSciWrite(INT8U sciid,INT8U *pstart,INT8U Length)
{
	SciDrvStruct		*psci;
	OS_CPU_SR cpu_sr;
	
	psci=pSciIndex[sciid];
	
	if(psci->bTxStatus==cSciTxBusy)
	{
		return(cSciTxBusy);
	}
	
	OS_ENTER_CRITICAL();
	psci->pbTx=pstart;
	psci->bTxLength=Length;
	psci->bTxStatus=cSciTxBusy;
	
	if(psci->bTxLength!=0)
	{
		sSciStartTx(sciid);
	}
	
	OS_EXIT_CRITICAL();
	
	return(cSciTxRdy);
}
/************************************************************************************
*Function Name: sSciGetTxStatus 													*
*Parameters:	sciid:		sci id													*
*Returns:		sci tx status		cSciTxRdy										*
									cSciTxBusy										*
*Description:	Get the sci trasmit status											*
*************************************************************************************/	
INT8U	sSciGetTxStatus(INT8U sciid)
{
	SciDrvStruct		*psci;
	
	psci=pSciIndex[sciid];
	
	return(psci->bTxStatus);
}		


/************************************************************************************
*Get Sci Rx Data																	*
*************************************************************************************/
INT8U	sbSciGetRxData(INT8U sciid)
{
	INT16U wTemp;
	
#if	SCI_SCI1_EN==1
	if(sciid==cUSART1_ID)
	{
		wTemp = USART_RxData(USART1);
		return (INT8U)wTemp;
	}
#endif

#if	SCI_SCI2_EN==1
	if(sciid==cUSART2_ID)
	{
		wTemp = USART_RxData(USART2);
		return (INT8U)wTemp;
	}
#endif

#if	SCI_SCI3_EN==1
	if(sciid==cUSART3_ID)
	{
		wTemp = USART_RxData(USART3);
		return (INT8U)wTemp;
	}
#endif

#if	SCI_SCI4_EN==1
	if(sciid==cUSART4_ID)
	{
		wTemp = USART_RxData(UART4);
		return (INT8U)wTemp;
	}
#endif

#if	SCI_SCI5_EN==1
	if(sciid==cUSART5_ID)
	{
		wTemp = USART_RxData(UART5);
		return (INT8U)wTemp;
	}
#endif
	
	return 0;
}

/************************************************************************************
*Get Sci Rx ready status															*
*************************************************************************************/
INT8U	sbSciGetRxRdy(INT8U sciid)
{
#if	SCI_SCI1_EN==1
	if(sciid==cUSART1_ID)
	{
		if(USART_ReadIntFlag(USART1, USART_INT_RXBNE))
		{
			return(cSciRxBusy);
		}
		else 
		{
			return(cSciRxRdy);
		}
	}
#endif

#if	SCI_SCI2_EN==1
	if(sciid==cUSART2_ID)
	{
		if(USART_ReadIntFlag(USART2, USART_INT_RXBNE))
		{
			return(cSciRxBusy);
		}
		else 
		{
			return(cSciRxRdy);
		}
	}
#endif

#if	SCI_SCI3_EN==1
	if(sciid==cUSART3_ID)
	{
		if(USART_ReadIntFlag(USART3, USART_INT_RXBNE))
		{
			return(cSciRxBusy);
		}
		else 
		{
			return(cSciRxRdy);
		}
	}
#endif

#if	SCI_SCI4_EN==1
	if(sciid==cUSART4_ID)
	{
		if(USART_ReadIntFlag(UART4, USART_INT_RXBNE))
		{
			return(cSciRxBusy);
		}
		else 
		{
			return(cSciRxRdy);
		}
	}
#endif

#if	SCI_SCI5_EN==1
	if(sciid==cUSART5_ID)
	{
		if(USART_ReadIntFlag(UART5, USART_INT_RXBNE))
		{
			return(cSciRxBusy);
		}
		else 
		{
			return(cSciRxRdy);
		}
	}
#endif
	
	return (cSciRxBusy);
}
/************************************************************************************
*Get Sci Tx status																	*
*************************************************************************************/
INT8U	sbSciGetTxRdy(INT8U sciid)
{
#if	SCI_SCI1_EN==1
	if(sciid==cUSART1_ID)
	{
		if(USART_ReadIntFlag(USART1, USART_INT_TXBE))
		{
			return(cSciTxRdy);
		}
		else 
		{
			return(cSciTxBusy);
		}
	}
#endif

#if	SCI_SCI2_EN==1
	if(sciid==cUSART2_ID)
	{
		if(USART_ReadIntFlag(USART2, USART_INT_TXBE))
		{
			return(cSciTxRdy);
		}
		else 
		{
			return(cSciTxBusy);
		}
	}
#endif

#if	SCI_SCI3_EN==1
	if(sciid==cUSART3_ID)
	{
		if(USART_ReadIntFlag(USART3, USART_INT_TXBE))
		{
			return(cSciTxRdy);
		}
		else 
		{
			return(cSciTxBusy);
		}
	}
#endif

#if	SCI_SCI4_EN==1
	if(sciid==cUSART4_ID)
	{
		if(USART_ReadIntFlag(UART4, USART_INT_TXBE))
		{
			return(cSciTxRdy);
		}
		else 
		{
			return(cSciTxBusy);
		}
	}
#endif

#if	SCI_SCI5_EN==1
	if(sciid==cUSART5_ID)
	{
		if(USART_ReadIntFlag(UART5, USART_INT_TXBE))
		{
			return(cSciTxRdy);
		}
		else 
		{
			return(cSciTxBusy);
		}
	}
#endif

	return(cSciTxBusy);
}

/************************************************************************************
*tx a byte and enable tx interrupt													*
*************************************************************************************/
void	sSciTxData(INT8U sciid,INT8U data)
{
#if	SCI_SCI1_EN==1
	if(sciid==cUSART1_ID)
	{
		USART_TxData(USART1, data);
		return;
	}
#endif

#if	SCI_SCI2_EN==1
	if(sciid==cUSART2_ID)
	{
		USART_TxData(USART2, data);
		return;
	}
#endif

#if	SCI_SCI3_EN==1
	if(sciid==cUSART3_ID)
	{
		USART_TxData(USART3, data);
		return;
	}
#endif

#if	SCI_SCI4_EN==1
	if(sciid==cUSART4_ID)
	{
		USART_TxData(UART4, data);
		return;
	}
#endif

#if	SCI_SCI5_EN==1
	if(sciid==cUSART5_ID)
	{
		USART_TxData(UART5, data);
		return;
	}
#endif

}

/************************************************************************************
*disable tx interrupt																*
*************************************************************************************/			
void	sSciStopTx(INT8U sciid)
{
#if	SCI_SCI1_EN==1
	if(sciid==cUSART1_ID)
	{
		USART_DisableInterrupt(USART1, USART_INT_TXBE);
		return;
	}
#endif

#if	SCI_SCI2_EN==1
	if(sciid==cUSART2_ID)
	{
		USART_DisableInterrupt(USART2, USART_INT_TXBE);
		return;
	}
#endif

#if	SCI_SCI3_EN==1
	if(sciid==cUSART3_ID)
	{
		USART_DisableInterrupt(USART3, USART_INT_TXBE);
		return;
	}
#endif

#if	SCI_SCI4_EN==1
	if(sciid==cUSART4_ID)
	{
		USART_DisableInterrupt(UART4, USART_INT_TXBE);
		return;
	}
#endif

#if	SCI_SCI5_EN==1
	if(sciid==cUSART5_ID)
	{
		USART_DisableInterrupt(UART5, USART_INT_TXBE);
		return;
	}
#endif

}

/************************************************************************************
*Enable tx interrupt																*
*************************************************************************************/			
void	sSciStartTx(INT8U sciid)
{
#if	SCI_SCI1_EN==1
	if(sciid==cUSART1_ID)
	{
		USART_EnableInterrupt(USART1, USART_INT_TXBE);
		return;
	}
#endif

#if	SCI_SCI2_EN==1
	if(sciid==cUSART2_ID)
	{
		USART_EnableInterrupt(USART2, USART_INT_TXBE);
		return;
	}
#endif

#if	SCI_SCI3_EN==1
	if(sciid==cUSART3_ID)
	{
		USART_EnableInterrupt(USART3, USART_INT_TXBE);
		return;
	}
#endif

#if	SCI_SCI4_EN==1
	if(sciid==cUSART4_ID)
	{
		USART_EnableInterrupt(UART4, USART_INT_TXBE);
		return;
	}
#endif

#if	SCI_SCI5_EN==1
	if(sciid==cUSART5_ID)
	{
		USART_EnableInterrupt(UART5, USART_INT_TXBE);
		return;
	}
#endif

}

/************************************************************************************
*Disable rx interrupt																*
*************************************************************************************/
void	sSciStopRx(INT8U sciid)
{
#if	SCI_SCI1_EN==1
	if(sciid==cUSART1_ID)
	{
		USART_DisableInterrupt(USART1, USART_INT_RXBNE);
		return;
	}
#endif

#if	SCI_SCI2_EN==1
	if(sciid==cUSART2_ID)
	{
		USART_DisableInterrupt(USART2, USART_INT_RXBNE);
		return;
	}
#endif

#if	SCI_SCI3_EN==1
	if(sciid==cUSART3_ID)
	{
		USART_DisableInterrupt(USART3, USART_INT_RXBNE);
		return;
	}
#endif

#if	SCI_SCI4_EN==1
	if(sciid==cUSART4_ID)
	{
		USART_DisableInterrupt(UART4, USART_INT_RXBNE);
		return;
	}
#endif

#if	SCI_SCI5_EN==1
	if(sciid==cUSART5_ID)
	{
		USART_DisableInterrupt(UART5, USART_INT_RXBNE);
		return;
	}
#endif

}

/************************************************************************************
*Enable rx interrupt																*
*************************************************************************************/
void	sSciStartRx(INT8U sciid)
{
#if	SCI_SCI1_EN==1
	if(sciid==cUSART1_ID)
	{
		USART_EnableInterrupt(USART1, USART_INT_RXBNE);
		return;
	}
#endif

#if	SCI_SCI2_EN==1
	if(sciid==cUSART2_ID)
	{
		USART_EnableInterrupt(USART2, USART_INT_RXBNE);
		return;
	}
#endif

#if	SCI_SCI3_EN==1
	if(sciid==cUSART3_ID)
	{
		USART_EnableInterrupt(USART3, USART_INT_RXBNE);
		return;
	}
#endif

#if	SCI_SCI4_EN==1
	if(sciid==cUSART4_ID)
	{
		USART_EnableInterrupt(UART4, USART_INT_RXBNE);
		return;
	}
#endif

#if	SCI_SCI5_EN==1
	if(sciid==cUSART5_ID)
	{
		USART_EnableInterrupt(UART5, USART_INT_RXBNE);
		return;
	}
#endif

}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
INT8U	bGetSciRxLen(INT8U sciid)
{
	return strSciDataList[sciid].bRxLen;
}

INT8U	*pbGetSciRxBuff(INT8U sciid)
{
	return strSciDataList[sciid].bRxBuf;
}

INT8U	*pbGetSciTxBuff(INT8U sciid)
{
	return strSciDataList[sciid].bTxBuf;
}

/********************************************************************************
* Input interface Routines														*
********************************************************************************/


