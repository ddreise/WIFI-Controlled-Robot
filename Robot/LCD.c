//LCD.c - Implementation to handle the LCD
//Justin Turcotte
//March 3, 2019

#include "LCD.h"
#include <stdarg.h>
#include <stdio.h>
#include "stm32f303xe.h"
#include "Macros.h"
#include "SysTick.h"

//FUNCTIONS
//configure LCD
void LCD_Init(void)
{
	//Initialize pins
	GPIO_PORT_ENABLE(LCD_PORT);	//intialize LCD_PORT clock
	
	GPIO_PIN_MODE(LCD_PORT, LCD_PIN_RS, MODER_OUT);
	GPIO_PIN_MODE(LCD_PORT, LCD_PIN_E, MODER_OUT);
	GPIO_PIN_MODE(LCD_PORT, LCD_PIN_BUS0, MODER_OUT);
	GPIO_PIN_MODE(LCD_PORT, LCD_PIN_BUS1, MODER_OUT);
	GPIO_PIN_MODE(LCD_PORT, LCD_PIN_BUS2, MODER_OUT);
	GPIO_PIN_MODE(LCD_PORT, LCD_PIN_BUS3, MODER_OUT);
	
	GPIO_PIN_DRV_TYPE(LCD_PORT, LCD_PIN_RS, OTYPER_PUSH_PULL);
	GPIO_PIN_DRV_TYPE(LCD_PORT, LCD_PIN_E, OTYPER_PUSH_PULL);
	GPIO_PIN_DRV_TYPE(LCD_PORT, LCD_PIN_BUS0, OTYPER_PUSH_PULL);
	GPIO_PIN_DRV_TYPE(LCD_PORT, LCD_PIN_BUS1, OTYPER_PUSH_PULL);
	GPIO_PIN_DRV_TYPE(LCD_PORT, LCD_PIN_BUS2, OTYPER_PUSH_PULL);
	GPIO_PIN_DRV_TYPE(LCD_PORT, LCD_PIN_BUS3, OTYPER_PUSH_PULL);
	
	//Magic code to configure LCD to 4-bit mode
	LCD_E_HI;
	LCD_BUS(0x03);	//wake up display & sync
	LCD_E_LO;
	
	Delay_ms(5);	//delay 5ms
	
	LCD_E_HI;
	LCD_BUS(0x03);	//wake up display & sync
	LCD_E_LO;
	
	Delay_ms(1);
	
	LCD_E_HI;
	LCD_BUS(0x03);	//wake up display & sync - go to 4-bit mode
	LCD_E_LO;
	
	LCD_E_HI;
	LCD_BUS(0x02);	//wake up display & sync - go to 4-bit mode
	LCD_E_LO;
	
	//configure the LCD display
	LCDcmd(LCD_CMD_FUNCTION | LCD_FUNCTION_4BIT | LCD_FUNCTION_2LINES | LCD_FUNCTION_5X8FONT);
	LCDcmd(LCD_CMD_DISPLAY | LCD_DISPLAY_OFF);
	LCDclear();
	LCDcmd(LCD_CMD_ENTRY | LCD_ENTRY_MOVE_CURSOR | LCD_ENTRY_INC);
	LCDcmd(LCD_CMD_DISPLAY | LCD_DISPLAY_ON | LCD_DISPLAY_NOCURSOR | LCD_DISPLAY_NOBLINK);
}

//print formatted string to LCD
void LCD_Printf(char location, char* params, ...)
{
	char buf[MAX_LCD_BUFSIZE];	//buffer to store final string
	
	va_list args;						//parameter for functions receiving variable arguments
	
	LCDcmd(LCD_CMD_DDRAM | location);	//sets the location to start printing the string on the LCD
	
	va_start(args, params);	//initializes args to retrieve argument inputted after params
	vsnprintf(buf, MAX_LCD_BUFSIZE, params, args);	//formats input and arguments like printf
	va_end(args);
	
	LCD_Puts(location, buf);
}

//send a string to the LCD display
void LCD_Puts(char location, char* str)
{
	//send each character of str to LCDdata
	
	uint8_t i = 0;	//disposable
	for(i = 0; str[i] != 0; i++) LCD_Putc(location, str[i]); //send the characters to putc unless null terminator is found
}

//send a character to the LCD display
void LCD_Putc(char location, char chr)
{
	//send character chr to LCDdata for display on LCD screen
	//ignore and do not send CR (13 ASCII) or LF (10 ASCII)
	
	static uint8_t cursor_loc = 0;	//stores location of cursor
	
	cursor_loc = location;	//need to set the cursor_loc to the proper location defined by user to avoid unintended line switches
	
	//ignore \0 and CR
	if(chr == 13 || chr == 0);
	else if (chr == 10)	//change lines when \n is detected
	{
		if(cursor_loc < 0x40)
		{
			LCDcmd(LCD_CMD_DDRAM | 0x40);
			cursor_loc = 0x40;
		}
		else
		{
			LCDcmd(LCD_CMD_DDRAM | 0x00);
			cursor_loc = 0x00;
		}
	}
	else
	{	
		if(cursor_loc == 0x10) //check if first line end is reached, goto second line
		{
			LCDcmd(LCD_CMD_DDRAM | 0x40);
			cursor_loc = 0x40;
		}
		else if(cursor_loc == 0x50)	//check if second line end is reached, goto first line
		{
			LCDcmd(LCD_CMD_DDRAM | 0x00);
			cursor_loc = 0x00;
		}
		
		LCDdata(chr);
		cursor_loc++;
	}
}

//clear LCD screen and return cursor home
void LCDclear(void)
{
	LCDcmd(LCD_CMD_CLEAR);
	LCDcmd(LCD_CMD_HOME);
}

//send commands to LCD
static void LCDcmd(uint8_t cmd)
{
	LCD_E_LO;
	LCD_RS_IR;
	LCD_E_HI;
	LCD_BUS(HI_NYBBLE(cmd));
	LCD_E_LO;
	Delay_ms(1);
	LCD_E_HI;
	LCD_BUS(LO_NYBBLE(cmd));
	LCD_E_LO;
	Delay_ms(20);
}

//send a byte to the LCD
static void LCDdata(uint8_t data)
{
	LCD_E_LO;
	LCD_RS_DR;
	LCD_E_HI;
	LCD_BUS(HI_NYBBLE(data));
	LCD_E_LO;
	Delay_ms(1);
	LCD_E_HI;
	LCD_BUS(LO_NYBBLE(data));
	LCD_E_LO;
	Delay_ms(20);
}
