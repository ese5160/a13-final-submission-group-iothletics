/*
 * IMUTask.c
 *
 * Created: 5/3/2023 10:17:32 PM
 *  Author: User
 */ 

#include "I2cDriver/I2cDriver.h"
#include "IMU/lsm6dso_reg.h"
#include "Haptic_Motor_Driver/Haptic.h"
#include "SerialConsole/SerialConsole.h"
#include "WifiHandlerThread/WifiHandler.h"
#include "CliThread/CliThread.h"

char WriteBuffer[100];

void vIMUTask( void *pvParameters )
{
	while (1)
	{
		int state;
		
		static int16_t data_raw_acceleration[3];
		static float acceleration_mg[3];
		uint8_t reg;
		stmdev_ctx_t *dev_ctx = GetImuStruct();
		struct ImuDataPacket imuPacket;

		/* Read output only if new xl value is available */
		lsm6dso_xl_flag_data_ready_get(dev_ctx, &reg);
		
		if (reg) {
			memset(data_raw_acceleration, 0x00, 3 * sizeof(int16_t));
			lsm6dso_acceleration_raw_get(dev_ctx, data_raw_acceleration);
			acceleration_mg[0] = lsm6dso_from_fs2_to_mg(data_raw_acceleration[0]);
			acceleration_mg[1] = lsm6dso_from_fs2_to_mg(data_raw_acceleration[1]);
			acceleration_mg[2] = lsm6dso_from_fs2_to_mg(data_raw_acceleration[2]);

			snprintf((char *)WriteBuffer, 100, "Acceleration [mg]:\r\nX %d\r\nY: %d\r\nZ: %d\r\n", (int)acceleration_mg[0], (int)acceleration_mg[1], (int)acceleration_mg[2]);
			imuPacket.xmg = (int)acceleration_mg[0];
			imuPacket.ymg = (int)acceleration_mg[1];
			imuPacket.zmg = (int)acceleration_mg[2];
			WifiAddImuDataToQueue(&imuPacket);
			//WifiAddImuYDataToQueue(&imuPacket);
			//WifiAddImuZDataToQueue(&imuPacket);
			
		}
		else {
			snprintf((char *)WriteBuffer, 100, "No data ready! Sending dummy data \r\n");
			imuPacket.xmg = -1;
			imuPacket.ymg = -2;
			imuPacket.zmg = -3;
			WifiAddImuDataToQueue(&imuPacket);
			//WifiAddImuYDataToQueue(&imuPacket);
			//WifiAddImuZDataToQueue(&imuPacket);
		
		}
		
		if ((int)acceleration_mg[0] > 500 || (int)acceleration_mg[0] < -500 || (int)acceleration_mg[1] < -500 || (int)acceleration_mg[1] > 500)
		{
			
			state = Haptic_Begin();
			state = Haptic_Mode();
			
		}
		vTaskDelay(100);
	}
}