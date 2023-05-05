/*
 * Haptic.c
 *
 * Created: 4/24/2023 9:10:12 PM
 *  Author: Arnav Gadre
			
 */ 

/*  The DRV2605L library code has been included in this file itself. */

/* #if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif */

// #include "Wire.h"
#include "Haptic_Motor_Driver\Haptic.h"
#include "I2cdriver\I2cDriver.h"
#include "CliThread\CliThread.h"
#include "stdint.h"
#include "SerialConsole.h"
#include "WifiHandlerThread/WifiHandler.h"


/***********************  Global Variable Declarations  ********************/


uint8_t msgOutHaptic[64];		//<USE ME AS A BUFFER FOR platform_write and platform_read
I2C_Data hapticData;
// SFE_HMD_DRV2605L HMD;			//Create Haptic Motor driver object 
#define HAPTIC_ON 4096			// Define the value for Haptic On state
#define HAPTIC_OFF 0			// Define value for Haptic Off state


/* SFE_HMD_DRV2605L SFE_HMD_DRV2605L()
{

} */


int Haptic_Begin(void)
{
	uint8_t bufp[2];
	uint16_t len, reg = 0x00;
	char bufCli[64];
	
	// Wire.begin();
	//Get a read from the status register
	//Want this to Read 0xE0, any other value than 0 and you have tripped the over-current protection=wrong moto
	
	msgOutHaptic[0] = reg;
	hapticData.address = 0x5A;
	hapticData.msgIn = bufp;
	hapticData.msgOut = &reg;
	hapticData.lenIn = sizeof(bufp);
	hapticData.lenOut = 1;
	
	uint16_t error = I2cReadDataWait(&hapticData, 0xff, 0xff);
	
	snprintf((char *) bufCli, sizeof(bufCli), "Begin Status : %d", bufp[0]);
	SerialConsoleWriteString(bufCli);
	SerialConsoleWriteString("\r\n");
	
	return error;
}

int Haptic_Mode(void)
{
	uint16_t len, reg;
	char bufCli[64];
	uint8_t bufp[2];
	
	msgOutHaptic[0] = 0x01;
	msgOutHaptic[1] = 0x03;

	
	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
	hapticData.msgIn = bufp;
	int error = I2cWriteDataWait(&hapticData, 0xff);
	snprintf(bufCli, sizeof(bufCli), "MODE Status: %d", error);
	SerialConsoleWriteString(bufCli);
	SerialConsoleWriteString("\r\n");
	
	vTaskDelay(500);
	
	msgOutHaptic[0] = 0x01;
	msgOutHaptic[1] = 0x00;

	
	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
	hapticData.msgIn = bufp;
	error = I2cWriteDataWait(&hapticData, 0xff);
	
	vTaskDelay(100);
	
	//WifiAddHapticDataToQueue(error);
	
	/* msgOutHaptic[0] = 0x02;
	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
	hapticData.msgIn = bufp;
	error = I2cWriteDataWait(&hapticData, 0xff);
	
	msgOutHaptic[1] = 1;
	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
	hapticData.msgIn = bufp;
	error = I2cWriteDataWait(&hapticData, 0xff);
	
	msgOutHaptic[0] = 0x05;
	msgOutHaptic[1] = 0;
	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
	hapticData.msgIn = bufp;
	 error = I2cWriteDataWait(&hapticData, 0xff);
	
	msgOutHaptic[0] = 0x0D;
	msgOutHaptic[1] = 0x00;
	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
	hapticData.msgIn = bufp;
	error = I2cWriteDataWait(&hapticData, 0xff);
	
	msgOutHaptic[0] = 0x0E;
	msgOutHaptic[1] = 0x00;
	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
	hapticData.msgIn = bufp;
	error = I2cWriteDataWait(&hapticData, 0xff);
	
	msgOutHaptic[0] = 0x0F;
	msgOutHaptic[1] = 0x00;
	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
	hapticData.msgIn = bufp;
	error = I2cWriteDataWait(&hapticData, 0xff);
	
	msgOutHaptic[0] = 0x10;
	msgOutHaptic[1] = 0x00;
	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
	hapticData.msgIn = bufp;
	error = I2cWriteDataWait(&hapticData, 0xff);
	
	msgOutHaptic[0] = 0x13;
	msgOutHaptic[1] = 0x64;
	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
	hapticData.msgIn = bufp;
	error = I2cWriteDataWait(&hapticData, 0xff);
	
	reg = 0x1A;
	hapticData.address = 0x5A;
	hapticData.msgOut = &reg;
	hapticData.lenOut = 1;
	hapticData.lenIn = sizeof(bufp);
	hapticData.msgIn = bufp;
	error = I2cReadDataWait(&hapticData, 0xff, 0xff);
	
	msgOutHaptic[0] = 0x1A;
	msgOutHaptic[1] = bufp[0] & 0x7F;
	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
	hapticData.msgIn = bufp;
	error = I2cWriteDataWait(&hapticData, 0xff);
	
	reg = 0x1D;
	hapticData.address = 0x5A;
	hapticData.msgOut = &reg;
	hapticData.lenOut = 1;
	hapticData.lenIn = sizeof(bufp);
	hapticData.msgIn = bufp;
	error = I2cReadDataWait(&hapticData, 0xff, 0xff);
	
	msgOutHaptic[0] = 0x1D;
	msgOutHaptic[1] = bufp[0] | 0x20;
	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
	hapticData.msgIn = bufp;
	error = I2cWriteDataWait(&hapticData, 0xff);
	
	msgOutHaptic[0] = 0x01;
	msgOutHaptic[1] = 0x00;
	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
	hapticData.msgIn = bufp;
	error = I2cWriteDataWait(&hapticData, 0xff);
	
	msgOutHaptic[0] = 0x04;
	msgOutHaptic[1] = 0x01;
	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
	hapticData.msgIn = bufp;
	error = I2cWriteDataWait(&hapticData, 0xff);
	
	msgOutHaptic[0] = 0x05;
	msgOutHaptic[1] = 0x00;
	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
	hapticData.msgIn = bufp;
	error = I2cWriteDataWait(&hapticData, 0xff); */
	
	return error;

}


