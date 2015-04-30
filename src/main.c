/*
  October 2012

  aq32Plus Rev -

  Copyright (c) 2012 John Ihlein.  All rights reserved.

  Open Source STM32 Based Multicopter Controller Software

  Includes code and/or ideas from:

  1)AeroQuad
  2)BaseFlight
  3)CH Robotics
  4)MultiWii
  5)S.O.H. Madgwick
  6)UAVX

  Designed to run on the AQ32 Flight Control Board

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

///////////////////////////////////////////////////////////////////////////////

#include "board.h"


///////////////////////////////////////////////////////////////////////////////

__attribute__((__section__(".eeprom"), used)) const int8_t eepromArray[16384];

eepromConfig_t eepromConfig;
uint8_t        execUpCount = 0;
sensors_t      sensors;
heading_t      heading;
gps_t          gps;
homeData_t     homeData;
uint16_t       timerValue;

///////////////////////////////////////////////////////////////////////////////

int main(void)
{
    uint32_t currentTime;

    // Matrix variables
	arm_matrix_instance_f32 a;
	arm_matrix_instance_f32 b;
	arm_matrix_instance_f32 x;

	// Initialize system
    systemReady = false;
    systemInit();
    systemReady = true;

    // Call main program start event
    evrPush(EVR_StartingMain, 0);

    #ifdef _DTIMING

        #define LA1_ENABLE       GPIO_SetBits(GPIOA,   GPIO_Pin_4)
        #define LA1_DISABLE      GPIO_ResetBits(GPIOA, GPIO_Pin_4)
        #define LA4_ENABLE       GPIO_SetBits(GPIOC,   GPIO_Pin_5)
        #define LA4_DISABLE      GPIO_ResetBits(GPIOC, GPIO_Pin_5)
        #define LA2_ENABLE       GPIO_SetBits(GPIOC,   GPIO_Pin_2)
        #define LA2_DISABLE      GPIO_ResetBits(GPIOC, GPIO_Pin_2)
        #define LA3_ENABLE       GPIO_SetBits(GPIOC,   GPIO_Pin_3)
        #define LA3_DISABLE      GPIO_ResetBits(GPIOC, GPIO_Pin_3)

        GPIO_InitTypeDef GPIO_InitStructure;

        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,   ENABLE);
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,   ENABLE);
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,   ENABLE);

        GPIO_StructInit(&GPIO_InitStructure);

        // Init pins
        GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

        GPIO_Init(GPIOA, &GPIO_InitStructure);

        // Init pins
        GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1;
        //GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
        //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        //GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

        GPIO_Init(GPIOB, &GPIO_InitStructure);

        // Init pins
        GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5;
        //GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
        //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        //GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

        GPIO_Init(GPIOC, &GPIO_InitStructure);

        // PB0_DISABLE;
        LA4_DISABLE;
        LA2_DISABLE;
        LA3_DISABLE;
        LA1_DISABLE;

    #endif

    // Main loop
    while (1)
    {

        checkUsbActive(false);	// Check CLI/USB connection
        evrCheck();	// Update event listeners

        ///////////////////////////////
        //	50 Hz Execution Tick
        ///////////////////////////////

        if (frame_50Hz)
        {
            #ifdef _DTIMING
                LA2_ENABLE;
            #endif
            frame_50Hz = false;

            // Update delta time for real-time calculations
            currentTime      = micros();
            deltaTime50Hz    = currentTime - previous50HzTime;
            previous50HzTime = currentTime;

            // Process RC Receiver commands
            processFlightCommands();

            // Update altitude/temperature measurements
            aglRead();
            if (newTemperatureReading && newPressureReading)
            {
                d1Value = d1.value;
                d2Value = d2.value;

                calculateTemperature();
                calculatePressureAltitude();

                newTemperatureReading = false;
                newPressureReading    = false;
            }
            sensors.pressureAlt50Hz = firstOrderFilter(sensors.pressureAlt50Hz, &firstOrderFilters[PRESSURE_ALT_LOWPASS]);

            // Measure RSSI (Received Signal Strength Indication) of RC receiver (Not supported by SPEKTRUM)
            if(eepromConfig.receiverType != SPEKTRUM)
            	rssiMeasure();

            // Update MAX7456 On-Screen Display IC (Not used)
            //updateMax7456(currentTime, 0);

            // Calculate execution tick process time
            executionTime50Hz = micros() - currentTime;

            #ifdef _DTIMING
                LA2_DISABLE;
            #endif
        }

        ///////////////////////////////
        //	10 Hz Execution Tick
        ///////////////////////////////

        if (frame_10Hz)
        {
            #ifdef _DTIMING
                LA4_ENABLE;
            #endif
            frame_10Hz = false;

            // Update delta time for real-time calculations
            currentTime      = micros();
            deltaTime10Hz    = currentTime - previous10HzTime;
            previous10HzTime = currentTime;

            // Update magnetometer data
            if (newMagData == true)
            {
            	// Apply biases
			    nonRotatedMagData[XAXIS] = (rawMag[XAXIS].value * magScaleFactor[XAXIS]) - eepromConfig.magBias[XAXIS + eepromConfig.externalHMC5883];
			    nonRotatedMagData[YAXIS] = (rawMag[YAXIS].value * magScaleFactor[YAXIS]) - eepromConfig.magBias[YAXIS + eepromConfig.externalHMC5883];
			    nonRotatedMagData[ZAXIS] = (rawMag[ZAXIS].value * magScaleFactor[ZAXIS]) - eepromConfig.magBias[ZAXIS + eepromConfig.externalHMC5883];

			    // Orient raw magnetometer data based on device orientation
			    arm_mat_init_f32(&a, 3, 3, (float *)hmcOrientationMatrix);
			    arm_mat_init_f32(&b, 3, 1, (float *)nonRotatedMagData);
			    arm_mat_init_f32(&x, 3, 1,          sensors.mag10Hz);
			    arm_mat_mult_f32(&a, &b, &x);

			    // New mag data available
				newMagData = false;
			    magDataUpdate = true;
            }

            // Decode GPS data
            decodeGPS();

        	// Update battery monitor
            batMonTick();

            // Check/process CLI port messages
            cliCom();

            // Send high frequency MAVLink messages
            if (eepromConfig.mavlinkEnabled == true)
            {
				mavlinkSendAttitude();
				mavlinkSendVfrHud();
			}

            // Calculate execution tick process time
            executionTime10Hz = micros() - currentTime;

            #ifdef _DTIMING
                LA4_DISABLE;
            #endif
        }

        ///////////////////////////////
        //	500 Hz Execution Tick
        ///////////////////////////////

        if (frame_500Hz)
        {
            #ifdef _DTIMING
                LA1_ENABLE;
            #endif
            frame_500Hz = false;

            // Update delta time for real-time calculations
            currentTime       = micros();
            deltaTime500Hz    = currentTime - previous500HzTime;
            previous500HzTime = currentTime;

            // For integrations in 500 Hz loop
            TIM_Cmd(TIM10, DISABLE);
            timerValue = TIM_GetCounter(TIM10);
            TIM_SetCounter(TIM10, 0);
            TIM_Cmd(TIM10, ENABLE);
            dt500Hz = (float)timerValue * 0.0000005f;

            // Compute accel/gyro temperature compensation bias
            computeMPU6000TCBias();

            // Apply accelerometer bias to raw accelerometer data
       	    nonRotatedAccelData[XAXIS] = ((float)accelSummedSamples500Hz[XAXIS] * 0.5f - accelTCBias[XAXIS]) * ACCEL_SCALE_FACTOR;
       	    nonRotatedAccelData[YAXIS] = ((float)accelSummedSamples500Hz[YAXIS] * 0.5f - accelTCBias[YAXIS]) * ACCEL_SCALE_FACTOR;
       	    nonRotatedAccelData[ZAXIS] = ((float)accelSummedSamples500Hz[ZAXIS] * 0.5f - accelTCBias[ZAXIS]) * ACCEL_SCALE_FACTOR;

       	    // Orient raw accelerometer data based on device orientation
		    arm_mat_init_f32(&a, 3, 3, (float *)mpuOrientationMatrix);
		    arm_mat_init_f32(&b, 3, 1, (float *)nonRotatedAccelData);
		    arm_mat_init_f32(&x, 3, 1,          sensors.accel500Hz);
		    arm_mat_mult_f32(&a, &b, &x);

		    // Apply gyro bias to raw gyro data
            nonRotatedGyroData[ROLL ] = ((float)gyroSummedSamples500Hz[ROLL]  * 0.5f - gyroRTBias[ROLL ] - gyroTCBias[ROLL ]) * GYRO_SCALE_FACTOR;
            nonRotatedGyroData[PITCH] = ((float)gyroSummedSamples500Hz[PITCH] * 0.5f - gyroRTBias[PITCH] - gyroTCBias[PITCH]) * GYRO_SCALE_FACTOR;
            nonRotatedGyroData[YAW  ] = ((float)gyroSummedSamples500Hz[YAW]   * 0.5f - gyroRTBias[YAW  ] - gyroTCBias[YAW  ]) * GYRO_SCALE_FACTOR;

            // Orient raw gyro data based on device orientation
		    arm_mat_init_f32(&a, 3, 3, (float *)mpuOrientationMatrix);
		    arm_mat_init_f32(&b, 3, 1, (float *)nonRotatedGyroData);
		    arm_mat_init_f32(&x, 3, 1,          sensors.gyro500Hz);
		    arm_mat_mult_f32(&a, &b, &x);

		    // Calculate attitude
            MargAHRSupdate(sensors.gyro500Hz[ROLL],   sensors.gyro500Hz[PITCH],  sensors.gyro500Hz[YAW],
                           sensors.accel500Hz[XAXIS], sensors.accel500Hz[YAXIS], sensors.accel500Hz[ZAXIS],
                           sensors.mag10Hz[XAXIS],    sensors.mag10Hz[YAXIS],    sensors.mag10Hz[ZAXIS],
                           eepromConfig.accelCutoff,
                           magDataUpdate,
                           dt500Hz);

            // Schedule a new magnetometer update
            magDataUpdate = false;

            // Calculate and update motor/servo outputs
            computeAxisCommands(dt500Hz);
            mixTable();
            writeServos();
            writeMotors();

            // Calculate execution tick process time
            executionTime500Hz = micros() - currentTime;

            #ifdef _DTIMING
                LA1_DISABLE;
            #endif
        }

        ///////////////////////////////
        //	100 Hz Execution Tick
        ///////////////////////////////

        if (frame_100Hz)
        {
            #ifdef _DTIMING
                LA3_ENABLE;
            #endif
            frame_100Hz = false;

            // Update delta time for real-time calculations
            currentTime       = micros();
            deltaTime100Hz    = currentTime - previous100HzTime;
            previous100HzTime = currentTime;

            // For integrations in 100 Hz loop
            TIM_Cmd(TIM11, DISABLE);
            timerValue = TIM_GetCounter(TIM11);
            TIM_SetCounter(TIM11, 0);
            TIM_Cmd(TIM11, ENABLE);
            dt100Hz = (float)timerValue * 0.0000005f;

            // Apply accelerometer bias to raw accelerometer data
       	    nonRotatedAccelData[XAXIS] = ((float)accelSummedSamples100Hz[XAXIS] * 0.1f - accelTCBias[XAXIS]) * ACCEL_SCALE_FACTOR;
       	    nonRotatedAccelData[YAXIS] = ((float)accelSummedSamples100Hz[YAXIS] * 0.1f - accelTCBias[YAXIS]) * ACCEL_SCALE_FACTOR;
       	    nonRotatedAccelData[ZAXIS] = ((float)accelSummedSamples100Hz[ZAXIS] * 0.1f - accelTCBias[ZAXIS]) * ACCEL_SCALE_FACTOR;

       	    // Orient raw accel data based on device orientation
		    arm_mat_init_f32(&a, 3, 3, (float *)mpuOrientationMatrix);
		    arm_mat_init_f32(&b, 3, 1, (float *)nonRotatedAccelData);
		    arm_mat_init_f32(&x, 3, 1,          sensors.accel100Hz);
		    arm_mat_mult_f32(&a, &b, &x);

		    // Calculate acceleration data relative to earth rather than sensor axis
            createRotationMatrix();
            bodyAccelToEarthAccel();

            // Update altitude estimation filter (compares altimeter with integrated vertical acceleration)
            vertCompFilter(dt100Hz);

            // Calculate execution tick process time
            executionTime100Hz = micros() - currentTime;

            #ifdef _DTIMING
                LA3_DISABLE;
            #endif
        }

        ///////////////////////////////
        //	5 Hz Execution Tick
        ///////////////////////////////

        if (frame_5Hz)
        {
            frame_5Hz = false;

            // Update delta time for real-time calculations
            currentTime     = micros();
            deltaTime5Hz    = currentTime - previous5HzTime;
            previous5HzTime = currentTime;

            // Send medium frequency MAVLink messages
            if (eepromConfig.mavlinkEnabled == true)
            {
				mavlinkSendGpsRaw();

			}

            // Update battery monitor
			if (batMonVeryLowWarning > 0)
			{
				LED1_TOGGLE;
				batMonVeryLowWarning--;
			}

			// Toggle blue board LED (Main loop running)
            if (execUp == true)
                BLUE_LED_TOGGLE;

            // Calculate execution tick process time
			executionTime5Hz = micros() - currentTime;
        }

        ///////////////////////////////
        //	1 Hz Execution Tick
        ///////////////////////////////

        if (frame_1Hz)
        {
            frame_1Hz = false;

            // Update delta time for real-time calculations
            currentTime     = micros();
            deltaTime1Hz    = currentTime - previous1HzTime;
            previous1HzTime = currentTime;

            // Toggle green board LED (Main loop running)
            if (execUp == false)
                execUpCount++;
            else
                GREEN_LED_TOGGLE;

            // Counter to allow main loop to run for a while before starting up motors
            if ((execUpCount == 5) && (execUp == false))
            {
				execUp = true;
                pwmEscInit();

                // Initialize vehicle heading
                homeData.magHeading = sensors.attitude500Hz[YAW];
			}

            // Update battery monitor
            if (batMonLowWarning > 0)
			{
				LED1_TOGGLE;
				batMonLowWarning--;
			}

            // Send low frequency MAVLink messages
            if (eepromConfig.mavlinkEnabled == true)
            {
				mavlinkSendHeartbeat();
				mavlinkSendSysStatus();
				mavlinkSendESBData();
				communication_receive();

			}

            // Update environmental sensors
            updateESB();

            // TODO Remove this
            cameraTestEcho();

            // Calculate execution tick process time
            executionTime1Hz = micros() - currentTime;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
