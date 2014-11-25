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
#include <msp430f2619.h>
#include <stdint.h>

/* DEFINITIONS */
typedef int bool;
#define true 1
#define false 0

/* VARIABLES */
uint16_t WR_ADDR = 0x08;
uint16_t RD_ADDR = 0x08;
unsigned char WR_DATA = 'x';
unsigned char data;
int reading = 0; // boolean for state of eeprom (whether it is reading0
unsigned char CS; // chip select


int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer

	eeprom_Init();
	UART_Init();

	// Wait for slave to initialize
	int j;
	for(j = 0; j < 100; j++){}

	__bis_SR_register(GIE);

	CS = BIT6;
	spi_eeprom_release(); // CS High

	eepromWrite(WR_ADDR, WR_DATA);
	UART_Print("EEPROM Write done:", &WR_DATA, true);

	while(1){
		eepromRead(RD_ADDR); // stuck in loop until interrupt
		UART_Print("EEPROM Read:", &data, true);
		for(j = 0; j < 10000; j++){}
	}

	//__bis_SR_register(LPM0_bits + GIE);       // CPU off, enable interrupts
}

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

		/*P2OUT &= ~BIT6;
	UCB0TXBUF = MST_Data;                     // Send next value
	UCB0TXBUF = 0x00;                     // Transmit address
	int j;
	for(j = 0; j < 10; j++){}*/
		//P2OUT |= BIT6;

		/* Write - With WREN
		 *
		 * P2OUT &= ~BIT6;
		 * UCB0TXBUF = 0x06; // WREN
		 * P2OUT |= BIT6;
		 *
		 * for(i = 0; i < 10; i++){} // Wait
		 * P2OUT &= ~BIT6;
		 *  UCB0TXBUF = 0x02;                     // Write Op
		 *  UCB0TXBUF = 0x00;                     // Transmit address
		 *  UCB0TXBUF = 0xFF;						// Data
		 *  P2OUT |= BIT6;
		 *
		 */

		//__delay_cycles(50);                     // Add time between transmissions to

		reading = 0;
	}
}                                           // make sure slave can keep up

/*
 * SPI Init
 */

void spi_eeprom_select() {
	P2OUT &= ~BIT6;
	__delay_cycles(200);
}

void spi_eeprom_release() {
	P2OUT |= BIT6;
	__delay_cycles(200);
}

void initEEPROM(){

	int i;

	if (CALBC1_1MHZ==0xFF)					// If calibration constant erased
	{
		while(1);                               // do not load, trap CPU!!
	}
	BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
	DCOCTL = CALDCO_1MHZ;
	for(i=2100;i>0;i--);                      // Wait for DCO to stabilize.


	// Select SPI
	P2DIR |= BIT6; // SS_1_SEL - output (EEPROM)
	P3DIR |= BIT0; // SS_2_SEL - output
	P2OUT |= BIT6;
	P3OUT |= BIT0; // SS_2_SEL = high

	P3SEL |= 0x0E;                            // P3.3,2,1 option select
	UCB0CTL0 |= UCCKPL + UCMSB + UCMST + UCSYNC;  // 3-pin, 8-bit SPI master
	UCB0CTL1 |= UCSSEL_2;                     // SMCLK
	UCB0BR0 |= 0x02;                          // /2
	UCB0BR1 = 0;                              //
	IE2 &= (~UCB0TXIE | UCB0TXIE); // disable TX int
	UCB0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	//IE2 |= UCB0RXIE;                          // Enable USCI0 RX interrupt

}

unsigned char spi_eeprom_exchg(unsigned char dataOut)
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
void eepromWrite(uint16_t addr, unsigned char value){

	// Write - With WREN
	spi_eeprom_select();
	spi_eeprom_exchg(0x06);
	spi_eeprom_release(); // CS = high

	//UART_Print("EEPROM wren (recv):", &data, true);

	// Wait
	//int j;
	//for(j = 0; j < 50; j++){}

	// Write Value
	spi_eeprom_select(); // CS Low
	spi_eeprom_exchg(0x02);
	spi_eeprom_exchg(addr>>8);                     // Transmit address
	spi_eeprom_exchg(addr & 0xFF);
	spi_eeprom_exchg(value);						// Data
	spi_eeprom_release(); // CS high

	//UART_Print("EEPROM write (recv):", &data, true);

	// wait
	//for(j = 0; j < 50; j++){}

	// Write Disable
	spi_eeprom_select(); // CS = Low
	spi_eeprom_exchg(0x04); // WREN
	spi_eeprom_release(); // CS = high

	//UART_Print("EEPROM wdisable (recv):", &data, true);

	// wait
	//for(j = 0; j < 50; j++){}

	return;
}

/*
 * EEPROM read
 * addr - Hex value for address of EEPROM to write
 */
void eepromRead(uint16_t addr){

	// READ
	//reading = 1;
	spi_eeprom_select(); // ~CS Low
	spi_eeprom_exchg(0x03);                     // Transmit operation
	spi_eeprom_exchg(addr>>8);                     // Transmit address
	spi_eeprom_exchg(addr & 0xFF);
	data = spi_eeprom_exchg(0);
	spi_eeprom_release(); // ~CS high

	int j;
	//UART_Print("Read recv: ", &data, true);
	//while(reading);



	//for(j = 0; j < 10; j++){}
	//P2OUT |= BIT6;
}

void UART_Init(){
	BCSCTL1 = CALBC1_1MHZ;                 // MCLK at 16MHz
	DCOCTL = CALDCO_1MHZ;


	P3SEL |= 0xC0;                             // P3.6,7 = USCI_A0 TXD/RXD

	//115200 baud, clocked from 16MHz SMCLK
	UCA1CTL1 |= UCSSEL_2 ;        // CLK = SMCL
	UCA1BR0 = 8;               // 115200 baud if SMCLK@16MHz
	UCA1BR1 = 0x00;
	UCA1MCTL = 6; //UCBRS_7;			  // Modulation UCBRSx = 7

	UC1IFG   &= ~(UCA1TXIFG | UCA1RXIFG);          // clear possible pending interrupts
	UC1IE	 &= ~UCA1TXIE;
	UCA1CTL1 &= ~UCSWRST;                          // Initialize USCI state machine
	UC1IE    |=  (UCA1RXIE  );//| UCA1TXIE);           // Enable USCI_A1 TX & RX interrupt

	__bis_SR_register(GIE);       			// set 'general interrupt enable' bit
	__enable_interrupt();
}

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
