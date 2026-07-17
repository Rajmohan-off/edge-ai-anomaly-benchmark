/*
 * MPU6050_drv.h
 *
 *  Created on: 04-Jul-2026
 *      Author: rajmo
 */

#ifndef INC_MPU6050_DRV_H_
#define INC_MPU6050_DRV_H_

#include "stm32f407xx.h"
#include "stm32f407xx_drv_I2C.h"

//Baseaddr of MPU6050 0x68
#define MPU_BASEADDR                         0x68


//Configs
#define MPU_SMPLRT_DIV                       0x19
#define MPU_CONFIG                           0X1A
#define MPU_GYROCONFIG                       0X1B
#define MPU_ACCELCONFIG                      0X1C
#define MPU_ACCELXOUT_H                      0x3B
#define MPU_TEMPOUT_H                        0x41
#define MPU_GYROXOUT_H                       0x43
#define MPU_PWRMGMT_1                        0x6B
#define MPU_WHO_AM_I                         0x75


//Struct of the peripherals
typedef struct
{
	int16_t Accel_X_RAW;
    int16_t Accel_Y_RAW;
	int16_t Accel_Z_RAW;

	int16_t Gyro_X_RAW;
	int16_t Gyro_Y_RAW;
	int16_t Gyro_Z_RAW;


	float AX;
	float AY;
	float AZ;
	float GX;
	float GY;
	float GZ;

}MPU_Data_t;

//API Prototypes
void MPU_Init(I2C_Handle_t *pI2C_Handle);
void MPU_Read_AccData(I2C_Handle_t *pI2C_Handle, MPU_Data_t *pMPU_data);
void MPU_Read_GyroData(I2C_Handle_t *pI2C_Handle, MPU_Data_t *pMPU_data);


#endif /* INC_MPU6050_DRV_H_ */
