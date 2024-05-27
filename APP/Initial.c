/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		Initial.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define		__INITIAL_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"Initial.h"
#include	"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
//#define	SYS_INIT_HSE		// 外部高速时钟HSE,最高可配置72MHz
//#define	SYS_INIT_PLL_DIS	// 禁止PLL
#define CAN1_BAUDRATE  125		// 125kBps
#define CAN2_BAUDRATE  500		// 500kBps

/********************************************************************************
* Constants																		*
********************************************************************************/


/********************************************************************************
* External variables															*
********************************************************************************/
extern INT16U wAdcBuffer[cADC_CH_NUM];


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
//static volatile ErrorStatus HSEStartUpStatus = SUCCESS;
//CAN_InitTypeDef        CAN_InitStructure;
//CAN_FilterInitTypeDef  CAN_FilterInitStructure;
CanTxMsg TxMessage;

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
void	sGPIO_Config(void);
void	sIWDG_Config(void);

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void	sGPIO_Config(void)
{
	GPIO_Config_T configStruct;
	
	// PA13/PA14/PA15/PB3/PB4/PD0/PD1需要开启复用时钟才可以控制
	// OSC_IN/OSC_OUT复用为PD0/1必须开启复用时钟
	RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_AFIO);
	// PA13/PA14/PA15/PB3/PB4是JTAG调试口,需禁止JTAG才允许作为IO口使用
	GPIO_ConfigPinRemap(GPIO_REMAP_SWJ_JTAGDISABLE);
	// PD0/PD1需使能映射才允许作为IO口使用
	//GPIO_ConfigPinRemap(GPIO_REMAP_PD01);
	
	RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_GPIOA);
	RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_GPIOB);
	RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_GPIOC);
	RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_GPIOD);
	
	// 调整IO速率,PC13-15口IO速率不能超过2MHz
	// 其他低速率应用也把IO速率调整为2MHz,改善EMI
	mAFE_EEPROM_OFF();
	configStruct.pin = PIN_AFE_SDA;
	configStruct.mode = GPIO_MODE_IN_PU;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_AFE_SDA, &configStruct);
	configStruct.pin = PIN_AFE_SCL;
	configStruct.mode = GPIO_MODE_OUT_PP;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_AFE_SCL, &configStruct);
	configStruct.pin = PIN_AFE_EEPROM_EN;
	configStruct.mode = GPIO_MODE_OUT_PP;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_AFE_EEPROM_EN, &configStruct);
	
	configStruct.pin = PIN_KEY_STATUS;
	configStruct.mode = GPIO_MODE_IN_PU;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_KEY_STATUS, &configStruct);
	configStruct.pin = PIN_LCD_KEY1;
	configStruct.mode = GPIO_MODE_IN_PU;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_LCD_KEY1, &configStruct);
	configStruct.pin = PIN_LCD_KEY2;
	configStruct.mode = GPIO_MODE_IN_PU;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_LCD_KEY2, &configStruct);
	
	mLED_STATUS_ON();
	mLED_ALARM_OFF();
	configStruct.pin = PIN_LED_TEST;
	configStruct.mode = GPIO_MODE_OUT_PP;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_LED_TEST, &configStruct);
	configStruct.pin = PIN_LED_STATUS;
	configStruct.mode = GPIO_MODE_OUT_PP;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_LED_STATUS, &configStruct);
	configStruct.pin = PIN_LED_ALARM;
	configStruct.mode = GPIO_MODE_OUT_PP;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_LED_ALARM, &configStruct);
	
	// LCD
	mLCD_BL_ON();
	mLCD_CS_HIGH();
	mLCD_SCL_HIGH();
	mLCD_SDA_HIGH();
	configStruct.pin = PIN_LCD_BL;
	configStruct.mode = GPIO_MODE_OUT_PP;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_LCD_BL, &configStruct);
	configStruct.pin = PIN_LCD_CS;
	configStruct.mode = GPIO_MODE_OUT_PP;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_LCD_CS, &configStruct);
	configStruct.pin = PIN_LCD_SCL;
	configStruct.mode = GPIO_MODE_OUT_PP;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_LCD_SCL, &configStruct);
	configStruct.pin = PIN_LCD_SDA;
	configStruct.mode = GPIO_MODE_OUT_PP;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_LCD_SDA, &configStruct);
	/*
	mLED_SOC1_OFF();
	mLED_SOC2_OFF();
	mLED_SOC3_OFF();
	mLED_SOC4_OFF();
	configStruct.pin = PIN_LED_SOC1;
	configStruct.mode = GPIO_MODE_OUT_PP;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_LED_SOC1, &configStruct);
	configStruct.pin = PIN_LED_SOC2;
	configStruct.mode = GPIO_MODE_OUT_PP;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_LED_SOC2, &configStruct);
	configStruct.pin = PIN_LED_SOC3;
	configStruct.mode = GPIO_MODE_OUT_PP;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_LED_SOC3, &configStruct);
	configStruct.pin = PIN_LED_SOC4;
	configStruct.mode = GPIO_MODE_OUT_PP;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_LED_SOC4, &configStruct);
	*/
	mSPS_SHUTDOWN_DISABLE();
	configStruct.pin = PIN_SPS_SD;
	configStruct.mode = GPIO_MODE_OUT_PP;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_SPS_SD, &configStruct);
	
	mMOS_CHG_OFF();
	mMOS_DSG_OFF();
	configStruct.pin = PIN_MOS_CHG;
	configStruct.mode = GPIO_MODE_OUT_PP;
	configStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIO_MOS_CHG, &configStruct);
	configStruct.pin = PIN_MOS_DSG;
	configStruct.mode = GPIO_MODE_OUT_PP;
	configStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIO_MOS_DSG, &configStruct);
	
}

