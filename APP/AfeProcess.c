/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		AfeProcess.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define			__AFE_PROCESS_C__

/********************************************************************************
* Include head files                                                            *
********************************************************************************/
#include		"Main.h"
#include		"AfeProcess.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define		EEPROM_SCONF_CN_5CELL		0x5
#define		EEPROM_SCONF_CN_6CELL		0x6
#define		EEPROM_SCONF_CN_7CELL		0x7
#define		EEPROM_SCONF_CN_8CELL		0x8
#define		EEPROM_SCONF_CN_9CELL		0x9
#define		EEPROM_SCONF_CN_10CELL		0xA
#define		EEPROM_SCONF_CN_11CELL		0xB
#define		EEPROM_SCONF_CN_12CELL		0xC
#define		EEPROM_SCONF_CN_13CELL		0xD
#define		EEPROM_SCONF_CN_14CELL		0xE
#define		EEPROM_SCONF_CN_15CELL		0xF
#define		EEPROM_SCONF_CN_16CELL		0x0

#define		EEPROM_OVT_100MS			0x0
#define		EEPROM_OVT_200MS			0x1
#define		EEPROM_OVT_300MS			0x2
#define		EEPROM_OVT_400MS			0x3
#define		EEPROM_OVT_600MS			0x4
#define		EEPROM_OVT_800MS			0x5
#define		EEPROM_OVT_1S				0x6
#define		EEPROM_OVT_2S				0x7
#define		EEPROM_OVT_3S				0x8
#define		EEPROM_OVT_4S				0x9
#define		EEPROM_OVT_6S				0xA
#define		EEPROM_OVT_8S				0xB
#define		EEPROM_OVT_10S				0xC
#define		EEPROM_OVT_20S				0xD
#define		EEPROM_OVT_30S				0xE
#define		EEPROM_OVT_40S				0xF

#define		EEPROM_UVT_100MS			0x0
#define		EEPROM_UVT_200MS			0x1
#define		EEPROM_UVT_300MS			0x2
#define		EEPROM_UVT_400MS			0x3
#define		EEPROM_UVT_600MS			0x4
#define		EEPROM_UVT_800MS			0x5
#define		EEPROM_UVT_1S				0x6
#define		EEPROM_UVT_2S				0x7
#define		EEPROM_UVT_3S				0x8
#define		EEPROM_UVT_4S				0x9
#define		EEPROM_UVT_6S				0xA
#define		EEPROM_UVT_8S				0xB
#define		EEPROM_UVT_10S				0xC
#define		EEPROM_UVT_20S				0xD
#define		EEPROM_UVT_30S				0xE
#define		EEPROM_UVT_40S				0xF

#define		EEPROM_OCD1V_20MV			0x0
#define		EEPROM_OCD1V_30MV			0x1
#define		EEPROM_OCD1V_40MV			0x2
#define		EEPROM_OCD1V_50MV			0x3
#define		EEPROM_OCD1V_60MV			0x4
#define		EEPROM_OCD1V_70MV			0x5
#define		EEPROM_OCD1V_80MV			0x6
#define		EEPROM_OCD1V_90MV			0x7
#define		EEPROM_OCD1V_100MV			0x8
#define		EEPROM_OCD1V_110MV			0x9
#define		EEPROM_OCD1V_120MV			0xA
#define		EEPROM_OCD1V_130MV			0xB
#define		EEPROM_OCD1V_140MV			0xC
#define		EEPROM_OCD1V_160MV			0xD
#define		EEPROM_OCD1V_180MV			0xE
#define		EEPROM_OCD1V_200MV			0xF

#define		EEPROM_OCD1T_50MS			0x0
#define		EEPROM_OCD1T_100MS			0x1
#define		EEPROM_OCD1T_200MS			0x2
#define		EEPROM_OCD1T_400MS			0x3
#define		EEPROM_OCD1T_600MS			0x4
#define		EEPROM_OCD1T_800MS			0x5
#define		EEPROM_OCD1T_1S				0x6
#define		EEPROM_OCD1T_2S				0x7
#define		EEPROM_OCD1T_4S				0x8
#define		EEPROM_OCD1T_6S				0x9
#define		EEPROM_OCD1T_8S				0xA
#define		EEPROM_OCD1T_10S			0xB
#define		EEPROM_OCD1T_15S			0xC
#define		EEPROM_OCD1T_20S			0xD
#define		EEPROM_OCD1T_30S			0xE
#define		EEPROM_OCD1T_40S			0xF

#define		EEPROM_OCD2V_30MV			0x0
#define		EEPROM_OCD2V_40MV			0x1
#define		EEPROM_OCD2V_50MV			0x2
#define		EEPROM_OCD2V_60MV			0x3
#define		EEPROM_OCD2V_70MV			0x4
#define		EEPROM_OCD2V_80MV			0x5
#define		EEPROM_OCD2V_90MV			0x6
#define		EEPROM_OCD2V_100MV			0x7
#define		EEPROM_OCD2V_120MV			0x8
#define		EEPROM_OCD2V_140MV			0x9
#define		EEPROM_OCD2V_160MV			0xA
#define		EEPROM_OCD2V_180MV			0xB
#define		EEPROM_OCD2V_200MV			0xC
#define		EEPROM_OCD2V_300MV			0xD
#define		EEPROM_OCD2V_400MV			0xE
#define		EEPROM_OCD2V_500MV			0xF

#define		EEPROM_OCD2T_10MS			0x0
#define		EEPROM_OCD2T_20MS			0x1
#define		EEPROM_OCD2T_40MS			0x2
#define		EEPROM_OCD2T_60MS			0x3
#define		EEPROM_OCD2T_80MS			0x4
#define		EEPROM_OCD2T_100MS			0x5
#define		EEPROM_OCD2T_200MS			0x6
#define		EEPROM_OCD2T_400MS			0x7
#define		EEPROM_OCD2T_600MS			0x8
#define		EEPROM_OCD2T_800MS			0x9
#define		EEPROM_OCD2T_1S				0xA
#define		EEPROM_OCD2T_2S				0xB
#define		EEPROM_OCD2T_4S				0xC
#define		EEPROM_OCD2T_8S				0xD
#define		EEPROM_OCD2T_10S			0xE
#define		EEPROM_OCD2T_20S			0xF

