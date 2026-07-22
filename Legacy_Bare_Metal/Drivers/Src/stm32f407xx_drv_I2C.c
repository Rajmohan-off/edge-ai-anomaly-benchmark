/*
 * stm32f407xx_drv_I2C.c
 *
 *  Created on: 29-Jun-2026
 *      Author: rajmo
 */

#include "stm32f407xx.h"
#include "stm32f407xx_drv_I2C.h"


static void I2CMasterStartCondition(I2C_RegDef_t *pI2Cx);
static void I2CMasterStopCondition(I2C_RegDef_t *pI2Cx);
static void I2CAddrClearFlag(I2C_RegDef_t *pI2Cx);
static void I2CSlaveAddrWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr);
static void I2CSlaveAddrRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr);


//AHB prescalar possibility of clock divide
uint16_t AHB_Prescalar[8] = {2, 4, 8, 16, 32, 64, 128, 512};

//APB prescalar possibility of clock divide
uint16_t APB_Prescalar[4] = {2, 4, 8, 16};

//read clk value
uint32_t RCC_Clk_Value(void)
{
	uint32_t clkvalue, pclk1;
	uint8_t ClkSrc, temp, AHB, temp1, APB;

	//to read the main clock source
	ClkSrc = (RCC->CFGR >> 2) & 0x3;

	if(ClkSrc == 0)
	{
		clkvalue = 16000000;    //where the clock is HSI so its 16Mhz
	}

	else if(ClkSrc == 1)
	{
		clkvalue = 8000000;     //where the clock is HSE so its 8Mhz
	}
	// PLL is not configured so ClkSrc is not there

	//to read the AHB prescaler return clock
	temp = (RCC->CFGR >> 4) & 0xF;
	{
		if (temp < 8)
		{
			AHB = 1;
		}
		else
		{
			AHB = AHB_Prescalar[temp - 8];
		}
	}

	//to read the APB prescalar return clock
	temp1 = (RCC->CFGR >> 10) & 0x7;
	{
		if(temp1 == 0)
		{
			APB = 1;
		}

		else
		{
			APB = APB_Prescalar[temp1 - 4];
		}
	}

	pclk1 = (clkvalue/AHB)/APB;

	return pclk1;
}



//I2C Peripheral clock declare
/*
 * @fn        - I2C_PeriClkCon
 * @brief     - This function enable/disable peripheral clock for the I2C
 *
 * @Param_1   - base address of respective I2C
 * @param_2   - Enable/Disable
 *
 * @return    - None
 *
 * @note      - none
 *
 */
void I2C_PeriClkCon(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
	if (EnorDi == ENABLE)
	{
		if (pI2Cx == I2C1)
		{
			I2C1_CLK_EN();
		}

		if (pI2Cx == I2C2)
		{
			I2C2_CLK_EN();
		}

		if (pI2Cx == I2C3)
		{
			I2C3_CLK_EN();
		}

	}

	else if (EnorDi == DISABLE)
	{
		if (pI2Cx == I2C1)
		{
			I2C1_CLK_DI();
		}

		if (pI2Cx == I2C2)
		{
			I2C2_CLK_DI();
		}

		if (pI2Cx == I2C3)
		{
			I2C3_CLK_DI();
		}

	}
}


//Init & De-init
/*
 * @fn        - I2C_Init
 * @brief     - This function initialize the I2C
 *
 * @Param_1   - Handle the port and pin
 *
 * @return    - None
 *
 * @note      - none
 *
 */
