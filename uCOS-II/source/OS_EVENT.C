/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		Adc.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#include "OS_HEAD.H"

TASK_EVENT OSMaskEventPend(TASK_EVENT mask);
TASK_EVENT OSEventGet(void);
void OSEventSend(INTOS prio,INTOS EventId);
void OSISREventSend(INTOS prio,INTOS EventId);
extern void OSSched(void);

/************************************************************************************************
Function name:  OSMaskEventPend
Description:    This function is to wait an event,if no event happened then suspend the task ,
                otherwise clear the event in TCB and return the event.
Calls:
Called By:      
Parameters:     None
Return:         Event
************************************************************************************************/
TASK_EVENT OSMaskEventPend(TASK_EVENT mask)
{
	TASK_EVENT event;
	OS_CPU_SR cpu_sr;
//	OS_TCB *ptcb;
	OS_ENTER_CRITICAL();
	
	OSTCBCur->OSEventBitMask = mask;
	event = OSTCBCur->OSEvent;
	
	if ((event & mask) == 0)
	{
		OSTaskSuspend(OSPrioCur);
		OS_EXIT_CRITICAL();
		OSSched();
		OS_ENTER_CRITICAL();
		event = OSTCBCur->OSEvent;
		OSTCBCur->OSEvent &= ~mask;
		OS_EXIT_CRITICAL();
		return (event&mask);
	}
	else
	{
		OSTCBCur->OSEvent &= ~mask;
		OS_EXIT_CRITICAL();
		return (event&mask);
	}
}

/************************************************************************************************
Function name:  OSEventGet
Description:    This function is to get event in the currrent TCB,and clear it,then return the event.
Calls:
Called By:      
Parameters:     None
Return:         Event     
************************************************************************************************/
TASK_EVENT OSEventGet()
{
	TASK_EVENT event;
	OS_CPU_SR cpu_sr;
	OS_ENTER_CRITICAL();
	event = OSTCBCur->OSEvent;
	OSTCBCur->OSEvent = 0;
	OS_EXIT_CRITICAL();
	return (event);
}

/************************************************************************************************
Function name:  OSEventSend
Description:    This function is to post an event to the task whose priority is prio,the EventId
                is bit number in the OSEvent,It can only be called in task level.
Calls:
Called By:      
Parameters:     prio:the task's priority
                EventId:the bit number in the OSEvent
Return:         None     
************************************************************************************************/
void OSEventSend(INTOS prio,INTOS EventId)
{
	OS_TCB *ptcb;
	OS_CPU_SR cpu_sr;
	OS_ENTER_CRITICAL();
	ptcb = &OSTCBTbl[prio];
	ptcb->OSEvent |= (1 << EventId);
	if ((ptcb->OSEvent) & (ptcb->OSEventBitMask))
	{
		OSTaskRdy(prio);
	}
	OS_EXIT_CRITICAL();
}

/************************************************************************************************
Function name:  OSISREventSend
Description:    This function is to post an event to the task whose priority is prio,the EventId
                is bit number in the OSEvent,It can only be called in ISRs.
Calls:
Called By:      
Parameters:     prio:the task's priority
                EventId:the bit number in the OSEvent
Return:         None     
************************************************************************************************/
void OSISREventSend(INTOS prio,INTOS EventId)
{
	OS_TCB *ptcb;
	ptcb = &OSTCBTbl[prio];
	ptcb->OSEvent |= (1 << EventId);
	if ((ptcb->OSEvent) & (ptcb->OSEventBitMask))
	{
		OSTaskRdy(prio);
	}
}
