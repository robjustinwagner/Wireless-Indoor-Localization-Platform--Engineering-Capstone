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
#include "spi.h"

/* DEFINITIONS */


/* VARIABLES */


/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SPI_Init()
{

	// Select SPI
	P2DIR |= BIT6; 								// SS_1_SEL - output (EEPROM)
	P3DIR |= BIT0; 								// SS_2_SEL - output
	P2OUT |= BIT6;
	P3OUT |= BIT0; 								// SS_2_SEL = high

	P3SEL |= 0x0E;                            	// P3.3,2,1 option select

	UCB0CTL0 |= UCCKPL + UCMSB + UCMST + UCSYNC;// 3-pin, 8-bit SPI master
	UCB0CTL1 |= UCSSEL_2;                     	// SMCLK
	UCB0BR0 |= 0x02;                          	// /2
	UCB0BR1 = 0;                              	//

	IE2 &= (~UCB0TXIE | UCB0TXIE); 				// disable TX int

	UCB0CTL1 &= ~UCSWRST;                     	// **Initialize USCI state machine**

	//IE2 |= UCB0RXIE;                          // Enable USCI0 RX interrupt

}
