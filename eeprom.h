/*
 * WEARABLE INDOOR LOCALIZATION DEVICE
 * ECE 453, FALL 2014
 *
 * Calvin Hareng
 * Pedro Melgarejo
 * Robert Wagner
 *
 * EEPROM.H
 */

#ifndef EEPROM_H_
#define EEPROM_H_

//eeprom functions
void eepromRead(uint16_t addr);
void eepromWrite(uint16_t addr, unsigned char value);
void UART_Print(unsigned char *label, unsigned char *dataToVerify, bool isChar);
void UART_Init();
void spi_eeprom_release(void);

#endif /* EEPROM_H_ */
