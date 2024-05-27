/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		sci_driver.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef	__CAN_DRIVER_H__
#define	__CAN_DRIVER_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"OS_CPU.h"
#include	"Library.h"
#include	"can_config.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef	__CAN_DRIVER_C__
#define	CAN_DRIVER
#else
#define	CAN_DRIVER	extern
#endif

#define	CAN_QUE_RX_Nframe		128		// CAN接收队列帧数目,以结构体T_CanFrame为单元来计算
#define	CAN_QUE_TX_Nframe		16		// CAN发送队列帧数目,以结构体T_CanFrame为单元来计算

#ifndef	NOT_OK
#define	NOT_OK					0xff	// 参数错误
#endif

#define	ERR_NONE				0x0000

#define	QUEUE_FULL				8		// 队列满
#define	QUEUE_EMPTY				4		// 无数据
#define	QUEUE_OK				0		// 操作成功

#ifndef DBG_CAN_QUE_MAX
#define DBG_CAN_QUE_MAX				1
#endif
#ifndef CAN_QUEUE_READ_EMPTY_EN
#define CAN_QUEUE_READ_EMPTY_EN		0
#endif
#ifndef CAN_QUEUE_WRITE_FULL_EN
#define CAN_QUEUE_WRITE_FULL_EN		1
#endif

/***************************************************************************************************/
typedef enum {CAN_TxRx_Tx, CAN_TxRx_Rx} CanTxRx;
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

typedef struct
{
  volatile INT32U TIR;
  volatile INT32U TDTR;
  volatile INT32U TDLR;
  volatile INT32U TDHR;
} CAN_TxMailBox_TypeDef;

typedef struct
{
  volatile INT32U RIR;
  volatile INT32U RDTR;
  volatile INT32U RDLR;
  volatile INT32U RDHR;
} CAN_FIFOMailBox_TypeDef;

typedef struct
{
  volatile INT32U FR1;
  volatile INT32U FR2;
} CAN_FilterRegister_TypeDef;

typedef struct
{
	volatile INT32U MCR;
	volatile INT32U MSR;
	volatile INT32U TSR;
	volatile INT32U RF0R;
	volatile INT32U RF1R;
	volatile INT32U IER;
	volatile INT32U ESR;
	volatile INT32U BTR;
	INT32U  RESERVED0[88];
	CAN_TxMailBox_TypeDef sTxMailBox[3];
	CAN_FIFOMailBox_TypeDef sFIFOMailBox[2];
	INT32U  RESERVED1[12];
	volatile INT32U FMR;
	volatile INT32U FM1R;
	INT32U  RESERVED2;
	volatile INT32U FS1R;
	INT32U  RESERVED3;
	volatile INT32U FFA1R;
	INT32U  RESERVED4;
	volatile INT32U FA1R;
	INT32U  RESERVED5[8];
	CAN_FilterRegister_TypeDef sFilterRegister[28];
}CAN_TypeDef;

typedef struct
{
	INT32U StdId;  /*!< Specifies the standard identifier.
	                This parameter can be a value between 0 to 0x7FF. */

	INT32U ExtId;  /*!< Specifies the extended identifier.
	                This parameter can be a value between 0 to 0x1FFFFFFF. */

	INT8U IDE;     /*!< Specifies the type of identifier for the message that 
	                will be transmitted. This parameter can be a value 
	                of @ref CAN_identifier_type */

	INT8U RTR;     /*!< Specifies the type of frame for the message that will 
	                be transmitted. This parameter can be a value of 
	                @ref CAN_remote_transmission_request */

	INT8U DLC;     /*!< Specifies the length of the frame that will be 
	                transmitted. This parameter can be a value between 
	                0 to 8 */

	INT8U Data[8]; /*!< Contains the data to be transmitted. It ranges from 0 
	                to 0xFF. */
} CanTxMsg;

typedef union
{
	INT8U 	u08[8];		// 定义字节型以满足需求
	INT16U	u16[4];		
	INT32U	u32[2];		// 定义寄存器类型以提高读取效率,32位0x12345678换成8位:0x78,0x56,0x34,0x12
}T_CanData;

// 因为发送邮箱与接收邮箱数据结构基本相同，所以队列数据可直接使用
typedef struct
{
	INT32U	IxR;		// 与TIxR/RIxR对应
	INT32U 	DTxR;		// 与TDTxR/RDTxR对应
	T_CanData 	Data;	// TLxR:THxR/RLxR:RHxR, 八字节数据
}T_CanFrame;

