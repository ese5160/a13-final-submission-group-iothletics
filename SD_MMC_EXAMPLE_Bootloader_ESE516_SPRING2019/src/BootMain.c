
/**************************************************************************//**
* @file      BootMain.c
* @brief     Main file for the ESE516 bootloader. Handles updating the main application
* @details   Main file for the ESE516 bootloader. Handles updating the main application
* @author    Eduardo Garcia
* @date      2020-02-15
* @version   2.0
* @copyright Copyright University of Pennsylvania
******************************************************************************/


/******************************************************************************
* Includes
******************************************************************************/
#include <asf.h>
#include "conf_example.h"
#include <string.h>
#include "sd_mmc_spi.h"

#include "SD Card/SdCard.h"
#include "Systick/Systick.h"
#include "SerialConsole/SerialConsole.h"
#include "ASF/sam0/drivers/dsu/crc32/crc32.h"





/******************************************************************************
* Defines
******************************************************************************/
#define APP_START_ADDRESS  ((uint32_t)0x12000) ///<Start of main application. Must be address of start of main application
#define APP_START_RESET_VEC_ADDRESS (APP_START_ADDRESS+(uint32_t)0x04) ///< Main application reset vector address
//#define MEM_EXAMPLE 1 //COMMENT ME TO REMOVE THE MEMORY WRITE EXAMPLE BELOW

/******************************************************************************
* Structures and Enumerations
******************************************************************************/

struct usart_module cdc_uart_module; ///< Structure for UART module connected to EDBG (used for unit test output)

/******************************************************************************
* Local Function Declaration
******************************************************************************/
static void jumpToApplication(void);
static bool StartFilesystemAndTest(void);
static void configure_nvm(void);


/******************************************************************************
* Global Variables
******************************************************************************/
//INITIALIZE VARIABLES
char test_file_name[] = "0:sd_mmc_test.txt";	///<Test TEXT File name
char test_bin_file[] = "0:sd_binary.bin";	///<Test BINARY File name
char test_bin_TestA[] = "0:ESE516_MAIN_FW.bin";	///<Test TEXT File name
char test_bin_TestB[] = "0:TestB.bin";	///<Test BINARY File name
char test_file_flagA[] = "0:FlagA.txt";
char test_file_flagB[] = "0:FlagB.txt";
int A_state = 1;
Ctrl_status status; ///<Holds the status of a system initialization
FRESULT res, res1, res2, res3, res4; //Holds the result of the FATFS functions done on the SD CARD TEST
FATFS fs; //Holds the File System of the SD CARD
FIL file_object; //FILE OBJECT used on main for the SD Card Test



/******************************************************************************
* Global Functions
******************************************************************************/

/**************************************************************************//**
* @fn		int main(void)
* @brief	Main function for ESE516 Bootloader Application
* @return	Unused (ANSI-C compatibility).
* @note		Bootloader code initiates here.
*****************************************************************************/

int main(void)
{

	/*1.) INIT SYSTEM PERIPHERALS INITIALIZATION*/
	system_init();
	delay_init();
	InitializeSerialConsole();
	system_interrupt_enable_global();
	/* Initialize SD MMC stack */
	sd_mmc_init();

	//Initialize the NVM driver
	configure_nvm();

	irq_initialize_vectors();
	cpu_irq_enable();

	//Configure CRC32
	dsu_crc32_init();
	uint32_t numBytesRead = 0;
	uint8_t readBuffer[256]; //Buffer the size of one row

	SerialConsoleWriteString("ESE516 - ENTER BOOTLOADER");	//Order to add string to TX Buffer

	/*END SYSTEM PERIPHERALS INITIALIZATION*/


	/*2.) STARTS SIMPLE SD CARD MOUNTING AND TEST!*/

	//EXAMPLE CODE ON MOUNTING THE SD CARD AND WRITING TO A FILE
	//See function inside to see how to open a file
	SerialConsoleWriteString("\x0C\n\r-- SD/MMC Card Example on FatFs --\n\r");

	if(StartFilesystemAndTest() == false)
	{
		SerialConsoleWriteString("SD CARD failed! Check your connections. System will restart in 5 seconds...");
		delay_cycles_ms(5000);
		system_reset();
	}
	else
	{
		SerialConsoleWriteString("SD CARD mount success! Filesystem also mounted. \r\n");
		
	}

	/*END SIMPLE SD CARD MOUNTING AND TEST!*/


	/*3.) STARTS BOOTLOADER HERE!*/
	
	test_file_flagB[0] = LUN_ID_SD_MMC_0_MEM + '0';
	res1 = f_open(&file_object, (char const *)test_file_flagB, FA_READ);
	test_file_flagA[0] = LUN_ID_SD_MMC_0_MEM + '0';
	res2 = f_open(&file_object, (char const *)test_file_flagA, FA_READ);

	if((port_pin_get_input_level(BUTTON_0_PIN)==BUTTON_0_ACTIVE) && res1 == FR_OK){					// Check if button is being pressed or not
												
			f_unlink((char const *)test_file_flagB);											// If TestB flag exists, unlink the flag for TestB
			SerialConsoleWriteString("0");														// Print '0'on the serial output.
			A_state = 0;																	// Change the state flag 'A_state' to False.

	}
		
	else if((port_pin_get_input_level(BUTTON_0_PIN)==BUTTON_0_ACTIVE) && res1 == FR_OK){
			
				f_unlink((char const *)test_file_flagA);							// Unlink the flag for TestA
				SerialConsoleWriteString("1");										// Print '1' on the serial output.
				A_state = 1;														// Change the state flag 'A_state' to True.
			}
			
	else{
		
		//jumpToApplication();
		SerialConsoleWriteString("Default \r\n");
		// A_state = 2;
		
	test_bin_TestA[0] = LUN_ID_SD_MMC_0_MEM + '0';
	res3 = f_open(&file_object, (char const *)test_bin_TestA, FA_READ | FA_OPEN_EXISTING);
	test_bin_TestB[0] = LUN_ID_SD_MMC_0_MEM + '0';
	res4 = f_open(&file_object, (char const *)test_bin_TestB, FA_READ | FA_OPEN_EXISTING);
	
		
		if (res3 == FR_OK) {
			A_state = 1;
		}
		
		else if (res4 == FR_OK){
			A_state = 0;
		}
		
		else {
			A_state = 2;
		}
		
		
		
	}
	


/**********************************************************
	When contents of 'TestA' file are to be executed 
**********************************************************/

if(A_state == 1){
	
// We need to fill a memory of 32KB. Each operation iterates 256 bytes of memory. 
// So we run the entire operation of 128 times.
// So, 256 bytes x 128 cycles gives 32,768 bytes.

for(int i=0; i<644; i++){
		
	// Iterate address by 256 times the i'th iteration.
		
	enum status_code nvmError = nvm_erase_row(APP_START_ADDRESS + (i*256));
	if(nvmError != STATUS_OK)
	{
		SerialConsoleWriteString("Erase error");
	}
	
	//Make sure it got erased - we read the page. Erasure in NVM is an 0xFF
	for(int iter = 0; iter < 256; iter++)
	{
		// Iterate address by 256 times the i th iteration.
		char *a = (char *)(APP_START_ADDRESS + iter + (256*i)); //Pointer pointing to address APP_START_ADDRESS
		if(*a != 0xFF)
		{
			SerialConsoleWriteString("Error - test page is not erased!");
			break;	
		}
	}	

	//Read SD Card File
	test_bin_TestA[0] = LUN_ID_SD_MMC_0_MEM + '0';
	res = f_open(&file_object, (char const *)test_bin_TestA, FA_READ);   // Check whether the TestA file exists
		
	if (res != FR_OK)
	{
		SerialConsoleWriteString("Could not open test file!\r\n");       // If it does not exist, throw an error message.
	}

	int numBytesLeft = 256;                                              // Number of bytes left.
	numBytesRead = 0;													 // Number of bytes read.			
	int numberBytesTotal = 0;											 // Total number of bytes
	res = f_lseek(&file_object, 256*i);
	while(numBytesLeft  != 0) 
	{
		
		res = f_read(&file_object, &readBuffer[numberBytesTotal], numBytesLeft, &numBytesRead); //The 'numBytesRead' helps us get a byte array from a FileStream. It tells us the number of bytes that have been read in that particular iteration of the ?f_read? function.
		numBytesLeft -= numBytesLeft;															// Update numBytesLeft	
		numberBytesTotal += numBytesRead;														// Update numBytesRead
	}
	


	//Write data to first row. Writes are per page, so we need four writes to write a complete row. The data will iterate in sets of 256 bytes.
	res = nvm_write_buffer (APP_START_ADDRESS + (i*256), &readBuffer[0], 64);
	res = nvm_write_buffer (APP_START_ADDRESS + 64 + (i*256), &readBuffer[64], 64);
	res = nvm_write_buffer (APP_START_ADDRESS + 128 + (i*256), &readBuffer[128], 64);
	res = nvm_write_buffer (APP_START_ADDRESS + 192 + (i*256), &readBuffer[192], 64);

		if (res != FR_OK)
		{
			SerialConsoleWriteString("Test write to NVM failed!\r\n");
		}
		else
		{
			//SerialConsoleWriteString("Test write to NVM succeeded!\r\n");
		}
	}
}
	
/**********************************************************
When contents of 'TestB' file are to be executed
**********************************************************/
	
/*if (A_state == 0){
		
// We need to fill a memory of 32KB. Each operation iterates 256 bytes of memory.
// So we run the entire operation of 128 times.
// So, 256 bytes x 128 cycles gives 32,768 bytes.
		
	for(int i=0; i<128; i++){
	
	// Iterate address by 256 times the i th iteration.
	
	enum status_code nvmError = nvm_erase_row(APP_START_ADDRESS + (i*256));
	if(nvmError != STATUS_OK)
	{
		SerialConsoleWriteString("Erase error");
	}
	
	//Make sure it got erased - we read the page. Erasure in NVM is an 0xFF
	for(int iter = 0; iter < 256; iter++)
	{
		// Iterate address by 256 times the i th iteration.
		char *a = (char *)(APP_START_ADDRESS + iter + (256*i)); //Pointer pointing to address APP_START_ADDRESS
		if(*a != 0xFF)
		{
			SerialConsoleWriteString("Error - test page is not erased!");
			break;	
		}
	}	

	//Read SD Card File
	test_bin_TestB[0] = LUN_ID_SD_MMC_0_MEM + '0';
	res = f_open(&file_object, (char const *)test_bin_TestB, FA_READ);		// Check whether the TestA file exists
		
	if (res != FR_OK)
	{
		SerialConsoleWriteString("Could not open test file!\r\n");
	}

	int numBytesLeft = 256;													// Number of bytes left.
	numBytesRead = 0;														// Number of bytes read.
	int numberBytesTotal = 0;												// Total number of bytes.
	res = f_lseek(&file_object, 256*i);
	while(numBytesLeft  != 0) 
	{
		
		res = f_read(&file_object, &readBuffer[numberBytesTotal], numBytesLeft, &numBytesRead); //The 'numBytesRead' helps us get a byte array from a FileStream.
		numBytesLeft -= numBytesLeft;															// Update numBytesLeft
		numberBytesTotal += numBytesRead;														// Update numBytesRead
	}
	


	//Write data to first row. Writes are per page, so we need four writes to write a complete row. The data iterates in sets of 256 bytes.
	res = nvm_write_buffer (APP_START_ADDRESS + (i*256), &readBuffer[0], 64);
	res = nvm_write_buffer (APP_START_ADDRESS + 64 + (i*256), &readBuffer[64], 64);
	res = nvm_write_buffer (APP_START_ADDRESS + 128 + (i*256), &readBuffer[128], 64);
	res = nvm_write_buffer (APP_START_ADDRESS + 192 + (i*256), &readBuffer[192], 64);

		if (res != FR_OK)
		{
			SerialConsoleWriteString("Test write to NVM failed!\r\n");
		}
		else
		{
			//SerialConsoleWriteString("Test write to NVM succeeded!\r\n");
		}
	}	
} */
	

	//4.) DEINITIALIZE HW AND JUMP TO MAIN APPLICATION!
	SerialConsoleWriteString("ESE516 - EXIT BOOTLOADER");	//Order to add string to TX Buffer
	delay_cycles_ms(100); //Delay to allow print
		
		//Deinitialize HW - deinitialize started HW here!
		DeinitializeSerialConsole(); //Deinitializes UART
		sd_mmc_deinit(); //Deinitialize SD CARD


		//Jump to application
		jumpToApplication();

		//Should not reach here! The device should have jumped to the main FW.
	
}







