/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		Adc.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#include "OS_HEAD.H"

/************************************************************************************************
Function name:  OSTimerTick
Description:    This function is called by a periodical interrupt.
Calls:
Called By:      
Parameters:     None
Return:         None     
************************************************************************************************/
void OSTimerTick(void)
{
	OS_TCB *ptcb;
	OS_TCB *ptcbend;
	INTOS prio;
	
#if	OS_TASK_IDLE_EN>0
	OSTime++;
	if (OSTime == OS_TICKS_PER_SEC)
	{
		OSTime = 0;
//		OSCpuFree = OSIdleCtr * OS_CPUFREE_FACTOR >> 20;
		OSIdleCtr = 0;
	}
#endif 	
	
	prio = 0;
	ptcbend = &OSTCBTbl[OS_LOWEST_PRIO];
	ptcb = OSTCBTbl;
	while (ptcb <= ptcbend)
	{
		if (ptcb->TimerPeriod != 0)
		{
			ptcb->TimerCnt++;
			if (ptcb->TimerCnt >= ptcb->TimerPeriod)
			{
				ptcb->TimerCnt = 0;
				OSISREventSend(prio,OS_EVENTID_TIMER);				
			}
		}
		ptcb++;
		prio++;
	}
}

