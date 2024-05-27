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
		INT16U ForcedOn:1;		// ǿ�ƿ���,�����޵�ѹ/�����������Ƶ��ģʽ5����
		INT16U DebugMode:1;		// LCD����DEBUGģʽ
		INT16U ConfigMode:1;	// LCD����DEBUG������ģʽ
		INT16U PressTrig:1;		// ���ڰ���
		
		// BIT4-7
		INT16U LCDMode:1;		// LCD��������
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

    // ��ȡ������ַ
    bKeyAddress = sbKeyAddressRead();

    // �����ȡ���İ�����ַΪ0��������Ϊ�����ַ
    temp = bKeyAddress;
    if(temp == 0)
    {
        temp = cMODBUS_ADDR_ERR;
    }
    // ����SCI��ַ
    sSetSciAddress(temp);
    // ����CAN��ַ
    sSetCanAddress(bKeyAddress);

    // ��ʼ������ֵ
    wKeyVal = 0;
}

void sKeyPWRScanProc(INT16U wOffCnt, INT16U wForceCnt)
{
    static INT8U key_scan_mode = cKEY_SCAN_READY; // ����ɨ��ģʽ
    static INT8U bReleaseFlag = false; // �����������Ƿ��ͷű�־
    static INT16U wPressCnt = 0; // �������¼�����

    switch(key_scan_mode)
    {
        case cKEY_SCAN_READY: // ׼��ɨ��
            wPressCnt = 0;
            if(mIS_KEY_PRESSED()) // ��鰴���Ƿ���
            {
                wPressCnt++;
                sSetKeyStatusPressTrig(true); // ���ð������´���״̬
                key_scan_mode = cKEY_SCAN_PRESS; // �л�����������ģʽ
            }
            else
            {
                bReleaseFlag = true; // �������ͷ�
            }
        break;
        case cKEY_SCAN_PRESS: // ��������ģʽ
            if(mIS_KEY_PRESSED()) // �������ְ���״̬
            {
                if(wPressCnt < cKEY_BIT16_COUNT_MAX) // �����������δ�����ֵ����������
                {
                    wPressCnt++;
                }

                if(bReleaseFlag == false) // �������δ�ͷ�
                {
                    if(wPressCnt >= wForceCnt) // ����������¼�������ǿ�ƿ�������
                    {
                        sSetKeyCode(cKEY_PRESS_FORCEON_BIT); // ���ð���ǿ�ƿ�����
                        key_scan_mode = cKEY_SCAN_STOP; // �л�������ֹͣģʽ
                    }
                }
            }
            else // �������ͷ�
            {
                if(wPressCnt >= wOffCnt) // ����������¼��������ػ�����
                {
                    sSetKeyCode(cKEY_PRESS_TURNOFF_BIT); // ���ð����ػ���
                }
                key_scan_mode = cKEY_SCAN_STOP; // �л�������ֹͣģʽ
            }
        break;
        case cKEY_SCAN_STOP: // ����ֹͣģʽ
            if(mIS_KEY_RELEASE()) // ����������ͷ�
            {
                key_scan_mode = cKEY_SCAN_READY; // �л���׼��ɨ��ģʽ
            }
        break;
        default:
            key_scan_mode = cKEY_SCAN_READY; // Ĭ���л���׼��ɨ��ģʽ
        break;
    }
}

void sKeyLCDScanProc(INT16U wOffCnt, INT16U wDbgCnt)
{
    static INT8U key_scan_mode = cKEY_SCAN_READY; // ����ɨ��ģʽ
    static INT16U wPressCnt = 0; // �������¼�����

    switch(key_scan_mode)
    {
        case cKEY_SCAN_READY: // ׼��ɨ��ģʽ
            wPressCnt = 0;
            if(mIS_LCD_KEY2_PRESSED()) // ���LCD����2�Ƿ���
            {
                wPressCnt++;
                key_scan_mode = cKEY_SCAN_PRESS; // �л�����������ģʽ
            }
        break;
        case cKEY_SCAN_PRESS: // ��������ģʽ
            if(mIS_LCD_KEY2_PRESSED()) // �������ְ���״̬
            {
                if(wPressCnt < cKEY_BIT16_COUNT_MAX) // �����������δ�����ֵ����������
                {
                    wPressCnt++;
                }

                if(wPressCnt >= wDbgCnt) // ����������¼����������Լ���
                {
                    wPressCnt = 0;
                    sRstLCDLoopCnt(); // ����LCDѭ������
                    sSetKeyStatusDebugMode(true); // ���ð�������ģʽ״̬
                    key_scan_mode = cKEY_SCAN_STOP; // �л�������ֹͣģʽ
                }
            }
            else // �������ͷ�
            {
                key_scan_mode = cKEY_SCAN_STOP; // �л�������ֹͣģʽ
            }
        break;
        case cKEY_SCAN_STOP: // ����ֹͣģʽ
            if(mIS_LCD_KEY2_RELEASE()) // ����������ͷ�
            {
                if(wPressCnt >= wOffCnt) // ����������¼��������ػ�����
                {
                    wPressCnt = 0;
                    sSetKeyStatusLCDMode(true); // ���ð���LCDģʽ״̬
                }
                key_scan_mode = cKEY_SCAN_READY; // �л���׼��ɨ��ģʽ
            }
        break;
        default:
            key_scan_mode = cKEY_SCAN_READY; // Ĭ���л���׼��ɨ��ģʽ
        break;
    }
}

void sKeyForceOnTimeout(INT16U wTimeOut)
{
    static INT16U wChkCnt = 0; // ��������

    if(sbGetKeyStatusForcedOn()) // ��鰴���Ƿ�ǿ�ƿ���
    {
        if(++wChkCnt >= wTimeOut) // ���������������ʱʱ��
        {
            wChkCnt = 0;
            sSetKeyStatusForcedOn(false); // ȡ��ǿ�ƿ���״̬
        }
    }
    else
    {
        if((swGetKeyCode() & cKEY_PRESS_FORCEON_BIT) != 0) // ��鰴�����Ƿ����ǿ�ƿ���λ
        {
            wChkCnt = 0;
            sSetKeyStatusForcedOn(true); // ����ǿ�ƿ���״̬
            sClrKeyCode(cKEY_PRESS_FORCEON_BIT); // ����������е�ǿ�ƿ���λ
        }
    }
}
void sKeyTurnOffProc(void)
{
	// ���ػ������Ƿ񱻰���
	if((swGetKeyCode() & cKEY_PRESS_TURNOFF_BIT) != 0)
	{
		// �ػ����������£����õ�ع���ϵͳ��BMS��Ϊ�����ػ�ģʽ
		sSetBmsParallelShutdown(true);

		// �����ȼ�Ϊ cPrioSuper �������͹ػ��¼� eSuperToShutdown
		OSEventSend(cPrioSuper, eSuperToShutdown);

		// ����ػ������ı�־λ
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