#define		EEPROM_SCV_50MV				0x0
#define		EEPROM_SCV_80MV				0x1
#define		EEPROM_SCV_110MV			0x2
#define		EEPROM_SCV_140MV			0x3
#define		EEPROM_SCV_170MV			0x4
#define		EEPROM_SCV_200MV			0x5
#define		EEPROM_SCV_230MV			0x6
#define		EEPROM_SCV_260MV			0x7
#define		EEPROM_SCV_290MV			0x8
#define		EEPROM_SCV_320MV			0x9
#define		EEPROM_SCV_350MV			0xA
#define		EEPROM_SCV_400MV			0xB
#define		EEPROM_SCV_500MV			0xC
#define		EEPROM_SCV_600MV			0xD
#define		EEPROM_SCV_800MV			0xE
#define		EEPROM_SCV_1000MV			0xF

#define		EEPROM_SCT_0US				0x0
#define		EEPROM_SCT_64US				0x1
#define		EEPROM_SCT_128US			0x2
#define		EEPROM_SCT_192US			0x3
#define		EEPROM_SCT_256US			0x4
#define		EEPROM_SCT_320US			0x5
#define		EEPROM_SCT_384US			0x6
#define		EEPROM_SCT_448US			0x7
#define		EEPROM_SCT_512US			0x8
#define		EEPROM_SCT_576US			0x9
#define		EEPROM_SCT_640US			0xA
#define		EEPROM_SCT_704US			0xB
#define		EEPROM_SCT_768US			0xC
#define		EEPROM_SCT_832US			0xD
#define		EEPROM_SCT_896US			0xE
#define		EEPROM_SCT_960US			0xF

#define		EEPROM_OCCV_20MV			0x0
#define		EEPROM_OCCV_30MV			0x1
#define		EEPROM_OCCV_40MV			0x2
#define		EEPROM_OCCV_50MV			0x3
#define		EEPROM_OCCV_60MV			0x4
#define		EEPROM_OCCV_70MV			0x5
#define		EEPROM_OCCV_80MV			0x6
#define		EEPROM_OCCV_90MV			0x7
#define		EEPROM_OCCV_100MV			0x8
#define		EEPROM_OCCV_110MV			0x9
#define		EEPROM_OCCV_120MV			0xA
#define		EEPROM_OCCV_130MV			0xB
#define		EEPROM_OCCV_140MV			0xC
#define		EEPROM_OCCV_160MV			0xD
#define		EEPROM_OCCV_180MV			0xE
#define		EEPROM_OCCV_200MV			0xF

#define		EEPROM_OCCT_10MS			0x0
#define		EEPROM_OCCT_20MS			0x1
#define		EEPROM_OCCT_40MS			0x2
#define		EEPROM_OCCT_60MS			0x3
#define		EEPROM_OCCT_80MS			0x4
#define		EEPROM_OCCT_100MS			0x5
#define		EEPROM_OCCT_200MS			0x6
#define		EEPROM_OCCT_400MS			0x7
#define		EEPROM_OCCT_600MS			0x8
#define		EEPROM_OCCT_800MS			0x9
#define		EEPROM_OCCT_1S				0xA
#define		EEPROM_OCCT_2S				0xB
#define		EEPROM_OCCT_4S				0xC
#define		EEPROM_OCCT_8S				0xD
#define		EEPROM_OCCT_10S				0xE
#define		EEPROM_OCCT_20S				0xF

#define		EEPROM_OV_5MV				5
#define		EEPROM_OVR_5MV				5
#define		EEPROM_UV_20MV				20
#define		EEPROM_UVR_20MV				20
#define		EEPROM_BALV_20MV			20
#define		EEPROM_PREV_20MV			20
#define		EEPROM_L0V_20MV				20
#define		EEPROM_PFV_20MV				20

#define		EEPROM_OCRT_8S				0x0
#define		EEPROM_OCRT_16S				0x1
#define		EEPROM_OCRT_32S				0x2
#define		EEPROM_OCRT_64S				0x3

#define		EEPROM_PFT_8S				0x0
#define		EEPROM_PFT_16S				0x1
#define		EEPROM_PFT_32S				0x2
#define		EEPROM_PFT_64S				0x3

#define		EEPROM_CHS_200UV			0x0
#define		EEPROM_CHS_500UV			0x1
#define		EEPROM_CHS_1000UV			0x2
#define		EEPROM_CHS_2000UV			0x3

#define		EEPROM_MOST_64US			0x0
#define		EEPROM_MOST_128US			0x1
#define		EEPROM_MOST_256US			0x2
#define		EEPROM_MOST_512US			0x3

#define		EEPROM_WDT_32S				0x0
#define		EEPROM_WDT_16S				0x1
#define		EEPROM_WDT_8S				0x2
#define		EEPROM_WDT_4S				0x3

#define		EEPROM_LDRT_100MS			0x0
#define		EEPROM_LDRT_500MS			0x1
#define		EEPROM_LDRT_1S				0x2
#define		EEPROM_LDRT_2S				0x3

// ENPCH=0:禁用预充电功能
// ENPCH=1:启用预充电功能
#define		EEPROM_ENPCH_DSIABLE		0	// DEFAULT
#define		EEPROM_ENPCH_ENABLE			1

// ENMOS=0:禁用充电MOS恢复控制位
// ENMOS=1:启用充电MOS恢复控制位(充电MOS关闭下如果检测到放电则强制打开充电MOS)
#define		EEPROM_ENMOS_DSIABLE		0
#define		EEPROM_ENMOS_ENABLE			1	// DEFAULT

