/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		LcdProcess.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define			__LCD_PROCESS_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"Main.h"
#include	"lcd_map.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define         cTimerBase                      Timer_500ms

#define         cT500ms                         (500 / cTimerBase)
#define         cT1sec                          (1000 / cTimerBase)
#define         cT1sec5                         (1500 / cTimerBase)
#define         cT2sec                          (2000 / cTimerBase)
#define         cT2sec5                         (2500 / cTimerBase)
#define         cT3sec                          (3000 / cTimerBase)
#define         cT3sec5                         (3500 / cTimerBase)
#define         cT4sec                          (4000 / cTimerBase)
#define         cT4sec5                         (4500 / cTimerBase)
#define         cT5sec                          (5000 / cTimerBase)
#define         cT6sec                          (6000 / cTimerBase)
#define         cT10sec                         (10000 / cTimerBase)
#define         cT20sec                         (20000 / cTimerBase)
#define         cT1min                          (60000 / cTimerBase)
#define         cT2min                          (120000 / cTimerBase)
#define         cT5min                          (300000 / cTimerBase)

#define			SEL_MODE_EXIT_TIME				cT10sec
#define			CF_MODE_EXIT_TIME				cT20sec
#define			LCD_BL_TIME						cT5min	// for test
#define			LCD_LOOP_CNT					3

#define			cLCD_UPDATE_RATE				cTimerBase
#define			cALWAYS_LIGHT					0
#define			cFLASH_TIME_1S					cT1sec

#define			cFIG_ALL_DISP_DISABLE			0
#define			cFIG_ALL_DISP_ENABLE			1

#define			cLCD_PAGE_HOME					0
#define			cLCD_PAGE_USETIME				1
#define 		cLCD_PAGE_VOLT					2
#define			cLCD_PAGE_SCI					3
#define			cLCD_PAGE_BLOFF					4

#define			mSetVramBit(bitADDR, bTime)		sSendBitToVram(bitADDR, DISPLAY_ON, bTime)
#define			mClearVramBit(bitADDR)			sSendBitToVram(bitADDR, DISPLAY_OFF, 0)

extern INT16S GetBattCurr1(void);
	
typedef struct{
	INT16U SelectMode:1;
	INT16U ConfigMode:1;
}DfPanleJob;

enum LCD_LOOP_ID
{
	SELECT_SOC_ID = 0,
	SELECT_VOLT_ID,
	SELECT_CURR_ID,
	SELECT_CELL_01_VOLT_ID,
	SELECT_CELL_02_VOLT_ID,
	SELECT_CELL_03_VOLT_ID,
	SELECT_CELL_04_VOLT_ID,
	SELECT_CELL_05_VOLT_ID,
	SELECT_CELL_06_VOLT_ID,
	SELECT_CELL_07_VOLT_ID,
	SELECT_CELL_08_VOLT_ID,
	SELECT_CELL_09_VOLT_ID,
	SELECT_CELL_10_VOLT_ID,
	SELECT_CELL_11_VOLT_ID,
	SELECT_CELL_12_VOLT_ID,
	SELECT_CELL_13_VOLT_ID,
	SELECT_CELL_14_VOLT_ID,
	SELECT_CELL_15_VOLT_ID,
	SELECT_CELL_16_VOLT_ID,
	SELECT_CELL_01_TEMP_ID,
	SELECT_CELL_02_TEMP_ID,
	SELECT_CELL_03_TEMP_ID,
	SELECT_CELL_04_TEMP_ID,
	SELECT_RATED_AH_ID,
	SELECT_ADS_ID,
	SELECT_HW_VER_ID,
	SELECT_SW_VER_ID,
	SELECT_MOS_TEMP_ID,
	SELECT_MAX_ITEM
};

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
DfPanleJob	fPanleJob;

