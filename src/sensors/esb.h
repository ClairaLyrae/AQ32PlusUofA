#pragma once
//
//typedef union {
//	char* name;
//	uint8_t status;
//	uint8_t (*updateFunction)(void);
//	uint8_t (*initFunction)(void);
//} esbDevice_t;


enum {
	ESB_MLX90614 = 0,
	ESB_GSB = 1,
	ESB_PDB = 2
};

extern uint8_t esbDeviceIndex;
#define ESB_MAX_DEVICES 16
#define ESB_NUM_DEVICES 3

void initESB(void);
void updateESB(void);
