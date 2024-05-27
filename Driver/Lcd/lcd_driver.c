/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		lcd_driver.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define		__LCD_DRIVER_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"lcd_driver.h"
#include	"lcd_map.h"
#include	"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define		cCmdModeID				0x08
#define		cWRModeID				0x0A

// Lcd Cmd define
#define		cMODECmd				0x03	// 显示模式设置命令,7位10段模式
#define		cRWCmd					0x40	// 数据读写设置命令,采用地址自动加1模式
#define		cADSCmd					0xC0	// 地址设置命令,从00H开始
#define		cKEYCmd					0x42	// 数据读写设置命令,读键扫数据

/********************************************************************************
* Constants																		*
********************************************************************************/
//definition for character code  table
const	INT8U	Seg7CodeTab[cCharLength]=
{
	cSeg7Code0,cSeg7Code1,cSeg7Code2,cSeg7Code3,cSeg7Code4,cSeg7Code5,cSeg7Code6,cSeg7Code7,cSeg7Code8,cSeg7Code9,
	cSeg7CodeA,cSeg7CodeB,cSeg7CodeC,cSeg7CodeD,cSeg7CodeE,cSeg7CodeF,cSeg7CodeG,cSeg7CodeH,cSeg7CodeI,cSeg7CodeJ,
	cSeg7CodeK,cSeg7CodeL,cSeg7CodeM,cSeg7CodeN,cSeg7CodeO,cSeg7CodeP,cSeg7CodeQ,cSeg7CodeR,cSeg7CodeS,cSeg7CodeT,
	cSeg7CodeU,cSeg7CodeV,cSeg7CodeW,cSeg7CodeX,cSeg7CodeY,cSeg7CodeZ,cSeg7CodeSpace
};

//Figure SEG table --图 SEG 表
const INT8U FigureSEG_TAB[FIG_7SEG_TOTAL * 7] =
   //horizontal display --水平显示
{   //7 segments, offset = 0, 
	cBIT_SEG5A, cBIT_SEG5B, cBIT_SEG5C, cBIT_SEG5D, cBIT_SEG5E, cBIT_SEG5F, cBIT_SEG5G,
	cBIT_SEG6A, cBIT_SEG6B, cBIT_SEG6C, cBIT_SEG6D, cBIT_SEG6E, cBIT_SEG6F, cBIT_SEG6G,
	cBIT_SEG7A, cBIT_SEG7B, cBIT_SEG7C, cBIT_SEG7D, cBIT_SEG7E, cBIT_SEG7F, cBIT_SEG7G
};

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
VIEWBUF ViewBuf[LCD_VRAM_SIZE];
enum LCD_BL_MODE eLCDBLMode;

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
void sLCDSend8bit(INT8U dat);
void sLCDCommand(INT8U com);
void sLcdDelay10us(void);

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void	sLcdHwInit(void)
{
	eLCDBLMode = cLCD_BL_1_16;
	
}

void	sLcdUpdate(void)
{
	INT8U i;
	
	sLCDCommand(cMODECmd);
	sLCDCommand(cRWCmd);
	sLCDCommand(cADSCmd);
	for(i = 0; i < LCD_VRAM_SIZE; i++)
	{
		if (1 == ViewBuf[i].BIT.val)
		{
			mLCD_SDA_HIGH();
		}
		else
		{
			mLCD_SDA_LOW();
		}
		sLcdDelay10us();
		mLCD_SCL_LOW();
		sLcdDelay10us();
		mLCD_SCL_HIGH();
		sLcdDelay10us();
	}
	sLCDCommand(eLCDBLMode);// 显示控制命令，打开显示并设置为最亮/最暗
	
	mLCD_CS_HIGH();
	sLcdDelay10us();
}

void	sLcdUpdateData(void)
{
	INT8U	i;
	
	for(i = 0; i < LCD_VRAM_SIZE; i++)
	{
		if(0 != ViewBuf[i].BIT.timeout)
		{
			if(ViewBuf[i].BIT.timeout > ViewBuf[i].BIT.Duty50)
			ViewBuf[i].BIT.val = ViewBuf[i].BIT.SetVal;
			ViewBuf[i].BIT.timeout--;
		}
	}
}

void	sLcdVramAreaSet(void)
{
	INT16U i;
	
	for(i = 0; i < LCD_VRAM_SIZE; i++)
	{
		if((i % 16) >= 8)	// 高字节默认0
		{
			ViewBuf[i].BIT.val = DISPLAY_OFF;
		}
		else
		{
			ViewBuf[i].BIT.val = DISPLAY_ON;
		}
		ViewBuf[i].BIT.timeout = 0;
		ViewBuf[i].BIT.Duty50 = 0;
	}
}

void	sLcdVramAreaClear(void)
{
	INT8U i;
	
    for(i = 0; i < LCD_VRAM_SIZE; i++)
    {
        ViewBuf[i].BIT.val = DISPLAY_OFF;
    }
}