typedef struct
{
	// 经测试将In, Out, 定义为索引方式比定义为指针执行效率低
	T_CanFrame		*Out;					/* 指向数据输出位置 		*/
	T_CanFrame 		*In;					/* 指向数据输入位置 		*/
	T_CanFrame 		*End;					/* 指向Buf的结束位置		*/
	INT16U			NData;					/* 队列中数据个数			*/
	INT16U			MaxData;				/* 队列中允许存储的数据个数 */
#if (CAN_QUEUE_READ_EMPTY_EN > 0)
	INT8U			(* ReadEmpty)(void);	/* 读空处理函数 			*/
#endif
#if (CAN_QUEUE_WRITE_FULL_EN > 0)
	INT8U			(* WriteFull)(INT8U,INT32U);	/* 写满处理函数 			*/
	INT32U			WriteFullParam;
#endif
	T_CanFrame 		*Buf; 					/* 存储数据的空间			*/
}T_CanQueue;

// STM32 CAN寄存器相关定义
#define CAN_TSR_TME_BITnL			26						/* CAN发送 */
#define CAN_TIxR_IDE				((INT32U)BIT(2))		/* 0:标准标识符, 1:扩展标识符 */
#define CAN_TIxR_RTR				((INT32U)BIT(1))		/* 0:数据帧, 1:远程帧 */
#define CAN_TIxR_TXRQ         		((INT32U)0x00000001)	/* CAN_TIxR */
#define CAN_TDTxR_DLC         		((INT32U)0x0000000F)	/* CAN_TDTxR  */
#define CAN_ESR_LEC_BITnL			4						/* CAN_ESC_LEC最低位号 */
#define CAN_ESR_LEC_USER			CAN_ESR_LEC				/* CAN_ESC_LEC位掩码 */
#define CAN_ESR_TEC_BITnL			16						/* CAN_ESR_TEC最低位号 */

#define CAN1_INT_TX_DISABLE()		CLRBITs(CAN1->INTEN, CAN_INT_TXME)	/* 关闭CAN发送中断 */
#define CAN1_INT_TX_ENABLE()		SETBITs(CAN1->INTEN, CAN_INT_TXME)	/* 开启CAN发送中断 */
#define CAN1_INT_RX0_DISABLE()		CLRBITs(CAN1->INTEN, CAN_INT_F0MP)	/* 关闭CAN接中断 */
#define CAN1_INT_RX0_ENABLE()		SETBITs(CAN1->INTEN, CAN_INT_F0MP)	/* 开启CAN接中断 */
#define CAN1_INT_RX1_DISABLE()		CLRBITs(CAN1->INTEN, CAN_INT_F1MP)	/* 关闭CAN接中断 */
#define CAN1_INT_RX1_ENABLE()		SETBITs(CAN1->INTEN, CAN_INT_F1MP)	/* 开启CAN接中断 */
#define CAN1_INT_RX_DISABLE()		CLRBITs(CAN1->INTEN, CAN_INT_F0MP | CAN_INT_F1MP)	/* 关闭CAN接中断 */
#define CAN1_INT_RX_ENABLE()		SETBITs(CAN1->INTEN, CAN_INT_F0MP | CAN_INT_F1MP)	/* 开启CAN接中断 */
#define CAN2_INT_TX_DISABLE()		CLRBITs(CAN2->INTEN, CAN_INT_TXME)	/* 关闭CAN发送中断 */
#define CAN2_INT_TX_ENABLE()		SETBITs(CAN2->INTEN, CAN_INT_TXME)	/* 开启CAN发送中断 */
#define CAN2_INT_RX0_DISABLE()		CLRBITs(CAN2->INTEN, CAN_INT_F0MP)	/* 关闭CAN接中断 */
#define CAN2_INT_RX0_ENABLE()		SETBITs(CAN2->INTEN, CAN_INT_F0MP)	/* 开启CAN接中断 */
#define CAN2_INT_RX1_DISABLE()		CLRBITs(CAN2->INTEN, CAN_INT_F1MP)	/* 关闭CAN接中断 */
#define CAN2_INT_RX1_ENABLE()		SETBITs(CAN2->INTEN, CAN_INT_F1MP)	/* 开启CAN接中断 */
#define CAN2_INT_RX_DISABLE()		CLRBITs(CAN2->INTEN, CAN_INT_F0MP | CAN_INT_F1MP)	/* 关闭CAN接中断 */
#define CAN2_INT_RX_ENABLE()		SETBITs(CAN2->INTEN, CAN_INT_F0MP | CAN_INT_F1MP)	/* 开启CAN接中断 */

#define CAN1_PERIPH					((CAN_TypeDef *) CAN1)
#define CAN2_PERIPH					((CAN_TypeDef *) CAN2)

//#define CAN_RF0R_FMP0				CAN_RFIFO0_RFL0
#define CAN_RF0R_FMP0				((uint8_t)0x03)               /*!< FIFO 0 Message Pending */
//#define CAN_RF0R_RFOM0				CAN_RFIFO0_RFD0
#define CAN_RF0R_RFOM0				((uint8_t)0x20)               /*!< Release FIFO 0 Output Mailbox */

//#define CAN_RF1R_FMP1				CAN_RFIFO1_RFL1
#define CAN_RF1R_FMP1				((uint8_t)0x03)               /*!< FIFO 1 Message Pending */

