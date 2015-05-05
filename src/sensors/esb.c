/*
 * ESB (Enviromental Sensor Bus) Handler
 *
 * Author:
 * Claira Safi
 */

///////////////////////////////////////////////////////////////////////////////

#include "board.h"

uint8_t esbDeviceIndex;

void initESB(void) {
	//initMLX90614();
	//initGSB();
	//initPDB();
}

void updateESB(void) {
	updateMLX90614();
	updateGSB();
//	if(ESB_NUM_DEVICES == 0)
//		return;
//	esbDeviceIndex++;
//	if(esbDeviceIndex >= ESB_NUM_DEVICES)
//		esbDeviceIndex = 0;
//	switch(esbDeviceIndex) {
//		case ESB_MLX90614 :
//			updateMLX90614();
//			break;
//		case ESB_GSB :
//			//updateGSB();
//			break;
//		case ESB_PDB :
//			//updatePDB();
//			break;
//		default: break;
//	}
}


/////////////////////////////////////////////////////////////////////////////////
//
//esbDevice_t *esbDevices[ESB_MAX_DEVICES];
//uint8_t	esbIndex = 0;
//uint8_t	esbNumDevices = 0;
//
////////////////////////////////////////////////////////////////////////////////
////	Registers an ESB device to the handler, initializes it. Returns true
////	If device passed initialization and was added to the list.
/////////////////////////////////////////////////////////////////////////////////
//
//uint8_t registerESBDevice(esbDevice_t* device) {
//	uint8_t success = false;
//	if(esbNumDevices >= ESB_MAX_DEVICES)
//		return false;
//	device->status = device->initFunction();
//
//	if(!device->status)
//		return false;
//	esbNumDevices++;
//	esbDevices[esbNumDevices] = device;
//	return true;
//}
//
////////////////////////////////////////////////////////////////////////////////
////	Initializes the ESB handler & all ESB devices
/////////////////////////////////////////////////////////////////////////////////
//
//void initESB(void) {
//	uint8_t i;
//	esbI2C = I2C2;
//	for(i = 0; i < esbNumDevices; i++)
//		esbDevices[i]->status = esbDevices[i]->initFunction();
//}
//
/////////////////////////////////////////////////////////////////////////////////
////	Updates the next device on the ESB bus. Returns index of device updated.
////	Each call to this updates a single ESB device, runnig through the full
////	list in a cyclic fashion
/////////////////////////////////////////////////////////////////////////////////
//
//uint8_t updateESB(void) {
//	uint8_t devicesChecked = 0;
//	do {
//		devicesChecked++;
//		esbIndex++;
//		if(esbIndex >= ESB_MAX_DEVICES)
//			esbIndex = 0;
//		if(devicesChecked >= esbNumDevices)
//			return 0;
//	} while(!(esbDevices[esbIndex]->status));
//	esbDevices[esbIndex]->status = esbDevices[esbIndex]->updateFunction();
//	return esbIndex;
//}
//
/////////////////////////////////////////////////////////////////////////////////
