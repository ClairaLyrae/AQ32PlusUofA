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

#define mlxConvFactor 		0.02f

#define mlxDefaultAddress 	0x5A
#define mlxCmdTempAmbient 	0x06
#define mlxCmdTempObj1 		0x07
#define mlxCmdTempObj2 		0x08
#define mlxCmdReadFlags		0xF0

///////////////////////////////////////////////////////////////////////////////

//esbDevice_t esbDeviceMLX90614 = {
//	.name = "MLX90614 IR Thermometer",
//	.updateFunction = updateMLX90614;
//	.initFunction = initMLX90614;
//};

I2C_TypeDef *mlx90614I2C = I2C2;
uint8_t     mlx90614Address = 0x5A;

uint16_t mlxRawAmbTemp;
uint16_t mlxRawObjTemp;

float mlxObjTempC;
float mlxAmbTempC;

uint8_t mlxSensorReady = false;

///////////////////////////////////////////////////////////////////////////////
// Read Ambient Temperature
///////////////////////////////////////////////////////////////////////////////

void readAmbientTemperature(void)
{
    uint8_t data[2];

    i2cRead(mlx90614I2C, mlx90614Address, mlxCmdTempAmbient, 2, data);    // Request pressure read

    mlxRawAmbTemp = data[0];
    mlxRawAmbTemp |= (data[1] << 8);
    mlxAmbTempC = ((float)mlxRawAmbTemp*mlxConvFactor) - 273.0f;
}

///////////////////////////////////////////////////////////////////////////////
// Read Object Temperature
///////////////////////////////////////////////////////////////////////////////

void readObjectTemperature(void)
{
    uint8_t data[2];

    i2cRead(mlx90614I2C, mlx90614Address, mlxCmdTempObj1, 2, data);    // Request pressure read

    mlxRawObjTemp = data[0];
    mlxRawObjTemp |= (data[1] << 8);
    mlxObjTempC = ((float)mlxRawObjTemp*mlxConvFactor) - 273.0f;
}

///////////////////////////////////////////////////////////////////////////////
// Sensor Initialization
///////////////////////////////////////////////////////////////////////////////

void initMLX90614(void)
{
	//uint8_t data[1];

    mlx90614I2C = I2C2;
    mlx90614Address = 0x5A;

    // Read device flags
    //i2cRead(mlx90614I2C, mlx90614Address, mlxCmdReadFlags, 1, data);

    // Return POR status (if device is active or not)
    //mlxSensorReady = ((data[0] & 0x10) >> 4);
}

///////////////////////////////////////////////////////////////////////////////
// Read all values
///////////////////////////////////////////////////////////////////////////////

void updateMLX90614(void)
{
	readAmbientTemperature();
	readObjectTemperature();
}

///////////////////////////////////////////////////////////////////////////////
