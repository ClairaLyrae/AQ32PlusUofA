///////////////////////////////////////////////////////////////////////////////

#include "board.h"

///////////////////////////////////////////////////////////////////////////////
// Camera CLI
///////////////////////////////////////////////////////////////////////////////

void cameraCLI()
{
    uint8_t cameraQuery = 'x';
    uint8_t validQuery = false;
    cliBusy = true;
    cliPortPrint("\nEntering Camera CLI....\n\n");

    while(true)
    {
        cliPortPrint("Camera CLI -> ");

        while ((cliPortAvailable() == false) && (validQuery == false));

	    if (validQuery == false)
	    	cameraQuery = cliPortRead();

	    cliPortPrint("\n");

	    switch(cameraQuery)
	    {
			///////////////////////////////

	    	// Camera Configuration
            case 'a':
                cliPortPrint("\nCamera Configuration:\n");
                validQuery = false;
                break;

        	///////////////////////////////

    	    // Camera Configuration
            case 'A':
                cliPortPrint("\nCamera Configuration:\n");
                validQuery = false;
                break;

        	///////////////////////////////

            // Exit
            case 'x':
			    cliPortPrint("\nExiting Camera CLI....\n\n");
			    cliBusy = false;
			    return;
			    break;

	    	///////////////////////////////

			// Write EEPROM Parameters
            case 'W':
                cliPortPrint("\nWriting EEPROM Parameters....\n\n");
                writeEEPROM();
                validQuery = false;
                break;

        	///////////////////////////////

            // Help menu
			case '?':
			   	cliPortPrint("\n");
			   	cliPortPrint("'a' Camera Configuration Data              'A' Send Hex Command\n\n");
			   	cliPortPrint("                                           'W' Write EEPROM Parameters\n");
   		        cliPortPrint("'x' Exit Camera CLI                     	 '?' Command Summary\n\n");
   		        break;

   		    ///////////////////////////////
	    }
	}
}

///////////////////////////////////////////////////////////////////////////////
