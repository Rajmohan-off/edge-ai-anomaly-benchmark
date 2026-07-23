/*
 * MPU6050_drv.c
 *
 *  Created on: 05-Jul-2026
 *      Author: rajmo
 */

#include "MPU6050_drv.h"
#include "stm32f407xx_drv_I2C.h"



// MPU6050 Init
/*
* @fn        - MPU_Init
* @brief     - Functionality to initialize MPU6050
*
* @Param_1   - Peripheral handles of MPU6050
*
* @return    - none
*
* @note      - none
*
*/
void MPU_Init(I2C_Handle_t *pI2C_Handle)
{
	uint8_t TxBuffer[2];

	//wakeup MPU 6050 from sleep mode by reset PWRMGMT_1 register
	TxBuffer[0] = MPU_PWRMGMT_1;
	TxBuffer[1] = 0x00;
	I2C_MasterSendData(pI2C_Handle, TxBuffer, 2, MPU_BASEADDR);

	//SET 1Khz to SMPLRT_div
	TxBuffer[0] = MPU_SMPLRT_DIV;
	TxBuffer[1] = 0x07;
	I2C_MasterSendData(pI2C_Handle, TxBuffer, 2, MPU_BASEADDR);

	//SET 0x00 to GYRO Config, operates in 250 degree/sec
	TxBuffer[0] = MPU_GYROCONFIG;
	TxBuffer[1] = 0x00;
	I2C_MasterSendData(pI2C_Handle, TxBuffer, 2, MPU_BASEADDR);

	//SET 0x00 to Accelero Config, operates in the range of +/-2g
	TxBuffer[0] = MPU_ACCELCONFIG;
	TxBuffer[1] = 0x00;
	I2C_MasterSendData(pI2C_Handle, TxBuffer, 2, MPU_BASEADDR);
}


/*
* @fn        - MPU_Read_AccData
* @brief     - Functionality to read accelerometer data from MPU6050
*
* @Param_1   - Peripheral handles of MPU6050
* @param_2   - Data from MPU6050
*
* @return    - none
*
* @note      - none
*
*/
void MPU_Read_AccData(I2C_Handle_t *pI2C_Handle, MPU_Data_t *pMPU_data)
{
	uint8_t TxBuffer = MPU_ACCELXOUT_H;
	I2C_MasterSendData(pI2C_Handle, &TxBuffer, 1, MPU_BASEADDR);

	uint8_t RxBuffer[6];
	I2C_MasterReceiveData(pI2C_Handle, RxBuffer, 6, MPU_BASEADDR);

	pMPU_data->Accel_X_RAW = (uint16_t)(RxBuffer[0] << 8 | RxBuffer[1]);
	pMPU_data->Accel_Y_RAW = (uint16_t)(RxBuffer[2] << 8 | RxBuffer[3]);
	pMPU_data->Accel_Z_RAW = (uint16_t)(RxBuffer[4] << 8 | RxBuffer[5]);

	pMPU_data->AX = pMPU_data->Accel_X_RAW / 16384.0;   // LSB sensitivity of constant value for +/- 2g range
	pMPU_data->AY = pMPU_data->Accel_Y_RAW / 16384.0;
	pMPU_data->AZ = pMPU_data->Accel_Z_RAW / 16384.0;
}


/*
* @fn        - MPU_Read_GyroData
* @brief     - Functionality to read gyroscope data from MPU6050
*
* @Param_1   - Peripheral handles of MPU6050
* @param_2   - Data from MPU6050
*
* @return    - none
*
* @note      - none
*
*/
void MPU_Read_GyroData(I2C_Handle_t *pI2C_Handle, MPU_Data_t *pMPU_data)
{
	uint8_t TxBuffer = MPU_GYROXOUT_H;
	I2C_MasterSendData(pI2C_Handle, &TxBuffer, 1, MPU_BASEADDR);

	uint8_t RxBuffer[6];
	I2C_MasterReceiveData(pI2C_Handle, RxBuffer, 6, MPU_BASEADDR);

	pMPU_data->Gyro_X_RAW = (uint16_t)(RxBuffer[0] << 8 | RxBuffer[1]);
	pMPU_data->Gyro_Y_RAW = (uint16_t)(RxBuffer[2] << 8 | RxBuffer[3]);
	pMPU_data->Gyro_Z_RAW = (uint16_t)(RxBuffer[4] << 8 | RxBuffer[5]);

	pMPU_data->GX = pMPU_data->Gyro_X_RAW / 131.0;   //LSB sensitivity of constant value for 250 deg/sec range
	pMPU_data->GY = pMPU_data->Gyro_Y_RAW / 131.0;
	pMPU_data->GZ = pMPU_data->Gyro_Z_RAW / 131.0;
}



