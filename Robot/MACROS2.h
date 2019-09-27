 /* MACROS.h
 *
 * This is a cool toolbox which has a bunch of macros and fancy tricks in C.
 * 
 * By Daniel Dreise
 * All Rights Reserved
 * 
 * Date Started: January 30, 2019
 * 
 * ***********************************************************************************************************************
 * 
 * CONTENTS:
 * Bit Manipulation
 *      Evaluate lower byte
 *      Set
 *      Clear
 *      Flip
 *      Force
 * 
 * GPIO Configuration
 *      Drive type
 *      Mode
 *      Variable port name
 * 
 * LCD Configuration
 * 
 ******************************************* Bit Manipulation ************************************************************/
#ifndef MACROS2_H
#define MACROS2_H

#include "stm32f303xe.h"

#define LOW(value)              ((value) & 0xFF)        // Evaluates the lower byte of specified value
#define LO_NYBBLE(value)        ((value) & 0x0F)        // Evaluates the lower nybble of specified value
#define HI_NYBBLE(value)        (((value) >> 4) & 0x0F) // Evaluates the upper nybble of specified value
#define SET_BITS(port, mask)    ((port) |= (mask))      // SET bits of port specified by mask
#define CLR_BITS(port, mask)    ((port) &= ~(mask))     // CLEARS bits of port specified by mask
#define FLIP_BITS(port, mask)   ((port) ^= (mask))       // FLIP bits of port specified by mask
#define FORCE_BITS(port, mask, value) \
            (port) = ((port) & (~(mask)) | ((value) & (mask)))  // FORCE bits of port specified by mask to specified value

/******************************************* GPIO Configuration **********************************************************/
// Allows GPIO ports to be given specific names and allows the variable GPIO to be read by other macros
#define GPIO_PORT(port) GPIOx(port)             
#define GPIOx(port)     GPIO ## port
#define LCDport B
#define LEDport A
 
/** can add more names here **/


// Set output type of specified GPIO to either push-pull or open-drain
#define GPIO_PIN_DRV_TYPE(port, pin, type) \
            FORCE_BITS(GPIO_PORT(port)->OTYPER, (1UL << (pin)), ((type) << (pin)))
#define PP 0
#define OD 1

// Set pin function of specified GPIO to IN, OUT, Alternate Function or Analog
#define GPIO_PIN_MODE(port, pin, mode) \
            FORCE_BITS(GPIO_PORT(port)->MODER, (3UL << ((pin) * 2)), ((mode) << ((pin) * 2))) 
#define IN 0
#define OUT 1
#define AF 2
#define ANALOG 3

// Set INPUT PIN to No pull-up/pull-down = 00, pull-up = 01, pull-down = 10, reserved = 11
#define  GPIO_PIN_PUPD(port, pin, mode) \
						FORCE_BITS(GPIO_PORT(port)->PUPDR, (3UL << ((pin) * 2)), ((mode) << ((pin) * 2)))
#define NONE	0
#define PULL_UP	1
#define PULL_DOWN 2
#define	RESRVD	3



/***************************************** LCD Configuration *************************************************************/
#define LCD_PORT    GPIOB->ODR          // Identified that the GPIO port B is connected to the LCD
#define LCD_E_BIT   GPIO_ODR_12     		// Mask to specify the E bit
#define LCD_RS_BIT  GPIO_ODR_13     		// Mask to identify the RS bit
#define LCD_BUS_BIT (GPIO_ODR_4 | GPIO_ODR_5 | GPIO_ODR_6 | GPIO_ODR_7) //0xF000              // Mask to specify 4 LCD bus bits
#define LCD_E_HI    SET_BITS(LCD_PORT, LCD_E_BIT)   // Set the E line to the LCD module HIGH
#define LCD_E_LO    CLR_BITS(LCD_PORT, LCD_E_BIT)   // Set the E line to the LCD module LOW
#define LCD_RS_IR		CLR_BITS((LCD_PORT), (LCD_RS_BIT))
#define	LCD_RS_DR		SET_BITS((LCD_PORT), (LCD_RS_BIT))
// Force the 4 bits of port connected to LCD data pins to the value of the lower nybble of its argument
#define LCD_BUS(nybble) FORCE_BITS(LCD_PORT, LCD_BUS_BIT, (nybble << 4)) // (nybble << 4)//(nybble << 12)

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
#define LCD_FUNCTION_5X7FONT    0x00
#define LCD_FUNCTION_2LINES     0x08
#define LCD_FUNCTION_1LINE      0x00
#define LCD_FUNCTION_8BIT       0x10
#define LCD_FUNCTION_4BIT       0x00

/***************************************** KEYPAD Configuration *************************************************************/
#define KEY_OUTPUT(value) \
							((FORCE_BITS(GPIOB->ODR, (1UL << 2), ((((value) & 8UL) >> 3) << 2))) | \
							(FORCE_BITS(GPIOB->ODR, (1UL << 6), ((((value) & 4UL) >> 2) << 6))) | \
							(FORCE_BITS(GPIOB->ODR, (1UL << 7), ((((value) & 2UL) >> 1) << 7))) | \
							(FORCE_BITS(GPIOD->ODR, (1UL << 0), ((((value) & 1UL) << 0)))))

#endif

