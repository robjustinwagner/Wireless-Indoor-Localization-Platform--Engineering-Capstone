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

const static bool COLLECT_SENSOR_DATA = true;
const static bool COLLECT_FM_DATA = false;
const int ROWS = COLLECTION_DURATION*SAMPLES_PER_DURATION;
int COLS;

// static float x_threshold = 0.5;
//const static float y_threshold = 0.5;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(void)
{
	// data-matrix setup
	if(COLLECT_SENSOR_DATA == 1) COLS = 3;
	else if(COLLECT_FM_DATA == 1) COLS = 6;

	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	/* INITALIZATION ROUTINES */
	CLK_Init();		//initalize CLK

	UART_Init();	//initalize UART for serial debug interface & BLE
	SPI_Init();		//initalize SPI for eeprom & sensors
	I2C_Init();		//initalize I2C for FM receivers

	BLE_turnOff();

	MPU_Init(0, 0);	//sensors
	//whoami();		//

	if(COLLECT_SENSOR_DATA)
	{
		float** sensorData = Make2DFloatMatrix(ROWS, COLS);
		collectSensorData(sensorData);
		dumpToTXT(sensorData);
		for (j = 0; j < ROWS; j++) free(sensorData[j]); //free memory
		free(sensorData);							   //
	}
	else if(COLLECT_FM_DATA)
	{
		float** fmData = Make2DFloatMatrix(ROWS, COLS);
		collectFMData(fmData);
		dumpToTXT(fmData);
		for (j = 0; j < ROWS; j++) free(fmData[j]); //free memory
		free(fmData);								//
	}
	else
	{

		BLE_Init();		//ble

		while(1)	//main loop
		{
			read_acc(); 	// Read Accelerometer Values
			for(j = 5000; j > 0; j--);
			//BLE_listServices();
			//unsigned char* temp = &accelerometer_data[0];

			/* Send X,Y,& Z MPU Values (for loop used to space data being sent from BLE) */
			BLE_setPublicChar("0018", floatToHex(&accelerometer_data[0], 8), 8, "0A");
			for(j = 5000; j > 0; j--);
			BLE_setPublicChar("0018", floatToHex(&accelerometer_data[1], 8), 8, "0B");
			for(j = 5000; j > 0; j--);
			BLE_setPublicChar("0018", floatToHex(&accelerometer_data[2], 8), 8, "0C");
			for(j = 5000; j > 0; j--);

			/*
			 ****************TARGETED FALL DETECTION ROUTIENE**************
			 *
			 * REMOVE "BLE_INIT(); from main init routiene
			 *
			 *
			 * read_acc();	//constantly read accelerometer values
			 * if(accelerometer_data[0] > x_threshold && accelerometer_data[2] > z_threshold)	// **FALL DETECTED**
			 * {
			 *  read_fm();
			 *	BLE_Init();		//wake up, initialize ble
			 *	BLE_setPublicChar("000D", floatToHex(&fm_data[0], 8), 8);
			 *	BLE_turnOff();	//turn off ble to optimize battery life
			 * }
			 *
			 */
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

float** Make2DFloatMatrix(int arraySizeX, int arraySizeY)
{
	float** theArray;
	theArray = (float**) malloc(arraySizeX*sizeof(float*));
	for (j = 0; j < arraySizeX; j++)
	{
		theArray[j] = (float*) malloc(arraySizeY*sizeof(float));
	}
	return theArray;
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
// I BELIEVE THIS CANT BE DONE ON THE DEVICE
void dumpToTXT(float* data[])
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
			DEBUG_UART_Print("data: ", floatToHex(&data[j][aux_j],8), true);
			//if(aux_j == COLS-1) fprintf(f, "%f\n", data[j][aux_j]);
			//if(aux_j == COLS-1) fputs("This is testing for fputs...\n", f);
			//else fprintf(f, "%f,", data[j][aux_j]);
			//else fputs("This is testing for fputs...", f);
		}
	}

	fclose(f);	//close file resource
}
