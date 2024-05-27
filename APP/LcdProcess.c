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
#define			cLCD_PAGE_SCI					1
#define			cLCD_PAGE_BLOFF					2

#define			mSetVramBit(bitADDR, bTime)		sSendBitToVram(bitADDR, DISPLAY_ON, bTime)
#define			mClearVramBit(bitADDR)			sSendBitToVram(bitADDR, DISPLAY_OFF, 0)

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


/********************************************************************************
* Routines' implementations														*
********************************************************************************/
// LCD 初始化函数
void sLcdInitial(void)
{
    wModeTimeOut = 0;              // 模式超时时间
    bSlectIndex = 0;               // 选择索引
    bMenuIndex = 0;                // 菜单索引
    bConfigMode = 0;               // 配置模式标志
    bIsSelConfigItem = 0;          // 是否选择配置项标志
    bIsAdjustVal = 0;              // 是否调整值标志
    wAdjustValue = 0;              // 调整值
    wLCDLoopCnt = LCD_LOOP_CNT;    // LCD 循环计数器
    wLCDBackLightCnt = LCD_BL_TIME;// LCD 背光计时器
    wLcdInitSec = 65535;           // LCD 初始化计时
}

// LCD 任务函数
void sLcdTask(INT16U wFilter)
{
    static INT16U wCnt = cLCD_UPDATE_RATE;  // 计数器，初始化为 LCD 更新速率
    static INT16U wNormalPage = cLCD_PAGE_HOME;  // 正常页面，初始化为主页

    // 背光初始化
    if(sbGetKeyStatusLCDMode() == true)
    {
        if(wLCDBackLightCnt == 0)
        {
            wCnt = cLCD_UPDATE_RATE;  // 重置计数器
            wLCDBackLightCnt = LCD_BL_TIME;  // 重置背光计时器
        }

        wNormalPage++;  // 切换到下一个页面
        if(wNormalPage == cLCD_PAGE_BLOFF)
        {
            wLCDBackLightCnt = 0;  // 关闭背光
        }
        else if(wNormalPage > cLCD_PAGE_BLOFF)
        {
            wNormalPage = cLCD_PAGE_HOME;  // 返回主页
        }
        sSetKeyStatusLCDMode(false);  // 重置键状态
    }

    wCnt += wFilter;  // 更新计数器
    if(++wCnt < cLCD_UPDATE_RATE)
    {
        return;  // 如果计数器未达到更新速率，返回
    }
    wCnt = 0;  // 重置计数器

    // 更新 VRAM 区域
    sLcdVramAreaClear();
    if((wLcdInitSec != 0) && (bBmsMode == cPowerOnMode))
    {
        sInitDisplay();  // 显示初始化内容
    }
    else if(bBmsMode == cShutdownMode)
    {
        sBackLightOff();  // 关闭背光
    }
    else if(sbGetKeyStatusDebugMode())
    {
        sProcSelectMode();  // 处理选择模式
        sLoopDisplay();     // 循环显示
        sStatusDisplay();   // 状态显示
    }
    else
    {
        if(wNormalPage == cLCD_PAGE_SCI)
        {
            sDisplaySciAddr();  // 显示 SCI 地址
        }
        else
        {
            sHomeDisplay();  // 显示主页
        }
        sStatusDisplay();  // 状态显示
    }

    if(wLCDLoopCnt == 0)
    {
        sSetKeyStatusDebugMode(false);  // 退出调试模式
    }

    // LED 背光控制
    if(wLCDBackLightCnt > 0)
    {
        sBackLightOn();  // 打开背光
        wLCDBackLightCnt--;
    }
    else if(swGetEepromBackLightEn() == 0)
    {
        sBackLightOff();  // 关闭背光
    }

    // 刷新 LCD
    sLcdHwInit();  // 硬件初始化
    sLcdUpdateData();  // 更新数据
    sLcdUpdate();  // 更新显示
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
		wTemp = (swGetBattVoltFiltNew() + 5) / 10;	// 0.1V
		sDisplayBattVolt(wTemp);
	}
}

