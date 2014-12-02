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
static unsigned int j; //for loop index
static unsigned int aux_j; //inner/auxiliary for loop index
const static bool COLLECT_SENSOR_DATA = false;
const static bool COLLECT_FM_DATA = false;
const int ROWS = COLLECTION_DURATION*SAMPLES_PER_DURATION;
int COLS;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
	//data matrix setup
	if(COLLECT_SENSOR_DATA) COLS = 3;
	else if(COLLECT_FM_DATA) COLS = 6;

    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    /* INITALIZATION ROUTINES */
    CLK_Init();		//initalize CLK

    UART_Init();	//initalize UART for serial debug interface & BLE
    SPI_Init();		//initalize SPI for eeprom & sensors
    I2C_Init();		//initalize I2C for FM receivers

    BLE_Init();		//ble

	MPU_Init(0, 0);	//sensors
	whoami();		//

	if(COLLECT_SENSOR_DATA)
	{
		float sensorData[ROWS][COLS];
		collectSensorData(sensorData);
		dumpToTXT(sensorData);
	}
	else if(COLLECT_FM_DATA)
	{
		float fmData[ROWS][COLS];
		collectFMData(fmData);
		dumpToTXT(fmData);
	}
	else
	{
		while(1)	//main loop
		{
        	read_acc(); 	// Read Accelerometer Values
    		BLE_setPublicChar("000D", floatToHex(&accelerometer_data[0], 8), 8);
    	}
    }

	//return 1;
}

void CLK_Init()
{
	if (CALBC1_1MHZ==0xFF) while(1); // If calibration constant erased, do not load, trap CPU!!

	BCSCTL1 = CALBC1_1MHZ;           // MCLK at 1MHz
	DCOCTL = CALDCO_1MHZ;

	for(j=2100;j>0;j--);             // Wait for DCO to stabilize.
}

/*
 * converts a floating number to a hex
 * returns a character pointer to the first value in the (char) hex array
 */
unsigned char* floatToHex(float* val, int byteSize)
{
	unsigned char* hexVal = (unsigned char*) malloc(byteSize);
	unsigned char* temp = (unsigned char*) val;

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

/*
 * dumps collected {sensor, fm} data into text file,
 * one line per data entry,
 * float values are delimited by a comma
 * e.g.
 * 			DATA			==>			TXT
 * [ ..... 	..... 	..... ]	    ....., ....., .....
 * [ 3.141  0.415   2.134 ] ==> 3.141, 0.415, 2.134
 * [ ..... 	..... 	..... ] 	....., ....., .....
 */
void dumpToTXT(float data[ROWS][COLS])
{
	//open file
	FILE *f = fopen("data.txt", "w");
	if (f == NULL)
	{
	    printf("Error opening/creating dump file! (data.txt)\n");
	    exit(1);
	}
	//write to file
	for(j = 0; j < ROWS; j++)
	{
		for(aux_j = 0; aux_j < COLS; aux_j++)
		{
			if(aux_j = COLS-1) fprintf(f, "%f\n", data[j][aux_j]);
			else fprintf(f, "%f,", data[j][aux_j]);
		}
	}

	fclose(f);	//close file resource
}
