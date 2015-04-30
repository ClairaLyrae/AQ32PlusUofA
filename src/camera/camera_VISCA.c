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
int cameraZoom;
uint8_t zero_byte = 0x00;
///////////////////////////////////////////////////////////////////////////////
// Initialize VISCA camera
///////////////////////////////////////////////////////////////////////////////

void cameraEnable(uint8_t state)
{
	if(state == true){
		cameraPortPrint("\x81\x01\x04");
		cameraPortWrite(zero_byte);
		cameraPortPrint	("\x02\xFF");
	}
	else{
		cameraPortPrint("\x81\x01\x04");
		cameraPortWrite(zero_byte);
		cameraPortPrint	("\x03\xFF");
	}
		//cameraPortPrintF("\x81\x01\x04%x\x03\xFF",0x00);
}
void cameraEnableOSD(uint8_t state){
	if(state == true){
		cameraPortPrint("\x81\x01\x04\x74\x2F\xFF");
		cameraPortPrint("\x81\x01\x04\x73\x10");
		cameraPortWrite(zero_byte);
		cameraPortPrint("\x01\x03\x01");
		cameraPortWrite(zero_byte);
		cameraPortWrite(zero_byte);
		cameraPortWrite(zero_byte);
		cameraPortWrite(zero_byte);
		cameraPortWrite(zero_byte);
		cameraPortWrite(zero_byte);
		cameraPortPrint("\xFF");
		cameraPortPrint("\x81\x01\x04\x73\x21\x84\x72\x73\x83\x65\x84\x69\x83\x84\x33\xFF");
		cameraPortPrint("\x81\x01\x04\x73\x32\\x84\x72\x73\x83\x65\x84\x69\x83\x84\x33\xFF");

	}
	else
		cameraPortPrint("\x81\x01\x04\x74\x3F\xFF");
}
void updateOSD(){
	cameraPortPrint("\x81\x01\x04\x73\x10");
	cameraPortWrite(zero_byte);
	cameraPortPrint("\x01\x03\x01");
	cameraPortWrite(zero_byte);
	cameraPortWrite(zero_byte);
	cameraPortWrite(zero_byte);
	cameraPortWrite(zero_byte);
	cameraPortWrite(zero_byte);
	cameraPortWrite(zero_byte);
	cameraPortPrint("\xFF");
	cameraPortPrint("\x81\x01\x04\x73\x21\x84\x72\x73\x83\x65\x84\x69\x83\x84\x33\xFF");
	cameraPortPrint("\x81\x01\x04\x73\x32\x84\x72\x73\x83\x65\x84\x69\x83\x84\x33\xFF");
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
void cameraZoomIn(){
	cameraPortPrintF("\x81\x01\x04\x07\x20\xFF");
}
void cameraZoomOut(){
	cameraPortPrintF("\x81\x01\x04\x07\x30\xFF");


}
void cameraStopZoom(){
	//8x 01 04 07 00 FF
	cameraPortPrint("\x81\x01\x04\x07");
	cameraPortWrite(zero_byte);
	cameraPortPrint("\xFF");
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
//void cameraSetText(){

//}

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
