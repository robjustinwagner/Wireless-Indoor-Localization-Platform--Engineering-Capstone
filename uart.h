/*
 * WEARABLE INDOOR LOCALIZATION DEVICE
 * ECE 453, FALL 2014
 *
 * Calvin Hareng
 * Pedro Melgarejo
 * Robert Wagner
 *
 * UART.H
 */

#ifndef UART_H_
#define UART_H_

void UART_Init(); //initialization for UART A0 & A1

void DEBUG_UART_Print(unsigned char *label, unsigned char *dataToVerify, bool isChar); //print

#endif /* UART_H_ */