void	sADC_Config(void)
{
	GPIO_Config_T configStruct;
	TMR_BaseConfig_T TMR_TimeBaseStruct;
    TMR_OCConfig_T OCcongigStruct;
	DMA_Config_T DMA_ConfigStruct;
	ADC_Config_T ADC_configStruct;
	
	// STEP1:配置GPIO为AD采样通道
	RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_GPIOA);
	RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_GPIOC);
	RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_ADC1);
	RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_DMA1);
	RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR2);
	RCM_ConfigADCCLK(RCM_PCLK2_DIV_4);	// 采样频率PCLK2/4=48MHz/4=12Mhz,不能超过14MHz
	
	configStruct.pin = PIN_ADC_VERSION1;
	configStruct.mode = GPIO_MODE_ANALOG;
	configStruct.speed = GPIO_SPEED_10MHz;
	GPIO_Config(GPIO_ADC_VERSION1, &configStruct);
	
	configStruct.pin = PIN_ADC_NTC_TEMP;
	configStruct.mode = GPIO_MODE_ANALOG;
	configStruct.speed = GPIO_SPEED_10MHz;
	GPIO_Config(GPIO_ADC_NTC_TEMP, &configStruct);
	
	configStruct.pin = PIN_ADC_CELL1_TEMP;
	configStruct.mode = GPIO_MODE_ANALOG;
	configStruct.speed = GPIO_SPEED_10MHz;
	GPIO_Config(GPIO_ADC_CELL1_TEMP, &configStruct);
	
	configStruct.pin = PIN_ADC_CELL2_TEMP;
	configStruct.mode = GPIO_MODE_ANALOG;
	configStruct.speed = GPIO_SPEED_10MHz;
	GPIO_Config(GPIO_ADC_CELL2_TEMP, &configStruct);
	
	configStruct.pin = PIN_ADC_CELL3_TEMP;
	configStruct.mode = GPIO_MODE_ANALOG;
	configStruct.speed = GPIO_SPEED_10MHz;
	GPIO_Config(GPIO_ADC_CELL3_TEMP, &configStruct);
	
	configStruct.pin = PIN_ADC_CELL4_TEMP;
	configStruct.mode = GPIO_MODE_ANALOG;
	configStruct.speed = GPIO_SPEED_10MHz;
	GPIO_Config(GPIO_ADC_CELL4_TEMP, &configStruct);
	
	// STEP2:配置定时器,用于定时触发ADC转换
	TMR_TimeBaseStruct.division = 0;	// 120MHz/(0+1)/6000=20000
	TMR_TimeBaseStruct.countMode = TMR_COUNTER_MODE_CENTERALIGNED3;
	TMR_TimeBaseStruct.period = 1200;	// 48MHz/2/20kHz/1
	TMR_TimeBaseStruct.clockDivision = TMR_CLOCK_DIV_1;
	TMR_TimeBaseStruct.repetitionCounter = 0;
	TMR_ConfigTimeBase(TMR2, &TMR_TimeBaseStruct);
	
	OCcongigStruct.mode = TMR_OC_MODE_PWM1;
	OCcongigStruct.outputState = TMR_OC_STATE_ENABLE;
	OCcongigStruct.pulse = 600;
	OCcongigStruct.polarity = TMR_OC_POLARITY_HIGH;
	TMR_ConfigOC2(TMR2, &OCcongigStruct);
	
	// STEP3:配置DMA,用于获取ADC转换结果
	DMA_Reset(DMA1_Channel1);
	DMA_ConfigStruct.peripheralBaseAddr = (INT32U)&ADC1->REGDATA;
	DMA_ConfigStruct.memoryBaseAddr = (INT32U)&wAdcBuffer;
	DMA_ConfigStruct.dir = DMA_DIR_PERIPHERAL_SRC;
	DMA_ConfigStruct.bufferSize = cADC_CH_NUM;
	DMA_ConfigStruct.peripheralInc = DMA_PERIPHERAL_INC_DISABLE;
	DMA_ConfigStruct.memoryInc = DMA_MEMORY_INC_ENABLE;
	DMA_ConfigStruct.peripheralDataSize = DMA_PERIPHERAL_DATA_SIZE_HALFWORD;
	DMA_ConfigStruct.memoryDataSize = DMA_MEMORY_DATA_SIZE_HALFWORD;
	DMA_ConfigStruct.loopMode = DMA_MODE_CIRCULAR;
	DMA_ConfigStruct.priority = DMA_PRIORITY_HIGH;
	DMA_ConfigStruct.M2M = DMA_M2MEN_DISABLE;
	DMA_Config(DMA1_Channel1, &DMA_ConfigStruct);
	DMA_ClearIntFlag(DMA1_INT_FLAG_TC1);
	DMA_EnableInterrupt(DMA1_Channel1, DMA_INT_TC);
	DMA_Enable(DMA1_Channel1);
	
	// STEP4:配置ADC
	ADC_configStruct.mode = ADC_MODE_INDEPENDENT;
	ADC_configStruct.scanConvMode = ENABLE;
	ADC_configStruct.continuosConvMode = DISABLE;
	ADC_configStruct.externalTrigConv = ADC_EXT_TRIG_CONV_TMR2_CC2;
	ADC_configStruct.dataAlign = ADC_DATA_ALIGN_RIGHT;
	ADC_configStruct.nbrOfChannel = cADC_CH_NUM;
	ADC_Config(ADC1, &ADC_configStruct);
	
	// STEP5:配置采样通道顺序(ADC_Channel_X与硬件采样顺序对应)
	// 每个采样通道总的时间=采样时间+转换时间(为固定值7.5周期)=13.5+7.5=15周期 = 15/12MHz=1.25us
	ADC_ConfigRegularChannel(ADC1, ADC_CHANNEL_0, cADC_CH_NTC_TEMP + 1, ADC_SAMPLETIME_7CYCLES5);
	ADC_ConfigRegularChannel(ADC1, ADC_CHANNEL_1, cADC_CH_VERSION1 + 1, ADC_SAMPLETIME_7CYCLES5);
	ADC_ConfigRegularChannel(ADC1, ADC_CHANNEL_10, cADC_CH_CELL1_TEMP + 1, ADC_SAMPLETIME_7CYCLES5);
	ADC_ConfigRegularChannel(ADC1, ADC_CHANNEL_11, cADC_CH_CELL2_TEMP + 1, ADC_SAMPLETIME_7CYCLES5);
	ADC_ConfigRegularChannel(ADC1, ADC_CHANNEL_12, cADC_CH_CELL3_TEMP + 1, ADC_SAMPLETIME_7CYCLES5);
	ADC_ConfigRegularChannel(ADC1, ADC_CHANNEL_13, cADC_CH_CELL4_TEMP + 1, ADC_SAMPLETIME_7CYCLES5);
	
	ADC_EnableExternalTrigConv(ADC1);
	ADC_EnableDMA(ADC1);
	
	// STEP6:ADC校准
	ADC_Enable(ADC1);								/* Enable ADC1 reset calibaration register */
	sDelay1ms(1);
	ADC_ResetCalibration(ADC1);
	while(ADC_ReadResetCalibrationStatus(ADC1)); 	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);
	while(ADC_ReadCalibrationStartFlag(ADC1));
	
	// STEP7:启动定时ADC
	TMR_Enable(TMR2);	// 20kHz
}