/********************************************************************************
* Internal variables															*
********************************************************************************/
INT8U	bSlectIndex;
INT8U	bMenuIndex;
INT8U	bConfigMode;
INT8U	bIsSelConfigItem;
INT8U	bIsAdjustVal;	// 0:未开始调节参数,1:调节参数中,2:参数调节完成
INT16S	wAdjustValue;
INT16U	wModeTimeOut;
INT16U	wLCDLoopCnt;
INT16U	wLCDBackLightCnt;
INT16U	wLcdInitSec;

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
void	sProcSelectMode(void);
void	sInitDisplay(void);
void	sHomeDisplay(void);
void	sLoopDisplay(void);
void	sStatusDisplay(void);
void	sDisplayFaultCode(INT16U wCode);
void	sDisplayChgSoc(INT16U wSoc);
void	sDisplayDsgSoc(INT16U wSoc);
void	sDisplayBattSoc(INT16U wSoc);
void	sDisplayBattVolt(INT16S wVolt);
void	sDisplayBattCurr(INT16S wCurr);
void	sDisplayBattWatt(INT16U wWatt);
void	sDisplayHwVersion(INT16U wVer);
void	sDisplaySwVersion(INT16U wVer);
void	sDisplayAddress(INT16U wAds);
void	sDisplayRatedAH(INT16U wAH);
void	sDisplayMosTemp(INT16S wTmp);
void	sDisplayCellVolt(INT8U	bNo, INT16S wVolt);
void	sDisplayCellTemp(INT8U	bNo, INT16S wTmp);
void	sDisplaySciAddr(void);
void	sVoltDisplay(void);
void	sTimeDisplay(void);
/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void	sLcdInitial(void)
{
	wModeTimeOut = 0;
	bSlectIndex = 0;
	bMenuIndex = 0;
	bConfigMode = 0;
	bIsSelConfigItem = 0;
	bIsAdjustVal = 0;
	wAdjustValue = 0;
	wLCDLoopCnt = LCD_LOOP_CNT;
	wLCDBackLightCnt = LCD_BL_TIME;
	wLcdInitSec = 65535;
	
}

void	sLcdTask(INT16U wFilter)
{
	static	INT16U	wCnt = cLCD_UPDATE_RATE;
	static  INT16U  wNormalPage = cLCD_PAGE_HOME;
	// BL initial
	if(sbGetKeyStatusLCDMode() == true)
	{
		if(wLCDBackLightCnt == 0)
		{
			wCnt = cLCD_UPDATE_RATE;
			wLCDBackLightCnt = LCD_BL_TIME;
		}

		wNormalPage++;
		if(wNormalPage == cLCD_PAGE_BLOFF)
		{
			wLCDBackLightCnt = 0;
		}
		else if(wNormalPage > cLCD_PAGE_BLOFF)
		{
			wNormalPage = cLCD_PAGE_HOME;
		}
		sSetKeyStatusLCDMode(false);
	}
	
	wCnt += wFilter;
	if(++wCnt < cLCD_UPDATE_RATE)
	{
		return;
	}
	wCnt = 0;
	
	// Update vram area
	sLcdVramAreaClear();
	if((wLcdInitSec != 0) && (bBmsMode == cPowerOnMode))
	{
		sInitDisplay();
	}
	else if(bBmsMode == cShutdownMode)
	{
		sBackLightOff();
	}
	else if(sbGetKeyStatusDebugMode())
	{
		sProcSelectMode();
		sLoopDisplay();
		sStatusDisplay();
	}
	else
	{
		if(wNormalPage == cLCD_PAGE_SCI)
		{
			sDisplaySciAddr();
		}
		else if(wNormalPage == cLCD_PAGE_HOME)
		{
			sHomeDisplay();
		}
		else if(wNormalPage == cLCD_PAGE_USETIME)
		{
			sTimeDisplay();
		}
		else if(wNormalPage == cLCD_PAGE_VOLT)
		{
			sVoltDisplay();
		}
		sStatusDisplay();
	}
	
	if(wLCDLoopCnt == 0)
	{
		sSetKeyStatusDebugMode(false);
	}
	
	// LED backlight
	if(wLCDBackLightCnt > 0)
	{
		sBackLightOn();
		wLCDBackLightCnt--;
	}
	//else if((swGetEepromBackLightEn() == 0) && (sbGetKeyStatusDebugMode() == 0))
	else if(swGetEepromBackLightEn() == 0)
	{
		sBackLightOff();
	}
	
	// Refresh
	sLcdHwInit();
	sLcdUpdateData();
	sLcdUpdate();
}