void	sLoopDisplay(void)
{
	static INT8U bSlectIndexPre = 0xFF; // 定义并初始化静态变量 bSlectIndexPre 为 0xFF，用于记录上一次选择的显示项索引
	INT16U wTemp; // 定义一个 16 位无符号整型变量 wTemp，用于临时存储数据

    // 检查当前选择的显示项索引是否与上一次不同
    if (bSlectIndexPre != bSlectIndex)
    {
        bSlectIndexPre = bSlectIndex; // 更新 bSlectIndexPre 为当前选择的显示项索引
        return; // 如果不同，退出函数，不执行后续代码
    }

    // 根据当前选择的显示项索引显示相应信息
    switch (bSlectIndex)
    {
        case SELECT_SOC_ID:
            wTemp = swGetSocNew(); // 获取新的 SOC（电池荷电状态），单位为 1%
            sDisplayBattSoc(wTemp); // 显示电池 SOC
            break;

        case SELECT_VOLT_ID:
            wTemp = (swGetBattVoltFiltNew() + 5) / 10; // 获取新的电池电压并进行转换，单位为 0.1V
            sDisplayBattVolt(wTemp); // 显示电池电压
            break;

        case SELECT_CURR_ID:
            wTemp = (swAbs(swGetBattCurrFiltNew()) + 5) / 10; // 获取新的电池电流并进行转换，单位为 1A
            sDisplayBattCurr(wTemp); // 显示电池电流
            break;

        case SELECT_CELL_01_VOLT_ID:
            sDisplayCellVolt(1, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH01)); // 显示第 1 节电池电压
            break;

        case SELECT_CELL_02_VOLT_ID:
            sDisplayCellVolt(2, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH02)); // 显示第 2 节电池电压
            break;

        case SELECT_CELL_03_VOLT_ID:
            sDisplayCellVolt(3, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH03)); // 显示第 3 节电池电压
            break;

        case SELECT_CELL_04_VOLT_ID:
            sDisplayCellVolt(4, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH04)); // 显示第 4 节电池电压
            break;

        case SELECT_CELL_05_VOLT_ID:
            sDisplayCellVolt(5, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH05)); // 显示第 5 节电池电压
            break;

        case SELECT_CELL_06_VOLT_ID:
            sDisplayCellVolt(6, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH06)); // 显示第 6 节电池电压
            break;

        case SELECT_CELL_07_VOLT_ID:
            sDisplayCellVolt(7, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH07)); // 显示第 7 节电池电压
            break;

        case SELECT_CELL_08_VOLT_ID:
            sDisplayCellVolt(8, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH08)); // 显示第 8 节电池电压
            break;

        case SELECT_CELL_09_VOLT_ID:
            sDisplayCellVolt(9, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH09)); // 显示第 9 节电池电压
            break;

        case SELECT_CELL_10_VOLT_ID:
            sDisplayCellVolt(10, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH10)); // 显示第 10 节电池电压
            break;

        case SELECT_CELL_11_VOLT_ID:
            sDisplayCellVolt(11, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH11)); // 显示第 11 节电池电压
            break;

        case SELECT_CELL_12_VOLT_ID:
            sDisplayCellVolt(12, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH12)); // 显示第 12 节电池电压
            break;

        case SELECT_CELL_13_VOLT_ID:
            sDisplayCellVolt(13, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH13)); // 显示第 13 节电池电压
            break;

        case SELECT_CELL_14_VOLT_ID:
            sDisplayCellVolt(14, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH14)); // 显示第 14 节电池电压
            break;

        case SELECT_CELL_15_VOLT_ID:
            sDisplayCellVolt(15, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH15)); // 显示第 15 节电池电压
            break;

        case SELECT_CELL_16_VOLT_ID:
            sDisplayCellVolt(16, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH16)); // 显示第 16 节电池电压
            break;

        case SELECT_CELL_01_TEMP_ID:
            sDisplayCellTemp(1, swGetCellTempFiltNew(cADC_CELL_TEMP_CH01)); // 显示第 1 节电池温度
            break;

        case SELECT_CELL_02_TEMP_ID:
            sDisplayCellTemp(2, swGetCellTempFiltNew(cADC_CELL_TEMP_CH02)); // 显示第 2 节电池温度
            break;

        case SELECT_CELL_03_TEMP_ID:
            sDisplayCellTemp(3, swGetCellTempFiltNew(cADC_CELL_TEMP_CH03)); // 显示第 3 节电池温度
            break;

        case SELECT_CELL_04_TEMP_ID:
            sDisplayCellTemp(4, swGetCellTempFiltNew(cADC_CELL_TEMP_CH04)); // 显示第 4 节电池温度
            break;

        case SELECT_RATED_AH_ID:
            wTemp = swGetBmsNominalAH() / 10; // 获取电池额定容量，单位为 0.1AH
            sDisplayRatedAH(wTemp); // 显示电池额定容量
            break;

        case SELECT_ADS_ID:
            wTemp = sbGetSciAddress(); // 获取 SCI（串行通信接口）地址
            sDisplayAddress(wTemp); // 显示 SCI 地址
            break;

        case SELECT_HW_VER_ID:
            wTemp = swGetHwVersion1Real() >> 2; // 获取硬件版本并右移 2 位
            sDisplayHwVersion(wTemp); // 显示硬件版本
            break;

        case SELECT_SW_VER_ID:
            wTemp = MACHINE_SW_VERSION; // 获取软件版本
            sDisplayHwVersion(wTemp); // 显示软件版本
            break;

        case SELECT_MOS_TEMP_ID:
            wTemp = swGetHsTempNew(); // 获取 MOS 管的温度，单位为 1℃
            sDisplayMosTemp(wTemp); // 显示 MOS 管温度
            break;

        default:
            break;
    }
}

