/*
 * WEARABLE INDOOR LOCALIZATION DEVICE
 * ECE 453, FALL 2014
 *
 * Calvin Hareng
 * Pedro Melgarejo
 * Robert Wagner
 *
 * MAIN.H
 */

#ifndef MAIN_H_
#define MAIN_H_

//general tasks
void General_Init();
unsigned char* floatToHex(float* val, int byteSize);
float** Make2DFloatMatrix(int arraySizeX, int arraySizeY);
void dumpToTXT(float* data[]);

/* VARIABLES */
extern int DEMO_MODE;
extern const int ROWS;
extern int COLS;

#endif /* MAIN_H_ */
