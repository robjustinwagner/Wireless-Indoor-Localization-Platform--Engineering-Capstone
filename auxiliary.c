/*
 * WEARABLE INDOOR LOCALIZATION DEVICE
 * ECE 453, FALL 2014
 *
 * Calvin Hareng
 * Pedro Melgarejo
 * Robert Wagner
 *
 * AUXILIARY.C
 */

/* INCLUDES */
#include "gen_lib.h"
#include "auxiliary.h"

/* DEFINITIONS */


/* VARIABLES */


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Auxiliary_Init(void)
{

	CCTL0 = CCIE;                             	// CCR0 interrupt enabled
	TACTL = TASSEL_2 + MC_1 + ID_3;           	// SMCLK/8, upmode
	CCR0 =  65000;                     			// 10,000 was 12.5 Hz

	// P6.5 & P6.6 enable (0x60 = BIT5 | BIT6)
	P6OUT &= ~(BIT5 | BIT6);
	P6DIR |= (BIT5 | BIT6);						// Set P6.5, P6.6 to output direction

	// SW1 = P4.6
	P4OUT |= BIT6;
	P4DIR &= ~(BIT6); 							// Set P4.6/SW1 to INPUT direction

}
