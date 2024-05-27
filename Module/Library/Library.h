/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		Library.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __LIBRARY_H__
#define __LIBRARY_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"
#include    	"stdlib.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef	__LIBRARY_C__
#define	LIBRARY
#else
#define	LIBRARY	extern
#endif

typedef		void (*pFunction)(void);

#define     cBack                               2
#define     cRunning                            3
	
#define     cDisable                            0
#define     cEnable                             1
#define     cMuted                              2

// For BmsStatus
#define     cPowerOnMode                        0
#define     cStandbyMode                        1
#define     cBatteryMode                        2
#define     cDischargeMode                      3
#define     cChargeMode                         4
#define     cSleepMode                          5
#define     cFaultMode                          6
#define     cShutdownMode                       7
#define     cTestMode                           8
#define     cUpgradeMode                        9

// For Ascii To Number
#define     c8UDataErr                          0xFF
#define     c16UDataErr                         0xFFFF
#define     c32UDataErr                         0xFFFFFFFF

//#define		BIT(n)				(1<<(n))
#define		LSHFT(x,n)			((x)<<(n))
#define		RSHFT(x,n)			((x)>>(n))

#define		CHKBIT(x,n)			(((x)&(1<<(n)))!=0)
#define		CHKNBIT(x,n)		(((x)&(1<<(n)))==0)
#define		CHKBITs(x,msk)		(((x)&(msk))==(msk))
#define		CHKNBITs(x,msk)		(((x)&(msk))==(0))

#define		SETBIT(x,n)			x|=(1<<(n))
#define		CLRBIT(x,n)			x&=(~(1<<(n)))
#define		SETBITs(x,msk)		x|=(msk)
#define		CLRBITs(x,msk)		x&=(~(msk))

#define		CHKBITMIN(x,n)		((x&((1<<(n))-1))==0)	// 查找n是否是最小位
#define		CHKBITMINNOT(x,n)	((x&((1<<(n))-1))!=0)	// 查找n是否不是最小位
#define		MAX(a,b)			((a) < (b) ? (b) : (a))
#define		MIN(a,b)			((a) < (b) ? (a) : (b))
#define		EQU(a,b)			((a) == (b) ? (1) : (0))
#define		NEQ(a,b)			((a) != (b) ? (1) : (0))
#define		LSS(a,b)			((a) < (b) ? (1) : (0))
#define		LEQ(a,b)			((a) <= (b) ? (1) : (0))
#define		GTR(a,b)			((a) > (b) ? (1) : (0))
#define		GEQ(a,b)			((a) >= (b) ? (1) : (0))

#define		HIGH8(x)			((( struct {INT8U l; INT8U h; } *)(&x))->h)
#define		LOW8(x)				((( struct {INT8U l; INT8U h; } *)(&x))->l)

#define		UNUSED(X)			(void)X	// To avoid gcc/g++ warnings

/********************************************************************************
* Global variable declaration													*
********************************************************************************/
LIBRARY INT8U	bBmsMode;

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
LIBRARY INT8U	sbGetBmsMode(void);

LIBRARY INT16U  swRoot(INT32U dwNumber);

LIBRARY INT8U   sb8UOutRangeChk(INT16S wCompareData,INT16S wHighRange,INT16S wLowRange,INT8U bFilter,INT8U *pbCounter);
LIBRARY INT8U   sb8UInRangeChk(INT16S wCompareData,INT16S wHighRange,INT16S wLowRange,INT8U bFilter,INT8U *pbCounter);
LIBRARY INT8U   sb16UOutRangeChk(INT16S wCompareData,INT16S wHighRange,INT16S wLowRange,INT16U wFilter,INT16U *pwCounter);
LIBRARY INT8U   sb16UInRangeChk(INT16S wCompareData,INT16S wHighRange,INT16S wLowRange,INT16U wFilter,INT16U *pwCounter);

LIBRARY INT8U   sb8UOverLevelChk(INT16S wCompareData,INT16S wHighLevel,INT8U bFilter,INT8U *pbCounter);
LIBRARY INT8U   sb8UUnderLevelChk(INT16S wCompareData,INT16S wLowLevel,INT8U bFilter,INT8U *pbCounter);
LIBRARY INT8U   sb16UOverLevelChk(INT16S wCompareData,INT16S wHighLevel,INT16U wFilter,INT16U *pwCounter);
LIBRARY INT8U   sb16UUnderLevelChk(INT16S wCompareData,INT16S wLowLevel,INT16U wFilter,INT16U *pwCounter);

