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

    BLE_Init();		//ble

	MPU_Init(0, 0);	//sensors
	whoami();		//

    while(1)
    {
    	read_acc(); // Read Accelerometer Values
		BLE_setPublicChar("000D", floatToHex(&accelerometer_data[0], 8), 8);
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

/*
 * converts a floating number to a hex
 * returns a character pointer to the first value in the (char) hex array
 */
unsigned char* floatToHex(float* val, int byteSize)
{
	unsigned char* hexVal = (unsigned char*) malloc(byteSize);
	unsigned char* temp = (unsigned char*) val;

	unsigned int j;
	for(j = 0; j < 8; j++)
	{
		if((j % 2) == 0) //0
		{
			hexVal[8-j-1] = (temp[j/2] >> 4) & 0x0F;
		}
		else if((j % 2) == 1) //1
		{
			hexVal[8-j-1] = (temp[j/2]) & 0x0F;
		}

		if(hexVal[8-j-1] < 10)
		{
			hexVal[8-j-1] = hexVal[8-j-1] + 48;
		}
		else
		{
			hexVal[8-j-1] = hexVal[8-j-1] + 55;
		}
	}

	//reverse endian
	for(j = 0; j < 4; j++)
	{
		char temp = hexVal[(2*j)+1];
		hexVal[(2*j)+1] = hexVal[(2*j)];
		hexVal[(2*j)] = temp;
	}

	return &hexVal[0];
}
