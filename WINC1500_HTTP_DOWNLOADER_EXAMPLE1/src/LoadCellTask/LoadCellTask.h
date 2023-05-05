/*
 * LoadCellTask.h
 *
 * Created: 5/3/2023 10:19:08 PM
 *  Author: User
 */ 


#ifndef LOADCELLTASK_H_
#define LOADCELLTASK_H_

#define LOADCELL_TASK_SIZE	400
#define LOADCELL_TASK_PRIORITY (configMAX_PRIORITIES - 1)
void vLoadCellTask( void *pvParameters );

#include "I2cDriver/I2cDriver.h"
#include "NAU7802/NAU7802.h"



#endif /* LOADCELLTASK_H_ */