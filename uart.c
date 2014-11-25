/*
 * WEARABLE INDOOR LOCALIZATION DEVICE
 * ECE 453, FALL 2014
 *
 * Calvin Hareng
 * Pedro Melgarejo
 * Robert Wagner
 *
 * UART.C
 */

/* INCLUDES */
#include <msp430f2619.h>
#include "uart.h"

/* DEFINITIONS */


/* VARIABLES */


void UART_Init()
{
	BCSCTL1 = CALBC1_1MHZ;                 			// MCLK at 16MHz
	DCOCTL = CALDCO_1MHZ;

	P3SEL |= 0xC0;                             		// P3.6,7 = USCI_A0 TXD/RXD

	//115200 baud, clocked from 16MHz SMCLK
	UCA1CTL1 |= UCSSEL_2 ;        					// CLK = SMCL
	UCA1BR0 = 8;               						// 115200 baud if SMCLK@16MHz
	UCA1BR1 = 0x00;
	UCA1MCTL = 6; 						  			// Modulation UCBRSx = 7

	UC1IFG   &= ~(UCA1TXIFG | UCA1RXIFG);          	// clear possible pending interrupts
	UC1IE	 &= ~UCA1TXIE;
	UCA1CTL1 &= ~UCSWRST;                          	// Initialize USCI state machine
	UC1IE    |=  (UCA1RXIE  ); //| UCA1TXIE);      	// Enable USCI_A1 RX interrupt

	__bis_SR_register(GIE);       					// set 'general interrupt enable' bit
	__enable_interrupt();
}

void UART_Print(unsigned char *label, unsigned char *dataToVerify, bool isChar)
{
	bool print = true;
	while(print) { 						// Loop until null char is flagged
		while(!(UC1IFG & UCA1TXIFG)); 	// Wait for TX buffer to be ready for new data
		UCA1TXBUF = *label; 			//Write the character at the location specified py the pointer
		label++; 						//Increment the TxString pointer to point to the next character
		if(*label == '\0')print = false;//Terminate loop
	}

	print = true;
	while(print) { 											// Loop until null char is flagged
		while(!(UC1IFG & UCA1TXIFG)); 						// Wait for TX buffer to be ready for new data
		UCA1TXBUF = *dataToVerify; 							//Write the character at the location specified py the pointer
		dataToVerify++; 									//Increment the TxString pointer to point to the next character
		if(*dataToVerify == '\0' || isChar) print = false;	//Terminate loop
	}

	//terminate line
	while(!(UC1IFG & UCA1TXIFG));
	UCA1TXBUF = '\n';
	while(!(UC1IFG & UCA1TXIFG));
	UCA1TXBUF = '\r';
	while(!(UC1IFG & UCA1TXIFG));
}
