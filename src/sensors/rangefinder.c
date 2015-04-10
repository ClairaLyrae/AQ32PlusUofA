///////////////////////////////////////////////////////////////////////////////

#include "board.h"

///////////////////////////////////////////////////////////////////////////////

#define RANGE_SCALE_FACTOR_7m  3.125f  // mV to meters, 3.2 mV = 1 cm
#define RANGE_SCALE_FACTOR_10m  2.45f  // For MB1260, MB1261, MB1360, and MB1361 sensors
#define RANGE_MINIMUM  0.25f  // in meters
#define RANGE_MAXIMUM  7.0f  // in meters

///////////////////////////////////////

float   agl = 0.0f;

uint8_t aglLanded = false;
uint8_t aglValid = false;

///////////////////////////////////////////////////////////////////////////////
// AGL Read
///////////////////////////////////////////////////////////////////////////////

float aglRead(void)
{
    agl = constrain(adcValue(eepromConfig.aglPin) * VOLTS_PER_BIT * eepromConfig.aglScale + eepromConfig.aglBias, 0.0f, 7.0f);
	aglValid = false;
	if(agl <= RANGE_MINIMUM)
    	aglLanded = true;
    else if(agl >= RANGE_MAXIMUM)
    	aglLanded = false;
    else
    	aglValid = true;
    return agl;
}

///////////////////////////////////////////////////////////////////////////////
