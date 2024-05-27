/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		lcd_map.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __LCD_MAP_H__
#define __LCD_MAP_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"OS_CPU.h"
#include	"lcd_config.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef	DISPLAY_MAP
#define	DISP_MAP
#else
#define	DISP_MAP	extern
#endif

// definition for display direction
#define	cDIR_HORIZONTAL				0
#define	cDIR_VERTICAL				1

// definition for character index
enum	CHAR_INDEX
{
	char0 = 0,
	char1,
	char2,
	char3,
	char4,
	char5,
	char6,
	char7,
	char8,
	char9,
	charA,
	charB,
	charC,
	charD,
	charE,
	charF,
	charG,
	charH,
	charI,
	charJ,
	charK,
	charL,
	charM,
	charN,
	charO,
	charP,
	charQ,
	charR,
	charS,
	charT,
	charU,
	charV,
	charW,
	charX,
	charY,
	charZ,
	cCharSpace,
	cCharLength
};

// definition for character code (7 segments)
// 7 Segment code define
// Bit: 0, 1, 2, 3, 4, 5, 6, 7
// Seg: a, b, c, d, e, f, g, -  
// ///////////////////////////
//    __a__          
//   |     |
//  f|     |b
//   |__g__|
//   |     |
//  e|     |c
//   |____ |         
//      d          
// ///////////////////////////
#define	cSeg7Code0		0x3F
#define	cSeg7Code1      0x06
#define	cSeg7Code2      0x5B
#define	cSeg7Code3      0x4F
#define	cSeg7Code4      0x66
#define	cSeg7Code5      0x6D
#define	cSeg7Code6      0x7D
#define	cSeg7Code7      0x07
#define	cSeg7Code8      0x7F
#define	cSeg7Code9      0x6F
#define cSeg7CodeA      0x77
#define cSeg7CodeB      0x7C
#define cSeg7CodeC      0x39
#define cSeg7CodeD      0x5E
#define cSeg7CodeE      0x79
#define cSeg7CodeF      0x71
#define cSeg7CodeG      0x3D	//0x3E
#define cSeg7CodeH      0x76
#define cSeg7CodeI      0x30
#define cSeg7CodeJ      0x1E
#define cSeg7CodeK      0x70
#define cSeg7CodeL      0x38
#define cSeg7CodeM      0x54	//0x4F
#define cSeg7CodeN      0x37	//0x54
#define cSeg7CodeO      0x3F	//0x5C
#define cSeg7CodeP      0x73
#define cSeg7CodeQ      0x67
#define cSeg7CodeR      0x70	//0x5F
#define cSeg7CodeS      0x6D
#define cSeg7CodeT      0x78
#define cSeg7CodeU      0x3E
#define cSeg7CodeV      0x62
#define cSeg7CodeW      0x1C	//0x49	//
#define cSeg7CodeX      0x64
#define cSeg7CodeY      0x6E	//0x46	//
#define cSeg7CodeZ      0x5B	//0x49	//
#define cSeg7CodeSpace  0x00

// definition for LCD display bit(BIT mode)	// 16*7bit
enum LCD_BIT
{
	// Grid1
	cBIT_SEG1A_BAT = 0,
	cBIT_SEG2A_PCT25,
	cBIT_SEG3A_PCT100,
	cBIT_SEG4A_ALM,
	cBIT_SEG5A,
	cBIT_SEG6A,
	cBIT_SEG7A,
	cBIT_SEG8A_TEMPC,
	cBIT_SEG9A,
	cBIT_SEG10A,
	cBIT_X11A,
	cBIT_SEG12A,
	cBIT_SEG13A,
	cBIT_SEG14A,
	cBIT_X15A,
	cBIT_X16A,
	
	// Grid2
	cBIT_SEG1B_BAT,
	cBIT_SEG2B_PCT25,
	cBIT_SEG3B_BAT,
	cBIT_SEG4B_RUNSTS,
	cBIT_SEG5B,
	cBIT_SEG6B,
	cBIT_SEG7B,
	cBIT_SEG8B_VOLT,
	cBIT_SEG9B,
	cBIT_SEG10B,
	cBIT_X11B,
	cBIT_SEG12B,
	cBIT_SEG13B,
	cBIT_SEG14B,
	cBIT_X15B,
	cBIT_X16B,
	
