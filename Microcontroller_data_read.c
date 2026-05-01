
#include "main.h"
#include <stdio.h>

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

/* PV */
float ax = 0.0f;
float ay = 0.0f;
float az = 0.0f;
/* PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);

/* Printf initiation code */
int _write(int file, char *ptr, int len)
{
  for(int i=0; i<len; i++)
  {
    ITM_SendChar((*ptr++)); // This specifically routes printf to the SWV Console
  }
  return len;
}

int main(void)
{

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();

  uint8_t mpu_addr = 0xD0; // MPU6050 I2C address (0x68 << 1)
    uint8_t wake_cmd = 0x00; // 0 wakes the sensor up

    // Send wake-up command
    if (HAL_I2C_Mem_Write(&hi2c1, mpu_addr, 0x6B, 1, &wake_cmd, 1, 100) == HAL_OK) {
        printf("MPU6050 Awake and Ready!\n");
    } else {
        printf("Failed to wake MPU6050. Check wiring!\n");
    }

  while (1)
  {
        uint8_t data[6];

        // Read 6 bytes starting from register 0x3B (ACCEL_XOUT_H)
        if (HAL_I2C_Mem_Read(&hi2c1, mpu_addr, 0x3B, 1, data, 6, 100) == HAL_OK) {

            // 1. Reconstruct the 16-bit raw values (High Byte << 8 | Low Byte)
            int16_t x_raw = (int16_t)(data[0] << 8 | data[1]);
            int16_t y_raw = (int16_t)(data[2] << 8 | data[3]);
            int16_t z_raw = (int16_t)(data[4] << 8 | data[5]);

            // 2. Convert to G-units
            // The default MPU6050 setting is +/- 2g. The scale factor is 16384.
            ax = x_raw / 16384.0f;
            ay = y_raw / 16384.0f;
            az = z_raw / 16384.0f;

            // 3. Print in CSV format: ax,ay,az
            printf("%.4f,%.4f,%.4f\n", ax, ay, az);
        } else {
            printf("I2C Read Error\n");
        }

        // 4. Sample rate delay (1ms delay = ~1000Hz sampling rate)
        HAL_Delay(1);
    }
}

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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_I2C1_Init(void)
{

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

}

static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}

static void MX_GPIO_Init(void)
{
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif 