/*
 * Gas Sensor Board
 *
 * Author:
 * Claira Safi
 */

///////////////////////////////////////////////////////////////////////////////

#include "board.h"

///////////////////////////////////////////////////////////////////////////////

#define gsbDefaultAddress	0x54

#define gsbConvFactor 		0.02f
#define gsbRefConvFactor 	0.02f
#define gsbHtrConvFactor 	0.02f

#define gsbCmdStatus 		0x5A
#define gsbCmdCatalytic1 	0x0F
#define gsbCmdCatalyticHtr1 0x5A
#define gsbCmdCatalyticCon1 0x00
#define gsbCmdCatalytic2 	0x11
#define gsbCmdCatalyticHtr2 0x13
#define gsbCmdCatalyticCon2 0x5A
#define gsbCmdElectro1 		0x13
#define gsbCmdElectro1Ref 	0x11
#define gsbCmdElectro1Con 	0xF0
#define gsbCmdElectro2 		0x15
#define gsbCmdElectro2Ref 	0xF0
#define gsbCmdElectro2Con 	0xF0
#define gsbCmdCon		 	0xF0

///////////////////////////////////////////////////////////////////////////////

//esbDevice_t esbDeviceGSB = {
//	.name = "Gas Sensor Board",
//	.updateFunction = updateGSB;
//	.initFunction = initGSB;
//};

I2C_TypeDef *gsbI2C;
uint8_t     gsbAddress;

uint16_t gsbRawElectro1;
uint16_t gsbRawElectro2;
uint16_t gsbRawCatalytic1;
uint16_t gsbRawCatalytic2;

uint16_t gsbRawElectroRef1;
uint16_t gsbRawElectroRef2;
uint16_t gsbRawCatalyticHtr1;
uint16_t gsbRawCatalyticHtr2;

uint16_t gsbElectro1PPM;
uint16_t gsbElectro2PPM;
uint16_t gsbCatalytic1PPM;
uint16_t gsbCatalytic2PPM;

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

void readElectrochemicalSensor1(void)
{
    uint8_t data[2];

    i2cRead(gsbI2C, gsbAddress, gsbCmdElectro1, 2, data);    // Request pressure read
    gsbElectro1PPM = data[0];

    //gsbRawElectro1 = data[0] | (data[1] << 8);
    //gsbElectro1PPM = calculatePPM(gsbRawElectro1);
}

///////////////////////////////////////////////////////////////////////////////
// Read Electrochemical Sensor Values
///////////////////////////////////////////////////////////////////////////////

void readElectrochemicalSensor2(void)
{
    uint8_t data[1];

    i2cRead(gsbI2C, gsbAddress, gsbCmdElectro2, 1, data);    // Request pressure read
    gsbElectro2PPM = data[0];

    //gsbRawElectro2 = data[0] | (data[1] << 8);
    //gsbElectro2PPM = calculatePPM(gsbRawElectro2);
}

///////////////////////////////////////////////////////////////////////////////
// Read Catalytic Sensor #1
///////////////////////////////////////////////////////////////////////////////

void readCatalyticSensor1(void)
{
    uint8_t data[1];

    i2cRead(gsbI2C, gsbAddress, gsbCmdCatalytic1, 1, data);
    gsbCatalytic1PPM = data[0];

    //gsbRawCatalytic1 = data[0] | (data[1] << 8);
    //gsbCatalytic1PPM = calculatePPM(gsbRawCatalytic1);
}

///////////////////////////////////////////////////////////////////////////////
// Read Catalytic Sensor #2
///////////////////////////////////////////////////////////////////////////////

void readCatalyticSensor2(void)
{
    uint8_t data[2];

    i2cRead(gsbI2C, gsbAddress, gsbCmdCatalytic2, 1, data);
    gsbCatalytic2PPM = data[0];

    //gsbRawCatalytic2 = data[0] | (data[1] << 8);
    //gsbCatalytic2PPM = calculatePPM(gsbRawCatalytic2);
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

void writeElectroRef1(float voltage)
{
    uint8_t data[1];
    data[0] = (unsigned)(voltage / gsbRefConvFactor);
    i2cWriteBuffer(gsbI2C, gsbAddress, gsbCmdElectro1Ref, 1, data);
}

///////////////////////////////////////////////////////////////////////////////
// Set Electrochemical Sensor Reference Voltage
///////////////////////////////////////////////////////////////////////////////

void writeElectroRef2(float voltage)
{
    uint8_t data[1];
    data[0] = (unsigned)(voltage / gsbRefConvFactor);
    i2cWriteBuffer(gsbI2C, gsbAddress, gsbCmdElectro2Ref, 1, data);
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
    gsbI2C = I2C2;
    gsbAddress = gsbDefaultAddress;

	readElectrochemicalSensor1();
	readElectrochemicalSensor2();
	readCatalyticSensor1();
	readCatalyticSensor2();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