void	sProcSelectMode(void)
{
	static INT16U wCnt = 0;
	
	if(++wCnt >= cT2sec5)
	{
		wCnt = 0;
		bSlectIndex++;
		if((bSlectIndex >= (SELECT_CELL_01_VOLT_ID + swGetBmsCellVoltNum())) \
			&& (bSlectIndex <= SELECT_CELL_16_VOLT_ID))
		{
			bSlectIndex = SELECT_CELL_16_VOLT_ID+1;
		}
		if((bSlectIndex >= (SELECT_CELL_01_TEMP_ID + swGetBmsCellTempNum())) \
			&& (bSlectIndex <= SELECT_CELL_04_TEMP_ID))
		{
			bSlectIndex = SELECT_CELL_04_TEMP_ID+1;
		}
		
		if(bSlectIndex >= SELECT_MAX_ITEM)
		{
			bSlectIndex = 0;
			if(wLCDLoopCnt > 0)
			{
				wLCDLoopCnt--;
			}
		}
	}
}

void	sInitDisplay(void)
{
	if(wLcdInitSec > cT5sec)
	{
		wLcdInitSec = cT5sec;
	}
	
	if(wLcdInitSec > 0)
	{
		sLcdVramAreaSet();
//		mClearVramBit(cBIT_ICON_RIGHT);
//		mClearVramBit(cBIT_ICON_LEFT);
//		mClearVramBit(cBIT_ICON_MASTER);
//		mClearVramBit(cBIT_ICON_SLAVE);
//		mClearVramBit(cBIT_ICON_PCS);
//		mClearVramBit(cBIT_CHAR_PCT1);
//		mClearVramBit(cBIT_CHAR_TEMP);
//		mClearVramBit(cBIT_CHAR_A);
//		mClearVramBit(cBIT_CHAR_V2);
//		mClearVramBit(cBIT_CHAR_KWH_H);
//		mClearVramBit(cBIT_CHAR_VAH_H);
//		mClearVramBit(cBIT_CHAR_TEMP_F);
	}
	
	wLcdInitSec--;
}

void	sStatusDisplay(void)
{
	INT16U wTemp;
	
	// 电池图标
	wTemp = swGetSocNew();
	if((bBmsMode == cBatteryMode) || (bBmsMode == cChargeMode))
	{
		if(sbGetBmsCurrentStatus() == cBMS_CURR_CHG)
		{
			sDisplayChgSoc(wTemp);
		}
		else
		{
			sDisplayDsgSoc(wTemp);
		}
	}
	else
	{
		sDisplayDsgSoc(wTemp);
	}
	
	// 设置图标
	if(sbGetKeyStatusDebugMode())
	{
		mSetVramBit(cBIT_SEG4D_WARN, cALWAYS_LIGHT);
	}
	
	// 运行/告警图标
	if(bBmsMode == cFaultMode)
	{
		mSetVramBit(cBIT_SEG4A_ALM, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG4C_ALMSTS, cALWAYS_LIGHT);
	}
	else if(sbGetKeyStatusForcedOn())
	{
		mSetVramBit(cBIT_SEG3G_RUN, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG4B_RUNSTS, cALWAYS_LIGHT);
		
		mSetVramBit(cBIT_SEG4A_ALM, cFLASH_TIME_1S);
		mSetVramBit(cBIT_SEG4C_ALMSTS, cFLASH_TIME_1S);
	}
	else
	{
		mSetVramBit(cBIT_SEG3G_RUN, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG4B_RUNSTS, cALWAYS_LIGHT);
	}
}

void	sHomeDisplay(void)
{
	INT16U wTemp;
	
	// Power/Alarm
	if(bBmsMode == cFaultMode)
	{
		wTemp = sbGetBmsFaultCode();
		sDisplayFaultCode(wTemp);
	}
	else if(bBmsMode != cPowerOnMode)
	{
		wTemp = swGetSocNew();		// 1%
		sDisplayBattSoc(wTemp);
	}
}

void 	sVoltDisplay(void)
{
	INT16U wTemp;
	wTemp = (swGetBattVoltFiltNew() + 5) / 10;	// 0.1V
	sDisplayBattVolt(wTemp);
}

