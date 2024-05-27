/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		Initial.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef	__INITIAL_H__
#define __INITIAL_H__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include		"OS_CPU.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#ifdef	__INITIAL_C__
#define	INITIAL
#else
#define	INITIAL	extern
#endif

#define PIN_AFE_SDA					GPIO_PIN_11
#define GPIO_AFE_SDA				GPIOA
#define CLOCK_AFE_SDA				RCM_APB2_PERIPH_GPIOA

#define PIN_AFE_SCL					GPIO_PIN_12
#define GPIO_AFE_SCL				GPIOA
#define CLOCK_AFE_SCL				RCM_APB2_PERIPH_GPIOA

#define PIN_AFE_EEPROM_EN			GPIO_PIN_8
#define GPIO_AFE_EEPROM_EN			GPIOA
#define CLOCK_AFE_EEPROM_EN			RCM_APB2_PERIPH_GPIOA

#define PIN_LED_TEST				GPIO_PIN_4
#define GPIO_LED_TEST				GPIOA
#define CLOCK_LED_TEST				RCM_APB2_PERIPH_GPIOA

#define PIN_ADC_VERSION1			GPIO_PIN_1
#define GPIO_ADC_VERSION1			GPIOA
#define CLOCK_ADC_VERSION1			RCM_APB2_PERIPH_GPIOA

#define PIN_ADC_NTC_TEMP			GPIO_PIN_0
#define GPIO_ADC_NTC_TEMP			GPIOA
#define CLOCK_ADC_NTC_TEMP			RCM_APB2_PERIPH_GPIOA

#define PIN_ADC_CELL1_TEMP			GPIO_PIN_0
#define GPIO_ADC_CELL1_TEMP			GPIOC
#define CLOCK_ADC_CELL1_TEMP		RCM_APB2_PERIPH_GPIOC

#define PIN_ADC_CELL2_TEMP			GPIO_PIN_1
#define GPIO_ADC_CELL2_TEMP			GPIOC
#define CLOCK_ADC_CELL2_TEMP		RCM_APB2_PERIPH_GPIOC

#define PIN_ADC_CELL3_TEMP			GPIO_PIN_2
#define GPIO_ADC_CELL3_TEMP			GPIOC
#define CLOCK_ADC_CELL3_TEMP		RCM_APB2_PERIPH_GPIOC

#define PIN_ADC_CELL4_TEMP			GPIO_PIN_3
#define GPIO_ADC_CELL4_TEMP			GPIOC
#define CLOCK_ADC_CELL4_TEMP		RCM_APB2_PERIPH_GPIOC

#define PIN_SPS_SD					GPIO_PIN_15
#define GPIO_SPS_SD					GPIOB
#define CLOCK_SPS_SD				RCM_APB2_PERIPH_GPIOB

#define PIN_MOS_CHG					GPIO_PIN_7
#define GPIO_MOS_CHG				GPIOC
#define CLOCK_MOS_CHG				RCM_APB2_PERIPH_GPIOC

#define PIN_MOS_DSG					GPIO_PIN_6
#define GPIO_MOS_DSG				GPIOC
#define CLOCK_MOS_DSG				RCM_APB2_PERIPH_GPIOC

#define PIN_CAN_RX					GPIO_PIN_8
#define GPIO_CAN_RX					GPIOB
#define CLOCK_CAN_RX				RCM_APB2_PERIPH_GPIOB

#define PIN_CAN_TX					GPIO_PIN_9
#define GPIO_CAN_TX					GPIOB
#define CLOCK_CAN_TX				RCM_APB2_PERIPH_GPIOB

#define PIN_SCI_RJ45_TXD			GPIO_PIN_9
#define GPIO_SCI_RJ45_TXD			GPIOA
#define CLOCK_SCI_RJ45_TXD			RCM_APB2_PERIPH_GPIOA

#define PIN_SCI_RJ45_RXD			GPIO_PIN_10
#define GPIO_SCI_RJ45_RXD			GPIOA
#define CLOCK_SCI_RJ45_RXD			RCM_APB2_PERIPH_GPIOA

