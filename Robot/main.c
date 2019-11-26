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
#include "Robot_Command.h"
#include "DAC.h"
#include "Control_Law.h"

// FOLLOWING MACROS USED TO DETERMINE WHICH LAB TO TEST //
//#define LED		0 *** OBSOLETE
//#define LCD		0 *** Implemented into RS232_Interface
//#define STP		0 *** Implemented into RS232_Interface
//#define SERVO 0 *** Implemented into RS232_Interface
//#define DC		0 *** Implemented into RS232_Interface
#define ENC		0
//#define UART	0 *** Implemented into RS232_Interface
#define BUZ		0

#define RS232_Interface	0
#define COMMAND_TEST 1

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
	uint8_t i = 0;										// Synchronous command test variable
	uint8_t t = 0;
	uint32_t pulseL;
	uint32_t pulseR;
	
	int busy = 0;
	
	System_Clock_Init(); 							// Switch System Clock = 79 MHz
	
	// TIMER INIT //
	SysTick_Init(SYSTICK_TICKS);			// initialize the systick interrupt
	
	// PERIPHERAL INITIALIZATION //
	LCD_Init();												// initialize the LCD screen
	LCDclear();												// clears residual data in the LCD display from previous operation
	
	//////////////////////////////////////////
	// STEPPER MOTOR & LIMIT SWITCH TESTING //
	//////////////////////////////////////////
	
	#if STP

	#endif
	
	/////////////
	// ENCODER //
	/////////////
	
	#if ENC


	
	LCD_Init();												// initialize the LCD screen
	LCDclear();												// clears residual data in the LCD display from previous operation
	Encoder_Init();
	TIM3_Init();
	UART1_init();
	LimSwitch_Init();
	DC_Init();
	
	RC_Init();	//needs to init last. Correction: doesn't need to
	stepperInit();
	

	
	LCD_Printf(FIRST_LINE, "Encoder:");
	
	Motor(DC_M1, 100, DC_FORWARD);
	Motor(DC_M2, 100, DC_FORWARD);
	
	
	while(TRUE)
	{
		pulseL = Wheel_Speed(ENCODER_LEFT);
		pulseR = Wheel_Speed(ENCODER_RIGHT);
//			pulseL = Encoder_Read(ENCODER_LEFT);
//			pulseR = Encoder_Read(ENCODER_RIGHT);
		
		
		LCD_Printf(SECOND_LINE, "LW %ld RW %ld ", pulseL, pulseR);
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
		
		
		// **********************************
		if (str[0] == "$"){
			CMD(str);
		}
		// **********************************
		
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
	
	
	
	// **************************************************************************
	// **     									COMMAND FUNCTION    													**
	// *************************************************************************
	#if COMMAND_TEST
	
	TIM3_Init();
	
	UART1_init();
	Encoder_Init();
	LimSwitch_Init();
	DC_Init();
	DAC_Init();
	//Control_Law_Init();
	
	
	RC_Init();	//needs to init last. Correction: doesn't need to
	stepperInit();
	Control_Law_Init();
	

	//Command_Menu();
	
	while(TRUE){
		
		if(!get_Input(str))
		{
			busy = 0;
		}
		else
		{
			busy = 1;
		}
		
		if(!busy)
		{
			UARTputs("$ACK%");
		}
		else
		{
			UARTputs("$NACK%");
			CMD(str);
		}
		
		//Motor(DC_M1, 100, DC_FORWARD);
		//Motor(DC_M2, 100, DC_FORWARD);
		
		//DAC_output(Wheel_Speed(ENCODER_LEFT));
		
		//Delay_ms(500);

//		
//		// Motor
//		if(str[1] == '1'){
//			strcpy(str, "$M100F100F%");
//			CMD(str);
//			Delay_s(2);
//			
//			strcpy(str, "$M050B050B%");
//			CMD(str);
//			Delay_s(2);
//			
//			strcpy(str, "$M100F050B%");
//			CMD(str);
//			Delay_s(2);
//			
//			strcpy(str, "$M050B100F%");
//			CMD(str);
//			Delay_s(2);
//			
//			strcpy(str, "$M000F000F%");
//			CMD(str);
//			Delay_s(2);
//		}
//		
//		// Homing
//		else if (str[1] == '2'){
//			strcpy(str, "$H%");
//			CMD(str);
//			strcpy(str, "\n");
//			CMD(str);
//			Delay_s(2);
//		}
//		
//		// Servo down
//		else if (str[1] == '3'){
//			for(t = 0; t < 0xFF; t++){
//				strcpy(str, "$CL0D%");
//				CMD(str);
//			}
//		}
//		
//		// Servo up
//		else if (str[1] == '4'){
//			for(t = 0; t < 0xFF; t++){
//				strcpy(str, "$CL0U");
//				CMD(str);
//			}
//		}
//		
//		// Stepper Right
//		else if (str[1] == '5'){
//			for(t = 0; t < 0xFF; t++){
//				strcpy(str, "$CR1S");
//				CMD(str);
//			}
//		}
//		
//		// Stepper Left
//		else if (str[1] == '6'){
//			for(t = 0; t < 0xFF; t++){
//				strcpy(str, "$CL1S");
//				CMD(str);
//			}
//		}
//		else{
//			CMD(str);
//		}
//			
//			
//		

//		
//		if(i == 0xFF) {
//			LCD_Printf(FIRST_LINE, "STP%d SRV%d", Get_Stepper_Position, servo_position);
//			LCD_Printf(SECOND_LINE, "DCR%d DCL%d", right_speed, left_speed);
//		}
//		i++;

		
		// *********************************************
		// Home camera test

//		strcpy(str, "$H%");
//		CMD(str);
//		strcpy(str, "\n");
//		CMD(str);
		
		// *********************************************
		
		// DC Motor Test
//		strcpy(str, "$M100F100F%");
//		CMD(str);
//		Delay_s(2);
//		
//		strcpy(str, "$M050B050B%");
//		CMD(str);
//		Delay_s(2);
//		
//		strcpy(str, "$M100F050B%");
//		CMD(str);
//		Delay_s(2);
//		
//		strcpy(str, "$M050B100F%");
//		CMD(str);
//		Delay_s(2);
//		
//		strcpy(str, "$M000F000F%");
//		CMD(str);
//		Delay_s(2);
		
		// *************************************************
		// Camera movement test	
		
//		strcpy(str, "$CR0U%");
//		CMD(str);

//		strcpy(str, "$CR1S%");
//		CMD(str);
		
		// *************************************************
		// Acknowledgement Test
		
		
		
		// *************************************************
		// SYNCHRONOUS COMMAND TEST
//		strcpy(str, "$M100F100F%");
//		CMD(str);
//		
//		
//		strcpy(str, "$CL0D%");
//		CMD(str);
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
		
		// Test max and min
		RC_Position(90);
		Delay_s(5);
		RC_Position(-10);
		Delay_s(5);
	}
}

void Stepper_Test()
{
	LCD_Printf(FIRST_LINE, "STEPPER");
	
	Stepper_Home();
	LCD_Printf(SECOND_LINE, "Stepper Pos: %d ", Get_Stepper_Position());
	Delay_ms(500);
	
	Delay_s(1);
	
	Stepper_Set(STPR_HALF, 0x13B, 95);
	LCD_Printf(SECOND_LINE, "Stepper Pos: %d ", Get_Stepper_Position());
	Delay_ms(500);
	
	Delay_s(1);
	
	Stepper_Set(STPR_HALF, 0x13B, -95);
	LCD_Printf(SECOND_LINE, "Stepper Pos: %d ", Get_Stepper_Position());
	Delay_ms(500);
	
	Delay_s(1);
	
	Stepper_Set(STPR_HALF, 0x13B, 0);
	LCD_Printf(SECOND_LINE, "Stepper Pos: %d ", Get_Stepper_Position());
	Delay_ms(500);
	
	Delay_s(1);
	

}

void Beeper_Test()
{
	
}