// OCPM=0:充电过流只关充电MOS，放电过流只关放电MOS
// OCPM=1:电流保护关闭充、放电MOS
#define		EEPROM_OCPM_CHG_OR_DSG		0	// DEFAULT
#define		EEPROM_OCPM_CHG_AND_DSG		1

// BAL=0:平衡开启由SH367309内部逻辑控制
// BAL=1:平衡开启由外部MCU控制,平衡时序由SH367309内部逻辑控制
#define		EEPROM_BAL_AFE				0
#define		EEPROM_BAL_MCU				1	// DEFAULT

// E0VB=0:关闭“禁止低压充电功能”
// E0VB=1:开启“禁止低压充电功能”
#define		EEPROM_E0VB_DISABLE			0	// DEFAULT
#define		EEPROM_E0VB_ENABLE			1

// DIS_PF=0:启用二次过充电保护
// DIS_PF=1:禁用二次过充电保护
#define		EEPROM_DIS_PF_ENABLE		0	// DEFAULT
#define		EEPROM_DIS_PF_DISABLE		1	// 如果报二段过压,采样会停止,AFE必须重启/软复位/进退SHIP等可以恢复

// CTLC=0:充放电与预充电MOS由内部逻辑控制,CTLC管脚输入无效
// CTLC=1:控制充电和预充电MOS,CTLC输入低电平强制关闭充电和预充电MOS,高电平则由内部逻辑控制
// CTLC=2:控制放电MOS,CTLC输入低电平强制关闭放电MOS,高电平则由内部逻辑控制
// CTLC=3:控制充放电和预充电MOS,CTLC输入低电平强制关闭充放电和预充电MOS,高电平则由内部逻辑控制
#define		EEPROM_CTLC_DISABLE			0	// DEFAULT
#define		EEPROM_CTLC_CHG				1
#define		EEPROM_CTLC_DSG				2
#define		EEPROM_CTLC_CHG_DSG			3

// OCRA=0:不允许电流保护定时恢复
// OCRA=1:允许电流保护定时恢复
#define		EEPROM_OCRA_DISABLE			0
#define		EEPROM_OCRA_ENABLE			1	// DEFAULT

// EUVR=0:过放电保护状态释放与负载释放无关
// EUVR=1:过放电保护状态释放还需负载释放
#define		EEPROM_EUVR_DISABLE			0	// DEFAULT
#define		EEPROM_EUVR_ENABLE			1

// UV_OP=0:过放电只关闭放电MOS
// UV_OP=1:过放电关闭充放电MOS
#define		EEPROM_UV_OP_DSG_OFF		0	// DEFAULT
#define		EEPROM_UV_OP_CHG_DSG_OFF	1

/********************************************************************************
* Constants																		*
********************************************************************************/
const	INT16U	NTC103AT[161] = {
	32950,	30970,	29124,	27404,	25799,	24770,	23335,	21995,	20743,	19572,	
	18850,	17796,	16810,	15885,	15019,	14410,	13633,	12904,	12219,	11576,	
	11130,	10550,	10005,	9492,	9009,	8643,	8208,	7798,	7412,	7048,	
	6777,	6447,	6136,	5842,	5564,	5341,	5090,	4852,	4627,	4414,	
	4247,	4053,	3870,	3696,	3532,	3390,	3241,	3099,	2964,	2836,	
	2728,	2611,	2501,	2395,	2295,	2205,	2114,	2027,	1944,	1866,	
	1796,	1724,	1655,	1590,	1527,	1469,	1412,	1357,	1305,	1256,	
	1209,	1163,	1120,	1078,	1038,	1000,	963,	928,	894,	862,	
	831,	801,	772,	745,	719,	694,	670,	646,	624,	603,	
	583,	562,	543,	525,	508,	491,	474,	459,	444,	430,	
	416,	402,	389,	377,	365,	354,	342,	331,	321,	311,	
	302,	292,	283,	275,	267,	259,	251,	243,	236,	229,	
	223,	216,	210,	204,	198,	192,	186,	181,	176,	171,	
	167,	162,	157,	153,	149,	145,	141,	137,	133,	130,	
	127,	123,	120,	117,	114,	111,	108,	105,	102,	100,	
	97,		95,		92,		90,		88,		86,		83,		81,		79,		78,
	76};

/********************************************************************************
* External variables															*
********************************************************************************/


/********************************************************************************
* External routine prototypes													*
********************************************************************************/


/********************************************************************************
* Input variables																*
********************************************************************************/
typedef struct	_SYSINFOR_							
{
	INT16U		VCell[16];
	INT32U		Voltage;
	INT32S		CurCadc;
	INT16U		Temperature1;
	INT16U		Temperature2;
	INT16U		Temperature3;
	INT32U		E2ulFCC;
	INT32U		RC;
	INT16U		RSOC;
	INT16U		E2uiCycleCount;
	INT16U		PackStatus;
	INT16U		BatStatus;
	INT16U		PackConfig;
	INT16U		ManufactureAccess;
}SYSINFOR;
SYSINFOR  Info;

INT16U wPackStatus;