#define PIN_CAN_PCS_RX				GPIO_PIN_5
#define GPIO_CAN_PCS_RX				GPIOB
#define CLOCK_CAN_PCS_RX			RCM_APB2_PERIPH_GPIOB

#define PIN_CAN_PCS_TX				GPIO_PIN_6
#define GPIO_CAN_PCS_TX				GPIOB
#define CLOCK_CAN_PCS_TX			RCM_APB2_PERIPH_GPIOB

#define PIN_KEY_STATUS				GPIO_PIN_14
#define GPIO_KEY_STATUS				GPIOB
#define CLOCK_KEY_STATUS			RCM_APB2_PERIPH_GPIOB

#define PIN_LCD_KEY1				GPIO_PIN_6
#define GPIO_LCD_KEY1				GPIOA
#define CLOCK_LCD_KEY1				RCM_APB2_PERIPH_GPIOA

#define PIN_LCD_KEY2				GPIO_PIN_5
#define GPIO_LCD_KEY2				GPIOA
#define CLOCK_LCD_KEY2				RCM_APB2_PERIPH_GPIOA

#define PIN_LED_STATUS				GPIO_PIN_2
#define GPIO_LED_STATUS				GPIOA
#define CLOCK_LED_STATUS			RCM_APB2_PERIPH_GPIOA

#define PIN_LED_ALARM				GPIO_PIN_3
#define GPIO_LED_ALARM				GPIOA
#define CLOCK_LED_ALARM				RCM_APB2_PERIPH_GPIOA

#define PIN_LED_SOC1				GPIO_PIN_7
#define GPIO_LED_SOC1				GPIOA
#define CLOCK_LED_SOC1				RCM_APB2_PERIPH_GPIOA

#define PIN_LED_SOC2				GPIO_PIN_4
#define GPIO_LED_SOC2				GPIOC
#define CLOCK_LED_SOC2				RCM_APB2_PERIPH_GPIOC

#define PIN_LED_SOC3				GPIO_PIN_5
#define GPIO_LED_SOC3				GPIOC
#define CLOCK_LED_SOC3				RCM_APB2_PERIPH_GPIOC

#define PIN_LED_SOC4				GPIO_PIN_0
#define GPIO_LED_SOC4				GPIOB
#define CLOCK_LED_SOC4				RCM_APB2_PERIPH_GPIOB

// LCD
#define PIN_LCD_CS					GPIO_PIN_7
#define GPIO_LCD_CS					GPIOA
#define CLOCK_LCD_CS				RCM_APB2_PERIPH_GPIOA

#define PIN_LCD_BL					GPIO_PIN_4
#define GPIO_LCD_BL					GPIOC
#define CLOCK_LCD_BL				RCM_APB2_PERIPH_GPIOC

#define PIN_LCD_SCL					GPIO_PIN_5
#define GPIO_LCD_SCL				GPIOC
#define CLOCK_LCD_SCL				RCM_APB2_PERIPH_GPIOC

#define PIN_LCD_SDA					GPIO_PIN_0
#define GPIO_LCD_SDA				GPIOB
#define CLOCK_LCD_SDA				RCM_APB2_PERIPH_GPIOB

#define	mLED_TEST_ON()				GPIO_ResetBit(GPIO_LED_TEST, PIN_LED_TEST)
#define	mLED_TEST_OFF()				GPIO_SetBit(GPIO_LED_TEST, PIN_LED_TEST)
#define	mIS_LED_TEST_ON()			(GPIO_ReadOutputBit(GPIO_LED_TEST, PIN_LED_TEST)==0)

#define	mIS_KEY_PRESSED()			(GPIO_ReadInputBit(GPIO_KEY_STATUS, PIN_KEY_STATUS) == 0)
#define	mIS_KEY_RELEASE()			(GPIO_ReadInputBit(GPIO_KEY_STATUS, PIN_KEY_STATUS) == 1)

#define	mIS_LCD_KEY1_PRESSED()		(GPIO_ReadInputBit(GPIO_LCD_KEY1, PIN_LCD_KEY1) == 1)
#define	mIS_LCD_KEY1_RELEASE()		(GPIO_ReadInputBit(GPIO_LCD_KEY1, PIN_LCD_KEY1) == 0)

