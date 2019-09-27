/* UART.h
 * 
 * Purpose: Interface for Lab 2 -- uses UART to continuously loop a message and send it to the serial port
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
 */
 
#ifndef	UART_H
#define UART_H
 
#define MAX_UART_BUFSIZ 255						// max size of the UART buff size
#define RS232_PORT C
#define RS232_TX_PIN 4
#define RS232_RX_PIN 5
#define UART_PIN_DTX	4
#define UART_PIN_DRX	5
 


/****************** PROTOTYPE FUNCTIONS ********************/

void UART1_init(void);
void UART1_printf(char *fmt, ...);
void UARTputc(char str);
void UARTputs(char *str);
void Test_Menu(void);
void receive(USART_TypeDef *USARTx, char *buffer, uint32_t *pCounter);
char read_USART(void);
char read(USART_TypeDef *USARTx, char *buffer, uint32_t *pCounter);
void get_Input(char* str);

 
#endif 