/******************************************************************************
* Static Functions
******************************************************************************/



/**************************************************************************//**
* function      static void StartFilesystemAndTest()
* @brief        Starts the filesystem and tests it. Sets the filesystem to the global variable fs
* @details      Jumps to the main application. Please turn off ALL PERIPHERALS that were turned on by the bootloader
*				before performing the jump!
* @return       Returns true is SD card and file system test passed. False otherwise.
******************************************************************************/
static bool StartFilesystemAndTest(void)
{
	bool sdCardPass = true;
	uint8_t binbuff[256];

	//Before we begin - fill buffer for binary write test
	//Fill binbuff with values 0x00 - 0xFF
	for(int i = 0; i < 256; i++)
	{
		binbuff[i] = i;
	}

	//MOUNT SD CARD
	Ctrl_status sdStatus= SdCard_Initiate();
	if(sdStatus == CTRL_GOOD) //If the SD card is good we continue mounting the system!
	{
		SerialConsoleWriteString("SD Card initiated correctly!\n\r");

		//Attempt to mount a FAT file system on the SD Card using FATFS
		SerialConsoleWriteString("Mount disk (f_mount)...\r\n");
		memset(&fs, 0, sizeof(FATFS));
		res = f_mount(LUN_ID_SD_MMC_0_MEM, &fs); //Order FATFS Mount
		if (FR_INVALID_DRIVE == res)
		{
			LogMessage(LOG_INFO_LVL ,"[FAIL] res %d\r\n", res);
			sdCardPass = false;
			goto main_end_of_test;
		}
		SerialConsoleWriteString("[OK]\r\n");

		//Create and open a file
		SerialConsoleWriteString("Create a file (f_open)...\r\n");

		test_file_name[0] = LUN_ID_SD_MMC_0_MEM + '0';
		res = f_open(&file_object,
		(char const *)test_file_name,
		FA_CREATE_ALWAYS | FA_WRITE);
		
		if (res != FR_OK)
		{
			LogMessage(LOG_INFO_LVL ,"[FAIL] res %d\r\n", res);
			sdCardPass = false;
			goto main_end_of_test;
		}

		SerialConsoleWriteString("[OK]\r\n");

		//Write to a file
		SerialConsoleWriteString("Write to test file (f_puts)...\r\n");

		if (0 == f_puts("Test SD/MMC stack\n", &file_object))
		{
			f_close(&file_object);
			LogMessage(LOG_INFO_LVL ,"[FAIL]\r\n");
			sdCardPass = false;
			goto main_end_of_test;
		}

		SerialConsoleWriteString("[OK]\r\n");
		f_close(&file_object); //Close file
		SerialConsoleWriteString("Test is successful.\n\r");


		//Write binary file
		//Read SD Card File
		test_bin_file[0] = LUN_ID_SD_MMC_0_MEM + '0';
		res = f_open(&file_object, (char const *)test_bin_file, FA_WRITE | FA_CREATE_ALWAYS);
		
		if (res != FR_OK)
		{
			SerialConsoleWriteString("Could not open binary file!\r\n");
			LogMessage(LOG_INFO_LVL ,"[FAIL] res %d\r\n", res);
			sdCardPass = false;
			goto main_end_of_test;
		}

		//Write to a binaryfile
		SerialConsoleWriteString("Write to test file (f_write)...\r\n");
		uint32_t varWrite = 0;
		if (0 != f_write(&file_object, binbuff,256, &varWrite))
		{
			f_close(&file_object);
			LogMessage(LOG_INFO_LVL ,"[FAIL]\r\n");
			sdCardPass = false;
			goto main_end_of_test;
		}

		SerialConsoleWriteString("[OK]\r\n");
		f_close(&file_object); //Close file
		SerialConsoleWriteString("Test is successful.\n\r");
		
		main_end_of_test:
		SerialConsoleWriteString("End of Test.\n\r");

	}
	else
	{
		SerialConsoleWriteString("SD Card failed initiation! Check connections!\n\r");
		sdCardPass = false;
	}

	return sdCardPass;
}



