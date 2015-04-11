///////////////////////////////////////////////////////////////////////////////

#include "board.h"

///////////////////////////////////////////////////////////////////////////////

uint32_t (*cameraPortAvailable)(void);
uint16_t (*cameraPortNumCharsAvailable)(void);
void     (*cameraPortClearBuffer)(void);
uint8_t  (*cameraPortRead)(void);
void     (*cameraPortPrint)(char *str);
void     (*cameraPortPrintF)(const char * fmt, ...);
void     (*cameraPortWrite)(uint8_t data);

///////////////////////////////////////////////////////////////////////////////
// Initialize VISCA camera
///////////////////////////////////////////////////////////////////////////////

void cameraEnable(uint8_t state)
{
	if(state == true)
		cameraPortPrint("\x81\x01\x04\x00\x02\xFF");
	else
		cameraPortPrint("\x81\x01\x04\x00\x03\xFF");
}

void cameraFlip(uint8_t state)
{
	if(state == true)
		cameraPortPrint("\x81\x01\x04\x66\x02\xFF");
	else
		cameraPortPrint("\x81\x01\x04\x66\x03\xFF");
}

void cameraSetZoom(uint8_t optical, uint8_t digital)
{
	cameraPortPrintF("\x81\x01\x04\x47%c%c\xFF", optical, digital);
}

void cameraSetFocusMode(uint8_t mode)
{
	cameraPortPrintF("\x81\x01\x04\x38%c\xFF", mode);
}

void cameraSetAF(uint8_t state, uint8_t mode)
{
	if(state == true)
		cameraPortPrintF("\x81\x01\x04\x58\x02\xFF", mode);
	else
		cameraPortPrintF("\x81\x01\x04\x58\x03\xFF", mode);
	cameraPortPrintF("\x81\x01\x04\x57%c\xFF", mode);
}

void cameraSetWhiteBalance(uint8_t mode)
{
	cameraPortPrintF("\x81\x01\x04\x35%c\xFF", mode);
}

void cameraFreezeImage(uint8_t state)
{
	if(state == true)
		cameraPortPrint("\x81\x01\x04\x62\x02\xFF");
	else
		cameraPortPrint("\x81\x01\x04\x62\x03\xFF");
}

void cameraSetEffect(uint8_t mode)
{
	cameraPortPrintF("\x81\x01\x04\x63%c\xFF", mode);
}

void initCamera(void)
{
	cameraEnable(true);
}

void cameraTestEcho(void)
{
	// Testing echo on camera port
	cameraPortPrintF("Echoing: ");
	if (cameraPortAvailable())
	{

		int i = 0;
		int charsLeft = cameraPortNumCharsAvailable();
		for(i = 0; i < charsLeft; i++)
			cameraPortWrite(cameraPortRead());
	}
	cameraPortPrintF("\n");
}


////////////////////////////////////////////////////////////////////////
