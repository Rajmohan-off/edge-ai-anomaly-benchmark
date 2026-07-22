/*
 * stm32f407xxI2C_drv.h
 *
 *  Created on: 29-Jun-2026
 *      Author: rajmo
 */

#ifndef INC_STM32F407XX_DRV_I2C_H_
#define INC_STM32F407XX_DRV_I2C_H_

#include "stm32f407xx.h"


//config I2C peripherals
typedef struct{
	uint32_t I2C_SCLSpeed;
	uint8_t  I2C_DeviceAddress;
	uint8_t  I2C_ACKControl;
	uint16_t I2CFMDutyCycle;

}I2C_Config_t;


//config handle
typedef struct{
	I2C_RegDef_t  *pI2Cx;
	I2C_Config_t  I2C_Config;

}I2C_Handle_t;


//declare the speed modes
#define I2C_SCL_SPEED_SM        100000    //STANDARD MODE
#define I2C_SCL_SPEED_FM_2K     200000    //CUSTOMIZED MODE
#define I2C_SCL_SPEED_FM_4K     400000    //FAST MODE


//Declare the acknowledge modes
#define I2C_ACK_ENABLE          1
#define I2C_ACK_DISABLE         0


//Declare the duty cycles mode
#define I2C_FM_2                0
#define I2C_FM_16_9             1


//I2C Peripheral clock declare
void I2C_PeriClkCon(I2C_RegDef_t *pI2Cx, uint8_t EnorDi);


//Init & De-init
void I2C_Init(I2C_Handle_t *pI2C_Handle);
void I2C_DeInit(I2C_RegDef_t *pI2Cx);


// IRQ pin and priority Config
void I2C_IRQpinConfig(uint8_t IRQNumber, uint8_t EnorDi);
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint8_t IRQPriority);


// other peripherals control
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi);
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName);
void I2C_Manage_ACK (I2C_RegDef_t *pI2Cx, uint8_t EnorDi);

//API for send data from master
void I2C_MasterSendData(I2C_Handle_t *pI2C_Handle, uint8_t *pTxBuffer, uint32_t len, uint8_t slaveaddr);
void I2C_MasterReceiveData(I2C_Handle_t *pI2C_Handle, uint8_t *pRxBuffer, uint32_t len, uint8_t slaveaddr);

//Macros for flag
#define I2C_FLAG_SB               (1 << I2C_SR1_SB)
#define I2C_FLAG_ADDR             (1 << I2C_SR1_ADDR)
#define I2C_FLAG_BTF              (1 << I2C_SR1_BTF)
#define I2C_FLAG_SB               (1 << I2C_SR1_SB)
#define I2C_FLAG_STOPF            (1 << I2C_SR1_STOPF)
#define I2C_FLAG_RxNE             (1 << I2C_SR1_RXNE)
#define I2C_FLAG_TxE              (1 << I2C_SR1_TXE)
#define I2C_FLAG_BERR             (1 << I2C_SR1_BERR)
#define I2C_FLAG_ARLO             (1 << I2C_SR1_ARLO)
#define I2C_FLAG_AF               (1 << I2C_SR1_AF)
#define I2C_FLAG_OVR              (1 << I2C_SR1_OVR)
#define I2C_FLAG_PECERR           (1 << I2C_SR1_PECERR)
#define I2C_FLAG_TIMEOUT          (1 << I2C_SR1_TIMEOUT
#define I2C_FLAG_SMBALERT         (1 << I2C_SR1_SMBALERT)


//application callback declaration
void I2C_ApplicationEventCallBack(I2C_Handle_t *pI2C_Handle);



#endif /* INC_STM32F407XX_DRV_I2C_H_ */