// 此结构体必须确保数据长度对齐
// 比如前三位数据:INT8,INT16,INT8,INT8,INT8则是不允许,会导致获取的数据出错
// 正确的数据结构:INT8,INT8,INT16,INT8,INT8由于数据长度是对齐的，所以允许
typedef struct	_AFE_EEPROM_
{
	union SCONF1_TABLE
	{
		struct SCONF1_BIT_FEILD
		{
			INT8U CN:4;
			INT8U BAL:1;
			INT8U OCPM:1;
			INT8U ENMOS:1;
			INT8U ENPCH:1;
		} bits;
		INT8U bData;
	} SCONF1;
	union SCONF2_TABLE
	{
		struct SCONF2_BIT_FEILD
		{
			INT8U EUVR:1;
			INT8U OCRA:1;
			INT8U CTLC:2;
			INT8U DIS_PF:1;
			INT8U UV_OP:1;
			INT8U RSVD:1;
			INT8U E0VB:1;
		} bits;
		INT8U bData;
	} SCONF2;
	union OVT_LDRT_OVH_TABLE
	{
		struct OVT_LDRT_OVH_BIT_FEILD
		{
			INT8U OVH:2;
			INT8U LDRT:2;
			INT8U OVT:4;
		} bits;
		INT8U bData;
	} OVT_LDRT_OVH;
	INT8U OVL;
	union UVT_OVRH_TABLE
	{
		struct UVT_OVRH_BIT_FEILD
		{
			INT8U OVRH:2;
			INT8U RSVD:2;
			INT8U UVT:4;
		} bits;
		INT8U bData;
	} UVT_OVRH;
	INT8U OVRL;
	INT8U UV;
	INT8U UVR;
	INT8U BALV;
	INT8U PREV;
	INT8U L0V;
	INT8U PFV;
	union OCD1V_OCD1T_TABLE
	{
		struct OCD1V_OCD1T_BIT_FEILD
		{
			INT8U OCD1T:4;
			INT8U OCD1V:4;
		} bits;
		INT8U bData;
	} OCD1V_OCD1T;
	union OCD2V_OCD2T_TABLE
	{
		struct OCD2V_OCD2T_BIT_FEILD
		{
			INT8U OCD2T:4;
			INT8U OCD2V:4;
		} bits;
		INT8U bData;
	} OCD2V_OCD2T;
	union SCV_SCT_TABLE
	{
		struct SCV_SCT_BIT_FEILD
		{
			INT8U SCT:4;
			INT8U SCV:4;
		} bits;
		INT8U bData;
	} SCV_SCT;
	union OCCV_OCCT_TABLE
	{
		struct OCCV_OCCT_BIT_FEILD
		{
			INT8U OCCT:4;
			INT8U OCCV:4;
		} bits;
		INT8U bData;
	} OCCV_OCCT;
	union MOST_OCRT_PFT_TABLE
	{
		struct MOST_OCRT_PFT_FEILD
		{
			INT8U PFT:2;
			INT8U OCRT:2;
			INT8U MOST:2;
			INT8U CHS:2;
		} bits;
		INT8U bData;
	} MOST_OCRT_PFT;
	INT8U OTC;
	INT8U OTCR;
	INT8U UTC;
	INT8U UTCR;
	INT8U OTD;
	INT8U OTDR;
	INT8U UTD;
	INT8U UTDR;
	INT8U TR;
}AFEEEPROM;

union CONF_TABLE
{
	struct CONF_BIT_FEILD
	{
		INT8U IDLE:1;
		INT8U SLEEP:1;
		INT8U ENWDT:1;
		INT8U CADCON:1;
		INT8U CHGMOS:1;
		INT8U DSGMOS:1;
		INT8U PCHMOS:1;
		INT8U OCRC:1;
	} bits;
	INT8U bData;
};

union RSTSTAT_TABLE
{
	struct RSTSTAT_BIT_FEILD
	{
		INT8U WDT:2;
		INT8U RSVD:6;
	} bits;
	INT8U bData;
};

typedef struct	_AFE_RAM_							
{
	union CONF_TABLE CONF;
	INT8U BALANCEH;
	INT8U BALANCEL;
	union BSTATUS1_TABLE
	{
		struct BSTATUS1_BIT_FEILD
		{
			INT8U OV:1;
			INT8U UV:1;
			INT8U OCD1:1;
			INT8U OCD2:1;
			INT8U OCC:1;
			INT8U SC:1;
			INT8U PF:1;
			INT8U WDT:1;
		} bits;
		INT8U bData;
	} BSTATUS1;
	union BSTATUS2_TABLE
	{
		struct BSTATUS2_BIT_FEILD
		{
			INT8U UTC:1;
			INT8U OTC:1;
			INT8U UTD:1;
			INT8U OTD:1;
			INT8U RSVD:4;
		} bits;
		INT8U bData;
	} BSTATUS2;
	union BSTATUS3_TABLE
	{
		struct BSTATUS3_BIT_FEILD
		{
			INT8U DSG_FET:1;
			INT8U CHG_FET:1;
			INT8U PCHG_FET:1;
			INT8U L0V:1;
			INT8U EEPR_WR:1;
			INT8U RSVD5:1;
			INT8U DSGING:1;
			INT8U CHGING:1;
		} bits;
		INT8U bData;
	} BSTATUS3;
	INT16S wTemp[3];
	INT16S wCurr;
	INT16S wVolt[16];
	INT16S wCadc;
	union BFLAG1_TABLE
	{
		struct BFLAG1_BIT_FEILD
		{
			INT8U OV_FLG:1;
			INT8U UV_FLG:1;
			INT8U OCD_FLG:1;
			INT8U LOAD_FLG:1;
			INT8U OCC_FLG:1;
			INT8U SC_FLG:1;
			INT8U PF_FLG:1;
			INT8U WDT_FLG:1;
		} bits;
		INT8U bData;
	} BFLAG1;
	union BFLAG2_TABLE
	{
		struct BFLAG2_BIT_FEILD
		{
			INT8U UTC_FLG:1;
			INT8U OTC_FLG:1;
			INT8U UTD_FLG:1;
			INT8U OTD_FLG:1;
			INT8U VADC_FLG:1;
			INT8U CADC_FLG:1;
			INT8U WAKE_FLG:1;
			INT8U RST_FLG:1;
		} bits;
		INT8U bData;
	} BFLAG2;
	union RSTSTAT_TABLE RSTSTAT;
}AFERAM;

union AFE_STATUS_TABLE
{
	struct AFE_STATUS_BIT_FEILD
	{
		// BIT0-3
		INT16U	EepromInitAlarm:1;
		INT16U	RamRwAlarm:1;
		INT16U	EepromWrAlarm:1;
		INT16U	Rsvd1:1;
		
		// BIT4-7
		INT16U	Rsvd2:4;
		
		// BIT8-11
		INT16U	Rsvd3:4;
		
