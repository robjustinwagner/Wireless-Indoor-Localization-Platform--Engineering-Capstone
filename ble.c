/*
 * WEARABLE INDOOR LOCALIZATION DEVICE
 * ECE 453, FALL 2014
 *
 * Calvin Hareng
 * Pedro Melgarejo
 * Robert Wagner
 *
 * BLE.C
 * ====================
 * BLE WAKE_HW 	-- P1.0
 * BLE_RST		-- P1.5
 * BLE WAKE_SW	-- P1.6
 *
 */

/* INCLUDES */
#include <msp430f2619.h>
#include "ble.h"
#include "uart.c"

/* DEFINITIONS */
typedef int bool;
#define true 1
#define false 0

/* VARIABLES */
static unsigned char data_from_terminal[8*20];
static unsigned char data_from_ble[8*20];
static bool terminal_received = false;
static bool terminal_sent = false;
unsigned char data;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// (BLE WAKE_SW) P1.6 enable
void turnOnBLE()  {  P1OUT |= BIT6; }

void turnOffBLE()  { P1OUT &= ~BIT6; }

/* EXAMPLE USAGE:
 * call this method in order to put the device in an infinite loop state.
 * from this state, any commands sent via the terminal to the device will
 * be sent to the BLE
 */
void DEBUG_BLE_Echo_To_Terminal()
{
	//DEBUG_UART_Print("Starting BLE Communication: ", "here", false);
	//__bis_SR_register(LPM4_bits + GIE);            // sleep, leave interrupts on

	//while(1)
	//{
		if(terminal_sent)
		{
			terminal_sent = false;
			DEBUG_UART_Print("Terminal sent: ", &data_from_terminal[0], false);
			unsigned int index = 0;
			while(data_from_terminal[index] != 0x0D)
			{
				while(!(UC1IFG & UCA1TXIFG)); //wait for last transmit
				UCA0TXBUF = data_from_terminal[index];
				index++;
			}
			while(!(UC1IFG & UCA1TXIFG)); //wait for last transmit
			UCA0TXBUF = data_from_terminal[index];
		}
		if(terminal_received)
		{
			{
				terminal_received = false;
				DEBUG_UART_Print("Terminal received from BLE: ", &data_from_ble[0], false);
				unsigned int index = 0;
				while(data_from_ble[index] != 0x0D)
				{
					while(!(UC1IFG & UCA1TXIFG)); //wait for last transmit
					UCA1TXBUF = data_from_ble[index];
					index++;
				}
				while(!(UC1IFG & UCA1TXIFG)); //wait for last transmit
				UCA1TXBUF = data_from_ble[index];
			}
		}
	//}

}

// Echo back RXed character, confirm TX buffer is ready first
#pragma vector=USCIAB1RX_VECTOR
__interrupt void USCI1RX_ISR(void)
{
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
	static unsigned char *bleDataP = &data_from_ble[0];

	*bleDataP = UCA0RXBUF;
	/*
	if(*bleDataP == 0x0D)
	{
		terminal_received = true;
		bleDataP = &data_from_ble[0]; //reset ptr
	}
	else
	{
		bleDataP++;
	}
	*/
	terminal_received = true;
}
