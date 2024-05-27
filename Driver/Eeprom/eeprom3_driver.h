/**
  ******************************************************************************
  * @file    EEPROM_Emulation/inc/eeprom.h 
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    07/27/2009
  * @brief   This file contains all the functions prototypes for the EEPROM 
  *          emulation firmware library.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EEPROM3_H
#define __EEPROM3_H

/* Includes ------------------------------------------------------------------*/
#include	"apm32e10x.h"
#include	"Main.h"

/* Exported constants --------------------------------------------------------*/
/* EEPROM start address in Flash */
#define EEPROM3_START_ADDRESS    ((uint32_t)FLASH_EEP3_ADDR_START)

/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint16_t EE3_Init(void);
uint16_t EE3_ReadVariable(uint16_t VirtAddress, uint16_t* Data);
uint16_t EE3_WriteVariable(uint16_t VirtAddress, uint16_t Data);

#endif /* __EEPROM3_H */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
