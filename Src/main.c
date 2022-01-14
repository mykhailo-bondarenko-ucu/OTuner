/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "encoder.h"
#include "interface.h"
#include "tunings.h"
#include "supmath.h"
#include "ssd1306.h"
#include "string.h"
#include "stdio.h"
#include "fonts.h"
#include "notes.h"
#include "math.h"

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

// === Functions for delay timer

volatile uint8_t tim2UpdateInterruptFlag = 0;
void tim2_32BitDelay(uint32_t delay_ticks) {
  uint16_t tim2_st = __HAL_TIM_GET_COUNTER(&htim2);
  uint16_t ticksUntilInterr = 0xFFFF - tim2_st;

  if (delay_ticks < ((uint32_t) ticksUntilInterr)) {
    while (((uint16_t) (__HAL_TIM_GET_COUNTER(&htim2) - tim2_st)) < delay_ticks) {}
    return;
  }

  delay_ticks -= ticksUntilInterr;
  tim2UpdateInterruptFlag = 0;
  while (!tim2UpdateInterruptFlag) {}

  while (delay_ticks > 0xFFFF) {
    delay_ticks -= 0xFFFF;
    tim2UpdateInterruptFlag = 0;
    while (!tim2UpdateInterruptFlag) {}
  }

  while (((uint16_t) __HAL_TIM_GET_COUNTER(&htim2)) < delay_ticks) {}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM2) {
    tim2UpdateInterruptFlag = 1;
  }
}

// === GPIO Interrupt Handlers

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == ENC_BTN_Pin) {
    encoder_handle_button_interrupt();
  }

  if (GPIO_Pin == ENC_CLK_Pin) {
    encoder_handle_clk_interrupt();
  }
}

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
  MX_TIM2_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  // === setup timers

  HAL_TIM_Base_Start_IT(&htim2);

  // === setup all pins

  HAL_Delay(10);  // blink C13 for restart confirmation
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
  // reset diodes
  HAL_GPIO_WritePin(S1_GPIO_Port, S1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(S2_GPIO_Port, S2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(S3_GPIO_Port, S3_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(S4_GPIO_Port, S4_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(S5_GPIO_Port, S5_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(S6_GPIO_Port, S6_Pin, GPIO_PIN_RESET);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  interface_init(&hi2c1);

  while (1)
  {
    if (encoder_position_change() == ENCODER_OK) {
      interface_register_encoder_position(encoder.position);
    }

    if (encoder_long_press() == ENCODER_OK) {
      interface_register_long_press();
    }

    if (encoder_single_press() == ENCODER_OK) {
      interface_register_single_press();
    }

    // TODO: diode interface
    HAL_GPIO_WritePin(GPIOA, INTERFACE_P.pitch_selection_lighted_diode_pin, GPIO_PIN_SET);
    // TODO: timer interface (?)
    tim2_32BitDelay(INTERFACE_P.diode_delay_ticks[INTERFACE_P.pitch_selection_current_string_id].light_ticks);
    HAL_GPIO_WritePin(GPIOA, INTERFACE_P.pitch_selection_lighted_diode_pin, GPIO_PIN_RESET);
    tim2_32BitDelay(INTERFACE_P.diode_delay_ticks[INTERFACE_P.pitch_selection_current_string_id].pause_ticks);

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