void I2C_Init(I2C_Handle_t *pI2C_Handle)
{
	//config CR1 register (only ACK reg is used)
	uint32_t regtemp = 0;
	regtemp = (pI2C_Handle->I2C_Config.I2C_ACKControl << 10);        // ack config
	pI2C_Handle->pI2Cx->I2C_CR1 = regtemp;

	//config CR2 register (only FREQ reg is used)
	regtemp = 0;
	regtemp = RCC_Clk_Value()/1000000U;
	pI2C_Handle->pI2Cx->I2C_CR2 = regtemp & 0x3F;

	//config the slave own address register
	regtemp = 0;
	regtemp = (pI2C_Handle->I2C_Config.I2C_DeviceAddress << 1);
	pI2C_Handle->pI2Cx->I2C_OAR1 |= (1 << 14);
	pI2C_Handle->pI2Cx->I2C_OAR1 |= regtemp;

	//config CCR register for control I2C based on speed
	regtemp=0;
	if (pI2C_Handle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
	{
		uint16_t CCR_Value = (RCC_Clk_Value()/(2*pI2C_Handle->I2C_Config.I2C_SCLSpeed));
		regtemp |= CCR_Value & 0xFFF;
	}
	else
	{
		uint16_t CCR_Value;
		regtemp |= (1 << 15);
		regtemp |= (pI2C_Handle->I2C_Config.I2CFMDutyCycle << 14);
		if (pI2C_Handle->I2C_Config.I2CFMDutyCycle == 0)
		{
			CCR_Value = (RCC_Clk_Value()/(3*pI2C_Handle->I2C_Config.I2C_SCLSpeed));
		}
		else
		{
			CCR_Value = (RCC_Clk_Value()/(25*pI2C_Handle->I2C_Config.I2C_SCLSpeed));
		}
		regtemp |= CCR_Value & 0xFFF;
	}

	pI2C_Handle->pI2Cx->I2C_CCR = regtemp;

	//config TRISE
	if (pI2C_Handle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
	{
		regtemp = (RCC_Clk_Value()/1000000U)+1;
	}
	else
	{
		regtemp = (RCC_Clk_Value()/1000000U)+1;
	}
	pI2C_Handle->pI2Cx->I2C_TRISE = (regtemp & 0x3F);

}

/*
 * @fn        - I2C_DeInit
 * @brief     - deinitialize the I2C
 *
 * @Param_1   - Base address of respective I2C
 *
 * @return    - None
 *
 * @note      - none
 *
 */
void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{
	if (pI2Cx == I2C1)
	{
		I2C1_RESET();
	}

	if (pI2Cx == I2C2)
	{
		I2C2_RESET();
	}

	if (pI2Cx == I2C3)
	{
		I2C3_RESET();
	}
}



// IRQ pin and priority Config
/*
* @fn        - I2C_IRQpinConfig
* @brief     - Functionality to config Interrupt pin
*
* @Param_1   - Interrupt request querry number
* @param_2   - enable/disable the interrupt
*
* @return    - None
*
* @note      - none
*
*/
void I2C_IRQpinConfig(uint8_t IRQNumber, uint8_t EnorDi)
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
* @fn        - I2C_IRQPriorityConfig
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
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority)
{
	uint8_t temp_4 = IRQNumber % 4;
	uint8_t temp_3 = IRQNumber / 4;

	uint8_t shift  = (8 * temp_4) + (8 - NO_PR_BITS);  // IRQ priority has 4 32 register with categorized by 4 and consist of 8 pin each, among those 4 were reserved
	*(NVIC_IPR_BASE_ADDR + (temp_3)) |=  (IRQPriority << shift);
}



// other peripherals control
/*
* @fn        - I2C_PeripheralControl
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
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
	if (EnorDi == ENABLE)
	{
		pI2Cx->I2C_CR1 |= (1 << (I2C_CR1_PE));
	}

	if (EnorDi == DISABLE)
	{
		pI2Cx->I2C_CR1 &= ~(0 << (I2C_CR1_PE));
	}

}


// declared as static because the value should't get change, used for SET start BIT
/*
* @fn        - I2CMasterStartCondition
* @brief     - Functionality to start the master condition in STM32
*
* @Param_1   - Address of the respective I2C 
*
* @return    - None
*
* @note      - none
*
*/
static void I2CMasterStartCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->I2C_CR1 |= (1 << I2C_CR1_START);
}

