///////////////////////////////////////////////////////////////////////////////

#include "board.h"

///////////////////////////////////////////////////////////////////////////////

#define RANGE_MINIMUM  0.25f  // Meters
#define RANGE_MAXIMUM  7.0f  // Meters
#define RANGE_SCALE_10M_5V 4.08163265f	// meters/volt
#define RANGE_SCALE_10M_3V 2.04081633f	// meters/volt
#define RANGE_SCALE_7M_5V  6.25f	// meters/volt
#define RANGE_SCALE_7M_3V  3.125f	// meters/volt

float   agl = 0.0f;

uint8_t aglLanded = false;
uint8_t aglValid = false;

///////////////////////////////////////////////////////////////////////////////
// AGL Read
///////////////////////////////////////////////////////////////////////////////

float aglRead(void)
{
	// Convert analog pin voltage to distance measurement (meters)
    agl = constrain(adcValue(eepromConfig.aglPin) * VOLTS_PER_BIT * eepromConfig.aglScale + eepromConfig.aglBias, 0.0f, 7.0f);

    // Update state
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
