/*
 * main.c
 *
 *  Created on: 10-Jul-2026
 *      Author: rajmo
 */

#include <stdint.h>
#include "stm32f407xx.h"
#include "stm32f407xx_drv_I2C.h"
#include "stm32f407xx_drv_GPIO.h"
#include "MPU6050_drv.h"
#include <stdio.h>
#include <string.h>


// Cortex M4 ITM Hardware Registers for SWV Printf
#define ITM_TER      (*(volatile uint32_t*)0xE0000E00) // Trace Enable Register
#define ITM_TCR      (*(volatile uint32_t*)0xE0000E80) // Trace Control Register
#define ITM_STIM0_8  (*(volatile uint8_t*)0xE0000000)  // Stimulus Port 0 (8-bit)
#define ITM_STIM0_32 (*(volatile uint32_t*)0xE0000000) // Stimulus Port 0 (32-bit)


// Custom implementation of ITM_SendChar for bare-metal from-scratch projects
int ITM_SendChar(int ch)
{
    // Check the ITM Trace Control is enabled AND Port 0 is enabled
    if ((ITM_TCR & 1) && (ITM_TER & 1))
    {
        while (ITM_STIM0_32 == 0);

        ITM_STIM0_8 = (uint8_t)ch;
    }
    return ch;
}


// This function overrides the default GCC _write function to use SWV
int _write(int file, char *ptr, int len)
{
    for (int i = 0; i < len; i++)
    {
        ITM_SendChar(*ptr++);
    }
    return len;
}

// Simple software delay
void delay(void)
{
    for(uint32_t i = 0; i < 5000/2; i ++);
}


MPU_Data_t MPU6050_Data;

int main(void)
{

	(*((volatile uint32_t*)0xE000EDFC)) |= (1 << 24);

	GPIO_Handle_t GPIO;
	memset(&GPIO, 0, sizeof(GPIO));
	GPIO.pGPIOx = GPIOB;
	GPIO.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_No_6;
	GPIO.GPIO_PinConfig.GPIO_PinMode = GPIO_Mod_ALTFN;
	GPIO.GPIO_PinConfig.GPIO_PinAltFunMode = GPIO_ALTFN_I2C_1_3;
	GPIO.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_HIGH;
	GPIO.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;
	GPIO.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PU;



	GPIO_PeriClkCon(GPIOB, SET);
	GPIO_Init(&GPIO);

	GPIO.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_No_7;
	GPIO_Init(&GPIO);


    // Handle structs to hold configuration and data
    I2C_Handle_t I2C1Handle;
    memset(&I2C1Handle, 0, sizeof(I2C1Handle));

    I2C1Handle.pI2Cx = I2C1;
    I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;
    I2C1Handle.I2C_Config.I2C_DeviceAddress = 0x61;
    I2C1Handle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
    I2C1Handle.I2C_Config.I2CFMDutyCycle = 0;

    I2C_PeriClkCon(I2C1, ENABLE);

    I2C_Init(&I2C1Handle);

    I2C_PeripheralControl(I2C1, ENABLE);

    MPU_Init(&I2C1Handle);


    while(1)
    {
        // Read the Accelerometer Data
        MPU_Read_AccData(&I2C1Handle, &MPU6050_Data);

        printf("Accel X: %f | Accel Y: %f | Accel Z: %f\r\n",
        		(float)MPU6050_Data.AX, (float)MPU6050_Data.AY, (float)MPU6050_Data.AZ);

        delay();
    }

    return 0;
}
