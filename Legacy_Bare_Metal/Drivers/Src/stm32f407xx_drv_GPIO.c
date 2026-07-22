/*
 * stm32f407xx_drv_GPIO.c
 *
 *  Created on: 18-Jun-2026
 *      Author: rajmo
 */

#include "stm32f407xx_drv_GPIO.h"
#include "stm32f407xx.h"


/*
 * @fn        - GPIO_PeriClkCon
 * @brief     - This function enable/disable clock for the given GPIO port
 *
 * @Param_1   - base address of GPIO
 * @param_2   - Enable/Disable
 *
 * @return    - None
 *
 * @note      - none
 *
 */
void GPIO_PeriClkCon(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi)
{
	if(EnorDi == ENABLE){

		if (pGPIOx == GPIOA){
			GPIOA_CLK_EN();
		}

		else if (pGPIOx == GPIOB){
			GPIOB_CLK_EN();
		}

		else if (pGPIOx == GPIOC){
			GPIOC_CLK_EN();
		}

		else if (pGPIOx == GPIOD){
			GPIOD_CLK_EN();
		}

		else if (pGPIOx == GPIOE){
			GPIOE_CLK_EN();
		}

		else if (pGPIOx == GPIOF){
			GPIOF_CLK_EN();
		}

		else if (pGPIOx == GPIOG){
			GPIOG_CLK_EN();
		}

		else if (pGPIOx == GPIOH){
			GPIOH_CLK_EN();
		}

		else if (pGPIOx == GPIOI){
			GPIOI_CLK_EN();
		}

	}

	else if (EnorDi == DISABLE){
		if (pGPIOx == GPIOA){
			GPIOA_CLK_DI();
		}

		else if (pGPIOx == GPIOB){
			GPIOB_CLK_DI();
		}

		else if (pGPIOx == GPIOC){
			GPIOC_CLK_DI();
		}

		else if (pGPIOx == GPIOD){
			GPIOD_CLK_DI();
		}

		else if (pGPIOx == GPIOE){
			GPIOE_CLK_DI();
		}

		else if (pGPIOx == GPIOF){
			GPIOF_CLK_DI();
		}

		else if (pGPIOx == GPIOG){
			GPIOG_CLK_DI();
		}

		else if (pGPIOx == GPIOH){
			GPIOH_CLK_DI();
		}

		else if (pGPIOx == GPIOI){
			GPIOI_CLK_DI();
		}

	}

}

/*
 * @fn        - GPIO_Init
 * @brief     - initialize the handle typedef
 *
 * @Param_1   - Handle the port and pin
 *
 * @return    - None
 *
 * @note      - none
 *
 */

