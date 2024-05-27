/*!
 * @file        apm32e10x_int.c
 *
 * @brief      Main Interrupt Service Routines
 *
 * @version     V1.0.1
 *
 * @date        2022-07-29
 *
 * @attention
 *
 *  Copyright (C) 2021-2022 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be usefull and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

/* Includes */
#include "main.h"
#include "apm32e10x_int.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup USART_LIN
  @{
  */

/** @defgroup USART_LIN_INT_Functions
  @{
  */

/*!
 * @brief   This function handles NMI exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void NMI_Handler(void)
{
}

/*!
 * @brief   This function handles Hard Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void HardFault_Handler(void)
{
    /** Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Memory Manage exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void MemManage_Handler(void)
{
    /** Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Bus Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void BusFault_Handler(void)
{
    /** Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Usage Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void UsageFault_Handler(void)
{
    /** Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}


/*!
 * @brief   This function handles Debug Monitor exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void DebugMon_Handler(void)
{
}

void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_ReadIntFlag(DMA1_INT_FLAG_TC1)!=RESET)
	{
		//mLED_TEST_ON();	// 17.8us
		
		sAdcISR();
		
		//mLED_TEST_OFF();
		
		DMA_ClearIntFlag(DMA1_INT_FLAG_TC1);
	}
}

void USART1_IRQHandler(void)
{
	if(RESET != USART_ReadIntFlag(USART1, USART_INT_RXBNE))
	{
		sSciRxISR(cUSART1_ID);
	}
	
	if(RESET != USART_ReadIntFlag(USART1, USART_INT_TXBE))
	{
		sSciTxISR(cUSART1_ID);
	}
	
	if(RESET != USART_ReadStatusFlag(USART1, USART_FLAG_OVRE))
	{
		USART_ClearStatusFlag(USART1, USART_FLAG_OVRE);
		USART_RxData(USART1);
	}
}

void USBD1_LP_CAN1_RX0_IRQHandler(void)
{
	CAN_Rx0ISR(cCAN1_ID);
}

void CAN1_RX1_IRQHandler(void)
{
	CAN_Rx1ISR(cCAN1_ID);
}

void USBD1_HP_CAN1_TX_IRQHandler(void)
{
	CAN_TxISR(cCAN1_ID);
}

void CAN1_SCE_IRQHandler(void)
{
	CAN_SceISR(cCAN1_ID);
}

void USBD2_LP_CAN2_RX0_IRQHandler(void)
{
	CAN_Rx0ISR(cCAN2_ID);
}

void CAN2_RX1_IRQHandler(void)
{
	CAN_Rx1ISR(cCAN2_ID);
}

void USBD2_HP_CAN2_TX_IRQHandler(void)
{
	CAN_TxISR(cCAN2_ID);
}

void CAN2_SCE_IRQHandler(void)
{
	CAN_SceISR(cCAN2_ID);
}

/**@} end of group USART_LIN_INT_Functions */
/**@} end of group USART_LIN */
/**@} end of group Examples */
