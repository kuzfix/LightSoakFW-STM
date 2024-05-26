/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

#include "stm32g4xx_ll_usart.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_cortex.h"
#include "stm32g4xx_ll_system.h"
#include "stm32g4xx_ll_utils.h"
#include "stm32g4xx_ll_pwr.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_dma.h"

#include "stm32g4xx_ll_exti.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define COMPILE_DATE __DATE__
#define COMPILE_TIME __TIME__
#define FW_VERSION "1.0.47"

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CH4_CUR_EN_Pin GPIO_PIN_2
#define CH4_CUR_EN_GPIO_Port GPIOE
#define CH2_CUR_EN_Pin GPIO_PIN_3
#define CH2_CUR_EN_GPIO_Port GPIOE
#define CH1_SHNTRNG_100_Pin GPIO_PIN_4
#define CH1_SHNTRNG_100_GPIO_Port GPIOE
#define CH1_SHNTRNG_10_Pin GPIO_PIN_5
#define CH1_SHNTRNG_10_GPIO_Port GPIOE
#define CH3_SHNTRNG_100_Pin GPIO_PIN_6
#define CH3_SHNTRNG_100_GPIO_Port GPIOE
#define CH5_SHNTRNG_100_Pin GPIO_PIN_13
#define CH5_SHNTRNG_100_GPIO_Port GPIOC
#define CH3_SHNTRNG_10_Pin GPIO_PIN_14
#define CH3_SHNTRNG_10_GPIO_Port GPIOC
#define CH5_SHNTRNG_10_Pin GPIO_PIN_15
#define CH5_SHNTRNG_10_GPIO_Port GPIOC
#define CH1_SHNTRNG_1_Pin GPIO_PIN_3
#define CH1_SHNTRNG_1_GPIO_Port GPIOF
#define CH3_SHNTRNG_1_Pin GPIO_PIN_4
#define CH3_SHNTRNG_1_GPIO_Port GPIOF
#define CH5_SHNTRNG_1_Pin GPIO_PIN_7
#define CH5_SHNTRNG_1_GPIO_Port GPIOF
#define CH5_CUR_EN_Pin GPIO_PIN_8
#define CH5_CUR_EN_GPIO_Port GPIOF
#define CH3_CUR_EN_Pin GPIO_PIN_9
#define CH3_CUR_EN_GPIO_Port GPIOF
#define CH1_CUR_EN_Pin GPIO_PIN_10
#define CH1_CUR_EN_GPIO_Port GPIOF
#define CH1_VOLT_Pin GPIO_PIN_0
#define CH1_VOLT_GPIO_Port GPIOC
#define CH3_VOLT_Pin GPIO_PIN_1
#define CH3_VOLT_GPIO_Port GPIOC
#define CH5_VOLT_Pin GPIO_PIN_2
#define CH5_VOLT_GPIO_Port GPIOC
#define CH2_VOLT_Pin GPIO_PIN_3
#define CH2_VOLT_GPIO_Port GPIOC
#define CH4_VOLT_Pin GPIO_PIN_0
#define CH4_VOLT_GPIO_Port GPIOA
#define CH6_VOLT_Pin GPIO_PIN_1
#define CH6_VOLT_GPIO_Port GPIOA
#define LEDDRV_DAC_1_Pin GPIO_PIN_4
#define LEDDRV_DAC_1_GPIO_Port GPIOA
#define LEDDRV_DAC_2_Pin GPIO_PIN_5
#define LEDDRV_DAC_2_GPIO_Port GPIOA
#define DBG_PAD_3_Pin GPIO_PIN_6
#define DBG_PAD_3_GPIO_Port GPIOA
#define DBG_PAD_4_Pin GPIO_PIN_7
#define DBG_PAD_4_GPIO_Port GPIOA
#define CH1_CUR_Pin GPIO_PIN_7
#define CH1_CUR_GPIO_Port GPIOE
#define CH3_CUR_Pin GPIO_PIN_8
#define CH3_CUR_GPIO_Port GPIOE
#define CH5_CUR_Pin GPIO_PIN_9
#define CH5_CUR_GPIO_Port GPIOE
#define CH2_CUR_Pin GPIO_PIN_10
#define CH2_CUR_GPIO_Port GPIOE
#define CH4_CUR_Pin GPIO_PIN_11
#define CH4_CUR_GPIO_Port GPIOE
#define CH6_CUR_Pin GPIO_PIN_12
#define CH6_CUR_GPIO_Port GPIOE
#define DBG_LED_2_Pin GPIO_PIN_11
#define DBG_LED_2_GPIO_Port GPIOB
#define DBG_LED_1_Pin GPIO_PIN_12
#define DBG_LED_1_GPIO_Port GPIOB
#define DBG_PAD_1_Pin GPIO_PIN_9
#define DBG_PAD_1_GPIO_Port GPIOD
#define DBG_PAD_2_Pin GPIO_PIN_11
#define DBG_PAD_2_GPIO_Port GPIOD
#define CH1_PWM_Pin GPIO_PIN_14
#define CH1_PWM_GPIO_Port GPIOD
#define CH3_PWM_Pin GPIO_PIN_15
#define CH3_PWM_GPIO_Port GPIOD
#define LEDDRV_GPIO2_Pin GPIO_PIN_8
#define LEDDRV_GPIO2_GPIO_Port GPIOC
#define LEDDRV_GPIO1_Pin GPIO_PIN_9
#define LEDDRV_GPIO1_GPIO_Port GPIOC
#define CH5_PWM_Pin GPIO_PIN_9
#define CH5_PWM_GPIO_Port GPIOA
#define CH2_PWM_Pin GPIO_PIN_10
#define CH2_PWM_GPIO_Port GPIOA
#define CH4_PWM_Pin GPIO_PIN_11
#define CH4_PWM_GPIO_Port GPIOA
#define CH6_PWM_Pin GPIO_PIN_12
#define CH6_PWM_GPIO_Port GPIOA
#define USART3_COMMS_TX_Pin GPIO_PIN_10
#define USART3_COMMS_TX_GPIO_Port GPIOC
#define USART3_COMMS_RX_Pin GPIO_PIN_11
#define USART3_COMMS_RX_GPIO_Port GPIOC
#define ONEWIRE_Pin GPIO_PIN_6
#define ONEWIRE_GPIO_Port GPIOG
#define LPUART1_DBG_TX_Pin GPIO_PIN_7
#define LPUART1_DBG_TX_GPIO_Port GPIOG
#define LPUART1_DBG_RX_Pin GPIO_PIN_8
#define LPUART1_DBG_RX_GPIO_Port GPIOG
#define CH2_SHNTRNG_100_Pin GPIO_PIN_2
#define CH2_SHNTRNG_100_GPIO_Port GPIOD
#define CH4_SHNTRNG_100_Pin GPIO_PIN_3
#define CH4_SHNTRNG_100_GPIO_Port GPIOD
#define CH2_SHNTRNG_10_Pin GPIO_PIN_4
#define CH2_SHNTRNG_10_GPIO_Port GPIOD
#define CH6_SHNTRNG_100_Pin GPIO_PIN_5
#define CH6_SHNTRNG_100_GPIO_Port GPIOD
#define CH4_SHNTRNG_10_Pin GPIO_PIN_6
#define CH4_SHNTRNG_10_GPIO_Port GPIOD
#define CH2_SHNTRNG_1_Pin GPIO_PIN_7
#define CH2_SHNTRNG_1_GPIO_Port GPIOD
#define CH6_SHNTRNG_10_Pin GPIO_PIN_4
#define CH6_SHNTRNG_10_GPIO_Port GPIOB
#define CH6_SHNTRNG_1_Pin GPIO_PIN_5
#define CH6_SHNTRNG_1_GPIO_Port GPIOB
#define CH4_SHNTRNG_1_Pin GPIO_PIN_6
#define CH4_SHNTRNG_1_GPIO_Port GPIOB
#define CH6_CUR_EN_Pin GPIO_PIN_7
#define CH6_CUR_EN_GPIO_Port GPIOB
#define USART1_TEC_TX_Pin GPIO_PIN_0
#define USART1_TEC_TX_GPIO_Port GPIOE
#define USART1_TEC_RX_Pin GPIO_PIN_1
#define USART1_TEC_RX_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
