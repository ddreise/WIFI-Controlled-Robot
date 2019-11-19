// DAC.c
//
// Implementation of a Digital-to-Analog Converter in order to physically display speeds as voltages
//
// 

#include <stdio.h>
#include "stm32f303xe.h"
#include "DAC.h"
#include "Control_Law.h"
#include "Encoder.h"
#include "Macros.h"

void DAC_Init(){
	
	GPIO_PORT_ENABLE(DAC_PORT);									// Enable output port
	
	SET_BITS(RCC->APB1ENR, RCC_APB1ENR_DAC1EN);		// Enable DAC1 interface clock
	
	GPIO_PIN_MODE(DAC_PORT, DAC_PIN, ANALOG);		// Set as analog output
	
	SET_BITS(DAC->CR, DAC_CR_EN1);							// Enable DAC1
	
	CLR_BITS(DAC->DHR8R1, DAC_DHR8R1_DACC1DHR);	// Clear output register
	
}

/* DAC Output

DACoutput = Vref * DOR/4096

Current Vref = around 3.3V

*/


void DAC_output(uint16_t wheel_speed){
	
	FORCE_BITS(DAC->DHR8R1, DAC_DHR8R1_DACC1DHR, wheel_speed);
	
}
