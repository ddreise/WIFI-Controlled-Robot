//	Macros.h	-	will hold personal macros that will come in handy
//	Justin Turcotte
//	April 21, 2019

#ifndef MACROS_H
#define MACROS_H

#include "stm32f303xe.h"

#define TRUE 	1
#define FALSE 0

#define CLOCK_FREQUENCY_HZ		72000000UL

// GPIO CLOCKS //
// Enables specified port clock - needs port letter as parameter
#define	GPIO_PORT_ENABLE(port)	SET_BITS(RCC->AHBENR, GPIO_PORT_EN(port))
#define GPIO_PORT_EN(port) 			GPIO_PORT_ENx(port)
#define GPIO_PORT_ENx(port)			RCC_AHBENR_GPIO ## port ## EN

//OTYPER Register
#define OTYPER_PUSH_PULL 	0
#define OTYPER_OPEN_DRAIN 1

//MODER Register
#define MODER_IN 	0UL
#define MODER_OUT 1UL
#define AF				2UL
#define ANALOG		3UL

//OSPEEDR Register
#define OSPEEDR_LOW			0
#define OSPEEDR_MEDIUM	1
#define OSPEEDR_FAST		2
#define OSPEEDR_HIGH		3

//PUPDR Register
#define PUPDR_NO_PUPD		0
#define PUPDR_PU				1
#define PUPDR_PD				2
#define PUPDR_RESERVED	3

//BIT MANIPULATION
#define LOW(value) ((value)&(0xFF)) 						//isolates lower byte of value	
#define LO_NYBBLE(value) ((value)&0x0F) 				//isolates low nybble of lowest byte
#define HI_NYBBLE(value) (((value)>>4)&(0x0F))	//isolates high nybble of lowest byte
#define SET_BITS(reg,mask) ((reg)|=(mask)) 			//sets register with mask
#define CLR_BITS(reg,mask) ((reg)&=(~mask)) 		//clears register with mask
#define FLIP_BITS(reg,mask) ((reg)^=(mask))			//flips bits in register with mask

//force bits of mask in register to pattern
#define FORCE_BITS(reg,mask,pat) \
	(reg = ((reg) & (~mask)) | (pat))

//PIN MANIPULATION

//set output type (OTYPER) of a pin (push-pull = 0, open-drain = 1)
#define GPIO_PIN_DRV_TYPE(port,pin,type) \
	FORCE_BITS((GPIO_PORT(port))->OTYPER, ((1UL)<<(pin)), ((type)<<(pin)))

//set pin mode (MODER) of a GPIO port (in = 0, out = 1, AF = 2, analog = 3)
#define GPIO_PIN_MODE(port,pin,mode) \
	FORCE_BITS((GPIO_PORT(port))->MODER, ((3UL)<<((pin)*(2))), ((mode)<<((pin)*(2))))
	
//set speed of GPIO port (OSPEEDR)
#define GPIO_PIN_SPEED(port, pin, mode) \
	FORCE_BITS((GPIO_PORT(port))->OSPEEDR, ((3UL)<<((pin)*(2))), ((mode)<<((pin)*(2))))

//set pull-up/pull-down of GPIO port (PUPDR)
#define GPIO_PIN_PUPD(port, pin, mode) \
	FORCE_BITS((GPIO_PORT(port))->PUPDR, ((3UL)<<((pin)*(2))), ((mode)<<((pin)*(2))))

//evaluates the GPIO_PORT name using token string merging
#define GPIO_PORT(port) GPIO_PORTx(port)
#define GPIO_PORTx(port) GPIO ## port

#endif
