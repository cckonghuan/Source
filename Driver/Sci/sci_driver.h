/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		sci_driver.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifdef	SCI_DRIVER
#define	SCI_DRV
#else
#define	SCI_DRV	extern
#endif

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"sci_config.h"
#include	"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
// return value of write operation
#define	cSciTxRdy				0
#define	cSciTxBusy				1

#define cSCI_RX_LEN				255
#define cSCI_TX_LEN				255

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
SCI_DRV	void	sSciInitial(INT8U sciid, INT8U rx_size, INT8U prio, INT8U event);							
SCI_DRV	void	sSciRxTask(INT8U sciid, INT8U bTimeout);
SCI_DRV	void	sSciRxISR(INT8U sciid);
SCI_DRV	void	sSciTxISR(INT8U sciid);
SCI_DRV	INT8U	sSciGetTxStatus(INT8U sciid);
SCI_DRV	INT8U	sSciWrite(INT8U sciid,INT8U *pstart,INT8U Length);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
SCI_DRV	INT8U	bGetSciRxLen(INT8U sciid);
SCI_DRV	INT8U	*pbGetSciRxBuff(INT8U sciid);
SCI_DRV	INT8U	*pbGetSciTxBuff(INT8U sciid);

/********************************************************************************
* Input interface Routines														*
********************************************************************************/


