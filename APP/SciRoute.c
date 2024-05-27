/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		SciRoute.c
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#define		__SCI_ROUTE_C__

/********************************************************************************
* Include head files															*
********************************************************************************/
#include	"Main.h"

/********************************************************************************
* Macros 																		*
********************************************************************************/
#define cSCI_ROUTE_ADDR					0x00
#define cSCI_ROUTE_CMD					0x01
#define cSCI_ROUTE_REG_ADDR_H			0x02
#define cSCI_ROUTE_REG_ADDR_L			0x03
#define cSCI_ROUTE_REG_NUM_H			0x04
#define cSCI_ROUTE_REG_NUM_L			0x05
#define cSCI_ROUTE_CRC_H				0x02
#define cSCI_ROUTE_CRC_L				0x01

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


/********************************************************************************
* Internal variables															*
********************************************************************************/
INT8U bRouteProt = cROUTE_PROT_SELFDEFINE;

/********************************************************************************
* Internal routine prototypes													*
********************************************************************************/


/********************************************************************************
* Routines' implementations														*
********************************************************************************/
void	sSciRouting(INT8U sciid)
{
	INT8U	*RxBuff, RxLen;// 指向接收缓冲区的指针和接收到的数据长度
	//INT16U id, func, addr, nums;
	//INT16U id, addr, nums,id2;
	INT16U addr;// 存储地址信息的变量
	INT16U temp;// 临时变量
	
	// 获取串口接收缓冲区和数据长度
	RxBuff = pbGetSciRxBuff(sciid);
	RxLen = bGetSciRxLen(sciid);
	
	//id =  RxBuff[cSCI_ROUTE_ADDR];
	//func = RxBuff[cSCI_ROUTE_CMD];
	
	// 解析地址信息
	addr = ((INT16U)RxBuff[cSCI_ROUTE_REG_ADDR_H] << 8) + RxBuff[cSCI_ROUTE_REG_ADDR_L];
	//nums = ((INT16U)RxBuff[cSCI_ROUTE_REG_NUM_H] << 8) + RxBuff[cSCI_ROUTE_REG_NUM_L];
	
	// Route
	//如果接收到的数据判断路由协议
	// 如果接收到的数据长度为3，第一个字节为0x02，第二个字节为0xFF，则发送事件到优先级为cPrioSuper的事件队列，并设置路由协议为cROUTE_PROT_OPENBLT
	if((RxLen==0x03)&&(RxBuff[0]==0x02)&&(RxBuff[1]==0xFF))
	{
		temp = sbGetSciAddress();
		if((RxBuff[2]==0x00) || (RxBuff[2]==0xff))
		{
			OSEventSend(cPrioSuper, eSuperToUpgrade);
		}
		else if(RxBuff[2]==temp)
		{
			OSEventSend(cPrioSuper, eSuperToUpgrade);
		}
		bRouteProt = cROUTE_PROT_OPENBLT;
	}
	// 如果第一个字节大于大于0X20，则设置路由协议位cROUTE_PROT_PYLON
	else if(RxBuff[cSCI_ROUTE_ADDR] >= 0x20)
	{
		bRouteProt = cROUTE_PROT_PYLON;
	}
	// 如果地址小于0x0100，则根据cProtocol_Branch的定义选择不同的路由协议
	else if(addr < 0x0100)
	{
    #if(cProtocol_Branch == cROUTE_PROT_VOLTRONIC)
		bRouteProt = cROUTE_PROT_VOLTRONIC;
    #else
        bRouteProt = cROUTE_PROT_Growatt;
    #endif
	}
	// 其他情况下，设置路由协议为cROUTE_PROT_SELFDEFINE
	else
	{
		bRouteProt = cROUTE_PROT_SELFDEFINE;
	}
}

/********************************************************************************
* Output interface Routines														*
********************************************************************************/
//获取当前的路由协议
INT8U	sbGetRouteProt(void)
{
	return bRouteProt;
}

/********************************************************************************
* Input interface Routines														*
********************************************************************************/
// 设置路由协议
void	sSetRouteProt(INT8U prot)
{
	bRouteProt = prot;
}