/*
* @fn        - I2CMasterStopCondition
* @brief     - Functionality to stop the master condition in STM32
*
* @Param_1   - Address of the respective I2C 
*
* @return    - None
*
* @note      - none
*
*/
static void I2CMasterStopCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->I2C_CR1 |= (1 << I2C_CR1_STOP);
}

/*
* @fn        - I2CSlaveAddrWrite
* @brief     - Functionality to write slave address
*
* @Param_1   - Address of the respective I2C 
* @Param_2   - Slave address of the respective I2C device
*
* @return    - None
*
* @note      - none
*
*/
static void I2CSlaveAddrWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = (SlaveAddr << 1); // getting space for the R/W bit
	SlaveAddr &= ~(1);  // the first BIT is 0 because the of Write function
	pI2Cx->I2C_DR = SlaveAddr;
}

/*
* @fn        - I2CSlaveAddrRead
* @brief     - Functionality to read slave address
*
* @Param_1   - Address of the respective I2C 
* @Param_2   - Slave address of the respective I2C device
*
* @return    - None
*
* @note      - none
*
*/
static void I2CSlaveAddrRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = (SlaveAddr << 1); // getting space for the R/W bit
	SlaveAddr |= 1;  // the first BIT is 0 because the of Write function
	pI2Cx->I2C_DR = SlaveAddr;
}

/*
* @fn        - I2CAddrClearFlag
* @brief     - Functionality to clear flag
*
* @Param_1   - Address of the respective I2C 
*
* @return    - None
*
* @note      - none
*
*/
static void I2CAddrClearFlag(I2C_RegDef_t *pI2Cx)
{
	uint32_t dummy_read = pI2Cx->I2C_SR1;
	dummy_read = pI2Cx->I2C_SR2;
	(void) dummy_read;
}

//API for send data from master
/*
* @fn        - I2C_MasterSendData
* @brief     - Functionality to send data 
*
* @Param_1   - I2C Handle peripheral
* @param_2   - Data to receive 
* @param_3   - len of the data
* @param_4   - Slave address of the respective I2C device
*
* @return    - None
*
* @note      - none
*
*/
void I2C_MasterSendData(I2C_Handle_t *pI2C_Handle, uint8_t *pTxBuffer, uint32_t len, uint8_t slaveaddr)
{
	// To SET start BIT, here call the predefined fun for that
	I2CMasterStartCondition(pI2C_Handle->pI2Cx );

	// check the status of the Start bit, until it On the CPU gets locked here
	while(! I2C_GetFlagStatus(pI2C_Handle->pI2Cx, I2C_FLAG_SB));

	// To SET the slave address in Data Register
	I2CSlaveAddrWrite(pI2C_Handle->pI2Cx, slaveaddr);

	// check whether the slave address got SET
	while(! I2C_GetFlagStatus(pI2C_Handle->pI2Cx, I2C_FLAG_ADDR));

    // To clear the flag address
	I2CAddrClearFlag(pI2C_Handle->pI2Cx);

	// check the len of the data and transmit it from TxBuffer to DataRegister
	while(len > 0)
	{
		while (! I2C_GetFlagStatus(pI2C_Handle->pI2Cx, I2C_FLAG_TxE));
		pI2C_Handle->pI2Cx->I2C_DR = *pTxBuffer;                            // Put current 1 byte into DR
		pTxBuffer++;                                                        // Shift the pointer to next byte
		len--;                                                              // Decrease the count of bytes
	}

	// once the data got succesfully transmitted, with the confirmation from TxE that DR is cleared
	// check for the BTF, if BTF = 1 then the entire data got transmitted, no left
	while(! I2C_GetFlagStatus(pI2C_Handle->pI2Cx, I2C_FLAG_TxE));
	while(! I2C_GetFlagStatus(pI2C_Handle->pI2Cx, I2C_FLAG_BTF));


	I2CMasterStopCondition(pI2C_Handle->pI2Cx );
}



