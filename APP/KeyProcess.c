/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		KeyProcess.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define			__KEY_PROCESS_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"KeyProcess.h"
#include		"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define		cKEY_PRESS_TURNOFF_BIT			0x0001
#define		cKEY_PRESS_FORCEON_BIT			0x0002
#define		cKEY_PRESS_DEBUG_BIT			0x0004
#define		cKEY_PRESS_CONFIG_BIT			0x0008
#define		cKEY_PRESS_ADJUST_BIT			0x0010
#define		cKEY_PRESS_ENTER_BIT			0x0020

#define		cKEY_BIT16_COUNT_MAX			0xFFFF

#define		cKEY_SCAN_READY					0
#define		cKEY_SCAN_PRESS					1
#define		cKEY_SCAN_STOP					2

#define 	cMODBUS_ADDR_ERR				0x00	// 16


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
INT8U	bKeyAddress;
INT8U	bSciAddress;
INT16U	wKeyVal;

/********************************************************************************
* Internal variables															*
********************************************************************************/
union KEY_STATUS_TABLE
{
	struct KEY_STATUS_BIT_FEILD
	{
		// BIT0-3
		INT16U ForcedOn:1;		// 强制开机,允许无电压/电流条件限制电池模式5分钟
		INT16U DebugMode:1;		// LCD处于DEBUG模式
		INT16U ConfigMode:1;	// LCD处于DEBUG中配置模式
		INT16U PressTrig:1;		// 存在按键
		
		// BIT4-7
		INT16U LCDMode:1;		// LCD按键动作
		INT16U rsvd2:3;
		
		// BIT8-11
		INT16U rsvd3:4;
		
		// BIT12-15
		INT16U rsvd4:4;
	} bits;
	INT16U data;
} fKeyStatus;

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
INT8U	sbKeyAddressRead(void);
void	sSetKeyCode(INT16U wKeyCodeBit);
void	sClrKeyCode(INT16U wKeyCodeBit);

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void sKeyProcessInit(void)
{
    INT8U temp;

    // 读取按键地址
    bKeyAddress = sbKeyAddressRead();

    // 如果读取到的按键地址为0，则设置为错误地址
    temp = bKeyAddress;
    if(temp == 0)
    {
        temp = cMODBUS_ADDR_ERR;
    }
    // 设置SCI地址
    sSetSciAddress(temp);
    // 设置CAN地址
    sSetCanAddress(bKeyAddress);

    // 初始化按键值
    wKeyVal = 0;
}

void sKeyPWRScanProc(INT16U wOffCnt, INT16U wForceCnt)
{
    static INT8U key_scan_mode = cKEY_SCAN_READY; // 按键扫描模式
    static INT8U bReleaseFlag = false; // 按键开机后是否释放标志
    static INT16U wPressCnt = 0; // 按键按下计数器

    switch(key_scan_mode)
    {
        case cKEY_SCAN_READY: // 准备扫描
            wPressCnt = 0;
            if(mIS_KEY_PRESSED()) // 检查按键是否按下
            {
                wPressCnt++;
                sSetKeyStatusPressTrig(true); // 设置按键按下触发状态
                key_scan_mode = cKEY_SCAN_PRESS; // 切换到按键按下模式
            }
            else
            {
                bReleaseFlag = true; // 按键已释放
            }
        break;
        case cKEY_SCAN_PRESS: // 按键按下模式
            if(mIS_KEY_PRESSED()) // 按键保持按下状态
            {
                if(wPressCnt < cKEY_BIT16_COUNT_MAX) // 如果按键计数未到最大值，继续计数
                {
                    wPressCnt++;
                }

                if(bReleaseFlag == false) // 如果按键未释放
                {
                    if(wPressCnt >= wForceCnt) // 如果按键按下计数超过强制开机计数
                    {
                        sSetKeyCode(cKEY_PRESS_FORCEON_BIT); // 设置按键强制开机码
                        key_scan_mode = cKEY_SCAN_STOP; // 切换到按键停止模式
                    }
                }
            }
            else // 按键已释放
            {
                if(wPressCnt >= wOffCnt) // 如果按键按下计数超过关机计数
                {
                    sSetKeyCode(cKEY_PRESS_TURNOFF_BIT); // 设置按键关机码
                }
                key_scan_mode = cKEY_SCAN_STOP; // 切换到按键停止模式
            }
        break;
        case cKEY_SCAN_STOP: // 按键停止模式
            if(mIS_KEY_RELEASE()) // 如果按键已释放
            {
                key_scan_mode = cKEY_SCAN_READY; // 切换到准备扫描模式
            }
        break;
        default:
            key_scan_mode = cKEY_SCAN_READY; // 默认切换到准备扫描模式
        break;
    }
}