		// BIT12-15
		INT16U	Rsvd4:4;
	} bits;
	INT16U data;
} fAfe;
AFEEEPROM	strAfeEepromDefault;
AFEEEPROM	strAfeEeprom;
AFERAM		strAfeRamDefault;
AFERAM		strAfeRam;
INT16U		wAfeTempRes;
INT16U		wAfeRamUpdateCnt;

/********************************************************************************
* Output variables																*
********************************************************************************/


/********************************************************************************
* Internal variables															*
********************************************************************************/
INT8U	bAfeCurrOffsetInit = 0;
INT16S	wAfeCurrOffset = 0;

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/
INT8U	sbAfeRamDataRead(void);
INT8U	sbAfeRamDataWrite(void);
void	sAfeRamDataUpdate(void);
void	sAfeEepromDataUpdate(void);
INT8U	sbAfeEepromDataRead(void);
INT8U	sbAfeEepromDataWrite(void);
void	sAfeReset(void);
void	sAfeReady(void);
INT8U	sbAfeRamRead(INT8U RdAddr, INT8U Length, INT8U *RdBuf);
INT8U	sbAfeRamWrite(INT8U WrAddr, INT8U Length, INT8U *WrBuf);
INT8U	sbAfeEepromRead(INT8U RdAddr, INT8U Length, INT8U *RdBuf);
INT8U	sbAfeEepromWrite(INT8U WrAddr, INT8U Length, INT8U *WrBuf);

/********************************************************************************
* Routines' implementations														*
********************************************************************************/
// check SH367309 is ready, and initialization MTP Buffer
void	sAfeInitial(void)
{
	memset(&strAfeEepromDefault, 0, sizeof(AFEEEPROM));
	memset(&strAfeEeprom, 0, sizeof(AFEEEPROM));
	memset(&strAfeRamDefault, 0, sizeof(AFERAM));
	memset(&strAfeRam, 0, sizeof(AFERAM));
	wAfeTempRes = 0;
	wAfeRamUpdateCnt = 0;
}

void	sAfeRamProc(INT8U bFilter)
{
	static INT8U bChkCnt = 0;
	static INT8U bErrCnt = 0;
	
	if(bBmsMode == cPowerOnMode)
	{
		bChkCnt = 0;
		bErrCnt = 0;
	}
 	else if(++bChkCnt >= bFilter)
	{
		bChkCnt = 0;
		if(sbAfeRamDataRead())
		{
			sAfeRamDataUpdate();
			if(sbAfeRamDataWrite())
			{
				bErrCnt = 0;
			}
			else
			{
				bErrCnt++;
			}
		}
		else
		{
			bErrCnt++;
		}
		
		if(bErrCnt >= 20)
		{
			bErrCnt = 20;
			sSetAfeRamRwAlarm(true);
		}
		else if(bErrCnt == 0)
		{
			sSetAfeRamRwAlarm(false);
		}
	}
}

// 必须等RAM数据读取一段时间(200ms以上)后才允许执行
INT8U	sbAfeEepromProc(INT8U bFilter)	// 此任务只在开机后初始化时执行一次
{
	static INT8U bReadCnt = 0;
	INT8U	bCnt, i, bNoChanged, bOk;
	INT8U	*pDefault;
	INT8U	*pAfe;
	
	if(++bReadCnt < bFilter)
	{
		if(sbAfeRamDataRead())
		{
			sAfeRamDataUpdate();
			sbAfeRamDataWrite();
		}
		
		return AFE_INIT_WRITTING;
	}
	bReadCnt = 0;
	bCnt = 0;
	sAfeEepromDataUpdate();
	while(1)
	{
		if(++bCnt >= 101)
		{
			sSetAfeEepromInitAlarm(true);
			return AFE_INIT_WRITE_FAIL;
		}
		
		if(sbAfeEepromDataRead() == 0)
		{
			continue;
		}
 		
		if(sbAfeRamDataRead() == 0)
		{
			continue;
		}
 		sAfeRamDataUpdate();
		
	#if (UPDATE_BATT_CURR_AFE == 1)
		if(bAfeCurrOffsetInit == 0)
		{
			wAfeCurrOffset = (INT16S)swHighLowSwap((INT16U *)&strAfeRam.wCadc);
			if(wAfeCurrOffset > 100)
			{
				wAfeCurrOffset = 100;
			}
			else if(wAfeCurrOffset < -100)
			{
				wAfeCurrOffset = -100;
			}
			bAfeCurrOffsetInit = 1;
		}
	#endif
		
		// 如果AFE供电低于18V，烧写时VPRO电压可能不足8V，不能烧写
		if((swGetBattVoltReal() < 1800) \
			|| (strAfeRam.BFLAG2.bits.VADC_FLG == 0))
		{
			//continue;
		}
		
		bNoChanged = false;
		pDefault = (INT8U *)&strAfeEepromDefault;
		pAfe = (INT8U *)&strAfeEeprom;
		for(i = 0; i < sizeof(AFEEEPROM) - 1; i++)	// TR只读不可写,所以不比较
		{
			if(*pAfe != *pDefault)
			{
				bNoChanged = true;
			}
			pAfe++;
			pDefault++;
		}
		
		if(bNoChanged == false)
		{
			break;
		}
		else
		{
			mAFE_EEPROM_ON();
			sDelay1ms(20);
			bOk = sbAfeEepromDataWrite();
			sDelay1ms(1);
			mAFE_EEPROM_OFF();
			
			if(bOk)
			{
				sAfeReset();
				sDelay1ms(5);
				sAfeReady();
				return AFE_INIT_WRITE_PASS;
			}
		}
	}
	
	return AFE_INIT_WRITE_PASS;
}