void	sTimeDisplay(void)
{
	if(GetBattCurr1()<0)
	{
		sViewFigModule(cFIG_TYPE_7, FIG1_7SEG, charC, cALWAYS_LIGHT);
		sViewFigModule(cFIG_TYPE_7, FIG2_7SEG, charH, cALWAYS_LIGHT);
	}
	else
	{
		INT16U wTemp2,wTemp3,Time;
		INT16U wTemp1;
		wTemp1 = (swAbs(swGetBattCurrFiltNew()) + 5) / 10;	// 1A
		wTemp2 = swGetSocNew();
		wTemp3 = swGetBmsRatedAH();
		if(wTemp1 > 0)
		{	
				Time = (wTemp3/1000*wTemp2)/wTemp1;
			if(Time>99)
			{
				Time = 99;
			}
			else if(Time<10)
			{
				Time = (wTemp3/100*wTemp2)/wTemp1;
				mSetVramBit(cBIT_SEG4F_DOT2, cALWAYS_LIGHT);
			}
				sFigGroupDisplayNum(FIG1_7SEG, 2, Time, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
				sViewFigModule(cFIG_TYPE_7, FIG3_7SEG, charH, cALWAYS_LIGHT);
		}
//		else if(swGetBattCurrFiltNew() < 0)
//		{
//			sViewFigModule(cFIG_TYPE_7, FIG1_7SEG, charC, cALWAYS_LIGHT);
//			sViewFigModule(cFIG_TYPE_7, FIG2_7SEG, charH, cALWAYS_LIGHT);
//		}
		 else if(wTemp1 == 0)
		{
			Time = 999;
			sFigGroupDisplayNum(FIG1_7SEG, 3, Time, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
		}
	}
}

void	sLoopDisplay(void)
{
	static INT8U bSlectIndexPre = 0xFF;
	INT16U 	wTemp;
	
	if(bSlectIndexPre != bSlectIndex)
	{
		bSlectIndexPre = bSlectIndex;
		return;
	}
	
	// Loop information
	switch(bSlectIndex)
	{
		case SELECT_SOC_ID:
			wTemp = swGetSocNew();		// 1%
			sDisplayBattSoc(wTemp);
		break;
		case SELECT_VOLT_ID:
			wTemp = (swGetBattVoltFiltNew() + 5) / 10;			// 0.1V
			sDisplayBattVolt(wTemp);
		break;
		case SELECT_CURR_ID:
			wTemp = (swAbs(swGetBattCurrFiltNew()) + 5) / 10;	// 1A
			sDisplayBattCurr(wTemp);
		break;
		case SELECT_CELL_01_VOLT_ID:
			sDisplayCellVolt(1, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH01));
		break;
		case SELECT_CELL_02_VOLT_ID:
			sDisplayCellVolt(2, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH02));
		break;
		case SELECT_CELL_03_VOLT_ID:
			sDisplayCellVolt(3, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH03));
		break;
		case SELECT_CELL_04_VOLT_ID:
			sDisplayCellVolt(4, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH04));
		break;
		case SELECT_CELL_05_VOLT_ID:
			sDisplayCellVolt(5, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH05));
		break;
		case SELECT_CELL_06_VOLT_ID:
			sDisplayCellVolt(6, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH06));
		break;
		case SELECT_CELL_07_VOLT_ID:
			sDisplayCellVolt(7, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH07));
		break;
		case SELECT_CELL_08_VOLT_ID:
			sDisplayCellVolt(8, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH08));
		break;
		case SELECT_CELL_09_VOLT_ID:
			sDisplayCellVolt(9, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH09));
		break;
		case SELECT_CELL_10_VOLT_ID:
			sDisplayCellVolt(10, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH10));
		break;
		case SELECT_CELL_11_VOLT_ID:
			sDisplayCellVolt(11, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH11));
		break;
		case SELECT_CELL_12_VOLT_ID:
			sDisplayCellVolt(12, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH12));
		break;
		case SELECT_CELL_13_VOLT_ID:
			sDisplayCellVolt(13, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH13));
		break;
		case SELECT_CELL_14_VOLT_ID:
			sDisplayCellVolt(14, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH14));
		break;
		case SELECT_CELL_15_VOLT_ID:
			sDisplayCellVolt(15, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH15));
		break;
		case SELECT_CELL_16_VOLT_ID:
			sDisplayCellVolt(16, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH16));
		break;
		case SELECT_CELL_01_TEMP_ID:
			sDisplayCellTemp(1, swGetCellTempFiltNew(cADC_CELL_TEMP_CH01));
		break;
		case SELECT_CELL_02_TEMP_ID:
			sDisplayCellTemp(2, swGetCellTempFiltNew(cADC_CELL_TEMP_CH02));
		break;
		case SELECT_CELL_03_TEMP_ID:
			sDisplayCellTemp(3, swGetCellTempFiltNew(cADC_CELL_TEMP_CH03));
		break;
		case SELECT_CELL_04_TEMP_ID:
			sDisplayCellTemp(4, swGetCellTempFiltNew(cADC_CELL_TEMP_CH04));
		break;
		case SELECT_RATED_AH_ID:
			wTemp = swGetBmsNominalAH() / 10;
			sDisplayRatedAH(wTemp);
		break;
		case SELECT_ADS_ID:
			wTemp = sbGetSciAddress();
			sDisplayAddress(wTemp);
		break;
		case SELECT_HW_VER_ID:
			wTemp = swGetHwVersion1Real() >> 2;
			sDisplayHwVersion(wTemp);
		break;
		case SELECT_SW_VER_ID:
			wTemp = MACHINE_SW_VERSION;
			sDisplayHwVersion(wTemp);
		break;
		case SELECT_MOS_TEMP_ID:
			wTemp = swGetHsTempNew();		// 1℃
			sDisplayMosTemp(wTemp);
		break;
		default:break;
	}
}


