/*
  February 2013

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
// GPS Variables
///////////////////////////////////////////////////////////////////////////////

static const uint8_t ubx5Hz[14] = {0xb5,0x62,0x06,0x08,0x06,0x00,0xc8,0x00,0x01,0x00,0x01,0x00,0xde,0x6a};  // 06 08 (6)

///////////////////////////////////////////////////////////////////////////////

uint32_t (*gpsPortAvailable)(void);

uint16_t (*gpsPortNumCharsAvailable)(void);

void     (*gpsPortClearBuffer)(void);

uint8_t  (*gpsPortRead)(void);

void     (*gpsPortPrint)(char *str);

void     (*gpsPortPrintF)(const char * fmt, ...);

void     (*gpsPortWrite)(uint8_t data);

///////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Initialize GPS Receiver
///////////////////////////////////////////////////////////////////////////////

void initGPS(void)
{
    uint8_t i;

    switch(eepromConfig.gpsType)
    {
		///////////////////////////////

		case NO_GPS:
		    break;

		///////////////////////////////

		case MEDIATEK_3329_BINARY:     // MediaTek 3329 in binary mode
		    gpsPortPrint("$PGCMD,16,0,0,0,0,0*6A\r\n");  // Set Binary Output
            gpsPortPrint("$PMTK220,200*2C\r\n");         // Set 5 Hz Output
            gpsPortPrint("$PMTK313,1*2E\r\n");           // Set SBAS On - Not sure if this does anything on MTK16 software
            gpsPortPrint("$PMTK301,2*2E\r\n");           // Set WAAS On - Not sure if this does anything on MTK16 software
            gpsPortPrint("$PMTK397,0*23\r\n");           // Set Nav Speed Threshold to 0

            mtk19ProcessDataState = MTK19_WAIT_SYNC1;
            break;

        ///////////////////////////////

        case MEDIATEK_3329_NMEA:       // MediaTek 3329 in NMEA mode
        	gpsPortPrint("$PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");  // GPRMC, GPGGA and GPGSA
			gpsPortPrint("$PMTK220,200*2C\r\n");                                    // Set 5 Hz Update Rate
			gpsPortPrint("$PMTK313,1*2E\r\n");                                      // Set SBAS On
            gpsPortPrint("$PMTK301,2*2E\r\n");                                      // Set WAAS On
            gpsPortPrint("$PMTK397,0*23\r\n");                                      // Set Nav Speed Threshold to 0

            nmeaProcessDataState = WAIT_START;
            break;

        ///////////////////////////////

        case UBLOX:             // UBLOX in binary mode
        	for (i = 0; i < sizeof(ubx5Hz); i++)                // Set 5 Hz Update Rate
        	   gpsPortWrite(ubx5Hz[i]);

        	gpsPortPrint("$PUBX,41,1,0003,0001,38400,0*26\r\n");  // Set Binary Output

        	ubloxProcessDataState = WAIT_SYNC1;
        	break;

        ///////////////////////////////
	}

	gps.latitude    = GPS_INVALID_ANGLE;
	gps.longitude   = GPS_INVALID_ANGLE;
	gps.height	   = GPS_INVALID_ALTITUDE;
	gps.gSpeed = GPS_INVALID_SPEED;
	gps.heading = GPS_INVALID_ANGLE;
	gps.numSats     = GPS_INVALID_SATS;
	gps.fix         = GPS_INVALID_FIX;
	gps.year        = GPS_INVALID_DATE;
	gps.month        = GPS_INVALID_DATE;
	gps.day        = GPS_INVALID_DATE;
	gps.iTOW        = GPS_INVALID_TIME;
	gps.hDop        = GPS_INVALID_HDOP;

	gpsPortClearBuffer();
}

////////////////////////////////////////////////////////////////////////
