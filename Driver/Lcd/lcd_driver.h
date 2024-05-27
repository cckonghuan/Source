/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		lcd_driver.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __LCD_DRIVER_H__
#define __LCD_DRIVER_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
//#include   		"Main.h"
#include		"OS_CPU.h"
#include		"lcd_config.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef	__LCD_DRIVER_C__
#define	LCD_DRIVER
#else
#define	LCD_DRIVER	extern
#endif

typedef union 
{
	INT8U bByte;
	struct
	{
		INT8U SetVal:1;
		INT8U Duty50:3;
		INT8U timeout:3;
		INT8U val:1;
	}BIT;
}VIEWBUF;

enum LCD_BL_MODE
{
	cLCD_BL_OFF = 0x80,
	cLCD_BL_1_16 = 0x88,
	cLCD_BL_2_16 = 0x89,
	cLCD_BL_4_16 = 0x8A,
	cLCD_BL_10_16 = 0x8B,
	cLCD_BL_11_16 = 0x8C,
	cLCD_BL_12_16 = 0x8D,
	cLCD_BL_13_16 = 0x8E,
	cLCD_BL_14_16 = 0x8F
};

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
LCD_DRIVER	void	sLcdHwInit(void); 
LCD_DRIVER	void	sLcdUpdate(void);
LCD_DRIVER	void	sLcdUpdateData(void);
LCD_DRIVER	void	sLcdVramAreaSet(void);
LCD_DRIVER	void	sLcdVramAreaClear(void);
LCD_DRIVER	void	sSendBitToVram(INT8U bitAddr, INT8U bVal, INT8U Time);
LCD_DRIVER	void	sBackLightOn(void);
LCD_DRIVER	void	sBackLightOff(void);
LCD_DRIVER	void	sFigGroupDisplayNum(INT8U bFirstFigID, INT16U bFigNum, INT16U bNum, INT8U bTime, INT8U bAll);
LCD_DRIVER	void	sFigGroupDisplayLetter(INT8U bFirstFigID, INT8U *bPbuf, INT8U bTime);
LCD_DRIVER	void	sViewFigModule(INT8U bSegNum, INT8U bFigID, INT8U bChar, INT8U bTime);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/


/********************************************************************************
* Input interface Routines														*
********************************************************************************/


#endif	// __LCD_DRIVER_H__

