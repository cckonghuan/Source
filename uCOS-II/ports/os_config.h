/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*                                  uC/OS-II Configuration File for V2.8x
*
*                               (c) Copyright 2005-2009, Micrium, Weston, FL
*                                          All Rights Reserved
*
*
* File    : OS_CFG.H
* By      : Jean J. Labrosse
* Version : V2.88
*
* LICENSING TERMS:
* ---------------
*   uC/OS-II is provided in source form for FREE evaluation, for educational use or for peaceful research.
* If you plan on using  uC/OS-II  in a commercial product you need to contact Micri祄 to properly license
* its use in your product. We provide ALL the source code for your convenience and to help you experience
* uC/OS-II.   The fact that the  source is provided does  NOT  mean that you can use it without  paying a
* licensing fee.
*********************************************************************************************************
*/

#ifndef OS_CFG_H
#define OS_CFG_H

/************************************************************************************************
                                    CONTROL SWITCH
*************************************************************************************************/
#define OS_TASK_IDLE_EN         0       // Idle task enable switch
#define OS_TABLE_IN_FLASH       1       // Constant variable in RAM or FLASH
#define OS_STK_GROWTH_UP        0       // Stack growth direction
#define OS_STK_CHK_EN           0       // Stack check enable

/************************************************************************************************
                                    STACK CONFIG
*************************************************************************************************/
#define OS_MAX_STK_SIZE         1024	// Maximum system stack size

/************************************************************************************************
                                    TASK CONFIG
*************************************************************************************************/
#define OS_LOWEST_PRIO          3       // MAX TASK NUMBER-1,与实际任务数不一致会导致程序跑飞

// 如果使能OS_TASK_IDLE_EN则需要加大OS_MAX_STK_SIZE长度
#if	OS_TASK_IDLE_EN>0
#define OS_IDLE_PRIO            4       // Lowest priority
#define OS_TASK_IDLE_STK_SIZE   200     // Idle task stack size
#define OS_CPUFREE_FACTOR       34      // Calculation the CPU usage
#endif

/************************************************************************************************
                                    EVENT NUMNER DEFINITION
*************************************************************************************************/
#define TASK_EVENT              INT16U

/************************************************************************************************
                                    TIMER CONFIG
*************************************************************************************************/
#define OS_TICKS_PER_SEC        1000     // The timer ISR number in 1ms

/************************************************************************************************
*					message queue configer														*
*************************************************************************************************/
#define 	OS_Q_EN					0
#define 	OS_MAX_QS				2
#define 	OS_Q_ACCEPT_EN			1
#define 	OS_Q_FLUSH_EN			1
#define 	OS_Q_POST_EN 			1


#endif  // __OS_CONFIG_H__
