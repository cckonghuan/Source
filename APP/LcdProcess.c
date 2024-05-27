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
INT8U	bIsAdjustVal;	// 0:δ��ʼ���ڲ���,1:���ڲ�����,2:�����������
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
// LCD ��ʼ������
void sLcdInitial(void)
{
    wModeTimeOut = 0;              // ģʽ��ʱʱ��
    bSlectIndex = 0;               // ѡ������
    bMenuIndex = 0;                // �˵�����
    bConfigMode = 0;               // ����ģʽ��־
    bIsSelConfigItem = 0;          // �Ƿ�ѡ���������־
    bIsAdjustVal = 0;              // �Ƿ����ֵ��־
    wAdjustValue = 0;              // ����ֵ
    wLCDLoopCnt = LCD_LOOP_CNT;    // LCD ѭ��������
    wLCDBackLightCnt = LCD_BL_TIME;// LCD �����ʱ��
    wLcdInitSec = 65535;           // LCD ��ʼ����ʱ
}

// LCD ������
void sLcdTask(INT16U wFilter)
{
    static INT16U wCnt = cLCD_UPDATE_RATE;  // ����������ʼ��Ϊ LCD ��������
    static INT16U wNormalPage = cLCD_PAGE_HOME;  // ����ҳ�棬��ʼ��Ϊ��ҳ

    // �����ʼ��
    if(sbGetKeyStatusLCDMode() == true)
    {
        if(wLCDBackLightCnt == 0)
        {
            wCnt = cLCD_UPDATE_RATE;  // ���ü�����
            wLCDBackLightCnt = LCD_BL_TIME;  // ���ñ����ʱ��
        }

        wNormalPage++;  // �л�����һ��ҳ��
        if(wNormalPage == cLCD_PAGE_BLOFF)
        {
            wLCDBackLightCnt = 0;  // �رձ���
        }
        else if(wNormalPage > cLCD_PAGE_BLOFF)
        {
            wNormalPage = cLCD_PAGE_HOME;  // ������ҳ
        }
        sSetKeyStatusLCDMode(false);  // ���ü�״̬
    }

    wCnt += wFilter;  // ���¼�����
    if(++wCnt < cLCD_UPDATE_RATE)
    {
        return;  // ���������δ�ﵽ�������ʣ�����
    }
    wCnt = 0;  // ���ü�����

    // ���� VRAM ����
    sLcdVramAreaClear();
    if((wLcdInitSec != 0) && (bBmsMode == cPowerOnMode))
    {
        sInitDisplay();  // ��ʾ��ʼ������
    }
    else if(bBmsMode == cShutdownMode)
    {
        sBackLightOff();  // �رձ���
    }
    else if(sbGetKeyStatusDebugMode())
    {
        sProcSelectMode();  // ����ѡ��ģʽ
        sLoopDisplay();     // ѭ����ʾ
        sStatusDisplay();   // ״̬��ʾ
    }
    else
    {
        if(wNormalPage == cLCD_PAGE_SCI)
        {
            sDisplaySciAddr();  // ��ʾ SCI ��ַ
        }
        else
        {
            sHomeDisplay();  // ��ʾ��ҳ
        }
        sStatusDisplay();  // ״̬��ʾ
    }

    if(wLCDLoopCnt == 0)
    {
        sSetKeyStatusDebugMode(false);  // �˳�����ģʽ
    }

    // LED �������
    if(wLCDBackLightCnt > 0)
    {
        sBackLightOn();  // �򿪱���
        wLCDBackLightCnt--;
    }
    else if(swGetEepromBackLightEn() == 0)
    {
        sBackLightOff();  // �رձ���
    }

    // ˢ�� LCD
    sLcdHwInit();  // Ӳ����ʼ��
    sLcdUpdateData();  // ��������
    sLcdUpdate();  // ������ʾ
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
	
	// ���ͼ��
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
	
	// ����ͼ��
	if(sbGetKeyStatusDebugMode())
	{
		mSetVramBit(cBIT_SEG4D_WARN, cALWAYS_LIGHT);
	}
	
	// ����/�澯ͼ��
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
	static INT8U bSlectIndexPre = 0xFF; // ���岢��ʼ����̬���� bSlectIndexPre Ϊ 0xFF�����ڼ�¼��һ��ѡ�����ʾ������
	INT16U wTemp; // ����һ�� 16 λ�޷������ͱ��� wTemp��������ʱ�洢����

    // ��鵱ǰѡ�����ʾ�������Ƿ�����һ�β�ͬ
    if (bSlectIndexPre != bSlectIndex)
    {
        bSlectIndexPre = bSlectIndex; // ���� bSlectIndexPre Ϊ��ǰѡ�����ʾ������
        return; // �����ͬ���˳���������ִ�к�������
    }

    // ���ݵ�ǰѡ�����ʾ��������ʾ��Ӧ��Ϣ
    switch (bSlectIndex)
    {
        case SELECT_SOC_ID:
            wTemp = swGetSocNew(); // ��ȡ�µ� SOC����غɵ�״̬������λΪ 1%
            sDisplayBattSoc(wTemp); // ��ʾ��� SOC
            break;

        case SELECT_VOLT_ID:
            wTemp = (swGetBattVoltFiltNew() + 5) / 10; // ��ȡ�µĵ�ص�ѹ������ת������λΪ 0.1V
            sDisplayBattVolt(wTemp); // ��ʾ��ص�ѹ
            break;

        case SELECT_CURR_ID:
            wTemp = (swAbs(swGetBattCurrFiltNew()) + 5) / 10; // ��ȡ�µĵ�ص���������ת������λΪ 1A
            sDisplayBattCurr(wTemp); // ��ʾ��ص���
            break;

        case SELECT_CELL_01_VOLT_ID:
            sDisplayCellVolt(1, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH01)); // ��ʾ�� 1 �ڵ�ص�ѹ
            break;

        case SELECT_CELL_02_VOLT_ID:
            sDisplayCellVolt(2, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH02)); // ��ʾ�� 2 �ڵ�ص�ѹ
            break;

        case SELECT_CELL_03_VOLT_ID:
            sDisplayCellVolt(3, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH03)); // ��ʾ�� 3 �ڵ�ص�ѹ
            break;

        case SELECT_CELL_04_VOLT_ID:
            sDisplayCellVolt(4, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH04)); // ��ʾ�� 4 �ڵ�ص�ѹ
            break;

        case SELECT_CELL_05_VOLT_ID:
            sDisplayCellVolt(5, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH05)); // ��ʾ�� 5 �ڵ�ص�ѹ
            break;

        case SELECT_CELL_06_VOLT_ID:
            sDisplayCellVolt(6, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH06)); // ��ʾ�� 6 �ڵ�ص�ѹ
            break;

        case SELECT_CELL_07_VOLT_ID:
            sDisplayCellVolt(7, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH07)); // ��ʾ�� 7 �ڵ�ص�ѹ
            break;

        case SELECT_CELL_08_VOLT_ID:
            sDisplayCellVolt(8, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH08)); // ��ʾ�� 8 �ڵ�ص�ѹ
            break;

        case SELECT_CELL_09_VOLT_ID:
            sDisplayCellVolt(9, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH09)); // ��ʾ�� 9 �ڵ�ص�ѹ
            break;

        case SELECT_CELL_10_VOLT_ID:
            sDisplayCellVolt(10, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH10)); // ��ʾ�� 10 �ڵ�ص�ѹ
            break;

        case SELECT_CELL_11_VOLT_ID:
            sDisplayCellVolt(11, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH11)); // ��ʾ�� 11 �ڵ�ص�ѹ
            break;

        case SELECT_CELL_12_VOLT_ID:
            sDisplayCellVolt(12, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH12)); // ��ʾ�� 12 �ڵ�ص�ѹ
            break;

        case SELECT_CELL_13_VOLT_ID:
            sDisplayCellVolt(13, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH13)); // ��ʾ�� 13 �ڵ�ص�ѹ
            break;

        case SELECT_CELL_14_VOLT_ID:
            sDisplayCellVolt(14, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH14)); // ��ʾ�� 14 �ڵ�ص�ѹ
            break;

        case SELECT_CELL_15_VOLT_ID:
            sDisplayCellVolt(15, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH15)); // ��ʾ�� 15 �ڵ�ص�ѹ
            break;

        case SELECT_CELL_16_VOLT_ID:
            sDisplayCellVolt(16, swGetCellVoltFiltNew(cADC_CELL_VOLT_CH16)); // ��ʾ�� 16 �ڵ�ص�ѹ
            break;

        case SELECT_CELL_01_TEMP_ID:
            sDisplayCellTemp(1, swGetCellTempFiltNew(cADC_CELL_TEMP_CH01)); // ��ʾ�� 1 �ڵ���¶�
            break;

        case SELECT_CELL_02_TEMP_ID:
            sDisplayCellTemp(2, swGetCellTempFiltNew(cADC_CELL_TEMP_CH02)); // ��ʾ�� 2 �ڵ���¶�
            break;

        case SELECT_CELL_03_TEMP_ID:
            sDisplayCellTemp(3, swGetCellTempFiltNew(cADC_CELL_TEMP_CH03)); // ��ʾ�� 3 �ڵ���¶�
            break;

        case SELECT_CELL_04_TEMP_ID:
            sDisplayCellTemp(4, swGetCellTempFiltNew(cADC_CELL_TEMP_CH04)); // ��ʾ�� 4 �ڵ���¶�
            break;

        case SELECT_RATED_AH_ID:
            wTemp = swGetBmsNominalAH() / 10; // ��ȡ��ض��������λΪ 0.1AH
            sDisplayRatedAH(wTemp); // ��ʾ��ض����
            break;

        case SELECT_ADS_ID:
            wTemp = sbGetSciAddress(); // ��ȡ SCI������ͨ�Žӿڣ���ַ
            sDisplayAddress(wTemp); // ��ʾ SCI ��ַ
            break;

        case SELECT_HW_VER_ID:
            wTemp = swGetHwVersion1Real() >> 2; // ��ȡӲ���汾������ 2 λ
            sDisplayHwVersion(wTemp); // ��ʾӲ���汾
            break;

        case SELECT_SW_VER_ID:
            wTemp = MACHINE_SW_VERSION; // ��ȡ����汾
            sDisplayHwVersion(wTemp); // ��ʾ����汾
            break;

        case SELECT_MOS_TEMP_ID:
            wTemp = swGetHsTempNew(); // ��ȡ MOS �ܵ��¶ȣ���λΪ 1��
            sDisplayMosTemp(wTemp); // ��ʾ MOS ���¶�
            break;

        default:
            break;
    }
}

