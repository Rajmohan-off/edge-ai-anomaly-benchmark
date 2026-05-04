#include "main.h"
#include <stdio.h>
#include "my_model.h"
#include "my_model_data.h"

I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart1;

/* PV */
float ax = 0.0f;
float ay = 0.0f;
float az = 0.0f;

static uint8_t activations[AI_MY_MODEL_DATA_ACTIVATIONS_SIZE];
/* PV */

/* Private function prototypes*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);

/* Printf initiation code */
int _write(int file, char *ptr, int len)
{
  for(int i=0; i<len; i++)
  {
    ITM_SendChar((*ptr++)); // config for printf to the SWV Console
  }
  return len;
}

int main(void)
{
  //initialization clock, pin, I2C and USART in microcontroller
	HAL_Init();
	  SystemClock_Config();
	  MX_GPIO_Init();
	  MX_I2C1_Init();
	  MX_USART1_UART_Init();
	uint8_t mpu_addr = 0xD0;
	  uint8_t wake_cmd = 0x00;

	  if (HAL_I2C_Mem_Write(&hi2c1, mpu_addr, 0x6B, 1, &wake_cmd, 1, 100) == HAL_OK) {
	      printf("MPU6050 Awake!\n");
	  } else {
	      printf("Failed to wake MPU6050.\n");
	  }

	  // --- X-CUBE-AI INITIALIZATION ---

	    // 1. Declare the network handle
	    ai_handle my_model_handle;
	    ai_error ai_err;

	    // 2. Create the network in memory
	    ai_err = ai_my_model_create(&my_model_handle, AI_MY_MODEL_DATA_CONFIG);
	    if (ai_err.type != AI_ERROR_NONE) {
	        printf("AI Init Error: Type %d, Code %d\n", ai_err.type, ai_err.code);
	        Error_Handler();
	    }

	    // 3. Mount the weights and activations
	    ai_network_params ai_params = AI_NETWORK_PARAMS_INIT(
	          AI_MY_MODEL_DATA_WEIGHTS(ai_my_model_data_weights_get()),
	          AI_MY_MODEL_DATA_ACTIVATIONS(activations) 
	      );

	      if (!ai_my_model_init(my_model_handle, &ai_params)) {
	          printf("AI Weights Init Failed!\n");
	          Error_Handler();
	      }

	    printf("AI Model Initialized successfully!\n");

	    // --- END AI INITIALIZATION ---
	    uint8_t fault_counter = 0;
	    while (1)
	      {
	          uint8_t data[6];
	          if (HAL_I2C_Mem_Read(&hi2c1, mpu_addr, 0x3B, 1, data, 6, 100) == HAL_OK) {

	              int16_t x_raw = (int16_t)(data[0] << 8 | data[1]);
	              int16_t y_raw = (int16_t)(data[2] << 8 | data[3]);
	              int16_t z_raw = (int16_t)(data[4] << 8 | data[5]);

	              ax = x_raw / 16384.0f;
	              ay = y_raw / 16384.0f;
	              az = z_raw / 16384.0f;

	              // 1. Put MPU sensor data into the input array
	              ai_float in_data[3];
	              in_data[0] = ax;
	              in_data[1] = ay;
	              in_data[2] = az;

	              // 2. Create an array to hold the predictions
	              ai_float out_data[2] = {0.0f, 0.0f};

	              // 3. Set up the AI buffers required by the ST API
	              // 3.1 Get the pointers to the model's I/O structures
	              ai_buffer *ai_input = ai_my_model_inputs_get(my_model_handle, NULL);
	              ai_buffer *ai_output = ai_my_model_outputs_get(my_model_handle, NULL);

	              // 3.2 Link local data arrays to the AI buffers
	              ai_input[0].data = AI_HANDLE_PTR(in_data);
	              ai_output[0].data = AI_HANDLE_PTR(out_data);

	              // 3.3 Run the model
	              ai_i32 batch = ai_my_model_run(my_model_handle, ai_input, ai_output);
	              if (batch != 1) {
	                  printf("AI Invoke failed!\n");
	              } else {
	                  float prob_fault = out_data[0];
	                  float prob_normal = out_data[1];

	                  float threshold = 0.70f; // Trigger if confidence > 70%

	                              if (prob_fault > threshold) {
	                                  fault_counter++; // Increase strike counter

	                                  if (fault_counter >= 5) { // 5 strikes = real anomaly
	                                      printf(" ANOMALY! ax:%.2f ay:%.2f az:%.2f [Fault Conf: %.1f%%]\n",
	                                             ax, ay, az, prob_fault * 100.0f);

	                                      // HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET); // Turn on Red LED
	                                  } else {
	                                      printf(" Spike (%d/5)... [Fault Conf: %.1f%%]\n",
	                                             fault_counter, prob_fault * 100.0f);
	                                  }
	                              } else {
	                                  fault_counter = 0; // Reset counter if normal
	                                  printf(" Normal. ax:%.2f ay:%.2f az:%.2f [Normal Conf: %.1f%%]\n",
	                                         ax, ay, az, prob_normal * 100.0f);

	  
	                              }
	                          }

	                      } else {
	              printf("I2C Read Error\n");
	          }

	          HAL_Delay(100);
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
