/********************************************************************************
Right Reserve:	Guangzhou Felicity Solar Technology Co., Ltd
Project:		LPBT
File Name:		AntiThief.c
Author:			Andy
Date:			2019.08.15
Description:	None
********************************************************************************/
#define		__ANTI_THIEF_C__

/********************************************************************************
* Include head files                                                            *
********************************************************************************/
#include "Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define		ZI_JIE_SHU			(4)			// STM32/GD32保持为4，不要修改


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
static	INT8U	D[12];

INT16U *sAdcSimulatNum = NULL;
INT32U UIDCrc32NumA;
INT32U UIDCrc32NumB;
INT32U UIDCrc32NumC;
INT8U  UIDCount;



/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
 void sGetUIDCrc32NumA(void);   
 void sGetUIDCrc32NumB(void);  


/********************************************************************************
* Routines' implementations														*
********************************************************************************/

void	AntiThiefIntal(void)
{
	UIDCrc32NumA = 0;
	UIDCrc32NumB = 0;
	UIDCrc32NumC = 0;
	UIDCount = 0;
	sGetCrc32Table();  			   
//	sGetUIDCrc32NumC();  			
}

void	isPirated(void)
{
	INT8U* pUid;
	
	//指向芯片的UID区首地址，用来取芯片的UID值
	pUid=(INT8U*)UID_BASE_ADDRESS;
	
	//对D数组按配置赋值
	D[0]=pUid[D0_ID_INDEX];
	D[1]=pUid[D1_ID_INDEX];
	D[2]=pUid[D2_ID_INDEX];
	D[3]=pUid[D3_ID_INDEX];
	D[4]=pUid[D4_ID_INDEX];
	D[5]=pUid[D5_ID_INDEX];
	D[6]=pUid[D6_ID_INDEX];
	D[7]=pUid[D7_ID_INDEX];
	D[8]=pUid[D8_ID_INDEX];
	D[9]=pUid[D9_ID_INDEX];
	D[10]=pUid[D10_ID_INDEX];
	D[11]=pUid[D11_ID_INDEX];
	
	//
	sGetUIDCrc32NumC();  	
}


void sGetUIDCrc32NumA(void)   
{
	// 计算UID的CRC32校验值，存储到UIDCrc32NumA
    UIDCrc32NumA = sxCrc32Check(D,12,0);
}

void sGetUIDCrc32NumB(void)   	
{	
	// 获取ADC模拟值，用于计算CRC32
	sAdcSimulatNum = swGetAdcSimulationValue();
	// 计算ADC模拟值的CRC32校验值，存储到UIDCrc32NumB
	UIDCrc32NumB = sxCrc32Check((INT8U*) sAdcSimulatNum,sizeof(sAdcSimulatNum),0);
}

//CRC32校验码计算
void sGetUIDCrc32NumC(void)   		
{	
	sGetUIDCrc32NumA();
	sGetUIDCrc32NumB();
	if(UIDCount == 0)
	{
		UIDCrc32NumC = UIDCrc32NumB ^ UIDCrc32NumA;
		if(UIDCrc32NumC <= 0)
		{
			UIDCrc32NumC = UIDCrc32NumC+1;
		}
		else if(UIDCrc32NumC > 0xFFFFFFF)
		{
			UIDCrc32NumC = UIDCrc32NumC-1;
		}
		
	    UIDCount = 1;
	}
}


/********************************************************************************
* Output interface Routines														*
********************************************************************************/
INT32U sGetUIDCrc32NumAValue(void)
{
  return UIDCrc32NumA;
}


INT32U sGetUIDCrc32NumBValue(void)
{
  return UIDCrc32NumB;
}

INT32U sGetUIDCrc32NumCValue(void)
{
  return UIDCrc32NumC;
}




/********************************************************************************
* Input interface Routines														*
********************************************************************************/




/************************************
STM32 GD32 UID算法加密代码DEMO。
www.xwopen.com
xuanweikeji.taobao.com
版权所有。
*************************************/




/*****************下面的不要修改！！！***********************/





