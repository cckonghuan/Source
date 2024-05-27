/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		Adc.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#ifndef __OS_HEAD_H__
#define __OS_HEAD_H__

#include "..\ports\OS_CPU.h"
#include "..\ports\os_config.h"

#ifdef   OS_GLOBALS
#define  OS_EXT
#else
#define  OS_EXT  extern
#endif

/************************************************************************************************
                                    Task ready control
Description: 	These two macros control the ready bit of a task in OSRdyMap
*************************************************************************************************/
#define OSTaskRdy(prio)         OSRdyMap |= (1<<(prio))       // Insert a ready task into the OSRdyMap
#define OSTaskSuspend(prio)     OSRdyMap &= ~(1<<(prio))      // Delete a redy task from OSRdyMap

/************************************************************************************************
                                    Constant for stack check
*************************************************************************************************/
#if	OS_STK_CHK_EN>0
#define	OS_STK_FULL	0
#endif

/************************************************************************************************
                                    Macro for CPU usage calculation
*************************************************************************************************/
#if	OS_TASK_IDLE_EN>0
#define	OSGetCPUTimeFree()      OSCpuFree
#endif

/************************************************************************************************
                                    Macro for timer
*************************************************************************************************/
#define OSTimerStart(prio,cnt,period)	OSTCBTbl[(prio)].TimerCnt = (cnt); OSTCBTbl[(prio)].TimerPeriod = (period)
#define OSTimerStop(prio)		        OSTCBTbl[(prio)].TimerPeriod = 0
		
/************************************************************************************************
                                    Macro for event
*************************************************************************************************/
#define	OSEventPend()           OSMaskEventPend(~0)

/************************************************************************************************
                                    System event define
*************************************************************************************************/
#define OS_EVENTID_TIMER        0

/************************************************************************************************
                                    Constant for timer
*************************************************************************************************/
#define false                   0
#define true                    1


/************************************************************************************************
                                    Task control block define
*************************************************************************************************/
typedef struct
{
    OS_STK *OSTCBStkPtr;               // Pointer to current top of stack
#if	OS_STK_CHK_EN>0
    OS_STK *OSTCBStkBot;               // Pointer to bottom of stack
    INTOS  OSTCBStkSize;               // Stack size
#endif
    INT16U TimerPeriod;                // Timer period
    INT16U TimerCnt;                   // Timer counter
    TASK_EVENT OSEvent;                // Event
    TASK_EVENT OSEventBitMask;         // Event mask bits
}OS_TCB;

		
/************************************************************************************************
                                    Task schedule variables define
*************************************************************************************************/
OS_EXT INT16U OSRdyMap;
OS_EXT OS_TCB OSTCBTbl[OS_LOWEST_PRIO + 1];             // Table of TCB
OS_EXT INTOS OSPrioHighRdy;                             // Priority of highest priority in ready table
OS_EXT INTOS OSPrioCur;                                 // Priority of current task 
OS_EXT OS_TCB *OSTCBHighRdy;                            // Pointer to highest priority TCB R-to-R 
OS_EXT OS_TCB *OSTCBCur;                                // Pointer to currently running TCB
OS_EXT OS_STK OSSystemStk[OS_MAX_STK_SIZE];             // Stack of SYSTEM task
OS_EXT OS_STK *OSStkPtr;

/************************************************************************************************
                                    CPU usage
*************************************************************************************************/
#if	OS_TASK_IDLE_EN>0
OS_EXT INT32U OSIdleCtr;
OS_EXT INT16U OSTime;
OS_EXT INTOS OSCpuFree;
#endif

#if	OS_STK_CHK_EN>0
OS_EXT INTOS bStkUsage[OS_LOWEST_PRIO + 1];
OS_EXT INTOS bStkPrio;
#endif


/************************************************************************************************
                                    Priority resolution table
*************************************************************************************************/
#if	OS_TABLE_IN_FLASH>0
extern const INTOS OSUnMapTbl[16];
#else
extern INTOS OSUnMapTbl[16];
#endif


/************************************************************************************************
                                    Function prototype
*************************************************************************************************/
extern void OSInit(void);
extern void	OSStart(void);
extern void OSTaskCreate(void (*task)(void), INTOS stksize, INTOS prio);
extern TASK_EVENT OSMaskEventPend(TASK_EVENT mask);
extern TASK_EVENT OSEventGet(void);
extern void OSEventSend(INTOS prio,INTOS EventId);
extern void OSISREventSend(INTOS prio,INTOS EventId);
extern void OSIntExit (void);
extern void OSTimerTick(void);

/************************************************************************************************
                                    System function list
*************************************************************************************************/
#if	OS_STK_CHK_EN>0
INTOS OSStkChk(INTOS prio);
#endif

#if OS_Q_EN > 0
typedef struct os_q {                  
    struct os_q   *OSQPtr;             
    void         **OSQStart;           
    void         **OSQEnd;             
    void         **OSQIn;              
    void         **OSQOut;             
    INT16U         OSQSize;            
    INT16U         OSQEntries;         
} OS_Q;
#endif

#if (OS_Q_EN > 0) && (OS_MAX_QS > 0)
extern  OS_Q             *OSQFreeList;         
extern  OS_Q              OSQTbl[OS_MAX_QS];   
#endif

/*
*********************************************************************************************************
*                                             ERROR CODES for message queue
*********************************************************************************************************
*/
#define 	OS_Q_NULL				1
#define 	OS_Q_FULL				2
#define 	OS_NO_ERR				3

/*
*********************************************************************************************************
*                                         queue Function Prototypes
*********************************************************************************************************
*/

#if (OS_Q_EN > 0) && (OS_MAX_QS > 0)
extern		void  *OSQAccept (OS_Q *pq);
extern		OS_Q  *OSQCreate (void **start, INT16U size);
extern		INT8U  OSQFlush (OS_Q *pq);
extern		INT8U  OSQPost (OS_Q *pq, void *msg);
extern		void  OS_QInit (void);
#endif

#endif  // __OS_HEAD_H__

