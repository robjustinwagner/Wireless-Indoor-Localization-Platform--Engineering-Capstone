/*
 * WEARABLE INDOOR LOCALIZATION DEVICE
 * ECE 453, FALL 2014
 *
 * Calvin Hareng
 * Pedro Melgarejo
 * Robert Wagner
 *
 * INTERRUPTS.C
 */

/* INCLUDES */
#include "gen_lib.h"
#include "ble.h"

/* DEFINITIONS */

/* VARIABLES */

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
	//FROM Auxiliary.c
	P6OUT ^= BIT5;                            // Toggle P1.0
}

// Echo back RXed character, confirm TX buffer is ready first
#pragma vector=USCIAB1RX_VECTOR
__interrupt void USCI1RX_ISR(void)
{
	//FROM BLE.c
	static unsigned char *terminalDataP = &data_from_terminal[0];

	*terminalDataP = UCA1RXBUF;
	if(*terminalDataP == 0x0D)
	{
		terminal_sent = true;
		terminalDataP = &data_from_terminal[0]; //reset ptr
	}
	else
	{
		terminalDataP++;
	}

}


#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
	//FROM BLE.c
	static unsigned char *bleDataP = &data_from_ble[0];

	*bleDataP = UCA0RXBUF;
	if(*bleDataP == 0x0D)
	{
		terminal_received = true;
		bleDataP = &data_from_ble[0]; //reset ptr
	}
	else
	{
		bleDataP++;
	}
}
