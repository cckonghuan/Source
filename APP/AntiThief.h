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

// �Ƿ�ʹ�ܼ���,1=ʹ�ܼ���,0=��ֹ����
#define cENCRYPT_EN				(1)

// �޸�Ϊ��Ҫʹ�õĹ�ʽ�ţ���ֵҪ��<�������>��һ��
#define cENCRYPT_NO				(110)

// �޸�Ϊ��Ҫʹ�õĳ���ֵ����ֵҪ��<�������>��һ��
#define cENCRYPT_MASK			(0xFFB9B3AC)

// �޸�Ϊ�����õĽ��ֵ�Ĵ�ŵ�ַ����ֵҪ��<�������>��һ��,������4�ı�����ע�������ַҪ���������Ϻã���Ҫ��д��
#define cENCRYPT_RESULT_ADDR	(FLASH_IAP_ENCRYPT_ADDR)

// �޸�Ϊ��Ŀ��оƬ��UID���ڵĵ�ַ��ע�ⲻͬоƬ�ͺŲ�һ�������Ҫ���ֲ�ȷ��
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

// �����õ�˳���޸����UID˳���б���ֵҪ��<�������>��һ��
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

