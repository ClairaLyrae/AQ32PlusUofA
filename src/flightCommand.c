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
// Process Pilot Commands Defines and Variables
///////////////////////////////////////////////////////////////////////////////
//AERT  receiver->pin(rxcommand&rcSwitch) Gear-> 5(aux1&switchD)    AUX1->7(aux3&switch A)  AUX2-> 6(aux2&switch E)   AUX4-> 8(aux4&left scroll)
float    rxCommand[12] = { 0.0f, 0.0f, 0.0f, 2000.0f, 2000.0f, 2000.0f, 2000.0f, 2000.0f, 2000.0f, 2000.0f, 2000.0f, 2000.0f };

uint8_t  commandInDetent[3]         = { true, true, true };
uint8_t  previousCommandInDetent[3] = { true, true, true };

///////////////////////////////////////////////////////////////////////////////
// Flight Mode Defines and Variables
///////////////////////////////////////////////////////////////////////////////

uint8_t flightMode = RATE;
uint8_t headingHoldEngaged     = false;

///////////////////////////////////////////////////////////////////////////////
// Arm State Variables
///////////////////////////////////////////////////////////////////////////////

semaphore_t armed          = false;
uint8_t     armingTimer    = 0;
uint8_t     disarmingTimer = 0;

///////////////////////////////////////////////////////////////////////////////
// Vertical Mode State Variables
///////////////////////////////////////////////////////////////////////////////

uint8_t  verticalModeState = ALT_DISENGAGED_THROTTLE_ACTIVE;

uint16_t previousAUX2State = MINCOMMAND;
uint16_t previousAUX4State = MINCOMMAND;

uint8_t  vertRefCmdInDetent         = true;
uint8_t  previousVertRefCmdInDetent = true;

float    verticalReferenceCommand;

///////////////////////////////////////////////////////////////////////////////
// Read Flight Commands
///////////////////////////////////////////////////////////////////////////////