void	sAfeRamDataUpdate(void)
{
	INT8U	i;
	INT16S	wTemp;
	INT32S	dwVolt = 0;
	
	if(wAfeTempRes == 0)
	{
		if(strAfeEeprom.TR != 0)
		{
			wAfeTempRes = 680 + ((INT16U)strAfeEeprom.TR * 5);
		}
	}
	
#if (UPDATE_CELL_TEMP_AFE == 1)
	for(i = 0; i < 3; i++)
	{
		wTemp = (INT16S)swHighLowSwap((INT16U *)&strAfeRam.wTemp[i]);
		if(wTemp < 32768)
		{
			wTemp = ((INT32U)wTemp * wAfeTempRes) / (32768 - wTemp);
		}
		else
		{
			wTemp = 0;
		}
		sSetCellTempSample(i, wTemp);
	}
#endif
	
#if (UPDATE_CELL_VOLT_AFE == 1)
	for(i = 0; i < 16; i++)
	{
		wTemp = (INT16S)swHighLowSwap((INT16U *)&strAfeRam.wVolt[i]);
		if(wTemp < 0)
		{
			wTemp = 0;
		}
		sSetCellVoltSample(i, wTemp);
		dwVolt += wTemp;
	}
#endif
	
#if (UPDATE_BATT_VOLT_AFE == 1)
	sSetBattVoltSample((INT16S)(dwVolt / 16));
#endif
	
#if (UPDATE_BATT_CURR_AFE == 1)
	wTemp = (INT16S)swHighLowSwap((INT16U *)&strAfeRam.wCadc) - wAfeCurrOffset;
	sSetBattCurrSample((INT16S)wTemp);
#endif
	
#if (UPDATE_ALARM_STATUS_AFE == 1)
	sSetBattChgCurrHi(strAfeRam.BSTATUS1.bits.OCC);
	//sSetBattChgCurrHi2(strAfeRam.BSTATUS1.bits.);
	sSetBattDcgCurrHi(strAfeRam.BSTATUS1.bits.OCD1);
	sSetBattDcgCurrHi2(strAfeRam.BSTATUS1.bits.OCD2);
	sSetCellVoltHi(strAfeRam.BSTATUS1.bits.OV);
	sSetCellVoltLo(strAfeRam.BSTATUS1.bits.UV);
	sSetCellVoltLo2(strAfeRam.BSTATUS3.bits.L0V);
	sSetCellChgTempHi(strAfeRam.BSTATUS2.bits.OTC);
	sSetCellChgTempLo(strAfeRam.BSTATUS2.bits.UTC);
	sSetCellDcgTempHi(strAfeRam.BSTATUS2.bits.OTD);
	sSetCellDcgTempLo(strAfeRam.BSTATUS2.bits.UTD);
	if(wAfeRamUpdateCnt < 100)
	{
		// 开机一段时间内是断线,之后是异常高压
		wAfeRamUpdateCnt++;
		if(strAfeRam.BSTATUS1.bits.PF)
		{
			sSetBreakWireFault(true);
		}
		else if(strAfeRam.BSTATUS1.bits.OV && strAfeRam.BSTATUS1.bits.UV)
		{
			sSetBreakWireFault(true);
		}
		else
		{
			sSetBreakWireFault(false);
		}
	}
	else
	{
		if(strAfeRam.BSTATUS1.bits.PF)
		{
			//sSetCellVoltHi2(true);
			sSetBreakWireFault(false);
		}
		else if(strAfeRam.BSTATUS1.bits.OV && strAfeRam.BSTATUS1.bits.UV)
		{
			//sSetCellVoltHi2(false);
			sSetBreakWireFault(true);
		}
		else
		{
			//sSetCellVoltHi2(false);
			sSetBreakWireFault(false);
		}
	}
#endif
	
#if (UPDATE_CURR_STATUS_AFE == 1)
	if(strAfeRam.BSTATUS3.bits.CHGING)
	{
		sSetBmsCurrentStatus(cBMS_CURR_CHG);
	}
	else if(strAfeRam.BSTATUS3.bits.DSGING)
	{
		sSetBmsCurrentStatus(cBMS_CURR_DSG);
	}
	else
	{
		sSetBmsCurrentStatus(cBMS_CURR_IDLE);
	}
#endif
	
	sSetAfeEepromWrAlarm(strAfeRam.BSTATUS3.bits.EEPR_WR);
}