void	sIWDG_Config(void)
{
	// 独立看门狗使用内部专用40kHz低速时钟(LSI)
	// 独立看门狗设置的超时时间=1563/(40kHz/128)=5s
	
	// STEP1:使能IWDG分频系数IWDG_PR和加载值IWDG_RLR
	RCM_EnableLSI();
	IWDT_EnableWriteAccess();
	
	// STEP2:设置IWDG分频系数(=40kHz/128=)
	IWDT_ConfigDivider(IWDT_DIVIDER_128);
	
	// STEP3:设置IWDG计数值(0-4095可设置)
	IWDT_ConfigReload(1563);	// =1563/(40kHz/128)=5s
	
	// STEP4:复位IWDG计数值
	IWDT_Refresh();
	
	// STEP5:使能IWDG
	IWDT_Enable();
}

void	sUSART1_Config(void)		// to Host
{
	GPIO_Config_T configStruct;
	USART_Config_T USART_ConfigStruct;
	
	// STEP1:使能时钟 USART1&USART3
	RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_USART1);
	RCM_EnableAPB2PeriphClock(CLOCK_SCI_RJ45_TXD);
	RCM_EnableAPB2PeriphClock(CLOCK_SCI_RJ45_RXD);
    
	// STEP2:初始化GPIO
	configStruct.pin = PIN_SCI_RJ45_TXD;
	configStruct.mode = GPIO_MODE_AF_PP;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_SCI_RJ45_TXD, &configStruct);
	
	configStruct.pin = PIN_SCI_RJ45_RXD;
	configStruct.mode = GPIO_MODE_IN_FLOATING;
	configStruct.speed = GPIO_SPEED_2MHz;
	GPIO_Config(GPIO_SCI_RJ45_RXD, &configStruct);
	
	// STEP3:初始化串口
	USART_ConfigStruct.baudRate = 9600;
    USART_ConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    USART_ConfigStruct.mode = USART_MODE_TX_RX;
    USART_ConfigStruct.parity = USART_PARITY_NONE;
    USART_ConfigStruct.stopBits = USART_STOP_BIT_1;
    USART_ConfigStruct.wordLength = USART_WORD_LEN_8B;
    USART_Config(USART1, &USART_ConfigStruct);
	USART_EnableInterrupt(USART1, USART_INT_RXBNE);
	
	// STEP4:使能串口
	USART_Enable(USART1);
}

