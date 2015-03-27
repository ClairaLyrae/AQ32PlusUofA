
#pragma once

///////////////////////////////////////////////////////////////////////////////

extern uint32_t (*cliPortAvailable)(void);

extern void     (*cliPortClearBuffer)(void);

extern uint8_t  (*cliPortRead)(void);

extern void     (*cliPortPrint)(char *str);

extern void     (*cliPortPrintF)(const char * fmt, ...);

///////////////////////////////////////

extern uint8_t cliBusy;
//////////////////////////////////////////

static void communication_receive(void);
