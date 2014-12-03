/*
 * WEARABLE INDOOR LOCALIZATION DEVICE
 * ECE 453, FALL 2014
 *
 * Calvin Hareng
 * Pedro Melgarejo
 * Robert Wagner
 *
 * BLE.H
 */

#ifndef BLE_H_
#define BLE_H_

//variables
extern unsigned char data_from_terminal[];
extern unsigned char data_from_ble[];
extern bool terminal_received;
extern bool terminal_sent;

//ble init
void BLE_Init();

//ble functions
void BLE_turnOn();
void BLE_turnOff();
void BLE_toggleEcho();
void BLE_listServices();
void BLE_factoryReset();
void BLE_enableServices();
void BLE_enableAutoAdvert();
void BLE_Reboot();
void BLE_startAdvertisement();
void BLE_stopAdvertisement();
void BLE_changeNameTo(unsigned char *label);
void BLE_setPublicChar(unsigned char *charID, unsigned char *charVal, int charValLength, unsigned char* type);

//debug
void DEBUG_BLE_Echo_To_Terminal();

#endif /* BLE_H_ */