void sKeyLCDScanProc(INT16U wOffCnt, INT16U wDbgCnt)
{
    static INT8U key_scan_mode = cKEY_SCAN_READY; // 按键扫描模式
    static INT16U wPressCnt = 0; // 按键按下计数器

    switch(key_scan_mode)
    {
        case cKEY_SCAN_READY: // 准备扫描模式
            wPressCnt = 0;
            if(mIS_LCD_KEY2_PRESSED()) // 检查LCD按键2是否按下
            {
                wPressCnt++;
                key_scan_mode = cKEY_SCAN_PRESS; // 切换到按键按下模式
            }
        break;
        case cKEY_SCAN_PRESS: // 按键按下模式
            if(mIS_LCD_KEY2_PRESSED()) // 按键保持按下状态
            {
                if(wPressCnt < cKEY_BIT16_COUNT_MAX) // 如果按键计数未到最大值，继续计数
                {
                    wPressCnt++;
                }

                if(wPressCnt >= wDbgCnt) // 如果按键按下计数超过调试计数
                {
                    wPressCnt = 0;
                    sRstLCDLoopCnt(); // 重置LCD循环计数
                    sSetKeyStatusDebugMode(true); // 设置按键调试模式状态
                    key_scan_mode = cKEY_SCAN_STOP; // 切换到按键停止模式
                }
            }
            else // 按键已释放
            {
                key_scan_mode = cKEY_SCAN_STOP; // 切换到按键停止模式
            }
        break;
        case cKEY_SCAN_STOP: // 按键停止模式
            if(mIS_LCD_KEY2_RELEASE()) // 如果按键已释放
            {
                if(wPressCnt >= wOffCnt) // 如果按键按下计数超过关机计数
                {
                    wPressCnt = 0;
                    sSetKeyStatusLCDMode(true); // 设置按键LCD模式状态
                }
                key_scan_mode = cKEY_SCAN_READY; // 切换到准备扫描模式
            }
        break;
        default:
            key_scan_mode = cKEY_SCAN_READY; // 默认切换到准备扫描模式
        break;
    }
}

void sKeyForceOnTimeout(INT16U wTimeOut)
{
    static INT16U wChkCnt = 0; // 检查计数器

    if(sbGetKeyStatusForcedOn()) // 检查按键是否强制开机
    {
        if(++wChkCnt >= wTimeOut) // 如果计数器超过超时时间
        {
            wChkCnt = 0;
            sSetKeyStatusForcedOn(false); // 取消强制开机状态
        }
    }
    else
    {
        if((swGetKeyCode() & cKEY_PRESS_FORCEON_BIT) != 0) // 检查按键码是否包含强制开机位
        {
            wChkCnt = 0;
            sSetKeyStatusForcedOn(true); // 设置强制开机状态
            sClrKeyCode(cKEY_PRESS_FORCEON_BIT); // 清除按键码中的强制开机位
        }
    }
}
void sKeyTurnOffProc(void)
{
	// 检查关机按键是否被按下
	if((swGetKeyCode() & cKEY_PRESS_TURNOFF_BIT) != 0)
	{
		// 关机按键被按下，设置电池管理系统（BMS）为并联关机模式
		sSetBmsParallelShutdown(true);

		// 向优先级为 cPrioSuper 的任务发送关机事件 eSuperToShutdown
		OSEventSend(cPrioSuper, eSuperToShutdown);

		// 清除关机按键的标志位
		sClrKeyCode(cKEY_PRESS_TURNOFF_BIT);
	}
}