#define	mIS_LCD_KEY2_PRESSED()		(GPIO_ReadInputBit(GPIO_LCD_KEY2, PIN_LCD_KEY2) == 1)
#define	mIS_LCD_KEY2_RELEASE()		(GPIO_ReadInputBit(GPIO_LCD_KEY2, PIN_LCD_KEY2) == 0)

#define	mLED_STATUS_ON()			GPIO_SetBit(GPIO_LED_STATUS, PIN_LED_STATUS)
#define	mLED_STATUS_OFF()			GPIO_ResetBit(GPIO_LED_STATUS, PIN_LED_STATUS)

#define	mLED_ALARM_ON()				GPIO_SetBit(GPIO_LED_ALARM, PIN_LED_ALARM)
#define	mLED_ALARM_OFF()			GPIO_ResetBit(GPIO_LED_ALARM, PIN_LED_ALARM)

#define	mLED_SOC_RED_ON()			//gpio_bit_set(GPIO_LED_SOC_RED, PIN_LED_SOC_RED)
#define	mLED_SOC_RED_OFF()			//gpio_bit_reset(GPIO_LED_SOC_RED, PIN_LED_SOC_RED)

#define	mLED_SOC1_ON()				GPIO_SetBit(GPIO_LED_SOC1, PIN_LED_SOC1)
#define	mLED_SOC1_OFF()				GPIO_ResetBit(GPIO_LED_SOC1, PIN_LED_SOC1)

#define	mLED_SOC2_ON()				GPIO_SetBit(GPIO_LED_SOC2, PIN_LED_SOC2)
#define	mLED_SOC2_OFF()				GPIO_ResetBit(GPIO_LED_SOC2, PIN_LED_SOC2)

#define	mLED_SOC3_ON()				GPIO_SetBit(GPIO_LED_SOC3, PIN_LED_SOC3)
#define	mLED_SOC3_OFF()				GPIO_ResetBit(GPIO_LED_SOC3, PIN_LED_SOC3)

#define	mLED_SOC4_ON()				GPIO_SetBit(GPIO_LED_SOC4, PIN_LED_SOC4)
#define	mLED_SOC4_OFF()				GPIO_ResetBit(GPIO_LED_SOC4, PIN_LED_SOC4)

#define	mADD_SW1_STATUS()			0

#define	mADD_SW2_STATUS()			0

#define	mADD_SW3_STATUS()			0

#define	mADD_SW4_STATUS()			0

#define	mSPS_SHUTDOWN_ENABLE()		GPIO_SetBit(GPIO_SPS_SD, PIN_SPS_SD)
#define	mSPS_SHUTDOWN_DISABLE()		GPIO_ResetBit(GPIO_SPS_SD, PIN_SPS_SD)

#define	mAFE_EEPROM_ON()			GPIO_SetBit(GPIO_AFE_EEPROM_EN, PIN_AFE_EEPROM_EN)
#define	mAFE_EEPROM_OFF()			GPIO_ResetBit(GPIO_AFE_EEPROM_EN, PIN_AFE_EEPROM_EN)
#define	mIS_AFE_EEPROM_ON()			(GPIO_ReadOutputBit(GPIO_AFE_EEPROM_EN, PIN_AFE_EEPROM_EN) == 1)

#define	mI2C_CS_HIGH()				GPIO_SetBit(GPIO_AFE_SDA, PIN_AFE_SDA)
#define	mI2C_CS_LOW()				GPIO_ResetBit(GPIO_AFE_SDA,PIN_AFE_SDA)

#define	mI2C_DATA_HIGH()			GPIO_SetBit(GPIO_AFE_SDA, PIN_AFE_SDA)
#define	mI2C_DATA_LOW()				GPIO_ResetBit(GPIO_AFE_SDA,PIN_AFE_SDA)
#define	mI2C_DATA_READ()			GPIO_ReadInputBit(GPIO_AFE_SDA, PIN_AFE_SDA)