// 显示故障代码
void sDisplayFaultCode(INT16U wCode)
{
    INT16U wTemp;
    
    wTemp = wCode; // 将故障代码存入临时变量 wTemp
    if (wTemp > 99)
    {
        wTemp = 99; // 如果故障代码大于 99，将其设为 99
    }
    sViewFigModule(cFIG_TYPE_7, FIG1_7SEG, charF, cALWAYS_LIGHT); // 设置显示模块
    sFigGroupDisplayNum(FIG2_7SEG, 2, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE); // 显示故障代码
}

// 显示电池荷电状态（SOC），单位为 1%
void sDisplayChgSoc(INT16U wSoc)
{
    static INT16U wSocCnt = cT6sec; // 定义并初始化静态变量 wSocCnt，初始值为 cT6sec，用于计时
    static INT16U wCntBias = 0; // 定义并初始化静态变量 wCntBias，用于存储偏移量
    INT16U wTemp;
    
    wTemp = wSoc; // 将 SOC 存入临时变量 wTemp
    if (++wSocCnt >= cT5sec) // 每经过一段时间（cT5sec）更新一次偏移量
    {
        if (wTemp < 25)
        {
            wCntBias = 0; // 如果 SOC 小于 25，偏移量为 0
        }
        else if (wTemp < 50)
        {
            wCntBias = cT1sec; // 如果 SOC 小于 50，偏移量为 cT1sec
        }
        else if (wTemp < 75)
        {
            wCntBias = cT2sec; // 如果 SOC 小于 75，偏移量为 cT2sec
        }
        else if (wTemp < 100)
        {
            wCntBias = cT3sec; // 如果 SOC 小于 100，偏移量为 cT3sec
        }
        else
        {
            wCntBias = cT4sec; // 如果 SOC 等于或大于 100，偏移量为 cT4sec
        }
        wSocCnt = wCntBias; // 更新计数器为偏移量
    }
    
    // 根据计数器值点亮不同的显示段
    if (wSocCnt < cT1sec)
    {
        // 不点亮任何显示段
    }
    else if (wSocCnt < cT2sec)
    {
        mSetVramBit(cBIT_SEG2A_PCT25, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2B_PCT25, cALWAYS_LIGHT);
    }
    else if (wSocCnt < cT3sec)
    {
        mSetVramBit(cBIT_SEG2A_PCT25, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2B_PCT25, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2C_PCT50, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2D_PCT50, cALWAYS_LIGHT);
    }
    else if (wSocCnt < cT4sec)
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
    
    // 点亮电池图标的所有段
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

// 显示电池放电状态（SOC），单位为 1%
void sDisplayDsgSoc(INT16U wSoc)
{
    INT16U wTemp;
    
    wTemp = wSoc; // 将 SOC 存入临时变量 wTemp
    
    // 根据 SOC 的值点亮相应的显示段
    if (wTemp == 0)
    {
        // 如果 SOC 为 0，不点亮任何显示段
    }
    else if (wTemp <= 25)
    {
        // 如果 SOC 小于或等于 25，点亮 25% 的显示段
        mSetVramBit(cBIT_SEG2A_PCT25, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2B_PCT25, cALWAYS_LIGHT);
    }
    else if (wTemp <= 50)
    {
        // 如果 SOC 小于或等于 50，点亮 50% 的显示段
        mSetVramBit(cBIT_SEG2A_PCT25, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2B_PCT25, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2C_PCT50, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2D_PCT50, cALWAYS_LIGHT);
    }
    else if (wTemp <= 75)
    {
        // 如果 SOC 小于或等于 75，点亮 75% 的显示段
        mSetVramBit(cBIT_SEG2A_PCT25, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2B_PCT25, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2C_PCT50, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2D_PCT50, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2E_PCT75, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2F_PCT75, cALWAYS_LIGHT);
    }
    else
    {
        // 如果 SOC 大于 75，点亮 100% 的显示段
        mSetVramBit(cBIT_SEG2A_PCT25, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2B_PCT25, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2C_PCT50, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2D_PCT50, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2E_PCT75, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2F_PCT75, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2G_PCT100, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG3A_PCT100, cALWAYS_LIGHT);
    }
    
    // 点亮电池图标的所有段
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

// 显示电池荷电状态（SOC），单位为 1%
void sDisplayBattSoc(INT16U wSoc)
{
    INT16U wTemp;
    
    wTemp = wSoc; // 将 SOC 值存入临时变量 wTemp
    if (wTemp >= 100)
    {
        // 如果 SOC 大于或等于 100，显示 100%
        sFigGroupDisplayNum(FIG1_7SEG, 3, 100, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
    }
    else
    {
        // 否则，显示 SOC 的值
        sFigGroupDisplayNum(FIG2_7SEG, 2, wTemp % 100, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
    }
}

extern INT8U bParallelErrChkCnt;

// 显示电池电压，单位为 0.1V
void sDisplayBattVolt(INT16S wVolt)
{
    INT16U wTemp;
    
    wTemp = wVolt; // 将电压值存入临时变量 wTemp
    if (wTemp < 10)
    {
        wTemp = 0; // 如果电压小于 1V，则显示为 0
    }
    else if (wTemp > 999)
    {
        wTemp = 999; // 如果电压大于 99.9V，则显示为 99.9V
    }
    if (wTemp != 0)
    {
        // 如果电压不为 0，则点亮小数点
        mSetVramBit(cBIT_SEG4G_DOT1, cALWAYS_LIGHT);
    }
    
    // 显示电压值
    // wTemp = bParallelErrChkCnt; // 这行代码被注释掉了，可以用于调试或其他用途
    sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
    
    // 点亮电压单位符号
    mSetVramBit(cBIT_SEG8B_VOLT, cALWAYS_LIGHT);
}

// 显示电池电流，单位为 1A
void sDisplayBattCurr(INT16S wCurr)
{
    INT16S wTemp;
    
    wTemp = wCurr; // 将电流值存入临时变量 wTemp
    if (wTemp > 999)
    {
        wTemp = 999; // 如果电流大于 999A，则显示为 999A
    }
    else if (wTemp < 0)
    {
        wTemp = 0; // 如果电流小于 0A，则显示为 0A
    }
    
    // 显示电流值
    sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
}

// 显示电池功率，单位为 0.01kW
void sDisplayBattWatt(INT16U wWatt)
{
    INT16U wTemp;
    
    wTemp = wWatt; // 将功率值存入临时变量 wTemp
    if (wTemp < 999)
    {
        // 如果功率小于 9.99kW，点亮第二个小数点
        mSetVramBit(cBIT_SEG4F_DOT2, cALWAYS_LIGHT);
        // 显示功率值
        sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
    }
    else
    {
        // 如果功率大于或等于 9.99kW，将功率值四舍五入并除以10，显示为kW
        wTemp = (wTemp + 5) / 10;
        // 点亮第一个小数点
        mSetVramBit(cBIT_SEG4G_DOT1, cALWAYS_LIGHT);
        // 显示功率值
        sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
    }
}

// 显示硬件版本号
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
	wTemp = wVer; // 将硬件版本号存入临时变量 wTemp
    if (wTemp > 999)
    {
        wTemp = 999; // 如果版本号大于 999，则显示为 999
    }
    // 显示版本号
    sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
}

// 显示软件版本号
void sDisplaySwVersion(INT16U wVer)
{
    INT16U wTemp;
    
    wTemp = wVer; // 将软件版本号存入临时变量 wTemp
    if (wTemp > 999)
    {
        wTemp = 999; // 如果版本号大于 999，则显示为 999
    }
    // 显示版本号
    sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
}

// 显示地址
void sDisplayAddress(INT16U wAds)
{
    INT16U wTemp;
    
    wTemp = wAds; // 将地址存入临时变量 wTemp
    if (wTemp > 32)
    {
        wTemp = 32; // 如果地址大于 32，则显示为 32
    }
    // 显示地址
    sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
}

// 显示额定容量，单位为 Ah
void sDisplayRatedAH(INT16U wAH)
{
    INT16U wTemp;
    
    wTemp = wAH; // 将额定容量存入临时变量 wTemp
    if (wTemp > 999)
    {
        wTemp = 999; // 如果额定容量大于 999Ah，则显示为 999Ah
    }
    // 显示额定容量
    sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
}

// 显示 MOS 温度，单位为 ℃
void sDisplayMosTemp(INT16S wTmp)
{
    INT16S wTemp;
    
    if (wTmp >= 0)
    {
        // 如果温度为正值
        wTemp = wTmp;
        if (wTemp > 999)
        {
            wTemp = 999; // 如果温度大于 999℃，则显示为 999℃
        }
        // 显示温度值
        sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
    }
    else
    {
        // 如果温度为负值
        wTemp = -wTmp;
        if (wTemp <= 9)
        {
            // 如果温度的绝对值小于等于 9℃，点亮负号，并显示温度值
            mSetVramBit(cBIT_SEG6G, cALWAYS_LIGHT); // 负号
            sFigGroupDisplayNum(FIG3_7SEG, 1, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
        }
        else
        {
            // 如果温度的绝对值大于 9℃
            if (wTemp > 99)
            {
                wTemp = 99; // 如果温度的绝对值大于 99℃，则显示为 99℃
            }
            // 点亮负号，并显示温度值
            mSetVramBit(cBIT_SEG5G, cALWAYS_LIGHT); // 负号
            sFigGroupDisplayNum(FIG2_7SEG, 2, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
        }
    }
    // 点亮温度单位符号
    mSetVramBit(cBIT_SEG8A_TEMPC, cALWAYS_LIGHT);
}

// 显示单体电芯电压
void sDisplayCellVolt(INT8U bNo, INT16S wVolt)
{
    INT16U wTemp;
    
    // 电芯电压
    wTemp = (swAbs(wVolt) + 5) / 10; // 将电压取绝对值并四舍五入到最接近的 0.1V
    if (wTemp > 999)
    {
        wTemp = 999; // 如果电压大于 99.9V，则显示为 99.9V
    }
    // 点亮小数点
    mSetVramBit(cBIT_SEG4F_DOT2, cALWAYS_LIGHT);
    // 显示电压值
    sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
    // 点亮电压单位符号
    mSetVramBit(cBIT_SEG8B_VOLT, cALWAYS_LIGHT);
}

// 显示单体温感温度
void sDisplayCellTemp(INT8U bNo, INT16S wTmp)
{
    INT16S wTemp;
    
    // 温感温度
    wTemp = wTmp;
    if (wTemp >= 0)
    {
        // 正温度值处理
        if (wTemp > 999)
        {
            wTemp = 999; // 如果温度大于 999℃，则显示为 999℃
        }
        // 显示温度值
        sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
    }
    else
    {
        // 负温度值处理
        if (wTemp <= -10)
        {
            // 温度小于 -10℃
            if (wTemp < -99)
            {
                wTemp = -99; // 如果温度小于 -99℃，则显示为 -99℃
            }
            // 点亮负号，并显示温度值
            mSetVramBit(cBIT_SEG5G, cALWAYS_LIGHT); // 负号
            sFigGroupDisplayNum(FIG2_7SEG, 2, -wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
        }
        else
        {
            // 温度大于 -10℃
            mSetVramBit(cBIT_SEG6G, cALWAYS_LIGHT); // 负号
            sFigGroupDisplayNum(FIG3_7SEG, 1, -wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
        }
    }
    // 点亮温度单位符号
    mSetVramBit(cBIT_SEG8A_TEMPC, cALWAYS_LIGHT);
}

// 显示 SCI 地址
void sDisplaySciAddr(void)
{
    // 显示 'A' 字符
    sViewFigModule(cFIG_TYPE_7, FIG1_7SEG, charA, cALWAYS_LIGHT);
    // 显示 SCI 地址
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


