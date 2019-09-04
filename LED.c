//LED.c - implementation to flash an LED
//Justin Turcotte
//January 10, 2018

#include "LED.h"
#include "stm32f303xe.h"
#include "Macros.h"

//Initializes GPIOB or GPIOE to use the LED (will probably work on other ports with a LED)
void LED_Init(GPIO_TypeDef* PORT, uint32_t PIN)
{
	GPIO_PORT_ENABLE(LED_PORT);				// Enable clock of Port A
	
	//setup LED
	PORT->MODER &= ~(3UL << (2*PIN)); // Clear mode bits
	PORT->MODER |=   1UL << (2*PIN);  // Set mode to output

	PORT->OTYPER &= ~(1UL << PIN); // Select push-pull output

}

//Toggles a PIN on a PORT high and low with a DELAY
void Toggle_PIN(GPIO_TypeDef* PORT, uint32_t PIN, uint32_t DELAY)
{
	Change_PIN(PORT, PIN, 1);
	
	for(uint32_t i = 0; i < DELAY; i++);
	
	Change_PIN(PORT, PIN, 0);
	
	for(uint32_t i = 0; i < DELAY; i++);
}

//Change the PORTs PIN to a new STATE (high or low, 1 or 0) NOTE: if you use > 1, you'll be changing multiple pins
void Change_PIN(GPIO_TypeDef* PORT, uint32_t PIN, uint8_t STATE)
{
	if(STATE == 0)
	{
		PORT->ODR		&= ~1UL << PIN;	// Output low to PORT
	}
	else
	{
		PORT->ODR   |= 1UL << PIN;  // Output high to PORT
	}
}