#define	mI2C_CLOCK_HIGH()			GPIO_SetBit(GPIO_AFE_SCL, PIN_AFE_SCL)
#define	mI2C_CLOCK_LOW()			GPIO_ResetBit(GPIO_AFE_SCL, PIN_AFE_SCL)
#define	mI2C_CLOCK_READ()			GPIO_ReadInputBit(GPIO_AFE_SCL, PIN_AFE_SCL)

#define	mI2C_BIT_DELAY()			sDelay4us()

#define	mI2C_DATA_TYPE_DEF()		GPIO_Config_T configStruct;
#define	mI2C_OUT_DATA_PORT()		RCM_EnableAPB2PeriphClock(CLOCK_AFE_SDA);\
									configStruct.pin = PIN_AFE_SDA;\
									configStruct.mode = GPIO_MODE_OUT_PP;\
									configStruct.speed = GPIO_SPEED_2MHz;\
									GPIO_Config(GPIO_AFE_SDA, &configStruct);

#define	mI2C_IN_DATA_PORT()			RCM_EnableAPB2PeriphClock(CLOCK_AFE_SDA);\
									configStruct.pin = PIN_AFE_SDA;\
									configStruct.mode = GPIO_MODE_IN_PU;\
									configStruct.speed = GPIO_SPEED_2MHz;\
									GPIO_Config(GPIO_AFE_SDA, &configStruct);

#define	mI2C_CLOCK_TYPE_DEF()		GPIO_Config_T configStruct;
#define	mI2C_OUT_CLOCK_PORT()		RCM_EnableAPB2PeriphClock(CLOCK_AFE_SCL);\
									configStruct.pin = PIN_AFE_SCL;\
									configStruct.mode = GPIO_MODE_OUT_PP;\
									configStruct.speed = GPIO_SPEED_2MHz;\
									GPIO_Config(GPIO_AFE_SCL, &configStruct);

#define	mI2C_IN_CLOCK_PORT()		RCM_EnableAPB2PeriphClock(CLOCK_AFE_SCL);\
									configStruct.pin = PIN_AFE_SCL;\
									configStruct.mode = GPIO_MODE_IN_PU;\
									configStruct.speed = GPIO_SPEED_2MHz;\
									GPIO_Config(GPIO_AFE_SCL, &configStruct);

#define	mMOS_CHG_ON()				GPIO_SetBit(GPIO_MOS_CHG, PIN_MOS_CHG)
#define	mMOS_CHG_OFF()				GPIO_ResetBit(GPIO_MOS_CHG, PIN_MOS_CHG)
#define	mIS_MOS_CHG_ON()			(GPIO_ReadOutputBit(GPIO_MOS_CHG, PIN_MOS_CHG) == 1)

#define	mMOS_DSG_ON()				GPIO_SetBit(GPIO_MOS_DSG, PIN_MOS_DSG)
#define	mMOS_DSG_OFF()				GPIO_ResetBit(GPIO_MOS_DSG, PIN_MOS_DSG)
#define	mIS_MOS_DSG_ON()			(GPIO_ReadOutputBit(GPIO_MOS_DSG, PIN_MOS_DSG) == 1)

#define	mMOS_SOFT_CHG_ON()			
#define	mMOS_SOFT_CHG_OFF()			
#define	mIS_MOS_SOFT_CHG_ON()		0

#define	mMOS_SOFT_DSG_ON()			
#define	mMOS_SOFT_DSG_OFF()			
#define	mIS_MOS_SOFT_DSG_ON()		0

#define	mMOS_ALL_ON()				mMOS_CHG_ON();\
									mMOS_DSG_ON();\

#define	mMOS_ALL_OFF()				mMOS_CHG_OFF();\
									mMOS_DSG_OFF();

#define	mMOS_SOFT_ALL_ON()			mMOS_SOFT_CHG_ON();\
									mMOS_SOFT_DSG_ON();\

#define	mMOS_SOFT_ALL_OFF()			mMOS_SOFT_CHG_OFF();\
									mMOS_SOFT_DSG_OFF();