void	sDisplayFaultCode(INT16U wCode)
{
	INT16U	wTemp;
	
	wTemp = wCode;
	if(wTemp > 99)
	{
		wTemp = 99;
	}
	sViewFigModule(cFIG_TYPE_7, FIG1_7SEG, charF, cALWAYS_LIGHT);
	sFigGroupDisplayNum(FIG2_7SEG, 2, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
}

void	sDisplayChgSoc(INT16U wSoc)	// 1%
{
	static INT16U wSocCnt = cT6sec;
	static INT16U wCntBias = 0;
	INT16U	wTemp;
	
	wTemp = wSoc;
	if(++wSocCnt >= cT5sec)
	{
		if(wTemp < 25)
		{
			wCntBias = 0;
		}
		else if(wTemp < 50)
		{
			wCntBias = cT1sec;
		}
		else if(wTemp < 75)
		{
			wCntBias = cT2sec;
		}
		else if(wTemp < 100)
		{
			wCntBias = cT3sec;
		}
		else
		{
			wCntBias = cT4sec;
		}
		wSocCnt = wCntBias;
	}
	
	if(wSocCnt < cT1sec)
	{
		;
	}
	else if(wSocCnt < cT2sec)
	{
		mSetVramBit(cBIT_SEG2A_PCT25, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2B_PCT25, cALWAYS_LIGHT);
	}
	else if(wSocCnt < cT3sec)
	{
		mSetVramBit(cBIT_SEG2A_PCT25, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2B_PCT25, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2C_PCT50, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2D_PCT50, cALWAYS_LIGHT);
	}
	else if(wSocCnt < cT4sec)
	{
		mSetVramBit(cBIT_SEG2A_PCT25, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2B_PCT25, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2C_PCT50, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2D_PCT50, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2E_PCT75, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2F_PCT75, cALWAYS_LIGHT);
	}
	else
	{
		mSetVramBit(cBIT_SEG2A_PCT25, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2B_PCT25, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2C_PCT50, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2D_PCT50, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2E_PCT75, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2F_PCT75, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2G_PCT100, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG3A_PCT100, cALWAYS_LIGHT);
	}
	mSetVramBit(cBIT_SEG1A_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG1B_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG1C_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG1D_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG1E_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG1F_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG1G_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG3B_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG3C_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG3D_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG3E_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG3F_BAT, cALWAYS_LIGHT);
}

void	sDisplayDsgSoc(INT16U wSoc)	// 1%
{
	INT16U	wTemp;
	
	wTemp = wSoc;
	if(wTemp == 0)
	{
		;
	}
	else if(wTemp <= 25)
	{
		mSetVramBit(cBIT_SEG2A_PCT25, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2B_PCT25, cALWAYS_LIGHT);
	}
	else if(wTemp <= 50)
	{
		mSetVramBit(cBIT_SEG2A_PCT25, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2B_PCT25, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2C_PCT50, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2D_PCT50, cALWAYS_LIGHT);
	}
	else if(wTemp <= 75)
	{
		mSetVramBit(cBIT_SEG2A_PCT25, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2B_PCT25, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2C_PCT50, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2D_PCT50, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2E_PCT75, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2F_PCT75, cALWAYS_LIGHT);
	}
	else
	{
		mSetVramBit(cBIT_SEG2A_PCT25, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2B_PCT25, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2C_PCT50, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2D_PCT50, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2E_PCT75, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2F_PCT75, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG2G_PCT100, cALWAYS_LIGHT);
		mSetVramBit(cBIT_SEG3A_PCT100, cALWAYS_LIGHT);
	}
	mSetVramBit(cBIT_SEG1A_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG1B_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG1C_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG1D_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG1E_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG1F_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG1G_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG3B_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG3C_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG3D_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG3E_BAT, cALWAYS_LIGHT);
	mSetVramBit(cBIT_SEG3F_BAT, cALWAYS_LIGHT);
}

void	sDisplayBattSoc(INT16U wSoc)	// 1%
{
	INT16U	wTemp;
	
	wTemp = wSoc;
	if(wTemp >= 100)
	{
		sFigGroupDisplayNum(FIG1_7SEG, 3, 100, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
	}
	else
	{
		sFigGroupDisplayNum(FIG2_7SEG, 2, wTemp%100, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
	}
	mSetVramBit(cBIT_SEG8A_TEMPC, cALWAYS_LIGHT);
}

extern INT8U	bParallelErrChkCnt;
void	sDisplayBattVolt(INT16S wVolt)	// 0.1V
{
	INT16U	wTemp;
	
	wTemp = wVolt;
	if(wTemp < 10)
	{
		wTemp = 0;
	}
	else if(wTemp > 999)
	{
		wTemp = 999;
	}
	if(wTemp != 0)
	{
		mSetVramBit(cBIT_SEG4G_DOT1, cALWAYS_LIGHT);
	}

//	wTemp = bParallelErrChkCnt;
	sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
	mSetVramBit(cBIT_SEG8B_VOLT, cALWAYS_LIGHT);
}

void	sDisplayBattCurr(INT16S wCurr)	// 1A
{
	INT16S	wTemp;
	
	wTemp = wCurr;
	if(wTemp > 999)
	{
		wTemp = 999;
	}
	else if(wTemp < 0)
	{
		wTemp = 0;
	}
	sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
}

void	sDisplayBattWatt(INT16U wWatt)	// 0.01kW
{
	INT16U	wTemp;
	
	wTemp = wWatt;
	if(wTemp < 999)
	{
		mSetVramBit(cBIT_SEG4F_DOT2, cALWAYS_LIGHT);
		sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
	}
	else
	{
		wTemp = (wTemp + 5) / 10;
		mSetVramBit(cBIT_SEG4G_DOT1, cALWAYS_LIGHT);
		sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
	}
}

void	sDisplayHwVersion(INT16U wVer)
{
	INT16U	wTemp;
	
	// HwVerion
	//    0	 205(  0   51)
	//  205  614( 51  154)
	//  614 1024(154  256)
	// 1024	1434(256  359)
	// 1434	1843(359  461)
	// 1843	2253(461  563)
	// 2253	2662(563  666)
	// 2662	3072(666  768)
	// 3072	3482(768  871)
	// 3482	3891(871  973)
	// 3891	4096(973 1024)
	wTemp = wVer;
	if(wTemp > 999)
	{
		wTemp = 999;
	}
	sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
}

void	sDisplaySwVersion(INT16U wVer)
{
	INT16U	wTemp;
	
	wTemp = wVer;
	if(wTemp > 999)
	{
		wTemp = 999;
	}
	sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
}

void	sDisplayAddress(INT16U wAds)
{
	INT16U	wTemp;
	
	wTemp = wAds;
	if(wTemp > 32)
	{
		wTemp = 32;
	}
	sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
}

void	sDisplayRatedAH(INT16U wAH)
{
	INT16U	wTemp;
	
	wTemp = wAH;
	if(wTemp > 999)
	{
		wTemp = 999;
	}
	sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
}

void	sDisplayMosTemp(INT16S wTmp)
{
	INT16S	wTemp;
	
	if(wTmp >= 0)
	{
		wTemp = wTmp;
		if(wTemp > 999)
		{
			wTemp = 999;
		}
		sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
	}
	else
	{
		wTemp = -wTmp;
		if(wTemp <= 9)
		{
			mSetVramBit(cBIT_SEG6G, cALWAYS_LIGHT);	// 负号
			sFigGroupDisplayNum(FIG3_7SEG, 1, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
		}
		else
		{
			if(wTemp > 99)
			{
				wTemp = 99;
			}
			mSetVramBit(cBIT_SEG5G, cALWAYS_LIGHT);	// 负号
			sFigGroupDisplayNum(FIG2_7SEG, 2, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
		}
	}
	//mSetVramBit(cBIT_SEG8A_TEMPC, cALWAYS_LIGHT);
}

void	sDisplayCellVolt(INT8U	bNo, INT16S wVolt)
{
	INT16U	wTemp;
	
//	// 电芯序号
//	wTemp = bNo;
//	if(wTemp > 99)
//	{
//		wTemp = 99;
//	}
//	sViewFigModule(cFIG_TYPE_7, FIG1_7SEG, charC, cALWAYS_LIGHT);
//	sFigGroupDisplayNum(FIG2_7SEG, 2, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
	
	// 电芯电压
	wTemp = (swAbs(wVolt) + 5) / 10;	// x.xxV
	if(wTemp > 999)
	{
		wTemp = 999;
	}
	mSetVramBit(cBIT_SEG4F_DOT2, cALWAYS_LIGHT);
	sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
	mSetVramBit(cBIT_SEG8B_VOLT, cALWAYS_LIGHT);
}

void	sDisplayCellTemp(INT8U	bNo, INT16S wTmp)
{
	INT16S	wTemp;
	
//	// 温感序号
//	wTemp = bNo;
//	if(wTemp > 99)
//	{
//		wTemp = 99;
//	}
//	sViewFigModule(cFIG_TYPE_7, FIG1_7SEG, charC, cALWAYS_LIGHT);
//	sFigGroupDisplayNum(FIG2_7SEG, 2, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
	
	// 温感温度
	wTemp = wTmp;
	if(wTemp >= 0)
	{
		if(wTemp > 999)
		{
			wTemp = 999;
		}
		sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
	}
	else
	{
		if(wTemp <= -10)
		{
			if(wTemp < -99)
			{
				wTemp = -99;
			}
			mSetVramBit(cBIT_SEG5G, cALWAYS_LIGHT);	// 负号
			sFigGroupDisplayNum(FIG2_7SEG, 2, -wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
		}
		else
		{
			mSetVramBit(cBIT_SEG6G, cALWAYS_LIGHT);	// 负号
			sFigGroupDisplayNum(FIG3_7SEG, 1, -wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
		}
	}
	//mSetVramBit(cBIT_SEG8A_TEMPC, cALWAYS_LIGHT);
}

void	sDisplaySciAddr(void)
{
	sViewFigModule(cFIG_TYPE_7, FIG1_7SEG, charA, cALWAYS_LIGHT);
	sFigGroupDisplayNum(FIG2_7SEG, 2, sbGetSciAddress(), cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
}


/********************************************************************************
* Output interface Routines														*
********************************************************************************/


/********************************************************************************
* Input interface Routines														*
********************************************************************************/
void	sRstLCDLoopCnt(void)
{
	wLCDLoopCnt = LCD_LOOP_CNT;
}


