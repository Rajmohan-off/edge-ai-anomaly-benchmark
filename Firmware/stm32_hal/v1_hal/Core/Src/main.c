/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "app_x-cube-ai.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
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
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */
#define MPU6050_ADDR         (0x68 << 1)
#define MPU6050_PWR_MGMT_1   0x6B
#define MPU6050_ACCEL_XOUT_H 0x3B

float ax = 0.0f;
float ay = 0.0f;
float az = 0.0f;

float probability[3] = {0.0f};
int8_t prediction=0;

#define WINDOW_SIZE 100 // Collect 100 samples before making a final decision
uint8_t prediction_window[WINDOW_SIZE];
uint16_t window_idx = 0;
int window_count = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
int _write(int file, char *ptr, int len)
{
    for (int i = 0; i < len; i++)
    {
        // ITM_SendChar is a built-in ARM CMSIS function included with HAL
        ITM_SendChar(*ptr++);
    }
    return len;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_I2C1_Init();
  MX_X_CUBE_AI_Init();
  /* USER CODE BEGIN 2 */
  uint8_t reset_cmd = 0x80;
  uint8_t wake_cmd = 0x00;
  float probability[3] = {0};
  uint8_t accel_raw_bytes[6];

    printf("\r\n--- Boot Sequence ---\r\n");

    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, MPU6050_PWR_MGMT_1, 1, &reset_cmd, 1, 100);


    HAL_Delay(100);

    if (HAL_I2C_IsDeviceReady(&hi2c1, MPU6050_ADDR, 3, 100) == HAL_OK)
    {
        printf("[SUCCESS] MPU6050 detected at address 0x68!\r\n");

        if (HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, MPU6050_PWR_MGMT_1, 1, &wake_cmd, 1, 100) == HAL_OK) {
            printf("[SUCCESS] MPU6050 is awake and ready.\r\n");
        } else {
            printf("[ERROR] Found sensor, but failed to write wake command.\r\n");
        }
    }
    else
    {
        printf("[FATAL HARDWARE ERROR] No MPU6050 detected. Check wires & address!\r\n");
    }


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, MPU6050_ACCEL_XOUT_H, 1, accel_raw_bytes, 6, 100) == HAL_OK)
	        {
	            // Combine High and Low bytes (16-bit signed integers)
	            int16_t raw_ax = (int16_t)(accel_raw_bytes[0] << 8 | accel_raw_bytes[1]);
	            int16_t raw_ay = (int16_t)(accel_raw_bytes[2] << 8 | accel_raw_bytes[3]);
	            int16_t raw_az = (int16_t)(accel_raw_bytes[4] << 8 | accel_raw_bytes[5]);

	            // Convert raw LSB to physical acceleration units (±2g range => divide by 16384.0)
	            ax = (float)raw_ax / 16384.0f;
	            ay = (float)raw_ay / 16384.0f;
	            az = (float)raw_az / 16384.0f;

	            //Run the AI Inference
	            MX_X_CUBE_AI_Process();

	            uint8_t instant_prediction = prediction;

	             // Store the result in Sliding Window
	            window_count++;
	            prediction_window[window_idx] = instant_prediction;
	            window_idx++;


	                      if (window_idx >= WINDOW_SIZE)
	                      {

	                          int count_ideal = 0;
	                          int count_minor = 0;
	                          int count_major = 0;

	                          for (int i = 0; i < WINDOW_SIZE; i++) {
	                              if (prediction_window[i] == 0) count_ideal++;
	                              else if (prediction_window[i] == 1) count_minor++;
	                              else if (prediction_window[i] == 2) count_major++;
	                          }

	                          uint8_t reliable_status = 0;
	                          if (count_minor > count_ideal && count_minor > count_major) {
	                              reliable_status = 1;
	                          } else if (count_major > count_ideal && count_major > count_minor) {
	                              reliable_status = 2;
	                          }


	                          switch (reliable_status)
	                          {
	                              case 0: printf("Ideal\r\n"); break;
	                              case 1: printf("Normal Load\r\n"); break;
	                              case 2: printf("Abnormal Load\r\n"); break;
	                          }
	                          printf("window_count %d\r\n,", window_count);


	                          window_idx = 0;
	                      }
	        }
	        else
	        {
	            printf("[I2C BUS ERROR] Reading MPU6050 data failed.\r\n");
	        }


    /* USER CODE END WHILE */
  }

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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
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
#ifdef USE_FULL_ASSERT
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
