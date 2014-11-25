/*
 * WEARABLE INDOOR LOCALIZATION DEVICE
 * ECE 453, FALL 2014
 *
 * Calvin Hareng
 * Pedro Melgarejo
 * Robert Wagner
 *
 * MAIN.C
 */

/* INCLUDES */
#include "gen_lib.h"
#include "main.h"
#include "uart.h"
#include "ble.h"
#include "spi.h"
#include "eeprom.h"
#include "sensors.h"
#include "i2c.h"
#include "fm.h"
#include "auxiliary.h"

/* DEFINITIONS */


/* VARIABLES */


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    /* INITALIZATION ROUTINES */
    CLK_Init();		//initalize CLK
    UART_Init();	//initalize UART for serial debug interface & BLE
    SPI_Init();		//initalize SPI for eeprom & sensors
    I2C_Init();		//initalize I2C for FM receivers

    //BLE startup
	BLE_turnOn();
	int i;
	for(i=10;i>0;i--); 								//wait for cmd msg to transmit from ble
    BLE_toggleEcho();
    BLE_startAdvertisement();

    int i;
    	for(i=11;i>0;i--); 								//wait for cmd msg to transmit from ble

    while(1)
    {

    	/* BLE
    	 * SN, WearableIndoorLocalizationDevice //set name
    	 * issue "+" command to turn on echo
    	 * issue "A" to start advertisement
    	 * launch app.
    	 * configure app to be "central" device and start active scan for BLE
    	 * pair
    	 * @O to output analog signal
    	 * @I to input analog signal
    	 * |O to set PIO's output
    	 * |I to get PIO's inpit
    	 */
    	DEBUG_BLE_Echo_To_Terminal();

    }

	//return 1;
}

void CLK_Init()
{
	if (CALBC1_1MHZ==0xFF) while(1); 	// If calibration constant erased, do not load, trap CPU!!

	BCSCTL1 = CALBC1_1MHZ;                 		// MCLK at 1MHz
	DCOCTL = CALDCO_1MHZ;

	int i;
	for(i=2100;i>0;i--);                      	// Wait for DCO to stabilize.
}