#define	mDISPLAY_SOC_EMPTY()		(mLED_SOC1_OFF(), mLED_SOC2_OFF(), mLED_SOC3_OFF(), mLED_SOC4_OFF())
#define	mDISPLAY_SOC_25PCT()		(mLED_SOC1_ON(), mLED_SOC2_OFF(), mLED_SOC3_OFF(), mLED_SOC4_OFF())
#define	mDISPLAY_SOC_50PCT()		(mLED_SOC1_ON(), mLED_SOC2_ON(), mLED_SOC3_OFF(), mLED_SOC4_OFF())
#define	mDISPLAY_SOC_75PCT()		(mLED_SOC1_ON(), mLED_SOC2_ON(), mLED_SOC3_ON(), mLED_SOC4_OFF())
#define	mDISPLAY_SOC_FULL()			(mLED_SOC1_ON(), mLED_SOC2_ON(), mLED_SOC3_ON(), mLED_SOC4_ON())

//#define	mDISPLAY_SOC_25PCT_BLK()	(mLED_SOC1_OFF(), mLED_SOC2_OFF(), mLED_SOC3_OFF(), mLED_SOC4_OFF())
//#define	mDISPLAY_SOC_50PCT_BLK()	(mLED_SOC1_ON(), mLED_SOC2_OFF(), mLED_SOC3_OFF(), mLED_SOC4_OFF())
//#define	mDISPLAY_SOC_75PCT_BLK()	(mLED_SOC1_ON(), mLED_SOC2_ON(), mLED_SOC3_OFF(), mLED_SOC4_OFF())
//#define	mDISPLAY_SOC_FULL_BLK()		(mLED_SOC1_ON(), mLED_SOC2_ON(), mLED_SOC3_ON(), mLED_SOC4_OFF())

#define	mDISPLAY_BATT_VOLT_HI()		(mLED_SOC1_ON(), mLED_SOC2_OFF(), mLED_SOC3_OFF(), mLED_SOC4_OFF())
#define	mDISPLAY_BATT_VOLT_LO()		(mLED_SOC1_OFF(), mLED_SOC2_ON(), mLED_SOC3_OFF(), mLED_SOC4_OFF())
#define	mDISPLAY_CHG_CURR_HI()		(mLED_SOC1_ON(), mLED_SOC2_ON(), mLED_SOC3_OFF(), mLED_SOC4_OFF())
#define	mDISPLAY_DSG_CURR_HI()		(mLED_SOC1_OFF(), mLED_SOC2_OFF(), mLED_SOC3_ON(), mLED_SOC4_OFF())
#define	mDISPLAY_CELL_VOLT_HI()		(mLED_SOC1_ON(), mLED_SOC2_OFF(), mLED_SOC3_ON(), mLED_SOC4_OFF())
#define	mDISPLAY_CELL_VOLT_LO()		(mLED_SOC1_OFF(), mLED_SOC2_ON(), mLED_SOC3_ON(), mLED_SOC4_OFF())
#define	mDISPLAY_CELL_TEMP_HI()		(mLED_SOC1_ON(), mLED_SOC2_ON(), mLED_SOC3_ON(), mLED_SOC4_OFF())
#define	mDISPLAY_CELL_TEMP_LO()		(mLED_SOC1_OFF(), mLED_SOC2_OFF(), mLED_SOC3_OFF(), mLED_SOC4_ON())
#define	mDISPLAY_HS_TEMP_HI()		(mLED_SOC1_ON(), mLED_SOC2_OFF(), mLED_SOC3_OFF(), mLED_SOC4_ON())
#define	mDISPLAY_HS_TEMP_LO()		(mLED_SOC1_OFF(), mLED_SOC2_ON(), mLED_SOC3_OFF(), mLED_SOC4_ON())
#define	mDISPLAY_CURR_SENSOR()		(mLED_SOC1_ON(), mLED_SOC2_ON(), mLED_SOC3_OFF(), mLED_SOC4_ON())
#define	mDISPLAY_OP_RES_LO()		(mLED_SOC1_OFF(), mLED_SOC2_OFF(), mLED_SOC3_ON(), mLED_SOC4_ON())
#define	mDISPLAY_PARALLEL_FAIL()	(mLED_SOC1_ON(), mLED_SOC2_OFF(), mLED_SOC3_ON(), mLED_SOC4_ON())
#define	mDISPLAY_ALARM_NULL()		(mLED_SOC1_OFF(), mLED_SOC2_OFF(), mLED_SOC3_OFF(), mLED_SOC4_OFF())

