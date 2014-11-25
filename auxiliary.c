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
int aux_Crap(void) {
	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer

	CCTL0 = CCIE;                             // CCR0 interrupt enabled
	TACTL = TASSEL_2 + MC_1 + ID_3;           // SMCLK/8, upmode
	CCR0 =  10000;                     // 12.5 Hz

	P6OUT &= 0x00;
	P6DIR |= (BIT5 | BIT6);					// Set P1.0 to output direction

	// P6.5 & P6.6 enable (0x60 = BIT5 | BIT6)
	P6OUT &= ~(BIT5 | BIT6);

	// SW1 = P4.6
	//P4REN |= BIT6; 	// Enable Pull-up/Pull-down resistors
	P4DIR &= ~(BIT6); // SW1 - input
	//P4OUT |= BIT6;  // Enable SW1?

	// Push Button Interrupts
	/*
	P4IE |= BIT6; // P4.6 interrupt enabled
	P4IES |= BIT6; // P4.6 Hi/lo edge
	P4IFG &= ~BIT6; // P4.6 IFG cleared
	*/
	//_BIS_SR(CPUOFF + LPM4 + GIE); // Enter LPM4 w/interrupt
	__enable_interrupt();




	for(;;){

/*
		switch(P4OUT & BIT6) {
			case BIT6: // dec: 2^6 = 64
				P6OUT ^= (BIT5 | BIT6); // P1.0 = toggle
				//P4IFG &= ~BIT6;    // clear the interrupt flag
				break;
			default:
				break;
			}
*/
	}

	//return 1;
}

// Port 1 interrupt service routine
/*#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
	switch(P4IFG&BIT6) {
	case BIT6:
		P6OUT ^= (BIT5 | BIT6); // P1.0 = toggle
		P4IFG &= ~BIT6;    // clear the interrupt flag
		return;
	default:
		P4IFG = 0;

		return;
	}



}
*/
// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
	P6OUT ^= BIT5;                            // Toggle P1.0
}

