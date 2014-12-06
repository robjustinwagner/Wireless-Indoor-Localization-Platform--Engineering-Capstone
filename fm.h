/*
 * WEARABLE INDOOR LOCALIZATION DEVICE
 * ECE 453, FALL 2014
 *
 * Calvin Hareng
 * Pedro Melgarejo
 * Robert Wagner
 *
 * FM.H
 */

#ifndef FM_H_
#define FM_H_

/* VARIABLES */

void FM_Init(); //initialize fm

//fm functions
void FM_Read();
void FM_CollectData(float* fmData[]);

#endif /* FM_H_ */