#define mLCD_BL_ON()				GPIO_SetBit(GPIO_LCD_BL, PIN_LCD_BL)
#define mLCD_BL_OFF()				GPIO_ResetBit(GPIO_LCD_BL, PIN_LCD_BL)
#define mLCD_BL_IS_ON()				(GPIO_ReadOutputBit(GPIO_LCD_BL, PIN_LCD_BL) == 1)

#define mLCD_CS_HIGH()				GPIO_SetBit (GPIO_LCD_CS, PIN_LCD_CS)
#define mLCD_CS_LOW()				GPIO_ResetBit(GPIO_LCD_CS, PIN_LCD_CS)
#define mLCD_CS_IS_HIGH()			(GPIO_ReadOutputBit(GPIO_LCD_CS, PIN_LCD_CS) == 1)

#define mLCD_SCL_HIGH()				GPIO_SetBit(GPIO_LCD_SCL, PIN_LCD_SCL)
#define mLCD_SCL_LOW()				GPIO_ResetBit(GPIO_LCD_SCL, PIN_LCD_SCL)
#define mLCD_SCL_IS_HIGH()			(GPIO_ReadOutputBit(GPIO_LCD_SCL, PIN_LCD_SCL) == 1)
#define mLCD_SCL_DATA()				GPIO_ReadInputBit(GPIO_LCD_SCL, PIN_LCD_SCL)

#define mLCD_SDA_HIGH()				GPIO_SetBit(GPIO_LCD_SDA, PIN_LCD_SDA)
#define mLCD_SDA_LOW()				GPIO_ResetBit(GPIO_LCD_SDA, PIN_LCD_SDA)
#define mLCD_SDA_IS_HIGH()			(GPIO_ReadOutputBit(GPIO_LCD_SDA, PIN_LCD_SDA) == 1)
#define mLCD_SDA_DATA()				GPIO_ReadInputBit(GPIO_LCD_SDA, PIN_LCD_SDA)

#define	mLCD_SDA_TYPE_DEF()			GPIO_Config_T configStruct;
#define	mLCD_SDA_OUT_PORT()			RCM_EnableAPB2PeriphClock(CLOCK_LCD_SDA);\
									configStruct.pin = PIN_LCD_SDA;\
									configStruct.mode = GPIO_MODE_OUT_PP;\
									configStruct.speed = GPIO_SPEED_2MHz;\
									GPIO_Config(GPIO_LCD_SDA, &configStruct);

#define	mLCD_SDA_IN_PORT()			RCM_EnableAPB2PeriphClock(CLOCK_LCD_SDA);\
									configStruct.pin = PIN_LCD_SDA;\
									configStruct.mode = GPIO_MODE_IN_PU;\
									configStruct.speed = GPIO_SPEED_2MHz;\
									GPIO_Config(GPIO_LCD_SDA, &configStruct);

#define	mIWDG_RELOAD_COUNTER()		IWDT_Refresh();
#define	mFLASH_UNLOCK()				FMC_Unlock()
#define	mFLASH_LOCK()				FMC_Lock()
#define	mFLASH_PAGE_ERASE(address)	FMC_ErasePage(address);
#define	mFLASH_PROGRAM_16B(address,data)	FMC_ProgramHalfWord(address, data)
#define	mFLASH_READ_PRO_ENABLE()	FMC_EnableReadOutProtection()
#define	mFLASH_READ_PRO_DISABLE()	FMC_DisableReadOutProtection()
#define	mFLASH_GET_READ_PRO()		FMC_GetReadProtectionStatus()

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
INITIAL void	sNVIC_Config(void);
INITIAL void	sSYSTEM_Config(void);
INITIAL void	sSYSTICK_Config(void);
INITIAL void	sPowerOnDelay(void);
INITIAL void	sDelay4us(void);
INITIAL void	sDelay1ms(unsigned char	ms);

/********************************************************************************
* Output interface Routines														*
********************************************************************************/


/********************************************************************************
* Input interface Routines														*
********************************************************************************/

#endif	// __INITIAL_H__

