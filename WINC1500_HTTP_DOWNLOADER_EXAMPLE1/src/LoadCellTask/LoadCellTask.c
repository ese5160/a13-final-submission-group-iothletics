/*
 * LoadCellTask.c
 *
 * Created: 5/3/2023 10:18:51 PM
 *  Author: User
 */ 

#include "LoadCellTask/LoadCellTask.h"
#include "NAU7802/NAU7802.h"
#include "SerialConsole/SerialConsole.h"
#include "CliThread/CliThread.h"
#include "WifiHandlerThread/WifiHandler.h"

char WriteBuffer[100];

void vLoadCellTask( void *pvParameters )
{
	uint32_t pressure;
	while(1)
	{
		NAUbegin();
		WifiAddLoadDataToQueue(&pressure);
		vTaskDelay(100);
	}
}