void	sAfeEepromDataUpdate(void)
{
	INT16U	wTemp;
	
	// NOTE：
	// SH367309如果只做采集模块，则不允许关闭VADC以及CADC模块
	// 采集模式以及保护模式：MODE接低电平保护模式,MODE接高电平采集模式(默认)
	// SH367309有四种情况会关闭VADC以及CADC模块：
	// 1:二次过压保护，可通过EEPROM->SCONF->DIS_PF=0禁止此功能
	// 2:断线保护，可通过EEPROM->SCONF->DIS_PF=0禁止此功能
	// 3:进入IDLE模式，可通过RAM->CONF->IDLE禁止此功能
	// 4:进入SLEEP模式，可通过RAM->CONF->SLEEP禁止此功能
	// 5:仓运模式，SHIP管脚低电平进入仓运模式,高电平则SH367309正常模式
	
	if(swGetBmsCellVoltNum() == 16)
	{
		strAfeEepromDefault.SCONF1.bits.CN = EEPROM_SCONF_CN_16CELL;
	}
	else if(swGetBmsCellVoltNum() >= 5)
	{
		strAfeEepromDefault.SCONF1.bits.CN = swGetBmsCellVoltNum();
	}
	else
	{
		strAfeEepromDefault.SCONF1.bits.CN = EEPROM_SCONF_CN_16CELL;
	}
	
#if (UPDATE_BAL_CTRL_MODE == 0)
	strAfeEepromDefault.SCONF1.bits.BAL = EEPROM_BAL_AFE;
#else
	strAfeEepromDefault.SCONF1.bits.BAL = EEPROM_BAL_MCU;
#endif
	strAfeEepromDefault.SCONF1.bits.OCPM = EEPROM_OCPM_CHG_OR_DSG;
	strAfeEepromDefault.SCONF1.bits.ENMOS = EEPROM_ENMOS_ENABLE;
	strAfeEepromDefault.SCONF1.bits.ENPCH = EEPROM_ENPCH_DSIABLE;
	
	strAfeEepromDefault.SCONF2.bits.EUVR = EEPROM_EUVR_DISABLE;
	strAfeEepromDefault.SCONF2.bits.OCRA = EEPROM_OCRA_ENABLE;
	strAfeEepromDefault.SCONF2.bits.CTLC = EEPROM_CTLC_DISABLE;
	strAfeEepromDefault.SCONF2.bits.DIS_PF = EEPROM_DIS_PF_DISABLE;
	strAfeEepromDefault.SCONF2.bits.UV_OP = EEPROM_UV_OP_DSG_OFF;
	if(swGetEepromCellVoltLo2() == 0)
	{
		strAfeEepromDefault.SCONF2.bits.E0VB = EEPROM_E0VB_ENABLE;
	}
	else
	{
		strAfeEepromDefault.SCONF2.bits.E0VB = EEPROM_E0VB_DISABLE;
	}
	
	strAfeEepromDefault.OVT_LDRT_OVH.bits.LDRT = EEPROM_LDRT_2S;
	strAfeEepromDefault.OVT_LDRT_OVH.bits.OVT = EEPROM_OVT_20S;
	wTemp = swGetEepromCellVoltHi2() / EEPROM_OV_5MV;
	strAfeEepromDefault.OVT_LDRT_OVH.bits.OVH = wTemp >> 8;
	strAfeEepromDefault.OVL = wTemp & 0xFF;
	
	strAfeEepromDefault.UVT_OVRH.bits.UVT = EEPROM_UVT_10S;
	wTemp = swGetEepromCellVoltHiBack() / EEPROM_OVR_5MV;
	strAfeEepromDefault.UVT_OVRH.bits.OVRH = wTemp >> 8;
	strAfeEepromDefault.OVRL = wTemp & 0xFF;
	
	strAfeEepromDefault.PFV = (swGetEepromCellVoltHi2() + 100) / EEPROM_PFV_20MV;
	strAfeEepromDefault.BALV = swGetEepromBalanceVoltEn() / EEPROM_BALV_20MV;
	strAfeEepromDefault.UVR = swGetEepromCellVoltLoBack() / EEPROM_UV_20MV;
	strAfeEepromDefault.UV = swGetEepromCellVoltLo() / EEPROM_UV_20MV;
	strAfeEepromDefault.PREV = swGetEepromCellVoltLo2() / EEPROM_PREV_20MV;
	strAfeEepromDefault.L0V = swGetEepromCellVoltLo2() / EEPROM_L0V_20MV;
	
	strAfeEepromDefault.OCD1V_OCD1T.bits.OCD1T = EEPROM_OCD1T_15S;
	strAfeEepromDefault.OCD1V_OCD1T.bits.OCD1V = EEPROM_OCD1V_100MV;
	strAfeEepromDefault.OCD2V_OCD2T.bits.OCD2T = EEPROM_OCD2T_4S;
	strAfeEepromDefault.OCD2V_OCD2T.bits.OCD2V = EEPROM_OCD2V_200MV;
	strAfeEepromDefault.SCV_SCT.bits.SCT = EEPROM_SCT_256US;
	strAfeEepromDefault.SCV_SCT.bits.SCV = EEPROM_SCV_400MV;
	strAfeEepromDefault.OCCV_OCCT.bits.OCCT = EEPROM_OCCT_10S;
	strAfeEepromDefault.OCCV_OCCT.bits.OCCV = EEPROM_OCCV_100MV;
	
	strAfeEepromDefault.MOST_OCRT_PFT.bits.PFT = EEPROM_PFT_64S;
	strAfeEepromDefault.MOST_OCRT_PFT.bits.OCRT = EEPROM_OCRT_32S;
	strAfeEepromDefault.MOST_OCRT_PFT.bits.MOST = EEPROM_MOST_128US;
	strAfeEepromDefault.MOST_OCRT_PFT.bits.CHS = EEPROM_CHS_200UV;
	
	strAfeEepromDefault.OTC = 255;
	strAfeEepromDefault.OTCR = 255;
	strAfeEepromDefault.OTD = 255;
	strAfeEepromDefault.OTDR = 255;
	strAfeEepromDefault.UTC = 255;
	strAfeEepromDefault.UTCR = 255;
	strAfeEepromDefault.UTD = 255;
	strAfeEepromDefault.UTDR = 255;
	
	strAfeEepromDefault.TR = 0x42;		// 默认值,不可修改
}

INT8U	sbAfeRamDataRead(void)
{
	return sbAfeRamRead(AFE_RAM_ADDR, sizeof(AFERAM), (INT8U *)&strAfeRam);
}

INT8U	sbAfeRamDataWrite(void)
{
	// 使能CHG&DSG&PCHG输出,且使能WDT和CADC模块
	strAfeRamDefault.CONF.bData = 0x70;		// 默认值0x70
	strAfeRamDefault.CONF.bits.CHGMOS = 1;
	strAfeRamDefault.CONF.bits.DSGMOS = 1;
	strAfeRamDefault.CONF.bits.PCHMOS = 1;
	strAfeRamDefault.CONF.bits.ENWDT = 1;
	strAfeRamDefault.CONF.bits.CADCON = 1;
#if (UPDATE_BAL_CTRL_MODE == 0)
	strAfeRamDefault.BALANCEH = 0xFF;
	strAfeRamDefault.BALANCEL = 0xFF;
#elif (UPDATE_BAL_CTRL_MODE == 1)
	strAfeRamDefault.BALANCEH = (INT8U)(swGetBalanceCellStatus() >> 8);
	strAfeRamDefault.BALANCEL = (INT8U)(swGetBalanceCellStatus() >> 0);
#endif
	if(!sbAfeRamWrite(AFE_RAM_CONF, 3, &strAfeRamDefault.CONF.bData))
	{
		return 0;
	}
	
	strAfeRamDefault.RSTSTAT.bData = 0x00;	// 默认值0x00
	strAfeRamDefault.RSTSTAT.bits.WDT = EEPROM_WDT_4S;
	if(strAfeRamDefault.RSTSTAT.bData != strAfeRam.RSTSTAT.bData)
	{
		if(!sbAfeRamWrite(AFE_RAM_RSTSTAT, 1, &strAfeRamDefault.RSTSTAT.bData))
		{
			return 0;
		}
	}
	
	// 发生看门狗故障,需要MCU主动清除后AFE才可以重新工作
	if(strAfeRam.BFLAG1.bits.WDT_FLG == 1)
	{
		strAfeRam.BFLAG1.bits.WDT_FLG = 0;
		if(!sbAfeRamWrite(AFE_RAM_BFLAG1, 1, &strAfeRam.BFLAG1.bData))
		{
			return 0;
		}
	}
	
	// 发生系统复位时,需要MCU主动清除此位,不清除也不影响正常使用
	if(strAfeRam.BFLAG2.bits.RST_FLG == 1)
	{
		strAfeRam.BFLAG2.bits.RST_FLG = 0;
		if(!sbAfeRamWrite(AFE_RAM_BFLAG2, 1, &strAfeRam.BFLAG2.bData))
		{
			return 0;
		}
	}
	
	return 1;
}

