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
void EEPROM_Select();
void EEPROM_Release();
unsigned char EEPROM_Exchange(unsigned char dataOut);
void EEPROM_Read(uint16_t addr);
void EEPROM_Write(uint16_t addr, unsigned char value);

#endif /* EEPROM_H_ */
