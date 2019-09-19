/* UART.c
 * 
 * Purpose: Implementation for RS232 -- uses UART to continuously loop a message and send it to the serial port
 * 
 * Project: Lab 2
 *
 * By: Daniel Dreise
 *
 * Date start: January 17, 2019
 * Date finish: January 18, 2019
 *
 * Semester: Winter 2019
 * Embedded Systems Software - Conestoga College
 *
 * PIN CONFIGURATION
 *
 * 		RS232_TX -> PC4
 *		RS232_RX -> PC5
 *
 *		USART1_TX AND _RX on ALTERNATE FUNCTION 7
 */
 
 
 /******************* LIBRARIES AND DEFINITIONS *******************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "stm32f303xe.h"
#include "UART.h"
//#include "MACROS2.h"
#include "LCD.h"
#include "Macros.h"

#define BUFFERSIZE 32

char USART1_Buffer[BUFFERSIZE];
char string_buffer[BUFFERSIZE];
volatile uint32_t Rx_Counter = 0;
uint8_t temp;
uint8_t out_counter = 0;
uint8_t in_counter = 0;







/******************** FUNCTION IMPLEMENTATIONS ********************/


void UART1_init(void) {
	
// *******************************************
// *** turn on clocks for USART 1 - 72 MHz ***
// *******************************************
	
	// Enable the clock of USART 1
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;  // Enable USART 1 clock		
	
	// Select the USART1 clock source
	// 00: PCLK selected as USART2 clock
	// 01: System clock (SYSCLK) selected as USART2 clock
	// 10: HSI16 clock selected as USART2 clock
	// 11: LSE clock selected as USART2 clock
	CLR_BITS(RCC->CFGR3, RCC_CFGR3_USART1SW);
	SET_BITS(RCC->CFGR3, RCC_CFGR3_USART1SW_0);

	
// ***************************************************************************************	
// *** configure GPIO port C so that bits 4 and 5 are connected to Tx and Rx of USART1 ***	
	// Alternate function, High Speed, Push pull, Pull up
	// **********************************************************	
		
	// Enable the peripheral clock of GPIO Port C 
	RCC->AHBENR |=   RCC_AHBENR_GPIOCEN;
	
	// Select pin mode as Alternate Function for pins 4 and 5
	GPIO_PIN_MODE(RS232_PORT, RS232_TX_PIN, AF);
	GPIO_PIN_MODE(RS232_PORT, RS232_RX_PIN, AF);
	
	// Select alternate function 7 (USART1) for pins 4 and 5
	//FORCE_BITS(GPIO_PORT(RS232_PORT)->AFR[0], GPIO_AFRL_AFRL4, 7UL << GPIO_AFRL_AFRL4_Pos);
	//FORCE_BITS(GPIO_PORT(RS232_PORT)->AFR[0], GPIO_AFRL_AFRL5, 7UL << GPIO_AFRL_AFRL5_Pos);
	
	GPIOC->AFR[0] &= ~(0xF << (UART_PIN_DTX * 4));
	GPIOC->AFR[0] &= ~(0xF << (UART_PIN_DRX * 4));
	
	GPIOC->AFR[0] |= 0x7 << (UART_PIN_DTX * 4);
	GPIOC->AFR[0] |= 0x7 << (UART_PIN_DRX * 4);
	
	// Select GPIO speed (select high
	// Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	SET_BITS(GPIO_PORT(RS232_PORT)->OSPEEDR, GPIO_OSPEEDER_OSPEEDR4);
	SET_BITS(GPIO_PORT(RS232_PORT)->OSPEEDR, GPIO_OSPEEDER_OSPEEDR5);
	
	// Select GPIO as PULL-UP
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIO_PIN_PUPD(RS232_PORT, RS232_TX_PIN, PULL_UP);
	GPIO_PIN_PUPD(RS232_PORT, RS232_RX_PIN, PULL_UP);

	// Select GPIO as PUSH-PULL
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
	GPIO_PIN_DRV_TYPE(RS232_PORT, RS232_TX_PIN, PP);
	GPIO_PIN_DRV_TYPE(RS232_PORT, RS232_RX_PIN, PP);
	

// ***************************************************************************************	
// *** Configure USART1 as:
// ***      No hardware flow control, 8 data bits, no parity, 1 start bit and 1 stop bit
// ***************************************************************************************	
	
	CLR_BITS(USART1->CR1, USART_CR1_UE);	// Disable USART
	//USART1->CR1 &= ~USART_CR1_UE;  // Disable USART
	
	// Configure word length to 8 bit
	//USART1->CR1 &= ~USART_CR1_M;   // M: 00 = 8 data bits, 01 = 9 data bits, 10 = 7 data bits
	CLR_BITS(USART1->CR1, USART_CR1_M);
	
	// Configure oversampling mode: Oversampling by 16 
	//USART1->CR1 &= ~USART_CR1_OVER8;  // 0 = oversampling by 16, 1 = oversampling by 8
	CLR_BITS(USART1->CR1, USART_CR1_OVER8);
	
	// Configure stop bits to 1 stop bit
	//   00: 1 Stop bit;      01: 0.5 Stop bit
	//   10: 2 Stop bits;     11: 1.5 Stop bit
	//USART1->CR2 &= ~USART_CR2_STOP;   
	CLR_BITS(USART1->CR2, USART_CR2_STOP);
                                    
	// Set Baudrate to 9600 using APB frequency (72,000,000 Hz)
	// If oversampling by 16, Tx/Rx baud = f_CK / USARTDIV,  
	// If oversampling by 8,  Tx/Rx baud = 2*f_CK / USARTDIV
  // When OVER8 = 0, BRR = USARTDIV
	// USARTDIV = 72MHz/9600 = 7500 = 0x1D4C
	USART1->BRR  = 0x1D4C; // Limited to 16 bits --> changed from 0x1C14
	
	// Enable USART
	//USART1->CR1  |= USART_CR1_UE;                 
	SET_BITS(USART1->CR1, USART_CR1_UE);

	// Enable Transmitter and Receiver
	SET_BITS(USART1->CR1, USART_CR1_RE | USART_CR1_TE);
	//USART1->CR1  |= (USART_CR1_RE | USART_CR1_TE);

	// Interrupt Configuration for Rx
	SET_BITS(USART1->CR1, USART_CR1_RXNEIE);
	CLR_BITS(USART1->CR1, USART_CR1_TXEIE);
	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_EnableIRQ(USART1_IRQn);
	

	
	while ( (USART1->ISR & USART_ISR_TEACK) == 0); // Verify that the USART is ready for reception
	while ( (USART1->ISR & USART_ISR_REACK) == 0); // Verify that the USART is ready for transmission
}