void	sCAN1_Config(void)
{
	GPIO_Config_T configStruct;
	CAN_Config_T canConfig;
    CAN_FilterConfig_T FilterStruct;
	
	// STEP1:使能时钟CAN1
	RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_CAN1);
	RCM_EnableAPB2PeriphClock(CLOCK_CAN_RX);
	RCM_EnableAPB2PeriphClock(CLOCK_CAN_TX);
	RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_AFIO);
	
	// STEP2:初始化GPIO
	configStruct.pin = PIN_CAN_RX;
	configStruct.mode = GPIO_MODE_IN_PU;
	configStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIO_CAN_RX, &configStruct);
	
	configStruct.pin = PIN_CAN_TX;
	configStruct.mode = GPIO_MODE_AF_PP;
	configStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIO_CAN_TX, &configStruct);
	
	GPIO_ConfigPinRemap(GPIO_REMAP1_CAN1);
	//GPIO_ConfigPinRemap(GPIO_REMAP2_CAN1);
	
	// STEP3:复位
	CAN_Reset(CAN1);
	
	// STEP4:初始化CAN
	CAN_ConfigStructInit(&canConfig);
	canConfig.autoBusOffManage = ENABLE;
	canConfig.autoWakeUpMode = DISABLE;
	canConfig.nonAutoRetran = ENABLE;
	canConfig.rxFIFOLockMode = DISABLE;
	canConfig.txFIFOPriority = ENABLE;
	canConfig.mode = CAN_MODE_NORMAL;
	canConfig.syncJumpWidth = CAN_SJW_1;
	canConfig.timeSegment1 = CAN_TIME_SEGMENT1_10;
	canConfig.timeSegment2 = CAN_TIME_SEGMENT2_5;