void GPIO_Init(GPIO_Handle_t *pGPIO_Handle)
{
	uint32_t temp =0;

	//Initialize the Mode configure for the pin

	if (pGPIO_Handle->GPIO_PinConfig.GPIO_PinMode <= GPIO_Mod_ANLOG)
	{
		temp = (pGPIO_Handle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIO_Handle->GPIO_PinConfig.GPIO_PinNumber));
		pGPIO_Handle->pGPIOx->MODER |= temp;
	}

	else{

		//confi for the Interrupt fallinf trigger
		if (pGPIO_Handle->GPIO_PinConfig.GPIO_PinMode == GPIO_Mod_IT_FT)
		{

			EXTI->FTSR |= (1 << pGPIO_Handle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->RTSR &= ~(1 << pGPIO_Handle->GPIO_PinConfig.GPIO_PinNumber);
		}

		//config for Interrupt rising trigger
		else if (pGPIO_Handle->GPIO_PinConfig.GPIO_PinMode == GPIO_Mod_IT_RT)
		{
			EXTI->RTSR |= (1 << pGPIO_Handle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->FTSR &= ~(1 << pGPIO_Handle->GPIO_PinConfig.GPIO_PinNumber);

		}

		//config for Interrupt both Rising and Falling trigger
		else if (pGPIO_Handle->GPIO_PinConfig.GPIO_PinMode == GPIO_Mod_IT_RFT){
			EXTI->FTSR |= (1 << pGPIO_Handle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->RTSR |= (1 << pGPIO_Handle->GPIO_PinConfig.GPIO_PinNumber);

		}


		//config the SYSCFG which used for declare the port for the interrupt

		uint8_t temp1= pGPIO_Handle->GPIO_PinConfig.GPIO_PinNumber / 4 ;
		uint8_t temp2= pGPIO_Handle->GPIO_PinConfig.GPIO_PinNumber % 4 ;
		uint8_t Codespace = GPIO_To_SYSCFG(pGPIO_Handle->pGPIOx);
		SYSCFG->EXTICR[temp1] |= (Codespace  << (4 * temp2));

		//config for Interrupt of IMR

		EXTI->IMR |= (1 << pGPIO_Handle->GPIO_PinConfig.GPIO_PinNumber);


	}


	//Configure the speed
	temp = 0;

	temp = pGPIO_Handle->GPIO_PinConfig.GPIO_PinSpeed << (2 * pGPIO_Handle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIO_Handle->pGPIOx->OSPEEDR &= ~(0x3 << (2 * pGPIO_Handle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIO_Handle->pGPIOx->OSPEEDR |= temp;


	//Pull up and Pull Down configuration
	temp = 0;

	temp = pGPIO_Handle->GPIO_PinConfig.GPIO_PinPuPdControl << (2 * pGPIO_Handle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIO_Handle->pGPIOx->PUPDR &= ~(0x3 << (2 * pGPIO_Handle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIO_Handle->pGPIOx->PUPDR |= temp;


	//Output Type register config
	temp = 0;

	temp = pGPIO_Handle->GPIO_PinConfig.GPIO_PinOPType << (pGPIO_Handle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIO_Handle->pGPIOx->OTYPER &= ~(0x1 << (pGPIO_Handle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIO_Handle->pGPIOx->OTYPER |= temp;


	//ALTfn config

	if(pGPIO_Handle->GPIO_PinConfig.GPIO_PinMode == GPIO_Mod_ALTFN){

		uint8_t temp_1, temp_2;

		temp_1 = pGPIO_Handle->GPIO_PinConfig.GPIO_PinNumber / 8;
		temp_2 = pGPIO_Handle->GPIO_PinConfig.GPIO_PinNumber % 8;
		pGPIO_Handle->pGPIOx->AFR[temp_1] &= ~(0xF << (4 * temp_2));
		pGPIO_Handle->pGPIOx->AFR[temp_1] |= (pGPIO_Handle->GPIO_PinConfig.GPIO_PinAltFunMode << 4 * temp_2);
	}



}

/*
 * @fn        - GPIO_DeInit
 * @brief     - deinitialize the handle typedef
 *
 * @Param_1   - Base address of GPIO
 *
 * @return    - None
 *
 * @note      - none
 *
 */

void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{

	if (pGPIOx == GPIOA){
		GPIOA_REG_RESET();
	}

	else if (pGPIOx == GPIOB){
		GPIOB_REG_RESET();
	}

	else if (pGPIOx == GPIOC){
		GPIOC_REG_RESET();
	}

	else if (pGPIOx == GPIOD){
		GPIOD_REG_RESET();
	}

	else if (pGPIOx == GPIOE){
		GPIOE_REG_RESET();
	}

	else if (pGPIOx == GPIOF){
		GPIOF_REG_RESET();
	}

	else if (pGPIOx == GPIOG){
		GPIOG_REG_RESET();
	}

	else if (pGPIOx == GPIOH){
		GPIOH_REG_RESET();
	}

	else if (pGPIOx == GPIOI){
		GPIOI_REG_RESET();
	}

}

/*
* @fn        - GPIO_ReadFromInputPin
* @brief     - Functionality to read from the particular pin from particular port
*
* @Param_1   - Base address of GPIO
* @param_2   - pin of the peripheral
*
* @return    - 0 or 1
*
* @note      - none
*
*/
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t GPIO_PinNumber)
{

	uint8_t value = (pGPIOx->IDR >> GPIO_PinNumber) & 0x00000001;

	return value;

}

/*
* @fn        - GPIO_ReadFromInputPort
* @brief     - Functionality to read from the particular port
*
* @Param_1   - Base address of GPIO
* @param_2   - none
*
* @return    - 0 or 1
*
* @note      - none
*
*/
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{

	uint16_t value = (pGPIOx->IDR);

	return value;

}


/*
* @fn        - GPIO_WriteToOutputPin
* @brief     - Functionality to write to particular pin
*
* @Param_1   - Base address of GPIO
* @param_2   - pin of the peripheral
* @param_3   - output value
*
* @return    - None
*
* @note      - none
*
*/
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t GPIO_PinNumber, uint8_t Value)
{

	if (Value == SET){
		pGPIOx->ODR |= (1 << GPIO_PinNumber);
	}

	else{
		pGPIOx->ODR &= ~(1 << GPIO_PinNumber);
	}

}


/*
* @fn        - GPIO_WriteToOutputPort
* @brief     - Functionality to write to particular port
*
* @Param_1   - Base address of GPIO
* @param_2   - output value
*
* @return    - None
*
* @note      - none
*
*/
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value)
{

	pGPIOx->ODR = Value;

}


/*
* @fn        - GPIO_ToggleOutputPin
* @brief     - Functionality to toggle output
*
* @Param_1   - Base address of GPIO
* @param_2   - pin of the peripheral
*
* @return    - None
*
* @note      - none
*
*/
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t GPIO_PinNumber)
{

	pGPIOx->ODR ^= (1 << GPIO_PinNumber);

}


/*
* @fn        - GPIO_IRQConfig
* @brief     - Functionality to config Interrupt
*
* @Param_1   - Interrupt request querry number
* @param_2   - Interrupt priority
* @param_3   - enable/disable the interrupt
*
* @return    - None
*
* @note      - none
*
*/
void GPIO_IRQpinConfig(uint8_t IRQNumber, uint8_t EnorDi)
{

	if(EnorDi == ENABLE)
	{
		if (IRQNumber <= 31)
		{
			*NVIC_ISER0 |= (1 << IRQNumber);
		}

		else if (IRQNumber > 31 && IRQNumber < 64)
		{
			*NVIC_ISER1 |= (1 << IRQNumber);
		}

		else if (IRQNumber > 64)
		{
			*NVIC_ISER2 |= (1 << IRQNumber);
		}
	}

	else
	{
		if (IRQNumber <= 31)
		{
			*NVIC_ICER0 |= (1 << IRQNumber);
		}

		else if (IRQNumber > 31 && IRQNumber < 64)
		{
			*NVIC_ICER1 |= (1 << IRQNumber);
		}

		else if (IRQNumber > 64)
		{
			*NVIC_ICER2 |= (1 << IRQNumber);

	    }
	}

}


/*
* @fn        - GPIO_PriorityConfig
* @brief     - Functionality to config priority Interrupt
*
* @Param_1   - Interrupt request querry number
* @param_2   - Interrupt priority
*
* @return    - None
*
* @note      - none
*
*/
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority)
{
	uint8_t temp_4 = IRQNumber % 4;
	uint8_t temp_3 = IRQNumber / 4;

	uint8_t shift  = (8 * temp_4) + (8 - NO_PR_BITS);  // IRQ priority has 4 32 register with categorized by 4 and consist of 8 pin each, among those 4 were reserved
	*(NVIC_IPR_BASE_ADDR + (temp_3)) |=  (IRQPriority << shift);
}


/*
* @fn        - GPIO_IRQHandling
* @brief     - Functionality to handle Interrupt
*
* @Param_1   - pin of the GPIO
*
* @return    - None
*
* @note      - none
*
*/
void GPIO_IRQHandling(uint8_t PinNumber)
{
	if (EXTI->PR & (1 << PinNumber))
	{
		EXTI->PR |= (1 << PinNumber);
	}
}









