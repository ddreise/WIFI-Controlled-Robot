//	main.c	-	execution of all nucleo functionality
//	Justin Turcotte, Daniel Dreise, Ramtin Alikhani
//	September 4, 2019

#include "stm32f303xe.h"
#include "SysClock.h"
#include "Macros.h"
#include "LED.h"
#include "SysTick.h"
//#include "LCD.h"
//#include "Stepper_Motor.h"
//#include "RC_Servo.h"
//#include "DC_Motor.h"
//#include "UART.h"
//#include "Encoder.h"

// FOLLOWING MACROS USED TO DETERMINE WHICH LAB TO TEST //
#define LED		1
#define LCD		0
#define STP		0
#define SERVO 0
#define DC		0
#define ENC		0
#define UART	0
#define BUZ		0

int main(void){

	System_Clock_Init(); 							// Switch System Clock = 79 MHz
	
	// TIMER INIT //
	SysTick_Init(SYSTICK_TICKS);			// initialize the systick interrupt
	
	// PERIPHERAL INITIALIZATION //
//	LCD_Init();												// initialize the LCD screen
//	LCDclear();												// clears residual data in the LCD display from previous operation
	
	/////////////////
	// LED TESTING //
	/////////////////
	
	#if LED
	LED_Init(GPIOA, LED_PIN);					// initialize the LED ports to be output
	
	while(TRUE)
	{
		// Code related to flashing the LED
		Toggle_PIN(GPIOA, LED_PIN, DELAY_COUNT); //toggle red LED on and off
	}
	#endif
	
	/////////////////
	// LCD TESTING //
	/////////////////
	
	#if LCD
	uint16_t counter = 0;		//counter for incrementing values on the LCD screen
	
	LCD_Printf(FIRST_LINE, "Hello World!");
	
	while(TRUE)
	{
		// Code related to testing the LCD
		LCD_Printf(SECOND_LINE, "Count: 0x%x", counter++);
		Delay_s(1);																				//delay for readability
	}
	#endif
	
	//////////////////////////////////////////
	// STEPPER MOTOR & LIMIT SWITCH TESTING //
	//////////////////////////////////////////
	
	#if STP
	TIM3_Init();
	stepperInit();
	LimSwitch_Init();
	
	LCD_Printf(FIRST_LINE, "STEPPER");
	
	Stepper_Home();
	
	Delay_s(5);
	
	Stepper_Set(STPR_FULL, 0x00FF, 90);
	
	Delay_s(5);
	
	Stepper_Set(STPR_FULL, 0x0FFF, -90);
	
	Delay_s(5);
	
	Stepper_Set(STPR_FULL, 0xFFFF, 0);
	
	while(TRUE)
	{
		LCD_Printf(SECOND_LINE, "Stepper Pos: %d ", Get_Stepper_Position());
		Delay_ms(20);
	}
	#endif
	
	//////////////
	// RC SERVO //
	//////////////
	
	#if SERVO
	RC_Init();
	
	LCD_Printf(FIRST_LINE, "RC SERVO");
	
	while(TRUE)
	{
		RC_Position(0);
		Delay_s(5);
		RC_Position(90);
		Delay_s(5);
		RC_Position(-90);
		Delay_s(5);
	}
	#endif
	
	//////////////
	// DC MOTOR //
	//////////////
	
	#if DC
	DC_Init();
	
	LCD_Printf(FIRST_LINE, "DC MOTOR");
	
	while(TRUE)
	{
		Delay_s(5);
		Motor(DC_M1, 50, DC_FORWARD);
		Motor(DC_M2, 50, DC_FORWARD);
		Delay_s(5);
		Motor(DC_M1, 100, DC_FORWARD);
		Motor(DC_M2, 100, DC_FORWARD);
		Delay_s(5);
		Motor(DC_M1, 50, DC_BACKWARD);
		Motor(DC_M2, 50, DC_BACKWARD);
		Delay_s(5);
		Motor(DC_M1, 100, DC_BACKWARD);
		Motor(DC_M2, 100, DC_BACKWARD);
		Delay_s(5);
		//coasting speed doesn't matter because inputs need to be low anyways
		Motor(DC_M1, 100, DC_COAST);
		Motor(DC_M2, 100, DC_COAST);
		Delay_s(5);
		Motor(DC_M1, 50, DC_BRAKE);
		Motor(DC_M2, 50, DC_BRAKE);
		Delay_s(5);
		Motor(DC_M1, 100, DC_BRAKE);
		Motor(DC_M2, 100, DC_BRAKE);
	}
	#endif
	
	/////////////
	// ENCODER //
	/////////////
	
	#if ENC
	uint32_t pulseL;
	uint32_t pulseR;
	
	Encoder_Init();
	
	LCD_Printf(FIRST_LINE, "Encoder:");
	
	while(TRUE)
	{
		pulseL = Encoder_Read(ENCODER_LEFT);
		pulseR = Encoder_Read(ENCODER_RIGHT);
		
		LCD_Printf(SECOND_LINE, "EL %ldms ER %ldms ", pulseL, pulseR);
		Delay_ms(50);
	}
	#endif
	
	//////////
	// UART //
	//////////
	
	#if UART
	char* buffer = 0;
	
	UARTinit();
	
	LCD_Printf(FIRST_LINE, "UART:");
	
	UARTprintf("Hello World!");
		
	buffer = Get_Buffer();
		
	LCD_Printf(SECOND_LINE, "%s", buffer);
	
	while(TRUE)
	{
		Delay_ms(50);
	}
	#endif
	
	////////////
	// BUZZER //
	////////////
	
	#if BUZ
	
	while(TRUE)
	{
	}
	#endif
	
}

