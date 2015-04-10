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
// GPS Defines
///////////////////////////////////////////////////////////////////////////////

#define MTK_GPS_REVISION_V16  16
#define MTK_GPS_REVISION_V19  19

#define SYNC1_V16 0xD0
#define SYNC1_V19 0xD1
#define SYNC2     0xDD

///////////////////////////////////////////////////////////////////////////////
// GPS Variables
///////////////////////////////////////////////////////////////////////////////

typedef struct __attribute__((packed))mtk19Msg_t
{
    int32_t	  latitude;
    int32_t	  longitude;
    int32_t	  altitude;
    int32_t	  groundSpeed;
    int32_t	  groundTrack;
    uint8_t   satellites;
    uint8_t   fixType;
    uint32_t  date;
    uint32_t  time;
    uint16_t  hdop;
} mtk19Msg_t;

union { mtk19Msg_t data;
        uint8_t    bytes[32];
      } mtk19Message;

uint8_t mtk19CkA;
uint8_t mtk19CkB;
uint8_t mtk19DataLength;
uint8_t mtk19ExpectedDataLength;
uint8_t mtkRevision;

///////////////////////////////////////////////////////////////////////////////
// Decode MediaTek 3329 Binary Message (DIY Drones Binary Protocol)
///////////////////////////////////////////////////////////////////////////////

uint8_t decodeMediaTek3329BinaryMsg(void)
{
    uint8_t 	data;
    uint8_t 	parsed = false;
    uint16_t    i;
    uint16_t    numberOfChars;

    numberOfChars = gpsPortNumCharsAvailable();

    if (numberOfChars == 0) return false;

    for (i = 0; i < numberOfChars; i++)
    {
		data = gpsPortRead();

        switch(mtk19ProcessDataState)
        {
            ///////////////////////////

            case MTK19_WAIT_SYNC1:
                if (data == SYNC1_V16)
                {
                	mtkRevision = MTK_GPS_REVISION_V16;
                	mtk19ProcessDataState = MTK19_WAIT_SYNC2;
                }
                else if (data == SYNC1_V19)
                {
					mtkRevision = MTK_GPS_REVISION_V19;
					mtk19ProcessDataState = MTK19_WAIT_SYNC2;
				}

                break;

            ///////////////////////////

            case MTK19_WAIT_SYNC2:
                if (data == SYNC2)
                    mtk19ProcessDataState = MTK19_GET_LEN;
                else
                    mtk19ProcessDataState = MTK19_WAIT_SYNC1;

                mtk19ExpectedDataLength = 0;

                break;

            ///////////////////////////

            case MTK19_GET_LEN:
                mtk19ExpectedDataLength = data;

                if (mtk19ExpectedDataLength == sizeof(mtk19Message))
                {
                    mtk19ProcessDataState = MTK19_GET_DATA;
                    mtk19CkA = data;
                    mtk19CkB = mtk19CkA;
                    mtk19DataLength = 0;
                }
                else
                    mtk19ProcessDataState = MTK19_WAIT_SYNC1;

                break;

            ///////////////////////////

            case MTK19_GET_DATA:
                mtk19CkA += data;
                mtk19CkB += mtk19CkA;

                if (mtk19DataLength < sizeof(mtk19Message))
                    mtk19Message.bytes[mtk19DataLength++] = data;

                if (mtk19DataLength >= mtk19ExpectedDataLength)
                    mtk19ProcessDataState = MTK19_GET_CKA;

                break;

            ///////////////////////////

            case MTK19_GET_CKA:
                if (mtk19CkA == data)
                    mtk19ProcessDataState = MTK19_GET_CKB;
                else
                    mtk19ProcessDataState = MTK19_WAIT_SYNC1;

			    break;

            ///////////////////////////

            case MTK19_GET_CKB:
                if (mtk19CkB == data)
                {
				    if (mtkRevision == MTK_GPS_REVISION_V16)
                    {
				    	gps.latitude  = (float)mtk19Message.data.latitude  * 0.000001f  * D2R; // Radians
                        gps.longitude = (float)mtk19Message.data.longitude * 0.000001f  * D2R; // Radians
			        }
				    else
				    {
				    	gps.latitude  = (float)mtk19Message.data.latitude  * 0.0000001f * D2R; // Radians
                        gps.longitude = (float)mtk19Message.data.longitude * 0.0000001f * D2R; // Radians
				    }

                    gps.height		 = (float)mtk19Message.data.altitude    * 0.01f;          // Meters
                    gps.gSpeed	 = (float)mtk19Message.data.groundSpeed * 0.01f;          // Meters/Sec
                    gps.heading	 = (float)mtk19Message.data.groundTrack * 0.01f * D2R;    // Radians

                    gps.numSats		 = mtk19Message.data.satellites;
                    gps.fix			 = mtk19Message.data.fixType;
                    //gps.date			 = mtk19Message.data.date;
                    gps.iTOW          = (float)mtk19Message.data.time * 0.001f;
                    gps.hDop			 = (float)mtk19Message.data.hdop * 0.01f;

                    parsed = true;
			    }
			    else
			    {
					gps.latitude    = GPS_INVALID_ANGLE;
					gps.longitude   = GPS_INVALID_ANGLE;
					gps.height	   = GPS_INVALID_ALTITUDE;
					gps.gSpeed = GPS_INVALID_SPEED;
					gps.heading = GPS_INVALID_ANGLE;
					gps.numSats     = GPS_INVALID_SATS;
					gps.fix         = GPS_INVALID_FIX;
					gps.year        = GPS_INVALID_DATE;
					//gps.month        = GPS_INVALID_DATE;
					//gps.day        = GPS_INVALID_DATE;
					gps.iTOW        = GPS_INVALID_TIME;
					gps.hDop        = GPS_INVALID_HDOP;
				}

                mtk19ProcessDataState = MTK19_WAIT_SYNC1;

                break;
        }
    }
    return parsed;
}

///////////////////////////////////////////////////////////////////////////////
