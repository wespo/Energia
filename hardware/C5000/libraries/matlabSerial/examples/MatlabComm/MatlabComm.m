% Demonstrates usage of the DSP Shield Serial Communications API
% 
% The serial communication API extends the Energia Serial API to
% send/receive arrays of arbitrary size (limited to the available memory).
% In addition, a command/data communication protocol facilitates sending
% commands and data via the serial interface.

% Define constants for the connection
baud = 115200;
% comport = 'COM6';
comport = '/dev/tty.usbserial-SDWT1KDZB';

% Connect to the DSP Shield using a handshake
[s, connected] = serial_connect(comport, baud);

% Input data
dataLength = 64;
input = (1:dataLength)';

% Parameters
gain = 7;
offset = 100;

% Only communicate if we successfully connect
if connected
    % Send a command to the DSP Shield
    % Format is serial_cmd(s, cmd, data, type)
    serial_cmd(s, 0, [int16(gain)],   'int16');
    serial_cmd(s, 1, [int32(offset)], 'int32');

    % Send data to DSP Shield
    serial_send_array(s, input, 'int16');

    % Receive data from DSP Shield
    output = serial_recv_array(s, 'int32');
end

% Compare results
output_ref = gain * input + offset;
error = sum(abs(output - output_ref))

% Close the stream
fclose(s);