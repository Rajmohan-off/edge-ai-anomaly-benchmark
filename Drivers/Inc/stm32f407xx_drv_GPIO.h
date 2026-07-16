/*
 * stm32f407xx_drv_GPIO.h
 *
 *  Created on: 18-Jun-2026
 *      Author: rajmo
 */

#ifndef INC_STM32F407XX_DRV_GPIO_H_
#define INC_STM32F407XX_DRV_GPIO_H_

#include "stm32f407xx.h"



// Pin peripherals config struct
typedef struct
{
	uint8_t GPIO_PinNumber;              //possible values from @GPIO_Pin_Num
	uint8_t GPIO_PinMode;                //possible values from @GPIO_Pin_Mode
	uint8_t GPIO_PinSpeed;               //possible values from @GPIO_Output_Speed
	uint8_t GPIO_PinPuPdControl;         //possible values from @GPIO_PUPD_Control
	uint8_t GPIO_PinOPType;              //possible values from @GPIO_Output_Type
	uint8_t GPIO_PinAltFunMode;

}GPIO_PinConfig_t;


//Handle
typedef struct
{
	GPIO_RegDef_t *pGPIOx;
	GPIO_PinConfig_t GPIO_PinConfig;

}GPIO_Handle_t;

/*
 * @GPIO_Pin_Num
 */
//GPIO Pin number macros
#define GPIO_PIN_No_0       0
#define GPIO_PIN_No_1       1
#define GPIO_PIN_No_2       2
#define GPIO_PIN_No_3       3
#define GPIO_PIN_No_4       4
#define GPIO_PIN_No_5       5
#define GPIO_PIN_No_6       6
#define GPIO_PIN_No_7       7
#define GPIO_PIN_No_8       8
#define GPIO_PIN_No_9       9
#define GPIO_PIN_No_10      10
#define GPIO_PIN_No_11      11
#define GPIO_PIN_No_12      12
#define GPIO_PIN_No_13      13
#define GPIO_PIN_No_14      14
#define GPIO_PIN_No_15      15



/*
 * @GPIO_Pin_Mode
 */
//GPIO Pin mode config macros
#define GPIO_Mod_IN        0
#define GPIO_Mod_OUT       1
#define GPIO_Mod_ALTFN     2
#define GPIO_Mod_ANLOG     3
#define GPIO_Mod_IT_FT     4   // input falling edge
#define GPIO_Mod_IT_RT     5   // input raising edge
#define GPIO_Mod_IT_RFT    6   // input raising/falling edge


/*
 * GPIO_Output_type
 */
//GPIO pin output
#define GPIO_OP_TYPE_PP    0   // Output type push pull
#define GPIO_OP_TYPE_OD    1   // Output type Open Drain

/*
 * @GPIO_Output_Speed
 */
//GPIO output speed
#define GPIO_SPEED_LOW     0
#define GPIO_SPEED_MEDIUM  1
#define GPIO_SPEED_FAST    2
#define GPIO_SPEED_HIGH    3

/*
 * @GPIO_PUPD_Control
 */
//GPIO PullUp / PullDown
#define GPIO_NO_PUPD       0
#define GPIO_PU            1
#define GPIO_PD            2


/*
 * @GPIO_PinAltFunMode
 */
//GPIO Alternate Function
#define GPIO_ALTFN_SYS                 0
#define GPIO_ALTFN_TIM_1_2             1
#define GPIO_ALTFN_TIM_3_5             2
#define GPIO_ALTFN_TIM_8_11            3
#define GPIO_ALTFN_I2C_1_3             4
#define GPIO_ALTFN_SPI_1_2             5
#define GPIO_ALTFN_SPI_3               6
#define GPIO_ALTFN_USART_1_3           7
#define GPIO_ALTFN_UART_4_5            8
#define GPIO_ALTFN_CAN_1_2__TIM12_14   9
#define GPIO_ALTFN_OTG                 10
#define GPIO_ALTFN_ETH                 11
#define GPIO_ALTFN_FSMC_SDIO_OTG       12
#define GPIO_ALTFN_DCMI                13

//API
//Peripherals clock control

void GPIO_PeriClkCon(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi);

//Init & De-init

void GPIO_Init(GPIO_Handle_t *pGPIO_Handle);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);

//Data Read&write
//Uint8 because the output will be either 0 or 1
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t GPIO_PinNumber);
//uint16, because the data read from port which is 16 bit
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);

void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t GPIO_PinNumber, uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value);
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t GPIO_PinNumber);


//IRG Config & handling

void GPIO_IRQpinConfig(uint8_t IRQNumber, uint8_t EnorDi);
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority);
void GPIO_IRQHandling(uint8_t PinNumber);




#endif /* INC_STM32F407XX_DRV_GPIO_H_ */
