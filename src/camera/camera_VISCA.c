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
uint8_t temperature = true;
uint8_t battery = true;
uint8_t altitude = true;
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
		//Command below enables OSD
		cameraPortPrint("\x81\x01\x04\x74\x2F\xFF");
		updateOSD();
		//cameraPortPrint("\x81\x01\x04\x73\x21\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\xFF");
		//cameraPortPrint("\x81\x01\x04\x73\x31\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\xFF");

	}
	else
		cameraPortPrint("\x81\x01\x04\x74\x3F\xFF");
}
void updateOSD(){
	cameraPortPrint("\x81\x01\x04\x74\x10\xFF");
	cameraPortPrint("\x81\x01\x04\x73\x10");
	cameraPortWrite(zero_byte);
	cameraPortWrite(zero_byte);
	cameraPortWrite(zero_byte);
	cameraPortWrite(zero_byte);
	cameraPortWrite(zero_byte);
	cameraPortWrite(zero_byte);
	cameraPortWrite(zero_byte);
	cameraPortWrite(zero_byte);
	cameraPortWrite(zero_byte);
	cameraPortWrite(zero_byte);
	cameraPortPrint("\xFF");
	cameraPortPrint("\x81\x01\x04\x73\x20\x01");
	cameraPortWrite(zero_byte);
	cameraPortPrint("\x13\x13\x4A\x1B");
	valueToVisca(batteryVoltage,4);
	//cameraPortPrint("\x22\x22\x22\x22");
	cameraPortPrint("\xFF");
	cameraPortPrint("\x81\x01\x04\x73\x30\x1B");
	cameraPortWrite(zero_byte);
	cameraPortPrint("\x0B\x13\x4A\x1B");
	valueToVisca(hEstimate,4);
	cameraPortPrint("\xFF");
	//cameraPortPrint("\x81\x01\x04\x73\x30\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\xFF");
	}
/*
	if(temperature == true){
		cameraPortPrint("\x81\x01\x04\x73\x21\x13\x04\x0C\x0F\x4A\x1B");
		//valueToVisca(mlxAmbTempC,4);
		cameraPortPrint("\x22\x22\x22\x22");
		cameraPortPrint("\xFF");
		cameraPortPrint("\x81\x01\x04\x73\x31\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\xFF");
	}

	if (altitude == true){
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
		cameraPortPrint("\x81\x01\x04\x73\x23\x13\x04\x0C\x0F\x4A\x1B");
		//valueToVisca(hEstimate,4);
		cameraPortPrint("\x22\x22\x22\x22");
		cameraPortPrint("\xFF");
		cameraPortPrint("\x81\x01\x04\x73\x33\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\x1B\xFF");
	}*/

void valueToVisca(float value, int charAvailable){
	char valueBuffer[charAvailable];
	sprintf(valueBuffer,"%4.1f",value);
	int i = 0;
	for (i = 0; i < charAvailable; i++) {
		switch ( valueBuffer[i] ) {
		case '1':
			cameraPortPrint("\x1E");
			break;
		case '2':
			cameraPortPrint("\x1F");
		  break;
		case '3':
			cameraPortPrint("\x20");
			break;
		case '4':
			cameraPortPrint("\x21");
			break;
		case '5':
			cameraPortPrint("\x22");
			break;
		case '6':
			cameraPortPrint("\x23");
			break;
		case '7':
			cameraPortPrint("\x24");
			break;
		case '8':
			cameraPortPrint("\x25");
			break;
		case '9':
			cameraPortPrint("\x26");
			break;
		case '0':
			cameraPortPrint("\x27");
			break;
		case '.':
			cameraPortPrint("\x4C");
			break;
		default:
			cameraPortPrint("\x1B");
		  break;
		}
	}

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