void processFlightCommands(void)
{
    uint8_t channel;
    uint8_t channelsToRead = 8;

    float hdgDelta, simpleX, simpleY;

    // RC Receiver active
    if ( rcActive == true )
    {
		// PPM channel (not implemented)
    	//if (eepromConfig.receiverType == PPM)
    	//	channelsToRead = eepromConfig.ppmChannels;

    	// Read all receiver channel values
		for (channel = 0; channel < channelsToRead; channel++)
		{
			if (eepromConfig.receiverType == SPEKTRUM)
			    rxCommand[channel] = spektrumRead(eepromConfig.rcMap[channel]);
			else if (eepromConfig.receiverType == SBUS)
				rxCommand[channel] = sBusRead(eepromConfig.rcMap[channel]);
			else
			    rxCommand[channel] = rxRead(eepromConfig.rcMap[channel]);
		}

		// Center flight command inputs around configured midpoint (zero)
        rxCommand[ROLL]  -= eepromConfig.midCommand;  // Roll Range  -1000:1000
        rxCommand[PITCH] -= eepromConfig.midCommand;  // Pitch Range -1000:1000
        rxCommand[YAW]   -= eepromConfig.midCommand;  // Yaw Range   -1000:1000

        // Unknown?
		for (channel = 3; channel < channelsToRead; channel++)
			rxCommand[channel] -= eepromConfig.midCommand - MIDCOMMAND;  // Range 2000:4000
    }

    // Set past command in detent values
    for (channel = 0; channel < 3; channel++)
    	previousCommandInDetent[channel] = commandInDetent[channel];

    // Apply deadbands and set detent discretes'
    for (channel = 0; channel < 3; channel++)
    {
    	if ((rxCommand[channel] <= DEADBAND) && (rxCommand[channel] >= -DEADBAND))
        {
            rxCommand[channel] = 0;
  	        commandInDetent[channel] = true;
  	    }
        else
  	    {
  	        commandInDetent[channel] = false;
  	        if (rxCommand[channel] > 0)
  	        {
  		        rxCommand[channel] = (rxCommand[channel] - DEADBAND) * DEADBAND_SLOPE;
  	        }
  	        else
  	        {
  	            rxCommand[channel] = (rxCommand[channel] + DEADBAND) * DEADBAND_SLOPE;
  	        }
        }
    }

    ///////////////////////////////////

    // Check for low throttle
    if ( rxCommand[THROTTLE] < eepromConfig.minCheck )
    {
		// Check for disarm command ( low throttle, left yaw )
		if ( (rxCommand[YAW] < (eepromConfig.minCheck - MIDCOMMAND)) && (armed == true) )
		{
			disarmingTimer++;

			// Count up to value before disarming
			if (disarmingTimer > eepromConfig.disarmCount)
			{
				zeroPIDstates();
			    armed = false;
			    disarmingTimer = 0;
			}
		}
		else
			disarmingTimer = 0;

		// Check for gyro bias command ( low throttle, left yaw, aft pitch, right roll )
		if ( (rxCommand[YAW  ] < (eepromConfig.minCheck - MIDCOMMAND)) && (rxCommand[ROLL ] > (eepromConfig.maxCheck - MIDCOMMAND)) && (rxCommand[PITCH] < (eepromConfig.minCheck - MIDCOMMAND)) )
		{
			computeMPU6000RTData();
			pulseMotors(3);
		}

		// Check for arm command (low throttle, right yaw)
		if ((rxCommand[YAW] > (eepromConfig.maxCheck - MIDCOMMAND) ) && (armed == false) && (execUp == true))
		{
			armingTimer++;

			// Count up to value before arming
			if (armingTimer > eepromConfig.armCount)
			{
				zeroPIDstates();
				armed = true;
				armingTimer = 0;
			}
		}
		else
			armingTimer = 0;
	}

	///////////////////////////////////

	// Check for armed true and throttle command > minThrottle

    if ((armed == true) && (rxCommand[THROTTLE] > eepromConfig.minThrottle))
    	pidReset = false;
    else
    	pidReset = true;

    ///////////////////////////////////

    // Check AUX1 for rate, attitude, or GPS mode (3 Position Switch) NOT COMPLETE YET....
    //AUX1(D) O(Rate)->2043 1(attitude)->3013 2(GPS&Att)->4081
    // Don't update flight mode if it has not changed

	if ((rxCommand[AUX1] > (MINCOMMAND+500)) && (flightMode != ATTITUDE))//for att and gps/att
	{
		flightMode = ATTITUDE;
		setPIDstates(ROLL_ATT_PID,  0.0f);
		setPIDstates(PITCH_ATT_PID, 0.0f);
		setPIDstates(HEADING_PID,   0.0f);
	}
	else if ((rxCommand[AUX1] <= (MINCOMMAND+500)) && (flightMode != RATE))
	{
		flightMode = RATE;
		setPIDstates(ROLL_RATE_PID,  0.0f);
		setPIDstates(PITCH_RATE_PID, 0.0f);
		setPIDstates(YAW_RATE_PID,   0.0f);
	}

	//no more gps flight mode
	if ((rxCommand[AUX1] > (MIDCOMMAND+500)) && (gps.fix != FIX_NONE) && (eepromConfig.gpsType != NO_GPS))
	{

//		gps.latitude ;
//		gps.longitude ;
//		gps.height	 ;
//		gps.gSpeed ;
//		gps.heading ;
//		gps.fix ;
	}


	///////////////////////////////////

	// Check yaw in detent and flight mode to determine hdg hold engaged state

	if ((commandInDetent[YAW] == true) && (flightMode == ATTITUDE) && (headingHoldEngaged == false))
	{
		headingHoldEngaged = true;
		setPIDstates(HEADING_PID,  0.0f);
		setPIDstates(YAW_RATE_PID, 0.0f);
		headingReference = heading.mag;
	}
	if (((commandInDetent[YAW] == false) || (flightMode != ATTITUDE)) && (headingHoldEngaged == true))
	{
		headingHoldEngaged = false;
	}

	///////////////////////////////////

	// Simple Mode Command Processing. Orients copter to pilot

	if (rxCommand[AUX3] > MIDCOMMAND)
	{
		// Compute heading error term based on attitude and known home heading
        hdgDelta = sensors.attitude500Hz[YAW] - homeData.magHeading;
        hdgDelta = standardRadianFormat(hdgDelta);

        simpleX = cosf(hdgDelta) * rxCommand[PITCH] + sinf(hdgDelta) * rxCommand[ROLL ];
        simpleY = cosf(hdgDelta) * rxCommand[ROLL ] - sinf(hdgDelta) * rxCommand[PITCH];

        rxCommand[ROLL ] = simpleY;
        rxCommand[PITCH] = simpleX;
	}

	///////////////////////////////////

	// Vertical Mode Command Processing

	verticalReferenceCommand = rxCommand[THROTTLE] - eepromConfig.midCommand;

    // Set past altitude reference in detent value
    previousVertRefCmdInDetent = vertRefCmdInDetent;

    // Apply deadband and set detent discrete'
    if ((verticalReferenceCommand <= ALT_DEADBAND) && (verticalReferenceCommand >= -ALT_DEADBAND))
    {
        verticalReferenceCommand = 0;
  	    vertRefCmdInDetent = true;
  	}
    else
  	{
  	    vertRefCmdInDetent = false;
  	    if (verticalReferenceCommand > 0)
  		    verticalReferenceCommand = (verticalReferenceCommand - ALT_DEADBAND) * ALT_DEADBAND_SLOPE;
  	    else
  	        verticalReferenceCommand = (verticalReferenceCommand + ALT_DEADBAND) * ALT_DEADBAND_SLOPE;
    }

    ///////////////////////////////////

    // Vertical Mode State Machine
    // DEFINITION : Detents are used to simply arrest rotation or divide a rotation into discrete increments

    switch (verticalModeState)
	{
		case ALT_DISENGAGED_THROTTLE_ACTIVE:

		    // Going into vertical velocity hold from off
		    if ((rxCommand[AUX2] < MINCOMMAND+500) && (previousAUX2State >= MIDCOMMAND+500))
		    {
		        verticalModeState = VERTICAL_VELOCITY_HOLD_AT_REFERENCE_VELOCITY;
		    }
			// Going into altitude mode from off
		    else if ((rxCommand[AUX2] < MIDCOMMAND+500) && (previousAUX2State >= MIDCOMMAND+500))
		    {
				verticalModeState = ALT_HOLD_FIXED_AT_ENGAGEMENT_ALT;
				setPIDstates(HDOT_PID,        0.0f);
				setPIDstates(H_PID,           0.0f);
                altitudeHoldReference = hEstimate;
                throttleReference     = rxCommand[THROTTLE];
		    }

		    break;

		///////////////////////////////Assigning alt hold mode

		case ALT_HOLD_FIXED_AT_ENGAGEMENT_ALT:
			// If throttle in detent, hold at reference altitude
		    if ((vertRefCmdInDetent == true))
		        verticalModeState = ALT_HOLD_AT_REFERENCE_ALTITUDE;

		    // Disengage
		    if ((rxCommand[AUX2] > MIDCOMMAND+500))  // AUX2 Falling edge detection
		        verticalModeState = ALT_DISENGAGED_THROTTLE_ACTIVE;

		    // Force velocity only
		    if ((rxCommand[AUX2] < MINCOMMAND+500))  // AUX2 Falling edge detection
		        verticalModeState = VERTICAL_VELOCITY_HOLD_AT_REFERENCE_VELOCITY;
		    break;

		case ALT_HOLD_AT_REFERENCE_ALTITUDE:
		    if ((vertRefCmdInDetent == false))
		        verticalModeState = VERTICAL_VELOCITY_HOLD_AT_REFERENCE_VELOCITY;

		    // Disengage
		    if ((rxCommand[AUX2] > MIDCOMMAND+500))  // AUX2 Falling edge detection
		        verticalModeState = ALT_DISENGAGED_THROTTLE_ACTIVE;

		    // Force velocity only
		    if ((rxCommand[AUX2] < MINCOMMAND+500))  // AUX2 Falling edge detection
		        verticalModeState = VERTICAL_VELOCITY_HOLD_AT_REFERENCE_VELOCITY;

		    break;

		///////////////////////////////

		case VERTICAL_VELOCITY_HOLD_AT_REFERENCE_VELOCITY:
		    if ((vertRefCmdInDetent == true))
		    {
				verticalModeState = ALT_HOLD_AT_REFERENCE_ALTITUDE;
				altitudeHoldReference = hEstimate;
			}
		    else if ((rxCommand[AUX2] > MIDCOMMAND+500) && ((previousAUX2State > MINCOMMAND+500) && (previousAUX2State <= MIDCOMMAND+500)))
		    {
				verticalModeState = ALT_DISENGAGED_THROTTLE_ACTIVE;
				altitudeHoldReference = hEstimate;
		    }

		    if ((rxCommand[AUX2] > MIDCOMMAND+500) && (previousAUX2State <= MINCOMMAND+500))  // AUX2 Falling edge detection, aux2 turned off
		    {
				verticalModeState = ALT_DISENGAGED_THROTTLE_ACTIVE;
				altitudeHoldReference = hEstimate;
			}
		    else if ((rxCommand[AUX2] > MINCOMMAND+500) && (previousAUX2State <= MINCOMMAND+500))  // AUX2 Falling edge detection, aux2 turned off
		    {
				verticalModeState = ALT_HOLD_FIXED_AT_ENGAGEMENT_ALT;
				setPIDstates(HDOT_PID,        0.0f);
				setPIDstates(H_PID,           0.0f);
                altitudeHoldReference = hEstimate;
                throttleReference     = rxCommand[THROTTLE];
			}

		    break;

		///////////////////////////////
    }

	previousAUX2State = rxCommand[AUX2];

	///////////////////////////////////
}

///////////////////////////////////////////////////////////////////////////////