/**************************************************************************//**
* function      static void jumpToApplication(void)
* @brief        Jumps to main application
* @details      Jumps to the main application. Please turn off ALL PERIPHERALS that were turned on by the bootloader
*				before performing the jump!
* @return       
******************************************************************************/
static void jumpToApplication(void)
{
// Function pointer to application section
void (*applicationCodeEntry)(void);

// Rebase stack pointer
__set_MSP(*(uint32_t *) APP_START_ADDRESS);

// Rebase vector table
SCB->VTOR = ((uint32_t) APP_START_ADDRESS & SCB_VTOR_TBLOFF_Msk);

// Set pointer to application section
applicationCodeEntry =
(void (*)(void))(unsigned *)(*(unsigned *)(APP_START_RESET_VEC_ADDRESS));

// Jump to application. By calling applicationCodeEntry() as a function we move the PC to the point in memory pointed by applicationCodeEntry, 
//which should be the start of the main FW.
applicationCodeEntry();
}



/**************************************************************************//**
* function      static void configure_nvm(void)
* @brief        Configures the NVM driver
* @details      
* @return       
******************************************************************************/
static void configure_nvm(void)
{
    struct nvm_config config_nvm;
    nvm_get_config_defaults(&config_nvm);
    config_nvm.manual_page_write = false;
    nvm_set_config(&config_nvm);
}