INT8U	sbAfeEepromDataRead(void)
{
	return sbAfeEepromRead(AFE_EEPROM_ADDR, sizeof(AFEEEPROM), (INT8U *)&strAfeEeprom);
}

INT8U	sbAfeEepromDataWrite(void)
{
	return sbAfeEepromWrite(AFE_EEPROM_ADDR, sizeof(AFEEEPROM) - 1, (INT8U *)&strAfeEepromDefault);
}

// Reset SH367309 IC, Send Data:0xEA+0xC0+CRC
void	sAfeReset(void)
{
	INT8U WrBuf[2];
	
	WrBuf[0] = 0xC0;
	WrBuf[1] = 0xA5;
	if(!sbTwiWrite(AFE_ID, 0xEA, 1, WrBuf))		// 0xEA+0xC0+CRC
	{
		sbTwiWrite(AFE_ID, 0xEA, 1, WrBuf);
	}
}

void	sAfeReady(void)
{
	INT8U bCnt = 0, bTemp;;
	
	while(1)
	{
		bTemp = 0;
		// 读取BLFG2，查看VADC是否转换完成
		if(sbAfeRamRead(AFE_RAM_BFLAG2, 1, &bTemp))
		{
			if((bTemp & 0x10) == 0x10)
			{
				break;
			}
		}
		
		sDelay1ms(20);
		if(++bCnt >= 50)
		{
			break;
		}
	}
}

INT8U	sbAfeRamRead(INT8U RdAddr, INT8U Length, INT8U *RdBuf)
{
	INT8U result=1;
	
    result = sbTwiRead(AFE_ID, RdAddr, Length, RdBuf);
	if(!result)
	{
		result = sbTwiRead(AFE_ID, RdAddr, Length, RdBuf);
	}
	return result;
}

INT8U	sbAfeRamWrite(INT8U WrAddr, INT8U Length, INT8U *WrBuf)
{
	INT8U result;
	INT8U i;
	
    for(i = 0; i < Length; i++)
	{
		result = sbTwiWrite(AFE_ID, WrAddr, 1, WrBuf);
		if(!result)
		{
			sDelay1ms(1);	// 最少延时1ms才能开始下一次的写RAM
            result = sbTwiWrite(AFE_ID, WrAddr, 1, WrBuf);
            if(!result)
            {
                break;
            }
		}
		WrAddr++;
		WrBuf++;
		sDelay1ms(1);
 	}
    
    return result;  
}

INT8U	sbAfeEepromRead(INT8U RdAddr, INT8U Length, INT8U *RdBuf)
{
	INT8U result=1;
	
    result = sbTwiRead(AFE_ID, RdAddr, Length, RdBuf);
	if(!result)
	{
		result = sbTwiRead(AFE_ID, RdAddr, Length, RdBuf);
	}
	return result;
}

INT8U	sbAfeEepromWrite(INT8U WrAddr, INT8U Length, INT8U *WrBuf)
{
	INT8U result;
	INT8U i;
	
    for(i = 0; i < Length; i++)
	{
		result = sbTwiWrite(AFE_ID, WrAddr, 1, WrBuf);
		if(!result)
		{
			sDelay1ms(40);	// 最少延时35ms才能开始下一次的写EEPROM
            result = sbTwiWrite(AFE_ID, WrAddr, 1, WrBuf);
            if(!result)
            {
                break;
            }
		}
		WrAddr++;
		WrBuf++;
		sDelay1ms(40);
 	}
	
    return result;  
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
INT8U	sbGetAfeEepromInitAlarm(void)
{
	return fAfe.bits.EepromInitAlarm;
}

INT8U	sbGetAfeRamRwAlarm(void)
{
	return fAfe.bits.RamRwAlarm;
}

INT8U	sbGetAfeEepromWrAlarm(void)
{
	return fAfe.bits.EepromWrAlarm;
}

INT16U	swGetAfeRamData(INT16U RdAddr)
{
	if(RdAddr < sizeof(AFERAM))
	{
		return *((INT8U *)&strAfeRam + RdAddr);
	}
	else
	{
		return 0xFFFF;
	}
}

INT16U	swGetAfeEepromData(INT16U RdAddr)
{
	if(RdAddr < sizeof(AFEEEPROM))
	{
		return *((INT8U *)&strAfeEeprom + RdAddr);
	}
	else
	{
		return 0xFFFF;
	}
}

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
void	sSetAfeEepromInitAlarm(INT8U bStatus)
{
	fAfe.bits.EepromInitAlarm = bStatus;
}

void	sSetAfeRamRwAlarm(INT8U bStatus)
{
	fAfe.bits.RamRwAlarm = bStatus;
}

void	sSetAfeEepromWrAlarm(INT8U bStatus)
{
	fAfe.bits.EepromWrAlarm = bStatus;
}

