/********************************************************************************
Right Reserve:	BMS Technology Co., Ltd
Project:		BMS
File Name:		Adc.h
Author:			BMSTeam
Date:			2023.07.01
Description:	None
********************************************************************************/
#include "OS_HEAD.H"

/*
*********************************************************************************************************
*                                          MESSAGE QUEUE DATA
*********************************************************************************************************
*/
#if (OS_Q_EN > 0) && (OS_MAX_QS > 0)
OS_Q             *OSQFreeList;
OS_Q              OSQTbl[OS_MAX_QS];
#endif


#if (OS_Q_EN > 0) && (OS_MAX_QS > 0)
/*
*********************************************************************************************************
* Function Name: OSQAccept
* 
* Description: This function checks the queue to see if a message is available.  
*            
*
* Input		 : pq --->It is a pointer to the message queue block
*
* Returns    : != (void *)0  is the message in the queue if one is available.  The message is removed
*                            from the so the next time OSQAccept() is called, the queue will contain
*                            one less entry.
*              == (void *)0  if the queue is empty or,
********************************************************************************************************
*/

#if OS_Q_ACCEPT_EN > 0

void  *OSQAccept (OS_Q *pq)
{
    OS_CPU_SR  cpu_sr;
    void      *msg;

    if(pq==(OS_Q *)0)
    {
    	return((void *)0);
    }
    	
    OS_ENTER_CRITICAL();    
    if (pq->OSQEntries > 0)						
    {                    
        msg = *pq->OSQOut++;                    
        pq->OSQEntries--;                       
        
        if (pq->OSQOut == pq->OSQEnd) 			
        {          
            pq->OSQOut = pq->OSQStart;
        }
    } 
    else 
    {
        msg = (void *)0;                        
    }
    OS_EXIT_CRITICAL();
    return (msg);                               
}
#endif

/*
*********************************************************************************************************
*Function Name:						CREATE A MESSAGE QUEUE
*
* Description: This function creates a message queue if free event control blocks are available.
*
* Arguments  : start         is a pointer to the base address of the message queue storage area.  The
*                            storage area MUST be declared as an array of pointers to 'void' as follows
*
*                            void *MessageStorage[size]
*
*              size          is the number of elements in the storage area
*
* Returns    : != (OS_Q *)0  is a pointer to the queue control clock
*
*              == (OS_Q *)0  if no queue control blocks were available or an error was detected
*********************************************************************************************************
*/
OS_Q  *OSQCreate (void **start, INT16U size)
{
	OS_CPU_SR  cpu_sr;  
	OS_Q      *pq;

    OS_ENTER_CRITICAL();
    pq = OSQFreeList;               	    	      
            
    if (pq != (OS_Q *)0) 
    {              		      
        OSQFreeList         = OSQFreeList->OSQPtr;    
        OS_EXIT_CRITICAL();
        pq->OSQStart        = start;                  
        pq->OSQEnd          = &start[size];
        pq->OSQIn           = start;
        pq->OSQOut          = start;
        pq->OSQSize         = size;
        pq->OSQEntries      = 0;
    }
    OS_EXIT_CRITICAL();

	return (pq);
}
/*
*********************************************************************************************************
*                                           FLUSH QUEUE
*
* Description : This function is used to flush the contents of the message queue.
*
* Arguments   : none
*
* Returns     : false :invalid message queue pointer
*               true  :Message queue is inilizated successlly
*               
*********************************************************************************************************
*/
#if OS_Q_FLUSH_EN > 0
INT8U  OSQFlush (OS_Q *pq)
{
	OS_CPU_SR  cpu_sr;
	if(pq == (OS_Q *)0)
	{
		return false;	
	}
	else
	{
	    OS_ENTER_CRITICAL();
	    pq->OSQIn      = pq->OSQStart;
	    pq->OSQOut     = pq->OSQStart;
	    pq->OSQEntries = 0;
	    OS_EXIT_CRITICAL();
	}
    return (true);
}
#endif

/*
*********************************************************************************************************
*                                        POST MESSAGE TO A QUEUE
*
* Description: This function sends a message to a queue
*
* Arguments  : pq            is a pointer to the queue control block 
*
*              msg           is a pointer to the message to send.  You MUST NOT send a NULL pointer.
*
* Returns    : OS_Q_NULL			 If 'pq' is a NULL pointer	
*			   OS_NO_ERR			 normal 	
*              OS_Q_FULL             If the queue cannot accept any more messages because it is full.
*********************************************************************************************************
*/
#if OS_Q_POST_EN > 0

INT8U  OSQPost (OS_Q *pq, void *msg)
{
    OS_CPU_SR  cpu_sr;
	if(pq == (OS_Q *)0)
	{
		return (OS_Q_NULL);	
	}
	OS_ENTER_CRITICAL();
    if (pq->OSQEntries >= pq->OSQSize) 
    {              									  
        OS_EXIT_CRITICAL();
        return (OS_Q_FULL);
    }
    *pq->OSQIn++ = msg;                               
    pq->OSQEntries++;                                 
    if (pq->OSQIn == pq->OSQEnd) 
    {                    							  
        pq->OSQIn = pq->OSQStart;
    }
    OS_EXIT_CRITICAL();
    return (OS_NO_ERR);
}
#endif
/*
*********************************************************************************************************
*                                      QUEUE MODULE INITIALIZATION
*
* Description : This function is called by uC/OS-II to initialize the message queue module.  Your
*               application MUST NOT call this function.
*
* Arguments   :  none
*
* Returns     : none
*
* Note(s)    : This function is INTERNAL to uC/OS-II and your application should not call it.
*********************************************************************************************************
*/

void  OS_QInit (void)
{
#if OS_MAX_QS == 1
    OSQFreeList         = &OSQTbl[0];            
    OSQFreeList->OSQPtr = (OS_Q *)0;
#endif

#if OS_MAX_QS >= 2
    INT16U  i;
    OS_Q   *pq1;
    OS_Q   *pq2;


    pq1 = &OSQTbl[0];
    pq2 = &OSQTbl[1];
    for (i = 0; i < (OS_MAX_QS - 1); i++) {    
        pq1->OSQPtr = pq2;
        pq1++;
        pq2++;
    }
    pq1->OSQPtr = (OS_Q *)0;
    OSQFreeList = &OSQTbl[0];
#endif
}
#endif

