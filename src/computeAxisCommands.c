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

float   attCmd[3];
float   attPID[3];
float   ratePID[3];
float   rateCmd[3];
float   headingReference;
float   altitudeHoldReference;
float   throttleReference;
float   verticalVelocityCmd;

///////////////////////////////////////////////////////////////////////////////
// Compute Axis Commands
///////////////////////////////////////////////////////////////////////////////
void computeAxisCommands(float dt)
{
    float error;
    float tempAttCompensation;

	///////////////////////////////////

    // Attitude mode is ON, apply attitude stabilization to input commands BEFORE rate calculations
    if (flightMode >= ATTITUDE)
    {
        if (flightMode == ATTITUDE)
        {
            // Scale user input in attitude mode
            attCmd[ROLL ] = rxCommand[ROLL ] * eepromConfig.attitudeScaling;
            attCmd[PITCH] = rxCommand[PITCH] * eepromConfig.attitudeScaling;
        }

        // Compute error terms for roll and pitch in attitude mode (User input compared to vehicle attitude)
        // Apply error terms to PID controllers as feedback

        error = standardRadianFormat(attCmd[ROLL] - sensors.attitude500Hz[ROLL]);
        attPID[ROLL]  = updatePID(error, dt, pidReset, &eepromConfig.PID[ROLL_ATT_PID]);

        error = standardRadianFormat(attCmd[PITCH] + sensors.attitude500Hz[PITCH]);
        attPID[PITCH] = updatePID(error, dt, pidReset, &eepromConfig.PID[PITCH_ATT_PID]);
    }


//    if (flightMode == GPS)//TODO
//    {
//
//    }

    // Rate mode is ON
    if (flightMode == RATE)
    {
        // Scale user input in rate mode
        rateCmd[ROLL ] = rxCommand[ROLL ] * eepromConfig.rollAndPitchRateScaling;
        rateCmd[PITCH] = rxCommand[PITCH] * eepromConfig.rollAndPitchRateScaling;
    }

    // Attitude mode is ON
    else
    {
        // Take control input from output of attitude PIDs
        rateCmd[ROLL ] = attPID[ROLL ];
        rateCmd[PITCH] = attPID[PITCH];
    }

    ///////////////////////////////////

    // Heading Hold is ON, compute error term for yaw, apply to PID, and use PID calculation for output
    if (headingHoldEngaged == true)
    {
    	error = standardRadianFormat(headingReference - heading.mag);
        rateCmd[YAW] = updatePID(error, dt, pidReset, &eepromConfig.PID[HEADING_PID]);
    }
    // Heading Hold is OFF, get yaw direct from user input
    else
	    rateCmd[YAW] = rxCommand[YAW] * eepromConfig.yawRateScaling;

	///////////////////////////////////

    // Compute error terms for roll, pitch and yaw from rate OR attitude inputs found above
    // Apply error terms to rate PID controllers as feedback based on gyro data
    // Rate PIDs based off error on gyro only

    error = rateCmd[ROLL] - sensors.gyro500Hz[ROLL];
    ratePID[ROLL] = updatePID(error, dt, pidReset, &eepromConfig.PID[ROLL_RATE_PID ]);

    error = rateCmd[PITCH] + sensors.gyro500Hz[PITCH];
    ratePID[PITCH] = updatePID(error, dt, pidReset, &eepromConfig.PID[PITCH_RATE_PID]);

    error = rateCmd[YAW] - sensors.gyro500Hz[YAW];
    ratePID[YAW] = updatePID(error, dt, pidReset, &eepromConfig.PID[YAW_RATE_PID  ]);

	///////////////////////////////////

    // Manual Mode is ON, no altitude hold
	if (verticalModeState == ALT_DISENGAGED_THROTTLE_ACTIVE)
	    throttleCmd = rxCommand[THROTTLE];
	else
	{
		// Altitude Hold is ON
	    if ((verticalModeState == ALT_HOLD_FIXED_AT_ENGAGEMENT_ALT) || (verticalModeState == ALT_HOLD_AT_REFERENCE_ALTITUDE) || (verticalModeState == ALT_DISENGAGED_THROTTLE_INACTIVE))
        {
	    	// Compute error term for altitude position based off height estimation from sensors
	    	// Use as feedback to vertical position hold PID, which serves as input to vertical velocity PID
	    	// altitudeHoldReference depends on particular type of altitude hold enabled (fixed, at engagement, or throttle inactive)

            error = altitudeHoldReference - hEstimate;
			verticalVelocityCmd = updatePID(error, dt, pidReset, &eepromConfig.PID[H_PID]);
		}
	    // Vertical Velocity Hold is ON
	    else
	    {
	    	// Get vertical velocity directly from velocity reference command (user input)
	        verticalVelocityCmd = verticalReferenceCommand * eepromConfig.hDotScaling;
	    }

	    // Compute error term for vertical velocity hold PID based of estimated vertical velocity (hDot)
	    // Use as feedback to vertical velocity hold PID
    	error = verticalVelocityCmd - hDotEstimate;
		throttleCmd = throttleReference + updatePID(error, dt, pidReset, &eepromConfig.PID[HDOT_PID]);

	    // Get Roll Angle, Constrain to +/-20 degrees (default)
	    tempAttCompensation = constrain(sensors.attitude500Hz[ROLL ], eepromConfig.rollAttAltCompensationLimit,  -eepromConfig.rollAttAltCompensationLimit);

	    // Compute Cosine of Roll Angle and Multiply by Att-Alt Gain
	    tempAttCompensation = eepromConfig.rollAttAltCompensationGain / cosf(tempAttCompensation);

	    // Apply Roll Att Compensation to Throttle Command
	    throttleCmd *= tempAttCompensation;

	    // Get Pitch Angle, Constrain to +/-20 degrees (default)
	    tempAttCompensation = constrain(sensors.attitude500Hz[PITCH], eepromConfig.pitchAttAltCompensationLimit,  -eepromConfig.pitchAttAltCompensationLimit);

	    // Compute Cosine of Pitch Angle and Multiply by Att-Alt Gain
	    tempAttCompensation = eepromConfig.pitchAttAltCompensationGain / cosf(tempAttCompensation);

	    // Apply Pitch Att Compensation to Throttle Command
	    throttleCmd *= tempAttCompensation;
	}

	///////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////
