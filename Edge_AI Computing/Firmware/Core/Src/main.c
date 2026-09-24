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
#include <string.h>
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

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

#define WINDOW_SIZE 50
#define NUM_FEATURES 5

// Declare the external 1D array
extern float sensor_window[WINDOW_SIZE * NUM_FEATURES];

// Tracks which timestep (0 to 49) we are currently on
int window_index = 0;

// --- I2C Addresses ---
#define MPU6050_ADDR 0xD0
#define INA226_ADDR  0x88

// MPU6050 Registers
#define MPU6050_REG_PWR_MGMT_1   0x6B
#define MPU6050_REG_ACCEL_XOUT_H 0x3B

// INA226 Registers
#define INA226_REG_CONFIG        0x00
#define INA226_REG_BUS_VOLTAGE   0x02
#define INA226_REG_CURRENT       0x04
#define INA226_REG_CALIBRATION   0x05

// --- Sensor Raw Data Arrays ---
uint8_t mpu_data[6];
uint8_t ina_data[2];

// --- Global Sensor Variables ---
// (These are the variables your app_x-cube-ai.c file will read)
float accel_g_x, accel_g_y, accel_g_z;
// INA226 Variables
uint8_t ina_data[2];
int16_t bus_voltage_raw;
float bus_voltage_V;       // Tracks Voltage in Volts
int16_t current_raw;
float current_mA;          // Tracks Current in milliamps

// --- AI Output Variables ---
// (We use 'extern' to fetch these answers from app_x-cube-ai.c)
extern int prediction;

#define WINDOW_SIZE 50
uint8_t prediction_window[WINDOW_SIZE];
int window_idx = 0;

const char* state_names[7] = {
    "Ideal off",                    // Maps to prediction = 0
    "Ideal off fallen",             // Maps to prediction = 1
    "Motor running - Fallen state", // Maps to prediction = 2
	"Motor running - Ideal",        // Maps to prediction = 3
	"Major load",                   // Maps to prediction = 4
    "Minor load",                   // Maps to prediction = 5
    "Heavy Obstacle",               // Maps to prediction = 6
};

// --- UART Buffer ---
char uart_buf[128];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, HAL_MAX_DELAY);
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
  MX_USART2_UART_Init();
  MX_X_CUBE_AI_Init();
  /* USER CODE BEGIN 2 */
  // ==========================================
  // 1. SOFTWARE RESET BOTH SENSORS
  // ==========================================
  uint8_t mpu_reset = 0x80;
  HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x6B, 1, &mpu_reset, 1, 100);
  // 1. Wake up MPU6050
  uint8_t wake_up = 0x00;
  HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, MPU6050_REG_PWR_MGMT_1, 1, &wake_up, 1, 100);

  // 2. Configure and Calibrate INA226
  // Write Configuration: 0x4127 (Default: 16 averages, 1.1ms conversion time, continuous mode)
  uint8_t ina_config[2] = {0x41, 0x27};
  HAL_I2C_Mem_Write(&hi2c1, INA226_ADDR, INA226_REG_CONFIG, 1, ina_config, 2, 100);

  // Write Calibration: 0x0200 (Sets Current LSB to 0.1mA for a 0.1 Ohm shunt)
  uint8_t ina226_cal[2] = {0x02, 0x00};
  HAL_I2C_Mem_Write(&hi2c1, INA226_ADDR, INA226_REG_CALIBRATION, 1, ina226_cal, 2, 100);
  // Brief delay to allow sensors to settle
  HAL_Delay(50);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, MPU6050_REG_ACCEL_XOUT_H, 1, mpu_data, 6, 100);
	  float accel_x = (int16_t)(mpu_data[0] << 8 | mpu_data[1]);
	  float accel_y = (int16_t)(mpu_data[2] << 8 | mpu_data[3]);
	  float accel_z = (int16_t)(mpu_data[4] << 8 | mpu_data[5]);

	  accel_g_x = accel_x / 16384.0f;
	  accel_g_y = accel_y / 16384.0f;
	  accel_g_z = accel_z / 16384.0f;

	  // ---------------------------------------------------------
	  // 2. READ INA226 (Current)
	  // ---------------------------------------------------------
	  HAL_I2C_Mem_Read(&hi2c1, INA226_ADDR, INA226_REG_CURRENT, 1, ina_data, 2, 100);
	  current_raw = (int16_t)(ina_data[0] << 8 | ina_data[1]);

	  // With a 0x1000 calibration, the current LSB is 0.1mA
	  current_mA = current_raw * 0.0125f;

	  MX_X_CUBE_AI_Process();

	  // Store the instant result in the Sliding Window
	        prediction_window[window_idx] = prediction;
	        window_idx++;

	        // ---------------------------------------------------------
	        // 5. EVALUATE WINDOW (MAJORITY VOTE)
	        // ---------------------------------------------------------
	        if (window_idx >= WINDOW_SIZE)
	        {
	            // Array to hold the vote tally for all 7 classes
	            int class_counts[7] = {0, 0, 0, 0, 0, 0, 0};

	            // Count how many times each class was predicted in the last 50 loops
	            for (int i = 0; i < WINDOW_SIZE; i++) {
	                class_counts[prediction_window[i]]++;
	            }

	            // Find which class got the highest number of votes
	            int max_votes = 0;
	            uint8_t reliable_status = 0;

	            for (int i = 0; i < 7; i++) {
	                if (class_counts[i] > max_votes) {
	                    max_votes = class_counts[i];
	                    reliable_status = i;
	                }
	            }

	            // Print the filtered, reliable status via UART
	            sprintf(uart_buf, "Reliable Status: %s\r\n", state_names[reliable_status]);
	            HAL_UART_Transmit(&huart2, (uint8_t*)uart_buf, strlen(uart_buf), 100);

	            // Reset window to start collecting the next batch
	            window_idx = 0;
	        }


    /* USER CODE END WHILE */

  MX_X_CUBE_AI_Process();
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
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
