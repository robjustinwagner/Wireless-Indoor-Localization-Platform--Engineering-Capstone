/*
 * WEARABLE INDOOR LOCALIZATION DEVICE
 * ECE 453, FALL 2014
 *
 * Calvin Hareng
 * Pedro Melgarejo
 * Robert Wagner
 *
 * FM.C
 */

/* INCLUDES */
#include "gen_lib.h"
#include "fm.h"
#include "main.h"

/* DEFINITIONS */


/* VARIABLES */


/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void initFM()
{

}

void collectFMData(float fmData[ROWS][COLS])
{
	int wait_time = 1000000/SAMPLES_PER_DURATION;	// 1MHz CLK

	/* BEGIN MODIFIED VERSION OF read_acc() TO POPULATE fmData[][] */
		/*
		uint8_t response[6];
	    int16_t bit_data;
	    float data_temp;
		*/

		unsigned int r;
		unsigned int c;
	    for(r = 0; r < ROWS; r++) {
	    	/* >>>>>> INSERT FM READING FUNCTION HERE <<<<<< */
		    for(c = 0; c < COLS; c++) {
		    	/*
		        bit_data=((int16_t)response[c*2]<<8)|response[c*2+1];
		        data_temp=(float)bit_data;
		        fmData[r][c]=data_temp/acc_divider;
		        */
		    }
	    	for(c=wait_time;c>0;c--);	//wait in between samples
	    }
	/* END MODIFIED VERSION OF read_acc() */
}
