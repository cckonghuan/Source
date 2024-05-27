/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		TwiDriver.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define		__TWI_DRIVER_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"Main.h"


/********************************************************************************
* Macros 																		*
********************************************************************************/


/********************************************************************************
* Constants																		*
********************************************************************************/
const INT8U CRC8Table[]=
{							// CRC Table
	0x00,0x07,0x0E,0x09,0x1C,0x1B,0x12,0x15,0x38,0x3F,0x36,0x31,0x24,0x23,0x2A,0x2D,
	0x70,0x77,0x7E,0x79,0x6C,0x6B,0x62,0x65,0x48,0x4F,0x46,0x41,0x54,0x53,0x5A,0x5D,
	0xE0,0xE7,0xEE,0xE9,0xFC,0xFB,0xF2,0xF5,0xD8,0xDF,0xD6,0xD1,0xC4,0xC3,0xCA,0xCD,
	0x90,0x97,0x9E,0x99,0x8C,0x8B,0x82,0x85,0xA8,0xAF,0xA6,0xA1,0xB4,0xB3,0xBA,0xBD,
	0xC7,0xC0,0xC9,0xCE,0xDB,0xDC,0xD5,0xD2,0xFF,0xF8,0xF1,0xF6,0xE3,0xE4,0xED,0xEA,
	0xB7,0xB0,0xB9,0xBE,0xAB,0xAC,0xA5,0xA2,0x8F,0x88,0x81,0x86,0x93,0x94,0x9D,0x9A,
	0x27,0x20,0x29,0x2E,0x3B,0x3C,0x35,0x32,0x1F,0x18,0x11,0x16,0x03,0x04,0x0D,0x0A,
	0x57,0x50,0x59,0x5E,0x4B,0x4C,0x45,0x42,0x6F,0x68,0x61,0x66,0x73,0x74,0x7D,0x7A,
	0x89,0x8E,0x87,0x80,0x95,0x92,0x9B,0x9C,0xB1,0xB6,0xBF,0xB8,0xAD,0xAA,0xA3,0xA4,
	0xF9,0xFE,0xF7,0xF0,0xE5,0xE2,0xEB,0xEC,0xC1,0xC6,0xCF,0xC8,0xDD,0xDA,0xD3,0xD4,
	0x69,0x6E,0x67,0x60,0x75,0x72,0x7B,0x7C,0x51,0x56,0x5F,0x58,0x4D,0x4A,0x43,0x44,
	0x19,0x1E,0x17,0x10,0x05,0x02,0x0B,0x0C,0x21,0x26,0x2F,0x28,0x3D,0x3A,0x33,0x34,
	0x4E,0x49,0x40,0x47,0x52,0x55,0x5C,0x5B,0x76,0x71,0x78,0x7F,0x6A,0x6D,0x64,0x63,
	0x3E,0x39,0x30,0x37,0x22,0x25,0x2C,0x2B,0x06,0x01,0x08,0x0F,0x1A,0x1D,0x14,0x13,
	0xAE,0xA9,0xA0,0xA7,0xB2,0xB5,0xBC,0xBB,0x96,0x91,0x98,0x9F,0x8A,0x8D,0x84,0x83,
	0xDE,0xD9,0xD0,0xD7,0xC2,0xC5,0xCC,0xCB,0xE6,0xE1,0xE8,0xEF,0xFA,0xFD,0xF4,0xF3
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


/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/


/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void	sTwiStart(void)
{
	mI2C_DATA_TYPE_DEF();
	
	mI2C_DATA_HIGH();
	mI2C_CLOCK_HIGH();
	mI2C_OUT_DATA_PORT();
	mI2C_OUT_CLOCK_PORT();
	mI2C_DATA_LOW();
	mI2C_BIT_DELAY();
	mI2C_CLOCK_LOW();
}

void	sTwiReStart(void)
{
	//mI2C_DATA_TYPE_DEF();
	
	mI2C_DATA_HIGH();
	mI2C_CLOCK_HIGH();
	//mI2C_OUT_DATA_PORT();
	//mI2C_OUT_CLOCK_PORT();
	mI2C_BIT_DELAY();
	mI2C_DATA_LOW();
	mI2C_BIT_DELAY();
	mI2C_CLOCK_LOW();
}

void	sTwiStop(void)
{
	//mI2C_DATA_TYPE_DEF();
	
	//mI2C_OUT_DATA_PORT();
	//mI2C_OUT_CLOCK_PORT();
	
	mI2C_DATA_LOW();
	mI2C_BIT_DELAY();
	mI2C_CLOCK_HIGH();
	mI2C_BIT_DELAY();
	mI2C_DATA_HIGH();	
	mI2C_BIT_DELAY();
	
	//mI2C_IN_DATA_PORT();
	//mI2C_IN_CLOCK_PORT();
}

INT8U	sbTwiChkClkRelease(void)
{
	INT16U	TimeoutCnt = 1000;	// If Clock is not released within 4ms, is considered overtime
	INT8U	result = 0;
	
	mI2C_CLOCK_TYPE_DEF();
	mI2C_IN_CLOCK_PORT();
	while(TimeoutCnt--)
	{
		mI2C_BIT_DELAY();
		if(mI2C_CLOCK_READ())
		{
			result = 1;
			break;
		}
	}
	mI2C_CLOCK_HIGH();
	mI2C_OUT_CLOCK_PORT();
	
	return result;
}

INT8U	sbTwiSendData(INT8U Data, INT8U ClkFlg)
{
	INT8U	i;
	INT8U	result=0;
	
	mI2C_DATA_TYPE_DEF();
	//mI2C_OUT_DATA_PORT();
	//mI2C_OUT_CLOCK_PORT();
	
	//1. After sending the Start signal, there is no need to detect Clock is released, And sending the first bit
	if(Data&0x80)
	{
		mI2C_DATA_HIGH();
	}
	else
	{
		mI2C_DATA_LOW();
	}
	
	if(ClkFlg == 1)
	{
		mI2C_BIT_DELAY();
		if(sbTwiChkClkRelease())
		{
			mI2C_CLOCK_LOW();
		}
		else
		{
			return result;
		}
	}
	else
	{
		mI2C_BIT_DELAY();
		mI2C_CLOCK_HIGH();
		mI2C_BIT_DELAY();
		mI2C_CLOCK_LOW();
	}
	
	//2. Send the remaining seven bit
	Data = Data<<1;
	for(i = 0; i < 7; i++)
	{
		if(Data&0x80)
		{
			mI2C_DATA_HIGH();
		}
		else
		{
			mI2C_DATA_LOW();
		}
		Data = Data << 1;
		mI2C_BIT_DELAY();
		mI2C_CLOCK_HIGH();
		mI2C_BIT_DELAY();
		mI2C_CLOCK_LOW();
	}
	
	mI2C_IN_DATA_PORT();
	mI2C_BIT_DELAY();
	
	for(i = 0; i < 10; i++)
	{
		if(mI2C_DATA_READ() == 0)
		{
			result = 1;
			break;
		}
	}
	mI2C_CLOCK_HIGH();
	
	mI2C_BIT_DELAY();
	mI2C_DATA_LOW();
	mI2C_OUT_DATA_PORT();
	mI2C_CLOCK_LOW();
	mI2C_BIT_DELAY();
	
	return result;
}

INT8U	sbTwiGetData(INT8U AckFlg)
{
	INT8U i, RdData = 0;
	
	mI2C_DATA_TYPE_DEF();
	mI2C_IN_DATA_PORT();
	//mI2C_OUT_CLOCK_PORT();
	mI2C_BIT_DELAY();
	
	for(i = 0; i < 8; i++)
	{
		mI2C_CLOCK_HIGH();
		mI2C_BIT_DELAY();
		if(mI2C_DATA_READ())
		{
			RdData |= (1<<(7-i));
		}
		mI2C_CLOCK_LOW();
		mI2C_BIT_DELAY();
	}
	
	mI2C_OUT_DATA_PORT();
	if(AckFlg != 0)
	{
		mI2C_DATA_LOW();
	}
	else
	{
		mI2C_DATA_HIGH();
	}
	mI2C_CLOCK_HIGH();
	mI2C_BIT_DELAY();
	mI2C_CLOCK_LOW();
	mI2C_BIT_DELAY();
	
	return RdData;
}

INT8U	sbCRC8cal(INT8U *p, INT8U Length)		// look-up table calculte CRC 
{
	INT8U crc8 = 0;
	
	for(; Length > 0; Length--)
	{
		crc8 = CRC8Table[crc8^*p];
		p++;
	}
	
	return(crc8);
}

INT8U	sbTwiWrite(INT8U SlaveID, INT16U WrAddr, INT8U Length, INT8U *WrBuf)
{
	INT8U i;
	INT8U TempBuf[4];
	INT8U result = 0;
	
	TempBuf[0] = SlaveID;
	TempBuf[1] = (INT8U)WrAddr;
	TempBuf[2] = *WrBuf;
	TempBuf[3] = sbCRC8cal(TempBuf, 3);
	
	if(Length > 0)
	{
		sTwiStart();
		if(!sbTwiSendData(SlaveID, 1))			// Send Slave ID
		{
			goto WrErr;
		}
		if(SlaveID == E2PROM_ID)
		{
			if(!sbTwiSendData(WrAddr>>8, 0))	// Send Write Address(High 8bit) For EEPROM
			{
				goto WrErr;
			}
		}
		if(sbTwiSendData(WrAddr, 0))			// Send Write Address(Low 8bit)
		{
			result = 1;
			for(i = 0; i < Length; i++)
			{
				if(sbTwiSendData(*WrBuf, 0))	// Send Write Data
				{
					WrBuf++;
				}
				else
				{
					result = 0;
					break;
				}
			}
			if(SlaveID == AFE_ID)
			{
				if(!sbTwiSendData(TempBuf[3], 0))	// write CRC
				{
					result = 0;
				}
			}
		}
WrErr:
		sTwiStop();
	}
	
	return result;
}

INT8U	sbTwiRead(INT8U SlaveID, INT16U RdAddr, INT8U Length, INT8U *RdBuf)
{
	INT8U i;
	INT8U result=0;
	INT8U TempBuf[46];
	INT8U RdCrc=0;
	
	TempBuf[0] = SlaveID;
	TempBuf[1] = (INT8U)RdAddr;
	TempBuf[2] = Length;
	TempBuf[3] = SlaveID | 0x01;
	
	if(Length > 0)
	{
		sTwiStart();
		if(!sbTwiSendData(SlaveID, 1))			// Send Slave ID
		{
			goto RdErr;
		}
		if(SlaveID == E2PROM_ID)
		{
			if(!sbTwiSendData(RdAddr>>8, 0))	//Send Read Address(High 8bit) For EEPROM
			{
				goto RdErr;
			}
		}
		if(!sbTwiSendData(RdAddr, 0))			// Send Read Address(Low 8bit)
		{
			goto RdErr;
		}
		if(SlaveID == AFE_ID)					// CRC
		{
			if(!sbTwiSendData(Length, 0))
			{
				goto RdErr;
			}
		}
		sTwiReStart();
		if(sbTwiSendData(SlaveID|0x1, 0))		// Send Slave ID
		{
			result = 1;
			if(SlaveID == AFE_ID)
			{
				for(i = 0; i < Length+1; i++)
				{
					if(i == Length)
					{
						RdCrc = sbTwiGetData(0);		// Get Data
					}
					else
					{
						TempBuf[4+i] = sbTwiGetData(1);	// Get Data
					}
				}
				if(RdCrc != sbCRC8cal(TempBuf, 4 + Length))
				{
					result = 0;    
				}
				else
				{
					for(i = 0; i < Length; i++)
					{
						*RdBuf = TempBuf[4+i];
						RdBuf++;   
					}
				}
			}
			else
			{
				for(i = 0; i < Length; i++)
				{
					*RdBuf = sbTwiGetData(Length-i-1);	// Get Data
					RdBuf++;
				}
			}
		}
RdErr:
		sTwiStop();
	}
	
	return result;
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/


/********************************************************************************
* Input interface Routines														*
********************************************************************************/