#if CAN1_BAUDRATE == 500		// 500KBps
	canConfig.prescaler = 3;
#elif CAN1_BAUDRATE == 250	// 250KBps
	canConfig.prescaler = 6;	// (SJW+BS1)/(SJW+BS1+BS2)~=80%,且SJW~=1us
#elif  CAN1_BAUDRATE == 125	// 125KBps/		// PCLK1为主频的一半
	canConfig.prescaler = 12;	// 分频设置:PCLK1/(SJW+BS1+BS2)/1=24MHz/24/1=1MHz
#elif  CAN1_BAUDRATE == 100	// 100KBps/		// PCLK1为主频的一半
	canConfig.prescaler = 15;	// 分频设置:PCLK1/(SJW+BS1+BS2)/1=24MHz/24/1=1MHz
#else
#error "Please select first the CAN Baudrate in Private defines in main.c "
#endif
	CAN_Config(CAN1, &canConfig);
	
	// STEP5:初始化CAN滤波器
	FilterStruct.filterNumber = 1;
	FilterStruct.filterMode = CAN_FILTER_MODE_IDMASK;
	FilterStruct.filterScale = CAN_FILTER_SCALE_32BIT;
	FilterStruct.filterIdHigh = 0x0000;
	FilterStruct.filterIdLow = 0x0000;
	FilterStruct.filterMaskIdHigh = 0x0000;
	FilterStruct.filterMaskIdLow = 0x0000;
	FilterStruct.filterFIFO = CAN_FILTER_FIFO_0;
	FilterStruct.filterActivation = ENABLE;
	CAN_ConfigFilter(CAN1, &FilterStruct);
	
	// STEP6:初始化CAN中断
	// CAN FIFO0 message pending interrupt enable
	// 一旦往FIFO存入一个报文，硬件就会更新FMP[1:0]位，并且如果CAN_IER寄存器的FMPIE位为’1’，那么就会产生一个中断请求。
	// 开启:FIFO0消息挂号中断CAN_IT_FMP0|CAN_IT_FMP1,
	// 发送邮箱空中断CAN_IT_TME, Datasheet.CN:p445
	//CAN_ITConfig(CAN1, CAN_IT_FMP0 | CAN_IT_FMP1 | CAN_IT_TME	// 发送接收中断
	//	| CAN_IT_ERR | CAN_IT_LEC | CAN_IT_BOF | CAN_IT_EPV		// 控制及状态变化中断
	//	| CAN_IT_EWG, ENABLE);	// 使能指定的CAN中断
	CAN_EnableInterrupt(CAN1, CAN_INT_F0MP | CAN_INT_F1MP | CAN_INT_TXME);	// 使能指定的CAN中断,发送接收中断
	
	// 初始化其它寄存器
	CAN1_PERIPH->ESR |= CAN_ESR_LEC_USER;	// 将上次错误号设置为用户状态(程序不会设置该状态)以区分新的状态
	
	// STEP7:初始化发送数据
	TxMessage.StdId = 0x0;
	TxMessage.ExtId = 0x0;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.IDE = CAN_ID_EXT;
	TxMessage.DLC = 0;
}