// ��ʾ���ϴ���
void sDisplayFaultCode(INT16U wCode)
{
    INT16U wTemp;
    
    wTemp = wCode; // �����ϴ��������ʱ���� wTemp
    if (wTemp > 99)
    {
        wTemp = 99; // ������ϴ������ 99��������Ϊ 99
    }
    sViewFigModule(cFIG_TYPE_7, FIG1_7SEG, charF, cALWAYS_LIGHT); // ������ʾģ��
    sFigGroupDisplayNum(FIG2_7SEG, 2, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE); // ��ʾ���ϴ���
}

// ��ʾ��غɵ�״̬��SOC������λΪ 1%
void sDisplayChgSoc(INT16U wSoc)
{
    static INT16U wSocCnt = cT6sec; // ���岢��ʼ����̬���� wSocCnt����ʼֵΪ cT6sec�����ڼ�ʱ
    static INT16U wCntBias = 0; // ���岢��ʼ����̬���� wCntBias�����ڴ洢ƫ����
    INT16U wTemp;
    
    wTemp = wSoc; // �� SOC ������ʱ���� wTemp
    if (++wSocCnt >= cT5sec) // ÿ����һ��ʱ�䣨cT5sec������һ��ƫ����
    {
        if (wTemp < 25)
        {
            wCntBias = 0; // ��� SOC С�� 25��ƫ����Ϊ 0
        }
        else if (wTemp < 50)
        {
            wCntBias = cT1sec; // ��� SOC С�� 50��ƫ����Ϊ cT1sec
        }
        else if (wTemp < 75)
        {
            wCntBias = cT2sec; // ��� SOC С�� 75��ƫ����Ϊ cT2sec
        }
        else if (wTemp < 100)
        {
            wCntBias = cT3sec; // ��� SOC С�� 100��ƫ����Ϊ cT3sec
        }
        else
        {
            wCntBias = cT4sec; // ��� SOC ���ڻ���� 100��ƫ����Ϊ cT4sec
        }
        wSocCnt = wCntBias; // ���¼�����Ϊƫ����
    }
    
    // ���ݼ�����ֵ������ͬ����ʾ��
    if (wSocCnt < cT1sec)
    {
        // �������κ���ʾ��
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
    
    // �������ͼ������ж�
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

// ��ʾ��طŵ�״̬��SOC������λΪ 1%
void sDisplayDsgSoc(INT16U wSoc)
{
    INT16U wTemp;
    
    wTemp = wSoc; // �� SOC ������ʱ���� wTemp
    
    // ���� SOC ��ֵ������Ӧ����ʾ��
    if (wTemp == 0)
    {
        // ��� SOC Ϊ 0���������κ���ʾ��
    }
    else if (wTemp <= 25)
    {
        // ��� SOC С�ڻ���� 25������ 25% ����ʾ��
        mSetVramBit(cBIT_SEG2A_PCT25, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2B_PCT25, cALWAYS_LIGHT);
    }
    else if (wTemp <= 50)
    {
        // ��� SOC С�ڻ���� 50������ 50% ����ʾ��
        mSetVramBit(cBIT_SEG2A_PCT25, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2B_PCT25, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2C_PCT50, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2D_PCT50, cALWAYS_LIGHT);
    }
    else if (wTemp <= 75)
    {
        // ��� SOC С�ڻ���� 75������ 75% ����ʾ��
        mSetVramBit(cBIT_SEG2A_PCT25, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2B_PCT25, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2C_PCT50, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2D_PCT50, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2E_PCT75, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2F_PCT75, cALWAYS_LIGHT);
    }
    else
    {
        // ��� SOC ���� 75������ 100% ����ʾ��
        mSetVramBit(cBIT_SEG2A_PCT25, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2B_PCT25, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2C_PCT50, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2D_PCT50, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2E_PCT75, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2F_PCT75, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG2G_PCT100, cALWAYS_LIGHT);
        mSetVramBit(cBIT_SEG3A_PCT100, cALWAYS_LIGHT);
    }
    
    // �������ͼ������ж�
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

// ��ʾ��غɵ�״̬��SOC������λΪ 1%
void sDisplayBattSoc(INT16U wSoc)
{
    INT16U wTemp;
    
    wTemp = wSoc; // �� SOC ֵ������ʱ���� wTemp
    if (wTemp >= 100)
    {
        // ��� SOC ���ڻ���� 100����ʾ 100%
        sFigGroupDisplayNum(FIG1_7SEG, 3, 100, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
    }
    else
    {
        // ������ʾ SOC ��ֵ
        sFigGroupDisplayNum(FIG2_7SEG, 2, wTemp % 100, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
    }
}

extern INT8U bParallelErrChkCnt;

// ��ʾ��ص�ѹ����λΪ 0.1V
void sDisplayBattVolt(INT16S wVolt)
{
    INT16U wTemp;
    
    wTemp = wVolt; // ����ѹֵ������ʱ���� wTemp
    if (wTemp < 10)
    {
        wTemp = 0; // �����ѹС�� 1V������ʾΪ 0
    }
    else if (wTemp > 999)
    {
        wTemp = 999; // �����ѹ���� 99.9V������ʾΪ 99.9V
    }
    if (wTemp != 0)
    {
        // �����ѹ��Ϊ 0�������С����
        mSetVramBit(cBIT_SEG4G_DOT1, cALWAYS_LIGHT);
    }
    
    // ��ʾ��ѹֵ
    // wTemp = bParallelErrChkCnt; // ���д��뱻ע�͵��ˣ��������ڵ��Ի�������;
    sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
    
    // ������ѹ��λ����
    mSetVramBit(cBIT_SEG8B_VOLT, cALWAYS_LIGHT);
}

// ��ʾ��ص�������λΪ 1A
void sDisplayBattCurr(INT16S wCurr)
{
    INT16S wTemp;
    
    wTemp = wCurr; // ������ֵ������ʱ���� wTemp
    if (wTemp > 999)
    {
        wTemp = 999; // ����������� 999A������ʾΪ 999A
    }
    else if (wTemp < 0)
    {
        wTemp = 0; // �������С�� 0A������ʾΪ 0A
    }
    
    // ��ʾ����ֵ
    sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
}

// ��ʾ��ع��ʣ���λΪ 0.01kW
void sDisplayBattWatt(INT16U wWatt)
{
    INT16U wTemp;
    
    wTemp = wWatt; // ������ֵ������ʱ���� wTemp
    if (wTemp < 999)
    {
        // �������С�� 9.99kW�������ڶ���С����
        mSetVramBit(cBIT_SEG4F_DOT2, cALWAYS_LIGHT);
        // ��ʾ����ֵ
        sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
    }
    else
    {
        // ������ʴ��ڻ���� 9.99kW��������ֵ�������벢����10����ʾΪkW
        wTemp = (wTemp + 5) / 10;
        // ������һ��С����
        mSetVramBit(cBIT_SEG4G_DOT1, cALWAYS_LIGHT);
        // ��ʾ����ֵ
        sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
    }
}

// ��ʾӲ���汾��
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
	wTemp = wVer; // ��Ӳ���汾�Ŵ�����ʱ���� wTemp
    if (wTemp > 999)
    {
        wTemp = 999; // ����汾�Ŵ��� 999������ʾΪ 999
    }
    // ��ʾ�汾��
    sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
}

// ��ʾ����汾��
void sDisplaySwVersion(INT16U wVer)
{
    INT16U wTemp;
    
    wTemp = wVer; // ������汾�Ŵ�����ʱ���� wTemp
    if (wTemp > 999)
    {
        wTemp = 999; // ����汾�Ŵ��� 999������ʾΪ 999
    }
    // ��ʾ�汾��
    sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
}

// ��ʾ��ַ
void sDisplayAddress(INT16U wAds)
{
    INT16U wTemp;
    
    wTemp = wAds; // ����ַ������ʱ���� wTemp
    if (wTemp > 32)
    {
        wTemp = 32; // �����ַ���� 32������ʾΪ 32
    }
    // ��ʾ��ַ
    sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
}

// ��ʾ���������λΪ Ah
void sDisplayRatedAH(INT16U wAH)
{
    INT16U wTemp;
    
    wTemp = wAH; // �������������ʱ���� wTemp
    if (wTemp > 999)
    {
        wTemp = 999; // ������������ 999Ah������ʾΪ 999Ah
    }
    // ��ʾ�����
    sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
}

// ��ʾ MOS �¶ȣ���λΪ ��
void sDisplayMosTemp(INT16S wTmp)
{
    INT16S wTemp;
    
    if (wTmp >= 0)
    {
        // ����¶�Ϊ��ֵ
        wTemp = wTmp;
        if (wTemp > 999)
        {
            wTemp = 999; // ����¶ȴ��� 999�棬����ʾΪ 999��
        }
        // ��ʾ�¶�ֵ
        sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
    }
    else
    {
        // ����¶�Ϊ��ֵ
        wTemp = -wTmp;
        if (wTemp <= 9)
        {
            // ����¶ȵľ���ֵС�ڵ��� 9�棬�������ţ�����ʾ�¶�ֵ
            mSetVramBit(cBIT_SEG6G, cALWAYS_LIGHT); // ����
            sFigGroupDisplayNum(FIG3_7SEG, 1, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
        }
        else
        {
            // ����¶ȵľ���ֵ���� 9��
            if (wTemp > 99)
            {
                wTemp = 99; // ����¶ȵľ���ֵ���� 99�棬����ʾΪ 99��
            }
            // �������ţ�����ʾ�¶�ֵ
            mSetVramBit(cBIT_SEG5G, cALWAYS_LIGHT); // ����
            sFigGroupDisplayNum(FIG2_7SEG, 2, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
        }
    }
    // �����¶ȵ�λ����
    mSetVramBit(cBIT_SEG8A_TEMPC, cALWAYS_LIGHT);
}

// ��ʾ�����о��ѹ
void sDisplayCellVolt(INT8U bNo, INT16S wVolt)
{
    INT16U wTemp;
    
    // ��о��ѹ
    wTemp = (swAbs(wVolt) + 5) / 10; // ����ѹȡ����ֵ���������뵽��ӽ��� 0.1V
    if (wTemp > 999)
    {
        wTemp = 999; // �����ѹ���� 99.9V������ʾΪ 99.9V
    }
    // ����С����
    mSetVramBit(cBIT_SEG4F_DOT2, cALWAYS_LIGHT);
    // ��ʾ��ѹֵ
    sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
    // ������ѹ��λ����
    mSetVramBit(cBIT_SEG8B_VOLT, cALWAYS_LIGHT);
}

// ��ʾ�����¸��¶�
void sDisplayCellTemp(INT8U bNo, INT16S wTmp)
{
    INT16S wTemp;
    
    // �¸��¶�
    wTemp = wTmp;
    if (wTemp >= 0)
    {
        // ���¶�ֵ����
        if (wTemp > 999)
        {
            wTemp = 999; // ����¶ȴ��� 999�棬����ʾΪ 999��
        }
        // ��ʾ�¶�ֵ
        sFigGroupDisplayNum(FIG1_7SEG, 3, wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_DISABLE);
    }
    else
    {
        // ���¶�ֵ����
        if (wTemp <= -10)
        {
            // �¶�С�� -10��
            if (wTemp < -99)
            {
                wTemp = -99; // ����¶�С�� -99�棬����ʾΪ -99��
            }
            // �������ţ�����ʾ�¶�ֵ
            mSetVramBit(cBIT_SEG5G, cALWAYS_LIGHT); // ����
            sFigGroupDisplayNum(FIG2_7SEG, 2, -wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
        }
        else
        {
            // �¶ȴ��� -10��
            mSetVramBit(cBIT_SEG6G, cALWAYS_LIGHT); // ����
            sFigGroupDisplayNum(FIG3_7SEG, 1, -wTemp, cALWAYS_LIGHT, cFIG_ALL_DISP_ENABLE);
        }
    }
    // �����¶ȵ�λ����
    mSetVramBit(cBIT_SEG8A_TEMPC, cALWAYS_LIGHT);
}

// ��ʾ SCI ��ַ
void sDisplaySciAddr(void)
{
    // ��ʾ 'A' �ַ�
    sViewFigModule(cFIG_TYPE_7, FIG1_7SEG, charA, cALWAYS_LIGHT);
    // ��ʾ SCI ��ַ
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


