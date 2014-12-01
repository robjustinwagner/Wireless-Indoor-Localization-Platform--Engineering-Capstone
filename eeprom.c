/*
 * WEARABLE INDOOR LOCALIZATION DEVICE
 * ECE 453, FALL 2014
 *
 * Calvin Hareng
 * Pedro Melgarejo
 * Robert Wagner
 *
 * EEPROM.C
 */

/* INCLUDES */
#include "gen_lib.h"
#include "eeprom.h"
#include "uart.h"

/* DEFINITIONS */

/* VARIABLES */
uint16_t WR_ADDR = 0x08;
uint16_t RD_ADDR = 0x08;
unsigned char WR_DATA = 'x';
unsigned char data;
int reading = 0; // boolean for state of eeprom (whether it is reading0
unsigned const char CS = BIT6; // chip select

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int eeprom_crap()
{

	EEPROM_Release(); // CS High

	EEPROM_Write(WR_ADDR, WR_DATA);
	DEBUG_UART_Print("EEPROM Write done: ", &WR_DATA, true);

	while(1){
		EEPROM_Read(RD_ADDR); // stuck in loop until interrupt
		DEBUG_UART_Print("EEPROM Read: ", &data, true);
		int j;
		for(j = 0; j < 10000; j++){}
	}

	//__bis_SR_register(LPM0_bits + GIE);       // CPU off, enable interrupts
}
/*
// Test for valid RX and TX character
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCIB0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCIB0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
	P2OUT |= BIT6;
	data = UCB0RXBUF;
	if(reading == 1){
		volatile unsigned int i;

		while (!(IFG2 & UCB0TXIFG));              // USCI_A0 TX buffer ready?

		if (data == WR_DATA){                // Test for correct character RX'd
		}
		else{
		}

	//P2OUT &= ~BIT6;
	//UCB0TXBUF = MST_Data;                     // Send next value
	//UCB0TXBUF = 0x00;                     // Transmit address
	//int j;
	//for(j = 0; j < 10; j++){}
		//P2OUT |= BIT6;

		//__delay_cycles(50);                     // Add time between transmissions to

		reading = 0;
	}
}                                           // make sure slave can keep up
*/

/*
 * SPI Init
 */

void EEPROM_Select() {
	P2OUT &= ~CS;
	__delay_cycles(200);
}

void EEPROM_Release() {
	P2OUT |= CS;
	__delay_cycles(200);
}

unsigned char EEPROM_Exchange(unsigned char dataOut)
{
	unsigned char dataIn = 0;

	while (!(IFG2 & UCB0TXIFG));
	UCB0TXBUF = dataOut;

	while (!(IFG2 & UCB0RXIFG));
	dataIn = UCB0RXBUF;

	return dataIn;
}

/*
 * EEPROM Write
 * addr - Hex value for address of EEPROM to write
 * value - Hex value of DATA to write to EEPROM
 */
void EEPROM_Write(uint16_t addr, unsigned char value)
{

	// Write - With WREN
	EEPROM_Select();
	EEPROM_Exchange(0x06);
	EEPROM_Release(); // CS = high

	//UART_Print("EEPROM wren (recv):", &data, true);

	// Wait
	//int j;
	//for(j = 0; j < 50; j++){}

	// Write Value
	EEPROM_Select(); // CS Low
	EEPROM_Exchange(0x02);
	EEPROM_Exchange(addr>>8);                     // Transmit address
	EEPROM_Exchange(addr & 0xFF);
	EEPROM_Exchange(value);						// Data
	EEPROM_Release(); // CS high

	//UART_Print("EEPROM write (recv):", &data, true);

	// wait
	//for(j = 0; j < 50; j++){}

	// Write Disable
	EEPROM_Select(); // CS = Low
	EEPROM_Exchange(0x04); // WREN
	EEPROM_Release(); // CS = high

	//UART_Print("EEPROM wdisable (recv):", &data, true);

	// wait
	//for(j = 0; j < 50; j++){}

	return;
}

/*
 * EEPROM read
 * addr - Hex value for address of EEPROM to write
 */
void EEPROM_Read(uint16_t addr)
{

	// READ
	//reading = 1;
	EEPROM_Select(); // ~CS Low
	EEPROM_Exchange(0x03);                     // Transmit operation
	EEPROM_Exchange(addr>>8);                     // Transmit address
	EEPROM_Exchange(addr & 0xFF);
	data = EEPROM_Exchange(0);
	EEPROM_Release(); // ~CS high

	//DEBUG_UART_Print("Read recv: ", &data, true);
	//while(reading);

	//int j;
	//for(j = 0; j < 10; j++){}
	//P2OUT |= BIT6;
}