void	sCAN2_Config(void)
{
	GPIO_Config_T configStruct;
	CAN_Config_T canConfig;
    CAN_FilterConfig_T FilterStruct;
	
	// STEP1:使能时钟CAN2
	RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_CAN2);
	RCM_EnableAPB2PeriphClock(CLOCK_CAN_PCS_RX);
	RCM_EnableAPB2PeriphClock(CLOCK_CAN_PCS_TX);
	RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_AFIO);
	
	// STEP2:初始化GPIO
	configStruct.pin = PIN_CAN_PCS_RX;
	configStruct.mode = GPIO_MODE_IN_PU;
	configStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIO_CAN_PCS_RX, &configStruct);
	
	configStruct.pin = PIN_CAN_PCS_TX;
	configStruct.mode = GPIO_MODE_AF_PP;
	configStruct.speed = GPIO_SPEED_50MHz;
	GPIO_Config(GPIO_CAN_PCS_TX, &configStruct);
	
	GPIO_ConfigPinRemap(GPIO_REMAP_CAN2);
	
	// STEP3:复位
	CAN_Reset(CAN2);
	
	// STEP4:初始化CAN
	CAN_ConfigStructInit(&canConfig);
	canConfig.autoBusOffManage = ENABLE;
	canConfig.autoWakeUpMode = DISABLE;
	canConfig.nonAutoRetran = ENABLE;
	canConfig.rxFIFOLockMode = DISABLE;
	canConfig.txFIFOPriority = ENABLE;
	canConfig.mode = CAN_MODE_NORMAL;
	canConfig.syncJumpWidth = CAN_SJW_1;
	canConfig.timeSegment1 = CAN_TIME_SEGMENT1_10;
	canConfig.timeSegment2 = CAN_TIME_SEGMENT2_5;
#if CAN2_BAUDRATE == 500		// 500KBps
	canConfig.prescaler = 3;
#elif CAN2_BAUDRATE == 250	// 250KBps
	canConfig.prescaler = 6;	// (SJW+BS1)/(SJW+BS1+BS2)~=80%,且SJW~=1us
#elif  CAN2_BAUDRATE == 125	// 125KBps/		// PCLK1为主频的一半
	canConfig.prescaler = 12;	// 分频设置:PCLK1/(SJW+BS1+BS2)/1=24MHz/24/1=1MHz
#elif  CAN2_BAUDRATE == 100	// 100KBps/		// PCLK1为主频的一半
	canConfig.prescaler = 15;	// 分频设置:PCLK1/(SJW+BS1+BS2)/1=24MHz/24/1=1MHz
