/*
 * IMUTask.h
 *
 * Created: 5/3/2023 10:17:44 PM
 *  Author: User
 */ 


#ifndef IMUTASK_H_
#define IMUTASK_H_

#define IMU_TASK_SIZE	400
#define IMU_TASK_PRIORITY (configMAX_PRIORITIES - 1)
void vIMUTask( void *pvParameters );

#include "I2cDriver/I2cDriver.h"
#include "IMU/lsm6dso_reg.h"




#endif /* IMUTASK_H_ */