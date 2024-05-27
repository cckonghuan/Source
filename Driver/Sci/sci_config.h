/************************************************************************************
*File name:	SciConfig.h																*
*Description:	the sci driver settings												*
*************************************************************************************/
#define 	cHOST_SCI			cUSART1_ID

#define		cUSART1_ID			0

/************************************************************************************
*SCI ENABLE/DISANLE SWITCH															*
*0:			SCI DISABLE															*
*1:			SCI ENABLE																*
*************************************************************************************/
#define		SCI_SCI1_EN			1
#define		SCI_SCI2_EN			0
#define		SCI_SCI3_EN			0
#define		SCI_SCI4_EN			0
#define		SCI_SCI5_EN			0

/************************************************************************************
*Maxmam	Sci Number																	*
*************************************************************************************/
#define		MAX_SCI_NO			(SCI_SCI1_EN+SCI_SCI2_EN+SCI_SCI3_EN+SCI_SCI4_EN+SCI_SCI5_EN)

/************************************************************************************
*Maxmam	Sci buffer size																*
*************************************************************************************/
#if(SCI_SCI1_EN==1)
#define		MAX_SCI1_RX_SIZE	32
#else
#define		MAX_SCI1_RX_SIZE	0
#endif
#if(SCI_SCI2_EN==1)
#define		MAX_SCI2_RX_SIZE	32
#else
#define		MAX_SCI2_RX_SIZE	0
#endif
#if(SCI_SCI3_EN==1)
#define		MAX_SCI3_RX_SIZE	32
#else
#define		MAX_SCI3_RX_SIZE	0
#endif
#if(SCI_SCI4_EN==1)
#define		MAX_SCI4_RX_SIZE	32
#else
#define		MAX_SCI4_RX_SIZE	0
#endif
#if(SCI_SCI5_EN==1)
#define		MAX_SCI5_RX_SIZE	32
#else
#define		MAX_SCI5_RX_SIZE	0
#endif
#define		MAX_SCI_RX_SIZE	MAX_SCI1_RX_SIZE+MAX_SCI2_RX_SIZE+MAX_SCI3_RX_SIZE+MAX_SCI4_RX_SIZE+MAX_SCI5_RX_SIZE

