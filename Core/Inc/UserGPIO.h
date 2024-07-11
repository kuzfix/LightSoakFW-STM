/*
 * UserGPIO.h
 *
 *  Created on: Jul 10, 2024
 *      Author: maticpi
 */

#ifndef INC_USERGPIO_H_
#define INC_USERGPIO_H_

#include "main.h"

#define D1Tgl() HAL_GPIO_TogglePin(DBG_PAD_1_GPIO_Port, DBG_PAD_1_Pin)
#define D2Tgl() HAL_GPIO_TogglePin(DBG_PAD_2_GPIO_Port, DBG_PAD_2_Pin)
#define D3Tgl() HAL_GPIO_TogglePin(DBG_PAD_3_GPIO_Port, DBG_PAD_3_Pin)
#define D4Tgl() HAL_GPIO_TogglePin(DBG_PAD_4_GPIO_Port, DBG_PAD_4_Pin)
#define L1Tgl() HAL_GPIO_TogglePin(DBG_LED_1_GPIO_Port, DBG_LED_1_Pin)
#define L2Tgl() HAL_GPIO_TogglePin(DBG_LED_2_GPIO_Port, DBG_LED_2_Pin)

#define D1On() HAL_GPIO_WritePin(DBG_PAD_1_GPIO_Port, DBG_PAD_1_Pin,GPIO_PIN_SET)
#define D2On() HAL_GPIO_WritePin(DBG_PAD_2_GPIO_Port, DBG_PAD_2_Pin,GPIO_PIN_SET)
#define D3On() HAL_GPIO_WritePin(DBG_PAD_3_GPIO_Port, DBG_PAD_3_Pin,GPIO_PIN_SET)
#define D4On() HAL_GPIO_WritePin(DBG_PAD_4_GPIO_Port, DBG_PAD_4_Pin,GPIO_PIN_SET)
#define L1On() HAL_GPIO_WritePin(DBG_LED_1_GPIO_Port, DBG_LED_1_Pin,GPIO_PIN_SET)
#define L2On() HAL_GPIO_WritePin(DBG_LED_2_GPIO_Port, DBG_LED_2_Pin,GPIO_PIN_SET)

#define D1Off() HAL_GPIO_WritePin(DBG_PAD_1_GPIO_Port, DBG_PAD_1_Pin,GPIO_PIN_RESET)
#define D2Off() HAL_GPIO_WritePin(DBG_PAD_2_GPIO_Port, DBG_PAD_2_Pin,GPIO_PIN_RESET)
#define D3Off() HAL_GPIO_WritePin(DBG_PAD_3_GPIO_Port, DBG_PAD_3_Pin,GPIO_PIN_RESET)
#define D4Off() HAL_GPIO_WritePin(DBG_PAD_4_GPIO_Port, DBG_PAD_4_Pin,GPIO_PIN_RESET)
#define L1Off() HAL_GPIO_WritePin(DBG_LED_1_GPIO_Port, DBG_LED_1_Pin,GPIO_PIN_RESET)
#define L2Off() HAL_GPIO_WritePin(DBG_LED_2_GPIO_Port, DBG_LED_2_Pin,GPIO_PIN_RESET)

#endif /* INC_USERGPIO_H_ */