#else
#error "Please select first the CAN Baudrate in Private defines in main.c "
#endif
	CAN_Config(CAN2, &canConfig);
	
	// STEP5:初始化CAN滤波器
	FilterStruct.filterNumber = 14;
	FilterStruct.filterMode = CAN_FILTER_MODE_IDMASK;
	FilterStruct.filterScale = CAN_FILTER_SCALE_32BIT;
	FilterStruct.filterIdHigh = 0x0000;
	FilterStruct.filterIdLow = 0x0000;
	FilterStruct.filterMaskIdHigh = 0x0000;
	FilterStruct.filterMaskIdLow = 0x0000;
	FilterStruct.filterFIFO = CAN_FILTER_FIFO_1;
	FilterStruct.filterActivation = ENABLE;
	CAN_ConfigFilter(CAN2, &FilterStruct);
	
	// STEP6:初始化CAN中断
	// CAN FIFO0 message pending interrupt enable
	// 一旦往FIFO存入一个报文，硬件就会更新FMP[1:0]位，并且如果CAN_IER寄存器的FMPIE位为’1’，那么就会产生一个中断请求。
	// 开启:FIFO0消息挂号中断CAN_IT_FMP0|CAN_IT_FMP1,
	// 发送邮箱空中断CAN_IT_TME, Datasheet.CN:p445
	//CAN_ITConfig(CAN1, CAN_IT_FMP0 | CAN_IT_FMP1 | CAN_IT_TME	// 发送接收中断
	//	| CAN_IT_ERR | CAN_IT_LEC | CAN_IT_BOF | CAN_IT_EPV		// 控制及状态变化中断
	//	| CAN_IT_EWG, ENABLE);	// 使能指定的CAN中断
	CAN_EnableInterrupt(CAN2, CAN_INT_F0MP | CAN_INT_F1MP | CAN_INT_TXME);	// 使能指定的CAN中断,发送接收中断
	
	// 初始化其它寄存器
	CAN2_PERIPH->ESR |= CAN_ESR_LEC_USER;	// 将上次错误号设置为用户状态(程序不会设置该状态)以区分新的状态
	
	// STEP7:初始化发送数据
	TxMessage.StdId = 0x0;
	TxMessage.ExtId = 0x0;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.IDE = CAN_ID_EXT;
	TxMessage.DLC = 0;
}

void	sSYSTICK_Config(void)
{
	SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC);
}

void	sNVIC_Config(void) 
{
	NVIC_ConfigPriorityGroup(NVIC_PRIORITY_GROUP_3);
	
	NVIC_EnableIRQRequest(DMA1_Channel1_IRQn, 0, 0);
	
	NVIC_EnableIRQRequest(USBD1_LP_CAN1_RX0_IRQn, 1, 0);
	NVIC_EnableIRQRequest(CAN1_RX1_IRQn, 1, 1);
	NVIC_EnableIRQRequest(USBD1_HP_CAN1_TX_IRQn, 2, 0);
	NVIC_EnableIRQRequest(CAN1_SCE_IRQn, 7, 0);
	
	NVIC_EnableIRQRequest(USBD2_LP_CAN2_RX0_IRQn, 3, 0);
	NVIC_EnableIRQRequest(CAN2_RX1_IRQn, 3, 1);
	NVIC_EnableIRQRequest(USBD2_HP_CAN2_TX_IRQn, 4, 0);
	NVIC_EnableIRQRequest(CAN2_SCE_IRQn, 7, 1);
	
    NVIC_EnableIRQRequest(USART1_IRQn, 5, 0);
}

void	sSYSTEM_Config(void)
{
	sIWDG_Config();
	
	sGPIO_Config();
	
	sADC_Config();
	
	sCAN1_Config();
	
	sCAN2_Config();
	
	sUSART1_Config();
}

void	sPowerOnDelay(void)
{
	INT16U i,j;
	
	for(j = 0; j < 10; j++)
	{
		for(i = 0; i < 0xFFFF; i++)
		{
			__nop();
		}
	}
}

void	sDelay4us(void)
{
	INT8U	xxx;
	
	for(xxx = 0; xxx< 40; xxx++)	// 实测120MHz时100＝5us,如果用于IIC则对应100kHz频率
	{
		__asm("nop");
	}
}

void	sDelay1ms(INT8U	ms)
{
	INT8U	yyy;
	INT16U	xxx;
	
	for(yyy = 0; yyy < ms; yyy++)
	{
		for(xxx = 0; xxx < 8000; xxx++)	// 实测120MHz时20000＝1ms
		{
			__asm("nop");
		}
	}
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/


/********************************************************************************
* Input interface Routines														*
********************************************************************************/


