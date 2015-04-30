/*
 * Modified by: Fabian De La Pena Montero
 *
 * Original source code obtained from
 * http://qgroundcontrol.org/dev/mavlink_onboard_integration_tutorial
 *
 * Filename: mavlink_receive.c
 *
 * Description: This file contains a function that receives and handles MAVLink
 * messages sent to the UAV by the ground station.
 */

#include <mavlink/common/mavlink.h>
#include <mavlink/firebird/firebird.h>
#include <board.h>

// Example variable, by declaring them static they're persistent
// and will thus track the system state
static int packet_drops = 0;


///////////////////////////////////////

uint8_t cliBusy = false;

/**
* @brief Receive communication packets and handle them
*
* This function decodes packets on the protocol level and also handles
* their value by calling the appropriate functions.
*/

void communication_receive(void)
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
				//COMMANDS
				case MAVLINK_MSG_ID_COMMAND_LONG:
				{
				mavlink_command_long_t cmd;
				mavlink_msg_command_long_decode(&msg, &cmd);
				switch (cmd.command) {
					/*CAMERA CONTROL COMMAND*/
					case MAV_CMD_VIDEO_START_CAPTURE:
						/*Enable camera*/
							cameraEnable(true);
					break;
					case MAV_CMD_VIDEO_STOP_CAPTURE:
							cameraEnable(false);
						break;
					case MAV_CMD_DO_DIGICAM_CONTROL:
						if (((int)(cmd.param1)) == 1){
							cameraFlip(true);
						}
						else if (((int)(cmd.param1)) == 2){
							cameraFlip(false);

						}
						else if (((int)(cmd.param3)) == 1){
							cameraZoomIn();
						}
						else if (((int)(cmd.param3)) == -1){
							cameraZoomOut();
						}
						else if (((int)(cmd.param3)) == 2){
							cameraStopZoom();
						}
						else if (((int)(cmd.param7)) == 1){
							cameraEnableOSD(true);
						}
						else if (((int)(cmd.param7)) == -1){
							cameraEnableOSD(false);
						}
						break;
				/*OSD CONTROL COMMAND
				 ***UNCOMMENT WHEN THIS FUNCTIONALITY IS READY****
				 * 	case MAV_CMD_OSD_CONTROL:
				 * 	if ((int)(cmd.param1)) == 0){
				 * 		//Disable component 1
				 * 	}
				 * 	else if((int)(cmd.param1)) == 1){
				 * 		//Enable component 1
				 * 	}
				 * 	if((int)(cmd.param2)) == 0){
				 * 		//Disable component 2
				 * 	}
				 * 	else if((int)(cmd.param2)) == 0{
				 * 		//Enable component 2
				 * 	}
				 * 	//Continue statements for each OSD component we wish to enable or disable
				 * 	break;
				 */
					default:
					//mavlink_msg_command_ack_send(MAVLINK_COMM_0, cmd.command, MAV_RESULT_UNSUPPORTED);
					break;
				}
			}
					break;
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
