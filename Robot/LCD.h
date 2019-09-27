//LCD.h - Interface to handle the LCD
//Justin Turcotte
//March 3, 2019

#ifndef LCD_H
#define LCD_H

#include "Macros.h"
#include "stm32f303xe.h"

//For changing row of LCD module
#define FIRST_LINE	0x00
#define SECOND_LINE	0x40

// Commands to LCD module
#define LCD_CMD_CLEAR           0x01
#define LCD_CMD_HOME            0x02
#define LCD_CMD_ENTRY           0x04
#define LCD_CMD_DISPLAY         0x08
#define LCD_CMD_CD_SHIFT        0x10
#define LCD_CMD_FUNCTION        0x20
#define LCD_CMD_CGRAMADDR       0x40
#define LCD_CMD_SET_DDADDR      0x80

// Settings for LCD_CMD_ENTRY
#define LCD_ENTRY_MOVE_DISPLAY  0x01
#define LCD_ENTRY_MOVE_CURSOR   0x00
#define LCD_ENTRY_INC           0x02    
#define LCD_ENTRY_DEC           0x00

// Settings for LCD_CMD_DISPLAY
#define LCD_DISPLAY_BLINK       0x01
#define LCD_DISPLAY_NOBLINK     0x00
#define LCD_DISPLAY_CURSOR      0x02
#define LCD_DISPLAY_NOCURSOR    0x00
#define LCD_DISPLAY_ON          0x04
#define LCD_DISPLAY_OFF         0x00

// Settings for LCD_CMD_CD_SHIFT (shift cursor or display without changing data)
#define LCD_CD_SHIFT_RIGHT      0x04
#define LCD_CD_SHIFT_LEFT       0x00
#define LCD_CD_SHIFT_DISPLAY    0x08
#define LCD_CD_SHIFT_CURSOR     0x00

// Settings for LCD_CMD_FUNCTION
#define LCD_FUNCTION_5X10FONT   0x04
#define LCD_FUNCTION_5X8FONT    0x00
#define LCD_FUNCTION_2LINES     0x08
#define LCD_FUNCTION_1LINE      0x00
#define LCD_FUNCTION_8BIT       0x10
#define LCD_FUNCTION_4BIT       0x00

//LCD MACROS
#define MAX_LCD_BUFSIZE 32	//the display will only take enough input to fill the screen

#define LCD_PORT 	C																	//port letter of LCD
#define LCD_PORT_OUT ((GPIO_PORT(LCD_PORT))->ODR)		//output register for LCD

#define LCD_PIN_RS		6
#define LCD_PIN_E			7
#define LCD_PIN_BUS0	0
#define LCD_PIN_BUS1	1
#define LCD_PIN_BUS2	2
#define LCD_PIN_BUS3	3

#define LCD_E_BIT 		(0x1UL << (LCD_PIN_E))				//bit mask for the LCD E bit
#define LCD_RS_BIT 		(0x1UL << (LCD_PIN_RS))				//bit mask for the LCD RS bit
#define LCD_BUS_BITS	(0xFUL << (LCD_PIN_BUS0))			//bit mask for the LCD bus bits

#define LCD_E_HI	SET_BITS(LCD_PORT_OUT, LCD_E_BIT)	//enables LCD
#define LCD_E_LO	CLR_BITS(LCD_PORT_OUT, LCD_E_BIT)	//disables LCD

#define LCD_RS_DR	SET_BITS(LCD_PORT_OUT, LCD_RS_BIT)	//sets LCD to data register
#define LCD_RS_IR	CLR_BITS(LCD_PORT_OUT, LCD_RS_BIT)	//sets LCD to instruction register

//sends the nybble to the LCD bus
#define LCD_BUS(nybble) \
	FORCE_BITS(LCD_PORT_OUT, LCD_BUS_BITS, ((nybble)<<(LCD_PIN_BUS0)))
	
//Command to set CGRAM address (must not be more than 6 bits)
#define LCD_CMD_CGRAM	0x40

//Command to set DDRAM address (must not be more than 7 bits)
#define LCD_CMD_DDRAM	0x80

//PROTOTYPES
void LCD_Init(void);																		//will configure LCD
void LCD_Printf(char location, char* params, ...);			//print formatted string to LCD
void LCD_Puts(char location, char* str);								//send a string to LCD
void LCD_Putc(char location, char chr);									//send a character to LCD
void LCDclear(void);																		//clear LCD

static void LCDcmd(uint8_t cmd);												//send commands to LCD
static void LCDdata(uint8_t data);											//send data to LCD

#endif
