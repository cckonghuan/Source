/********************************************************************************
Right Reserve:	Guangzhou Felicity Solar Technology Co., Ltd
Project:		LPBT
File Name:		AntiThief.h
Author:			Andy
Date:			2019.08.15
Description:	None
********************************************************************************/
#ifndef __ANTI_THIEF_H__
#define __ANTI_THIEF_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef  __ANTI_THIEF_C__
#define ANTI_THIEF
#else
#define ANTI_THIEF	extern
#endif

// 是否使能加密,1=使能加密,0=禁止加密
#define cENCRYPT_EN				(1)

// 修改为你要使用的公式号，此值要与<软件配置>的一致
#define cENCRYPT_NO				(110)

// 修改为你要使用的常数值，此值要与<软件配置>的一致
#define cENCRYPT_MASK			(0xFFB9B3AC)

// 修改为你配置的结果值的存放地址。此值要与<软件配置>的一致,必须是4的倍数。注意这个地址要与你程序配合好，不要乱写。
#define cENCRYPT_RESULT_ADDR	(FLASH_IAP_ENCRYPT_ADDR)

// 修改为你目标芯片的UID所在的地址。注意不同芯片型号不一样。这个要查手册确定
//	Device Line					Starting address
//	F0/F3						0x1FFFF7AC
//	F1							0x1FFFF7E8
//	F2/F4						0x1FFF7A10
//	F7							0x1FF0F420
//	L0							0x1FF80050
//	L0/L1 Cat.1/Cat.2			0x1FF80050
//	L1 Cat.3/Cat.4/Cat.5/Cat.6	0x1FF800D0
//  GD32F30X					0x1FFFF7E8
#define UID_BASE_ADDRESS		(0x1FFFF7E8)

// 据配置的顺序修改这个UID顺序列表，此值要与<软件配置>的一致
#define D0_ID_INDEX     0X4  
#define D1_ID_INDEX     0XB
#define D2_ID_INDEX     0X5
#define D3_ID_INDEX     0X0
#define D4_ID_INDEX     0X4
#define D5_ID_INDEX     0X2
#define D6_ID_INDEX     0X5
#define D7_ID_INDEX     0X4
#define D8_ID_INDEX     0X3
#define D9_ID_INDEX     0X6
#define D10_ID_INDEX    0X3
#define D11_ID_INDEX    0X4

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
ANTI_THIEF void	AntiThiefIntal(void);
ANTI_THIEF void	isPirated(void);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/

ANTI_THIEF void sGetUIDCrc32NumA(void);   
ANTI_THIEF void sGetUIDCrc32NumB(void); 
ANTI_THIEF void sGetUIDCrc32NumC(void); 



ANTI_THIEF INT32U sGetUIDCrc32NumAValue(void); 
ANTI_THIEF INT32U sGetUIDCrc32NumBValue(void); 
ANTI_THIEF INT32U sGetUIDCrc32NumCValue(void); 



/********************************************************************************
* Input interface Routines														*
********************************************************************************/

#endif	// __ANTI_THIEF_H__

