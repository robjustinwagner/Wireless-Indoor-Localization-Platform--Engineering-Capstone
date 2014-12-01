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
#include "gen_lib.h"
#include "ble.h"
#include "uart.h"

/* DEFINITIONS */

/* VARIABLES */
unsigned char data_from_terminal[8*20];
unsigned char data_from_ble[8*20];
bool terminal_received = false;
bool terminal_sent = false;
unsigned char data;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void BLE_Init()
{
	//BLE startup
	BLE_turnOff();
	BLE_turnOn();							//wait for cmd msg to transmit from ble
	//BLE_toggleEcho();
	BLE_startAdvertisement();
	BLE_changeNameTo("WILDevice");
	BLE_setPublicChar("000D", "00111100", 8);
}


// (BLE WAKE_SW) P1.6 enable
void BLE_turnOn()
{
	int i;
	for(i=10;i>0;i--); //wait for cmd msg to transmit from ble
	P1OUT |= BIT6;
}
void BLE_turnOff()
{
	int i;
	for(i=10;i>0;i--); //wait for cmd msg to transmit from ble
	P1OUT &= ~BIT6;
}
void BLE_toggleEcho()
{
	terminal_sent = true;
	data_from_terminal[0] = '+';	//echo command
	data_from_terminal[1] = 0x0D;	//enter

	DEBUG_BLE_Echo_To_Terminal();	//force trigger
}
void BLE_startAdvertisement()
{
	terminal_sent = true;
	data_from_terminal[0] = 'A';	//echo command
	data_from_terminal[1] = 0x0D;	//enter

	DEBUG_BLE_Echo_To_Terminal();	//force trigger
}
void BLE_stopAdvertisement()
{
	terminal_sent = true;
	data_from_terminal[0] = 'Y';	//echo command
	data_from_terminal[1] = 0x0D;	//enter

	DEBUG_BLE_Echo_To_Terminal();	//force trigger
}
void BLE_changeNameTo(unsigned char *label)
{
	terminal_sent = true;

	data_from_terminal[0] = 'S';
	data_from_terminal[1] = '-';
	data_from_terminal[2] = ',';

	bool print = true;
	unsigned int index = 3;
	while(print) { 							// Loop until null char is flagged
		data_from_terminal[index] = *label;	//Write the character at the location specified by the pointer
		label++; 							//Increment the TxString pointer to point to the next character
		index++;							//Increment the data_from_terminal to point to the next empty array index
		if(*label == '\0')print = false;	//Terminate loop
	}
	data_from_terminal[index] = 0x0D;		//enter to end cmd

	DEBUG_BLE_Echo_To_Terminal();		//force trigger
}
void BLE_setPublicChar(unsigned char *charID, unsigned char *charVal, int charValLength)
{
	terminal_sent = true;

	data_from_terminal[0] = 'S';
	data_from_terminal[1] = 'H';
	data_from_terminal[2] = 'W';
	data_from_terminal[3] = ',';

	unsigned int index = 4;
	while(index < 8)
	{
		data_from_terminal[index] = *charID;
		charID++;
		index++;
	}
	while(index < (charValLength+8)) { 			// Loop until null char is flagged
		data_from_terminal[index] = *charVal;	//Write the character at the location specified by the pointer
		charVal++; 								//Increment the TxString pointer to point to the next character
		index++;								//Increment the data_from_terminal to point to the next empty array index
	}
	data_from_terminal[index] = 0x0D;			//enter to end cmd

	DEBUG_BLE_Echo_To_Terminal();		//force trigger
}

/* EXAMPLE USAGE:
 * call this method in an infinite loop in order to manage the ble
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
				while(!(UC0IFG & UCA0TXIFG)); //wait for last transmit
				UCA0TXBUF = data_from_terminal[index];
				index++;
			}
			while(!(UC0IFG & UCA0TXIFG)); //wait for last transmit
			UCA0TXBUF = data_from_terminal[index];
		}
		if(terminal_received)
		{
			{
				terminal_received = false;
				DEBUG_UART_Print("Terminal received from BLE: ", &data_from_ble[0], false);
				/*
				unsigned int index = 0;
				while(data_from_ble[index] != 0x0D)
				{
					while(!(UC1IFG & UCA1TXIFG)); //wait for last transmit
					UCA1TXBUF = data_from_ble[index];
					index++;
				}
				while(!(UC1IFG & UCA1TXIFG)); //wait for last transmit
				UCA1TXBUF = data_from_ble[index];
				*/
			}
		}
	//}

}