	// Grid3
	cBIT_SEG1C_BAT,
	cBIT_SEG2C_PCT50,
	cBIT_SEG3C_BAT,
	cBIT_SEG4C_ALMSTS,
	cBIT_SEG5C,
	cBIT_SEG6C,
	cBIT_SEG7C,
	cBIT_SEG8C,
	cBIT_SEG9C,
	cBIT_SEG10C,
	cBIT_X11C,
	cBIT_SEG12C,
	cBIT_SEG13C,
	cBIT_SEG14C,
	cBIT_X15C,
	cBIT_X16C,
	
	// Grid4
	cBIT_SEG1D_BAT,
	cBIT_SEG2D_PCT50,
	cBIT_SEG3D_BAT,
	cBIT_SEG4D_WARN,
	cBIT_SEG5D,
	cBIT_SEG6D,
	cBIT_SEG7D,
	cBIT_SEG8D,
	cBIT_SEG9D,
	cBIT_SEG10D,
	cBIT_X11D,
	cBIT_SEG12D,
	cBIT_SEG13D,
	cBIT_SEG14D,
	cBIT_X15D,
	cBIT_X16D,
	
	// Grid5
	cBIT_SEG1E_BAT,
	cBIT_SEG2E_PCT75,
	cBIT_SEG3E_BAT,
	cBIT_SEG4E_OVT,
	cBIT_SEG5E,
	cBIT_SEG6E,
	cBIT_SEG7E,
	cBIT_SEG8E,
	cBIT_SEG9E,
	cBIT_SEG10E,
	cBIT_X11E,
	cBIT_SEG12E,
	cBIT_SEG13E,
	cBIT_SEG14E,
	cBIT_X15E,
	cBIT_X16E,
	
	// Grid6
	cBIT_SEG1F_BAT,
	cBIT_SEG2F_PCT75,
	cBIT_SEG3F_BAT,
	cBIT_SEG4F_DOT2,
	cBIT_SEG5F,
	cBIT_SEG6F,
	cBIT_SEG7F,
	cBIT_SEG8F,
	cBIT_SEG9F,
	cBIT_SEG10F,
	cBIT_X11F,
	cBIT_SEG12F,
	cBIT_SEG13F,
	cBIT_SEG14F,
	cBIT_X15F,
	cBIT_X16F,
	
	// Grid7
	cBIT_SEG1G_BAT,
	cBIT_SEG2G_PCT100,
	cBIT_SEG3G_RUN,
	cBIT_SEG4G_DOT1,
	cBIT_SEG5G,
	cBIT_SEG6G,
	cBIT_SEG7G,
	cBIT_SEG8G,
	cBIT_SEG9G,
	cBIT_SEG10G,
	cBIT_X11G,
	cBIT_SEG12G,
	cBIT_SEG13G,
	cBIT_SEG14G,
	cBIT_X15G,
	cBIT_X16G
};

// definition for LCD display cell(figure mode)
//Figure type
#define		cFIG_TYPE_7			7

//start address in FigureSEG_TAB array
#define		FIG_7SEG_POS		0

//definition index for figure
enum SEG7_INDEX
{
	FIG1_7SEG = 0,
	FIG2_7SEG,
	FIG3_7SEG
};

//definiton for figure struct
typedef union
{
	INT8U	bByte;
	struct
	{
		INT16U a:1;  //0
		INT16U b:1;  //1
		INT16U c:1;  //2
		INT16U d:1;  //3
		INT16U e:1;  //4
		INT16U f:1;  //5
		INT16U g:1;  //6
		INT16U i:1;  //7
	}Seg;
}Seg8_Field;

/********************************************************************************
* Routines' implementations														*
********************************************************************************/


/********************************************************************************
* Output interface Routines														*
********************************************************************************/


/********************************************************************************
* Input interface Routines														*
********************************************************************************/

#endif	// __LCD_MAP_H__

