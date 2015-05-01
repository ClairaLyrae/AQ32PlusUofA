///////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////

extern uint32_t (*cameraPortAvailable)(void);
extern uint16_t (*cameraPortNumCharsAvailable)(void);
extern void     (*cameraPortClearBuffer)(void);
extern uint8_t  (*cameraPortRead)(void);
extern void     (*cameraPortPrint)(char *str);
extern void     (*cameraPortPrintF)(const char * fmt, ...);
extern void     (*cameraPortWrite)(uint8_t data);
extern int cameraZoom;


///////////////////////////////////////////////////////////////////////////////
// Camera Defines
///////////////////////////////////////////////////////////////////////////////

#define VISCA_BEGIN 0x81
#define VISCA_QQ_COMMAND 0x01
#define VISCA_QQ_INQUIRY 0x09
#define VISCA_RR_INTERFACE 0x00
#define VISCA_RR_CAM1 0x04
#define VISCA_RR_CAM2 0x07
#define VISCA_RR_PAN_TILT 0x06
#define VISCA_END 0xFF

// VISCA Command (81 QQ RR <Command> FF)
char* VISCA_CMD_HEADER = "\x81\x01\x04";
char* VISCA_INQ_HEADER = "\x81\x09\x04";

char* VISCA_ACK_HEADER = "\x90\x40";
char* VISCA_DONE_HEADER = "\x90\x50";
char* VISCA_ERROR_HEADER = "\x90\x60";

enum {
	CAMERA_EFFECT_OFF = 0,
	CAMERA_EFFECT_BW = 2,
	CAMERA_EFFECT_NEG = 4
};

enum {
	CAMERA_ORIENT_UP = 0,
	CAMERA_ORIENT_DOWN = 1
};

enum {
	CAMERA_FOCUS_AUTO = 2,
	CAMERA_FOCUS_MANUAL = 3,
	CAMERA_FOCUS_INFINITY = 2
};

enum {
	CAMERA_WB_NORMAL = 0,
	CAMERA_WB_OUTDOOR = 1,
	CAMERA_WB_INDOOR = 2,
	CAMERA_WB_TRACING = 4,
	CAMERA_WB_MANUAL = 5,
};

enum {
	CAMERA_AF_NORMAL = 0,
	CAMERA_AF_INTERVAL = 1,
	CAMERA_AF_ZOOM = 2
};

enum {
	CAMERA_AE_AUTO = 0,
	CAMERA_AE_MANUAL = 3,
	CAMERA_AE_SHUTTER = 10,
	CAMERA_AE_IRIS = 11,
	CAMERA_AE_BRIGHT = 13,
};

///////////////////////////////////////////////////////////////////////////////

void cameraEnable(uint8_t state);
void cameraEnablesOSD(uint8_t state);
void updateOSD();
void valueToVisca(float value);
void cameraSetOrientation(uint8_t mode);

void cameraSetZoom(uint8_t optical, uint8_t digital);
void cameraZoomIn();
void cameraZoomOut();
void cameraStopZoom();
void cameraSetFocusMode(uint8_t mode);

void cameraSetAF(uint8_t sensitivity, uint8_t mode);

void cameraSetWhiteBalance(uint8_t mode);

void cameraFreezeImage(uint8_t state);

void cameraSetEffect(uint8_t mode);

void initCamera(void);

void cameraTestEcho(void);

///////////////////////////////////////////////////////////////////////////////
