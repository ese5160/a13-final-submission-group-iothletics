/*
 * Haptic.h
 *
 * Created: 4/24/2023 9:10:36 PM
 *  Author: Arnav
 */ 


#ifndef HAPTIC_H_
#define HAPTIC_H_

#ifdef __cplusplus
extern "C"{
	#endif

int Haptic_Begin(void);
int Haptic_Mode(void);
int Haptic_MotorSelect(void);
int Haptic_Library(void);
int Haptic_Waveform(void);
int Haptic_Go(int state);
int Haptic_Drive(void);
int Haptic_Vibration(void);
// int Haptic_Setup(void);
//void Stop(void);

#ifdef __cplusplus
}
#endif
#endif /* HAPTIC_H_ */

