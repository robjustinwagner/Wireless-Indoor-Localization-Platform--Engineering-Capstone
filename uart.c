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
#include "gen_lib.h"
#include "uart.h"
#include "ble.h"

/* DEFINITIONS */


/* VARIABLES */


/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UART_Init()
{

	P3SEL |= BIT4 | BIT5;                      		// P3.4,5 = USCI_A0 TXD/RXD
	P3SEL |= 0xC0;                             		// P3.6,7 = USCI_A1 TXD/RXD

	//115200 baud, clocked from 1MHz SMCLK for UART A0
	UCA0CTL1 |= UCSSEL_2;        					// CLK = SMCL
	UCA0BR0 = 8;               						// 115200 baud if SMCLK@1MHz
	UCA0BR1 = 0x00;
	UCA0MCTL = 6;			  						// Modulation UCBRSx = 6

	//115200 baud, clocked from 1MHz SMCLK for UART A1
	UCA1CTL1 |= UCSSEL_2;        					// CLK = SMCL
	UCA1BR0 = 8;               						// 115200 baud if SMCLK@1MHz
	UCA1BR1 = 0x00;
	UCA1MCTL = 6; 						  			// Modulation UCBRSx = 6

	//UART A0
	//UC0IFG   &= ~(UCA0TXIFG | UCA0RXIFG);          	// clear possible pending interrupts
	//UC0IE	 &= ~UCA0TXIE;
	//UART A1
	//UC1IFG   &= ~(UCA1TXIFG | UCA1RXIFG);          	// clear possible pending interrupts
	//UC1IE	 &= ~UCA1TXIE;

	UCA0CTL1 &= ~UCSWRST;                          	// Initialize USCI state machine
	UCA1CTL1 &= ~UCSWRST;                          	// Initialize USCI state machine
	UC0IE |= UCA0RXIE;								// Enable USCI_A1 TX & RX interrupt
	UC1IE |= UCA1RXIE;				      			// Enable USCI_A1 RX interrupt

	//BLE WAKE_HW 	-- P1.0
	//BLE_RST 		-- P1.5
	//BLE WAKE_SW	-- P1.6
	P1OUT = 0x00;
	P1DIR |= BIT0 | BIT5 | BIT6;					// Set P1.0 to output direction for BLE

	__bis_SR_register(GIE);       					// set 'general interrupt enable' bit
	__enable_interrupt();
}

/* EXAMPLE USAGE: DEBUG_UART_Print("Received Sensor Value", &sensorValue, false); */
void DEBUG_UART_Print(unsigned char *label, unsigned char *dataToVerify, bool isChar)
{
	bool print = true;
	while(print) { 						// Loop until null char is flagged
		while(!(UC1IFG & UCA1TXIFG)); 	// Wait for TX buffer to be ready for new data
		UCA1TXBUF = *label; 			//Write the character at the location specified by the pointer
		label++; 						//Increment the TxString pointer to point to the next character
		if(*label == '\0')print = false;//Terminate loop
	}

	print = true;
	while(print) { 											// Loop until null char is flagged
		while(!(UC1IFG & UCA1TXIFG)); 						// Wait for TX buffer to be ready for new data
		UCA1TXBUF = *dataToVerify; 							//Write the character at the location specified by the pointer
		dataToVerify++; 									//Increment the TxString pointer to point to the next character
		if(*dataToVerify == '\0' || isChar
				|| *dataToVerify == 0x0D) print = false;	//Terminate loop
	}

	//terminate line
	while(!(UC1IFG & UCA1TXIFG));
	UCA1TXBUF = '\n';
	while(!(UC1IFG & UCA1TXIFG));
	UCA1TXBUF = '\r';
	while(!(UC1IFG & UCA1TXIFG));
}