LIBRARY INT8U   sb8UCountDown(INT8U *pbCounter);
LIBRARY INT8U   sb16UCountDown(INT16U *pwCounter);
LIBRARY INT8U   sb32UCountDown(INT32U *pdwCounter);

LIBRARY INT8U   sbNumToX(INT16U wNumber,INT8U *pbBuffer);
LIBRARY INT8U   sbNumToXx(INT16U wNumber,INT8U *pbBuffer);
LIBRARY INT8U   sbNumToXxx(INT16U wNumber,INT8U *pbBuffer);
LIBRARY INT8U   sbNumToXxxx(INT16U wNumber,INT8U *pbBuffer);
LIBRARY INT8U   sbNumToXxxxx(INT32U dwNumber,INT8U *pbBuffer);

LIBRARY INT8U   sbNumToXX(INT16U wNumber,INT8U *pbBuffer);
LIBRARY INT8U   sbNumToXX_16Hex(INT16U wNumber,INT8U *pbBuffer);
LIBRARY INT8U   sbNumToXXx(INT16U wNumber,INT8U *pbBuffer);
LIBRARY INT8U   sbNumToXXxx(INT16U wNumber,INT8U *pbBuffer);
LIBRARY INT8U   sbNumToXXxxx(INT32U dwNumber,INT8U *pbBuffer);

LIBRARY INT8U   sbNumToXXX(INT16U wNumber,INT8U *pbBuffer);
LIBRARY INT8U   sbNumToXXXx(INT16U wNumber,INT8U *pbBuffer);
LIBRARY INT8U   sbNumToXXXxx(INT32U dwNumber,INT8U *pbBuffer);

LIBRARY INT8U   sbNumToXXXX(INT16U wNumber,INT8U *pbBuffer);
LIBRARY INT8U   sbNumToXXXX_16Hex(INT16U wNumber,INT8U *pbBuffer);
LIBRARY INT8U   sbNumToXXXXx(INT32U dwNumber,INT8U *pbBuffer);

LIBRARY INT8U   sbNumToXXXXX(INT32U dwNumber,INT8U *pbBuffer);

LIBRARY void    sNumToAscii(INT16U wNumber,INT8U *pbBuffer);
LIBRARY void    sNumToAscii_16Hex(INT16U wNumber,INT8U *pbBuffer);

LIBRARY INT8U   sbAsciiToX(const INT8U *pbBuffer);
LIBRARY INT8U   sbAsciiToXX(const INT8U *pbBuffer);
LIBRARY INT16U  swAsciiToXXX(const INT8U *pbBuffer);
LIBRARY INT16U  swAsciiToXXXX(const INT8U *pbBuffer);
LIBRARY INT32U  sdwAsciiToXXXXX(const INT8U *pbBuffer);

LIBRARY INT32U  sdwAsciiToNum(const INT8U *pbBuffer);
LIBRARY INT32U  sdwAsciiDotxToNum(const INT8U *pbBuffer);

LIBRARY INT8U	sbAbs(INT8S bData);
LIBRARY INT16U	swAbs(INT16S wData);

LIBRARY INT16U	swHighLowByteSwap(INT16U wData);
LIBRARY INT8U	sbHighBitNum8(INT8U value);
LIBRARY INT8U	sbHighBitNum16(INT16U value);
LIBRARY INT8U	sbHighBitNum32(INT32U value);
LIBRARY INT8U	sbFirstBitHighNo8(INT8U value);
LIBRARY INT8U	sbFirstBitHighNo16(INT16U value);
LIBRARY INT8U	sbFirstBitHighNo32(INT32U value);
LIBRARY void	sMemorySet(INT8U *pt, INT8U setval, INT8U length);
LIBRARY void	sMemoryCopy(INT8U *source, INT8U *target, INT8U length);
LIBRARY INT16U	swHighLowSwap(INT16U *pData);
LIBRARY void	sInvertUint8(INT8U *dBuf, INT8U *srcBuf);
LIBRARY void	sInvertUint16(INT16U *dBuf, INT16U *srcBuf);
LIBRARY INT16U	swModbusCrc16(INT8U *puchMsg, INT8U ucDataLen);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
LIBRARY void sGetCrc32Table(void); 
LIBRARY INT32U sxCrc32Check(INT8U *datptr , INT8U len , INT32U InitValue);


/********************************************************************************
* Input interface Routines														*
********************************************************************************/

#endif	// __LIBRARY_H__