// Function to handle the reception of data from host computer to Nucleo
void USART1_IRQHandler(void){
	receive(USART1, USART1_Buffer, &Rx_Counter); // &Rx_Counter
}

void receive(USART_TypeDef *USARTx, char *buffer, uint32_t *pCounter){	// *pCounter

	
		buffer[in_counter] = USART1->RDR;
		in_counter = (in_counter + 1) % BUFFERSIZE;
	
		//(*pCounter)++;
		Rx_Counter++;
}


char read_USART(void){
	char something;
	//temp = read(USART1, USART1_Buffer, &Rx_Counter);
	while (Rx_Counter == 0);
	something = USART1_Buffer[out_counter];
	out_counter = (out_counter + 1) % BUFFERSIZE;
	NVIC_DisableIRQ(USART1_IRQn);
	Rx_Counter--;
	NVIC_EnableIRQ(USART1_IRQn);
	
	return something;
	
}


char read(USART_TypeDef *USARTx, char *buffer, uint32_t *pCounter){
			
	
	//if( buffer[*pCounter - 1] == '\r'){
		
		//buffer[out_counter] = NULL;
		//buffer[out_counter - 1] = NULL;
		
		//while((*pCounter) != 0){
									
			
			USART1->RDR = buffer[out_counter];
			
			//LCDputc(buffer[out_counter]);
			
			out_counter = (out_counter + 1) % BUFFERSIZE;
			
			NVIC_DisableIRQ(USART1_IRQn);
			(*pCounter)--;
			NVIC_EnableIRQ(USART1_IRQn);
			
		//LCDputc(buffer[out_counter]);
								
		//}
				
		//LCDprintf(FIRST, buffer);
		
	//}
	return buffer[out_counter];
}


// PRINT VALUES SENT FROM MAIN USING VARIABLE FUNCTION
//
// The following is a variable function. The process follwing is how to implement a function so that
// it can take numerous parameters depending on how many parameters are sent into function.
void UART1_printf(char *fmt, ...){
	

	va_list valist;
	//unsigned int i = 0;
	
	char buffer[MAX_UART_BUFSIZ];

	va_start(valist, fmt);
	
	vsnprintf( buffer, MAX_UART_BUFSIZ, fmt, valist);
	
	va_end(valist);
	
	
	UARTputs(buffer);
	
}


// TRANSMIT VALUE TO USART
void UARTputc(char str){
	
	while (!(USART1->ISR & USART_ISR_TXE));		// check to see if USART1 is finished transmitting AND that USART1 is emtpy
	USART1->TDR = (uint8_t)str;								// transmit value
}


// TRANSMIT STRING TO USART
void UARTputs(char *str){
	
	while(*str)																// check if string is NULL, if not, call function to transmit value.
			UARTputc(*str++);
	
}

void Test_Menu(void){
	
	char menu[] = {"\n\rEnter test you would like to run:\n\n\r 1 - LCD \n\r 2 - DC Motor \n\r 3 - DC Motor Encoders \n\r 4 - RC Servo \n\r 5 - Stepper Motor \n\r 6 - Beeper \n\r "};
	
	UART1_printf(menu);
	
}

void get_Input(char *str){
	
	uint8_t i = 0;
	do {
		str[i] = read_USART();
		i++;
		
		if(str[i-1] == '\r')
		{
			str[i-1] = 0;
			break;
		}
		
	} while (TRUE);
	
}



		
