/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2020 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l1xx_hal.h"
#include "adc.h"
#include "comp.h"
#include "dac.h"
#include "fatfs.h"
#include "rtc.h"
#include "sdio.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

typedef enum event_type_t_def {
	EVENT_NONE,
	EVENT_GEIGER1,
	EVENT_GEIGER2,
	EVENT_GEIGER3,
	EVENT_PHOTODIODE,
	EVENT_RTC,
} event_type_t;
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

FATFS my_fatfs;
FIL my_file;
UINT my_error;

extern char SD_Path[4];

char uart_buf[256];

volatile uint8_t event_flag = 0;
volatile event_type_t event_type = EVENT_NONE;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (!event_flag) {
		event_flag = 1;

		if (GPIO_Pin == GPIO_PIN_12) {
			// not working now - some HW error
			//event_type = EVENT_GEIGER1;
		} else if (GPIO_Pin == GPIO_PIN_13) {
			event_type = EVENT_GEIGER2;
		} else if (GPIO_Pin == GPIO_PIN_14) {
			event_type = EVENT_GEIGER3;
		}
	}
}

void HAL_COMP_TriggerCallback(COMP_HandleTypeDef *hcomp) {
	if (hcomp == &hcomp2) {
		if (!event_flag) {
			event_flag = 1;
			event_type = EVENT_PHOTODIODE;
		}
	}
}

void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc) {
	if (!event_flag) {
		event_flag = 1;
		event_type = EVENT_RTC;
	}
}

static uint16_t WriteToSD(uint8_t* buf, uint8_t len);
static void delay_us(uint16_t us);
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
	RTC_TimeTypeDef timestamp = { 0 };
	RTC_DateTypeDef date = { 0 };
	event_type_t event_local = EVENT_NONE;
	uint32_t geiger1_counts = 0;
	uint32_t geiger2_counts = 0;
	uint32_t geiger3_counts = 0;
	uint32_t photodiode_counts = 0;
	uint32_t photodiode_val = 0;
	uint8_t uart_len = 0;
	HAL_StatusTypeDef adc_status = HAL_OK;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_SDIO_SD_Init();
  MX_RTC_Init();
  MX_FATFS_Init();
  MX_ADC_Init();
  MX_USART2_UART_Init();
  MX_DAC_Init();
  MX_COMP2_Init();
  MX_TIM7_Init();

  /* USER CODE BEGIN 2 */

	f_mount(&my_fatfs, SD_Path, 1);
	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 249); // 249 - ~~200mV~
	HAL_COMP_Start_IT(&hcomp2);
	RTC_SetCompilationTime();

	HAL_RTC_GetTime(&hrtc, &timestamp, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BCD);
	uart_len = sprintf(uart_buf, "%d:%d:%d, RESET\r\n", timestamp.Hours,
			timestamp.Minutes, timestamp.Seconds);
	HAL_UART_Transmit_IT(&huart2, (uint8_t*) uart_buf, uart_len);
	WriteToSD((uint8_t*) uart_buf, uart_len);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
		if (event_flag) {
			event_local = event_type;
			event_flag = 0;
			switch (event_local) {
			case EVENT_GEIGER1:
				geiger1_counts++;
				HAL_Delay(1);
				// write to uart
				break;
			case EVENT_GEIGER2:
				geiger2_counts++;
				HAL_Delay(1);
				HAL_GPIO_TogglePin(LED5_GPIO_Port, LED5_Pin);
				break;
			case EVENT_GEIGER3:
				geiger3_counts++;
				HAL_Delay(1);
				HAL_GPIO_TogglePin(LED5_GPIO_Port, LED5_Pin);
				break;
			case EVENT_PHOTODIODE:
				photodiode_counts++;
				HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
				// wait and measure voltage
				HAL_Delay(1);
				HAL_ADC_Start(&hadc);
				adc_status = HAL_ADC_PollForConversion(&hadc, 100);

				if (adc_status == HAL_OK) {
					photodiode_val = HAL_ADC_GetValue(&hadc);
				}

				// reset peak detector
				HAL_GPIO_WritePin(PeakReset_GPIO_Port, PeakReset_Pin,
						GPIO_PIN_SET);
				HAL_Delay(1);
				HAL_GPIO_WritePin(PeakReset_GPIO_Port, PeakReset_Pin,
						GPIO_PIN_RESET);
				// write to uart & SD card
				HAL_RTC_GetTime(&hrtc, &timestamp, RTC_FORMAT_BIN);
				HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BCD);
				uart_len = sprintf(uart_buf, "%d:%d:%d, PHOTODIODE, %lu\r\n",
						timestamp.Hours, timestamp.Minutes, timestamp.Seconds,
						photodiode_val);
				HAL_UART_Transmit_IT(&huart2, (uint8_t*) uart_buf, uart_len);
				WriteToSD((uint8_t*) uart_buf, uart_len);

				break;
			case EVENT_RTC:
				HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
				// write number of counts to SD card
				HAL_RTC_GetTime(&hrtc, &timestamp, RTC_FORMAT_BIN);
				HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BCD);
				uart_len =
						sprintf(uart_buf,
								"%d:%d:%d, GEIGER1: %lu, GEIGER2: %lu, GEIGER3: %lu\r\n",
								timestamp.Hours, timestamp.Minutes,
								timestamp.Seconds, geiger1_counts,
								geiger2_counts, geiger3_counts);
				HAL_UART_Transmit_IT(&huart2, (uint8_t*) uart_buf, uart_len);
				WriteToSD((uint8_t*) uart_buf, uart_len);
				geiger1_counts = 0;
				geiger2_counts = 0;
				geiger3_counts = 0;
				break;
			case EVENT_NONE:
			default:
				// do nothing
				break;
			}
		}
		// todo: go to sleep here
	}
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
static uint16_t WriteToSD(uint8_t* buf, uint8_t len) {
	char my_file_name[20];
	static uint8_t datalogNum = 0;
	uint16_t returnCode = 0;
	FRESULT fresult;
	uint32_t fileSize;

	memset(my_file_name, ' ', sizeof(my_file_name));
	sprintf(my_file_name, "ARMS_%d.txt", datalogNum);
	fresult = f_open(&my_file, my_file_name, FA_WRITE | FA_OPEN_ALWAYS);
	fileSize = f_size(&my_file);
	fresult = f_close(&my_file);
	if (fileSize > 1000000) {
		datalogNum++;
		sprintf(my_file_name, "datalog%d.txt", datalogNum);
		// reset file object
		memset(&my_file, 0x00, sizeof(my_file));
		fileSize = 0;
	}
	fresult = f_open(&my_file, my_file_name, FA_WRITE | FA_OPEN_ALWAYS);
	if (fresult == FR_OK) {
		fresult = f_lseek(&my_file, fileSize);
		fresult = f_write(&my_file, buf, len, &my_error);
		if (fresult == FR_OK) {
			fresult = f_close(&my_file);
			if (fresult != FR_OK) {
				returnCode = 1;
			}
		} else {
			returnCode = 1;
		}
	} else {
		returnCode = 1;
	}
	return returnCode;
}

static void delay_us(uint16_t us) {
	__HAL_TIM_SET_COUNTER(&htim7, 0);
	__HAL_TIM_ENABLE(&htim7);
	while (__HAL_TIM_GET_COUNTER(&htim7) <= us)
		;
	__HAL_TIM_DISABLE(&htim7);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
