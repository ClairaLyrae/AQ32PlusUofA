/*
 * Gas Sensor Board
 *
 * Author:
 * Claira Safi
 *
 * TODO:
 * Requires testing to see if it works. Also requires testing to make sure
 * I2C restart is being sent as it it necessary for the mlx sensor to
 * correctly interpret the read command
 */

///////////////////////////////////////////////////////////////////////////////

#include "board.h"

///////////////////////////////////////////////////////////////////////////////

#define gsbConvFactor 		0.02f
#define gsbRefConvFactor 	0.02f
#define gsbHtrConvFactor 	0.02f

#define gsbDefaultAddress 	0x5A
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
// Read Electrochemical Sensor Values
///////////////////////////////////////////////////////////////////////////////

void readElectrochemicalSensor(void)
{
    uint16_t data[2];

    i2cRead(gsbI2C, gsbAddress, gsbCmdElectro, 2, data);    // Request pressure read

    gsbRawElectro = data[0] | (data[1] << 8);
    gsbElectroPPM = calculatePPM(gsbRawElectro);
}

///////////////////////////////////////////////////////////////////////////////
// Read Catalytic Sensor #1
///////////////////////////////////////////////////////////////////////////////

void readCatalyticSensor1(void)
{
    uint16_t data[2];

    i2cRead(gsbI2C, gsbAddress, gsbCmdCatalytic1, 2, data);

    gsbRawCatalytic1 = data[0] | (data[1] << 8);
    gsbCatalytic1PPM = calculatePPM(gsbRawCatalytic1);
}

///////////////////////////////////////////////////////////////////////////////
// Read Catalytic Sensor #2
///////////////////////////////////////////////////////////////////////////////

void readCatalyticSensor2(void)
{
    uint16_t data[2];

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
// Calculate PPM Value From Raw
///////////////////////////////////////////////////////////////////////////////

float calculatePartsPerMillion(uint16_t rawVal)
{
	return rawVal*gsbConvFactor;
}

///////////////////////////////////////////////////////////////////////////////
// Sensor Initialization
///////////////////////////////////////////////////////////////////////////////

uint8_t initSensorGSB(void)
{
	uint8_t data[1];

    gsbI2C = I2C2;
    gsbAddress = 0x5A;

    // Read device status
    i2cRead(gsbI2C, gsbAddress, gsbCmdStatus, 1, data);

    // Return POR status (if device is active or not)
    gsbSensorReady = ((data[0] & 0x10) >> 4);

    return gsbSensorReady;
}

///////////////////////////////////////////////////////////////////////////////
