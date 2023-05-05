/*
 * BNO085.c
 *
 * Created: 4/27/2023 4:20:47 PM
 *  Author: User
 */ 

#include "BNO085\BNO085.h"
#include "I2cdriver\I2cDriver.h"
#include "CliThread\CliThread.h"
#include "stdint.h"


/**************  GLOBAL VARIABLES  ***************/

uint8_t msgOutBNO[64];		//<USE ME AS A BUFFER FOR platform_write and platform_read
I2C_Data BNOData;
// SFE_HMD_DRV2605L HMD;			//Create Haptic Motor driver object
#define HAPTIC_ON 4096			// Define the value for Haptic On state
#define HAPTIC_OFF 0			// Define value for Haptic Off state



/**************  FUNCTIONS  *************/

void BNObegin(void)
{
	uint8_t bufp[2];
	uint16_t len, reg = 0xAA;
	char bufCli[64];
	
	// Wire.begin();
	//Get a read from the status register
	//Want this to Read 0xE0, any other value than 0 and you have tripped the over-current protection=wrong moto
	
	msgOutBNO[0] = reg;
	BNOData.address = 0x4B;
	BNOData.msgIn = bufp;
	BNOData.lenIn = sizeof(bufp);
	BNOData.lenOut = 1;
	BNOData.msgOut = &reg;
	
	uint16_t error = I2cReadDataWait(&BNOData, 0xff, 0xff);
	snprintf(bufCli, sizeof(bufCli), "ID : %d", bufp[0]);
	SerialConsoleWriteString(bufCli);
	
	// SerialConsoleWriteString("Status Register 0x:");
	// SerialConsoleWriteString(error);
	SerialConsoleWriteString("\r\n");
	
	return error;
}


void BNOwrite(void)
{
	
	
	
}
