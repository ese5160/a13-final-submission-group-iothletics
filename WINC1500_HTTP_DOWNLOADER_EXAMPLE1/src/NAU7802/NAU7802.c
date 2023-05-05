/**************************************************************************/ /**
 * @file      Nau7802.c
 * @brief     Driver for the Nau7802

 
 * @date      2023-04-18
 *  Author: Arnav Gadre

 ******************************************************************************/

#include "I2cDriver/I2cDriver.h"
#include "NAU7802/NAU7802.h"
#include "SerialConsole.h"
#include "asf.h"
#include "CliThread/CliThread.h"
#include "WifiHandlerThread/WifiHandler.h"


uint8_t msgOutLoad[64]; //<USE ME AS A BUFFER FOR platform_write and platform_read
I2C_Data LoadData;

I2C_Data cellData;  ///< Global variable to use for I2C communications with the Load Cell Device
char bufCli[32];

int NAUbegin(){
    
	msgOutLoad[0] = 0x00;
	msgOutLoad[1] = 0x01;

	LoadData.address = 0x2A;
	LoadData.msgOut = &msgOutLoad;
	LoadData.lenOut = 2;
	LoadData.lenIn = 0;

	uint32_t rst = I2cWriteDataWait(&LoadData, 100);
	
	vTaskDelay(10);
	
	msgOutLoad[0] = 0x00;
	msgOutLoad[1] = 0x02;

	LoadData.address = 0x2A;
	LoadData.msgOut = &msgOutLoad;
	LoadData.lenOut = 2;
	LoadData.lenIn = 0;

	uint32_t powerup = I2cWriteDataWait(&LoadData, 100);
	
	vTaskDelay(50);
	
	uint8_t buffer[1];
	msgOutLoad[0] = 0x00;

	LoadData.address = 0x2A;
	LoadData.msgIn = buffer;
	LoadData.lenIn = sizeof(buffer);
	LoadData.msgOut = &msgOutLoad;
	LoadData.lenOut = 1;

	uint32_t read_error = I2cReadDataWait(&LoadData, 50, 100);
	vTaskDelay(10);

	
	msgOutLoad[0] = 0x01;
	msgOutLoad[1] = 0x27;

	LoadData.address = 0x2A;
	LoadData.msgOut = &msgOutLoad;
	LoadData.lenOut = 2;
	LoadData.lenIn = 0;

	uint32_t ldo_gain = I2cWriteDataWait(&LoadData, 100);
	
	vTaskDelay(1);
	
	msgOutLoad[0] = 0x00;
	msgOutLoad[1] = 0x96;

	LoadData.address = 0x2A;
	LoadData.msgOut = &msgOutLoad;
	LoadData.lenOut = 2;
	LoadData.lenIn = 0;

	uint32_t pdpa = I2cWriteDataWait(&LoadData, 100);
	
	vTaskDelay(1);
	
	msgOutLoad[0] = 0x15;
	msgOutLoad[1] = 0x30;

	LoadData.address = 0x2A;
	LoadData.msgOut = &msgOutLoad;
	LoadData.lenOut = 2;
	LoadData.lenIn = 0;

	uint32_t chopper_off = I2cWriteDataWait(&LoadData, 100);
	
	
	uint8_t buffer1[1];
	
	msgOutLoad[0] = 0x00;

	LoadData.address = 0x2A;
	LoadData.msgIn = buffer1;
	LoadData.lenIn = sizeof(buffer1);
	LoadData.msgOut = &msgOutLoad;
	LoadData.lenOut = 1;

	read_error = I2cReadDataWait(&LoadData, 50, 100);
	vTaskDelay(10);
	
	uint8_t bit_position1 = 5; // ???3???0???
	uint8_t mask1 = 1 << bit_position1; // ????????1??bit_position?
	uint8_t result1 = buffer1[0] & mask1; // ??????????????1
	uint8_t data_buffer[3];
	
	vTaskDelay(10);
	
	msgOutLoad[0] = 0x12;

	LoadData.address = 0x2A;
	LoadData.msgIn = data_buffer;
	LoadData.lenIn = sizeof(data_buffer);
	LoadData.msgOut = &msgOutLoad;
	LoadData.lenOut = 1;

	read_error = I2cReadDataWait(&LoadData, 50, 100);
	
	vTaskDelay(10);
	
	uint32_t pressure = (data_buffer[0]<<16) | (data_buffer[1]<<8) | (data_buffer[2]);

	snprintf((char *) bufCli, sizeof(bufCli), "Foot Pressure: %d", (int32_t)pressure);
	//SerialConsoleWriteString(bufCli);
	//SerialConsoleWriteString("\r\n");
	
	WifiAddLoadDataToQueue(pressure);

}