void	sSendBitToVram(INT8U bitAddr, INT8U bVal, INT8U Time)
{
    if(((0 == ViewBuf[bitAddr].BIT.timeout) \
		|| (Time == 0) || ((Time & 0x80) != 0)) \
		&& (bitAddr < LCD_VRAM_SIZE))
    {
        Time &= 0x7F;
        ViewBuf[bitAddr].BIT.val = bVal;
        ViewBuf[bitAddr].BIT.SetVal = bVal;
        ViewBuf[bitAddr].BIT.timeout = Time;
        ViewBuf[bitAddr].BIT.Duty50 = Time >> 1;
    }
}

void	sFigGroupDisplayNum(INT8U bFirstFigID, INT16U bFigNum, INT16U bNum, INT8U bTime, INT8U bAll)
{
	INT8U bChar,bIsMostBit = 0;
	
	if(3 == bFigNum)
		bFigNum = 100;
	else if(2 == bFigNum)
		bFigNum = 10;
	else
		bFigNum = 1;
	
	bNum %= (bFigNum*10);
	
	for(; 0 != bFigNum ;)
	{
		bChar = bNum/bFigNum;
		
		bNum %= bFigNum;
		if(0 == bChar && bFigNum != 1 && 0 == bIsMostBit)
		{
			if(bAll == true)	// if value = 0, then dispaly 000(num len = 3)
			{
				sViewFigModule(cFIG_TYPE_7, bFirstFigID++, char0, bTime);
			}
			else
			{
				sViewFigModule(cFIG_TYPE_7, bFirstFigID++, cCharSpace, bTime);
			}
		}
		else
		{
			sViewFigModule(cFIG_TYPE_7, bFirstFigID++, bChar, bTime);
			bIsMostBit = 1;
		}
		bFigNum /= 10;
	}
} 

void	sFigGroupDisplayLetter(INT8U bFirstFigID, INT8U *bPbuf, INT8U bTime)
{
	INT8U bFigNum = *bPbuf ++;
	
	for(; bFigNum > 0; bFigNum --)
	{
		if((*bPbuf) <= cCharSpace)
		{
			sViewFigModule(cFIG_TYPE_7, bFirstFigID++, (*bPbuf++), bTime);
		}
	}
}

void	sViewFigModule(INT8U bSegNum, INT8U bFigID, INT8U bChar, INT8U bTime)
{
	Seg8_Field     sTemp;
	bFigID *= bSegNum;
	
	sTemp.bByte = Seg7CodeTab[bChar];
	bFigID += FIG_7SEG_POS;
	
	sSendBitToVram(FigureSEG_TAB[bFigID++], sTemp.Seg.a, bTime);
	sSendBitToVram(FigureSEG_TAB[bFigID++], sTemp.Seg.b, bTime);
	sSendBitToVram(FigureSEG_TAB[bFigID++], sTemp.Seg.c, bTime);
	sSendBitToVram(FigureSEG_TAB[bFigID++], sTemp.Seg.d, bTime);
	sSendBitToVram(FigureSEG_TAB[bFigID++], sTemp.Seg.e, bTime);
	sSendBitToVram(FigureSEG_TAB[bFigID++], sTemp.Seg.f, bTime);
	sSendBitToVram(FigureSEG_TAB[bFigID++], sTemp.Seg.g, bTime);
}

void	sBackLightOn(void)
{
	eLCDBLMode = cLCD_BL_14_16;
}

void	sBackLightOff(void)
{
	INT8U i;
	
	eLCDBLMode = cLCD_BL_1_16;
	
	for(i = 0; i < LCD_VRAM_SIZE; i++)
	{
		if((i != cBIT_SEG3G_RUN) && (i != cBIT_SEG4B_RUNSTS) \
			&& (i != cBIT_SEG4A_ALM) && (i != cBIT_SEG4C_ALMSTS))
		{
			ViewBuf[i].BIT.val = DISPLAY_OFF;
			ViewBuf[i].BIT.timeout = 0;
			ViewBuf[i].BIT.Duty50 = 0;
		}
	}
}

void	sLCDSend8bit(INT8U dat)
{
	INT8U i;
	
	for(i = 0; i < 8; i++)
	{
		if(dat & 0x01)
		{
			mLCD_SDA_HIGH();
		}
		else
		{
			mLCD_SDA_LOW();
		}
		sLcdDelay10us();
		mLCD_SCL_LOW();
		sLcdDelay10us();
		mLCD_SCL_HIGH();
		sLcdDelay10us();
		dat = dat >> 1;
	}
}

void	sLCDCommand(INT8U com)
{
	mLCD_CS_HIGH();
	sLcdDelay10us();
	mLCD_CS_LOW();
	sLcdDelay10us();
	sLCDSend8bit(com);
}

void	sLcdDelay10us(void)
{
	INT8U	xxx;
	
	for(xxx = 0; xxx< 11; xxx++)	// 实测120MHz时11＝10.35us,如果用于IIC则对应100kHz频率
	{
		__asm("nop");
	}
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/


/********************************************************************************
* Input interface Routines														*
********************************************************************************/


