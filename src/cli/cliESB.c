///////////////////////////////////////////////////////////////////////////////

#include "board.h"

///////////////////////////////////////////////////////////////////////////////
// Environmental Sensor Bus CLI
///////////////////////////////////////////////////////////////////////////////

void esbCLI()
{
	// For I2C R/W test commands
	uint8_t i2cTestAddress;
	uint8_t i2cTestRegister;
	uint8_t i2cTestDataLength;
	uint8_t* i2cTestData;
	uint8_t i2cIndexer;

    uint8_t esbQuery = 'x';
    uint8_t validQuery = false;
    cliBusy = true;
    cliPortPrint("\nEntering ESB CLI....\n\n");

    while(true)
    {
        cliPortPrint("Environmental Sensor Bus CLI -> ");

        while ((cliPortAvailable() == false) && (validQuery == false));

	    if (validQuery == false)
	    	esbQuery = cliPortRead();

	    cliPortPrint("\n");

	    switch(esbQuery)
	    {
			///////////////////////////////

	    	// Show ESB data
            case 'a':// Print all ESB data
				//updateMLX90614();
				cliPortPrintF("MLX90614[AMB:%d,OBJ:%d]\n", mlxRawAmbTemp, mlxRawObjTemp);
				cliPortPrintF("MLX90614[AMB:%f*C,OBJ:%f*C]\n", mlxAmbTempC, mlxObjTempC);
				esbQuery = 'x';
                validQuery = false;
                break;

			///////////////////////////////

			case '<': // I2C read command
				cliBusy = true;

				i2cTestAddress = readFloatCLI();
				i2cTestRegister = readFloatCLI();
				i2cTestDataLength = readFloatCLI();


				cliPortPrintF("Reading I2C (add:%d, reg:%d, data:%d)\n", i2cTestAddress, i2cTestRegister, i2cTestDataLength);

				if(i2cRead(I2C2, i2cTestAddress, i2cTestRegister, i2cTestDataLength, i2cTestData))
					cliPortPrint("ACK!");
				else
					cliPortPrint("NACK!");
				for(i2cIndexer = 0; i2cIndexer < i2cTestDataLength; i2cIndexer++)
					cliPortPrintF("Received: %d\n", i2cTestData[i2cIndexer]);
				cliBusy = false;
				esbQuery = 'x';
                validQuery = false;
				break;

			///////////////////////////////

			case '>': // I2C write command
				cliBusy = true;

				i2cTestAddress = readFloatCLI();
				i2cTestRegister = readFloatCLI();
				i2cTestDataLength = readFloatCLI();

				cliPortPrintF("Writing I2C (add:%d, reg:%d, data:%d)\n", i2cTestAddress, i2cTestRegister, i2cTestDataLength);

				if(i2cWriteBuffer(I2C2, i2cTestAddress, i2cTestRegister, i2cTestDataLength, i2cTestData))
					cliPortPrint("ACK!");
				else
					cliPortPrint("NACK!");
				cliBusy = false;
				esbQuery = 'x';
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
			   	cliPortPrint("'a' Show ESB data\n");
			   	cliPortPrint("'<' Read I2C data			ADD;REG;DAT	     '>' Write I2C data	       ADD;REG;DAT\n");
			   	cliPortPrint("											 'W' Write EEPROM Parameters\n");
   		        cliPortPrint("'x' Exit ESB CLI                     		 '?' Command Summary\n\n");
   		        break;

   			///////////////////////////////
	    }
	}
}

///////////////////////////////////////////////////////////////////////////////
