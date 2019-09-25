//	main.c	-	execution of all nucleo functionality
//	Justin Turcotte, Daniel Dreise, Ramtin Alikhani
//	September 4, 2019

#include "stdio.h"
#include "string.h"

#include "stm32f303xe.h"
#include "SysClock.h"
#include "Macros.h"
#include "LED.h"
#include "SysTick.h"
#include "LCD.h"
#include "Stepper_Motor.h"
#include "RC_Servo.h"
#include "DC_Motor.h"
#include "UART.h"
#include "Encoder.h"
//#include "MACROS2.h"

// FOLLOWING MACROS USED TO DETERMINE WHICH LAB TO TEST //
//#define LED		0 *** OBSOLETE
//#define LCD		0 *** Implemented into RS232_Interface
#define STP		0
//#define SERVO 0 *** Implemented into RS232_Interface
//#define DC		0 *** Implemented into RS232_Interface
#define ENC		0
//#define UART	0 *** Implemented into RS232_Interface
#define BUZ		0

#define RS232_Interface	1

//Prototypes
void LCD_Test(void);
void DC_Motor_Test(void);
void Encoder_Test(void);
void Servo_Test(void);
void Stepper_Test(void);
void Beeper_Test(void);

int main(void){

	uint16_t counter;
	char str[32];											// For UART
	
	System_Clock_Init(); 							// Switch System Clock = 79 MHz
	
	// TIMER INIT //
	SysTick_Init(SYSTICK_TICKS);			// initialize the systick interrupt
	
	// PERIPHERAL INITIALIZATION //
	LCD_Init();												// initialize the LCD screen
	LCDclear();												// clears residual data in the LCD display from previous operation
	
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
	
	//////////////////////////////////////////
	// STEPPER MOTOR & LIMIT SWITCH TESTING //
	//////////////////////////////////////////
	
	#if STP
	TIM3_Init();
	stepperInit();
	LimSwitch_Init();
	
	LCD_Printf(FIRST_LINE, "STEPPER");
	
	//Stepper_Home();
	
	//Delay_s(5);
	
	//Stepper_Set(STPR_FULL, 0x00FF, 90);
	
	//Delay_s(5);
	
	//Stepper_Set(STPR_FULL, 0x0FFF, -90);
	
	//Delay_s(5);
	
	//Stepper_Set(STPR_FULL, 0xFFFF, 0);
	
	while(!Get_LimSwitch_State()) 
	{	
		stepperRun(STPR_HALF_FRWRD);
		Delay_ms(50);
	}
	
	while(TRUE)
	{
		//LCD_Printf(SECOND_LINE, "Stepper Pos: %d ", Get_Stepper_Position());
		//LCD_Printf(SECOND_LINE, "Lim switch: %d", Get_LimSwitch_State());
		Delay_ms(20);
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
	
	////////////
	// BUZZER //
	////////////
	
	#if BUZ
	
	while(TRUE)
	{
	}
	#endif
	
	#if RS232_Interface
	
	TIM3_Init();
	
	UART1_init();
	Encoder_Init();
	stepperInit();
	LimSwitch_Init();
	DC_Init();
	
	RC_Init();	//needs to init last
	
	while(TRUE)
	{
		int i = 0;
		for(i = 0; i < 32; i++)
		{
			str[i] = 0;
		}
		
		//send menu to host
		Test_Menu();
		
		//get user input
		get_Input(str);
		
		LCDclear(); //need to clear LCD so that there is no left over characters
		
		LCD_Printf(SECOND_LINE, "%s", str); //output input to LCD screen for reference
		//perform correct action based on input
		if(!strcmp(str, "1")) LCD_Test();
		else if(!strcmp(str, "2")) DC_Motor_Test();
		else if(!strcmp(str, "3")) Encoder_Test();
		else if(!strcmp(str, "4")) Servo_Test();
		else if(!strcmp(str, "5")) Stepper_Test();
		else if(!strcmp(str, "6")) Beeper_Test();
		
		//keep performing action
	}
	
	#endif

}

void LCD_Test()
{
	LCD_Printf(FIRST_LINE, "LCD test go!");
}

void DC_Motor_Test()
{
	LCD_Printf(FIRST_LINE, "DC MOTOR");
	
	Motor(DC_M1, 50, DC_FORWARD);
	Motor(DC_M2, 50, DC_FORWARD);
	Delay_s(2);
	Motor(DC_M1, 100, DC_FORWARD);
	Motor(DC_M2, 100, DC_FORWARD);
	Delay_s(2);	
	Motor(DC_M1, 50, DC_BACKWARD);
	Motor(DC_M2, 50, DC_BACKWARD);
	Delay_s(2);
	Motor(DC_M1, 100, DC_BACKWARD);
	Motor(DC_M2, 100, DC_BACKWARD);
	Delay_s(2);
	//coasting speed doesn't matter because inputs need to be low anyways
	Motor(DC_M1, 100, DC_COAST);
	Motor(DC_M2, 100, DC_COAST);
	Delay_s(2);
	Motor(DC_M1, 50, DC_BRAKE);
	Motor(DC_M2, 50, DC_BRAKE);
	Delay_s(2);
	Motor(DC_M1, 100, DC_BRAKE);
	Motor(DC_M2, 100, DC_BRAKE);
	Delay_s(2);
}

void Encoder_Test()
{
	uint32_t pulseL;
	uint32_t pulseR;
	
	LCD_Printf(FIRST_LINE, "Encoder:");
	
	while(TRUE)
	{
		pulseL = Encoder_Read(ENCODER_LEFT);
		pulseR = Encoder_Read(ENCODER_RIGHT);
		
		LCD_Printf(SECOND_LINE, "EL %ldms ER %ldms ", pulseL, pulseR);
		Delay_ms(50);
	}
}

void Servo_Test()
{
	//RC_Init();
	
	LCD_Printf(FIRST_LINE, "RC SERVO");
	
	while(TRUE)
	{
		RC_Position(15);
		Delay_s(5);
		RC_Position(45);		// In testing, Servo could not exceed +89 degrees. If > 89 degrees, the gears grind.
		Delay_s(5);
		RC_Position(-5);
		Delay_s(5);
	}
}

void Stepper_Test()
{
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
}

void Beeper_Test()
{
	
}