//API for receive data from slave device
/*
* @fn        - I2C_MasterReceiveData
* @brief     - Functionality to receive data 
*
* @Param_1   - I2C Handle peripheral
* @param_2   - Data to receive 
* @param_3   - len of the data
* @param_4   - Slave address of the respective I2C device
*
* @return    - None
*
* @note      - none
*
*/
void I2C_MasterReceiveData(I2C_Handle_t *pI2C_Handle, uint8_t *pRxBuffer, uint32_t len, uint8_t slaveaddr)
{
	// SET the start BIT
	I2CMasterStartCondition(pI2C_Handle->pI2Cx);

	// To checK the start bit STATUS is SET
	while(! I2C_GetFlagStatus(pI2C_Handle->pI2Cx, I2C_FLAG_SB));

	// To send the slaveaddr
	I2CSlaveAddrRead(pI2C_Handle->pI2Cx, slaveaddr);

	// To check for the slave address SET status
	while (! I2C_GetFlagStatus(pI2C_Handle->pI2Cx, I2C_FLAG_ADDR));

	//Receive a single Byte of data from slave device
	if (len == 1)
	{
		//Disable ACK
		I2C_Manage_ACK(pI2C_Handle->pI2Cx, I2C_ACK_DISABLE);

	    //clear the addr flag
		I2CAddrClearFlag(pI2C_Handle->pI2Cx);

		//wait until RxNE SET
		while (! I2C_GetFlagStatus(pI2C_Handle->pI2Cx, I2C_FLAG_RxNE));

		//STOP condition
		I2CMasterStopCondition(pI2C_Handle->pI2Cx );

		//read data into buffer
		*pRxBuffer = pI2C_Handle->pI2Cx->I2C_DR;

	}

	//Receive a multi Byte of data from slave device
	if (len > 1)
	{
		// 1. CRITICAL: Force ACK on BEFORE clearing the ADDR flag!
		I2C_Manage_ACK(pI2C_Handle->pI2Cx, I2C_ACK_ENABLE);
	    //clear the addr flag
		I2CAddrClearFlag(pI2C_Handle->pI2Cx);

		for(uint32_t i = len; i > 0; i--)
		{
			//wait until RxNE SET
			while (! I2C_GetFlagStatus(pI2C_Handle->pI2Cx, I2C_FLAG_RxNE));

			if(i == 2)
			{
				//Disable ACK
				I2C_Manage_ACK(pI2C_Handle->pI2Cx, I2C_ACK_DISABLE);

			    //clear the addr flag
				I2CMasterStopCondition(pI2C_Handle->pI2Cx);

			}
			*pRxBuffer = pI2C_Handle->pI2Cx->I2C_DR;
			pRxBuffer++;
		}
	}
	if(pI2C_Handle->I2C_Config.I2C_ACKControl == ENABLE)
	{
		I2C_Manage_ACK(pI2C_Handle->pI2Cx, I2C_ACK_ENABLE);
	}
}


/*
* @fn        - I2C_GetFlagStatus
* @brief     - Functionality to manage Flag status
*
* @Param_1   - Base address of respective I2C
* @param_2   - peripheral flagname
*
* @return    - 0 or 1
*
* @note      - none
*
*/
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName)
{
	if (pI2Cx->I2C_SR1 & FlagName)
	{
		return SET;
	}
	return RESET;
}


/*
* @fn        - I2C_Manage_ACK
* @brief     - Functionality to manage acknowledge BIT
*
* @Param_1   - Base address of respective I2C
* @param_2   - enable/disable the interrupt
*
* @return    - None
*
* @note      - none
*
*/
void I2C_Manage_ACK (I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
	if (EnorDi == I2C_ACK_ENABLE)
	{
		pI2Cx->I2C_CR1 |= (1 << I2C_CR1_ACK);
	}
	else
	{
		pI2Cx->I2C_CR1 &= ~(1 << I2C_CR1_ACK);
	}
}




