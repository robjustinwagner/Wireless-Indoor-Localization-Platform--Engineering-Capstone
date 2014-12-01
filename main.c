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

	//int i; //for loop index

    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    /* INITALIZATION ROUTINES */
    CLK_Init();		//initalize CLK
    UART_Init();	//initalize UART for serial debug interface & BLE
    SPI_Init();		//initalize SPI for eeprom & sensors
    I2C_Init();		//initalize I2C for FM receivers

    BLE_Init();

    while(1)
    {
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
