/************************************************************************************
*File name:	CanConfig.h																*
*Description:	the can driver settings												*
*************************************************************************************/
#define 	cBMS_CAN			cCAN1_ID
#define 	cPCS_CAN			cCAN2_ID

#define		cCAN1_ID			0
#define		cCAN2_ID			1

/************************************************************************************
*SCI ENABLE/DISANLE SWITCH															*
*0:			SCI DISABLE															*
*1:			SCI ENABLE																*
*************************************************************************************/
#define		CAN_CAN1_EN			1
#define		CAN_CAN2_EN			1

/************************************************************************************
*Maxmam	Sci Number																	*
*************************************************************************************/
#define		MAX_CAN_NO			(CAN_CAN1_EN+CAN_CAN2_EN)

/************************************************************************************
*Maxmam	Sci buffer size																*
*************************************************************************************/
#if(CAN_CAN1_EN==1)
#define		MAX_CAN1_RX_SIZE	32
#else
#define		MAX_CAN1_RX_SIZE	0
#endif
#if(CAN_CAN2_EN==1)
#define		MAX_CAN2_RX_SIZE	32
#else
#define		MAX_CAN2_RX_SIZE	0
#endif
#define		MAX_CAN_RX_SIZE		MAX_CAN1_RX_SIZE+MAX_CAN2_RX_SIZE

