/*
 * WEARABLE INDOOR LOCALIZATION DEVICE
 * ECE 453, FALL 2014
 *
 * Calvin Hareng
 * Pedro Melgarejo
 * Robert Wagner
 *
 * BLE.C
 */

/* INCLUDES */
#include <msp430f2619.h>
#include "ble.h"

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

void main(void)

{

	WDTCTL = WDTPW + WDTHOLD;               // disable watchdog timer

	initBLE();

	__bis_SR_register(GIE);       			// set 'general interrupt enable' bit
	__enable_interrupt();

	//UART_Print("Starting BLE Communication: ", "here", false);
	//__bis_SR_register(LPM4_bits + GIE);            // sleep, leave interrupts on

	while(1)
	{
		if(terminal_sent)
		{
			terminal_sent = false;
			UART_Print("Terminal sent: ", &data_from_terminal[0], false);
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
				UART_Print("Terminal received from BLE: ", &data_from_ble[0], false);
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
	}

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


// (BLE WAKE_SW) P1.6 enable
void turnOnBLE()  {  P1OUT |= BIT6; }
void turnOffBLE()  { P1OUT &= ~BIT6; }


// Example usage: UART_Print("Hello", &data, false);
void UART_Print(unsigned char *label, unsigned char *dataToVerify, bool isChar)
{

	bool print = true;
	while(print){ // Loop until null char is flagged
		while(!(UC1IFG & UCA1TXIFG)); // Wait for TX buffer to be ready for new data
		UCA1TXBUF = *label; //Write the character at the location specified py the pointer
		label++; //Increment the TxString pointer to point to the next character
		if(*label == '\0') { print = false; } //Terminate loop
	}

	print = true;
	while(print){ // Loop until null char is flagged
		while(!(UC1IFG & UCA1TXIFG)); // Wait for TX buffer to be ready for new data
		UCA1TXBUF = *dataToVerify; //Write the character at the location specified py the pointer
		dataToVerify++; //Increment the TxString pointer to point to the next character
		if(*dataToVerify == '\0' || isChar) { print = false; } //Terminate loop
	}

	//terminate line
	while(!(UC1IFG & UCA1TXIFG));
	UCA1TXBUF = '\n';
	while(!(UC1IFG & UCA1TXIFG));
	UCA1TXBUF = '\r';
	while(!(UC1IFG & UCA1TXIFG));

}

void initBLE()
{
//INIT START
	BCSCTL1 = CALBC1_1MHZ;                 // MCLK at 1MHz
	DCOCTL = CALDCO_1MHZ;

	P3SEL |= BIT4 | BIT5;                      // P3.4,5 = USCI_A0 TXD/RXD 1
	P3SEL |= 0xC0;                             // P3.6,7 = USCI_A1 TXD/RXD

	//115200 baud, clocked from 1MHz SMCLK for UART A0
	UCA0CTL1 |= UCSSEL_2 ;        // CLK = SMCL
	UCA0BR0 = 8;               // 115200 baud if SMCLK@1MHz
	UCA0BR1 = 0x00;
	UCA0MCTL = 6;//UCBRS_7;			  // Modulation UCBRSx = 7

	//115200 baud, clocked from 1MHz SMCLK for UART A1
	UCA1CTL1 |= UCSSEL_2 ;        // CLK = SMCLK
	UCA1BR0 = 8;               // 115200 baud if SMCLK@1MHz
	UCA1BR1 = 0x00;
	UCA1MCTL = 6;//UCBRS_7;			  // Modulation UCBRSx = 7


	//UART A0
	UC0IFG   &= ~(UCA0TXIFG | UCA0RXIFG);          // clear possible pending interrupts
	UC0IE	 &= ~UCA0TXIE;

	//UART A1
	UC1IFG   &= ~(UCA1TXIFG | UCA1RXIFG);          // clear possible pending interrupts
	UC1IE	 &= ~UCA1TXIE;

	UCA0CTL1 &= ~UCSWRST;                          // Initialize USCI state machine
	UCA1CTL1 &= ~UCSWRST;                          // Initialize USCI state machine
	UC0IE    |=  (UCA0RXIE  );//| UCA1TXIE);           // Enable USCI_A1 TX & RX interrupt
	UC1IE    |=  (UCA1RXIE  );//| UCA1TXIE);           // Enable USCI_A1 TX & RX interrupt

	//BLE WAKE_HW P1.0
	//BLE_RST P1.5
	//BLE WAKE_SW P1.6
	P1OUT = 0x00;
	P1DIR |= BIT0 | BIT5| BIT6 ;					// Set P1.0 to output direction

	turnOnBLE();

	int i;
	for(i=10;i>0;i--); //wait for cmd msg to transmit from ble

}
