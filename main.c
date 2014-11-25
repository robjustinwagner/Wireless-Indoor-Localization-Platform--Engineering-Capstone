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
#include <msp430f2619.h>
//#include <stdio.h>
#include "uart.c"
#include "ble.c"
#include "spi.c"
#include "eeprom.c"
#include "sensors.c"
#include "i2c.c"
#include "fm.c"
#include "auxiliary.c"

/* DEFINITIONS */


/* VARIABLES */

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    /* INITALIZATION ROUTINES */
    UART_Init();	//initalize UART for serial debug interface & BLE
    SPI_Init();		//initalize SPI for eeprom & sensors
    I2C_Init();		//initalize I2C for FM receivers



	return 0;
}