INT8U	sbKeyAddressRead(void)
{
	INT8U	bTemp = 0;
	/*
	if(!mADD_SW4_STATUS())
	{
		bTemp += 0x8;
	}
	if(!mADD_SW3_STATUS())
	{
		bTemp += 0x4;
	}
	if(!mADD_SW2_STATUS())
	{
		bTemp += 0x2;
	}
	if(!mADD_SW1_STATUS())
	{
		bTemp += 0x1;
	}
	
	while(bTemp >= cBMS_DEVICE_MAX)
	{
		;
	}
	*/
	return bTemp;
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
INT8U	sbGetKeyStatusForcedOn(void)
{
	return fKeyStatus.bits.ForcedOn;
}

INT8U	sbGetKeyStatusDebugMode(void)
{
	return fKeyStatus.bits.DebugMode;
}

INT8U	sbGetKeyStatusConfigMode(void)
{
	return fKeyStatus.bits.ConfigMode;
}

INT8U	sbGetKeyStatusPressTrig(void)
{
	return fKeyStatus.bits.PressTrig;
}

INT8U	sbGetKeyStatusLCDMode(void)
{
	return fKeyStatus.bits.LCDMode;
}

INT8U	sbGetKeyAddress(void)
{
	return bKeyAddress;
}

INT8U	sbGetSciAddress(void)
{
	return bSciAddress;
}

void	sSetSciAddress(INT8U bAddr)
{
	bSciAddress = bAddr;
}

INT16U	swGetKeyCode(void)
{
	return(wKeyVal);	
}

INT8U	sbGetKeyPressTurnOff(void)
{
	if((wKeyVal & cKEY_PRESS_TURNOFF_BIT) != 0)
	{
		return true;
	}
	return false;
}

INT8U	sbGetKeyPressForceOn(void)
{
	if((wKeyVal & cKEY_PRESS_FORCEON_BIT) != 0)
	{
		return true;
	}
	return false;
}

INT8U	sbGetKeyPressDebug(void)
{
	if((wKeyVal & cKEY_PRESS_DEBUG_BIT) != 0)
	{
		return true;
	}
	return false;
}

INT8U	sbGetKeyPressConfig(void)
{
	if((wKeyVal & cKEY_PRESS_CONFIG_BIT) != 0)
	{
		return true;
	}
	return false;
}

INT8U	sbGetKeyPressAdjust(void)
{
	if((wKeyVal & cKEY_PRESS_ADJUST_BIT) != 0)
	{
		return true;
	}
	return false;
}

INT8U	sbGetKeyPressEnter(void)
{
	if((wKeyVal & cKEY_PRESS_ENTER_BIT) != 0)
	{
		return true;
	}
	return false;
}

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
void	sSetKeyStatusForcedOn(INT8U bStatus)
{
	fKeyStatus.bits.ForcedOn = bStatus;
}

void	sSetKeyStatusDebugMode(INT8U bStatus)
{
	fKeyStatus.bits.DebugMode = bStatus;
}

void	sSetKeyStatusConfigMode(INT8U bStatus)
{
	fKeyStatus.bits.ConfigMode = bStatus;
}

void	sSetKeyStatusPressTrig(INT8U bStatus)
{
	fKeyStatus.bits.PressTrig = bStatus;
}

void	sSetKeyStatusLCDMode(INT8U bStatus)
{
	fKeyStatus.bits.LCDMode = bStatus;
}

void	sSetKeyCode(INT16U wKeyCodeBit)
{
	OS_CPU_SR  cpu_sr;
	
	OS_ENTER_CRITICAL();
	wKeyVal |= wKeyCodeBit;
	OS_EXIT_CRITICAL();
}

void	sClrKeyCode(INT16U wKeyCodeBit)
{
	OS_CPU_SR  cpu_sr;
	
	OS_ENTER_CRITICAL();
	wKeyVal &= ~wKeyCodeBit;
	OS_EXIT_CRITICAL();
}

void	sClrKeyPressTurnOff(void)
{
	sClrKeyCode(cKEY_PRESS_TURNOFF_BIT);
}

void	sClrKeyPressForceOn(void)
{
	sClrKeyCode(cKEY_PRESS_FORCEON_BIT);
}

void	sClrKeyPressDebug(void)
{
	sClrKeyCode(cKEY_PRESS_DEBUG_BIT);
}

void	sClrKeyPressConfig(void)
{
	sClrKeyCode(cKEY_PRESS_CONFIG_BIT);
}

void	sClrKeyPressAdjust(void)
{
	sClrKeyCode(cKEY_PRESS_ADJUST_BIT);
}

void	sClrKeyPressEnter(void)
{
	sClrKeyCode(cKEY_PRESS_ENTER_BIT);
}

