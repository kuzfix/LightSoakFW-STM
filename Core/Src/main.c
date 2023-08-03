/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "usart.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "front_end_control.h"
#include "debug.h"
#include "led_control.h"
#include "SCI.h"
#include "micro_sec.h"
#include "daq.h"
#include "measurements.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

volatile uint16_t adcBuf[600] = {0};
uint8_t adcCpltFlag = 0;

uint64_t test = 0xFFFFFF;





/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC3_Init();
  MX_DAC1_Init();
  MX_USART3_UART_Init();
  MX_LPUART1_UART_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_TIM20_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  dbg(Warning, "Booting LighSoak V1...\n");


  fec_init();
  ledctrl_init();
  SCI_init();
  usec_init();
  daq_init();

  uint32_t t1, t2;

  HAL_Delay(3000);

  //turn on active LED
  HAL_GPIO_WritePin(DBG_LED_1_GPIO_Port, DBG_LED_1_Pin, GPIO_PIN_SET);

//  fec_set_shunt_10x(1);
//  fec_set_force_voltage(1, 0.1f);
//  fec_enable_current(1);





  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

//////    dbg(Debug, "prepare for sample\n");
////    daq_prepare_for_sampling(1000);
//////    dbg(Debug, "prepare done\n");
////    HAL_Delay(10);
//////    dbg(Debug, "start sampling\n");
////    t1 = usec_get_timestamp_64();
////    daq_start_sampling();
//////    dbg(Debug, "sampling started\n");
////    while(!daq_is_sampling_done());
////    t2 = usec_get_timestamp_64();
//////    dbg(Debug, "sampling done. took: %d\n", t2-t1);
//////    dbg(Debug, "started sampling at: %llu\n", t1);
////
////    t_daq_sample_raw avg = daq_volt_raw_get_average(1000);
////    dbg(Debug, "avg voltage ch1: %d, at timestamp: %llu\n", avg.ch1, avg.timestamp);
////
////    t_daq_sample_raw avgcur = daq_curr_raw_get_average(1000);
////    dbg(Debug, "avg current ch1: %d, at timestamp: %llu\n", avgcur.ch1, avgcur.timestamp);
////
////    dbg(Debug, "preforming single shot measures...\n");
//
//    t_daq_sample_convd single_volt = daq_single_shot_volt(100);
//    t_daq_sample_convd single_curr = daq_single_shot_curr_no_autorng(100);
//
//    dbg(Warning, "single shot voltage: %f\n", single_volt.ch1);
//    dbg(Warning, "single shot current: %f\n", single_curr.ch1);



//    meas_get_current(0);


//    meas_get_voltage(0);

//    meas_basic_volt_test_dump_single_ch(1, 10);
//    meas_get_voltage_and_current(0);

    meas_get_current_at_forced_voltage(1, 0.14f);

    HAL_Delay(3000);
//    dbg(Warning, "   \n");






    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  dbg(Error, "Assert failed!!! file %s on line %d\r\n", file, line);
  HAL_Delay(100);
  //dissable all interrupts
  __disable_irq();
  while(1)
  {
  }
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
