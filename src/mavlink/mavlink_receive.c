/*
 * Modified by: Fabian De La Pena Montero
 * Original source code obtained from
 * http://qgroundcontrol.org/dev/mavlink_onboard_integration_tutorial
 */

#include <mavlink/common/mavlink.h>
#include <board.h>

// Example variable, by declaring them static they're persistent
// and will thus track the system state
static int packet_drops = 0;

uint32_t (*cliPortAvailable)(void);

void     (*cliPortClearBuffer)(void);

uint8_t  (*cliPortRead)(void);

void     (*cliPortPrint)(char *str);

void     (*cliPortPrintF)(const char * fmt, ...);

///////////////////////////////////////

uint8_t cliBusy = false;

/**
* @brief Receive communication packets and handle them
*
* This function decodes packets on the protocol level and also handles
* their value by calling the appropriate functions.
*/

static void communication_receive(void)
{
	mavlink_message_t msg;
	mavlink_status_t status;

	// COMMUNICATION THROUGH EXTERNAL UART PORT (XBee serial)

	while(cliPortAvailable())
	{
		uint8_t c = cliPortRead();
		// Try to get a new message
		if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
			// Handle message

			switch(msg.msgid)
			{
			//CAMERA CONTROL MESSAGE CASE
			case MAVLINK_MSG_ID_CAMERA_CONTROL:
				// EXECUTE ACTION RELATED TO CAMERA CONTROL
				break;
			//OSD CONTROL MESSAGE CASE
			/*case MAVLINK_MSG_ID_OSD_CONTROL:
				//EXECUTE ACTION RELATED TO OSD CONTROL
				break;*/
			default:
				//Do nothing
				break;
			}
		}

		// And get the next one
	}

	// Update global packet drops counter
	packet_drops += status.packet_rx_drop_count;
}
