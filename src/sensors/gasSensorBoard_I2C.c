/*
 * Gas Sensor Board
 *
 * Author:
 * Claira Safi
 */

///////////////////////////////////////////////////////////////////////////////

#include "board.h"

///////////////////////////////////////////////////////////////////////////////

#define gsbDefaultAddress	0x33

#define gsbConvFactor 		0.02f
#define gsbRefConvFactor 	0.02f
#define gsbHtrConvFactor 	0.02f

#define gsbCmdStatus 		0x5A
#define gsbCmdCatalytic1 	0x5A
#define gsbCmdCatalyticHtr1 0x5A
#define gsbCmdCatalyticCon1 0x5A
#define gsbCmdCatalytic2 	0x5A
#define gsbCmdCatalyticHtr2 0x5A
#define gsbCmdCatalyticCon2 0x5A
#define gsbCmdElectro 		0xF0
#define gsbCmdElectroRef 	0xF0
#define gsbCmdElectroCon 	0xF0
#define gsbCmdCon		 	0xF0

///////////////////////////////////////////////////////////////////////////////

//esbDevice_t esbDeviceGSB = {
//	.name = "Gas Sensor Board",
//	.updateFunction = updateGSB;
//	.initFunction = initGSB;
//};

I2C_TypeDef *gsbI2C;
uint8_t     gsbAddress;

uint16_t gsbRawElectro;
uint16_t gsbRawCatalytic1;
uint16_t gsbRawCatalytic2;

uint16_t gsbRawElectroRef;
uint16_t gsbRawCatalyticHtr1;
uint16_t gsbRawCatalyticHtr2;

float gsbElectroPPM;
float gsbCatalytic1PPM;
float gsbCatalytic2PPM;

uint8_t gsbSensorReady = false;

///////////////////////////////////////////////////////////////////////////////
// Calculate PPM Value From Raw
///////////////////////////////////////////////////////////////////////////////

float calculatePPM(uint16_t rawVal)
{
	return rawVal*gsbConvFactor;
}

///////////////////////////////////////////////////////////////////////////////
// Read Electrochemical Sensor Values
///////////////////////////////////////////////////////////////////////////////

void readElectrochemicalSensor(void)
{
    uint8_t data[2];

    i2cRead(gsbI2C, gsbAddress, gsbCmdElectro, 2, data);    // Request pressure read

    gsbRawElectro = data[0] | (data[1] << 8);
    gsbElectroPPM = calculatePPM(gsbRawElectro);
}

///////////////////////////////////////////////////////////////////////////////
// Read Catalytic Sensor #1
///////////////////////////////////////////////////////////////////////////////

void readCatalyticSensor1(void)
{
    uint8_t data[2];

    i2cRead(gsbI2C, gsbAddress, gsbCmdCatalytic1, 2, data);

    gsbRawCatalytic1 = data[0] | (data[1] << 8);
    gsbCatalytic1PPM = calculatePPM(gsbRawCatalytic1);
}

///////////////////////////////////////////////////////////////////////////////
// Read Catalytic Sensor #2
///////////////////////////////////////////////////////////////////////////////

void readCatalyticSensor2(void)
{
    uint8_t data[2];

    i2cRead(gsbI2C, gsbAddress, gsbCmdCatalytic2, 2, data);

    gsbRawCatalytic2 = data[0] | (data[1] << 8);
    gsbCatalytic2PPM = calculatePPM(gsbRawCatalytic2);
}

///////////////////////////////////////////////////////////////////////////////
// Set Catalytic Sensor #1 Heater Voltage
///////////////////////////////////////////////////////////////////////////////

void writeCatalyticHtr1(float voltage)
{
    uint8_t data[1];
    data[0] = (unsigned)(voltage / gsbHtrConvFactor);
    i2cWriteBuffer(gsbI2C, gsbAddress, gsbCmdCatalyticHtr1, 1, data);
}

///////////////////////////////////////////////////////////////////////////////
// Set Catalytic Sensor #2 Heater Voltage
///////////////////////////////////////////////////////////////////////////////

void writeCatalyticHtr2(float voltage)
{
    uint8_t data[1];
    data[0] = (unsigned)(voltage / gsbHtrConvFactor);
    i2cWriteBuffer(gsbI2C, gsbAddress, gsbCmdCatalyticHtr2, 1, data);
}


///////////////////////////////////////////////////////////////////////////////
// Set Electrochemical Sensor Reference Voltage
///////////////////////////////////////////////////////////////////////////////

void writeElectroRef(float voltage)
{
    uint8_t data[1];
    data[0] = (unsigned)(voltage / gsbRefConvFactor);
    i2cWriteBuffer(gsbI2C, gsbAddress, gsbCmdElectroRef, 1, data);
}

///////////////////////////////////////////////////////////////////////////////
// Sensor Initialization
///////////////////////////////////////////////////////////////////////////////

uint8_t initGSB(void)
{
	uint8_t data[1];

    gsbI2C = I2C2;
    gsbAddress = gsbDefaultAddress;

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Update Function
///////////////////////////////////////////////////////////////////////////////

uint8_t updateGSB(void) {
	readElectrochemicalSensor();
	readCatalyticSensor1();
	readCatalyticSensor2();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