//#define CAN_RF1R_RFOM1				CAN_RFIFO1_RFD1
#define CAN_RF1R_RFOM1				((uint8_t)0x20)               /*!< Release FIFO 1 Output Mailbox */

//#define CAN_TSR_RQCP0				CAN_TSTAT_MTF0
#define CAN_TSR_RQCP0				((uint32_t)0x00000001)        /*!< Request Completed Mailbox0 */

//#define CAN_TSR_RQCP1				CAN_TSTAT_MTF1
#define CAN_TSR_RQCP1				((uint32_t)0x00000100)        /*!< Request Completed Mailbox1 */

//#define CAN_TSR_RQCP2				CAN_TSTAT_MTF2
#define CAN_TSR_RQCP2				((uint32_t)0x00010000)        /*!< Request Completed Mailbox2 */

//#define CAN_MSR_ERRI				CAN_STAT_ERRIF
#define CAN_MSR_ERRI				((uint16_t)0x0004)            /*!< Error Interrupt */

//#define CAN_ESR_EWGF				CAN_ERR_WERR
#define CAN_ESR_EWGF				((uint32_t)0x00000001)        /*!< Error Warning Flag */

//#define CAN_ESR_EPVF				CAN_ERR_PERR
#define CAN_ESR_EPVF				((uint32_t)0x00000002)        /*!< Error Passive Flag */

//#define CAN_ESR_BOFF				CAN_ERR_BOERR
#define CAN_ESR_BOFF				((uint32_t)0x00000004)        /*!< Bus-Off Flag */

//#define CAN_ESR_LEC					CAN_ERR_ERRN
#define CAN_ESR_LEC					((uint32_t)0x00000070)        /*!< LEC[2:0] bits (Last Error Code) */

//#define CAN_ESR_TEC					CAN_ERR_TECNT
#define CAN_ESR_TEC					((uint32_t)0x00FF0000)        /*!< Least significant byte of the 9-bit Transmit Error Counter */

//#define CAN_ESR_REC					CAN_ERR_RECNT
#define CAN_ESR_REC					((uint32_t)0xFF000000)        /*!< Receive Error Counter */

//#define CAN_FMR_FINIT				CAN_MODE_INITIALIZE
#define CAN_FMR_FINIT				((uint8_t)0x01)               /*!< Filter Init Mode */

//#define CAN_RTR_Data                CAN_FT_DATA
#define CAN_RTR_Data				((uint32_t)0x00000000)  /*!< Data frame */

//#define CAN_RTR_Remote              CAN_FT_REMOTE
#define CAN_RTR_Remote              ((uint32_t)0x00000002)  /*!< Remote frame */

//#define CAN_Id_Standard             CAN_FF_STANDARD
#define CAN_Id_Standard				((uint32_t)0x00000000)  /*!< Standard Id */

//#define CAN_Id_Extended             CAN_FF_EXTENDED
#define CAN_Id_Extended				((uint32_t)0x00000004)  /*!< Extended Id */

#define CAN_RTR_DATA                CAN_RTR_Data
#define CAN_RTR_REMOTE              CAN_RTR_Remote
#define CAN_ID_STD					CAN_Id_Standard           
#define CAN_ID_EXT					CAN_Id_Extended

#define CAN_FIFO0					((uint8_t)0x00U)             /*!< receive FIFO0 */
#define CAN_FIFO1					((uint8_t)0x01U)             /*!< receive FIFO1 */

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
CAN_DRIVER	void	CAN_Initial(INT8U canid);
CAN_DRIVER	INT8U	CAN_QueueWrite(INT8U canid, T_CanQueue *Queue, T_CanFrame *pData);
CAN_DRIVER	INT8U	CAN_QueueWriteIsr(INT8U canid, T_CanQueue *Queue, T_CanFrame *pData);
CAN_DRIVER	INT8U	CAN_QueueRead(INT8U canid, T_CanQueue *Queue, T_CanFrame *pData);
CAN_DRIVER	INT8U	CAN_QueueReadIsr(INT8U canid, T_CanFrame *pData, T_CanQueue *Queue);
CAN_DRIVER	INT8U	CAN_ReadFrame(INT8U canid, T_CanFrame *pData);
CAN_DRIVER	INT32U	CAN_SendFrame(INT8U canid, T_CanFrame *pCanFrame);
CAN_DRIVER	void	CAN_wrFilterExt32MaskMode (INT32U filter_number, 
						INT32U id, INT32U mask, INT32U fifo_number);
CAN_DRIVER	INT8U	CAN_QueueWriteFull(INT8U canid, INT32U dwTxRx);
CAN_DRIVER	void	CAN_Rx0ISR(INT8U canid);
CAN_DRIVER	void	CAN_Rx1ISR(INT8U canid);
CAN_DRIVER	void	CAN_TxISR(INT8U canid);
CAN_DRIVER	void	CAN_SceISR(INT8U canid);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/


/********************************************************************************
* Input interface Routines														*
********************************************************************************/


#endif // __CAN_DRIVER_H__
