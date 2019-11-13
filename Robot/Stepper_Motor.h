//Stepper_Motor.h - Interface to handle the stepper motor output
//Justin Turcotte
//February 4, 2019

#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#include "stm32f303xe.h"

// PIN SETUP - STPR//
#define STPR_PORT					A

#define STPR_PIN_IN1			0
#define STPR_PIN_IN2			1
#define STPR_PIN_IN3			6
#define STPR_PIN_IN4			7

// HARD-WIRED ON DAN'S BOARD
//#define STPR_PIN_NSLEEP		4
//#define STPR_PIN_NFAULT		6
//#define STPR_PIN_TRQ			7
//#define STPR_PIN_ISEN12		8
//#define STPR_PIN_ISEN34		9
//#define STPR_PIN_MODE			10

#define STPR_OUT_MASK	(0xC3 << STPR_PIN_IN1)

// PIN SETUP - LIM //
#define LIM_PORT					B

#define LIM_PIN_RIGHT			14
#define LIM_PIN_LEFT			13

#define LIM_NO_TOUCH			0
#define LIM_LEFT_TOUCH		1
#define LIM_RIGHT_TOUCH		2

#define LIM_IN					(GPIO_PORT(LIM_PORT)->IDR)

//stepper motor states
//#define STATES		8
//#define STATE0		0x8
//#define STATE1		0xA
//#define STATE2		0x2
//#define STATE3		0x6
//#define STATE4		0x4
//#define STATE5		0x5
//#define STATE6		0x1
//#define STATE7		0x9

//stepper motor stated - CONFIGURED FOR PINOUT
#define STATES		8
#define STATE0		0x80
#define STATE1		0x82
#define STATE2		0x02
#define STATE3		0x42
#define STATE4		0x40
#define STATE5		0x41
#define STATE6		0x01
#define STATE7		0x81

// OUTPUT TO STEPPER //
#define COUNTER_MAX					7
#define STPR_MIN_DELAY_MS		5		//minimum delay between steps

#define STEP_HALF_FRWRD			1
#define STEP_FULL_FRWRD			2
#define STEP_HALF_BCKWRD		1
#define STEP_FULL_BCKWRD		2

//ENUMS
//hold the modes of the stepper motor
typedef enum STEPPER_MODE
{
	STPR_INVALID,
	STPR_STOP,
	STPR_HALF_FRWRD,
	STPR_FULL_FRWRD,
	STPR_HALF_BCKWRD,
	STPR_FULL_BCKWRD,
	STPR_FULL,
	STPR_HALF
}STEPPER_MODE;

// PROTOTYPES //

// STEPPER MOTOR //
void stepperInit(void);								//initiates everything needed to run the stepper motor
void stepperRun(uint8_t pattern);			//will write the pattern to the upper 4 bits of Port E to send to the stepper motor
void TIM3_IRQHandler(void);
void TIM3_Init(void);
void Stepper_Home(void);
void Stepper_Set(uint8_t mode, uint16_t speed, int16_t position_degrees);
void Set_Stepper_Mode(uint8_t mode);
uint8_t Get_Stepper_Mode(void);
void Set_Stepper_Steps(uint16_t steps);
uint16_t Get_Stepper_Steps(void);
void Set_Stepper_Speed(uint16_t speed);
int16_t Get_Stepper_Position(void);

void Set_Servo_State(int8_t state);

// LIMIT SWITCHES //
void LimSwitch_Init(void);
uint8_t Get_LimSwitch_State(void);

#endif