//Select ERM or LRA
//Set Motor Type Using the Feedback Control Register
//Set ERM or LRA
//Example: 0X39 0011 1001: ERM Mode, 4x Brake factor, Medium Gain, 1.365x Back EMF
//Example: 0xB9 1011 1001: LRA MODE, 4X Brake Factor, Medium Gain, 7.5x Back EMF

int Haptic_MotorSelect(void)
{
	uint8_t val, bufp[2];
	uint16_t len;
	char bufCli[64];
	// uint8_t *bufp;
	
	msgOutHaptic[0] = 0x1A;
	msgOutHaptic[1] = 0x0A;

	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
 	hapticData.msgIn = bufp;
	
	uint16_t error = I2cWriteDataWait(&hapticData, 0xff);
	return error;
	
	/* snprintf(bufCli, sizeof(bufCli), "MOTOR SELECT : %d", error);
	SerialConsoleWriteString(bufCli);
	SerialConsoleWriteString("\r\n"); */
	
	//uint8_t motor = readDRV2605L(FEEDBACK_REG);
	//Serial.print("Feedback Register/Motor Selector 0x"); Serial.println(motor, HEX);
}

//Select Library
int Haptic_Library(void)
{
	
	uint8_t lib, bufp[2]; 
	uint16_t len;
	uint8_t bufCli[64];
	
	msgOutHaptic[0] = 0x03;
	msgOutHaptic[1] = 0x07;

	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
	hapticData.msgIn = bufp;
	
	uint16_t error = I2cWriteDataWait(&hapticData, 0xff);
	return error;
	
	/* snprintf(bufCli, sizeof(bufCli), "LIBRARY : %d", error);
	SerialConsoleWriteString(bufCli);
	SerialConsoleWriteString("\r\n"); */
	// writeDRV2605L(LIB_REG, lib);
}


//Select waveform from list of waveform library effects
//data sheet page 60. This function selects the sequencer
//and the effects from the library.
int Haptic_Waveform(void)
{
	uint8_t seq, wav, bufp[2]; 
	uint16_t len;
	char bufCli[64];
	
	msgOutHaptic[0] = 0x04;
	msgOutHaptic[1] = 0x00;
	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
	hapticData.msgIn = bufp;
	
	uint16_t error = I2cWriteDataWait(&hapticData, 0xff);
	return error;
	
	/* snprintf(bufCli, sizeof(bufCli), "WAVEFORM : %d", error);
	SerialConsoleWriteString(bufCli);
	SerialConsoleWriteString("\r\n"); */
	// writeDRV2605L(WAVESEQ1+seq, wav);
}


int Haptic_Vibrations(void)
{
	uint8_t bufp[2];
	uint16_t len;
	char bufCli[64];
	
	msgOutHaptic[0] = 0x0B;
	msgOutHaptic[1] = 0x80;
	msgOutHaptic[2] = 0xFF;
	msgOutHaptic[3] = 0x60;
	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 4;
	hapticData.lenIn = 0;
	hapticData.msgIn = bufp;
	
	uint16_t error = I2cWriteDataWait(&hapticData, 0xff);
	return error;
	
}

//Go command
int Haptic_Go(int state)
{
	uint16_t len; 
	uint8_t bufp;
	char bufCli[64];
	
	msgOutHaptic[0] = 0x0C;
	msgOutHaptic[1] = state;
	
	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
	// hapticData.msgIn = bufp;
	
	uint16_t error = I2cWriteDataWait(&hapticData, 0xff);
	
	/* snprintf(bufCli, sizeof(bufCli), "GO : %d", error);
	SerialConsoleWriteString(bufCli);
	SerialConsoleWriteString("\r\n"); */
	
	return error;
	
	// writeDRV2605L(GO_REG, 1);
}


/* void Stop(void)
{
	uint16_t len;
	uint8_t bufp;
	char bufCli[64];
	
	msgOutHaptic[0] = 0x0C;
	msgOutHaptic[1] = 0x00;
	
	hapticData.address = 0x5A;
	hapticData.msgOut = &msgOutHaptic;
	hapticData.lenOut = 2;
	hapticData.lenIn = 0;
	// hapticData.msgIn = bufp;
	
	uint16_t error = I2cWriteDataWait(&hapticData, 0xff);
	snprintf(bufCli, sizeof(bufCli), "GO : %d", error);
	SerialConsoleWriteString(bufCli);
	SerialConsoleWriteString("\r\n");
	
	return error;
	
	// writeDRV2605L(GO_REG, 1);
}
*/

/* void Setup(void)
{
	begin();
	// Serial.begin(9600);
	Mode(); // Internal trigger input mode -- Must use the GO() function to trigger playback.
	MotorSelect(); // ERM motor, 4x Braking, Medium loop gain, 1.365x back EMF gain
	Library(); //1-5 & 7 for ERM motors, 6 for LRA motors
} */


int Haptic_Drive(void)
{
	 int error;
	 for (int i=0; i<2000; i++){
		 
		 error = Haptic_Go(0x01);
	 }
	 return error;
}




