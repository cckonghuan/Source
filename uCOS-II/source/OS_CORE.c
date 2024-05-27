/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		Adc.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#include "OS_HEAD.H"
//#include "derivative.h"

#if	OS_TABLE_IN_FLASH>0
const INTOS OSUnMapTbl[16] =
{              
	255, 0, 1, 0,
	2  , 0, 1, 0,
	3  , 0, 1, 0,
	2  , 0, 1, 0
};
#else
INTOS OSUnMapTbl[16] = 
{              
	255, 0, 1, 0,
	2  , 0, 1, 0,
	3  , 0, 1, 0,
	2  , 0, 1, 0
};
#endif

void OSInitSys(void);
void OSInitRdyList(void);
void OSInitTCBList(void);
INTOS OSFindHighPrio(void);
void OSSched(void);

/************************************************************************************************
Function name:  OSInitSys
Description:    This function is called by OSInit() to initialize miscellaneous variables.
Calls:
Called By:      
Parameters:     None
Return:         None     
************************************************************************************************/
void OSInitSys(void)
{ 	    	
#if	OS_STK_CHK_EN>0
	int	i;
	
	for (i = 0; i < OS_MAX_STK_SIZE; i++)
	{
		OSSystemStk[i] = 0;
	}
#endif  	

#if	OS_TASK_IDLE_EN>0
    OSIdleCtr = 0;                          // Clear the 32-bit idle counter
	OSTime = 0;                             // Clear the 32-bit system clock
#endif

#if	OS_STK_GROWTH_UP>0
	OSStkPtr = OSSystemStk;
#else
	OSStkPtr = OSSystemStk + OS_MAX_STK_SIZE - 1;
#endif
}

/************************************************************************************************
Function name:  OSInitRdyList
Description:    This function is called by OSInit() to initialize the Ready List.
Calls:
Called By:      
Parameters:     None
Return:         None     
************************************************************************************************/
void  OSInitRdyList(void)
{
	// Clear the ready list
	OSRdyMap = 0x00;
	OSPrioCur = 0;
	OSPrioHighRdy = 0;
	OSTCBHighRdy = (OS_TCB *)0;                                 
	OSTCBCur = (OS_TCB *)0;
}

/************************************************************************************************
Function name:  OSInitTCBList
Description:    This function is called by OSInit() to initialize the free list of OS_TCBs.
Calls:
Called By:      
Parameters:     None
Return:         None     
************************************************************************************************/
void OSInitTCBList(void)
{
	INTOS i;
	OS_TCB *ptcb;
	
	ptcb = &OSTCBTbl[0];
	
	for (i = 0; i < OS_LOWEST_PRIO + 1; i++) 
	{
	    // Clear the priority table
		ptcb->OSTCBStkPtr = (void*)0;
		ptcb->TimerPeriod = 0;
		ptcb->TimerCnt = 0;
		ptcb->OSEvent = 0;
		ptcb->OSEventBitMask = ~0;
		ptcb++;
	}
}

/************************************************************************************************
Function name:  OSFindHighPrio
Description:    This function is used to find the highest ready priority
Calls:
Called By:      
Parameters:     None
Return:         The highest ready priority     
************************************************************************************************/
INTOS OSFindHighPrio(void)
{
    if (OSRdyMap & 0x000F)
	{
		return (OSUnMapTbl[OSRdyMap & 0x000F]);
	}
	else if (OSRdyMap & 0x00F0)
	{
		return (OSUnMapTbl[(OSRdyMap >> 4) & 0x000F] + 4);
	}
	else if (OSRdyMap & 0x0F00)
	{
		return (OSUnMapTbl[(OSRdyMap >> 8) & 0x000F] + 8);
	}
	else if (OSRdyMap & 0xF000)
	{
		return (OSUnMapTbl[(OSRdyMap >> 12) & 0x000F] + 12);
	}
	else
	{
		return (OS_LOWEST_PRIO);
	}
}

/************************************************************************************************
Function name:  OSSched
Description:    This function is called by other OS services to determine whether a new, high level
                code priority task has been made ready to run.It is invoked by TASK and is not used
                to reschedule tasks from ISRs(see OSIntExit() for ISR rescheduling).
                1) This function is INTERNAL to OS and your application should not call it.
                2) Rescheduling is prevented when the scheduler is locked (see OS_SchedLock()).
Calls:
Called By:      
Parameters:     None
Return:         None    
************************************************************************************************/
void OSSched(void) 
{
	OS_CPU_SR cpu_sr;
	OS_ENTER_CRITICAL();      
	OSPrioHighRdy = OSFindHighPrio();           // Get the highest priority from table	
	OSPrioCur = OSPrioHighRdy;
	OSTCBHighRdy = &OSTCBTbl[OSPrioHighRdy];	
	OSCtxSw();                                  // Perform a context switch
	OS_EXIT_CRITICAL();
}

/************************************************************************************************
Function name:  OSTaskIdle
Description:    This task is internal to RTOS and executes whenever no other higher priority tasks
                executes because they are ALL waiting for event(s) to occur.
Calls:
Called By:      
Parameters:     None
Return:         None     
************************************************************************************************/
#if	OS_TASK_IDLE_EN>0
void OSTaskIdle(void)
{
	OS_CPU_SR cpu_sr;
	
	for (;;) 
    {
        OS_ENTER_CRITICAL();
        OSIdleCtr++;
        OS_EXIT_CRITICAL();
		
#if OS_STK_CHK_EN > 0
		for(bStkPrio = 0; bStkPrio < (OS_LOWEST_PRIO + 1); bStkPrio++)
		{
			bStkUsage[bStkPrio] = OSStkChk(bStkPrio);
		}
#endif
    }
}
#endif

/************************************************************************************************
Function name:  OSInitTaskIdle
Description:    This function creates the Idle Task.
Calls:
Called By:      
Parameters:     None
Return:         None     
************************************************************************************************/
#if	OS_TASK_IDLE_EN>0
void OSInitTaskIdle(void)
{
	OSTaskCreate(OSTaskIdle,OS_TASK_IDLE_STK_SIZE,OS_IDLE_PRIO);
}
#endif







