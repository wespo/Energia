/** \mainpage MatlabComm.ino: demonstrates usage of the DSP Shield Serial Communications API

  The serial communication API extends the Energia Serial API to send/receive arrays
  of arbitrary size (limited to the available memory).  In addition, a command/data
  communication protocol facilitates sending commands and data via the serial interface.
  
  The companion Matlab code implements the equivalent functions.
  
  Data is exchanged in the following sequence:
  - Receives first command to set gain (cmd = 0) or offset (cmd = 1)
  - Receives second command to set gain (cmd = 0) or offset (cmd = 1)
  - Receives input array of integers
  - Calculates the output = gain * input + offset
  - Sends output vector
*/

#include "matlabSerial.h"
#include <OLED.h>

// Baud rate
const long baudRate = 115200;

// Create instance of the serial command class
SerialCmd cmd;

// Global variables
const int maxArrayLength = 1024;
int   gain   = 1;
long  offset = 0;
int   *input;
long  *output;


/** \brief Parse serial command
  
  Commands:
  - cmd = 0 : sets gain to (int)data[0]
  - cmd = 1 : sets offset to (long)data[0]

  \param c is and integer specifying a command number
*/
void parse(int c);

/** \brief Setup function

  Allocate memory for input/output arrays.
  
  Initialize OLED display and serial communication module.

*/
void setup()
{  
    // Create input and output arrays
    input  = new int[maxArrayLength];
    output = new long[maxArrayLength];
    
    // Initialize the display
    disp.oledInit();
    disp.clear();
    disp.setline(0);
    disp.setOrientation(1);
    disp.print("Ready");
    
    // Connect to Matlab
    serial_connect(baudRate);
}

/** \brief Main application loop

  - Receives two serial commands from Matlab
    - Commands can set the gain or offset
    - 0: sets the gain
    - 1: sets the offset
  - Receives input vector from Matlab
  - Sends output vector to Matlab.
    - Output = gain * input + offset
*/
void loop()
{
    int command;
    int recvLength = 0;
    int sendLength = 0;
    
    cmd.recv();        
    command = cmd.getCmd();
    parse(command);
    
    cmd.recv();        
    command = cmd.getCmd();
    parse(command);

    // Receive input array from Matlab
    recvLength = serial_recv_array(input, maxArrayLength);
    
    // If received vector length is less than the max allocted,
    // calculates Y = a * X + B
    if (recvLength < maxArrayLength)
    {
        for(int n = 0; n < recvLength; n++)
        {
          output[n] = gain * input[n] + offset;
        }
        
        // Send output array to Matlab
        sendLength = serial_send_array(output, recvLength);
    } // Else, do nothing
}

void parse(int c)
{
    switch(c)
    {
        case 0: // Set gain parameter
            const int *data_int = cmd.getDataIntPointer();
            gain = data_int[0];
            int num_int = cmd.getDataIntLength();      
            disp.clear();
            disp.setline(0);
            disp.print("Cmd 0: gain = ");
            disp.setline(1);
            disp.print((long)data_int[0]);
            break;
        case 1: // set offset parameter
            const long *data_long = cmd.getDataLongPointer();
            offset = data_long[0];
            int num_long = cmd.getDataLongLength();
            disp.clear();
            disp.setline(0);
            disp.print("Cmd 1: offset = ");
            disp.setline(1);
            disp.print((long)data_long[0]);
            break;
        default:
            disp.clear();
            disp.setline(0);
            disp.print("Unknown Command");
            break;
    }
}
