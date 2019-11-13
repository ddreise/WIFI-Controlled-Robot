//Encoder.h - Interface to handle the Ultrasonic range finder
//Justin Turcotte
//April 11, 2019

#ifndef ENCODER_H
#define ENCODER_H

// INCLUDES //
#include "Macros.h"
#include "stm32f303xe.h"

// INTERRUPTS //
#define EnableInterrupts asm(" ISB; CPSIE I")
#define DisableInterrupts asm(" CPSID I")

// ENCODER PORT INFORMATION //
#define ENCODER_PORT			B

#define ENCODER_PIN_LEFT	11
#define ENCODER_PIN_RIGHT	10

#define ENCODER_LEFT			0
#define ENCODER_RIGHT			1

// PROTOTYPES //
void Encoder_Init(void);							//initializes encoders
uint32_t Encoder_Read(uint8_t encoder);		// Reads encoder pulse width
uint32_t Wheel_Speed(uint8_t encoder);		// Outputs wheel speed in cm/s

#endif
